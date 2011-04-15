/*
 * irl_cc.c
 *
 *  Created on: Mar 30, 2011
 *      Author: mlchan
 */
#include <string.h>

#include "bele.h"
#include "irl_api.h"
#include "os_intf.h"
#include "config_intf.h"
#include "network_intf.h"
#include "ei_security.h"
#include "ei_msg.h"
#include "irl_cc.h"

enum {
	IRL_CC_STATE_REDIRECT_REPORT,
	IRL_CC_STATE_CONNECT_REPORT,
	IRL_CC_STATE_REDIRECT_SERVER1,
	IRL_CC_STATE_REDIRECT_SERVER2
};

struct irl_connection_control_t	{
	char 	server_url[IRL_SERVER_URL_LENGTH];
	char	server_url2[IRL_SERVER_URL_LENGTH];
	uint8_t	report_code;
	uint8_t	security_code;
	int		facility_state;
	unsigned item;

};

struct irl_connection_control_t	* gIrlConnectionControl = NULL;

static int cc_redirect_report(struct irl_setting_t * irl_ptr, struct irl_connection_control_t * cc_ptr)
{
	int							rc = IRL_SUCCESS;
	IrlStatus_t					status;
	struct e_packet 			* p;
	struct irl_redirect_t 		redirect_report;
	uint8_t						report_msg_length = 0;
	uint16_t					url_length;

	DEBUG_TRACE("--- send redirect_report\n");
	p = &irl_ptr->data_packet;
	irl_send_packet_init(irl_ptr, p, PKT_PRE_FACILITY);

	/*
	 *  ----------------------------------------------------
	 * |   0    |    1   |    2    |   3...  | x1-x2  | ... |
	 *  ----------------------------------------------------
	 * | opcode | report | message | report  |  url   | url |
 	 * |        |  code  | length  | message | length |     |
 	 *  ----------------------------------------------------
	 */
	p->buf[0] = FAC_CC_REDIRECT_REPORT;  /* opcode */
	p->buf[1] = cc_ptr->report_code;	 /* report code */
	p->length = 2;

	if (cc_ptr->report_code != IRL_CC_NOT_REDIRECT)
	{
		/* callback to notify caller the redirection.
		 * callback should return report message.
		 */
		uint16_t	rx_keepalive, tx_keepalive;

		if (strlen(cc_ptr->server_url) > 0)
			redirect_report.destination = cc_ptr->server_url;
		else
			redirect_report.destination = cc_ptr->server_url2;

		redirect_report.report_code = cc_ptr->report_code;

		rx_keepalive = GET_RX_KEEPALIVE((struct irl_setting_t *)irl_ptr);
		tx_keepalive = GET_TX_KEEPALIVE((struct irl_setting_t *)irl_ptr);
		redirect_report.timeout = IRL_MIN(rx_keepalive, tx_keepalive);
		if (redirect_report.timeout == 0) redirect_report.timeout = IRL_MIN_NETWORK_TIMEOUT;

		status = irl_get_config(irl_ptr, IRL_REDIRECT, &redirect_report);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}
		if (status != IRL_STATUS_CONTINUE)
		{
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}

		/* skip buf[0] for report message length.
		 * let's construct report message first.
		 */
		report_msg_length = strlen(redirect_report.report_message);
		if (redirect_report.report_message != NULL && report_msg_length > 0)
		{
			memcpy(&p->buf[p->length+1], redirect_report.report_message, report_msg_length);
			p->length += report_msg_length;
		}
	}
	/* report message length */
	p->buf[p->length] = report_msg_length;
	p->length++;

	/* URL length */
	url_length = TO_BE16(strlen(cc_ptr->server_url));
	memcpy(&p->buf[p->length], &url_length, 2);
	p->length += 2;

	if (url_length > 0)
	{	/* URL */
		memcpy(&p->buf[p->length], cc_ptr->server_url, url_length);
		p->length += url_length;
	}

	rc = irl_send_facility_layer(irl_ptr, p, E_MSG_FAC_CC_NUM, cc_ptr->security_code);

_ret:
	return rc;
}

static int cc_connection_report(struct irl_setting_t * irl_ptr, struct irl_connection_control_t * cc_ptr)
{
	int							rc = IRL_BUSY;
	IrlStatus_t					status;
	struct e_packet 			* p;
	struct irl_config_ip_addr_t	* addr;

	DEBUG_TRACE("--- send connection report\n");

	p = &irl_ptr->data_packet;

	if (cc_ptr->item == IRL_CONFIG_IP_ADDR)
	{
		irl_send_packet_init(irl_ptr, p, PKT_PRE_FACILITY);


		/* Build Connection report
		 *  -------------------------------------------------------
		 * |   0    |    1   | 2  - 17 |     18      |  19 ...     |
		 *  -------------------------------------------------------
		 * | opcode | client |   IP    | connection  | connection  |
	 	 * |        |  type  | address |    type     | information |
	 	 *  -------------------------------------------------------
		 */

		p->buf[0] = FAC_CC_CONNECTION_REPORT;  /* opcode */
		p->buf[1] = FAC_CC_CLIENTTYPE_DEVICE;  /* client type */
		p->length = 2;

		/* callback for ip address */
		memset(&p->buf[2], 0x00, IRL_IPV6_ADDRESS_LENGTH);

		/* IP address (use IPv6 format) */
		status = irl_get_config(irl_ptr, IRL_CONFIG_IP_ADDR, &irl_ptr->config.data[IRL_CONFIG_IP_ADDR]);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}
		if (status != IRL_STATUS_CONTINUE)
		{
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}

		addr = (struct irl_config_ip_addr_t *)irl_ptr->config.data[IRL_CONFIG_IP_ADDR];

		if (addr != NULL && addr->ip_addr != NULL)
		{
			if (addr->sa_family == AF_INET)
			{
				uint32_t	* ipv4;

				ipv4 = (uint32_t *)addr->ip_addr;
				if (*ipv4 == 0x00000000 || *ipv4 == 0xffffffff)
				{
					/* bad addr */
					status = irl_error_status(irl_ptr->callback, IRL_CONFIG_IP_ADDR, IRL_INVALID_DATA);
					if (status == IRL_STATUS_BUSY)
					{
						rc = IRL_BUSY;
						goto _ret;
					}
					if (status != IRL_STATUS_CONTINUE)
					{
						rc = IRL_CONFIG_ERR;
						goto _ret;
					}
				}
				else
				{
					/* good ipv4 addr. convert to ipv6 format */
					memset(&p->buf[12], 0xFF, 2);
					memcpy(&p->buf[14], ipv4, 4);
					cc_ptr->item = IRL_CONFIG_CONNECTION_TYPE;
					p->length += IRL_IPV6_ADDRESS_LENGTH;

				}
			}
			else
			{
				memcpy(&p->buf[12], addr->ip_addr, IRL_IPV6_ADDRESS_LENGTH);
				cc_ptr->item = IRL_CONFIG_CONNECTION_TYPE;
				p->length += IRL_IPV6_ADDRESS_LENGTH;

			}
		}

	}


	if (cc_ptr->item == IRL_CONFIG_CONNECTION_TYPE)
	{
		/* callback for connection type */
		uint8_t * connection_type;

		/* connection type */
		status = irl_get_config(irl_ptr, IRL_CONFIG_CONNECTION_TYPE, &irl_ptr->config.data[IRL_CONFIG_CONNECTION_TYPE]);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}
		if (status != IRL_STATUS_CONTINUE)
		{
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}

		connection_type = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_CONNECTION_TYPE];

		if (connection_type == NULL || (*connection_type != IRL_LAN_CONNECTION && *connection_type != IRL_WAN_CONNECTION))
		{
				/* bad connection type */
				status = irl_error_status(irl_ptr->callback, IRL_CONFIG_CONNECTION_TYPE, IRL_INVALID_DATA);
				if (status == IRL_STATUS_BUSY)
				{
					rc = IRL_BUSY;
					goto _ret;
				}
				if (status != IRL_STATUS_CONTINUE)
				{
					rc = IRL_CONFIG_ERR;
					goto _ret;
				}
		}
		else
		{
			p->buf[p->length] = *connection_type;
			p->length++;
			if (*connection_type == IRL_LAN_CONNECTION)
			{
				cc_ptr->item = IRL_CONFIG_MAC_ADDR;
			}
			else
			{
				cc_ptr->item = IRL_CONFIG_LINK_SPEED;
			}
		}

	}


	if (cc_ptr->item == IRL_CONFIG_MAC_ADDR)
	{
		/* callback for MAC for LAN connection type */
		uint8_t * mac, * connection_type;

		/* MAC address */
		status = irl_get_config(irl_ptr, IRL_CONFIG_MAC_ADDR, &irl_ptr->config.data[IRL_CONFIG_MAC_ADDR]);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}
		if (status != IRL_STATUS_CONTINUE)
		{
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}

		mac = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_MAC_ADDR];

		memcpy(&p->buf[p->length], mac, IRL_MAC_ADDR_LENGTH);
		p->length += IRL_MAC_ADDR_LENGTH;

		connection_type = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_CONNECTION_TYPE];
		rc = IRL_SUCCESS;
	}


	if (cc_ptr->item == IRL_CONFIG_LINK_SPEED)
	{
		/* callback for Link speed for WAN connection type */
		uint8_t * link_speed;

		/* Link speed for WAN */
		status = irl_get_config(irl_ptr, IRL_CONFIG_LINK_SPEED, &irl_ptr->config.data[IRL_CONFIG_LINK_SPEED]);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}
		if (status != IRL_STATUS_CONTINUE)
		{
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}

		link_speed = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_LINK_SPEED];

		memcpy(&p->buf[p->length], link_speed, IRL_LINK_SPEED_LENGTH);
		p->length += IRL_LINK_SPEED_LENGTH;

		cc_ptr->item = IRl_CONFIG_PHONE_NUM;

	}

	if (cc_ptr->item == IRl_CONFIG_PHONE_NUM)
	{
		/* callback for phone number for WAN connection type */
		struct irl_config_phone_t * phone;

		/* phone number */
		status = irl_get_config(irl_ptr, IRl_CONFIG_PHONE_NUM, &irl_ptr->config.data[IRl_CONFIG_PHONE_NUM]);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}
		if (status != IRL_STATUS_CONTINUE)
		{
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}

		phone = (struct irl_config_phone_t *)irl_ptr->config.data[IRl_CONFIG_PHONE_NUM];

		memcpy(&p->buf[p->length], phone->number, phone->length);
		p->length += phone->length;
		rc = IRL_SUCCESS;
	}

	if (rc == IRL_SUCCESS)
	{
		rc = irl_send_facility_layer(irl_ptr, p, E_MSG_FAC_CC_NUM, cc_ptr->security_code);
	}

_ret:
	return rc;
}

int cc_discover_facility(struct irl_setting_t * irl_ptr, struct irl_connection_control_t * cc_ptr)
{
	int rc = IRL_SUCCESS;

	if (cc_ptr->facility_state == IRL_CC_STATE_REDIRECT_REPORT)
	{
		rc = cc_redirect_report(irl_ptr, cc_ptr);
		if (rc == IRL_SUCCESS)
		{
			cc_ptr->facility_state = IRL_CC_STATE_CONNECT_REPORT;
			rc = IRL_BUSY;
		}
	}
	else if (cc_ptr->facility_state == IRL_CC_STATE_CONNECT_REPORT)
	{
		rc = cc_connection_report(irl_ptr, cc_ptr);
	}

	return rc;
}

int cc_process_disconnect(struct irl_setting_t * irl_ptr, struct irl_connection_control_t * cc_ptr)
{
	int 		rc;
	IrlStatus_t	status;

	DEBUG_TRACE("---Connection Disconnect\n");

	rc = irl_close(irl_ptr);
	if (rc == IRL_SUCCESS)
	{

		status = irl_get_config(irl_ptr, IRL_DISCONNECTED, NULL);

		if (status == IRL_STATUS_BUSY)
		{
			rc = IRL_BUSY;
			goto _ret;
		}

		if (status != IRL_STATUS_CONTINUE)
		{
			DEBUG_TRACE("irl_cc_process: server disconnect\n");
			rc = IRL_SERVER_DISCONNECTED;
		}
		/* reset to initial state */
		irl_init_setting(irl_ptr);
	}
_ret:
	return rc;
}

int cc_process_redirect(struct irl_setting_t * irl_ptr, struct irl_connection_control_t * cc_ptr, struct e_packet * p)
{
	int 							rc = IRL_SUCCESS;
	uint8_t		url_count;
	uint16_t	url_length;
	uint8_t		* buf;
	uint16_t	length;
	char		* server_url;

	if (cc_ptr->facility_state != IRL_CC_STATE_REDIRECT_SERVER1 &&
		cc_ptr->facility_state != IRL_CC_STATE_REDIRECT_SERVER1)
	{
		buf = p->buf+1;
		length = p->length;

		url_count = *buf;
		buf++;
		length--;

		if (url_count == 0)
		{	/* nothing to redirect */
			DEBUG_TRACE("cc_process_redirect: redirect with no url specified\n");
			goto _ret;
		}

		rc = irl_close(irl_ptr);
		if (rc != IRL_SUCCESS)
		{
			goto _ret;
		}


		url_length = FROM_BE16(*((uint16_t *)buf));
		buf += sizeof url_length;
		if (url_length > sizeof cc_ptr->server_url)
		{
			DEBUG_TRACE("cc_process_redirect: url length (%d) > max size (%d)\n", url_length, sizeof cc_ptr->server_url);
		}
		else
		{
			memcpy(cc_ptr->server_url, buf, url_length);
			cc_ptr->server_url[url_length] = '\0';
			cc_ptr->facility_state = IRL_CC_STATE_REDIRECT_SERVER1;
		}

		buf += url_length;

		url_length = FROM_BE16(*((uint16_t *)buf));
		buf += sizeof url_length;
		if (url_length > sizeof cc_ptr->server_url)
		{
			DEBUG_TRACE("cc_process_redirect: url2 length (%d) > max size (%d)\n", url_length, sizeof cc_ptr->server_url);
		}
		else
		{
			memcpy(cc_ptr->server_url2, buf, url_length);
			cc_ptr->server_url2[url_length] = '\0';
			if (cc_ptr->facility_state != IRL_CC_STATE_REDIRECT_SERVER1)
			{
				cc_ptr->facility_state = IRL_CC_STATE_REDIRECT_SERVER2;
			}
		}
		irl_init_setting(irl_ptr);
	}

	if (cc_ptr->facility_state == IRL_CC_STATE_REDIRECT_SERVER1 ||
		cc_ptr->facility_state == IRL_CC_STATE_REDIRECT_SERVER2)
	{
		if (cc_ptr->facility_state == IRL_CC_STATE_REDIRECT_SERVER1)
		{
			server_url = cc_ptr->server_url;
		}
		else
		{
			server_url = cc_ptr->server_url2;
		}

		rc = irl_connect_server(irl_ptr, server_url, IRL_MT_PORT);
		if (rc == IRL_SUCCESS)
		{
			gIrlConnectionControl->report_code = IRL_CC_REDIRECT_SUCCESS;
			irl_set_edp_state(irl_ptr, IRL_COMMUNICATION_LAYER);
		}
		else if (rc != IRL_BUSY)
		{
			gIrlConnectionControl->report_code = IRL_CC_REDIRECT_ERROR;
			if (cc_ptr->facility_state == IRL_CC_STATE_REDIRECT_SERVER1 &&
				strlen(cc_ptr->server_url2) > 0)
			{
				cc_ptr->server_url[0] = '\0';
				cc_ptr->facility_state = IRL_CC_STATE_REDIRECT_SERVER2;
			}
			else
			{
				irl_set_edp_state(irl_ptr, IRL_COMMUNICATION_LAYER);
			}
		}
	}

_ret:
	return rc;
}

int irl_cc_process(struct irl_setting_t * irl_ptr, struct irl_facility_handle_t * fac_ptr, struct e_packet * p)
{
	int 							rc = IRL_SUCCESS;
	uint8_t							opcode;
	struct irl_connection_control_t	* cc_ptr = (struct irl_connection_control_t *)fac_ptr->facility_data;


	DEBUG_TRACE("irl_cc_process...\n");

	if (p == NULL)
	{
		rc = cc_discover_facility(irl_ptr, cc_ptr);
	}
	else
	{
		opcode = p->buf[0];

		if (opcode == FAC_CC_DISCONNECT)
		{
			rc = cc_process_disconnect(irl_ptr, cc_ptr);
		}
		else if (opcode == FAC_CC_REDIRECT_TO_SDA)
		{
			rc = cc_process_redirect(irl_ptr, cc_ptr, p);
		}
		else
		{
			DEBUG_TRACE("irl_cc_process: unsupported opcode 0x%x\b", opcode);
		}
	}

	return rc;
}

int irlEnable_ConnectionControl(unsigned long irl_handle)
{
	struct irl_setting_t 					* irl_ptr = (struct irl_setting_t *) irl_handle;
	int 							rc = IRL_BUSY;
	struct irl_facility_handle_t	* fac_ptr;
	if (gIrlConnectionControl == NULL)
	{
		rc = irl_malloc(irl_ptr, sizeof(struct irl_connection_control_t), (void **)&gIrlConnectionControl);
		if (rc != IRL_SUCCESS)
		{
			goto _ret;
		}
		if (gIrlConnectionControl == NULL)
		{
			rc = IRL_MALLOC_ERR;
			goto _ret;
		}

		gIrlConnectionControl->report_code = IRL_CC_NOT_REDIRECT;
		gIrlConnectionControl->server_url[0] = '\0';
		gIrlConnectionControl->security_code = SECURITY_PROTO_NONE;
		gIrlConnectionControl->facility_state = IRL_CC_STATE_REDIRECT_REPORT;


	}

	rc =  irl_add_facility_handle(irl_ptr, gIrlConnectionControl, &fac_ptr);
	if (rc == IRL_SUCCESS)
	{
		/*  Add Connection Control facility to the IRL setting facility list.
		 *  This is not added by application so we use gIrlConnectionControl as user data.
		 *  [IRL setting]--->[facility_list]--->[facility_data]        --->[user_data]
		 *  [IRL setting]--->[facility_list]--->[gIrlConnectionControl]--->[gIrlConnectionControl]
		 */
		gIrlConnectionControl->item = IRL_CONFIG_IP_ADDR;

		rc = irl_add_facility(irl_ptr, gIrlConnectionControl, E_MSG_FAC_CC_NUM, gIrlConnectionControl, irl_cc_process);
	}

_ret:
	return rc;
}
