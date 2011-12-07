/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 *
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 * =======================================================================
 *
 */

/* The security operations other than encryption... */
#define SECURITY_OPER_IDENT_FORM     0x80
#define SECURITY_OPER_DEVICE_ID      0x81
#define SECURITY_OPER_URL            0x86

/* Identity verification form codes... */
#define SECURITY_IDENT_FORM_SIMPLE   0x00 /* simple verification */
#define SECURITY_IDENT_FORM_PASSWORD 0x02 /* simple+passwd */

#define MANDATORY_FACILITY          (idigi_config_request_t)-1

#define SET_FACILITY_SUPPORT(i) (0x01 << (i))
#define IS_FACILITY_SUPPORTED(idigi_ptr, table_index)    (idigi_ptr->facilities & SET_FACILITY_SUPPORT(table_index))

typedef idigi_callback_status_t (* idigi_facility_init_cb_t)(struct idigi_data * const idigi_ptr, unsigned int const facility_index);
typedef idigi_callback_status_t (* idigi_facility_delete_cb_t)(struct idigi_data * const idigi_ptr);
typedef idigi_callback_status_t (* idigi_facility_process_cb_t )(struct idigi_data * const idigi_ptr,
                                                                 void * const facility_data,
                                                                 uint8_t * const packet,
                                                                 unsigned int * const receive_timeout);

typedef struct {
    idigi_request_t request_id;
    idigi_facility_init_cb_t init_cb;
    idigi_facility_delete_cb_t delete_cb;
    idigi_facility_delete_cb_t cleanup_cb;
    idigi_facility_process_cb_t discovery_cb;
    idigi_facility_process_cb_t process_cb;
} idigi_facility_init_t;

/* Table of all facilites that iDigi supports.
 *
 * iDigi will call the callback to see whether it supports each optional facility.
 * iDigi will call init_cb to initialize the facility and delete_cb to remove the facility.
 * The init_cb must call add_facility_data() to add the facility into iDigi facility list.
 * The delete_cb is called to delete the facility from iDigi facility list when user terminates iDigi.
 */
static idigi_facility_init_t const idigi_supported_facility_table[] = {
        /* mandatory facilities */
        {{MANDATORY_FACILITY}, idigi_facility_cc_init, idigi_facility_cc_delete, idigi_facility_cc_cleanup, cc_discovery, cc_process},

        /* list of optional facilities */
#if defined(IDIGI_FIRMWARE_SERVICE)
    #if defined(IDIGI_FIRMWARE_SUPPORT)
        {{MANDATORY_FACILITY}, idigi_facility_firmware_init, idigi_facility_firmware_delete, NULL, fw_discovery, fw_process},
    #else
        {{idigi_config_firmware_facility}, idigi_facility_firmware_init, idigi_facility_firmware_delete, NULL, fw_discovery, fw_process},
    #endif
#endif
#if defined(IDIGI_DATA_SERVICE)
    #if defined(IDIGI_DATA_SERVICE_SUPPORT)
        {{MANDATORY_FACILITY}, idigi_facility_data_service_init, idigi_facility_data_service_delete, idigi_facility_data_service_cleanup, msg_discovery, msg_process}
    #else
        {{idigi_config_data_service}, idigi_facility_data_service_init, idigi_facility_data_service_delete, idigi_facility_data_service_cleanup, msg_discovery, msg_process}
    #endif
#endif
};

static size_t const idigi_facility_count = asizeof(idigi_supported_facility_table);

static idigi_callback_status_t layer_remove_facilities(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t result = idigi_callback_continue;
    idigi_facility_t * fac_ptr;

    /* remove all facilities. delete_cb should not return busy. */
    for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL && result == idigi_callback_continue; fac_ptr = fac_ptr->next)
    {
        unsigned int const i = fac_ptr->facility_index;

        if (idigi_supported_facility_table[i].delete_cb != NULL)
        {
            result = idigi_supported_facility_table[i].delete_cb(idigi_ptr);
        }
        ASSERT(result != idigi_callback_busy);
    }

    return result;
}

static idigi_callback_status_t layer_cleanup_facilities(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t result = idigi_callback_continue;
    idigi_facility_t * fac_ptr;

    /* cleanup all facilities. cleanup_cb should not return busy. */
    for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL && result == idigi_callback_continue; fac_ptr = fac_ptr->next)
    {
        unsigned int const i = fac_ptr->facility_index;

        if (idigi_supported_facility_table[i].cleanup_cb != NULL)
        {
            result = idigi_supported_facility_table[i].cleanup_cb(idigi_ptr);
        }
        ASSERT(result != idigi_callback_busy);
    }

    return result;
}

static size_t msg_add_keepalive_param(uint8_t * const edp_header, uint16_t const type, uint16_t const value)
{
enum edp_keepalive{
    field_define(edp_keepalive, interval, uint16_t),
    record_end(edp_keepalive)
};

    size_t const keepalive_size = record_bytes(edp_keepalive);
    uint8_t * edp_keepalive;

    edp_keepalive = GET_PACKET_DATA_POINTER(edp_header, PACKET_EDP_HEADER_SIZE);
    message_store_be16(edp_header, type, type);
    message_store_be16(edp_header, length, keepalive_size);
    message_store_be16(edp_keepalive, interval, value);


    return (PACKET_EDP_HEADER_SIZE + keepalive_size); /* return count of bytes added to buffer */
}

static idigi_callback_status_t discovery_facility_layer(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_facility_t * fac_ptr;

    /* invoke any facility that needs to send any message to server
     * during initialization phase at discovery layer.
     */
    fac_ptr = (idigi_ptr->active_facility == NULL)? idigi_ptr->facility_list: idigi_ptr->active_facility;

    for (;fac_ptr != NULL && status == idigi_callback_continue; fac_ptr = fac_ptr->next)
    {
        unsigned int const i = fac_ptr->facility_index;

        if (idigi_supported_facility_table[i].discovery_cb != NULL)
        {   /* function to send facility discovery */
            status = idigi_supported_facility_table[i].discovery_cb(idigi_ptr, fac_ptr->facility_data,
                                                                    NULL, &idigi_ptr->receive_packet.timeout);
            if (status != idigi_callback_continue)
            {
                idigi_ptr->active_facility = fac_ptr;
            }
        }
    }

    return status;
}

static idigi_callback_status_t get_configurations(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;

    static idigi_config_request_t const idigi_edp_init_config_ids[] = {
            idigi_config_server_url,
#if !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
            idigi_config_rx_keepalive,
#endif
#if !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS)
            idigi_config_tx_keepalive,
#endif
#if !defined(IDIGI_WAIT_COUNT)
            idigi_config_wait_count
#endif
    };
    unsigned i;

   /* Call callback to get server url, wait count, tx keepalive, rx keepalive, & password.
    * Call error status callback if error is encountered (NULL data, invalid range, invalid size).
    */
    for (i=0; i < asizeof(idigi_edp_init_config_ids); i++)
    {
        void * data = NULL;
        size_t length;
        idigi_request_t request_id;

        request_id.config_request = idigi_edp_init_config_ids[i];

#if !defined(IDIGI_CLOUD_URL) || !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS) || !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS) || !defined(IDIGI_WAIT_COUNT)
        status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &data, &length);
        if (status != idigi_callback_continue)
        {
            /* set error code if it has not been set */
            if (idigi_ptr->error_code == idigi_success)
            {
                idigi_ptr->error_code = idigi_configuration_error;
            }
            if (status == idigi_callback_unrecognized)
            {
                status = idigi_callback_abort;
            }
            goto done;
        }
        if (data == NULL)
        {
            /* callback cannot return */
            idigi_ptr->error_code = idigi_invalid_data;
            goto error;
        }
#endif /* !defined(IDIGI_CLOUD_URL) || !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS) ||
          !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS) || !defined(IDIGI_WAIT_COUNT) */


        switch(request_id.config_request)
        {
        case idigi_config_server_url:
#if defined(IDIGI_CLOUD_URL)
        {
            static char const idigi_cloud_url[]= IDIGI_CLOUD_URL;
            length = sizeof idigi_cloud_url -1;
            data = (void *)idigi_cloud_url;
        }
#endif
            if ((length == 0) || (length > SERVER_URL_LENGTH-1))
            {
                idigi_ptr->error_code = idigi_invalid_data_range;
                goto error;
            }
            memcpy(idigi_ptr->server_url, data, length);
            idigi_ptr->server_url[length] = 0x0;
            idigi_ptr->server_url_length = length;
            break;

#if !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS) || !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
        case idigi_config_tx_keepalive:
        case idigi_config_rx_keepalive:
        {
            uint16_t const * const value = data;
#if !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS) && !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
            idigi_bool_t const is_tx = (request_id.config_request == idigi_config_tx_keepalive);
            uint16_t const min_interval = is_tx ? MIN_TX_KEEPALIVE_INTERVAL_IN_SECONDS : MIN_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
            uint16_t const max_interval = is_tx ? MAX_TX_KEEPALIVE_INTERVAL_IN_SECONDS : MAX_RX_KEEPALIVE_INTERVAL_IN_SECONDS;

#elif !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS)
            uint16_t const min_interval = MIN_TX_KEEPALIVE_INTERVAL_IN_SECONDS;
            uint16_t const max_interval = MAX_TX_KEEPALIVE_INTERVAL_IN_SECONDS;

#elif !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
            uint16_t const min_interval = MIN_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
            uint16_t const max_interval = MAX_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
#endif
            if ((length != sizeof *value) || (*value < min_interval) || (*value > max_interval))
            {
                idigi_ptr->error_code = idigi_invalid_data_range;
                goto error;
            }
            {
#if !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS) && !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
                uint16_t * const store_at = is_tx ? &idigi_ptr->tx_keepalive_interval : &idigi_ptr->rx_keepalive_interval;
#elif !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS)
                uint16_t * const store_at = &idigi_ptr->tx_keepalive_interval;
#elif !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
                uint16_t * const store_at = &idigi_ptr->rx_keepalive_interval;
#endif
                *store_at = *value;
            }
            break;
        }
#endif

#if !defined(IDIGI_WAIT_COUNT)
        case idigi_config_wait_count:
        {
            uint16_t const * const value = data;

            if ((*value < WAIT_COUNT_MIN) ||
                (*value > WAIT_COUNT_MAX)||
                (length != sizeof *value))
            {
                idigi_ptr->error_code = idigi_invalid_data_range;
                goto error;
            }
            idigi_ptr->wait_count = *value;
            break;
        }
#endif
        default:
            /* get these configurations from different modules */
            ASSERT(idigi_false);
            break;
        }
    }

error:
    if (idigi_ptr->error_code != idigi_success)
    {
        idigi_request_t request_id;
        ASSERT(i < asizeof(idigi_edp_init_config_ids));
        request_id.config_request = idigi_edp_init_config_ids[i];
        notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
        status = idigi_callback_abort;
    }

#if !defined(IDIGI_CLOUD_URL) || !defined(IDIGI_RX_KEEPALIVE_IN_SECONDS) || !defined(IDIGI_TX_KEEPALIVE_IN_SECONDS) || !defined(IDIGI_WAIT_COUNT)
done:
#endif
    return status;

}

static idigi_callback_status_t layer_get_supported_facilities(idigi_data_t * const idigi_ptr)
{
#define NUMBER_FACILITY_PER_BYTE CHAR_BIT
    idigi_callback_status_t status = idigi_callback_continue;
    unsigned i;

    idigi_ptr->facilities = 0;

    ASSERT(CHAR_BIT == 8);
    ASSERT(idigi_facility_count <= (sizeof idigi_ptr->facilities * NUMBER_FACILITY_PER_BYTE));

    /* idigi_supported_facility_table[] table includes a list of facilities that iDigi supports.
     * Call callback to see which facility is supported.
     */

    for (i=0; i < idigi_facility_count; i++)
    {
        idigi_request_t const request_id = idigi_supported_facility_table[i].request_id;
        size_t length;
        idigi_service_supported_status_t facility_enable = (request_id.config_request == (idigi_config_request_t)MANDATORY_FACILITY) ? idigi_service_supported : idigi_service_unsupported;

        if (request_id.config_request != (idigi_config_request_t)MANDATORY_FACILITY)
        {   /* this is optional facility so ask application whether it supports this facility */
            status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &facility_enable, &length);
            if (status == idigi_callback_abort)
            {
                idigi_debug("initialize_facilities: callback returns %d on facility= %d\n", status, request_id.config_request);
                idigi_ptr->error_code = idigi_configuration_error;
                status = idigi_callback_abort;
                break;
            }
            else if (status == idigi_callback_busy)
            {
                break;
            }
            idigi_debug("initialize_facilities: callback %s  %d facility\n", facility_enable ? "supports" : "unsupports", request_id);
        }

        if (facility_enable == idigi_service_supported)
        {
            idigi_ptr->facilities |= SET_FACILITY_SUPPORT(i);
        }
    }
    return status;
}

static idigi_callback_status_t initialize_facilities(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    size_t  i;

    for (i=idigi_ptr->request_id; (i < idigi_facility_count) && (status == idigi_callback_continue); i++)
    {
        if (IS_FACILITY_SUPPORTED(idigi_ptr,i) &&
           idigi_supported_facility_table[i].init_cb != NULL)
        {
            status = idigi_supported_facility_table[i].init_cb(idigi_ptr, i);
        }
    }

    idigi_ptr->request_id = i;

    if (status == idigi_callback_continue)
    {
        idigi_facility_t * fac_ptr;
        /* initialize packet pointer for each facility */
        for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
        {
            fac_ptr->packet = NULL;
        }
    }

    return status;
}

static idigi_callback_status_t configuration_layer(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;

    /* This layer is called to get some of the EDP configuration and initialize each facility */
    status = get_configurations(idigi_ptr);
    if (status == idigi_callback_continue)
    {
        idigi_ptr->request_id = 0;
        set_idigi_state(idigi_ptr, edp_communication_layer);
    }

    return status;

}

static idigi_callback_status_t send_version(idigi_data_t * idigi_ptr, uint16_t const type, uint32_t const version)
{
enum edp_version {
    field_define(edp_version, version, uint32_t),
    record_end(edp_version)
};
    uint16_t const version_message_size = record_bytes(edp_version);

    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * edp_version;
    uint8_t * packet;

    /*
     *  version packet format:
     *  -------------------------
     * | 0 - 1 | 2 - 3 |  4 - 7  |
     *  -------------------------
     * |  EDP header   | version |
     *  -------------------------
    */

    packet = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &edp_version, NULL);
    if (packet != NULL)
    {
        message_store_be32(edp_version, version, version);

        status = initiate_send_packet(idigi_ptr, packet, version_message_size,
                                    type,
                                    release_packet_buffer,
                                    NULL);
    }

    return status;
}

static idigi_callback_status_t communication_layer(idigi_data_t * const idigi_ptr)
{
#define SERVER_OVERLOAD_RESPONSE    0x02

enum {
    communication_initialize_failities,
    communication_connect_server,
    communication_send_version,
    communication_receive_version_response,
    communication_send_keepalive
};

    idigi_callback_status_t status = idigi_callback_continue;

    /* communitcation layer:
     *  1. establishes connection.
     *  2. sends MT version
     *  3. receives and validates MT version response
     *  4. sends tx, rx, & waitcount parameters
     */
    switch (idigi_ptr->layer_state)
    {
    case communication_initialize_failities:
        status = initialize_facilities(idigi_ptr);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = communication_connect_server;
        }
        break;

    case communication_connect_server:
        if (!idigi_ptr->network_connected)
        {
            status = connect_server(idigi_ptr, idigi_ptr->server_url, idigi_ptr->server_url_length);
        }

        if (status == idigi_callback_continue)
        {
            idigi_ptr->request_id = 0;
            idigi_ptr->layer_state = communication_send_version;
        }
        break;

    case  communication_send_version:

        idigi_debug("communication layer: Send MT Version\n");
        status = send_version(idigi_ptr, E_MSG_MT2_TYPE_VERSION, EDP_MT_VERSION);

        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = communication_receive_version_response;
        }
        break;

    case communication_receive_version_response:
    {
        uint8_t * ptr;
        uint8_t * packet;

        status = receive_packet(idigi_ptr, &packet);

        if (status == idigi_callback_continue && packet != NULL)
        {
            uint16_t type;
            uint8_t  response_code;

            idigi_debug("communication layer: receive Mt version\n");
            /*
             * MT version response packet format:
             *  ---------------
             * |0 - 1 |  2 - 3 |
             *  ---------------
             * | Type | length |
             *  ---------------
             *
             */

            ptr = GET_PACKET_DATA_POINTER(packet, PACKET_EDP_HEADER_SIZE);
            {
                uint8_t * edp_header = packet;
                type = message_load_be16(edp_header, type);
            }
            response_code = *ptr;

            release_receive_packet(idigi_ptr, packet);
            if (type != E_MSG_MT2_TYPE_VERSION_OK)
            {
                idigi_request_t request_id;
                /*
                 * The received message is not acceptable. Return an error value
                 * appropriate to the message received.
                 */
                status = idigi_callback_abort;
                switch (type)
                {
                    /* Expected MTv2 message types... */
                    case E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP:
                        if (response_code == SERVER_OVERLOAD_RESPONSE) {
                            idigi_ptr->error_code = idigi_server_overload;
                        }
                        else {
                            idigi_ptr->error_code = idigi_bad_version;
                        }
                        break;
                    case E_MSG_MT2_TYPE_VERSION_BAD:
                        idigi_ptr->error_code = idigi_bad_version;
                        break;
                    case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
                        idigi_ptr->error_code = idigi_server_overload;
                        break;
                    /* Unexpected/unknown MTv2 message types... */
                    default:
                        idigi_ptr->error_code = idigi_invalid_packet;
                }
                /* mt version error. let's notify user.
                 *
                 * ignore error status callback return value since server
                 * will close the connection.
                 */
                request_id.network_request = idigi_network_receive;
                notify_error_status(idigi_ptr->callback, idigi_class_network, request_id, idigi_ptr->error_code);
            }
            else
            {   /* advance to send keepalive parameters */
                idigi_ptr->layer_state = communication_send_keepalive;
            }
        }
        break;
    }
    case communication_send_keepalive:
    {
        uint8_t * ptr;
        uint8_t * packet;
        uint8_t * start_ptr;
        int len;
        size_t i;

        struct {
            uint16_t type;
            uint16_t value;
        } keepalive_parameters[3];

#define init_param(i, t, v) keepalive_parameters[i].type = (t); keepalive_parameters[i].value = (v)
        init_param(0, E_MSG_MT2_TYPE_KA_RX_INTERVAL, GET_RX_KEEPALIVE_INTERVAL(idigi_ptr));
        init_param(1, E_MSG_MT2_TYPE_KA_TX_INTERVAL, GET_TX_KEEPALIVE_INTERVAL(idigi_ptr));
        init_param(2, E_MSG_MT2_TYPE_KA_WAIT,        GET_WAIT_COUNT(idigi_ptr));
#undef  init_param

        idigi_debug("communication layer: send keepalive params \n");
        idigi_debug("communication layer: Rx keepalive parameter = %d\n", keepalive_parameters[0].value);
        idigi_debug("communication layer: Tx keepalive parameter = %d\n", keepalive_parameters[1].value);
        idigi_debug("communication layer: Wait Count parameter = %d\n", keepalive_parameters[2].value);

        packet = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &ptr, NULL);
        ptr = (uint8_t *)packet;
        start_ptr = ptr;

        for (i=0; i < asizeof(keepalive_parameters); i++)
        {
            len = msg_add_keepalive_param(ptr, keepalive_parameters[i].type, keepalive_parameters[i].value);
            ptr += len;
        }
        /* Setting the total_length will enable send_packet_process.
         * Clear length to 0 for actual length that has been sent.
         */
        idigi_ptr->send_packet.total_length = ptr - start_ptr;
        idigi_ptr->send_packet.bytes_sent = 0;
        idigi_ptr->send_packet.ptr = packet;
        idigi_ptr->send_packet.complete_cb = release_packet_buffer;


        set_idigi_state(idigi_ptr, edp_initialization_layer);
        break;
    }
    } /* switch */

    return status;
}

static idigi_callback_status_t initialization_layer(idigi_data_t * const idigi_ptr)
{
#define EDP_PROTOCOL_VERSION    0x120

enum {
    initialization_send_protocol_version,
    initialization_receive_protocol_version
};
enum {
    initialization_version_response_acceptable,
    initialization_version_response_unacceptable,
    initialization_version_response_unavailable
};

    idigi_callback_status_t status = idigi_callback_continue;

    /* initialization layer:
     * 1. sends protocol version.
     * 2. receives and validates protocol version response
     */
    switch (idigi_ptr->layer_state)
    {
    case initialization_send_protocol_version:
        idigi_debug("initialization layer: send protocol version\n");
        status = send_version(idigi_ptr, E_MSG_MT2_TYPE_PAYLOAD, EDP_PROTOCOL_VERSION);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = initialization_receive_protocol_version;
        }
        break;
    case initialization_receive_protocol_version:
    {
        uint8_t * edp_header;

        status = receive_packet(idigi_ptr, &edp_header);

        if (status == idigi_callback_continue)
        {
            ASSERT(edp_header != NULL);
            idigi_debug("initialization layer: receive protocol version\n");
            /*
             *  version response packet format:
             *  ---------------------------------------
             * |    0 - 1     |  2 - 3 |     4         |
             *  ---------------------------------------
             * | Payload Type | length | response code |
             *  ----------------------------------------
            */
            /*
             * Empty data packet
             */
            if (message_load_be16(edp_header, length) > 0)
            {
                uint8_t * const response_code = GET_PACKET_DATA_POINTER(edp_header, PACKET_EDP_HEADER_SIZE);
                /* Parse the version response (0 = version response ok).
                 * If the protocol version number was not acceptable to the server,
                 * tell the application.
                 */
                if (*response_code != initialization_version_response_acceptable)
                {
                    idigi_request_t const request_id = {idigi_network_receive};
                    status = idigi_callback_abort;
                    idigi_ptr->error_code = idigi_bad_version;
                    notify_error_status(idigi_ptr->callback, idigi_class_network, request_id, idigi_ptr->error_code);
                    goto done;

                }
                set_idigi_state(idigi_ptr, edp_security_layer);
            }
            release_receive_packet(idigi_ptr, edp_header);
        }
        break;
    }
    }

done:

    return status;
}

static idigi_callback_status_t security_layer(idigi_data_t * const idigi_ptr)
{
enum {
        security_send_identity_verification,
        security_send_device_id,
        security_send_server_url
};

    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * edp_header;
    uint8_t * start_ptr;

    /* security layer:
     * 1. sends identity verification form
     * 2. sends device ID
     * 3. sends server URL
     * 4. sends password if identity verification form is PASSWORD identity
     */
    edp_header = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &start_ptr, NULL);
    if (edp_header == NULL)
    {
        goto done;
    }

    switch (idigi_ptr->layer_state)
    {
    case security_send_identity_verification:
    {
        enum edp_security {
            field_define(edp_security, opcode, uint8_t),
            field_define(edp_security, identity, uint8_t),
            record_end(edp_security)
        };
        uint16_t const edp_security_size = record_bytes(edp_security);
        uint8_t * edp_security = start_ptr;

        idigi_debug("Security layer: send security form\n");

        /*
         * packet format:
         *  -----------------------------------------------------
         * | 0 - 1 | 2 - 3 |     4       |     5                 |
         *  -----------------------------------------------------
         * |   EDP header  | Form opcode | identity verification |
         *  -----------------------------------------------------
        */
        message_store_u8(edp_security, opcode, SECURITY_OPER_IDENT_FORM);
        message_store_u8(edp_security, identity, SECURITY_IDENT_FORM_SIMPLE);

        status = initiate_send_packet(idigi_ptr, edp_header, edp_security_size,
                                    E_MSG_MT2_TYPE_PAYLOAD,
                                    release_packet_buffer,
                                    NULL);

        idigi_ptr->layer_state = security_send_device_id;
        break;
    }
    case security_send_device_id:
    {
        enum edp_device_id {
            field_define(edp_device_id, opcode, uint8_t),
            field_define_array(edp_device_id, id, DEVICE_ID_LENGTH),
            record_end(edp_device_id)
        };
        uint16_t const device_id_message_size = record_bytes(edp_device_id);
        uint8_t * edp_device_id = start_ptr;

        idigi_debug_hexvalue("security layer: send device ID", idigi_ptr->device_id, DEVICE_ID_LENGTH);
        /*
         * packet format:
         *  ----------------------------------------------
         * | 0 - 1 | 2 - 3 |        4         |  5 - 20   |
         *  ----------------------------------------------
         * |   EDP header  | Device ID opcode | Device ID |
         *  ----------------------------------------------
        */
        message_store_u8(edp_device_id, opcode, SECURITY_OPER_DEVICE_ID);
        message_store_array(edp_device_id, id, idigi_ptr->device_id, DEVICE_ID_LENGTH);

        status = initiate_send_packet(idigi_ptr, edp_header, device_id_message_size,
                                    E_MSG_MT2_TYPE_PAYLOAD, release_packet_buffer,
                                    NULL);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = security_send_server_url;
        }
        break;
    }
    case security_send_server_url:
    {
        /*
         * packet format:
         *  ------------------------------------------------
         * | 0 - 1 | 2 - 3 |    4       |   5 - 6    | 7... |
         *  ------------------------------------------------
         * |   EDP Header  | URL opcode | URL length | URL  |
         *  ------------------------------------------------
        */
        enum edp_server_url {
            field_define(edp_server_url, opcode, uint8_t),
            field_define(edp_server_url, url_length, uint16_t),
            record_end(edp_server_url)
        };
        size_t const device_url_header_size = record_bytes(edp_server_url);

        char * server_url = idigi_ptr->server_url;
        uint8_t * edp_server_url = start_ptr;
        char const url_prefix[] = URL_PREFIX;
        size_t const prefix_length = sizeof url_prefix -1;

        idigi_debug("security layer: send server url = %.*s\n", idigi_ptr->server_url_length, idigi_ptr->server_url);

        message_store_u8(edp_server_url, opcode, SECURITY_OPER_URL);

        {
            size_t const len = idigi_ptr->server_url_length + prefix_length;
            message_store_be16(edp_server_url, url_length, len);
        }

        edp_server_url += device_url_header_size;
        memcpy(edp_server_url, url_prefix, prefix_length);
        edp_server_url += prefix_length;

        memcpy(edp_server_url, server_url, idigi_ptr->server_url_length);
        edp_server_url += idigi_ptr->server_url_length;

        status = initiate_send_packet(idigi_ptr, edp_header, (uint16_t const)(edp_server_url-start_ptr),
                                    E_MSG_MT2_TYPE_PAYLOAD,
                                    release_packet_buffer,
                                    NULL);
        set_idigi_state(idigi_ptr, edp_discovery_layer);
    }
    } /* switch */

done:
    return status;
}

static idigi_callback_status_t discovery_layer(idigi_data_t * const idigi_ptr)
{
enum {
    discovery_send_vendor_id,
    discovery_send_device_type,
    discovery_facility,
    discovery_send_complete
};

    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * edp_header = NULL;
    uint8_t * start_ptr = NULL;

    /* discovery layer:
     * 1. send vendor ID
     * 2. send device type
     * 3. call each facility to send its own discovery layer
     * 4. send discovery complete message.
    */
    if (idigi_ptr->layer_state != discovery_facility)
    {
        edp_header = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &start_ptr, NULL);
        if (edp_header == NULL)
        {
            goto done;
        }
    }
    switch (idigi_ptr->layer_state)
    {
    case discovery_send_vendor_id:
    {
        /*
         * packet format:
         *  ----------------------------------------------------------------
         * | 0 - 1 | 2 - 3 |       4         |       5          |  6 - 9    |
         *  ----------------------------------------------------------------
         * |   EDP Header  | security coding | vendor ID opcode | vendor ID |
         *  ----------------------------------------------------------------
        */
        enum edp_vendor_msg {
            field_define(edp_vendor_msg, security_coding, uint8_t),
            field_define(edp_vendor_msg, opcode, uint8_t),
            field_define_array(edp_vendor_msg, vendor_id, VENDOR_ID_LENGTH),
            record_end(edp_vendor_msg)
        };
        uint16_t const discovery_vendor_header_size = record_bytes(edp_vendor_msg);
        uint8_t * edp_vendor_msg = start_ptr;

        message_store_u8(edp_vendor_msg, security_coding, SECURITY_PROTO_NONE);
        message_store_u8(edp_vendor_msg, opcode, DISC_OP_VENDOR_ID);
#if !defined(IDIGI_VENDOR_ID)
        message_store_array(edp_vendor_msg, vendor_id, idigi_ptr->vendor_id, VENDOR_ID_LENGTH);
        idigi_debug_hexvalue("discovery layer: send vendor id", idigi_ptr->vendor_id, VENDOR_ID_LENGTH);
#else
        {
            ASSERT(sizeof IDIGI_VENDOR_ID == VENDOR_ID_LENGTH);
            message_store_be32(edp_vendor_msg, vendor_id, IDIGI_VENDOR_ID);
            idigi_debug("discovery layer: send vendor id = 0x%X\n", IDIGI_VENDOR_ID);
        }
#endif


        status = initiate_send_packet(idigi_ptr, edp_header,
                                    discovery_vendor_header_size,
                                    E_MSG_MT2_TYPE_PAYLOAD,
                                    release_packet_buffer,
                                    NULL);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = discovery_send_device_type;
        }
        break;
    }

    case discovery_send_device_type:
    {
        /*
         * packet format:
         *  --------------------------------------------------------------------
         * | 0 - 1 | 2 - 3 |    4     |     5       |    6 - 7    |    8...     |
         *  --------------------------------------------------------------------
         * |   EDP Header  | security | device type | Device type | Device type |
         * |               |  coding  |   opcode    |   length    |             |
         *  --------------------------------------------------------------------
        */
        enum edp_device_type_msg {
            field_define(edp_device_type, security_coding, uint8_t),
            field_define(edp_device_type, opcode, uint8_t),
            field_define(edp_device_type, length, uint16_t),
            record_end(edp_device_type)
        };
        size_t const device_type_header_size = record_bytes(edp_device_type);

        uint8_t * edp_device_type = start_ptr;


#if defined(IDIGI_DEVICE_TYPE)
        static const char idigi_device_type[] = IDIGI_DEVICE_TYPE;
        size_t device_type_length  = sizeof idigi_device_type-1;
#else
        char * idigi_device_type = idigi_ptr->device_type;
        size_t const device_type_length = idigi_ptr->device_type_length;

#endif
        message_store_u8(edp_device_type, security_coding, SECURITY_PROTO_NONE);
        message_store_u8(edp_device_type, opcode, DISC_OP_DEVICETYPE);

        message_store_be16(edp_device_type, length, device_type_length);

        edp_device_type += device_type_header_size;
        memcpy(edp_device_type, idigi_device_type, device_type_length);

        idigi_debug("discovery layer: send device type = %.*s\n", device_type_length, idigi_device_type);

        status = initiate_send_packet(idigi_ptr, edp_header,
                                    (uint16_t)(device_type_header_size + device_type_length),
                                    E_MSG_MT2_TYPE_PAYLOAD,
                                    release_packet_buffer,
                                    NULL);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = discovery_facility;
        }
        break;
    }

    case discovery_facility:
        status = discovery_facility_layer(idigi_ptr);

        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = discovery_send_complete;
        }
        break;

    case discovery_send_complete:
    {
        /*
         * packet format:
         *  -----------------------------------------
         * | 0 - 1 | 2 - 3 |   4    |      5         |
         *  -----------------------------------------
         * |   EDP Header  | coding | initialization |
         * |               | scheme |   done opcode  |
         *  -----------------------------------------
        */
        enum edp_discovery_complete {
            field_define(edp_discovery_complete, security_coding, uint8_t),
            field_define(edp_discovery_complete, opcode, uint8_t),
            record_end(edp_discovery_complete)
        };
        uint16_t const discovery_complete_message_size = record_bytes(edp_discovery_complete);
        uint8_t * edp_discovery_complete = start_ptr;

        idigi_debug("discovery layer: send complete\n");
        message_store_u8(edp_discovery_complete, security_coding, SECURITY_PROTO_NONE);
        message_store_u8(edp_discovery_complete, opcode, DISC_OP_INITCOMPLETE);

        status = initiate_send_packet(idigi_ptr, edp_header,
                                    discovery_complete_message_size,
                                    E_MSG_MT2_TYPE_PAYLOAD,
                                    release_packet_buffer,
                                    NULL);
        if (status == idigi_callback_continue)
        {
            /* we are connected and EDP communication is fully established. */
            set_idigi_state(idigi_ptr, edp_facility_layer);
            idigi_ptr->edp_connected = idigi_true;
        }
         break;
    }
    }
done:
    return status;
}

static idigi_callback_status_t facility_layer(idigi_data_t * idigi_ptr)
{
enum {
    facility_receive_message,
    facility_process_message
};
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * packet = NULL;
    idigi_bool_t done_packet = idigi_true;
    idigi_facility_t * fac_ptr;


    /* Facility layer is the layer that IIK has fully established
     * communication with server. It keeps waiting messages from server
     * and passes it to the appropriate facility:
     * 1. waits message from server
     * 2. parses message and passes it to the facility
     * 3. invokes facility to process message.
     */
    switch (idigi_ptr->layer_state)
    {
    case facility_receive_message:
        status = receive_packet(idigi_ptr, &packet);
        if (status == idigi_callback_continue)
        {
            uint8_t * edp_header = packet;
            uint8_t * edp_protocol = packet + PACKET_EDP_HEADER_SIZE;
            ASSERT(packet != NULL);
            /*
             * received packet format:
             *  ----------------------------------------------------------
             * |  0 - 1  |  2 - 3 |   4    |     5     |  6 - 7   |  8... |
             *  ----------------------------------------------------------
             * | Payload | length | coding | discovery | facility | Data  |
             * |   Type  |        | scheme |  payload  |          |       |
             * -----------------------------------------------------------
             * |    EDP Header    |           EDP Protocol                |
             * -----------------------------------------------------------
            */
            if (message_load_be16(edp_header, type) == E_MSG_MT2_TYPE_PAYLOAD)
            {
                uint16_t length = message_load_be16(edp_header, length);
                uint8_t const sec_code = message_load_u8(edp_protocol, sec_coding);
                uint8_t const payload = message_load_u8(edp_protocol, payload);

                /* currently we don't support any other security protocol */
                ASSERT_GOTO(sec_code == SECURITY_PROTO_NONE, error);
                ASSERT_GOTO(payload == DISC_OP_PAYLOAD, error);
                ASSERT_GOTO(length > PACKET_EDP_PROTOCOL_SIZE, error);

                idigi_debug("idigi_facility_layer: receive data facility = 0x%04x\n", message_load_be16(edp_protocol, facility));
                /* adjust the length for facility process */
                length -= PACKET_EDP_PROTOCOL_SIZE;
                message_store_be16(edp_header, length, length);

            }
        }
        break;
    case facility_process_message:
        /* existing packet */
        packet = idigi_ptr->receive_packet.data_packet;
        break;
    }

    if ((status == idigi_callback_continue) && (packet != NULL) )
    {
        uint8_t * edp_header = packet;

        if (message_load_be16(edp_header, type) == E_MSG_MT2_TYPE_PAYLOAD)
        {
            uint16_t facility;
            uint8_t * edp_protocol = packet + PACKET_EDP_HEADER_SIZE;
            /* search facility
             *
             * Make sure the facility is not processing previous packet.
             */
            facility = message_load_be16(edp_protocol, facility);
            idigi_ptr->layer_state = facility_receive_message;
            for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
            {
                if (fac_ptr->facility_num  == facility)
                {
                    if (fac_ptr->packet == NULL)
                    {
                        fac_ptr->packet = packet;
                        idigi_ptr->active_facility = fac_ptr;
                        done_packet = idigi_false;

                    }
                    else
                    { /* Facility is busy so hold on the packet and stop receiving data */
                        idigi_ptr->layer_state = facility_process_message;
                        done_packet = idigi_false;
                    }
                    break;
                }
            }
        }
    }
error:
    /* if we are done with packet, release it for another
     * receive_packet .
     */
    if (done_packet && packet != NULL)
    {
        release_receive_packet(idigi_ptr, packet);
    }

    if (status != idigi_callback_abort)
    {
        /* invoke facility process */
        for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
        {
            unsigned int const i = fac_ptr->facility_index;

            if (idigi_supported_facility_table[i].process_cb)
            {
                status = idigi_supported_facility_table[i].process_cb(idigi_ptr, fac_ptr->facility_data,
                                                                      fac_ptr->packet, &idigi_ptr->receive_packet.timeout);
                if (status != idigi_callback_busy && fac_ptr->packet != NULL)
                {   /* release the packet when it's done */
                    release_receive_packet(idigi_ptr, fac_ptr->packet);
                    fac_ptr->packet = NULL;
                }

                if (status != idigi_callback_abort)
                {
                    uint32_t rx_keepalive;
                    uint32_t tx_keepalive;
                    uint32_t current_system_time;
                    /* check rx_keepalive and tx_keepalive timing */
                    status =  get_keepalive_timeout(idigi_ptr, &rx_keepalive, &tx_keepalive, &current_system_time);
                    if (rx_keepalive == 0 || tx_keepalive == 0 || status != idigi_callback_continue)
                    {
                        break;
                    }
                }
                else if (status == idigi_callback_unrecognized)
                {
                    status = idigi_callback_continue;
                }
            }
        }
    }

    return status;
}

