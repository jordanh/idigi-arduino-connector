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
#include "ei_security.h"
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
#define FAC_CC_RESERVED_OPCODE1     0x01
#define FAC_CC_RESERVED_OPCODE2     0x02
#define FAC_CC_REDIRECT_TO_SDA      0x03
#define FAC_CC_REDIRECT_REPORT      0x04
#define FAC_CC_CONNECTION_REPORT    0x05

#define FAC_CC_CLIENTTYPE_SIM       0
#define FAC_CC_CLIENTTYPE_DEVICE    1

#define CC_REDIRECT_SERVER_COUNT    2

enum {
    cc_state_redirect_report,
    cc_state_connect_report,
    cc_state_redirect_server,
};

typedef struct {
    idk_facility_t facility;
    char    server_url[CC_REDIRECT_SERVER_COUNT][SERVER_URL_LENGTH];
    uint8_t report_code;
    uint8_t security_code;
    int     state;
    unsigned item;

} idk_cc_data_t;

#define  idk_base_connection_control idk_base_connection_type

static idk_callback_status_t send_redirect_report(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr)
{
    idk_callback_status_t status;
    idk_facility_packet_t * packet;
    uint8_t             report_msg_length = 0;
    uint16_t            url_length;
    uint8_t             * ptr, * data_ptr;

    DEBUG_PRINTF("Connection Control: send redirect_report\n");

    /* build and send redirect report
     *  ----------------------------------------------------
     * |   0    |    1   |    2    |   3...  | x1-x2  | ... |
     *  ----------------------------------------------------
     * | opcode | report | message | report  |  url   | url |
     * |        |  code  | length  | message | length |     |
     *  ----------------------------------------------------
     */

    packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idk_callback_busy;
        goto done;
    }
    data_ptr = ptr;

    *ptr++ = FAC_CC_REDIRECT_REPORT;  /* opcode */
    *ptr++ = cc_ptr->report_code;    /* report code */

    /* report message length */
    *ptr++ = report_msg_length;
    ptr += report_msg_length;


    /* URL length */
    if (cc_ptr->item > 0)
    {
        cc_ptr->item--;
    }

    url_length = strlen(cc_ptr->server_url[cc_ptr->item]);
    StoreBE16(ptr, url_length);
    ptr += sizeof(url_length);

    if (url_length > 0)
    {   /* URL */
        memcpy(ptr, cc_ptr->server_url[cc_ptr->item], url_length);
        ptr += url_length;
    }

    packet->length = ptr - data_ptr;
    cc_ptr->item = idk_base_ip_addr;

    status = enable_facility_packet(idk_ptr, E_MSG_FAC_CC_NUM, cc_ptr->security_code);
done:
    return status;
}
static idk_callback_status_t get_ip_addr(idk_data_t * idk_ptr, uint8_t * ipv6_addr)
{
    idk_callback_status_t status;

    uint8_t * ip_addr = NULL;
    idk_request_t request_id;
    size_t length;

  /* Get IP address */
    request_id.base_request = idk_base_ip_addr;
    status = idk_callback(idk_ptr->callback, idk_class_base, request_id, NULL, 0, &ip_addr, &length);
    if (status != idk_callback_continue)
    {
        idk_ptr->error_code = idk_configuration_error;
        goto done;
    }
    if (ip_addr == NULL || (length != CC_IPV6_ADDRESS_LENGTH && length != CC_IPV4_ADDRESS_LENGTH))
    {
        idk_ptr->error_code = idk_invalid_data_size;
        goto error;
    }

    if (length == CC_IPV6_ADDRESS_LENGTH)
    {
        /* IPv6 address */
        memcpy(ipv6_addr, ip_addr, CC_IPV6_ADDRESS_LENGTH);
        goto done;
    }
    else
    {
        /* IPv4 address */
        if (*((uint32_t *)ip_addr) == CC_ZERO_IP_ADDR || *((uint32_t *)ip_addr) == CC_BOARDCAST_IP_ADDR)
        {
            /* bad addr */
            idk_ptr->error_code = idk_invalid_data;
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
    status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_ptr->error_code);
    if (status != idk_callback_abort)
    {
        /* set to busy to come here to get valid ip addr */
        status =idk_callback_busy;
    }

done:
    return status;

}
static idk_callback_status_t get_connection_type(idk_data_t * idk_ptr, uint8_t * connection_type)
{
    idk_callback_status_t status;

    idk_request_t request_id;
    idk_connection_type_t * type;

    /* callback for connection type */
    request_id.base_request = idk_base_connection_type;
    status = idk_callback(idk_ptr->callback, idk_class_base, request_id, NULL, 0, &type, NULL);
    if (status != idk_callback_continue)
    {
        idk_ptr->error_code = idk_configuration_error;
        goto done;
    }

    if (type == NULL)
    {
        /* bad connection type */
        idk_ptr->error_code = idk_invalid_data;
    }
    else
    {
        switch (*type)
        {
        case idk_lan_connection_type:
            *connection_type = ethernet_type;
            break;
        case idk_wan_connection_type:
            *connection_type = ppp_over_modem_type;
            break;
        default:
            idk_ptr->error_code = idk_invalid_data;
            goto error;

        }
        goto done;

   }
error:
    status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_ptr->error_code);
    if (status != idk_callback_abort)
    {
        /* set to busy to come here to get valid connection type */
        status =idk_callback_busy;
    }

done:
    return status;
}

static idk_callback_status_t get_mac_addr(idk_data_t * idk_ptr, uint8_t * mac_addr)
{
    idk_callback_status_t status = idk_callback_continue;;
    idk_request_t request_id;
    size_t length;

    uint8_t * mac;

    /* callback for MAC addr for LAN connection type */
    request_id.base_request = idk_base_mac_addr;
    status = idk_callback(idk_ptr->callback, idk_class_base, request_id, NULL, 0, &mac, &length);
    if (status != idk_callback_continue)
    {
        idk_ptr->error_code = idk_configuration_error;
    }
    else if (mac == NULL || length != MAC_ADDR_LENGTH)
    {
        /* bad connection type */
        idk_ptr->error_code = idk_invalid_data_size;
        status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_ptr->error_code);
        if (status != idk_callback_abort)
        {
            /* set to busy to come here to get valid mac address */
            status =idk_callback_busy;
        }
    }
    else
    {
        memcpy(mac_addr, mac, MAC_ADDR_LENGTH);
    }

    return status;
}

static idk_callback_status_t send_connection_report(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr)
{

    idk_callback_status_t status = idk_callback_continue;;
    idk_facility_packet_t * packet;
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
    packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idk_callback_busy;
        goto done;
    }

    if (cc_ptr->item == idk_base_ip_addr)
    {
        *ptr++ = FAC_CC_CONNECTION_REPORT;  /* opcode */
        *ptr++ = FAC_CC_CLIENTTYPE_DEVICE;  /* client type */
        packet->length = 2;

        status = get_ip_addr(idk_ptr, ptr);
        if (status != idk_callback_continue)
        {
            goto done;
        }

        cc_ptr->item = idk_base_connection_type;
        packet->length += CC_IPV6_ADDRESS_LENGTH;
    }


    if (cc_ptr->item == idk_base_connection_type)
    {
        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
        ptr += packet->length;
        status = get_connection_type(idk_ptr, ptr);
        if (status != idk_callback_continue)
        {
            goto done;
        }
        if (*ptr == ethernet_type)
        {
            cc_ptr->item = idk_base_mac_addr;
        }
        else
        {
            cc_ptr->item = idk_base_link_speed;
        }
        packet->length++;

    }


    if (cc_ptr->item == idk_base_mac_addr)
    {
        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
        ptr += packet->length;
        status = get_mac_addr(idk_ptr, ptr);
        if (status != idk_callback_continue)
        {
            goto done;
        }
        packet->length += MAC_ADDR_LENGTH;


    }


    if (cc_ptr->item == idk_base_link_speed)
    {
        /* callback for Link speed for WAN connection type */
        idk_request_t request_id;
        size_t length;
        uint32_t * link_speed;

        request_id.base_request = idk_base_link_speed;
        status = idk_callback(idk_ptr->callback, idk_class_base, request_id, NULL, 0, &link_speed, &length);
        if (status != idk_callback_continue)
        {
            idk_ptr->error_code = idk_configuration_error;
            goto done;
        }

        if (link_speed == NULL || length != sizeof(*link_speed))
        {
            /* bad connection type */
            idk_ptr->error_code = idk_invalid_data_size;
            status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_ptr->error_code);
            goto done;
        }

        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
        ptr += packet->length;
        StoreBE32(ptr, *link_speed);
        packet->length += sizeof(*link_speed);
        cc_ptr->item = idk_base_phone_number;
    }

    if (cc_ptr->item == idk_base_phone_number)
    {
        /* callback for phone number for WAN connection type */
        idk_request_t request_id;
        size_t length;
        uint8_t * phone = NULL;

        request_id.base_request = idk_base_phone_number;
        status = idk_callback(idk_ptr->callback, idk_class_base, request_id, NULL, 0, &phone, &length);

        if (status != idk_callback_continue)
        {
            idk_ptr->error_code = idk_configuration_error;
            goto done;
        }

        if (phone == NULL || length == 0)
        {
            /* bad connection type */
            idk_ptr->error_code = idk_invalid_data_size;
            status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_ptr->error_code);
            goto done;
        }

        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
        ptr += packet->length;
        memcpy(ptr, phone, length);
        packet->length += length;

    }
    status = enable_facility_packet(idk_ptr, E_MSG_FAC_CC_NUM, cc_ptr->security_code);

done:
    return status;
}

static idk_callback_status_t process_disconnect(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_request_t request_id;

    UNUSED_PARAMETER(cc_ptr);

    DEBUG_PRINTF("process_disconnect: Connection Disconnect\n");
    idk_ptr->network_busy = true;

    status = close_server(idk_ptr);
    if (status == idk_callback_continue)
    {
        request_id.base_request = idk_base_disconnected;

        status = idk_callback(idk_ptr->callback, idk_class_base, request_id, NULL, 0, NULL, NULL);
        if (status == idk_callback_continue)
        {
            init_setting(idk_ptr);
        }
        else if (status == idk_callback_abort)
        {
            idk_ptr->error_code = idk_server_disconnected;
        }
    }

    return status;
}

static idk_callback_status_t  process_redirect(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr, idk_facility_packet_t * packet)
{
    idk_callback_status_t status = idk_callback_continue;
    uint8_t     url_count;
    uint16_t    url_length;
    uint8_t     * buf;
    uint16_t    length, prefix_len;
    char        * server_url;

    DEBUG_PRINTF("process_redirect:  redirect to new destination\n");
    /* Redirect to new destination:
     * IDK will close connection and connect to new destination. If connect
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
    */
    buf = GET_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    buf++; /* skip redirect opcode */
    length = packet->length -1;


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

    if (cc_ptr->state != cc_state_redirect_server)
    {
        int i;
        /* Close the connection before parsing new destination url */
        status = close_server(idk_ptr);
        if (status != idk_callback_continue)
        {
            goto done;
        }
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
        prefix_len = strlen(URL_PREFIX);

        do {
            server_url = cc_ptr->server_url[cc_ptr->item];

            if (memcmp(server_url, URL_PREFIX, prefix_len) == 0)
            {
                server_url += prefix_len;
            }

            status = connect_server(idk_ptr, server_url, EDP_MT_PORT);
            if (status == idk_callback_continue && idk_ptr->network_handle != NULL)
            {
                cc_ptr->item++;
                cc_ptr->report_code = cc_redirect_success;
                idk_ptr->server_url = server_url;
                break;
            }
            if (status != idk_callback_busy)
            {
                cc_ptr->item++;
                cc_ptr->report_code = cc_redirect_error;
            }
        } while (cc_ptr->item < url_count);
    }
    if (status != idk_callback_busy)
    {
        /* Reset IDK to inital state to establish communication with the server.
         *
         * Even if connect fails, we want to continue to
         * orginal server.
         */
        init_setting(idk_ptr);
        cc_ptr->item = 0;
        cc_ptr->state = cc_state_redirect_report;
        set_idk_state(idk_ptr, edp_communication_layer);
        status = idk_callback_continue;
    }
done:
    return status;
}

static idk_callback_status_t cc_process(idk_data_t * idk_ptr, idk_facility_t * fac_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_facility_packet_t * packet = GET_FACILITY_PACKET(fac_ptr);;


    /* process incoming message from server for Connection Control facility */

    if (packet != NULL)
    {
        uint8_t opcode;
        uint8_t * ptr;
        idk_cc_data_t * cc_ptr = GET_FACILITY_POINTER(fac_ptr);

        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
        opcode = *ptr;

        if (opcode == FAC_CC_DISCONNECT)
        {
            status = process_disconnect(idk_ptr, cc_ptr);
        }
        else if (opcode == FAC_CC_REDIRECT_TO_SDA)
        {
            status = process_redirect(idk_ptr, cc_ptr, packet);
        }
        else
        {
            DEBUG_PRINTF("idk_cc_process: unsupported opcode %u\n", opcode);
        }
        if (status != idk_callback_busy)
        {
            /* clear this when it's done */
            DONE_FACILITY_PACKET(fac_ptr);
        }
    }

    return status;
}
static idk_callback_status_t cc_discovery(idk_data_t * idk_ptr, idk_facility_t * fac_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_cc_data_t * cc_ptr = GET_FACILITY_POINTER(fac_ptr);

    /* Connection control facility needs to send redirect and
     * connection reports on discovery layer.
     */
    if (cc_ptr->state == cc_state_redirect_report)
    {
        status = send_redirect_report(idk_ptr, cc_ptr);
        if (status == idk_callback_continue)
        {
            status = idk_callback_busy;
            cc_ptr->state = cc_state_connect_report;
        }
    }
    else if (cc_ptr->state == cc_state_connect_report)
    {
        status = send_connection_report(idk_ptr, cc_ptr);
    }

    return status;
}

static idk_status_t cc_delete_facility(idk_data_t * idk_ptr)
{
    return del_facility_data(idk_ptr, E_MSG_FAC_CC_NUM);
}

static idk_callback_status_t cc_init_facility(idk_data_t * idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_cc_data_t * cc_ptr;

    /* Add Connection control facility to IDK */
    cc_ptr = get_facility_data(idk_ptr, E_MSG_FAC_CC_NUM);
    if (cc_ptr == NULL)
    {
        void * ptr;

        status = add_facility_data(idk_ptr, E_MSG_FAC_CC_NUM, &ptr, sizeof(idk_cc_data_t), cc_discovery, cc_process);

        if (status == idk_callback_abort || ptr == NULL)
        {
            goto done;
        }
        cc_ptr = (idk_cc_data_t *)ptr;

    }
    cc_ptr->report_code = cc_not_redirect;
    cc_ptr->server_url[0][0] = '\0';
    cc_ptr->server_url[1][0] = '\0';
    cc_ptr->state = cc_state_redirect_report;
    cc_ptr->security_code = SECURITY_PROTO_NONE;
    cc_ptr->item = 0;

done:
    return status;
}

