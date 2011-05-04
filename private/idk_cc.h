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
//#include "bele.c"
#include "idk_def.h"
#include "ei_security.h"
#include "ei_msg.h"

#define IDK_IPV6_ADDRESS_LENGTH	16
#define IDK_IPV4_ADDRESS_LENGTH	4

#define IDK_CC_NOT_REDIRECT	0

#define IDK_CC_REDIRECT_SUCCESS 0
#define IDK_CC_REDIRECT_ERROR   1

#define FAC_CC_DISCONNECT			0
#define FAC_CC_RESERVED_OPCODE1		1
#define FAC_CC_RESERVED_OPCODE2		2
#define FAC_CC_REDIRECT_TO_SDA		3
#define FAC_CC_REDIRECT_REPORT		4
#define FAC_CC_CONNECTION_REPORT	5

#define	FAC_CC_CLIENTTYPE_SIM		0
#define	FAC_CC_CLIENTTYPE_DEVICE	1


enum {
	cc_state_redirect_report,
	cc_state_connect_report,
	cc_state_redirect_server1,
	cc_state_redirect_server2
};

typedef struct {
    idk_facility_t  facility;

	char 	server_url[IDK_SERVER_URL_LENGTH];
	char	server_url2[IDK_SERVER_URL_LENGTH];
	uint8_t	report_code;
	uint8_t	security_code;
	int		state;
	unsigned item;

} idk_cc_data_t;

#define  idk_base_connection_control idk_base_connection_type

static idk_callback_status_t process_redirect_report(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr)
{
	idk_callback_status_t status;
	idk_facility_packet_t * p;
	uint8_t				report_msg_length = 0;
	uint16_t			url_length;
	uint8_t				* ptr;

	DEBUG_PRINTF("--- send redirect_report\n");

	/* build and send redirect report
	 *  ----------------------------------------------------
	 * |   0    |    1   |    2    |   3...  | x1-x2  | ... |
	 *  ----------------------------------------------------
	 * | opcode | report | message | report  |  url   | url |
 	 * |        |  code  | length  | message | length |     |
 	 *  ----------------------------------------------------
	 */

	p = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
	ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));

	*ptr++ = FAC_CC_REDIRECT_REPORT;  /* opcode */
	*ptr++ = cc_ptr->report_code;	 /* report code */
	p->length = 2;
#if 0
	if (cc_ptr->report_code != IDK_CC_NOT_REDIRECT)
	{
		/* skip buf[0] for report message length.
		 * let's construct report message first.
		 */

		report_msg_length = strlen(redirect_report.report_message);
		if (redirect_report.report_message != NULL && report_msg_length > 0)
		{
			memcpy((ptr+ p->length+1), redirect_report.report_message, report_msg_length);
			p->length += report_msg_length;
		}
	}
#endif

	/* report message length */
	*ptr++ = report_msg_length;
	p->length++;
	ptr += report_msg_length;


	/* URL length */
	url_length = TO_BE16(strlen(cc_ptr->server_url));
	memcpy(ptr, &url_length, 2);
	ptr += 2;
	p->length += 2;

	if (url_length > 0)
	{	/* URL */
		memcpy(ptr, cc_ptr->server_url, url_length);
		p->length += url_length;
	}

	status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_CC_NUM, cc_ptr->security_code);

	return status;
}

static idk_callback_status_t process_connection_report(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr)
{
	idk_callback_status_t status = idk_callback_continue;;
	idk_facility_packet_t * p;
    idk_status_t err_status;
    idk_request_t request_id;
	uint8_t * ptr;
	size_t length;

	DEBUG_PRINTF("--- send connection report\n");

	/* Build Connection report
	 *  -------------------------------------------------------
	 * |   0    |    1   | 2  - 17 |     18      |  19 ...     |
	 *  -------------------------------------------------------
	 * | opcode | client |   IP    | connection  | connection  |
	 * |        |  type  | address |    type     | information |
	 *  -------------------------------------------------------
     *
     * 1. call callback to get ip address & build ip_address in the send packet
     * 2. call callback to get & build conenction type in the send packet
     * 3. if connection type is LAN, call callback to get and build mac address in the send packet
     * 4. if connection type is WAN, call callback to get and build link speed in the send packet
     * 5. if connection type is WAN, call callback to get and build phone number in the send packet
	 */
	p = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;

	if (cc_ptr->item == idk_base_ip_addr)
	{
        uint8_t * ip_addr = NULL;

		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));

		*ptr++ = FAC_CC_CONNECTION_REPORT;  /* opcode */
		*ptr++ = FAC_CC_CLIENTTYPE_DEVICE;  /* client type */
		p->length = 2;

		/* callback for ip address */
		memset(ptr, 0x00, IDK_IPV6_ADDRESS_LENGTH);

		/* IP address (use IPv6 format) */
        request_id.base_request = idk_base_ip_addr;
        status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &ip_addr, &length);
        if (status != idk_callback_continue)
        {
            goto _ret;
        }
        if (ip_addr == NULL || length == 0 || (length != IDK_IPV6_ADDRESS_LENGTH && length != IDK_IPV4_ADDRESS_LENGTH))
        {
            err_status = idk_invalid_data_size;
            goto _error_param;
        }

		if (length == IDK_IPV6_ADDRESS_LENGTH)
		{
            /* IPv6 address */
			memcpy(ptr, ip_addr, IDK_IPV6_ADDRESS_LENGTH);
			ptr += IDK_IPV6_ADDRESS_LENGTH;
			cc_ptr->item = idk_base_connection_type;
		}
		else 
		{
            /* IPv4 address */
			if (*((uint32_t *)ip_addr) == 0x00000000 || *((uint32_t *)ip_addr) == 0xffffffff)
			{
				/* bad addr */
                err_status = idk_invalid_data;
                goto _error_param;
			}
			else
			{
				/* good ipv4 addr. convert to ipv6 format */
				ptr+= 10;
				*ptr++= 0xFF;
				*ptr++ = 0xFF;
				memcpy(ptr, ip_addr, IDK_IPV4_ADDRESS_LENGTH);
				p->length += IDK_IPV6_ADDRESS_LENGTH;
				cc_ptr->item = idk_base_connection_type;
			}
		}

	}


	if (cc_ptr->item == idk_base_connection_type)
	{
		/* callback for connection type */
		idk_connection_type_t * connection_type;

		/* connection type */
        request_id.base_request = idk_base_connection_type;
        status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &connection_type, &length);
		if (status != idk_callback_continue)
		{
			goto _ret;
		}

		if (connection_type == NULL || (*connection_type != idk_lan_connection_type && *connection_type != idk_wan_connection_type))
		{
            /* bad connection type */
            err_status = idk_invalid_data;
            goto _error_param;
		}
		else
		{
			ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
			ptr += p->length;

			*ptr = (*connection_type+1);  /* +1 to matach the spec LAN or WAN connection type */
			p->length++;
			if (*connection_type == idk_lan_connection_type)
			{
				cc_ptr->item = idk_base_mac_addr;
			}
			else
			{
				cc_ptr->item = idk_base_link_speed;
			}
		}

	}


	if (cc_ptr->item == idk_base_mac_addr)
	{
		/* callback for MAC for LAN connection type */
		uint8_t * mac;

		/* MAC address */
        request_id.base_request = idk_base_mac_addr;
        status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &mac, &length);
		if (status != idk_callback_continue)
		{
			goto _ret;
		}
        if (mac == NULL || length != IDK_MAC_ADDR_LENGTH)
        {
            /* bad connection type */
            err_status = idk_invalid_data_size;
            goto _error_param;
        }

		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
		ptr += p->length;
		memcpy(ptr, mac, IDK_MAC_ADDR_LENGTH);
		p->length += IDK_MAC_ADDR_LENGTH;

	}


	if (cc_ptr->item == idk_base_link_speed)
	{
		/* callback for Link speed for WAN connection type */
		uint32_t * link_speed;

		/* Link speed for WAN */
        request_id.base_request = idk_base_link_speed;
        status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &link_speed, &length);
		if (status != idk_callback_continue)
		{
			goto _ret;
		}

        if (link_speed == NULL || length != sizeof(uint32_t))
        {
            /* bad connection type */
            err_status = idk_invalid_data_size;
            goto _error_param;
        }

		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
		ptr += p->length;
		*((uint32_t *)ptr) = *link_speed;

		p->length += sizeof(uint32_t);
		cc_ptr->item = idk_base_phone_number;
	}

	if (cc_ptr->item == idk_base_phone_number)
	{
		/* callback for phone number for WAN connection type */
		char * phone = NULL;

		/* phone number */
        request_id.base_request = idk_base_phone_number;
        status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &phone, &length);

		if (status != idk_callback_continue)
		{
			goto _ret;
		}

        if (phone == NULL || length == 0)
        {
            /* bad connection type */
            err_status = idk_invalid_data_size;
_error_param:
            status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, err_status);
            goto _ret;
        }


		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
		ptr += p->length;
		memcpy(ptr, phone, length);
		p->length += length;

		status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_CC_NUM, cc_ptr->security_code);
	}

_ret:
	return status;
}

static idk_status_t process_disconnect(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr)
{
	idk_status_t rc = idk_success;
	idk_callback_status_t	status;
	idk_request_t request_id;

    (void)cc_ptr;

	DEBUG_PRINTF("---Connection Disconnect\n");

	status = net_close(idk_ptr);
	if (status == idk_callback_continue)
	{
        request_id.base_request = idk_base_disconnected;

        status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, NULL, NULL);
        if (status == idk_callback_continue)
        {
            cc_ptr->facility.packet = NULL;
//            idk_ptr->network_busy = true;
        }
        else if (status == idk_callback_abort)
        {
    		rc = idk_server_disconnected;
        }
        else
        {
            idk_ptr->network_busy = true;
        }
	}

	return rc;
}

static idk_status_t  process_redirect(idk_data_t * idk_ptr, idk_cc_data_t * cc_ptr, idk_facility_packet_t * p)
{
	idk_status_t rc = idk_success;
    idk_callback_status_t status;
	uint8_t		url_count;
	uint16_t	url_length;
	uint8_t		* buf;
	uint16_t	length;
	char		* server_url;

	/* Redirect new destination:
	 *
	 * 1. close connection
	 * 2. Parse new destinations (server url & server ip)
	 * 3. connect to new destinations. If connect to server url fails,
	 *    this function will return SUCCESS to try server ip. If connection is made,
	 *    callback for redirect will be called.
	 * 4. reset edp state so that idk_task will re-establish the EDP connection.
	 */
	if (cc_ptr->state != cc_state_redirect_server1 &&
		cc_ptr->state != cc_state_redirect_server1)
	{
		buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
		length = p->length;

		url_count = *buf;
		buf++;
		length--;

		if (url_count == 0)
		{	/* nothing to redirect */
			DEBUG_PRINTF("cc_process_redirect: redirect with no url specified\n");
			goto _ret;
		}

		status = net_close(idk_ptr);
		if (status == idk_callback_abort)
		{
			rc = idk_close_error;
			goto _ret;
		}
		else if (status == idk_callback_busy)
		{
			goto _ret;
		}


		url_length = FROM_BE16(*((uint16_t *)buf));
		buf += sizeof url_length;
		if (url_length > sizeof cc_ptr->server_url)
		{
			DEBUG_PRINTF("cc_process_redirect: url length (%d) > max size (%d)\n", url_length, (int)sizeof cc_ptr->server_url);
		}
		else
		{
			memcpy(cc_ptr->server_url, buf, url_length);
			cc_ptr->server_url[url_length] = '\0';
			cc_ptr->state = cc_state_redirect_server1;
		}

		buf += url_length;

		url_length = FROM_BE16(*((uint16_t *)buf));
		buf += sizeof url_length;
		if (url_length > sizeof cc_ptr->server_url)
		{
			DEBUG_PRINTF("cc_process_redirect: url2 length (%d) > max size (%d)\n", url_length, (int)sizeof cc_ptr->server_url);
		}
		else
		{
			memcpy(cc_ptr->server_url2, buf, url_length);
			cc_ptr->server_url2[url_length] = '\0';
			if (cc_ptr->state != cc_state_redirect_server1)
			{
				cc_ptr->state = cc_state_redirect_server2;
			}
		}
		init_setting(idk_ptr);
	}

	if (cc_ptr->state == cc_state_redirect_server1 ||
		cc_ptr->state == cc_state_redirect_server2)
	{
		if (cc_ptr->state == cc_state_redirect_server1)
		{
			server_url = cc_ptr->server_url;
		}
		else
		{
			server_url = cc_ptr->server_url2;
		}

		status = net_connect_server(idk_ptr, server_url, IDK_MT_PORT);
		if (status == idk_callback_continue && idk_ptr->network_handle != NULL)
		{
			cc_ptr->report_code = IDK_CC_REDIRECT_SUCCESS;
			set_idk_state(idk_ptr, edp_communication_layer);
		}
		else if (status != idk_callback_busy)
		{
			cc_ptr->report_code = IDK_CC_REDIRECT_ERROR;
			if (cc_ptr->state == cc_state_redirect_server1 &&
				strlen(cc_ptr->server_url2) > 0)
			{
				cc_ptr->server_url[0] = '\0';
				cc_ptr->state = cc_state_redirect_server2;
			}
			else
			{
				set_idk_state(idk_ptr, edp_communication_layer);
			}
		}
	}

_ret:
	return rc;
}

static idk_status_t cc_process(idk_data_t * idk_ptr, idk_facility_t * fac_ptr)
{
	idk_status_t rc = idk_success;
	uint8_t opcode, * ptr;
	idk_facility_packet_t * p;
	idk_cc_data_t * cc_ptr = (idk_cc_data_t *)fac_ptr;


	DEBUG_PRINTF("idk_cc_process...\n");

	if (fac_ptr->packet != NULL)
	{
		p = (idk_facility_packet_t *)fac_ptr->packet;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
		opcode = *ptr;

		if (opcode == FAC_CC_DISCONNECT)
		{
			rc = process_disconnect(idk_ptr, cc_ptr);
		}
		else if (opcode == FAC_CC_REDIRECT_TO_SDA)
		{
			rc = process_redirect(idk_ptr, cc_ptr, p);
		}
		else
		{
			DEBUG_PRINTF("idk_cc_process: unsupported opcode 0x%x\b", opcode);
		}
	}

	return rc;
}

idk_callback_status_t cc_discovery_layer(idk_data_t * idk_ptr)
{
	idk_callback_status_t status;
   idk_cc_data_t * cc_ptr;

    cc_ptr = (idk_cc_data_t *)get_facility_data(idk_ptr, idk_base_connection_control);
    if (cc_ptr == NULL)
    {
        status = add_facility_data(idk_ptr, idk_base_connection_control, (idk_facility_t **) &cc_ptr, sizeof(idk_cc_data_t), cc_process);

        if (status == idk_callback_abort || cc_ptr == NULL)
        {
            goto _ret;
        }
        cc_ptr->state = cc_state_redirect_report;
		cc_ptr->report_code = IDK_CC_NOT_REDIRECT;
		cc_ptr->server_url[0] = '\0';
		cc_ptr->security_code = SECURITY_PROTO_NONE;
		cc_ptr->item = idk_base_ip_addr;

    }

	if (cc_ptr->state == cc_state_redirect_report)
	{
		status = process_redirect_report(idk_ptr, cc_ptr);
		if (status == idk_callback_continue)
		{
			cc_ptr->state = cc_state_connect_report;
		}
	}
	else if (cc_ptr->state == cc_state_connect_report)
	{
		status = process_connection_report(idk_ptr, cc_ptr);
	}

_ret:
	return status;
}

