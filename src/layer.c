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
#include <string.h>
#include "bele.h"
#include "irl_def.h"
#include "network_intf.h"
#include "os_intf.h"
#include "ei_security.h"
#include "ei_msg.h"
#include "ei_discover.h"
#include "irl_cc.h"
#include "layer.h"

extern int rci_process_function(IrlSetting_t * irl_ptr, IrlFacilityHandle_t * fac_ptr, struct e_packet * p);

int irl_add_facility_packet(IrlSetting_t * irl_ptr, IrlFacilityHandle_t * fac_ptr, uint8_t  * buf, unsigned length, uint8_t sec_coding)
{
    int     rc = IRL_SUCCESS;
    IrlFacilityPacket_t     * fac_pkt;

    rc = irl_malloc(irl_ptr, sizeof(IrlFacilityPacket_t), (void **)&fac_pkt);
    if (rc != IRL_SUCCESS)
    {
        goto _ret;
    }
    if (fac_pkt == NULL)
    {
        rc = IRL_MALLOC_ERR;
        goto _ret;
    }


    fac_pkt->active = TRUE;
    fac_pkt->packet.length = length;
//                      fac_ptr->packet.alloc_len = p->alloc_len-2;
      fac_pkt->packet.sec_coding = sec_coding;
//                      fac_ptr->packet.sec_cxn = p->sec_cxn;
//                      fac_ptr->packet.pre_len = p->pre_len +2;
      memcpy(fac_pkt->buffer, buf, length);
      fac_pkt->packet.buf = fac_pkt->buffer;

    fac_pkt->next = fac_ptr->packet;
    fac_ptr->packet = fac_pkt;
    fac_ptr->packet_count++;

    DEBUG_PRINTF("irl_add_facility_packet: facility =0x%x packet = 0x%x count = %d >>>\n", fac_ptr->facility_id, (unsigned)fac_pkt, fac_ptr->packet_count);
_ret:
    return rc;
}

int irl_del_facility_packet(IrlSetting_t * irl_ptr, IrlFacilityHandle_t * fac_ptr)
{
    int                             rc = IRL_SUCCESS;
    IrlFacilityPacket_t       * fac_pkt, * prev_pkt = NULL, * next_pkt;

    fac_pkt = fac_ptr->packet;

    while (fac_pkt != NULL)
    {
        next_pkt = fac_pkt->next;

        if (!fac_pkt->active)
        {
             rc = irl_free(irl_ptr, (void *)fac_pkt);
            if (rc == IRL_SUCCESS)
            {
                if (prev_pkt != NULL)
                {
                    prev_pkt->next = next_pkt;
                }
                else
                {
                    fac_ptr->packet = next_pkt;
                }
            }
            fac_ptr->packet_count--;
            DEBUG_PRINTF("irl_del_facility_packet: facility = 0x%x packet = 0x%x count = %d<<<\n", fac_ptr->facility_id, (unsigned)fac_pkt, fac_ptr->packet_count);
        }
        else
        {
            prev_pkt = fac_pkt;
        }
        fac_pkt = next_pkt;
    }

    return rc;
}

IrlFacilityPacket_t * irl_get_facility_packet(IrlSetting_t * irl_ptr, IrlFacilityHandle_t * fac_ptr)
{
     IrlFacilityPacket_t       * fac_pkt, * rc_pkt = NULL;

    for (fac_pkt = fac_ptr->packet; fac_pkt != NULL; fac_pkt = fac_pkt->next)
    {
        if (fac_pkt->active && (fac_pkt->next == NULL  || !fac_pkt->next->active))
        {
            rc_pkt = fac_pkt;
            break;
        }
    }

    return rc_pkt;
}

int irl_clean_facility_packet(IrlSetting_t * irl_ptr)
{
    int                             rc = IRL_SUCCESS;
    IrlFacilityHandle_t      * fac_ptr;
    IrlFacilityPacket_t       * fac_pkt, * next_pkt;

    for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        for (fac_pkt = fac_ptr->packet; fac_pkt != NULL; fac_pkt = next_pkt)
        {
            next_pkt = fac_pkt->next;

            rc = irl_free(irl_ptr, (void *)fac_pkt);
            if (rc != IRL_SUCCESS)
            {
                break;
            }

            fac_ptr->packet_count--;
        }
        fac_ptr->packet = NULL;
    }

    return rc;
}

static int msg_add_keepalive_param(IrlSetting_t * irl_ptr, uint8_t * buf, uint16_t type, uint16_t value)
{
	uint16_t 	msg_type, len=2, msg_value;
	int			rc;

    (void)irl_ptr;

	msg_type = TO_BE16(type);
	memcpy(&buf[0], &msg_type, sizeof msg_type);

	len = TO_BE16(2);
	memcpy(&buf[2], &len, sizeof len);

	msg_value = TO_BE16(value);
	memcpy((void *)&buf[4], (void *)&msg_value, sizeof msg_value);

	rc = 6;


	return rc; /* return count of bytes added to buffer */
}

void irl_set_edp_state(IrlSetting_t * irl_ptr, int state)
{
	irl_ptr->edp_state = state;
	irl_ptr->layer_state = IRL_LAYER_INIT;
	irl_ptr->config.id = 0;
}


unsigned irl_edp_init_config_ids[] = {
		IRL_CONFIG_SERVER_URL, IRL_CONFIG_PASSWORD, IRL_CONFIG_TX_KEEPALIVE, IRL_CONFIG_RX_KEEPALIVE, IRL_CONFIG_WAIT_COUNT,
};

int irl_edp_configuration_layer(IrlSetting_t * irl_ptr)
{
	int 		rc = IRL_SUCCESS;
	unsigned	config_id;
	IrlStatus_t	status;

	DEBUG_PRINTF("irl_edp_init: irl_edp_configuration_layer\n");
	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
		while(irl_ptr->config.id < (sizeof irl_edp_init_config_ids/ sizeof irl_edp_init_config_ids[0]))
		{
			config_id = irl_edp_init_config_ids[irl_ptr->config.id];

			DEBUG_PRINTF("--- getting config id %d\n", config_id);
			status = irl_get_config(irl_ptr, config_id, &irl_ptr->config.data[config_id]);

			if (status == IRL_STATUS_CONTINUE)
			{
				if (config_id != IRL_CONFIG_PASSWORD && irl_ptr->config.data[config_id] == NULL)
				{
					goto _param_err;
				}

				switch(config_id)
				{
				case IRL_CONFIG_TX_KEEPALIVE:
					if (GET_TX_KEEPALIVE(irl_ptr) < IRL_TX_INTERVAL_MIN || GET_TX_KEEPALIVE(irl_ptr) > IRL_TX_INTERVAL_MAX)
					{
						goto _param_err;
					}
					break;
				case IRL_CONFIG_RX_KEEPALIVE:
					if (GET_RX_KEEPALIVE(irl_ptr) < IRL_RX_INTERVAL_MIN || GET_RX_KEEPALIVE(irl_ptr) > IRL_RX_INTERVAL_MAX)
					{
						goto _param_err;
					}
					break;
				case IRL_CONFIG_WAIT_COUNT:
					if (GET_WAIT_COUNT(irl_ptr) < IRL_WAIT_COUNT_MIN || GET_WAIT_COUNT(irl_ptr) > IRL_WAIT_COUNT_MAX)
					{
_param_err:

						status = irl_error_status( irl_ptr->callback, config_id, IRL_INVALID_DATA);
						if (status == IRL_STATUS_CONTINUE)
						{
								goto _ret;
						}
						else
						{
							rc = IRL_INVALID_DATA;
							goto _ret;
						}
					}
					break;
				}

				irl_ptr->config.id++;
			}
			else if (status != IRL_STATUS_BUSY)
			{
				rc = IRL_CONFIG_ERR;
				goto _ret;
			}
			else
			{
				rc = IRL_BUSY;
				goto _ret;
			}

		}
		irl_ptr->layer_state = IRL_LAYER_CONNECT;
	}

	if (irl_ptr->layer_state == IRL_LAYER_CONNECT)
	{
		char * server_url;

		config_id = IRL_CONFIG_SERVER_URL;
		server_url = irl_ptr->config.data[config_id];

		DEBUG_PRINTF("--- connecting server = %s\n", server_url);
		rc = irl_connect_server(irl_ptr, server_url, IRL_MT_PORT);

		if (rc == IRL_BUSY)
		{
			rc = IRL_SUCCESS;
		}

		irl_set_edp_state(irl_ptr, IRL_COMMUNICATION_LAYER);
	}

_ret:
	return rc;

}

int irl_communication_layer(IrlSetting_t * irl_ptr)
{
	int				rc = IRL_SUCCESS;
//	int				send_status;
//	uint32_t		mswait = 2;
	struct e_packet pkt;
	uint32_t		version;
//	uint8_t			buffer[128];
	uint8_t			* buf;
	int				len;
	int				receive_status;



	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
		DEBUG_PRINTF("Communication layer\n");

		DEBUG_PRINTF("--- Send MT Version\n");
		/* Send the MT version message. */
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		pkt.length = sizeof version;

		version = TO_BE32(irl_ptr->edp_version);
		memcpy(pkt.buf, &version, 4);
		if (rc != IRL_SUCCESS)
		{
			goto _ret;
		}

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_VERSION);
		if (rc != IRL_SUCCESS)
		{
			/* either error */
			goto _ret;
		}

		irl_ptr->layer_state = IRL_COMMUNICATION_LAYER_VERSION;

	}

	if (irl_ptr->layer_state == IRL_COMMUNICATION_LAYER_VERSION)
	{
		DEBUG_PRINTF("--- waiting MT version response\n");
		irl_receive_init(irl_ptr);
		rc = irl_receive_packet(irl_ptr, &irl_ptr->data_packet);
		if (rc == IRL_STATUS_CONTINUE)
		{
			irl_ptr->layer_state = IRL_COMMUNICATION_LAYER_VERSION1;
		}
	}

	if (irl_ptr->layer_state == IRL_COMMUNICATION_LAYER_VERSION1)
	{
		rc = irl_receive_packet_status(irl_ptr, &receive_status);

		if (rc == IRL_SUCCESS && receive_status == IRL_NETWORK_BUFFER_COMPLETE)
		{
			DEBUG_PRINTF("--- receive Mt version\n");

			if (irl_ptr->data_packet.type != E_MSG_MT2_TYPE_VERSION_OK)
			{
				/*
				 * The received message is not acceptable. Return an error value
				 * appropriate to the message received.
				 */
				switch (irl_ptr->data_packet.type)
				{
					/* Expected MTv2 message types... */
					case E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP:
						if (irl_ptr->data_packet.buf[0] == 0x02) {
							rc = IRL_SERVER_OVERLOAD_ERR;
						}
						else {
							/* Assume a version error for all other values. */
							rc = IRL_BAD_VERSION_ERR;
						}
						break;
					case E_MSG_MT2_TYPE_VERSION_BAD:
						rc = IRL_BAD_VERSION_ERR;
						break;
					case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
						rc = IRL_SERVER_OVERLOAD_ERR;
						break;
					/* Unexpected/unknown MTv2 message types... */
			//		case E_MSG_MT2_TYPE_VERSION:
			//		case E_MSG_MT2_TYPE_LEGACY_EDP_VERSION:
			//		case E_MSG_MT2_TYPE_KA_RX_INTERVAL:
			//		case E_MSG_MT2_TYPE_KA_TX_INTERVAL:
			//		case E_MSG_MT2_TYPE_KA_WAIT:
			//		case E_MSG_MT2_TYPE_KA_KEEPALIVE:
			//		case E_MSG_MT2_TYPE_PAYLOAD:
					default:
						rc = IRL_INVALID_MESSAGE_ERR;
				}
			}
			else
			{
				irl_ptr->layer_state = IRL_COMMUNICATION_LAYER_KA_PARAMS;
			}
		}

		if (rc != IRL_SUCCESS)
		{
			/* mt version error. let's notify user.
			 *
			 * ignore error status callback return value since server
			 * will close the connection.
			 */
			irl_error_status(irl_ptr->callback, IRL_CONFIG_RECEIVE, rc);
			goto _ret;
		}
	}

	if (irl_ptr->layer_state == IRL_COMMUNICATION_LAYER_KA_PARAMS)
	{
		uint16_t	timeout;
		uint8_t		wait_count;

		DEBUG_PRINTF("--- send keepalive params \n");

		/* Send the MT version message. */
		irl_packet_init(&pkt);

		buf = pkt.buf = irl_ptr->send_packet.buffer;
//		pkt.alloc_len = sizeof(irl_ptr->send_packet.buffer);

		timeout = GET_RX_KEEPALIVE(irl_ptr);
		len = msg_add_keepalive_param(irl_ptr, buf, E_MSG_MT2_TYPE_KA_RX_INTERVAL, timeout);
		if (len < 0) goto _ret;
		buf += len;

		timeout = GET_TX_KEEPALIVE(irl_ptr);
		len = msg_add_keepalive_param(irl_ptr, buf, E_MSG_MT2_TYPE_KA_TX_INTERVAL, timeout);
		if (len < 0) goto _ret;
		buf += len;

		wait_count = GET_WAIT_COUNT(irl_ptr);
		len = msg_add_keepalive_param(irl_ptr, buf, E_MSG_MT2_TYPE_KA_WAIT, (uint16_t)wait_count);
		if (len < 0) goto _ret;
		buf += len;

		pkt.length = buf - pkt.buf;

		irl_ptr->send_packet.total_length = pkt.length;
		irl_ptr->send_packet.length = 0;

		rc = irl_send(irl_ptr, irl_ptr->connection.socket_fd, pkt.buf, pkt.length);
		if (rc > 0)
		{
			irl_ptr->send_packet.total_length -= rc;
			irl_ptr->send_packet.length += rc;
			rc = IRL_SUCCESS;
		}

		irl_set_edp_state(irl_ptr, IRL_INITIALIZATION_LAYER);

	}

_ret:
	return rc;
}

int irl_initialization_layer(IrlSetting_t * irl_ptr)
{
	int				rc = IRL_SUCCESS;
	struct e_packet pkt;
	uint32_t		version;
	//uint8_t			buffer[128];
	int				receive_status;

	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
		DEBUG_PRINTF("Initialization layer\n");
		DEBUG_PRINTF("--- send protocol version\n");
		/*
		 * Send the protocol version message.
		 */
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		pkt.length = sizeof(version);
		version = TO_BE32(IRL_PROTOCOL_VERSION);
		memcpy(&pkt.buf[0], &version, sizeof version);

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}
		irl_ptr->layer_state = IRL_LAYER_RECEIVE_PENDING;

	}

	if (irl_ptr->layer_state == IRL_LAYER_RECEIVE_PENDING)
	{
		irl_receive_init(irl_ptr);
		rc = irl_receive_packet(irl_ptr, &irl_ptr->data_packet);
        irl_ptr->layer_state = IRL_LAYER_RECEIVE_DATA;

		if (rc != IRL_SUCCESS)
		{
		    goto _ret;
		}

	}

	if (irl_ptr->layer_state == IRL_LAYER_RECEIVE_DATA)
	{
		rc = irl_receive_packet_status(irl_ptr, &receive_status);
		if (rc == IRL_SUCCESS && receive_status == IRL_NETWORK_BUFFER_COMPLETE)
		{
			DEBUG_PRINTF("--- receive protocol version\n");
			/*
			 * Process the response.
			 */
			if (irl_ptr->data_packet.length < 1) {
				/* An empty packet is an error. */
				rc = IRL_INVALID_MESSAGE_ERR;
				goto _ret;
			}
			/* MT version 1 reports server overload in the version response. */
		#if 0
			if (e_dp_mt_version == 1 && irl_ptr->data_packet.buf[0] == 2) {
				/* The server indicated an overload condition. */
				rc = IRL_BAD_VERSION_ERR;
				goto _ret;
			}
		#endif

			/* the protocol version number was not acceptable to the server.
			 *
			 * Tell the application. TO DO: IRL needs updated protocol version.
			 */
			if (irl_ptr->data_packet.buf[0] != 0)
			{
				rc = IRL_BAD_VERSION_ERR;
				irl_error_status(irl_ptr->callback, IRL_CONFIG_RECEIVE, rc);
				goto _ret;

			}
			irl_set_edp_state(irl_ptr, IRL_SECURITY_LAYER);
		}
	}

_ret:

	return rc;
}

int irl_security_layer(IrlSetting_t * irl_ptr)
{
#define URL_PREFIX	"en://"

	int				rc = IRL_SUCCESS;
//	int				send_status;
//	uint32_t		mswait = 2;
	struct e_packet pkt;
	//int				send_status;
	char 			* pwd = NULL;
	uint16_t		len, size;
	uint8_t * ptr;
	char *	url_prefix = URL_PREFIX;


	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
		DEBUG_PRINTF("Security layer\n");
		DEBUG_PRINTF("--- send security form\n");

		pwd = (char *)irl_ptr->config.data[IRL_CONFIG_PASSWORD];


		/* Send the identity verification form message. */
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		pkt.length = 2;
		pkt.buf[0] = SECURITY_OPER_IDENT_FORM;

		if (pwd != NULL)
		{
			pkt.buf[1] = (uint8_t)SECURITY_IDENT_FORM_PASSWORD;
		}
		else
		{
			pkt.buf[1] = (uint8_t)SECURITY_IDENT_FORM_SIMPLE;
		}
		irl_ptr->security_form = pkt.buf[1];

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}

		irl_ptr->layer_state = IRL_SECURITY_LAYER_DEVICE_ID;
	}

	if (irl_ptr->layer_state == IRL_SECURITY_LAYER_DEVICE_ID)
	{
		uint8_t	* device_id;

		DEBUG_PRINTF("--- send device ID\n");

		/* Send the device ID message. */
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		pkt.length = 1 + IRL_DEVICE_ID_LENGTH;
		pkt.buf[0] = SECURITY_OPER_DEVICE_ID;

		device_id = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_DEVICE_ID];

		memcpy(&pkt.buf[1], device_id, IRL_DEVICE_ID_LENGTH);
		if (rc != IRL_SUCCESS)
		{
			goto _ret;
		}
		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}
		irl_ptr->layer_state = IRL_SECURITY_LAYER_SERVER_URL;
	}

	if (irl_ptr->layer_state == IRL_SECURITY_LAYER_SERVER_URL)
	{
		char * server_url;

		DEBUG_PRINTF("--- send server url\n");
		/* Send the URL message. */
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		server_url = (char *)irl_ptr->config.data[IRL_CONFIG_SERVER_URL];

		len = strlen(server_url) + strlen(URL_PREFIX);

		pkt.length = 1 + sizeof(uint16_t) + len;
		pkt.buf[0] = SECURITY_OPER_URL;
		ptr = &pkt.buf[1];

		size = sizeof len;
		len = TO_BE16(len);

		memcpy(ptr, &len, size);
		ptr += size;

		len =  strlen(server_url);
		if (len > 0)
		{
			size = strlen(url_prefix);
			memcpy(ptr, url_prefix, size);

			ptr += size;
			memcpy(ptr, server_url, len);
		}
		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}
		if (irl_ptr->security_form == SECURITY_IDENT_FORM_PASSWORD)
		{
			irl_ptr->layer_state = IRL_SECURITY_LAYER_PASSWORD;
		}
		else
		{
			irl_set_edp_state(irl_ptr, IRL_DISCOVERY_LAYER);
		}
	}

	if (irl_ptr->layer_state == IRL_SECURITY_LAYER_PASSWORD)
	{
		DEBUG_PRINTF("--- send password\n");
		/* Send the password along in addition to the simple messages */
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		pkt.length = 1 + sizeof(uint16_t) + len;
		pkt.buf[0] = SECURITY_OPER_PASSWD;

		len = TO_BE16(strlen(pwd));

		size = sizeof len;

		memcpy(&pkt.buf[1], &len, size);

		if (len > 0)
		{
			memcpy(&pkt.buf[1+size], pwd, strlen(pwd));
		}

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}
		irl_set_edp_state(irl_ptr, IRL_DISCOVERY_LAYER);
	}

_ret:
	return rc;
}
int irl_get_security_code(IrlSetting_t * irl_ptr)
{
	int sec_coding = SECURITY_PROTO_NONE;

    (void) irl_ptr;
#if 0
		if (irl_ptr->security_form != SECURITY_IDENT_FORM_SIMPLE)
		{
			sec_coding = ;
		}
#endif
	return sec_coding;
}
int irl_discovery_layer(IrlSetting_t * irl_ptr)
{
	int				rc = IRL_SUCCESS;
//	int				send_status;
//	uint32_t		mswait = 2;
	struct e_packet pkt;
	//int				send_status;
	uint16_t		len, size;
	uint8_t			sec_coding = irl_get_security_code(irl_ptr);

	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
		uint8_t * vendor_id;

		DEBUG_PRINTF("Discovery layer\n");
		DEBUG_PRINTF("--- send vendor id\n");

		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

	#if 0
		if (irl_ptr->security_form != SECURITY_IDENT_FORM_SIMPLE)
		{
			sec_coding = ;
		}
	#endif

		pkt.buf[0] = sec_coding;
		pkt.buf[1] = DISC_OP_VENDOR_ID;
		vendor_id = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_VENDOR_ID];
		memcpy(&pkt.buf[2], vendor_id, IRL_VENDOR_ID_LENGTH);

		/* Send the message. */
		pkt.length = 2+(uint16_t)IRL_VENDOR_ID_LENGTH;

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}

		irl_ptr->layer_state = IRL_LAYER_DISCOVERY_DEVICE_TYPE;
	}

	if (irl_ptr->layer_state == IRL_LAYER_DISCOVERY_DEVICE_TYPE)
	{
		char * device_type;

		DEBUG_PRINTF("--- send device type\n");
		irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

		pkt.buf[0] = sec_coding;
		pkt.buf[1] = DISC_OP_DEVICETYPE;

		device_type = (char *)irl_ptr->config.data[IRL_CONFIG_DEVICE_TYPE];
		if (device_type == NULL)
		{
			len = 0;
		}
		else
		{
			len = TO_BE16(strlen(device_type));
		}

		size = sizeof len;
		memcpy(&pkt.buf[2], &len, size);

		len = strlen(device_type);
		if (len > 0)
		{
			memcpy(&pkt.buf[2+size], device_type, len);
		}

		/* Send the message. */
		pkt.length = 2 + size + len;

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}
		irl_ptr->layer_state = IRL_LAYER_DISCOVERY_FACILITY_INIT;
	}

	if (irl_ptr->layer_state == IRL_LAYER_DISCOVERY_FACILITY_INIT)
	{
		IrlFacilityHandle_t	* fac_ptr;

		if (irl_ptr->active_facility == NULL)
		{
			irl_ptr->active_facility = irl_ptr->facility_list;
		}

		fac_ptr = irl_ptr->active_facility;

		while (fac_ptr != NULL)
		{
			rc = fac_ptr->process_function(irl_ptr, fac_ptr, NULL);
			if (rc == IRL_BUSY)
			{
				goto _ret;
			}
			else if (rc != IRL_SUCCESS)
			{
				/* error */
				DEBUG_PRINTF("irl_discovery_layer: process facility returns error %d\n", rc);
				break;
			}

			fac_ptr->state = IRL_LAYER_RECEIVE_PENDING;
			irl_ptr->active_facility = fac_ptr->next;
			fac_ptr = irl_ptr->active_facility;

		}

		if (irl_ptr->active_facility == NULL)
		{
			irl_ptr->layer_state = IRL_LAYER_DISCOVERY_COMPLETE;
		}

	}

	if (irl_ptr->layer_state == IRL_LAYER_DISCOVERY_COMPLETE)
	{
		DEBUG_PRINTF("--- send complete\n");
		irl_packet_init(&pkt);

		pkt.buf = irl_ptr->send_packet.buffer + PKT_MT_LENGTH;
//		pkt.alloc_len = sizeof(irl_ptr->send_packet.buffer) - PKT_MT_LENGTH;
//		pkt.pre_len = PKT_MT_LENGTH;

		pkt.buf[0] = sec_coding;
		pkt.buf[1] = DISC_OP_INITCOMPLETE;
		pkt.length = 2;

		rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_PAYLOAD);
		if (rc != IRL_SUCCESS)
		{
			/* either error or send pending */
			goto _ret;
		}
		irl_set_edp_state(irl_ptr, IRL_FACILITY_LAYER);
	}

_ret:
	return rc;
}

int irl_facility_layer(IrlSetting_t * irl_ptr)
{
	int		rc = IRL_SUCCESS;
	int		ccode;
	int		receive_status;

	struct e_packet     * p;
    IrlFacilityPacket_t     * fac_pkt;
    IrlFacilityHandle_t     * fac_ptr;



	/* facility layer waits and processes packet from server.
	 *
	 * It initializes packet and waits for data from server.
	 * Once it gets a packet, it parses and mux to appropriate facility.
	 */
	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
	    /* initialize packet for incoming data */
		DEBUG_PRINTF("Facility_layer\n");
		irl_receive_init(irl_ptr);
		rc = irl_receive_packet(irl_ptr, &irl_ptr->data_packet);
		if (rc == IRL_STATUS_CONTINUE)
		{
			irl_ptr->layer_state = IRL_LAYER_RECEIVE_PENDING;
		}
	}

	if (irl_ptr->layer_state == IRL_LAYER_RECEIVE_PENDING)
	{
		uint8_t		sec_opcode;
		uint8_t		disc_opcode;

		uint16_t	                facility;

		/* wait for data */
		rc = irl_receive_packet_status(irl_ptr, &receive_status);
		if (rc == IRL_SUCCESS && receive_status == IRL_NETWORK_BUFFER_COMPLETE)
		{
			uint8_t			* ptr;
			unsigned	length;

			irl_ptr->layer_state = IRL_LAYER_INIT;

			p = &irl_ptr->data_packet;
			ptr = p->buf;
			length = p->length;

			if (p->type == E_MSG_MT2_TYPE_PAYLOAD)
			{

				if (p->length < PKT_OP_SECURITY)
				{
					/* just ignore this packet */
					DEBUG_PRINTF("irl_facility_layer: invalid length for security opcode. length received %d < %d min length\n", p->length, PKT_OP_FACILITY);
					goto _ret;
				}
				sec_opcode = *ptr;
				ptr++;
				length--;


				if (sec_opcode != SECURITY_PROTO_NONE)
				{
					/* handle security operation */
				}

//				p->buf += 1;
//				p->length -= 1;
//				p->alloc_len -= 1;
				p->sec_coding = sec_opcode;
//				p->sec_cxn = sc;

				if (length < (PKT_OP_DISCOVERY + PKT_OP_FACILITY))
				{
					/* just ignore this packet */
					DEBUG_PRINTF("irl_facility_layer: invalid length for discovery opcode. length received %d < %d min length\n",
												length, (PKT_OP_DISCOVERY + PKT_OP_FACILITY));
					goto _cont;
				}

				disc_opcode = *ptr;
				ptr++;
				length--;

				if (disc_opcode != DISC_OP_PAYLOAD)
				{
					/* just ignore this packet */
					DEBUG_PRINTF("irl_facility_layer: invalid discovery opcode (%d) received opcode = %d\n",
												DISC_OP_PAYLOAD, disc_opcode);
					goto _cont;
				}
//				p->buf += 1;
//				p->length -= 1;
//				p->alloc_len -= 1;
				facility = FROM_BE16(*((uint16_t *)ptr));
				ptr += sizeof(uint16_t);
				length -= sizeof(uint16_t);

				DEBUG_PRINTF("irl_facility_layer: receive data facility = 0x%04x, type = %d, length=%d\n",
											facility, p->type, length);

				/* fake RCI response */
				if (facility == E_MSG_FAC_RCI_NUM)
				{
					rc = rci_process_function(irl_ptr, NULL, p);
				}

				/* seach facility and pass the data to the facility */
				for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
				{
					if (fac_ptr->facility_id == facility)
					{
                        /* copy the packet to the local facility packet */
                        rc = irl_add_facility_packet(irl_ptr, fac_ptr, ptr, length, p->sec_coding);
                        if (rc == IRL_BUSY)
                        {
                            /* stop reading data from network */
                            irl_ptr->layer_state = IRL_LAYER_RECEIVE_PENDING;
                        }

#if 0
  						if (fac_ptr->state == IRL_LAYER_RECEIVE_PENDING)
						{
							fac_ptr->packet.length = length;
	//						fac_ptr->packet.alloc_len = p->alloc_len-2;
							fac_ptr->packet.sec_coding = p->sec_coding;
	//						fac_ptr->packet.sec_cxn = p->sec_cxn;
	//						fac_ptr->packet.pre_len = p->pre_len +2;
							memcpy(fac_ptr->buffer, ptr, length);
							fac_ptr->packet.buf = fac_ptr->buffer;
							if (rc == IRL_SUCCESS)
							    fac_ptr->state = IRL_LAYER_RECEIVE_DATA;


						}
						else
						{
							irl_ptr->layer_state = IRL_LAYER_RECEIVE_PENDING;
						}
#endif
						break;
					}
				}
			}
			else
			{
				DEBUG_PRINTF("irl_facility_layer: not MTv2 Payload\n");
			}

		}

		if (rc != IRL_SUCCESS && rc != IRL_BUSY)
		{
			/* some kind of error */
			goto _ret;
		}
	}

#if 0
		for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
		{
			if (fac_ptr->state == IRL_LAYER_RECEIVE_DATA)
			{
				rc = fac_ptr->process_function(irl_ptr, fac_ptr, &fac_ptr->packet);
				if (rc == IRL_SUCCESS)
				{
				    printf("facility_layer: receive pending\n");
					fac_ptr->state = IRL_LAYER_RECEIVE_PENDING;
				}
				else if (rc != IRL_BUSY)
				{
					/* error */
					DEBUG_PRINTF("irl_facility_layer: facility  (0x%04x) returns error %d\n", fac_ptr->facility_id, rc);
					break;
				}

			}
		}
#else
    for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        fac_pkt = irl_get_facility_packet(irl_ptr, fac_ptr);
        if (fac_pkt != NULL)
        {

            rc = fac_ptr->process_function(irl_ptr, fac_ptr, &fac_pkt->packet);
            if (rc != IRL_BUSY)
            {
                fac_pkt->active = FALSE;
            }

             if (rc != IRL_SUCCESS && rc != IRL_BUSY)
            {
                /* error */
                DEBUG_PRINTF("irl_facility_layer: facility  (0x%04x) returns error %d\n", fac_ptr->facility_id, rc);
                break;
            }
        }
    }

    for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        ccode = irl_del_facility_packet(irl_ptr, fac_ptr);
        if (ccode == IRL_STATUS_ERROR)
        {
            rc = ccode;
            break;
        }
    }

#endif

_cont:
	ccode = irl_send_rx_keepalive(irl_ptr);
	if (ccode == IRL_STATUS_ERROR)
	{
		rc = ccode;
	}

_ret:
	return rc;
}

