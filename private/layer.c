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

#define EDP_PROTOCOL_VERSION    0x120

#define KEEPALIVE_PARAMETER_LENGTH  6
#define SERVER_OVERLOAD_RESPONSE    0x02

#define MANDATORY_FACILITY          -1

typedef idigi_callback_status_t (* idigi_facility_init_cb_t )(struct idigi_data * idigi_ptr);

typedef struct {
    idigi_config_request_t  facility;
    idigi_facility_init_cb_t init_cb;
    idigi_facility_init_cb_t delete_cb;
} idigi_facility_init_t;

/* Table of all facilites that iDigi supports.
 *
 * iDigi will call the callback to see whether it supports each optional facility.
 * iDigi will call init_cb to initialize the facility and delete_cb to remove the facility.
 * The init_cb must call add_facility_data() to add the facility into iDigi facility list.
 * The delete_cb is called to delete the facility from iDigi facility list when user terminates iDigi.
 */
static idigi_facility_init_t idigi_facility_init_cb[] = {
        /* mandatory facilities */
        {(idigi_config_request_t)MANDATORY_FACILITY, (idigi_facility_init_cb_t)cc_init_facility, (idigi_facility_init_cb_t)cc_delete_facility},
        {(idigi_config_request_t)MANDATORY_FACILITY, (idigi_facility_init_cb_t)loopback_init_facility, (idigi_facility_init_cb_t)loopback_delete_facility},

        /* list of optional facilities */
        {idigi_config_firmware_facility, (idigi_facility_init_cb_t)fw_init_facility, (idigi_facility_init_cb_t)fw_delete_facility},

        /* list of optional services over messaging facility */
        {idigi_config_data_service, data_service_init, data_service_delete}
};

static size_t idigi_facility_count = asizeof(idigi_facility_init_cb);

static idigi_callback_status_t  remove_facility_layer(idigi_data_t * idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue, rc = idigi_callback_continue;

    while (idigi_ptr->request_id < (int)idigi_facility_count)
    {
        if ((idigi_ptr->facilities & (0x1 << idigi_ptr->request_id)) &&
             idigi_facility_init_cb[idigi_ptr->request_id].delete_cb != NULL)
        {
            status = idigi_facility_init_cb[idigi_ptr->request_id].delete_cb(idigi_ptr);
            if (status == idigi_callback_continue)
            {
                idigi_ptr->request_id++;
            }
            else
            {
                rc = idigi_callback_abort;
            }
        }
        else
        {
            idigi_ptr->request_id++;
        }
    }

    return rc;
}

static int msg_add_keepalive_param(uint8_t * buf, uint16_t type, uint16_t value)
{
    uint8_t     * ptr = buf;
    int         rc;
    size_t      size;

    size = sizeof(type);
    StoreBE16(ptr, type);
    ptr += size;

    StoreBE16(ptr, sizeof value);
    ptr += size;

    StoreBE16(ptr, value);

    rc = KEEPALIVE_PARAMETER_LENGTH;

    return rc; /* return count of bytes added to buffer */
}

static idigi_callback_status_t discovery_facility_layer(idigi_data_t * idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_facility_t * fac_ptr;

    /* invoke any facility that needs to send any message to server
     * during initialization phase at discovery layer.
     */
    fac_ptr = idigi_ptr->active_facility;
    if (fac_ptr == NULL)
    {
        fac_ptr = idigi_ptr->facility_list;
    }

    while (fac_ptr != NULL && status == idigi_callback_continue)
    {
        DEBUG_PRINTF("Discovery Facility layer: 0x%x\n", fac_ptr->facility_num);
        if (fac_ptr->discovery_cb == NULL)
        {
            status = idigi_callback_continue;
        }
        else
        {   /* function to send facility discovery */
            status = fac_ptr->discovery_cb(idigi_ptr, fac_ptr->facility_data, NULL);
        }

        if (status == idigi_callback_continue)
        {
            fac_ptr = idigi_ptr->active_facility = fac_ptr->next;
        }
    }
    return status;
}

static idigi_callback_status_t get_configurations(idigi_data_t * idigi_ptr)
{
#define INIT_CONFIG_ID_COUNT    5
    idigi_callback_status_t status = idigi_callback_continue;
    void * data = NULL;
    size_t length;
    idigi_request_t request_id;

    unsigned idigi_edp_init_config_ids[INIT_CONFIG_ID_COUNT] = {
            idigi_config_server_url, idigi_config_rx_keepalive, idigi_config_tx_keepalive, idigi_config_wait_count, idigi_config_password,
    };

   /* Call callback to get server url, wait count, tx keepalive, rx keepalive, & password.
    * Call error status callback if error is encountered (NULL data, invalid range, invalid size).
    */
    while(idigi_ptr->request_id < INIT_CONFIG_ID_COUNT)
    {
        request_id.config_request = idigi_edp_init_config_ids[idigi_ptr->request_id];

        status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &data, &length);
        if (status == idigi_callback_continue)
        {
            if (request_id.config_request != idigi_config_password && data == NULL)
            {
                /* callback cannot return NULL except idigi_config_password */
                idigi_ptr->error_code = idigi_invalid_data;
                goto error;
            }
            else
            {
                switch(request_id.config_request)
                {
                case idigi_config_password:
                    idigi_ptr->password = (char *)data;
                   break;
                case idigi_config_server_url:
                    idigi_ptr->server_url = (char *)data;
                    if (length == 0 || length > SERVER_URL_LENGTH)
                    {
                        idigi_ptr->error_code = idigi_invalid_data_range;
                        goto error;
                    }
                    break;
                case idigi_config_tx_keepalive:
                    idigi_ptr->tx_keepalive = (uint16_t *)data;

                    if (*idigi_ptr->tx_keepalive < MIN_TX_KEEPALIVE_INTERVAL_PER_SECOND || *idigi_ptr->tx_keepalive > MAX_TX_KEEPALIVE_INTERVAL_PER_SECOND ||
                        length != sizeof(uint16_t))
                    {
                        idigi_ptr->error_code = idigi_invalid_data_range;
                        goto error;
                    }
                    break;
                case idigi_config_rx_keepalive:
                    idigi_ptr->rx_keepalive = (uint16_t *)data;
                    if (*idigi_ptr->rx_keepalive < MIN_RX_KEEPALIVE_INTERVAL_PER_SECOND || *idigi_ptr->rx_keepalive > MAX_RX_KEEPALIVE_INTERVAL_PER_SECOND ||
                        length != sizeof(uint16_t))
                    {
                        idigi_ptr->error_code = idigi_invalid_data_range;
                        goto error;
                    }
                    break;
                case idigi_config_wait_count:
                    idigi_ptr->wait_count = (uint8_t *)data;
                    if (*idigi_ptr->wait_count < WAIT_COUNT_MIN || *idigi_ptr->wait_count > WAIT_COUNT_MAX ||
                        length != sizeof(uint8_t))
                    {
                        idigi_ptr->error_code = idigi_invalid_data_range;
                        goto error;
                        
                    }
                    break;
                case idigi_config_device_id:
                case idigi_config_vendor_id:
                case idigi_config_device_type:
                    /* got these configurations in init function */
                    break;
                case idigi_config_connection_type:
                case idigi_config_mac_addr:
                case idigi_config_link_speed:
                case idigi_config_phone_number:
                case idigi_config_ip_addr:
                case idigi_config_error_status:
                case idigi_config_disconnected:
                case idigi_config_firmware_facility:
                case idigi_config_data_service:
                    /* get these configurations from different modules */
                    break;
                }
                idigi_ptr->request_id++;
            }
        }
        else
        {
            if (status == idigi_callback_abort && idigi_ptr->error_code == idigi_success)
            {
                idigi_ptr->error_code = idigi_configuration_error;
            }
            goto done;
        }
    } /* while */


error:
    if (idigi_ptr->error_code != idigi_success)
    {
        status = notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
    }

    if (status == idigi_callback_continue && idigi_ptr->request_id != INIT_CONFIG_ID_COUNT)
    {
        status = idigi_callback_busy;
    }

done:
    return status;
}

static idigi_callback_status_t initialize_facilites(idigi_data_t * idigi_ptr)
{
    idigi_callback_status_t status;
    bool facility_enable;
    int  idx;
    idigi_request_t request_id;
    size_t length;

    /* idigi_facility_init_cb[] table includes a list of facilities that iDigi supports.
     * Call callback to see which facility is supported.
     */
    idigi_ptr->facilities = 0;
    while (idigi_ptr->request_id < idigi_facility_count)
    {
        idx = idigi_ptr->request_id;

        request_id.config_request = idigi_facility_init_cb[idx].facility;
        if (request_id.config_request != (idigi_config_request_t)MANDATORY_FACILITY)
        {   /* this is optional facility so ask application whether it supports this facility */
            status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &facility_enable, &length);
            if (status != idigi_callback_continue)
            {
                DEBUG_PRINTF("initialize_facilities: callback returns %d on facility= %d\n", status, request_id.config_request);
                idigi_ptr->error_code = idigi_configuration_error;
                goto done;
            }

        }
        else
        {
            /* mandatory facility */
            facility_enable = true;
        }

        if (facility_enable)
        {
            /* let's call facility init_cb function */
            if (idigi_facility_init_cb[idx].init_cb != NULL)
            {
                status = idigi_facility_init_cb[idx].init_cb(idigi_ptr);
            }
            idigi_ptr->facilities |= (0x01 << idx);
        }

        if (status == idigi_callback_continue)
        {
            idigi_ptr->request_id++;
        }
        else
        {
            goto done;
        }

    }

done:
    return status;
}
static idigi_callback_status_t configuration_layer(idigi_data_t * idigi_ptr)
{
    enum {
        configuration_get_configurations,
        configuration_init_facilities,
    };

    idigi_callback_status_t status = idigi_callback_continue;

    /* This layer is called to get some of the EDP configuration and initialize facilities */
    if (idigi_ptr->layer_state == configuration_get_configurations)
    {
        status = get_configurations(idigi_ptr);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->facilities = 0;
            idigi_ptr->request_id = 0;
            idigi_ptr->layer_state = configuration_init_facilities;
        }
    }
    if (idigi_ptr->layer_state == configuration_init_facilities)
    {
        status = initialize_facilites(idigi_ptr);
        if (status == idigi_callback_continue)
        {
            set_idigi_state(idigi_ptr, edp_communication_layer);
        }
    }

    return status;

}

static idigi_callback_status_t communication_layer(idigi_data_t * idigi_ptr)
{
    enum {
        communication_connect_server,
        communication_send_version,
        communication_receive_version_response,
        communication_send_keepalive
    };

    idigi_callback_status_t status = idigi_callback_continue;
    uint32_t version;
    uint8_t * ptr;
    int len;
    idigi_packet_t * packet;

    /* communitcation layer:
     *  1. establishes connection.
     *  2. sends MT version
     *  3. receives and validates MT version response
     *  4. sends tx, rx, & waitcount parameter
     */
    if (idigi_ptr->layer_state == communication_connect_server)
    {
        if (idigi_ptr->network_handle == NULL)
        {
            status = connect_server(idigi_ptr, idigi_ptr->server_url, EDP_MT_PORT);
        }

        if (status == idigi_callback_continue)
        {
            idigi_ptr->request_id = 0;
            idigi_ptr->layer_state = communication_send_version;
        }

    }

    if (idigi_ptr->layer_state == communication_send_version)
    {
        DEBUG_PRINTF("communication layer: Send MT Version\n");
        /*
         * MT version packet format:
         *  -------------------------
         * |0 - 1 |  2 - 3 |  4 - 7  |
         *  -------------------------
         * | Type | length | version |
         *  -------------------------
        */
        packet =(idigi_packet_t *) get_packet_buffer(idigi_ptr, sizeof(idigi_packet_t), &ptr);
        if (packet == NULL)
        {
            goto done;
        }
        packet->type = E_MSG_MT2_TYPE_VERSION;
        packet->length = sizeof version;

        StoreBE32(ptr, EDP_MT_VERSION);

        status = enable_send_packet(idigi_ptr, packet);
        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = communication_receive_version_response;
        }

    }


    else if (idigi_ptr->layer_state == communication_receive_version_response)
    {
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

            ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
            type = packet->type;
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
                goto done;
            }
            else
            {   /* advance to send keepalive parameters */
                idigi_ptr->layer_state = communication_send_keepalive;
            }
        }
    }

    else if (idigi_ptr->layer_state == communication_send_keepalive)
    {
        uint16_t    timeout;
        uint8_t     wait_count;

        DEBUG_PRINTF("communication layer: send keepalive params \n");
        /* combine Rx keepalive interval, Tx keepalive interval and wait count
         * into a single packet.
         *
         * packet format:
         *  -----------------------------
         * |0 - 1 |  2 - 3 |   4 - 5     |
         *  -----------------------------
         * | Type | length | Rx interval |
         *  -----------------------------
         *
         *  -----------------------------
         * |6 - 7 |  8 - 9 |  10 - 11    |
         *  -----------------------------
         * | Type | length | Tx interval |
         *  -----------------------------

         *  ----------------------------
         * |12- 13| 14 - 15|  16 - 17   |
         *  ----------------------------
         * | Type | length | wait count |
         *  ----------------------------
         */


        ptr = idigi_ptr->send_packet.ptr = idigi_ptr->send_packet.buffer;

        timeout = *idigi_ptr->rx_keepalive;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_RX_INTERVAL, timeout);
        ptr += len;
        idigi_ptr->send_packet.total_length = len;

        timeout = *idigi_ptr->tx_keepalive;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_TX_INTERVAL, timeout);
        ptr += len;
        idigi_ptr->send_packet.total_length += len;

        wait_count = *idigi_ptr->wait_count;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_WAIT, (uint16_t)wait_count);
        ptr += len;

        /* This will enable send_packet.
         * Set total_length to be sent and clear length to 0
         * for actual length that has been sent.
         */
        idigi_ptr->send_packet.total_length += len;
        idigi_ptr->send_packet.length = 0;

        set_idigi_state(idigi_ptr, edp_initialization_layer);

    }

done:
    return status;
}

static idigi_callback_status_t initialization_layer(idigi_data_t * idigi_ptr)
{
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
    idigi_packet_t * packet;
    uint8_t * ptr;

    /* initialization layer:
     * 1. sends protocol version.
     * 2. receives and validates protocol version response
     */
    switch (idigi_ptr->layer_state)
    {
    case initialization_send_protocol_version:
    {
        uint32_t version = EDP_PROTOCOL_VERSION;

        DEBUG_PRINTF("initialization layer: send protocol version\n");
        /*
         *  version packet format:
         *  ---------------------------------
         * |    0 - 1     |  2 - 3 |  4 - 7  |
         *  ----------------------------------
         * | Payload Type | length | version |
         *  ----------------------------------
        */

        packet =(idigi_packet_t *) get_packet_buffer(idigi_ptr, sizeof(idigi_packet_t), &ptr);
        if (packet == NULL)
        {
            goto done;
        }
        packet->length = sizeof(version);
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        StoreBE32(ptr, version);

        status = enable_send_packet(idigi_ptr, packet);
        idigi_ptr->layer_state = initialization_receive_protocol_version;
        break;
    }
    case initialization_receive_protocol_version:
        status = receive_packet(idigi_ptr, &packet);
        if (status == idigi_callback_continue && packet != NULL)
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
            if (packet->length > 0)
            {

                /* Parse the version response (0 = version response ok).
                 * If the protocol version number was not acceptable to the server,
                 * tell the application.
                 */
                ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
                if (*ptr != initialization_version_response_acceptable)
                {
                    idigi_request_t request_id;
                    status = idigi_callback_abort;
                    request_id.network_request = idigi_network_receive;
                    idigi_ptr->error_code = idigi_bad_version;
                    notify_error_status(idigi_ptr->callback, idigi_class_network, request_id, idigi_ptr->error_code);
                    goto done;

                }
                set_idigi_state(idigi_ptr, edp_security_layer);
            }
            release_receive_packet(idigi_ptr, packet);
        }
        break;
    }

done:

    return status;
}

static idigi_callback_status_t security_layer(idigi_data_t * idigi_ptr)
{
    enum {
            security_send_identity_verification,
            security_send_device_id,
            security_send_server_url,
            security_send_password,
    };
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_packet_t * packet;
    char * pwd = NULL;
    uint16_t len, size;
    uint8_t * ptr, * data_ptr;
    char * url_prefix = URL_PREFIX;


    /* security layer:
     * 1. sends identity verification form
     * 2. sends device ID
     * 3. sends server URL
     * 4. sends password if identity verification form is PASSWORD identity
     */
    packet =(idigi_packet_t *) get_packet_buffer(idigi_ptr, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        goto done;
    }
    data_ptr = ptr;

    switch (idigi_ptr->layer_state)
    {
    case security_send_identity_verification:
        DEBUG_PRINTF("Security layer: send security form\n");

        /*
         * packet format:
         *  -------------------------------------------------------------
         * |    0 - 1     |  2 - 3 |     4       |     5                 |
         *  -------------------------------------------------------------
         * | Payload Type | length | Form opcode | identity verification |
         *  -------------------------------------------------------------
        */
        *ptr++ = SECURITY_OPER_IDENT_FORM;
        /* If password is NULL, we use SIMPLE FORM identity.
         * Otherwise, use PASSWORD FORM identity.
         */
        if (pwd != NULL)
        {
            *ptr = (uint8_t)SECURITY_IDENT_FORM_PASSWORD;
        }
        else
        {
            *ptr = (uint8_t)SECURITY_IDENT_FORM_SIMPLE;
        }

        idigi_ptr->security_form = *ptr++ ;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;
        packet->length = ptr - data_ptr;

        status = enable_send_packet(idigi_ptr, packet);

        idigi_ptr->layer_state = security_send_device_id;
        break;

    case security_send_device_id:
    {
        uint8_t * device_id;

        DEBUG_PRINTF("security layer: send device ID\n");
        /*
         * packet format:
         *  ------------------------------------------------------
         * |    0 - 1     |  2 - 3 |        4         |  5 - 20   |
         *  ------------------------------------------------------
         * | Payload Type | length | Device ID opcode | Device ID |
         *  ------------------------------------------------------
        */

        packet->type = E_MSG_MT2_TYPE_PAYLOAD;
        *ptr++ = SECURITY_OPER_DEVICE_ID;
        packet->length = 1;

        device_id = (uint8_t *)idigi_ptr->device_id;

        memcpy(ptr, device_id, DEVICE_ID_LENGTH);
        packet->length += DEVICE_ID_LENGTH;

        status = enable_send_packet(idigi_ptr, packet);
        idigi_ptr->layer_state = security_send_server_url;
        break;
    }
    case security_send_server_url:
    {
        char * server_url;
        DEBUG_PRINTF("security layer: send server url\n");
        /*
         * packet format:
         *  --------------------------------------------------------
         * |    0 - 1     |  2 - 3 |    4       |   5 - 6    | 7... |
         *  --------------------------------------------------------
         * | Payload Type | length | URL opcode | URL length | URL  |
         *  --------------------------------------------------------
        */
        data_ptr = ptr;

        server_url = (char *)idigi_ptr->server_url;

        len = strlen(server_url) + strlen(URL_PREFIX);
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        *ptr = SECURITY_OPER_URL;
        ptr++;

        size = sizeof len;
        StoreBE16(ptr, len);
        ptr += size;

        len =  strlen(server_url);
        if (len > 0)
        {
            size = strlen(url_prefix);
            memcpy(ptr, url_prefix, size);
            ptr += size;
            memcpy(ptr, server_url, len);
            ptr += len;
        }
        packet->length = ptr - data_ptr;

        status = enable_send_packet(idigi_ptr, packet);
        if (idigi_ptr->security_form == SECURITY_IDENT_FORM_PASSWORD)
        {
            idigi_ptr->layer_state = security_send_password;
        }
        else
        {
            set_idigi_state(idigi_ptr, edp_discovery_layer);
        }
        break;
    }
    case security_send_password:
        DEBUG_PRINTF("security layer: send password\n");
        /*
         * packet format:
         *  ----------------------------------------------------------------------
         * |    0 - 1     |  2 - 3 |       4         |      5          | 6...     |
         *  ----------------------------------------------------------------------
         * | Payload Type | length | password opcode | password length | password |
         *  ----------------------------------------------------------------------
        */
        pwd = (char *)idigi_ptr->password;

        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        *ptr = SECURITY_OPER_PASSWD;
        ptr++;

        size = sizeof len;
        len = strlen(pwd);
        StoreBE16(ptr, len);
        ptr += size;
        if (len > 0)
        {
            memcpy(ptr, pwd, strlen(pwd));
            ptr += len;
        }
        packet->length = ptr - data_ptr;
        status = enable_send_packet(idigi_ptr, packet);
        set_idigi_state(idigi_ptr, edp_discovery_layer);
        break;
    }
done:
    return status;
}
static idigi_callback_status_t discovery_layer(idigi_data_t * idigi_ptr)
{
    enum {
        discovery_send_vendor_id,
        discovery_send_device_type,
        discovery_facility,
        discovery_send_complete
    };
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_packet_t * packet;
    uint8_t sec_coding = SECURITY_PROTO_NONE;
    uint8_t * ptr, * data_ptr;

    /* discovery layer:
     * 1. send vendor ID
     * 2. send device type
     * 3. call each facility to send its own discovery layer
     * 4. send discovery complete message.
    */
    packet =(idigi_packet_t *) get_packet_buffer(idigi_ptr, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        goto done;
    }
    data_ptr = ptr;

    switch (idigi_ptr->layer_state)
    {
    case discovery_send_vendor_id:
    {
        uint8_t * vendor_id;

        DEBUG_PRINTF("discovery layer: send vendor id\n");
        /*
         * packet format:
         *  ------------------------------------------------------------------------
         * |    0 - 1     |  2 - 3 |       4         |       5          |  6 - 9    |
         *  ------------------------------------------------------------------------
         * | Payload Type | length | coding scheme   | vendor ID opcode | vendor ID |
         *  ------------------------------------------------------------------------
        */

        *ptr++= sec_coding;
        *ptr++ = DISC_OP_VENDOR_ID;
        vendor_id = (uint8_t *)idigi_ptr->vendor_id;
        memcpy(ptr, vendor_id, VENDOR_ID_LENGTH);
        ptr += VENDOR_ID_LENGTH;

        packet->length = ptr - data_ptr;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        status = enable_send_packet(idigi_ptr, packet);

        idigi_ptr->layer_state = discovery_send_device_type;
        break;
    }

    case discovery_send_device_type:
    {
        char * device_type;
        uint16_t len, size;

        DEBUG_PRINTF("discovery layer: send device type\n");
        /*
         * packet format:
         *  ---------------------------------------------------------------------
         * |  0 - 1  |  2 - 3 |   4    |     5       |    6 - 7    |  8...       |
         *  ---------------------------------------------------------------------
         * | Payload | length | coding | device type | Device type | Device type |
         * |   Type  |        | scheme |   opcode    |   length    |             |
         *  ---------------------------------------------------------------------
        */

        *ptr++ = sec_coding;
        *ptr++ = DISC_OP_DEVICETYPE;

        device_type = (char *)idigi_ptr->device_type;
        len = strlen(device_type);

        size = sizeof len;
        StoreBE16(ptr, len);
        ptr += size;

        memcpy(ptr, device_type, len);
        ptr += len;

        /* Send the message. */
        packet->length = ptr - data_ptr;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        status = enable_send_packet(idigi_ptr, packet);
        idigi_ptr->layer_state = discovery_facility;
        break;
    }

    case discovery_facility:
    {
        status = discovery_facility_layer(idigi_ptr);

        if (status == idigi_callback_continue)
        {
            idigi_ptr->layer_state = discovery_send_complete;
        }
        break;
    }

    case discovery_send_complete:
        DEBUG_PRINTF("discovery layer: send complete\n");
        /*
         * packet format:
         *  --------------------------------------------
         * |  0 - 1  |  2 - 3 |   4    |      5         |
         *  --------------------------------------------
         * | Payload | length | coding | initialization |
         * |   Type  |        | scheme |   done opcode  |
         *  --------------------------------------------
        */
        *ptr++ = sec_coding;
        *ptr++ = DISC_OP_INITCOMPLETE;
        packet->length = ptr - data_ptr;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        status = enable_send_packet(idigi_ptr, packet);
        set_idigi_state(idigi_ptr, edp_facility_layer);
        break;
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
    idigi_callback_status_t status;
    idigi_facility_packet_t * packet = NULL;
    idigi_facility_t * fac_ptr;
    bool    done_packet = true;


    /* Facility layer is the layer that iDigi has fully established
     * communication with server. It keeps waiting messages from server
     * for each facility:
     * 1. waits message from server
     * 2. parses message and copies message to the facility
     * 2. invokes facility to process message.
     *
     * Once it gets a packet, it parses and passes it to the appropriate facility.
     */
    switch (idigi_ptr->layer_state)
    {
    case facility_receive_message:
        /* wait for a packet */
        status = receive_packet(idigi_ptr, (idigi_packet_t **)&packet);
        if (status == idigi_callback_continue && packet != NULL)
        {
            /*
             * received packet format:
             *  ----------------------------------------------------------
             * |  0 - 1  |  2 - 3 |   4    |     5     |  6 - 7   |  8... |
             *  ----------------------------------------------------------
             * | Payload | length | coding | discovery | facility | Data  |
             * |   Type  |        | scheme |  payload  |          |       |
             *  ----------------------------------------------------------
            */
            if (packet->type == E_MSG_MT2_TYPE_PAYLOAD)
            {
                uint16_t    facility;

                /* currently we don't support any other security protocol */
                ASSERT_GOTO(packet->sec_coding == SECURITY_PROTO_NONE, error);
                /* ignore this packet since it has invalid length */
                ASSERT_GOTO(packet->length > (PKT_OP_DISCOVERY + PKT_OP_FACILITY), error);
                /* ignore this packet since it not payload opcode */
                ASSERT_GOTO(packet->disc_payload == DISC_OP_PAYLOAD, error);

                facility = FROM_BE16(packet->facility);

                DEBUG_PRINTF("idigi_facility_layer: receive data facility = 0x%04x, type = %d, length=%d\n",
                                            facility, packet->type, packet->length);

                /* TODO: Removed this fake RCI response */
                if (facility == E_MSG_FAC_RCI_NUM)
                {
                    status = rci_process_function(idigi_ptr, NULL, packet);
                    goto error;
                }
                packet->facility = facility;
                /* The packet length includes discovery_payload, security_coding + facility number so
                 * must subtract all these length to get the length of facility data
                 */
                packet->length -= (sizeof packet->disc_payload + sizeof packet->sec_coding + sizeof packet->facility);
            }
        }
        break;
    case facility_process_message:
        /* existing packet */
        packet = (idigi_facility_packet_t *)idigi_ptr->receive_packet.data_packet;
        break;
    }

    if (status == idigi_callback_continue && packet != NULL)
    {
        /* search facility and copy the data to the facility
         *
         * Make sure the facility is not processing previous packet.
         */
        idigi_ptr->layer_state = facility_receive_message;

        for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
        {
            if (fac_ptr->facility_num  == packet->facility)
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

error:
    if (done_packet && packet != NULL)
    {
        release_receive_packet(idigi_ptr, (idigi_packet_t *)packet);
    }

    /* invoke facility process */
    for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->packet != NULL)
        {
            status = fac_ptr->process_cb(idigi_ptr, fac_ptr->facility_data, fac_ptr->packet);
            if (status != idigi_callback_busy)
            {
                release_receive_packet(idigi_ptr, (idigi_packet_t *)fac_ptr->packet);
                fac_ptr->packet = NULL;
            }
        }
    }

    return status;
}

