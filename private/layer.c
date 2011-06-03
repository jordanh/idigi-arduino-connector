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

typedef iik_callback_status_t (* iik_facility_init_cb_t )(struct iik_data * iik_ptr);

typedef struct {
    iik_config_request_t  facility;
    iik_facility_init_cb_t init_cb;
    iik_facility_init_cb_t delete_cb;
} iik_facility_init_t;

/* Table of all facilites that IDK supports.
 *
 * IDK will call callback to see whether it supports each optional facility.
 * IDK will call init_cb to initialize the facility and delete_cb to remove the facility.
 * The init_cb must call add_facility_data() to add the facility into IDK facility list.
 * The delete_cb is called to delete the facility from IDK facility list when user terminates IDK.
 */
static iik_facility_init_t iik_facility_init_cb[] = {
        /* mandatory facilities */
        {(iik_config_request_t)MANDATORY_FACILITY, (iik_facility_init_cb_t)cc_init_facility, (iik_facility_init_cb_t)cc_delete_facility},
        {(iik_config_request_t)MANDATORY_FACILITY, (iik_facility_init_cb_t)loopback_init_facility, (iik_facility_init_cb_t)loopback_delete_facility},

        /* list of optional facilities */
        {iik_config_firmware_facility, (iik_facility_init_cb_t)fw_init_facility, (iik_facility_init_cb_t)fw_delete_facility}
};

static size_t iik_facility_count = asizeof(iik_facility_init_cb);

static iik_callback_status_t  remove_facility_layer(iik_data_t * iik_ptr)
{
    iik_callback_status_t status = iik_callback_continue, rc = iik_callback_continue;

    while (iik_ptr->request_id < (int)iik_facility_count)
    {
        if ((iik_ptr->facilities & (0x1 << iik_ptr->request_id)) &&
             iik_facility_init_cb[iik_ptr->request_id].delete_cb != NULL)
        {
            status = iik_facility_init_cb[iik_ptr->request_id].delete_cb(iik_ptr);
            if (status == iik_callback_continue)
            {
                iik_ptr->request_id++;
            }
            else
            {
                rc = iik_callback_abort;
            }
        }
        else
        {
            iik_ptr->request_id++;
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

static iik_callback_status_t discovery_facility_layer(iik_data_t * iik_ptr)
{
    iik_callback_status_t status = iik_callback_continue;
    iik_facility_t * fac_ptr;

    /* invoke any facility that needs to send any message to server
     * during initialization phase at discovery layer.
     */
    fac_ptr = iik_ptr->active_facility;
    if (fac_ptr == NULL)
    {
        fac_ptr = iik_ptr->facility_list;
    }

    while (fac_ptr != NULL && status == iik_callback_continue)
    {
        DEBUG_PRINTF("Discovery Facility layer: 0x%x\n", fac_ptr->facility_num);
        if (fac_ptr->discovery_cb == NULL)
        {
            status = iik_callback_continue;
        }
        else
        {   /* function to send facility discovery */
            status = fac_ptr->discovery_cb(iik_ptr, fac_ptr);
        }

        if (status == iik_callback_continue)
        {
            fac_ptr = iik_ptr->active_facility = fac_ptr->next;
        }
    }
    return status;
}

static iik_callback_status_t get_configurations(iik_data_t * iik_ptr)
{
#define INIT_CONFIG_ID_COUNT    5
    iik_callback_status_t status = iik_callback_continue;
    void * data = NULL;
    size_t length;
    iik_request_t request_id;

    unsigned iik_edp_init_config_ids[INIT_CONFIG_ID_COUNT] = {
            iik_config_server_url, iik_config_rx_keepalive, iik_config_tx_keepalive, iik_config_wait_count, iik_config_password,
    };

   /* Call callback to get server url, wait count, tx keepalive, rx keepalive, & password.
    * Call error status callback if error is encountered (NULL data, invalid range, invalid size).
    */
    while(iik_ptr->request_id < INIT_CONFIG_ID_COUNT)
    {
        request_id.config_request = iik_edp_init_config_ids[iik_ptr->request_id];

        status = iik_callback(iik_ptr->callback, iik_class_config, request_id, NULL, 0, &data, &length);
        if (status == iik_callback_continue)
        {
            if (request_id.config_request != iik_config_password && data == NULL)
            {
                /* callback cannot return NULL except iik_config_password */
                iik_ptr->error_code = iik_invalid_data;
                goto error;
            }
            else
            {
                switch(request_id.config_request)
                {
                case iik_config_password:
                    iik_ptr->password = (char *)data;
                   break;
                case iik_config_server_url:
                    iik_ptr->server_url = (char *)data;
                    if (length == 0 || length > SERVER_URL_LENGTH)
                    {
                        iik_ptr->error_code = iik_invalid_data_range;
                        goto error;
                    }
                    break;
                case iik_config_tx_keepalive:
                    iik_ptr->tx_keepalive = (uint16_t *)data;

                    if (*iik_ptr->tx_keepalive < MIN_TX_KEEPALIVE_INTERVAL_PER_SECOND || *iik_ptr->tx_keepalive > MAX_TX_KEEPALIVE_INTERVAL_PER_SECOND ||
                        length != sizeof(uint16_t))
                    {
                        iik_ptr->error_code = iik_invalid_data_range;
                        goto error;
                    }
                    break;
                case iik_config_rx_keepalive:
                    iik_ptr->rx_keepalive = (uint16_t *)data;
                    if (*iik_ptr->rx_keepalive < MIN_RX_KEEPALIVE_INTERVAL_PER_SECOND || *iik_ptr->rx_keepalive > MAX_RX_KEEPALIVE_INTERVAL_PER_SECOND ||
                        length != sizeof(uint16_t))
                    {
                        iik_ptr->error_code = iik_invalid_data_range;
                        goto error;
                    }
                    break;
                case iik_config_wait_count:
                    iik_ptr->wait_count = (uint8_t *)data;
                    if (*iik_ptr->wait_count < WAIT_COUNT_MIN || *iik_ptr->wait_count > WAIT_COUNT_MAX ||
                        length != sizeof(uint8_t))
                    {
                        iik_ptr->error_code = iik_invalid_data_range;
                        goto error;
                        
                    }
                    break;
                case iik_config_device_id:
                case iik_config_vendor_id:
                case iik_config_device_type:
                    /* got these configurations in init function */
                    break;
                case iik_config_connection_type:
                case iik_config_mac_addr:
                case iik_config_link_speed:
                case iik_config_phone_number:
                case iik_config_ip_addr:
                case iik_config_error_status:
                case iik_config_disconnected:
                case iik_config_firmware_facility:
                    /* get these configurations from different modules */
                    break;
                }
                iik_ptr->request_id++;
            }
        }
        else
        {
            if (status == iik_callback_abort && iik_ptr->error_code == iik_success)
            {
                iik_ptr->error_code = iik_configuration_error;
            }
            goto done;
        }
    } /* while */


error:
    if (iik_ptr->error_code != iik_success)
    {
        status = notify_error_status(iik_ptr->callback, iik_class_config, request_id, iik_ptr->error_code);
    }

    if (status == iik_callback_continue && iik_ptr->request_id != INIT_CONFIG_ID_COUNT)
    {
        status = iik_callback_busy;
    }

done:
    return status;
}

static iik_callback_status_t initialize_facilites(iik_data_t * iik_ptr)
{
    iik_callback_status_t status;
    bool facility_enable;
    int  idx;
    iik_request_t request_id;
    size_t length;

    /* iik_edp_init_facility_ids[] table includes a list of facilities that IDK supports.
     * Call callback to see which facility is supported.
     */
    iik_ptr->facilities = 0;
    while (iik_ptr->request_id < iik_facility_count)
    {
        idx = iik_ptr->request_id;

        request_id.config_request = iik_facility_init_cb[idx].facility;
        if (request_id.config_request != (iik_config_request_t)MANDATORY_FACILITY)
        {   /* this is optional facility so ask application whether it supports this facility */
            status = iik_callback(iik_ptr->callback, iik_class_config, request_id, NULL, 0, &facility_enable, &length);
            if (status != iik_callback_continue)
            {
                DEBUG_PRINTF("initialize_facilities: callback returns %d on facility= %d\n", status, request_id.config_request);
                iik_ptr->error_code = iik_configuration_error;
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
            if (iik_facility_init_cb[idx].init_cb != NULL)
            {
                status = iik_facility_init_cb[idx].init_cb(iik_ptr);
            }
            iik_ptr->facilities |= (0x01 << idx);
        }

        if (status == iik_callback_continue)
        {
            iik_ptr->request_id++;
        }
        else
        {
            goto done;
        }

    }

done:
    return status;
}
static iik_callback_status_t configuration_layer(iik_data_t * iik_ptr)
{
    enum {
        configuration_get_configurations,
        configuration_init_facilities,
    };

    iik_callback_status_t status = iik_callback_continue;

    /* This layer is called to get some of the EDP configuration and initialize facilities */
    if (iik_ptr->layer_state == configuration_get_configurations)
    {
        status = get_configurations(iik_ptr);
        if (status == iik_callback_continue)
        {
            iik_ptr->facilities = 0;
            iik_ptr->request_id = 0;
            iik_ptr->layer_state = configuration_init_facilities;
        }
    }
    if (iik_ptr->layer_state == configuration_init_facilities)
    {
        status = initialize_facilites(iik_ptr);
        if (status == iik_callback_continue)
        {
            set_iik_state(iik_ptr, edp_communication_layer);
        }
    }

    return status;

}

static iik_callback_status_t communication_layer(iik_data_t * iik_ptr)
{
    enum {
        communication_connect_server,
        communication_send_version,
        communication_receive_version_response,
        communication_send_keepalive
    };

    iik_callback_status_t status = iik_callback_continue;
    uint32_t version;
    uint8_t * ptr;
    int len;
    iik_packet_t * packet;

    /* communitcation layer:
     *  1. establishes connection.
     *  2. sends MT version
     *  3. receives and validates MT version response
     *  4. sends tx, rx, & waitcount parameter
     */
    if (iik_ptr->layer_state == communication_connect_server)
    {
        if (iik_ptr->network_handle == NULL)
        {
            status = connect_server(iik_ptr, iik_ptr->server_url, EDP_MT_PORT);
        }

        if (status == iik_callback_continue)
        {
            iik_ptr->request_id = 0;
            iik_ptr->layer_state = communication_send_version;
        }

    }

    if (iik_ptr->layer_state == communication_send_version)
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
        packet =(iik_packet_t *) get_packet_buffer(iik_ptr, sizeof(iik_packet_t), &ptr);
        if (packet == NULL)
        {
            goto done;
        }
        packet->type = E_MSG_MT2_TYPE_VERSION;
        packet->length = sizeof version;

        StoreBE32(ptr, EDP_MT_VERSION);

        status = enable_send_packet(iik_ptr, packet);
        if (status == iik_callback_continue)
        {
            iik_ptr->layer_state = communication_receive_version_response;
        }

    }


    else if (iik_ptr->layer_state == communication_receive_version_response)
    {
        status = receive_packet(iik_ptr, &packet);

        if (status == iik_callback_continue && packet != NULL)
        {
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

            ptr = GET_PACKET_DATA_POINTER(packet, sizeof(iik_packet_t));

            if (packet->type != E_MSG_MT2_TYPE_VERSION_OK)
            {
                iik_request_t request_id;
                /*
                 * The received message is not acceptable. Return an error value
                 * appropriate to the message received.
                 */
                status = iik_callback_abort;
                switch (packet->type)
                {
                    /* Expected MTv2 message types... */
                    case E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP:
                        if (*ptr == SERVER_OVERLOAD_RESPONSE) {
                            iik_ptr->error_code = iik_server_overload;
                        }
                        else {
                            iik_ptr->error_code = iik_bad_version;
                        }
                        break;
                    case E_MSG_MT2_TYPE_VERSION_BAD:
                        iik_ptr->error_code = iik_bad_version;
                        break;
                    case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
                        iik_ptr->error_code = iik_server_overload;
                        break;
                    /* Unexpected/unknown MTv2 message types... */
                    default:
                        iik_ptr->error_code = iik_invalid_packet;
                }
                /* mt version error. let's notify user.
                 *
                 * ignore error status callback return value since server
                 * will close the connection.
                 */
                request_id.network_request = iik_network_receive;
                notify_error_status(iik_ptr->callback, iik_class_network, request_id, iik_ptr->error_code);
                goto done;
            }
            else
            {   /* advance to send keepalive parameters */
                iik_ptr->layer_state = communication_send_keepalive;
            }
        }
    }

    else if (iik_ptr->layer_state == communication_send_keepalive)
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


        ptr = iik_ptr->send_packet.ptr = iik_ptr->send_packet.buffer;

        timeout = *iik_ptr->rx_keepalive;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_RX_INTERVAL, timeout);
        ptr += len;
        iik_ptr->send_packet.total_length = len;

        timeout = *iik_ptr->tx_keepalive;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_TX_INTERVAL, timeout);
        ptr += len;
        iik_ptr->send_packet.total_length += len;

        wait_count = *iik_ptr->wait_count;
        len = msg_add_keepalive_param(ptr, E_MSG_MT2_TYPE_KA_WAIT, (uint16_t)wait_count);
        ptr += len;

        /* This will enable send_packet.
         * Set total_length to be sent and clear length to 0
         * for actual length that has been sent.
         */
        iik_ptr->send_packet.total_length += len;
        iik_ptr->send_packet.length = 0;

        set_iik_state(iik_ptr, edp_initialization_layer);

    }

done:
    return status;
}

static iik_callback_status_t initialization_layer(iik_data_t * iik_ptr)
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

    iik_callback_status_t status = iik_callback_continue;
    iik_packet_t * packet;
    uint32_t version;
    uint8_t * ptr;

    /* initialization layer:
     * 1. sends protocol version.
     * 2. receives and validates protocol version response
     */
    switch (iik_ptr->layer_state)
    {
    case initialization_send_protocol_version:
        DEBUG_PRINTF("initialization layer: send protocol version\n");
        /*
         *  version packet format:
         *  ---------------------------------
         * |    0 - 1     |  2 - 3 |  4 - 7  |
         *  ----------------------------------
         * | Payload Type | length | version |
         *  ----------------------------------
        */

        packet =(iik_packet_t *) get_packet_buffer(iik_ptr, sizeof(iik_packet_t), &ptr);
        if (packet == NULL)
        {
            goto done;
        }
        packet->length = sizeof(version);
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        version = EDP_PROTOCOL_VERSION;
        StoreBE32(ptr, version);

        status = enable_send_packet(iik_ptr, packet);
        iik_ptr->layer_state = initialization_receive_protocol_version;
        break;

    case initialization_receive_protocol_version:
        status = receive_packet(iik_ptr, &packet);
        if (status == iik_callback_continue && packet != NULL)
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
            if (packet->length < 1)
            {
                goto done;
            }

            /* Parse the version response (0 = version response ok).
             * If the protocol version number was not acceptable to the server,
             * tell the application.
             */
            ptr = GET_PACKET_DATA_POINTER(packet, sizeof(iik_packet_t));
            if (*ptr != initialization_version_response_acceptable)
            {
                iik_request_t request_id;
                status = iik_callback_abort;
                request_id.network_request = iik_network_receive;
                iik_ptr->error_code = iik_bad_version;
                notify_error_status(iik_ptr->callback, iik_class_network, request_id, iik_ptr->error_code);
                goto done;

            }
            set_iik_state(iik_ptr, edp_security_layer);
        }
        break;
    }

done:

    return status;
}

static iik_callback_status_t security_layer(iik_data_t * iik_ptr)
{
    enum {
            security_send_identity_verification,
            security_send_device_id,
            security_send_server_url,
            security_send_password,
    };
    iik_callback_status_t status = iik_callback_continue;
    iik_packet_t * packet;
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
    packet =(iik_packet_t *) get_packet_buffer(iik_ptr, sizeof(iik_packet_t), &ptr);
    if (packet == NULL)
    {
        goto done;
    }
    data_ptr = ptr;

    switch (iik_ptr->layer_state)
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

        iik_ptr->security_form = *ptr++ ;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;
        packet->length = ptr - data_ptr;

        status = enable_send_packet(iik_ptr, packet);

        iik_ptr->layer_state = security_send_device_id;
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

        device_id = (uint8_t *)iik_ptr->device_id;

        memcpy(ptr, device_id, DEVICE_ID_LENGTH);
        packet->length += DEVICE_ID_LENGTH;

        status = enable_send_packet(iik_ptr, packet);
        iik_ptr->layer_state = security_send_server_url;
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

        server_url = (char *)iik_ptr->server_url;

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

        status = enable_send_packet(iik_ptr, packet);
        if (iik_ptr->security_form == SECURITY_IDENT_FORM_PASSWORD)
        {
            iik_ptr->layer_state = security_send_password;
        }
        else
        {
            set_iik_state(iik_ptr, edp_discovery_layer);
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
        pwd = (char *)iik_ptr->password;

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
        status = enable_send_packet(iik_ptr, packet);
        set_iik_state(iik_ptr, edp_discovery_layer);
        break;
    }
done:
    return status;
}
static iik_callback_status_t discovery_layer(iik_data_t * iik_ptr)
{
    enum {
        discovery_send_vendor_id,
        discovery_send_device_type,
        discovery_facility,
        discovery_send_complete
    };
    iik_callback_status_t status = iik_callback_continue;
    iik_packet_t * packet;
    uint8_t sec_coding = SECURITY_PROTO_NONE;
    uint8_t * ptr, * data_ptr;

    /* discovery layer:
     * 1. send vendor ID
     * 2. send device type
     * 3. call each facility to send its own discovery layer
     * 4. send discovery complete message.
    */
    packet =(iik_packet_t *) get_packet_buffer(iik_ptr, sizeof(iik_packet_t), &ptr);
    if (packet == NULL)
    {
        goto done;
    }
    data_ptr = ptr;

    switch (iik_ptr->layer_state)
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
        vendor_id = (uint8_t *)iik_ptr->vendor_id;
        memcpy(ptr, vendor_id, VENDOR_ID_LENGTH);
        ptr += VENDOR_ID_LENGTH;

        packet->length = ptr - data_ptr;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        status = enable_send_packet(iik_ptr, packet);

        iik_ptr->layer_state = discovery_send_device_type;
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

        device_type = (char *)iik_ptr->device_type;
        len = strlen(device_type);

        size = sizeof len;
        StoreBE16(ptr, len);
        ptr += size;

        memcpy(ptr, device_type, len);
        ptr += len;

        /* Send the message. */
        packet->length = ptr - data_ptr;
        packet->type = E_MSG_MT2_TYPE_PAYLOAD;

        status = enable_send_packet(iik_ptr, packet);
        iik_ptr->layer_state = discovery_facility;
        break;
    }

    case discovery_facility:
    {
        status = discovery_facility_layer(iik_ptr);

        if (status == iik_callback_continue)
        {
            iik_ptr->layer_state = discovery_send_complete;
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

        status = enable_send_packet(iik_ptr, packet);
        set_iik_state(iik_ptr, edp_facility_layer);
        break;
    }
done:
    return status;
}

static iik_callback_status_t facility_layer(iik_data_t * iik_ptr)
{
    enum {
        facility_receive_message,
        facility_process_message
    };
    iik_callback_status_t status;
    iik_facility_packet_t * packet = NULL;
    iik_facility_t * fac_ptr;
    uint16_t    facility;


    /* Facility layer is the layer that IDK has fully established
     * communication with server. It keeps waiting messages from server
     * for each facility:
     * 1. waits message from server
     * 2. parses message and copies message to the facility
     * 2. invokes facility to process message.
     *
     * Once it gets a packet, it parses and passes it to the appropriate facility.
     */
    if (iik_ptr->layer_state == facility_receive_message)
    {
        /* wait for a packet */
        status = receive_packet(iik_ptr, (iik_packet_t **)&packet);
    }
    else if (iik_ptr->layer_state == facility_process_message)
    {
        /* existing packet */
        packet = (iik_facility_packet_t *)iik_ptr->receive_packet.data_packet;
    }

    if (packet != NULL)
    {
        unsigned    length;

        iik_ptr->layer_state = facility_receive_message;

        length = packet->length;

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

            /* currently we don't support any other security protocol */
            ASSERT_GOTO(packet->sec_coding == SECURITY_PROTO_NONE, done);
            /* ignore this packet since it has invalid length */
            ASSERT_GOTO(length > (PKT_OP_DISCOVERY + PKT_OP_FACILITY), done);
            /* ignore this packet since it not payload opcode */
            ASSERT_GOTO(packet->disc_payload == DISC_OP_PAYLOAD, done);


            facility = FROM_BE16(packet->facility);

            DEBUG_PRINTF("iik_facility_layer: receive data facility = 0x%04x, type = %d, length=%d\n",
                                        facility, packet->type, length);

            /* TODO: Removed this fake RCI response */
            if (facility == E_MSG_FAC_RCI_NUM)
            {
                status = rci_process_function(iik_ptr, NULL, packet);
            }

            /* search facility and copy the data to the facility
             *
             * Make sure the facility is not processing previous packet.
             */
            for (fac_ptr = iik_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
            {
                if (fac_ptr->facility_num  == facility)
                {
                    if (fac_ptr->packet == NULL)
                    {
                        uint8_t * src_ptr, * dst_ptr;

                        fac_ptr->packet = (iik_facility_packet_t *)fac_ptr->buffer;
                        fac_ptr->packet->disc_payload= packet->disc_payload;
                        fac_ptr->packet->sec_coding = packet->sec_coding;
                        fac_ptr->packet->facility = facility;
                        /* The packet length includes discovery_payload, security_coding + facility number so
                         * must subtract all these length to get the length of facility data
                         */
                        fac_ptr->packet->length = packet->length - sizeof packet->disc_payload - sizeof packet->sec_coding - sizeof packet->facility;
                        iik_ptr->active_facility = fac_ptr;

                        src_ptr = GET_PACKET_DATA_POINTER(packet, sizeof(iik_facility_packet_t));
                        dst_ptr = GET_PACKET_DATA_POINTER(fac_ptr->packet, sizeof(iik_facility_packet_t));
                        memcpy(dst_ptr, src_ptr, fac_ptr->packet->length);
                    }
                    else
                    { /* Facility is busy so hold on the packet and stop receiving data */
                        iik_ptr->layer_state = facility_process_message;
                    }
                    break;
                }
            }
        }

    }

    /* invoke facility process */
    for (fac_ptr = iik_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if ( fac_ptr->packet != NULL)
        {
            status = fac_ptr->process_cb(iik_ptr, fac_ptr);
             if (status == iik_callback_abort)
            {
                /* error */
                DEBUG_PRINTF("iik_facility_layer: facility  (0x%x) returns abort with error code %d\n", fac_ptr->facility_num, iik_ptr->error_code);
                goto done;
            }
        }
    }

done:
    return status;
}

