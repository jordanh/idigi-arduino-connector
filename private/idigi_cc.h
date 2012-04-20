/*
 * Copyright (c) 1996-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
*/
#define CC_IPV6_ADDRESS_LENGTH 16
#define CC_IPV4_ADDRESS_LENGTH 4

#define FAC_CC_DISCONNECT           0x00
#define FAC_CC_REDIRECT_TO_SDA      0x03
#define FAC_CC_REDIRECT_REPORT      0x04
#define FAC_CC_CONNECTION_REPORT    0x05
#define FAC_CC_REBOOT               0x06

#define CC_REDIRECT_SERVER_COUNT    2

typedef enum {
    cc_not_redirect,
    cc_redirect_success,
    cc_redirect_error
} cc_redirect_status_t;

typedef enum {
    ethernet_type = 1,
    ppp_over_modem_type
} cc_connection_type_t;

enum {
    cc_state_redirect_report,
    cc_state_connect_report,
    cc_state_redirect_server
};

typedef struct {
    char    origin_url[SERVER_URL_LENGTH];
    size_t  origin_url_length;
    int     state;
    uint16_t report_length;
    uint8_t report_code;
} idigi_cc_data_t;


static idigi_callback_status_t send_redirect_report(idigi_data_t * const idigi_ptr, idigi_cc_data_t * const cc_ptr)
{

enum cc_redirect_report {
    field_define(redirect_report, opcode, uint8_t),
    field_define(redirect_report, code, uint8_t),
    field_define(redirect_report, message_length, uint8_t),
    /* TODO: fix this enum if message_length > 0. For now, message length is always 0 */
    field_define(redirect_report, url_length, uint16_t),
    record_end(redirect_report)
};

    size_t const report_message_length = 0;
    size_t const redirect_report_header_size = record_bytes(redirect_report);

    idigi_callback_status_t status = idigi_callback_abort;
    uint8_t * edp_header;
    uint8_t * redirect_report;
    size_t url_length;
    size_t redirect_length;

    ASSERT(report_message_length == 0);

    idigi_debug("Connection Control: send redirect_report\n");

    /* build and send redirect report
     *  ----------------------------------------------------
     * |   0    |    1   |    2    |   3...  | x1-x2  | ... |
     *  ----------------------------------------------------
     * | opcode | report | message | report  |  url   | url |
     * |        |  code  | length  | message | length |     |
     *  ----------------------------------------------------
     */

    edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_CC_NUM, &redirect_report, &redirect_length);
    if (edp_header == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    message_store_u8(redirect_report, opcode, FAC_CC_REDIRECT_REPORT);
    message_store_u8(redirect_report, code, cc_ptr->report_code);
    message_store_u8(redirect_report, message_length, report_message_length);


    url_length = cc_ptr->origin_url_length;

    if (url_length > 0)
    {
        char const prefix_url[] = URL_PREFIX;
        size_t const prefix_len = sizeof prefix_url -1;
        uint8_t * const redirect_report_url = redirect_report + redirect_report_header_size;

        ASSERT(redirect_length > (redirect_report_header_size + prefix_len + cc_ptr->origin_url_length));

        memcpy(redirect_report_url, prefix_url, prefix_len);
        memcpy((redirect_report_url + prefix_len), cc_ptr->origin_url, cc_ptr->origin_url_length);

        url_length += prefix_len;
    }
    message_store_be16(redirect_report, url_length, url_length);

    status = initiate_send_facility_packet(idigi_ptr, edp_header,
                                           redirect_report_header_size + url_length,
                                           E_MSG_FAC_CC_NUM, release_packet_buffer, NULL);
    ASSERT(status == idigi_callback_continue);
done:
    return status;
}

static idigi_callback_status_t get_ip_addr(idigi_data_t * const idigi_ptr, uint8_t * ipv6_addr)
{
#define CC_ZERO_IP_ADDR         0x00000000
#define CC_BOARDCAST_IP_ADDR    0xFFFFFFFF

    idigi_callback_status_t status;

    uint8_t * ip_addr = NULL;
    idigi_request_t const request_id = {idigi_config_ip_addr};
    size_t length;

  /* Get IP address */
    status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &ip_addr, &length);
    if (status != idigi_callback_continue)
    {
        idigi_ptr->error_code = idigi_configuration_error;
        status = idigi_callback_abort;
        goto done;
    }
    if (ip_addr == NULL)
    {
        idigi_ptr->error_code = idigi_invalid_data;
        goto error;
    }
    if ((length != CC_IPV6_ADDRESS_LENGTH) && (length != CC_IPV4_ADDRESS_LENGTH))
    {
        idigi_ptr->error_code = idigi_invalid_data_size;
        goto error;
    }

    idigi_debug_hexvalue("get_ip_addr: Device IP address", ip_addr, length);

    if (length == CC_IPV6_ADDRESS_LENGTH)
    {
        /* Good IPv6 address */
        memcpy(ipv6_addr, ip_addr, CC_IPV6_ADDRESS_LENGTH);
        goto done;
    }
    else
    {
        /* Callback returns IPv4 address.
         * Convert IPv4 to IPv6.
         */
        if (*((uint32_t *)ip_addr) == CC_ZERO_IP_ADDR || *((uint32_t *)ip_addr) == CC_BOARDCAST_IP_ADDR)
        {
            /* bad addr */
            idigi_ptr->error_code = idigi_invalid_data;
            goto error;
        }
        else
        {
            /* good ipv4 addr. Map ipv4 to ipv6 address:
             * 10 all-zeros octets, 2 all-ones octets, and
             * then the ipv4 addr
             */

            static unsigned char const ipv6_padding_for_ipv4[] = {0,0,0,0,0,0,0,0,0,0,0xff,0xff};
            size_t const padding_length = sizeof ipv6_padding_for_ipv4/sizeof ipv6_padding_for_ipv4[0];

            ASSERT(padding_length == (CC_IPV6_ADDRESS_LENGTH - CC_IPV4_ADDRESS_LENGTH));

            memcpy(ipv6_addr, ipv6_padding_for_ipv4, padding_length);
            ipv6_addr += padding_length;
            memcpy(ipv6_addr, ip_addr, CC_IPV4_ADDRESS_LENGTH);
            goto done;
        }
    }


error:
    notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
    /* error occurs so let's abort */
    status =idigi_callback_abort;

done:
    return status;

}
static idigi_callback_status_t get_connection_type(idigi_data_t * const idigi_ptr, uint8_t * const connection_type)
{
#if defined(IDIGI_CONNECTION_TYPE)

    UNUSED_PARAMETER(idigi_ptr);

    switch (IDIGI_CONNECTION_TYPE)
    {
    case idigi_lan_connection_type:
        *connection_type = ethernet_type;
        break;
    case idigi_wan_connection_type:
        *connection_type = ppp_over_modem_type;
        break;
    }
    idigi_debug("get_connection_type: connection type = %d\n", IDIGI_CONNECTION_TYPE);
    return idigi_callback_continue;

#else
    idigi_callback_status_t status;
    idigi_request_t const request_id = {idigi_config_connection_type};
    idigi_status_t error_code = idigi_success;
    idigi_connection_type_t * type = NULL;

    /* callback for connection type */
    status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &type, NULL);
    switch (status)
    {
    case idigi_callback_continue:
        if (type == NULL)
        {
            error_code = idigi_invalid_data;
            break;
        }
        switch (*type)
        {
        case idigi_lan_connection_type:
            *connection_type = ethernet_type;
            break;
        case idigi_wan_connection_type:
            *connection_type = ppp_over_modem_type;
            break;
        default:
            error_code = idigi_invalid_data;
            break;
        }
        idigi_debug("get_connection_type: connection type = %d\n", *type);
        break;
    case idigi_callback_abort:
    case idigi_callback_unrecognized:
        idigi_ptr->error_code = idigi_configuration_error;
        status = idigi_callback_abort;
        break;
    case idigi_callback_busy:
        break;
    }

    if (error_code != idigi_success)
    {
        idigi_ptr->error_code = error_code;
        notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
        status = idigi_callback_abort;
    }
    return status;
#endif
}

static idigi_callback_status_t get_mac_addr(idigi_data_t * const idigi_ptr, uint8_t * const mac_addr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_request_t const request_id = {idigi_config_mac_addr};
    size_t length;
    uint8_t * mac;

    /* callback for MAC addr for LAN connection type */
    status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &mac, &length);

    switch (status)
    {
    case idigi_callback_continue:
        ASSERT(mac != NULL);
        if (length != MAC_ADDR_LENGTH)
        {
            /* bad connection type */
            idigi_ptr->error_code = idigi_invalid_data_size;
            notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
            status = idigi_callback_abort;
        }
        else
        {
            idigi_debug_hexvalue("get_mac_addr: MAC address", mac, MAC_ADDR_LENGTH);
            memcpy(mac_addr, mac, MAC_ADDR_LENGTH);
        }
        break;
    case idigi_callback_abort:
    case idigi_callback_unrecognized:
        idigi_ptr->error_code = idigi_configuration_error;
        status = idigi_callback_abort;
        break;
    case idigi_callback_busy:
        break;
    }

    return status;
}

static idigi_callback_status_t send_connection_report(idigi_data_t * const idigi_ptr, idigi_cc_data_t * const cc_ptr)
{
#define FAC_CC_CLIENTTYPE_REBOOTABLE_DEVICE 2

enum cc_connection_report {
    field_define(connection_report, opcode, uint8_t),
    field_define(connection_report, client_type, uint8_t),
    field_define(connection_report, ip1, uint32_t),
    field_define(connection_report, ip2, uint32_t),
    field_define(connection_report, ip3, uint32_t),
    field_define(connection_report, ip4, uint32_t),
    field_define(connection_report, connection_type, uint8_t),
    record_end(connection_report)
};

enum cc_connection_info {
    field_define(connection_info, link_speed, uint32_t),
    record_end(connection_info)
};
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * edp_header;
    uint8_t * connection_report;
    size_t avail_length;

    idigi_debug("Connection Control: send connection report\n");

    /* Build Connection report
     *  -------------------------------------------------------
     * |   0    |    1   | 2  - 17 |     18      |  19 ...     |
     *  -------------------------------------------------------
     * | opcode | client |   IP    | connection  | connection  |
     * |        |  type  | address |    type     | information |
     *  -------------------------------------------------------
     *
     * 1. call callback to get ip address & build ip_address
     * 2. call callback to get & build conenction type
     * 3. if connection type is LAN, call callback to get and build mac address for connection information
     * 4. if connection type is WAN, call callback to get and build link speed for connection information
     * 5. if connection type is WAN, call callback to get and build phone number for connection information
     */
    edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_CC_NUM, &connection_report, &avail_length);
    if (edp_header == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    {
        message_store_u8(connection_report, opcode, FAC_CC_CONNECTION_REPORT);
        message_store_u8(connection_report, client_type, FAC_CC_CLIENTTYPE_REBOOTABLE_DEVICE);
        cc_ptr->report_length = field_named_data(connection_report, opcode, size)+field_named_data(connection_report, client_type, size);

        status = get_ip_addr(idigi_ptr, (connection_report+cc_ptr->report_length));
        if (status != idigi_callback_continue)
        {
            goto error;
        }

        cc_ptr->report_length += CC_IPV6_ADDRESS_LENGTH;

    }

    {
        uint8_t type = ethernet_type;

        status = get_connection_type(idigi_ptr, &type);
        if (status != idigi_callback_continue)
        {
            goto error;
        }

        message_store_u8(connection_report, connection_type, type);
        cc_ptr->report_length += field_named_data(connection_report, connection_type, size);

        if (type == ethernet_type)
        {
            /* let's get MAC address for LAN connection type */
            status = get_mac_addr(idigi_ptr, connection_report + cc_ptr->report_length);
            if (status != idigi_callback_continue)
            {
                goto error;
            }
            cc_ptr->report_length += MAC_ADDR_LENGTH;
        }
        else
        {
            uint8_t * connection_info = connection_report + record_bytes(connection_report);

#if defined(IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND)
            message_store_be32(connection_info, link_speed, IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND);
            idigi_debug("send_connection_report: link_speed = %d\n", IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND);

#else
            /* callback for Link speed for WAN connection type */
            idigi_request_t const request_id = {idigi_config_link_speed};
            /* set for invalid size to cause an error if callback doesn't set it */
            size_t length = sizeof(uint16_t);
            uint32_t * speed = NULL;

            status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &speed, &length);
            if (status != idigi_callback_continue)
            {
                idigi_ptr->error_code =  idigi_configuration_error;
                goto error;
            }

            if (speed == NULL || length != sizeof *speed)
            {
                /* bad connection type */
                idigi_ptr->error_code = idigi_invalid_data_size;
                notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
                status = idigi_callback_abort;
                goto error;
            }
            message_store_be32(connection_info, link_speed, *speed);
            idigi_debug("send_connection_report: link_speed = %d\n", *speed);
#endif
            cc_ptr->report_length += field_named_data(connection_info, link_speed, size);

            {
#if !defined(IDIGI_WAN_PHONE_NUMBER_DIALED)
                /* callback for phone number for WAN connection type */
                idigi_request_t const request_id = {idigi_config_phone_number};
                size_t length = 0;
                uint8_t * phone = NULL;

                status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id, &phone, &length);
                if (status != idigi_callback_continue)
                {
                    idigi_ptr->error_code =  idigi_configuration_error;
                    goto error;
                }

                if (phone == NULL || length == 0)
                {
                    /* bad connection type */
                    idigi_ptr->error_code = idigi_invalid_data_size;
                    notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
                    status = idigi_callback_abort;
                    goto error;
                }
#else
                uint8_t const phone[] = IDIGI_WAN_PHONE_NUMBER_DIALED;
                size_t const length = sizeof phone -1;
#endif
                idigi_debug("send_connection_report: phone number = %.*s\n", length, phone);
                memcpy(connection_report+cc_ptr->report_length, phone, length);
                cc_ptr->report_length += length;
            }
        }
    }

error:
    if (status != idigi_callback_continue)
    {
        ASSERT(status == idigi_callback_abort);
        status = idigi_callback_abort;
        release_packet_buffer(idigi_ptr, edp_header, idigi_configuration_error, NULL);
        goto done;
    }

    ASSERT(avail_length > cc_ptr->report_length);
    status = initiate_send_facility_packet(idigi_ptr, edp_header, cc_ptr->report_length, E_MSG_FAC_CC_NUM, release_packet_buffer, NULL);
    ASSERT(status == idigi_callback_continue);

done:
    return status;
}

static idigi_callback_status_t process_connection_control(idigi_data_t * const idigi_ptr, idigi_cc_data_t * const cc_ptr, idigi_network_request_t const request)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(cc_ptr);

    /* server either disconnects or reboots us */
    idigi_debug("process_connection_control: Connection request %d\n", request);

    {
        idigi_request_t request_id;
        request_id.network_request = request;
        status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_network, request_id, NULL, 0);
    }

    if (status == idigi_callback_continue)
    {
        idigi_ptr->network_busy = idigi_true;
        status = close_server(idigi_ptr);
    }
    return status;
}


static idigi_callback_status_t  process_redirect(idigi_data_t * const idigi_ptr, idigi_cc_data_t * const cc_ptr, uint8_t const * const packet)
{
enum cc_redirect {
    field_define(redirect, opcode, uint8_t),
    field_define(redirect, count, uint8_t),
    record_end(redirect)
};
enum cc_redirect_url {
    field_define(redirect, url_length, uint16_t),
    record_end(redirect_url_length)
};


    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t url_count;
    uint8_t * redirect;
    idigi_bool_t redirect_done = idigi_false;
    size_t const prefix_len = sizeof URL_PREFIX -1;
    uint8_t i;

    idigi_debug("process_redirect:  redirect to new destination\n");
    /* Redirect to new destination:
     * iDigi will close connection and connect to new destination. If connect
     * to new destination fails, this function will returns SUCCESS to try
     * connecting to the original server.
     */

    /*
     * parse new destinations
     *  --------------------------------------------------------------------
     * |   0    |     1     |    2 - 3    |  4 ... |              |         |
     *  --------------------------------------------------------------------
     * | opcode | URL count | URL 1 Length|  URL 1 | URL 2 Length |  URL 2  |
     *  --------------------------------------------------------------------
     *
    */
    redirect = GET_PACKET_DATA_POINTER(packet, PACKET_EDP_FACILITY_SIZE);
    url_count = message_load_u8(redirect, count);

    if (url_count == 0)
    {   /* nothing to redirect */
        idigi_debug("cc_process_redirect: redirect with no url specified\n");
        goto done;
    }
    ASSERT(url_count <= CC_REDIRECT_SERVER_COUNT);
    if (url_count > CC_REDIRECT_SERVER_COUNT)
    {
        url_count = CC_REDIRECT_SERVER_COUNT;
    }


    /* let's start parsing url length and url string */
    redirect += record_bytes(redirect);

    idigi_ptr->network_busy = idigi_true;

    /* Close the connection before parsing new destination url */
    status = close_server(idigi_ptr);
    if (status != idigi_callback_continue)
    {
        goto done;
    }

    /* save original server url that we connected before */
    memcpy(cc_ptr->origin_url, idigi_ptr->server_url, idigi_ptr->server_url_length);
    cc_ptr->origin_url_length = idigi_ptr->server_url_length;

    /* let parse url length and url string */
    for (i = 0; i < url_count && !redirect_done; i++)
    {
        uint16_t const url_length = message_load_be16(redirect, url_length);
        redirect += record_bytes(redirect_url_length);

        {
            char const * server_url = (char const *)redirect;
            uint16_t server_url_length = url_length;

            if (memcmp(server_url, URL_PREFIX, prefix_len) == 0)
            {
                server_url += prefix_len;
                server_url_length -= prefix_len;
            }
            ASSERT(server_url_length < sizeof idigi_ptr->server_url);
            memcpy(idigi_ptr->server_url, server_url, server_url_length);
            idigi_ptr->server_url[server_url_length] = 0x0;
            idigi_ptr->server_url_length = server_url_length;

            status = connect_server(idigi_ptr, idigi_ptr->server_url, server_url_length);
            if (status == idigi_callback_continue)
            {
                cc_ptr->report_code = cc_redirect_success;
                redirect_done = idigi_true;
            }
            else
            {
                /* restore origin server url */
                memcpy(idigi_ptr->server_url, cc_ptr->origin_url, cc_ptr->origin_url_length);
                idigi_ptr->server_url[cc_ptr->origin_url_length] = 0x0;
                idigi_ptr->server_url_length = cc_ptr->origin_url_length;
                if (status == idigi_callback_busy)
                {
                    redirect_done = idigi_true;
                }
                else
                {
                    /* this is not error and we're
                     * going to connect to the origin server.
                     */
                    cc_ptr->report_code = cc_redirect_error;
                    status = idigi_callback_continue;
                }
            }
        }
        redirect += url_length;
    }

    if (status != idigi_callback_busy)
    {
        /* Reset iDigi to inital state to establish communication with the server.
         *
         * Even if connect fails, we want to continue to
         * orginal server.
         */
        cc_ptr->state = cc_state_redirect_report;
        set_idigi_state(idigi_ptr, edp_communication_layer);
        idigi_ptr->error_code = idigi_success;
        status = idigi_callback_continue;
    }
done:
    return status;
}

static idigi_callback_status_t cc_process(idigi_data_t * const idigi_ptr, void * const facility_data,
                                          uint8_t * const packet, unsigned int * const receive_timeout)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(receive_timeout);

    /* process incoming message from server for Connection Control facility */
    if (packet != NULL)
    {
        uint8_t opcode;
        uint8_t * ptr;
        idigi_cc_data_t * cc_ptr = facility_data;

        /* get the DATA portion of the packet */
        ptr = GET_PACKET_DATA_POINTER(packet, PACKET_EDP_FACILITY_SIZE);
        opcode = *ptr;

        switch (opcode)
        {
        case FAC_CC_DISCONNECT:
            status = process_connection_control(idigi_ptr, cc_ptr, idigi_network_disconnected);
            break;
        case FAC_CC_REDIRECT_TO_SDA:
            status = process_redirect(idigi_ptr, cc_ptr, packet);
            break;
        case FAC_CC_REBOOT:
            status = process_connection_control(idigi_ptr, cc_ptr, idigi_network_reboot);
            break;
        default:
            idigi_debug("idigi_cc_process: unsupported opcode %u\n", opcode);
            break;
        }
    }

    return status;
}
static idigi_callback_status_t cc_discovery(idigi_data_t * const idigi_ptr, void * const facility_data,
                                            uint8_t * const packet, unsigned int * receive_timeout)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_cc_data_t * cc_ptr = facility_data;

    UNUSED_PARAMETER(receive_timeout);
    UNUSED_PARAMETER(packet);
    /* Connection control facility needs to send redirect and
     * connection reports on discovery layer.
     */
    if (cc_ptr->state == cc_state_redirect_report)
    {
        status = send_redirect_report(idigi_ptr, cc_ptr);
        if (status == idigi_callback_continue)
        {
            status = idigi_callback_busy;
            cc_ptr->report_code = cc_not_redirect;
            cc_ptr->origin_url_length = 0;
            cc_ptr->state = cc_state_connect_report;
        }
    }
    else if (cc_ptr->state == cc_state_connect_report)
    {
        status = send_connection_report(idigi_ptr, cc_ptr);

    }

    return status;
}

static idigi_callback_status_t idigi_facility_cc_cleanup(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_cc_data_t * const cc_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_CC_NUM);
    if (cc_ptr != NULL)
    {
        cc_ptr->report_code = cc_not_redirect;
        cc_ptr->origin_url_length = 0;
    }
    return status;
}

static idigi_callback_status_t idigi_facility_cc_delete(idigi_data_t * const idigi_ptr)
{
    return del_facility_data(idigi_ptr, E_MSG_FAC_CC_NUM);
}

static idigi_callback_status_t idigi_facility_cc_init(idigi_data_t * const idigi_ptr, unsigned int const facility_index)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_cc_data_t * cc_ptr;

    /* Add Connection control facility to iDigi
     *
     * Make sure connection control is not already created. If Connection
     * control facility is already created, we probably reconnect to server
     * so just need to reset to initial state.
     *
     */
    cc_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_CC_NUM);
    if (cc_ptr == NULL)
    {
        void * ptr;

        status = add_facility_data(idigi_ptr, facility_index, E_MSG_FAC_CC_NUM, &ptr, sizeof *cc_ptr);

        if (status == idigi_callback_abort || ptr == NULL)
        {
            goto done;
        }
        cc_ptr = ptr;

        cc_ptr->report_code = cc_not_redirect;
        cc_ptr->origin_url_length = 0;

    }

    /* restart for sending redirect report */
    cc_ptr->state = cc_state_redirect_report;

done:
    return status;
}

