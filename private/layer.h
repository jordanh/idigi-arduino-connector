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
#include "idk_def.h"
//#include "bele.c"
#include "ei_security.h"
#include "ei_msg.h"
#include "ei_discover.h"


#define IDK_PROTOCOL_VERSION	0x120

/* Facility State */
enum {
	IDK_FACILITY_INIT,
	IDK_FACILITY_INIT_DONE,
	IDK_FACILITY_PROCESS,
	IDK_FACILITY_DONE
};

/* send and receive states */
enum {
	IDK_RECEIVE_LENGTH,
	IDK_RECEIVE_LENGTH_COMPLETE,
	IDK_RECEIVE_TYPE,
	IDK_RECEIVE_TYPE_COMPLETE,
	IDK_RECEIVE_DATA,
	IDK_RECEIVE_DATA_COMPLETE,
	IDK_RECEIVE_COMPLETE
};

static unsigned idk_edp_init_config_ids[] = {
		idk_base_server_url, idk_base_wait_count, idk_base_tx_keepalive, idk_base_rx_keepalive, idk_base_password,
};

static idk_base_request_t idk_edp_init_facility_ids[] = {
        idk_base_firmware_facility,
};

static size_t idk_facility_count = sizeof idk_edp_init_facility_ids/ sizeof idk_edp_init_facility_ids[0];

extern int rci_process_function(idk_data_t * idk_ptr, idk_facility_t * fac_ptr, idk_facility_packet_t * p);

static idk_status_t remove_facility_layer(idk_data_t * idk_ptr)
{
	idk_status_t rc = idk_success;
	idk_callback_status_t status;
	idk_base_request_t facility_id;

	while (idk_ptr->request_id < idk_facility_count)
	{
		facility_id = idk_edp_init_facility_ids[idk_ptr->request_id];
		status = del_facility_data(idk_ptr, facility_id);
		if (status == idk_callback_abort)
		{
			rc = idk_configuration_error;
			break;
		}
		else if (status == idk_callback_continue)
		{
			idk_ptr->request_id++;
		}
		else
		{
		/* TODO: need to wait a while before calling
		 * the callback again for BUSY return.
		 */
		}
	}

	return rc;
}

static int msg_add_keepalive_param(uint8_t * buf, uint16_t type, uint16_t value)
{
	uint16_t 	msg_type, len, msg_value;
	uint8_t		* ptr = buf;
	int			rc;
	size_t		size;

	msg_type = TO_BE16(type);
	size = sizeof msg_type;
	memcpy(ptr, &msg_type, size);
	ptr += size;

	size = sizeof len;
	len = TO_BE16(size);
	memcpy(ptr, &len, size);
	ptr += size;

	msg_value = TO_BE16(value);
	memcpy((void *)ptr, (void *)&msg_value, sizeof msg_value);

	rc = 6;


	return rc; /* return count of bytes added to buffer */
}

idk_status_t configuration_layer(idk_data_t * idk_ptr)
{
	idk_status_t rc = idk_success;
    idk_status_t err_code = idk_success;
	idk_request_t request_id;
	idk_callback_status_t status;
    void * data;
    size_t length;
    size_t request_count; 

	DEBUG_PRINTF("idk_edp_init: idk_edp_configuration_layer\n");
    request_count = (sizeof idk_edp_init_config_ids/ sizeof idk_edp_init_config_ids[0]);

	if (idk_ptr->layer_state == layer_init_state)
	{
        /* Call callback to get server url, wait count, tx keepalive, rx keepalive, & password at this layer.
         * Call error status callback if error is encountered (NULL data, invalid range, invalid size).
         * If all these configuration are successfully obtained, update layer_state to CONNECT state.
         */
		while(idk_ptr->request_id < request_count)
		{
			request_id.base_request = idk_edp_init_config_ids[idk_ptr->request_id];

			DEBUG_PRINTF("--- getting base request id %d\n", request_id.base_request);
			status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &data, &length);

			if (status == idk_callback_continue)
			{
				if (request_id.base_request != idk_base_password && data == NULL)
				{
                    err_code = idk_invalid_data;
					goto _param_err;
				}

				switch(request_id.base_request)
				{
                case idk_base_password:
                    idk_ptr->password = (char *)data;
                   break;
                case idk_base_server_url:
                    idk_ptr->server_url = (char *)data;
                    if (length == 0 || length > IDK_SERVER_URL_LENGTH)
                    {
                        err_code = idk_invalid_data_range;
                        goto _param_err;
                    }
                    break;
				case idk_base_tx_keepalive:
                    idk_ptr->tx_keepalive = (uint16_t *)data;

					if (*idk_ptr->tx_keepalive < IDK_TX_INTERVAL_MIN || *idk_ptr->tx_keepalive > IDK_TX_INTERVAL_MAX ||
                        length != sizeof(uint16_t))
					{
                        err_code = idk_invalid_data_range;
						goto _param_err;
					}
					break;
				case idk_base_rx_keepalive:
                    idk_ptr->rx_keepalive = (uint16_t *)data;
					if (*idk_ptr->rx_keepalive < IDK_RX_INTERVAL_MIN || *idk_ptr->rx_keepalive > IDK_RX_INTERVAL_MAX ||
                        length != sizeof(uint16_t))
					{
                        err_code = idk_invalid_data_range;
						goto _param_err;
					}
					break;
				case idk_base_wait_count:
					idk_ptr->wait_count = (uint8_t *)data;
					if (*idk_ptr->wait_count < IDK_WAIT_COUNT_MIN || *idk_ptr->wait_count > IDK_WAIT_COUNT_MAX ||
                        length != sizeof(uint8_t))
					{
_param_err:
						status = notify_error_status( idk_ptr->callback, idk_class_base, request_id, err_code);
                        
					}
					break;
				default:
					break;
				}

                if (err_code == idk_success)
                {   
    				idk_ptr->request_id++;
                }
			}

			if (status == idk_callback_abort)
			{
				rc = idk_configuration_error;
				goto _ret;
			}

		} /* while */

		/* List of facilities is defined in idk_edp_init_facility_ids[] table.
		 * Call the callback to see whether the facility is supported or not.
		 */
		while (idk_ptr->request_id >= request_count && idk_ptr->request_id < (request_count + idk_facility_count))
        {
            bool facility_enable;

            request_id.base_request = idk_edp_init_facility_ids [idk_ptr->request_id - request_count];

			status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, &facility_enable, &length);
			if (status == idk_callback_continue)
            {
				/* set the bit for supporting the facility */
                idk_ptr->facilities = 0x1 << (idk_ptr->request_id - request_count);
                idk_ptr->request_id++;
            }
			else if (status == idk_callback_abort)
            {
				rc = idk_configuration_error;
                goto _ret;
            }
			else if (status == idk_callback_busy)
			{
				goto _ret;
			}
            
        }

		if (idk_ptr->request_id == (request_count + idk_facility_count))
        {
    		idk_ptr->request_id = 0;
	    	idk_ptr->layer_state = layer_connect_state;
        }
	}

	if (idk_ptr->layer_state == layer_connect_state)
	{
		DEBUG_PRINTF("--- connecting server = %s\n", idk_ptr->server_url);
		status = net_connect_server(idk_ptr, idk_ptr->server_url, IDK_MT_PORT);

		if (status == idk_callback_continue)
		{
    		set_idk_state(idk_ptr, edp_communication_layer);
        }
	}

_ret:
	return rc;

}

static idk_callback_status_t discovery_facility_layer(idk_data_t * idk_ptr)
{
    idk_callback_status_t status;
    idk_base_request_t facility_id;

    while (idk_ptr->request_id < idk_facility_count)
    {


        if (idk_ptr->facilities & (0x1 << idk_ptr->request_id))
        {
        	facility_id = idk_edp_init_facility_ids[idk_ptr->request_id];

            switch(facility_id)
            {
            case idk_base_firmware_facility:
                status = firmware_discovery_layer(idk_ptr);
                break;
            default:
            	break;
            }
        }
        if (status == idk_callback_continue)
        {
            idk_ptr->request_id++;
        }
        else if (status == idk_callback_abort)
        {
            goto _ret;
        }
    }

    if (idk_ptr->request_id == idk_facility_count)
    {
        status = cc_discovery_layer(idk_ptr);
    }
    else
    {
        status = idk_callback_busy;
    }

_ret:
    return status;
}

idk_status_t communication_layer(idk_data_t * idk_ptr)
{
	idk_status_t rc = idk_success;
	uint32_t version;
	uint8_t	* buf;
	int	len;
	idk_packet_t * p;

	/* communitcation layer:
	 * 	1. sends MT version
	 * 	2. receives and validates MT version response
	 * 	3. sends tx, rx, & waitcount parameter
	 *
	 * 	When starts sending a packet (setup_send_packet), we need to exit and
	 * 	let idk_task to process and complete the send.
	 */
	if (idk_ptr->layer_state == layer_init_state)
	{

		DEBUG_PRINTF("Communication layer\n");
		DEBUG_PRINTF("--- Send MT Version\n");
		/* Send the MT version message. */

		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
		p->type = E_MSG_MT2_TYPE_VERSION;
		p->length = sizeof version;

		version = TO_BE32(IDK_MT_VERSION);
		memcpy(buf, &version, p->length);
		rc = net_enable_send_packet(idk_ptr);
		if (rc == idk_success)
        {
    		idk_ptr->layer_state = layer_communication_version_state;
        }

	}


	else if (idk_ptr->layer_state == layer_communication_version_state)
	{
		rc = net_get_receive_packet(idk_ptr, &p);

		if (rc == idk_success && p != NULL)
		{
			DEBUG_PRINTF("--- receive Mt version\n");

			buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

			if (p->type != E_MSG_MT2_TYPE_VERSION_OK)
			{
				/*
				 * The received message is not acceptable. Return an error value
				 * appropriate to the message received.
				 */
				switch (p->type)
				{
					/* Expected MTv2 message types... */
					case E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP:
						if (*buf == 0x02) {
							rc = idk_server_overload;
						}
						else {
							/* Assume a version error for all other values. */
							rc = idk_bad_version;
						}
						break;
					case E_MSG_MT2_TYPE_VERSION_BAD:
						rc = idk_bad_version;
						break;
					case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
						rc = idk_server_overload;
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
						rc = idk_invalid_packet;
				}
			}
			else
			{
				idk_ptr->layer_state = layer_communication_ka_params_state;
			}
		}

		if (rc != idk_success)
		{
			idk_request_t request_id;

			/* mt version error. let's notify user.
			 *
			 * ignore error status callback return value since server
			 * will close the connection.
			 */
			request_id.base_request = idk_base_receive;
			notify_error_status(idk_ptr->callback, idk_class_base, request_id, rc);
			goto _ret;
		}
	}

	else if (idk_ptr->layer_state == layer_communication_ka_params_state)
	{
		uint16_t	timeout;
		uint8_t		wait_count;

		DEBUG_PRINTF("--- send keepalive params \n");


		buf = idk_ptr->send_packet.ptr = idk_ptr->send_packet.buffer;

		timeout = *idk_ptr->rx_keepalive;
		len = msg_add_keepalive_param(buf, E_MSG_MT2_TYPE_KA_RX_INTERVAL, timeout);
		if (len < 0) goto _ret;
		buf += len;
		idk_ptr->send_packet.total_length = len;

		timeout = *idk_ptr->tx_keepalive;
		len = msg_add_keepalive_param(buf, E_MSG_MT2_TYPE_KA_TX_INTERVAL, timeout);
		if (len < 0) goto _ret;
		buf += len;
		idk_ptr->send_packet.total_length += len;

		wait_count = *idk_ptr->wait_count;
		len = msg_add_keepalive_param(buf, E_MSG_MT2_TYPE_KA_WAIT, (uint16_t)wait_count);
		if (len < 0) goto _ret;
		buf += len;
		idk_ptr->send_packet.total_length += len;

		idk_ptr->send_packet.length = 0;

		set_idk_state(idk_ptr, edp_initialization_layer);

	}

_ret:
	return rc;
}

idk_status_t initialization_layer(idk_data_t * idk_ptr)
{
	idk_status_t rc = idk_success;
	idk_packet_t * p;
	uint32_t version;
	uint8_t	* ptr;

	/* initialization layer:
	 * 1. sends protocol version.
	 * 2. receives and validates protocol version response
	 *
	 * 	When starts sending a packet (by calling setup_send_packet),
	 * 	we need to exit and let idk_task to process and complete the send.
	 */
	switch (idk_ptr->layer_state)
	{
	case layer_init_state:
		DEBUG_PRINTF("Initialization layer\n");
		DEBUG_PRINTF("--- send protocol version\n");
		/*
		 * Send the protocol version message.
		 */
		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
		p->length = sizeof(version);
		p->type = E_MSG_MT2_TYPE_PAYLOAD;

		version = TO_BE32(IDK_PROTOCOL_VERSION);
		memcpy(ptr, &version, sizeof version);

		rc = net_enable_send_packet(idk_ptr);
		idk_ptr->layer_state = layer_receive_data_state;
		break;

	case layer_receive_data_state:
		rc = net_get_receive_packet(idk_ptr, &p);
		if (rc == idk_success && p != NULL)
		{
			DEBUG_PRINTF("--- receive protocol version\n");
			/*
			 * Empty data packet
			 */
			if (p->length < 1)
			{
				idk_request_t request_id;

				request_id.base_request = idk_base_receive;
				if (notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_invalid_packet) == idk_callback_abort)
				{
					rc = idk_invalid_packet;
				}
				goto _ret;
			}

			/* Parse the version response.
			 * If the protocol version number was not acceptable to the server,
			 * tell the application. TO DO: IRL needs updated protocol version.
			 */
			ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
			if (*ptr != 0)
			{
				idk_request_t request_id;

				request_id.base_request = idk_base_receive;
				rc = idk_bad_version;
				notify_error_status(idk_ptr->callback, idk_class_base, request_id, rc);
				goto _ret;

			}
			set_idk_state(idk_ptr, edp_security_layer);
		}
		break;
	default:
		break;
	}

_ret:

	return rc;
}

idk_status_t security_layer(idk_data_t * idk_ptr)
{
#define URL_PREFIX	"en://"

	idk_status_t rc = idk_success;
	idk_packet_t * p;
	char * pwd = NULL;
	uint16_t len, size;
	uint8_t * ptr;
	char * url_prefix = URL_PREFIX;


	/* security layer:
	 * 1. sends identity verification form
	 * 2. sends device ID
	 * 3. sends server URL
	 * 4. sends password if identity verification form is PASSWORD identity
	 *
	 * 	When starts sending a packet (by calling setup_send_packet),
	 * 	we need to exit and let idk_task to process and complete the send.
	 */
	switch (idk_ptr->layer_state)
	{
	case layer_init_state:
		DEBUG_PRINTF("Security layer\n");
		DEBUG_PRINTF("--- send security form\n");

		/* get password.
		 * If password is NULL, we use SIMPLE FORM identity.
		 * Otherwise, use PASSWORD FORM identity.
		 */

		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

		*ptr++ = SECURITY_OPER_IDENT_FORM;

		if (pwd != NULL)
		{
			*ptr = (uint8_t)SECURITY_IDENT_FORM_PASSWORD;
		}
		else
		{
			*ptr = (uint8_t)SECURITY_IDENT_FORM_SIMPLE;
		}
		idk_ptr->security_form = *ptr++ ;
		p->type = E_MSG_MT2_TYPE_PAYLOAD;
		p->length = 2;

		rc = net_enable_send_packet(idk_ptr);

		idk_ptr->layer_state = layer_security_device_id_state;
		break;
	case layer_security_device_id_state:
	{
		uint8_t	* device_id;

		DEBUG_PRINTF("--- send device ID\n");

		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

		p->type = E_MSG_MT2_TYPE_PAYLOAD;
		p->length = 1 + IDK_DEVICE_ID_LENGTH;
		*ptr++ = SECURITY_OPER_DEVICE_ID;

		device_id = (uint8_t *)idk_ptr->device_id;

		memcpy(ptr, device_id, IDK_DEVICE_ID_LENGTH);
		rc = net_enable_send_packet(idk_ptr);
		idk_ptr->layer_state = layer_security_server_url_state;
		break;
	}
	case layer_security_server_url_state:
	{
		char * server_url;

		DEBUG_PRINTF("--- send server url\n");

		server_url = (char *)idk_ptr->server_url;

		len = strlen(server_url) + strlen(URL_PREFIX);

		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
		/*
		 * construct response packet
		 *  ------------------------------
		 * |   0    |  1 - 2 | 3 ...      |
		 *  ------------------------------
		 * | opcode | length | server URL |
		 *  ------------------------------
		*/
		p->type = E_MSG_MT2_TYPE_PAYLOAD;
		p->length = 1 + sizeof(uint16_t) + len; /* sizeof of opcode +  length + url length */
		*ptr = SECURITY_OPER_URL;
		ptr++;

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

		rc = net_enable_send_packet(idk_ptr);
		if (idk_ptr->security_form == SECURITY_IDENT_FORM_PASSWORD)
		{
			idk_ptr->layer_state = layer_security_password_state;
		}
		else
		{
			set_idk_state(idk_ptr, edp_discovery_layer);
		}
		break;
	}
	case layer_security_password_state:
		DEBUG_PRINTF("--- send password\n");
		/* Send the password along in addition to the simple messages */
		pwd = (char *)idk_ptr->password;

		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
		p->type = E_MSG_MT2_TYPE_PAYLOAD;

		/*
		 * construct response packet
		 *  ------------------------------
		 * |   0    |  1 - 2 | 3 ...      |
		 *  ------------------------------
		 * | opcode | length | password   |
		 *  ------------------------------
		*/
		p->length = 1 + sizeof(uint16_t) + len;
		*ptr = SECURITY_OPER_PASSWD;
		ptr++;

		len = TO_BE16(strlen(pwd));

		size = sizeof len;

		memcpy(ptr, &len, size);
		ptr += size;
		if (len > 0)
		{
			memcpy(ptr, pwd, strlen(pwd));
		}

		rc = net_enable_send_packet(idk_ptr);
		set_idk_state(idk_ptr, edp_discovery_layer);
		break;
	default:
		break;
	}

	return rc;
}

idk_status_t discovery_layer(idk_data_t * idk_ptr)
{
	idk_status_t rc = idk_success;
    idk_callback_status_t status;
	idk_packet_t * p;
	uint16_t len, size;
	uint8_t sec_coding = SECURITY_PROTO_NONE;
	uint8_t * ptr;

	/* discovery layer:
	 * 1. send vendor ID
	 * 2. send device type
	 * 3. call each facility process to send its own discovery layer
	 * 4. send discovery complete message.
	 *
	 * 	When starts sending a packet (by calling setup_send_packet),
	 * 	we need to exit and let idk_task to process and complete the send.
	 */
	switch (idk_ptr->layer_state)
	{
	case layer_init_state:
	{
		uint8_t * vendor_id;

		DEBUG_PRINTF("Discovery layer\n");
		DEBUG_PRINTF("--- send vendor id\n");

		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

		/*
		 * construct response packet
		 *  ------------------------------------
		 * |        0      |    1   |  2 - 5    |
		 *  ------------------------------------
		 * | coding scheme | opcode | vendor ID |
		 *  ------------------------------------
		*/
		*ptr++= sec_coding;
		*ptr++ = DISC_OP_VENDOR_ID;
		vendor_id = (uint8_t *)idk_ptr->vendor_id;
		memcpy(ptr, vendor_id, IDK_VENDOR_ID_LENGTH);
		ptr += IDK_VENDOR_ID_LENGTH;

		/* Send the message. */
		p->length = 2+(uint16_t)IDK_VENDOR_ID_LENGTH;
		p->type = E_MSG_MT2_TYPE_PAYLOAD;

		rc = net_enable_send_packet(idk_ptr);

		idk_ptr->layer_state = layer_discovery_device_type_state;
		break;
	}

	case layer_discovery_device_type_state:
	{
		char * device_type;

		DEBUG_PRINTF("--- send device type\n");
		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

		/*
		 * construct response packet
		 *  ---------------------------------------
		 * |        0      |    1   |  2 ...      |
		 *  --------------------------------------
		 * | coding scheme | opcode | Device type |
		 *  --------------------------------------
		*/
		*ptr++ = sec_coding;
		*ptr++ = DISC_OP_DEVICETYPE;

		device_type = (char *)idk_ptr->device_type;
		if (device_type == NULL)
		{
			len = 0;
		}
		else
		{
			len = TO_BE16(strlen(device_type));
		}

		size = sizeof len;
		memcpy(ptr, &len, size);
		ptr += size;

		len = strlen(device_type);
		if (len > 0)
		{
			memcpy(ptr, device_type, len);
			ptr += len;
		}

		/* Send the message. */
		p->length = 2 + size + len;
		p->type = E_MSG_MT2_TYPE_PAYLOAD;

		rc = net_enable_send_packet(idk_ptr);
		idk_ptr->layer_state = layer_discovery_facility_init_state;
		break;
	}

	case layer_discovery_facility_init_state:
	{
        status = discovery_facility_layer(idk_ptr);

        if (status == idk_callback_continue)
        {
			idk_ptr->layer_state = layer_discovery_complete_state;
        }
		else if (status == idk_callback_abort)
		{
            rc = idk_facility_init_error;
		}
		break;
	}

	case layer_discovery_complete_state:
		DEBUG_PRINTF("--- send complete\n");
		p = (idk_packet_t *)idk_ptr->send_packet.buffer;
		ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

		/*
		 * construct response packet
		 *  ------------------------
		 * |        0      |    1   |
		 *  ------------------------
		 * | coding scheme | opcode |
		 *  ------------------------
		*/
		*ptr++ = sec_coding;
		*ptr++ = DISC_OP_INITCOMPLETE;
		p->length = 2;
		p->type = E_MSG_MT2_TYPE_PAYLOAD;

		rc = net_enable_send_packet(idk_ptr);
		set_idk_state(idk_ptr, edp_facility_layer);
		break;
	default:
		break;
	}

	return rc;
}

idk_status_t facility_layer(idk_data_t * idk_ptr)
{
	idk_status_t rc = idk_success;
	int		ccode;
//	int		receive_status;

	idk_facility_packet_t * p;
//	idk_facility_packet_t		* fac_pkt;
	idk_facility_t * fac_ptr;



	/* facility layer waits and processes packet from server.
	 *
	 * It waits data from server.
	 * Once it gets a packet, it parses and mux to appropriate facility.
	 */
	if (idk_ptr->layer_state == layer_init_state)
	{
//		uint8_t		sec_opcode;
//		uint8_t		disc_opcode;

		uint16_t	facility;

		/* wait for data */
		rc = net_get_receive_packet(idk_ptr, (idk_packet_t **)&p);
		if (rc == idk_success && p != NULL)
		{
			uint8_t		* ptr;
			unsigned	length;

			ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));
			length = p->length;

			if (p->type == E_MSG_MT2_TYPE_PAYLOAD)
			{

				if (p->length < PKT_OP_SECURITY)
				{
					/* just ignore this packet */
					DEBUG_PRINTF("idk_facility_layer: invalid length for security opcode. length received %d < %d min length\n", p->length, PKT_OP_FACILITY);
					goto _ret;
				}

				if (p->sec_coding != SECURITY_PROTO_NONE)
				{
					/* handle security operation */
					DEBUG_PRINTF("idk_facility_layer: unsupported security\n");
				}

				if (length < (PKT_OP_DISCOVERY + PKT_OP_FACILITY))
				{
					/* just ignore this packet */
					DEBUG_PRINTF("idk_facility_layer: invalid length for discovery opcode. length received %d < %d min length\n",
												length, (PKT_OP_DISCOVERY + PKT_OP_FACILITY));
					goto _cont;
				}

				if (p->disc_payload != DISC_OP_PAYLOAD)
				{
					/* just ignore this packet */
					DEBUG_PRINTF("idk_facility_layer: invalid discovery opcode (%d) received opcode = %d\n",
												DISC_OP_PAYLOAD, p->disc_payload);
					goto _cont;
				}

				facility = FROM_BE16(p->facility);
				p->facility = facility;

				DEBUG_PRINTF("idk_facility_layer: receive data facility = 0x%04x, type = %d, length=%d\n",
											facility, p->type, length);

				/* fake RCI response */
				if (facility == E_MSG_FAC_RCI_NUM)
				{
					rc = rci_process_function(idk_ptr, NULL, p);
				}

				/* seach facility and pass the data to the facility */
				for (fac_ptr = idk_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
				{
					if (fac_ptr->facility_id == facility)
					{
						fac_ptr->packet = p;
						idk_ptr->active_facility = fac_ptr;
						idk_ptr->layer_state = layer_receive_data_state;
						break;
					}
				}
			}
			else
			{
				DEBUG_PRINTF("idk_facility_layer: not MTv2 Payload\n");
			}

		}

		if (rc != idk_success)
		{
			/* some kind of error */
			goto _ret;
		}
	}


	fac_ptr = idk_ptr->active_facility;
	if (fac_ptr != NULL && fac_ptr->packet != NULL)
	{

		rc = fac_ptr->process_cb(idk_ptr, fac_ptr);
		 if (rc != idk_success)
		{
			/* error */
			DEBUG_PRINTF("idk_facility_layer: facility  (0x%04x) returns error %d\n", fac_ptr->facility_id, rc);
		}
	    if (fac_ptr->packet == NULL)
        {
			idk_ptr->layer_state = layer_init_state;
        }

	}


_cont:
	ccode = net_send_rx_keepalive(idk_ptr);
	if (ccode != idk_success)
	{
		rc = ccode;
	}

_ret:
	return rc;
}

