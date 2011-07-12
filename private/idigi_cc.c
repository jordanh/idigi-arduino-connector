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
#include "ei_msg.h"

#define CC_IPV6_ADDRESS_LENGTH 16
#define CC_IPV4_ADDRESS_LENGTH 4

#define CC_ZERO_IP_ADDR         0x00000000
#define CC_BOARDCAST_IP_ADDR    0xFFFFFFFF

typedef enum {
    cc_not_redirect,
    cc_redirect_success,
    cc_redirect_error
} cc_redirect_status_t;

typedef enum {
    ethernet_type = 1,
    ppp_over_modem_type
} cc_connection_type_t;

#define FAC_CC_DISCONNECT           0x00
#define FAC_CC_REDIRECT_TO_SDA      0x03
#define FAC_CC_REDIRECT_REPORT      0x04
#define FAC_CC_CONNECTION_REPORT    0x05
#define FAC_CC_REBOOT               0x06

#define FAC_CC_CLIENTTYPE_DEVICE    1
#define FAC_CC_CLIENTTYPE_REBOOTABLE_DEVICE 2

#define CC_REDIRECT_SERVER_COUNT    2

enum {
    cc_state_redirect_report,
    cc_state_connect_report,
    cc_state_redirect_server,
};

typedef struct {
    char    server_url[CC_REDIRECT_SERVER_COUNT][SERVER_URL_LENGTH];
    char    origin_url[SERVER_URL_LENGTH];
    uint8_t report_code;
    int     state;
    unsigned item;

} idigi_cc_data_t;

#define  idigi_config_connection_control idigi_config_connection_type

static idigi_callback_status_t send_redirect_report(idigi_data_t * idigi_ptr, idigi_cc_data_t * cc_ptr)
{
    idigi_callback_status_t status;
    idigi_packet_t * packet;
    uint8_t             report_msg_length = 0;
    uint16_t            url_length;
    uint8_t             * ptr, * start_ptr;

    DEBUG_PRINTF("Connection Control: send redirect_report\n");

    /* build and send redirect report
     *  ----------------------------------------------------
     * |   0    |    1   |    2    |   3...  | x1-x2  | ... |
     *  ----------------------------------------------------
     * | opcode | report | message | report  |  url   | url |
     * |        |  code  | length  | message | length |     |
     *  ----------------------------------------------------
     */

    packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_CC_NUM, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }
    start_ptr = ptr;

    *ptr++ = FAC_CC_REDIRECT_REPORT;  /* opcode */
    *ptr++ = cc_ptr->report_code;    /* report code */

    /* report message length */
    *ptr++ = report_msg_length;
    ptr += report_msg_length;

    url_length = strlen(cc_ptr->origin_url);
    StoreBE16(ptr, (url_length));
    ptr += sizeof(url_length);

    if (url_length > 0)
    {   /* URL */
        memcpy(ptr, cc_ptr->origin_url, url_length);
        ptr += url_length;
    }

    packet->header.length = ptr - start_ptr;
    cc_ptr->item = idigi_config_ip_addr;

    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_CC_NUM, release_packet_buffer, NULL);
done:
    return status;
}
static idigi_callback_status_t get_ip_addr(idigi_data_t * idigi_ptr, uint8_t * ipv6_addr)
{
    idigi_callback_status_t status;

    uint8_t * ip_addr = NULL;
    idigi_request_t request_id;
    size_t length;

  /* Get IP address */
    request_id.config_request = idigi_config_ip_addr;
    status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &ip_addr, &length);
    if (status != idigi_callback_continue)
    {
        idigi_ptr->error_code = idigi_configuration_error;
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

            unsigned char ipv6_padding_for_ipv4[] = {0,0,0,0,0,0,0,0,0,0,0xff,0xff};
            size_t padding_length = sizeof ipv6_padding_for_ipv4/sizeof ipv6_padding_for_ipv4[0];

            ASSERT(padding_length == (CC_IPV6_ADDRESS_LENGTH - CC_IPV4_ADDRESS_LENGTH));

            memcpy(ipv6_addr, ipv6_padding_for_ipv4, padding_length);
            ipv6_addr += padding_length;
            memcpy(ipv6_addr, ip_addr, CC_IPV4_ADDRESS_LENGTH);
            ipv6_addr += CC_IPV4_ADDRESS_LENGTH;
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
static idigi_callback_status_t get_connection_type(idigi_data_t * idigi_ptr, uint8_t * connection_type)
{
    idigi_callback_status_t status;

    idigi_request_t request_id;
    idigi_connection_type_t * type;
    idigi_status_t error_code = idigi_success;

    /* callback for connection type */
    request_id.config_request = idigi_config_connection_type;
    status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &type, NULL);
    if (status != idigi_callback_continue)
    {
        idigi_ptr->error_code = idigi_configuration_error;
        goto done;
    }

    if (type == NULL)
    {
        /* bad connection type */
        error_code = idigi_invalid_data;
    }
    else
    {
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

   }

    if (error_code != idigi_success)
    {
        idigi_ptr->error_code = error_code;
        notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
        status = idigi_callback_abort;
    }

done:
    return status;
}

static idigi_callback_status_t get_mac_addr(idigi_data_t * idigi_ptr, uint8_t * mac_addr)
{
    idigi_callback_status_t status = idigi_callback_continue;;
    idigi_request_t request_id;
    size_t length;

    uint8_t * mac;

    /* callback for MAC addr for LAN connection type */
    request_id.config_request = idigi_config_mac_addr;
    status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &mac, &length);
    if (status != idigi_callback_continue)
    {
        idigi_ptr->error_code = idigi_configuration_error;
    }
    else if (mac == NULL || length != MAC_ADDR_LENGTH)
    {
        /* bad connection type */
        idigi_ptr->error_code = idigi_invalid_data_size;
        notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
        status = idigi_callback_abort;
    }
    else
    {
        memcpy(mac_addr, mac, MAC_ADDR_LENGTH);
    }

    return status;
}

static idigi_callback_status_t send_connection_report(idigi_data_t * idigi_ptr, idigi_cc_data_t * cc_ptr)
{

    idigi_callback_status_t status = idigi_callback_continue;;
    idigi_packet_t * packet;
    uint8_t * ptr;


    DEBUG_PRINTF("Connection Control: send connection report\n");

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
    packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_CC_NUM, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    if (cc_ptr->item == idigi_config_ip_addr)
    {
        *ptr++ = FAC_CC_CONNECTION_REPORT;  /* opcode */
        *ptr++ = FAC_CC_CLIENTTYPE_REBOOTABLE_DEVICE;
        packet->header.length = 2;

        status = get_ip_addr(idigi_ptr, ptr);
        if (status != idigi_callback_continue)
        {
            goto done;
        }

        cc_ptr->item = idigi_config_connection_type;
        packet->header.length += CC_IPV6_ADDRESS_LENGTH;
    }


    if (cc_ptr->item == idigi_config_connection_type)
    {
        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
        ptr += packet->header.length;
        status = get_connection_type(idigi_ptr, ptr);
        if (status != idigi_callback_continue)
        {
            goto done;
        }
        if (*ptr == ethernet_type)
        {
            cc_ptr->item = idigi_config_mac_addr;
        }
        else
        {
            cc_ptr->item = idigi_config_link_speed;
        }
        packet->header.length++;

    }


    if (cc_ptr->item == idigi_config_mac_addr)
    {
        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
        ptr += packet->header.length;
        status = get_mac_addr(idigi_ptr, ptr);
        if (status != idigi_callback_continue)
        {
            goto done;
        }
        packet->header.length += MAC_ADDR_LENGTH;


    }


    if (cc_ptr->item == idigi_config_link_speed)
    {
        /* callback for Link speed for WAN connection type */
        idigi_request_t request_id;
        size_t length;
        uint32_t * link_speed;

        request_id.config_request = idigi_config_link_speed;
        status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &link_speed, &length);
        if (status != idigi_callback_continue)
        {
            idigi_ptr->error_code = idigi_configuration_error;
            goto done;
        }

        if (link_speed == NULL || length != sizeof(*link_speed))
        {
            /* bad connection type */
            idigi_ptr->error_code = idigi_invalid_data_size;
            notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
            status = idigi_callback_abort;
            goto done;
        }

        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
        ptr += packet->header.length;
        StoreBE32(ptr, *link_speed);
        packet->header.length += sizeof(*link_speed);
        cc_ptr->item = idigi_config_phone_number;
    }

    if (cc_ptr->item == idigi_config_phone_number)
    {
        /* callback for phone number for WAN connection type */
        idigi_request_t request_id;
        size_t length;
        uint8_t * phone = NULL;

        request_id.config_request = idigi_config_phone_number;
        status = idigi_callback(idigi_ptr->callback, idigi_class_config, request_id, NULL, 0, &phone, &length);

        if (status != idigi_callback_continue)
        {
            idigi_ptr->error_code = idigi_configuration_error;
            goto done;
        }

        if (phone == NULL || length == 0)
        {
            /* bad connection type */
            idigi_ptr->error_code = idigi_invalid_data_size;
            notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_ptr->error_code);
            status = idigi_callback_abort;
            goto done;
        }

        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
        ptr += packet->header.length;
        memcpy(ptr, phone, length);
        packet->header.length += length;

    }
    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_CC_NUM, release_packet_buffer, NULL);

done:
    return status;
}

static idigi_callback_status_t process_connection_control(idigi_data_t * idigi_ptr, idigi_cc_data_t * cc_ptr, idigi_network_request_t request)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(cc_ptr);

    DEBUG_PRINTF("process_disconnect: Connection Disconnect\n");
    idigi_ptr->network_busy = true;

    status = close_server(idigi_ptr);
    if (status == idigi_callback_continue)
    {
        idigi_request_t request_id;
        request_id.network_request = request;

        status = idigi_callback(idigi_ptr->callback, idigi_class_network, request_id, NULL, 0, NULL, NULL);
        if (status == idigi_callback_continue)
        {
            init_setting(idigi_ptr);
        }
        else if (status == idigi_callback_abort)
        {
            idigi_ptr->error_code = idigi_server_disconnected;
        }
    }

    return status;
}


static idigi_callback_status_t  process_redirect(idigi_data_t * idigi_ptr, idigi_cc_data_t * cc_ptr, idigi_packet_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t     url_count;
    uint16_t    url_length;
    uint8_t     * buf;
    uint16_t    length;
    char        * server_url;
    uint16_t prefix_len;


    DEBUG_PRINTF("process_redirect:  redirect to new destination\n");
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
    buf = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
    buf++; /* skip redirect opcode */
    length = packet->header.length -1;

    url_count = *buf;
    buf++;
    length--;

    if (url_count == 0)
    {   /* nothing to redirect */
        DEBUG_PRINTF("cc_process_redirect: redirect with no url specified\n");
        goto done;
    }
    ASSERT(url_count <= CC_REDIRECT_SERVER_COUNT);

    if (url_count > CC_REDIRECT_SERVER_COUNT)
    {
        url_count = CC_REDIRECT_SERVER_COUNT;
    }

    prefix_len = strlen(URL_PREFIX);

    if (cc_ptr->state != cc_state_redirect_server)
    {
        int i;
        /* Close the connection before parsing new destination url */
        status = close_server(idigi_ptr);
        if (status != idigi_callback_continue)
        {
            goto done;
        }

        /* save original server url that we connected before */
        strcpy(cc_ptr->origin_url, URL_PREFIX);
        strcpy((cc_ptr->origin_url + prefix_len), idigi_ptr->server_url);

        cc_ptr->server_url[0][0] = 0x0;
        cc_ptr->server_url[1][0] = 0x0;

        for (i = 0; i < url_count; i++)
        {
            url_length = FROM_BE16(*((uint16_t *)buf));
            buf += sizeof url_length;
            ASSERT_GOTO(url_length < sizeof cc_ptr->server_url[i], done);

            strncpy(cc_ptr->server_url[i],(const char *) buf, url_length);
            cc_ptr->server_url[i][url_length] = '\0';
            cc_ptr->state = cc_state_redirect_server;
            buf += url_length;
            length -= url_length;
        }
        cc_ptr->item = 0;

    }

    if (cc_ptr->state == cc_state_redirect_server)
    {
        /* We got the new destination url and try connecting to it.
         *
         * We must first remove en:// prefix.
         */

        do {
            server_url = cc_ptr->server_url[cc_ptr->item];

            if (memcmp(server_url, URL_PREFIX, prefix_len) == 0)
            {
                server_url += prefix_len;
            }

            status = connect_server(idigi_ptr, server_url);
            if (status == idigi_callback_continue && idigi_ptr->network_handle != NULL)
            {
                cc_ptr->report_code = cc_redirect_success;
                /* now set the current server to this new redirect destination */
                strcpy(idigi_ptr->server_url, server_url);
                break;
            }
            if (status != idigi_callback_busy)
            {
                /* consider this is not error since we're
                 * going to connect to the origin server.
                 */
                cc_ptr->item++;
                cc_ptr->report_code = cc_redirect_error;
                strcpy(idigi_ptr->server_url, cc_ptr->origin_url + prefix_len);

            }
        } while (cc_ptr->item < url_count);
    }
    if (status != idigi_callback_busy)
    {
        /* Reset iDigi to inital state to establish communication with the server.
         *
         * Even if connect fails, we want to continue to
         * orginal server.
         */
        init_setting(idigi_ptr);
        cc_ptr->item = 0;
        cc_ptr->state = cc_state_redirect_report;
        set_idigi_state(idigi_ptr, edp_communication_layer);
        status = idigi_callback_continue;
    }
done:
    return status;
}

static idigi_callback_status_t cc_process(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;


    /* process incoming message from server for Connection Control facility */

    if (packet != NULL)
    {
        uint8_t opcode;
        uint8_t * ptr;
        idigi_cc_data_t * cc_ptr = facility_data;

        /* get the DATA portion of the packet */
        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
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
            DEBUG_PRINTF("idigi_cc_process: unsupported opcode %u\n", opcode);
            break;
        }
    }

    return status;
}
static idigi_callback_status_t cc_discovery(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_cc_data_t * cc_ptr = facility_data;

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
            cc_ptr->state = cc_state_connect_report;
        }
    }
    else if (cc_ptr->state == cc_state_connect_report)
    {
        status = send_connection_report(idigi_ptr, cc_ptr);
    }

    return status;
}

static idigi_callback_status_t cc_delete_facility(idigi_data_t * idigi_ptr)
{
    return del_facility_data(idigi_ptr, E_MSG_FAC_CC_NUM);
}

static idigi_callback_status_t cc_init_facility(idigi_data_t * idigi_ptr)
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

        status = add_facility_data(idigi_ptr, E_MSG_FAC_CC_NUM, &ptr, sizeof(idigi_cc_data_t), cc_discovery, cc_process);

        if (status == idigi_callback_abort || ptr == NULL)
        {
            goto done;
        }
        cc_ptr = (idigi_cc_data_t *)ptr;

    }
    cc_ptr->report_code = cc_not_redirect;
    cc_ptr->server_url[0][0] = '\0';
    cc_ptr->server_url[1][0] = '\0';
    cc_ptr->origin_url[0] = '\0';
    cc_ptr->state = cc_state_redirect_report;
    cc_ptr->item = 0;

done:
    return status;
}

