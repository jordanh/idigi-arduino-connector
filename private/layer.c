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

typedef idigi_callback_status_t (* idigi_facility_table_t)(struct idigi_data * const idigi_ptr);

typedef struct {
    idigi_config_request_t facility;
    idigi_facility_table_t init_cb;
    idigi_facility_table_t delete_cb;
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
        {MANDATORY_FACILITY, cc_init_facility, cc_delete_facility},

        /* list of optional facilities */
#if defined(_FIRMWARE_FACILITY)
        {idigi_config_firmware_facility, fw_init_facility, fw_delete_facility},
#endif
#if defined(IDIGI_DATA_SERVICE)
        {idigi_config_data_service, data_service_init, data_service_delete}
#endif
};

static size_t const idigi_facility_count = asizeof(idigi_supported_facility_table);

static idigi_callback_status_t  remove_facility_layer(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t result = idigi_callback_continue;
    size_t i = 0;

    /* remove all facilities. delete_cb should not return busy.
     * If it returns busy, just keep calling it.
     */
    while (i < idigi_facility_count)
    {
        idigi_callback_status_t status = idigi_callback_continue;

        if (IS_FACILITY_SUPPORTED(idigi_ptr, i) &&
            idigi_supported_facility_table[i].delete_cb != NULL)
        {
            status = idigi_supported_facility_table[i].delete_cb(idigi_ptr);
            if (status != idigi_callback_continue)
            {
                /* Abort by callback. Save the abort status and
                 * continue removing the rest of the facilities.
                 */
                result = idigi_callback_abort;
            }
        }
        if (status != idigi_callback_busy)
        {
            i++;
        }
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
        DEBUG_PRINTF("Discovery Facility layer: 0x%x\n", fac_ptr->facility_num);
        if (fac_ptr->discovery_cb != NULL)
        {   /* function to send facility discovery */
            status = fac_ptr->discovery_cb(idigi_ptr, fac_ptr->facility_data, NULL);
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
            idigi_config_server_url, idigi_config_rx_keepalive, idigi_config_tx_keepalive, idigi_config_wait_count
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

        status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &data, &length);
        if (status != idigi_callback_continue)
        {
            /* set error code if it has not been set */
            if (idigi_ptr->error_code == idigi_success)
            {
                idigi_ptr->error_code = idigi_configuration_error;
            }
            goto done;
        }

        if (data == NULL)
        {
            /* callback cannot return */
            idigi_ptr->error_code = idigi_invalid_data;
            goto error;
        }

        switch(request_id.config_request)
        {
        case idigi_config_server_url:
            if ((length == 0) || (length > SERVER_URL_LENGTH))
            {
                idigi_ptr->error_code = idigi_invalid_data_range;
                goto error;
            }
            memcpy(idigi_ptr->server_url, data, length);
            idigi_ptr->server_url[length] = 0x0;
            idigi_ptr->server_url_length = length;
            break;
        case idigi_config_tx_keepalive:
        case idigi_config_rx_keepalive:
        {
            uint16_t * value = data;

            if ((*value < MIN_KEEPALIVE_INTERVAL_IN_SECONDS) ||
                (*value > MAX_KEEPALIVE_INTERVAL_IN_SECONDS) ||
                (length != sizeof *value))
            {
                idigi_ptr->error_code = idigi_invalid_data_range;
                goto error;
            }
            if (request_id.config_request == idigi_config_tx_keepalive)
            {
                idigi_ptr->tx_keepalive_interval = (uint16_t *)data;
            }
            else
            {
                idigi_ptr->rx_keepalive_interval = (uint16_t *)data;
            }
            break;
        }
        case idigi_config_wait_count:
        {
            uint16_t * value = data;

            if ((*value < WAIT_COUNT_MIN) ||
                (*value > WAIT_COUNT_MAX)||
                (length != sizeof *value))
            {
                idigi_ptr->error_code = idigi_invalid_data_range;
                goto error;
            }
            idigi_ptr->wait_count = (uint16_t *)data;
            break;
        }
        default:
            /* get these configurations from different modules */
            ASSERT(false);
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

done:
    return status;
}

static idigi_callback_status_t get_supported_facilities(idigi_data_t * const idigi_ptr)
{
#define NUMBER_FACILITY_PER_BYTE CHAR_BIT

    idigi_callback_status_t status = idigi_callback_continue;
    size_t  i;

    idigi_ptr->facilities = 0;

    ASSERT(CHAR_BIT == 8);
    ASSERT(idigi_facility_count <= (sizeof idigi_ptr->facilities * NUMBER_FACILITY_PER_BYTE));

    /* idigi_supported_facility_table[] table includes a list of facilities that iDigi supports.
     * Call callback to see which facility is supported.
     */

    for (i=0; i < idigi_facility_count; i++)
    {
        idigi_request_t request_id;
        size_t length;
        bool facility_enable = true;

        request_id.config_request = idigi_supported_facility_table[i].facility;
        if (request_id.config_request != (idigi_config_request_t)MANDATORY_FACILITY)
        {   /* this is optional facility so ask application whether it supports this facility */
            status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &facility_enable, &length);
            if (status != idigi_callback_continue)
            {
                if (status == idigi_callback_abort)
                {
                    DEBUG_PRINTF("initialize_facilities: callback returns %d on facility= %d\n", status, request_id.config_request);
                    idigi_ptr->error_code = idigi_configuration_error;
                }
                break;
            }

        }

        if (facility_enable)
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
            status = idigi_supported_facility_table[i].init_cb(idigi_ptr);
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
enum {
    configuration_get_configurations,
    configuration_init_facilities,
};

    idigi_callback_status_t status = idigi_callback_continue;

    /* This layer is called to get some of the EDP configuration and initialize each facility */
    switch (idigi_ptr->layer_state)
    {
    case configuration_get_configurations:
        status = get_configurations(idigi_ptr);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->request_id = 0;
            idigi_ptr->layer_state = configuration_init_facilities;
        }
        /* fall through */
    case configuration_init_facilities:
        status = initialize_facilities(idigi_ptr);
        if (status == idigi_callback_continue)
        {
            set_idigi_state(idigi_ptr, edp_communication_layer);
        }
        break;
    }

    return status;

}

static idigi_callback_status_t send_version(idigi_data_t * idigi_ptr, uint16_t const type, uint32_t const version)
{
enum edp_version {
    field_define(edp_version, version, uint32_t),
    record_end(edp_version)
};
#define EDP_VERSION_SIZE   record_bytes(edp_version)

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

    packet = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &edp_version);
    if (packet != NULL)
    {
        message_store_be32(edp_version, version, version);

        status = initiate_send_packet(idigi_ptr, packet, EDP_VERSION_SIZE,
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
    case communication_connect_server:
        if (idigi_ptr->network_handle == NULL)
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

        DEBUG_PRINTF("communication layer: Send MT Version\n");
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

            DEBUG_PRINTF("communication layer: receive Mt version\n");
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
        uint16_t timeout;
        uint16_t wait_count;
        uint8_t * data_ptr;
        int len;

        DEBUG_PRINTF("communication layer: send keepalive params \n");

        packet = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &data_ptr);
        ptr = (uint8_t *)packet;

        timeout = *idigi_ptr->rx_keepalive_interval;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_RX_INTERVAL, timeout);
        ptr += len;
        idigi_ptr->send_packet.total_length = len;

        timeout = *idigi_ptr->tx_keepalive_interval;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_TX_INTERVAL, timeout);
        ptr += len;
        idigi_ptr->send_packet.total_length += len;

        wait_count = *idigi_ptr->wait_count;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_WAIT, wait_count);
        ptr += len;
        idigi_ptr->send_packet.total_length += len;

        /* Setting the total_length will enable send_packet_process.
         * Clear length to 0 for actual length that has been sent.
         */
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
        DEBUG_PRINTF("initialization layer: send protocol version\n");
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
        if (status == idigi_callback_continue && edp_header != NULL)
        {

            DEBUG_PRINTF("initialization layer: receive protocol version\n");
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
    uint16_t len;
    uint8_t * start_ptr;

    /* security layer:
     * 1. sends identity verification form
     * 2. sends device ID
     * 3. sends server URL
     * 4. sends password if identity verification form is PASSWORD identity
     */
    edp_header = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &start_ptr);
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
        size_t const edp_security_size = record_bytes(edp_security);
        uint8_t * edp_security = start_ptr;

        DEBUG_PRINTF("Security layer: send security form\n");

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
        #define EDP_DEVICE_ID_SIZE   record_bytes(edp_device_id)

        uint8_t * edp_device_id = start_ptr;

        DEBUG_PRINTF("security layer: send device ID length = %d\n", EDP_DEVICE_ID_SIZE);
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

        status = initiate_send_packet(idigi_ptr, edp_header, EDP_DEVICE_ID_SIZE,
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
        #define EDP_DEVICE_URL_HEADER_SIZE   record_bytes(edp_server_url)

        char * server_url = idigi_ptr->server_url;
        uint8_t * edp_server_url = start_ptr;
        char const url_prefix[] = URL_PREFIX;
        size_t const prefix_length = sizeof url_prefix -1;

        DEBUG_PRINTF("security layer: send server url\n");

        len = idigi_ptr->server_url_length + prefix_length;

        message_store_u8(edp_server_url, opcode, SECURITY_OPER_URL);
        message_store_be16(edp_server_url, url_length, len);
        edp_server_url += EDP_DEVICE_URL_HEADER_SIZE;

        len =  idigi_ptr->server_url_length;
        memcpy(edp_server_url, url_prefix, prefix_length);
        edp_server_url += prefix_length;
        memcpy(edp_server_url, server_url, len);
        edp_server_url += len;

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
        edp_header = get_packet_buffer(idigi_ptr, E_MSG_MT2_MSG_NUM, &start_ptr);
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
        #define EDP_DISCOVERY_VENDOR_HEADER_SIZE   record_bytes(edp_vendor_msg)

        uint8_t * edp_vendor_msg = start_ptr;

        DEBUG_PRINTF("discovery layer: send vendor id\n");
        message_store_u8(edp_vendor_msg, security_coding, SECURITY_PROTO_NONE);
        message_store_u8(edp_vendor_msg, opcode, DISC_OP_VENDOR_ID);
        message_store_array(edp_vendor_msg, vendor_id, idigi_ptr->vendor_id, VENDOR_ID_LENGTH);


        status = initiate_send_packet(idigi_ptr, edp_header,
                                    EDP_DISCOVERY_VENDOR_HEADER_SIZE,
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

        DEBUG_PRINTF("discovery layer: send device type\n");
        message_store_u8(edp_device_type, security_coding, SECURITY_PROTO_NONE);
        message_store_u8(edp_device_type, opcode, DISC_OP_DEVICETYPE);

        message_store_be16(edp_device_type, length, idigi_ptr->device_type_length);

        edp_device_type += device_type_header_size;
        memcpy(edp_device_type, idigi_ptr->device_type, idigi_ptr->device_type_length);

        status = initiate_send_packet(idigi_ptr, edp_header,
                                    (uint16_t)(device_type_header_size + idigi_ptr->device_type_length),
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
        #define EDP_DISCOVERY_COMPLETE_SIZE   record_bytes(edp_discovery_complete)

        uint8_t * edp_discovery_complete = start_ptr;

        DEBUG_PRINTF("discovery layer: send complete\n");
        message_store_u8(edp_discovery_complete, security_coding, SECURITY_PROTO_NONE);
        message_store_u8(edp_discovery_complete, opcode, DISC_OP_INITCOMPLETE);

        status = initiate_send_packet(idigi_ptr, edp_header,
                                    EDP_DISCOVERY_COMPLETE_SIZE,
                                    E_MSG_MT2_TYPE_PAYLOAD,
                                    release_packet_buffer,
                                    NULL);
        if (status == idigi_callback_continue)
        {
            set_idigi_state(idigi_ptr, edp_facility_layer);
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
    bool done_packet = true;
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
        if (status == idigi_callback_continue && packet != NULL)
        {
            uint8_t * edp_header = packet;
            uint8_t * edp_protocol = packet + PACKET_EDP_HEADER_SIZE;

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

                DEBUG_PRINTF("idigi_facility_layer: receive data facility = 0x%04x\n", message_load_be16(edp_protocol, facility));
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
                        done_packet = false;

                    }
                    else
                    { /* Facility is busy so hold on the packet and stop receiving data */
                        idigi_ptr->layer_state = facility_process_message;
                        done_packet = false;
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
            if (fac_ptr->packet != NULL)
            {
                status = fac_ptr->process_cb(idigi_ptr, fac_ptr->facility_data, fac_ptr->packet);
                if (status != idigi_callback_busy)
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

            }
        }/* for */
    }

    return status;
}

