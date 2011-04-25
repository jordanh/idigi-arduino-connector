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
//#include <malloc.h>
#include <string.h>

#include "bele.h"
#include "irl_api.h"
#include "os_intf.h"
#include "config_intf.h"
#include "network_intf.h"
#include "ei_security.h"
#include "ei_msg.h"
#include "fw_def.h"

#define IRL_FW_TARGET_LIST_MSG_INTERVAL				30 * IRL_MILLISECONDS /* time to send target list to keep download alive */

#define IRL_FW_INFO_REQUEST_OPCODE					0x01
#define IRL_FW_INFO_RESPONSE_OPCODE					0x02
#define IRL_FW_DOWNLOAD_REQUEST_OPCODE		0x03
#define IRL_FW_DOWNLOAD_RESPONSE_OPCODE		0x04
#define IRL_FW_BINARY_BLOCK_OPCODE					0x05
#define IRL_FW_BINARY_BLOCK_ACK_OPCODE			0x06
#define IRL_FW_DOWNLOAD_ABORT_OPCODE				0x07
#define IRL_FW_DOWNLOAD_COMPLETE_OPCODE		0x08
#define IRL_FW_DOWNLOAD_COMPLETE_RESPONSE_OPCODE	0x09
#define IRL_FW_TARGET_RESET_OPCODE									0x0a

struct irl_firmware_data_t {
	IrlSetting_t		* irl_ptr;
	int					idx;
	uint16_t			target_count;
	uint8_t			target;
	uint32_t			version;
	uint32_t			code_size;
	char				* description;
	char				* name_spec;
	uint32_t			ka_time;
	e_boolean_t	keepalive;

	struct e_packet 				packet;
	IrlFirmwareFacility_t	* facility_data;

};

struct irl_firmware_data_t	* gIrlFirmwareFacilityData = NULL;


typedef int (* irl_firmware_opcode_cb_t)(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);

struct irl_firmware_opcode_handle_t {
	uint8_t								opcode;
	irl_firmware_opcode_cb_t	callback;
};

static int process_fw_info_request(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);
static int process_fw_download_request(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);
static int process_fw_binary_block(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);
static int process_fw_abort(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);
static int process_fw_complete(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);
static int process_fw_info_request(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);
static int process_target_reset(struct irl_firmware_data_t * fw_ptr, struct e_packet * p);

struct irl_firmware_opcode_handle_t gIrlFirmwareOpcodeHandle[] = {
	{IRL_FW_INFO_REQUEST_OPCODE,  				(irl_firmware_opcode_cb_t)process_fw_info_request},
	{IRL_FW_DOWNLOAD_REQUEST_OPCODE, 	(irl_firmware_opcode_cb_t)process_fw_download_request},
	{IRL_FW_BINARY_BLOCK_OPCODE,					(irl_firmware_opcode_cb_t)process_fw_binary_block},
	{IRL_FW_DOWNLOAD_ABORT_OPCODE, 		(irl_firmware_opcode_cb_t)process_fw_abort},
	{IRL_FW_DOWNLOAD_COMPLETE_OPCODE, 	(irl_firmware_opcode_cb_t)process_fw_complete},
	{IRL_FW_TARGET_RESET_OPCODE,				(irl_firmware_opcode_cb_t)process_target_reset}
};

unsigned gIrlFirmwareOpcodeHandleCount = sizeof gIrlFirmwareOpcodeHandle/ sizeof gIrlFirmwareOpcodeHandle[0];
uint32_t gFwTimeout = 0;

static IrlStatus_t get_fw_config(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback,
							  unsigned command, IrlFirmwareReq_t  * request, IrlFirmwareRsp_t * response)
{
	IrlStatus_t	status;
//    unsigned 	actual_set;
	//int			sent_status = IRL_NETWORK_BUFFER_COMPLETE;
	//int 		receive_status;
//	e_boolean_t is_packet;
	unsigned	timeout;
	uint32_t		time_stamp, time_stamp1;
	int				ecode;
	uint32_t		rx_keepalive;
	uint32_t		tx_keepalive;
	IrlSetting_t	* irl_ptr = fw_ptr->irl_ptr;

//	struct e_packet	pkt;

	if (irl_get_system_time(irl_ptr, &time_stamp) != IRL_SUCCESS)
	{
		status = IRL_STATUS_ERROR;
		goto _ret;
	}

	rx_keepalive = (GET_RX_KEEPALIVE(irl_ptr) * IRL_MILLISECONDS) - (time_stamp - irl_ptr->rx_ka_time);
	tx_keepalive = (GET_TX_KEEPALIVE(irl_ptr) * IRL_MILLISECONDS) - (time_stamp - irl_ptr->tx_ka_time);

//	do
//	{
		ecode = IRL_CONFIG_ERR;


		timeout = IRL_MIN( rx_keepalive, tx_keepalive);

		if (fw_ptr->ka_time > 0)
		{
			/* when starts downloading, we need to send target list on every
			 * IRL_FW_TARGET_LIST_MSG_INTERVAL second. This ka_time is
			 * set when server sends IRL_FW_DOWNLOAD_COMPLETE_OPCODE.
			 *
			 * see this is min timeout value to the callback.
			 *
			 */
			timeout = IRL_MIN( timeout, IRL_FW_TARGET_LIST_MSG_INTERVAL - (time_stamp - fw_ptr->ka_time));

		}


		if (gFwTimeout != timeout)
		{
		    printf("fw timeout: %d\n", timeout);
		    gFwTimeout = timeout;
		}

		status = callback(command, request, response, timeout/IRL_MILLISECONDS);

		if (irl_get_system_time(irl_ptr, &time_stamp1) != IRL_SUCCESS)
		{
			status = IRL_STATUS_ERROR;
			goto _ret;
		}

		if (status == IRL_STATUS_BUSY && fw_ptr->ka_time > 0)
		{
			/*
			 * Check whether we need to send target list message
			 * to keep server alive.
			 */
			fw_ptr->keepalive = ((time_stamp1 - fw_ptr->ka_time) >= IRL_FW_TARGET_LIST_MSG_INTERVAL);
		}
		else
		{
			fw_ptr->keepalive = FALSE;
		}

		if ((time_stamp1- time_stamp) > timeout)
		{
			DEBUG_PRINTF("get_fw_config: callback exceeds timeout (%d - %d) > %d\n", (int)time_stamp1, (int)time_stamp, (int)timeout);
			ecode = IRL_TIMEOUT_ERR;
			status = irl_error_status(irl_ptr->callback, command, ecode);

			if (status != IRL_STATUS_CONTINUE)
			{
				status = IRL_STATUS_ERROR;
			}

		}

#if 0
		status = irl_error_status(irl_ptr->callback, command, ecode);

		if (status == IRL_STATUS_CONTINUE)
		{
			if (irl_ptr->connection.socket_fd >= 0)
			{
				irl_packet_init(&pkt);

				if (irl_select(irl_ptr, (irl_network_read_t_SET|IRL_NETWORK_TIMEOUT_SET|IRL_NETWORK_CALLBACK_SET), &actual_set) != IRL_SUCCESS)
				{
					status = IRL_STATUS_ERROR;
					break;
				}

				/* receive set */
				if (IRL_IS_SELECT_SET(actual_set, irl_network_read_t_SET))
				{
					/* just want to make sure it receives keepalive packet.
					 * Discard all other messages.
					 */
					if (irl_receive_packet_data(irl_ptr, &pkt, &is_packet) != IRL_SUCCESS)
					{
						status = IRL_STATUS_ERROR;
						break;
					}
#if 0
					if (irl_receive_packet_status(irl_ptr, &receive_status) != IRL_SUCCESS)
					{
						status = IRL_STATUS_ERROR;
						break;
					}
#endif
				}

#if 0
				/* send set */
				if (IRL_IS_SELECT_SET(actual_set, irl_network_write_t_SET))
				{
					/* send pending data */
					if (irl_send_packet_status(irl_ptr, &sent_status) != IRL_SUCCESS)
					{
						status = IRL_STATUS_ERROR;
						break;
					}
					if (sent_status == IRL_NETWORK_BUFFER_PENDING)
					{
					}
				}
#endif
				/* timeout */
//				if (IRL_IS_SELECT_SET(actual_set, IRL_NETWORK_TIMEOUT_SET) &&
//					sent_status == IRL_NETWORK_BUFFER_COMPLETE)

				if (IRL_IS_SELECT_SET(actual_set, IRL_NETWORK_TIMEOUT_SET))
				{
					/* handle rx_keepalive */
					if (irl_send_rx_keepalive(irl_ptr) != IRL_SUCCESS)
					{
						status = IRL_STATUS_ERROR;
						break;
					}
				}
			}
		}
#endif
//	} while (status == IRL_STATUS_CONTINUE);

_ret:
	if (status != IRL_STATUS_CONTINUE)
	{
		DEBUG_PRINTF(">>>> get_fw_config: config_id = %d status = %d\n", command, status);
	}
	return status;
}


static IrlStatus_t get_fw_target_count(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback, uint16_t * count)
{
	unsigned		config_id;
	IrlStatus_t 		status = IRL_STATUS_CONTINUE;
	IrlFirmwareRsp_t rsp;


    DEBUG_PRINTF("--- get FW target count\n");
	config_id = IRL_FA_TARGET_COUNT;

	status = get_fw_config(fw_ptr, callback, config_id, NULL, &rsp);

	if (status == IRL_STATUS_CONTINUE)
	{
		*count = rsp.target_count;
		DEBUG_PRINTF("irl_get_fw_target_count: target count = %d\n", *count);
	}

	return status;
}

static IrlStatus_t get_fw_version(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback, uint8_t target, uint32_t * version)
{
	unsigned				config_id;
	IrlStatus_t 				status = IRL_STATUS_CONTINUE;
	IrlFirmwareRsp_t	rsp;
	IrlFirmwareReq_t	req;

	DEBUG_PRINTF("--- get firmware target version\n");

	config_id = IRL_FA_VERSION;
	req.target = target;
	status = get_fw_config(fw_ptr, callback, config_id, &req, &rsp);

	if (status == IRL_STATUS_CONTINUE)
	{
		*version = rsp.target_version;
		DEBUG_PRINTF("irl_get_fw_version: target version = 0x%x\n", (unsigned)*version);
	}

	return status;
}

static IrlStatus_t get_fw_code_size(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback, uint8_t target, uint32_t * size)
{
	unsigned				config_id;
	IrlStatus_t 				status = IRL_STATUS_CONTINUE;
	IrlFirmwareRsp_t	rsp;
	IrlFirmwareReq_t	req;

	DEBUG_PRINTF("--- get firmware target code size\n");
	config_id = IRL_FA_CODE_SIZE;
	req.target = target;
	status = get_fw_config(fw_ptr, callback, config_id, &req, &rsp);

	if (status == IRL_STATUS_CONTINUE)
	{
		*size = rsp.avail_size;
		DEBUG_PRINTF("get_fw_code_size: available code size = %d\n", (unsigned)*size);
	}

	return status;
}

static IrlStatus_t get_fw_description(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback, uint8_t target, char ** desc)
{
	unsigned				config_id;
	IrlStatus_t 				status = IRL_STATUS_CONTINUE;
	IrlFirmwareRsp_t	rsp;
	IrlFirmwareReq_t	req;

	DEBUG_PRINTF("--- get firmware target description\n");
	config_id = IRL_FA_DESCRIPTION;
	req.target = target;
	status = get_fw_config(fw_ptr, callback, config_id, &req, &rsp);

	if (status == IRL_STATUS_CONTINUE)
	{
		*desc = rsp.desc_string;
		DEBUG_PRINTF("get_fw_description: description = %s\n", *desc);
	}

	return status;
}

static IrlStatus_t get_fw_name_spec(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback, uint8_t target, char ** spec)
{
	unsigned				config_id;
	IrlStatus_t 				status = IRL_STATUS_CONTINUE;
	IrlFirmwareRsp_t	rsp;
	IrlFirmwareReq_t	req;

	DEBUG_PRINTF("--- get firmware target filename spec\n");
	config_id = IRL_FA_FILE_NAME_SPEC;
	req.target = target;
	status = get_fw_config(fw_ptr, callback, config_id, &req, &rsp);

	if (status == IRL_STATUS_CONTINUE)
	{
		*spec = rsp.file_name_spec;
		DEBUG_PRINTF("get_fw_name_spec: file name spec = %s\n", *spec);
	}

	return status;
}

int fw_send(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 	rc;

	rc = irl_send_facility_layer(fw_ptr->irl_ptr, p, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);

	return rc;
}

static int fw_discovery_layer_init( struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t facility_cb)
{
	IrlSetting_t * irl_ptr = fw_ptr->irl_ptr;;
	int 		rc = IRL_SUCCESS;
	uint32_t		version;
	uint8_t			* ptr;
	IrlStatus_t		status;


	DEBUG_PRINTF("fw_discovery_layer_init: sends target list info\n");

	if (fw_ptr->idx == 0)
	{
		if (fw_ptr->target_count == 0)
		{
			status = get_fw_target_count(fw_ptr, facility_cb, &fw_ptr->target_count);
			if (status == IRL_STATUS_ERROR)
			{
				rc = IRL_CONFIG_ERR;
				goto _ret;
			}

			else if (status == IRL_STATUS_BUSY)
			{
				rc = IRL_BUSY;
				goto _ret;
			}
			else if (fw_ptr->target_count == 0)
			{
				goto _ret;
			}
		}

		irl_send_packet_init(irl_ptr, &fw_ptr->packet, PKT_PRE_FACILITY);

#if 0
		pkt.buf = irl_ptr->send_packet.buffer + PKT_PRE_FAC_FU;
		pkt.alloc_len = sizeof(irl_ptr->send_packet.buffer) - PKT_PRE_FAC_FU;
		pkt.pre_len = PKT_PRE_FAC_FU;
#endif

		/* Future: add security coding process */


		ptr = fw_ptr->packet.buf;
		/***********************************************************************/

		/* if no new boot code, go ahead and send the target list msg */
		*ptr++ = E_FAC_FU_OP_LIST_TARGETS;

		fw_ptr->packet.length = 1;

		fw_ptr->idx++;
	}

	if (fw_ptr->idx == 1)
	{
		ptr = fw_ptr->packet.buf + fw_ptr->packet.length;

		while (fw_ptr->target < fw_ptr->target_count)
		{
			uint32_t ver;

			/* get the current firmware version for this target */
			status = get_fw_version(fw_ptr, facility_cb, fw_ptr->target, &ver);
			if (status == IRL_STATUS_ERROR)
			{
				rc = IRL_CONFIG_ERR;
				goto _ret;
			}
			else if (status == IRL_STATUS_BUSY)
			{
				rc = IRL_BUSY;
				goto _ret;
			}


			/* now add this target to the target list message */
			*ptr++ = fw_ptr->target;  /* target number */
			version = TO_BE32(ver);
			memcpy((void *)ptr, (void *)&version, sizeof version);
			ptr += sizeof version; /* sizeof uint32_t */

			fw_ptr->packet.length += 5; /* size of one target's info */
			fw_ptr->target++;
		}

		if (fw_ptr->target == fw_ptr->target_count)
		{
			rc = fw_send(fw_ptr, &fw_ptr->packet);
			fw_ptr->idx = 0;
			fw_ptr->target = 0;
		}
	}


_ret:
	return rc;
}

static int send_fw_abort(struct irl_firmware_data_t * fw_ptr, uint8_t target, uint8_t status)
{
	int 	rc;
	uint8_t	* ptr;
	uint8_t	status_code = status;
	struct e_packet	pkt;
	IrlSetting_t	* irl_ptr = fw_ptr->irl_ptr;

	/* send firmware info request
	 *  ---------------------------------------------------
	 * |   0    |    1   |  2 - 5  |  2 - 9   |  10 ...    |
	 *  ---------------------------------------------------
	 * | opcode | target | version | Available | Firmware  |
 	 * |        |        |         | code size | ID string |
 	 *  --------------------------------------------------
 	 *
 	 *  Firmware ID string: [descr]0xa[file name spec]
 	 */
	irl_send_packet_init(irl_ptr, &pkt, PKT_PRE_FACILITY);

	if (status < IRL_FA_USER_ABORT_ERR || status > IRL_FA_HARWARE_ERR)
	{
		status_code = IRL_FA_DEVICE_ERR;
	}
	/* now add this target to the target list message */
	ptr = pkt.buf;
	*ptr++ = IRL_FW_DOWNLOAD_ABORT_OPCODE;
	*ptr++ = target;
	*ptr++ = status_code - IRL_FA_USER_ABORT_ERR;  /* adjust the Firmware download abort status code */

	pkt.length = 3;

	rc = fw_send(fw_ptr, &pkt);

	return rc;
}

typedef IrlStatus_t (* irl_fw_callback_t)(struct irl_firmware_data_t * fw_ptr, IrlFimwareFacilityCb_t callback, uint8_t target, void * data);

irl_fw_callback_t	gIrlFwConfigFunction[] = {
		(irl_fw_callback_t)get_fw_version, (irl_fw_callback_t)get_fw_code_size,
		(irl_fw_callback_t)get_fw_description, (irl_fw_callback_t)get_fw_name_spec
};

static int process_fw_info_request(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 				rc = IRL_BUSY;
	IrlStatus_t			status;
	uint8_t				target;
	uint32_t			value;
	unsigned			id_string_length = 0;
	uint8_t				* ptr;
	struct e_packet		pkt;
	IrlSetting_t		* irl_ptr = fw_ptr->irl_ptr;
	int					i;
	void				* data[] = {(void *)&fw_ptr->version, (void *)&fw_ptr->code_size, (void *)&fw_ptr->description, (void *)&fw_ptr->name_spec};

	DEBUG_PRINTF("process_fw_info_request...\n");
//	opcode = p->buf[0]
	target = p->buf[1];

	while (fw_ptr->idx < (int)(sizeof gIrlFwConfigFunction/ sizeof gIrlFwConfigFunction[0]))
	{
		i = fw_ptr->idx;
		/* get the current firmware version for this target */
		status = gIrlFwConfigFunction[i](fw_ptr, fw_ptr->facility_data->callback, target, data[i]);

		if (status == IRL_STATUS_ERROR)
		{
			fw_ptr->idx = 0;
			rc = IRL_CONFIG_ERR;
			goto _ret;
		}
		else if (status == IRL_STATUS_BUSY)
		{
			goto _ret;
		}
		else if (status == IRL_STATUS_ERROR)
		{
			fw_ptr->idx = 0;
			rc = IRL_SUCCESS;
			goto _ret;
		}
		fw_ptr->idx++;
	}

	if (fw_ptr->idx == (sizeof gIrlFwConfigFunction/ sizeof gIrlFwConfigFunction[0]))
	{

		if (fw_ptr->description != NULL)
		{
			id_string_length += strlen(fw_ptr->description);
		}
		if (fw_ptr->name_spec != NULL)
		{
			id_string_length += strlen(fw_ptr->description);
		}

		if (id_string_length > (IRL_FA_ID_STRING_LENGTH-1))
		{

			status = irl_error_status(irl_ptr->callback, IRL_FA_DESCRIPTION, IRL_INVALID_DATA_LENGTH);
			if (status == IRL_STATUS_ERROR)
			{
				rc = IRL_CONFIG_ERR;
			}
			else if (status == IRL_STATUS_ERROR)
			{
				rc = IRL_SUCCESS;
			}
			goto _ret;
		}

		/* send firmware info request
		 *  ---------------------------------------------------
		 * |   0    |    1   |  2 - 5  |  2 - 9   |  10 ...    |
		 *  ---------------------------------------------------
		 * | opcode | target | version | Available | Firmware  |
	 	 * |        |        |         | code size | ID string |
	 	 *  --------------------------------------------------
	 	 *
	 	 *  Firmware ID string: [descr]0xa[file name spec]
	 	 */
		irl_send_packet_init(irl_ptr, &pkt, PKT_PRE_FACILITY);

		/* now add this target to the target list message */
		ptr = pkt.buf;
		*ptr++ = IRL_FW_INFO_RESPONSE_OPCODE;
		*ptr++ = target;  /* target number */

		value = TO_BE32(fw_ptr->version);
		memcpy((void *)ptr, (void *)&value, sizeof value);
		ptr += sizeof value; /* sizeof uint32_t */

		value = TO_BE32(fw_ptr->code_size);
		memcpy((void *)ptr, (void *)&value, sizeof value);
		ptr += sizeof value; /* sizeof uint32_t */

		if (fw_ptr->description != NULL)
		{
			memcpy((void *)ptr, (void *)fw_ptr->description, strlen(fw_ptr->description));
			ptr += strlen(fw_ptr->description);
		}

		*ptr++ = 0x0a;

		if (fw_ptr->name_spec != NULL)
		{
			memcpy((void *)ptr, (void *)fw_ptr->name_spec, strlen(fw_ptr->name_spec));
			ptr += strlen(fw_ptr->name_spec);
		}

		*ptr++ = 0x00;

		pkt.length = ptr - pkt.buf;

		fw_ptr->idx = 0; /* reset back to original state */

		rc = fw_send(fw_ptr, &pkt);
	}


_ret:
	return rc;
}

static int process_fw_download_request(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 				rc = IRL_SUCCESS;
	IrlStatus_t			status;
	uint32_t			value;
	IrlFirmwareReq_t	request;
	IrlFirmwareRsp_t	response;
	IrlSetting_t			* irl_ptr = fw_ptr->irl_ptr;
	struct e_packet		pkt;
	int					i;
	uint8_t				* buf;
	uint16_t			length;
	uint8_t 			* ptr;


	DEBUG_PRINTF("Process Firmware Download request...\n");
	request.download_request.file_name_spec = NULL;
	request.download_request.filename = NULL;
	request.download_request.desc_string = NULL;

	/* firmware download request format
	 *  ------------------------------------------------------------------------------
	 * |     0      |     1     |   2 - 5    |    6 - 9     |  10...                       |
	 *  ------------------------------------------------------------------------------
	 * | opcode | target | version | code size | firmware ID string |
	 *  -------------------------------------------------------------------------------
	 *
	 *  Firmware ID string: [label]0x0a[file name spec]0xa[file name]
	 */
	buf = p->buf + 1;
	length = p->length-1;

	request.download_request.target = *buf;
	length--;
	buf++;

	value = *((uint32_t *)buf);
	request.download_request.version = FROM_BE32(value);
	length -= sizeof value;
	buf += sizeof value;

	value = *((uint32_t *)buf);
	request.download_request.code_size = FROM_BE32(value);
	length -= sizeof value;
	buf += sizeof value;

	if (*buf != 0x0a && *buf != 0x00)
	{
		request.download_request.desc_string = (char *)buf;
	}

	for (i=0; i < p->length; i++)
	{
		if (buf[i] == 0x0a || buf[i] == 0x00)
		{
			buf[i] = 0x00;
			break;
		}
	}
	length -= (i+1);
	buf += (i+1);


	if (*buf != 0x0a && * buf != 0x00)
	{
		request.download_request.file_name_spec = (char *)buf;
	}
	for (i=0; i < p->length; i++)
	{
		if (buf[i] == 0x0a || buf[i] == 0x00)
		{
			buf[i] = 0x00;
			break;
		}
	}
	length -= (i+1);
	buf += (i+1);

	if (*buf != 0x0a && *buf != 0x00)
	{
		request.download_request.filename = (char *)buf;
	}

	for (i=0; i < length; i++)
	{
		if (buf[i] == 0x0a || buf[i] == 0x00)
		{
			buf[i] = 0x00;
			break;
		}
	}

	status = get_fw_config(fw_ptr, fw_ptr->facility_data->callback, IRL_FA_DOWNLOAD_REQ, &request, &response);


	if (status == IRL_STATUS_ERROR)
	{
		if (response.error_status > IRL_FA_USER_ABORT_ERR)
		{
			send_fw_abort(fw_ptr, request.download_request.target, response.error_status);
		}
		else
		{
			goto _rsp;
		}
	}
	else if (status == IRL_STATUS_CONTINUE)
	{

		response.error_status = IRL_FA_SUCCESS;

_rsp:
		/* send firmware download response
		 *  ---------------------------------------------
		 * |     0       |     1    |     2                   |
		 *  ---------------------------------------------
		 * | opcode | target | response type |
		 *  --------------------------------------------
		 *
		 *  Firmware ID string: [descr]0xa[file name spec]
		 */

		irl_send_packet_init(irl_ptr, &pkt, PKT_PRE_FACILITY);

		/* now add this target to the target list message */
		ptr = pkt.buf;
		*ptr++ = IRL_FW_DOWNLOAD_RESPONSE_OPCODE;
		*ptr++ = request.download_request.target;  /* target number */
		*ptr++ = response.error_status;

		pkt.length = ptr - pkt.buf;
		rc = fw_send(fw_ptr, &pkt);
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}
	else
	{
		rc = IRL_DOWNLOAD_ERR;
	}

	return rc;
}

static int process_fw_binary_block(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 				rc = IRL_SUCCESS;
	IrlStatus_t			status;
	uint32_t			value;
	uint8_t				ack_required;
	IrlFirmwareReq_t	request;
	IrlFirmwareRsp_t	response;
	struct e_packet		pkt;
	IrlSetting_t * irl_ptr = fw_ptr->irl_ptr;
	uint8_t 			* buf;
	uint16_t			length;

	DEBUG_PRINTF("Process Firmware Binary Block...\n");

	/* firmware binary block
	 *  ------------------------------------------------------------------------
	 * |     0      |     1     |           2          |  3 - 6  |   7...             |
	 *  ------------------------------------------------------------------------
	 * | opcode | target | Ack required | offset | binary data |
	 *  ------------------------------------------------------------------------
	 *
	 */

	buf = p->buf+1;
	length = p->length-1;

	request.image_data.target = *buf;
	length--;
	buf++;

	ack_required = *buf;
	length--;
	buf++;

	value = *((uint32_t *)buf);
	request.image_data.offset = FROM_BE32(value);
	length -= sizeof value;
	buf += sizeof value;

	request.image_data.data = buf;
	request.image_data.length = length;

	status = get_fw_config(fw_ptr, fw_ptr->facility_data->callback, IRL_FA_DOWNLOAD_DATA, &request, &response);


	if (status == IRL_STATUS_CONTINUE)
	{

		if(ack_required)
		{
			/* send firmware binary block acknowledge
			 *  ---------------------------------------------------
			 * |   0   	  |     1     | 2 - 5  |    6       |
			 *  -----------------------------------------------------
			 * | opcode | target | offset | status |
			 *  ----------------------------------------------------
			 *
			 *  Firmware ID string: [descr]0xa[file name spec]
			 */
			uint8_t * ptr;

			irl_send_packet_init(irl_ptr, &pkt, PKT_PRE_FACILITY);

			/* now add this target to the target list message */
			ptr = pkt.buf;
			*ptr++ = IRL_FW_BINARY_BLOCK_ACK_OPCODE;
			*ptr++ = request.image_data.target;  /* target number */
			value = TO_BE32(request.image_data.offset);
			memcpy(ptr, &value, sizeof value);
			ptr += sizeof value;

			*ptr++ = IRL_FA_SUCCESS;

			pkt.length = ptr - pkt.buf;

			rc = fw_send(fw_ptr, &pkt);
		}
	}
	else if (status == IRL_STATUS_ERROR)
	{

		send_fw_abort(fw_ptr, request.download_request.target, IRL_FA_DEVICE_ERR);
		if (status == IRL_STATUS_ERROR)
			rc = IRL_DOWNLOAD_ERR;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;
}

int process_fw_abort(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 				rc = IRL_SUCCESS;
	IrlStatus_t			status;
	IrlFirmwareReq_t	request;
	uint8_t				* buf;
	uint16_t			length;
	DEBUG_PRINTF("Process Firmware Abort...\n");

	/* firmware download abort
	 *  ----------------------------------
	 * |      0     |      1    |    2      |
	 *  ----------------------------------
	 * | opcode | target | status |
	 *  ----------------------------------
	 *
	 */
	buf = p->buf + 1;
	length = p->length-1;

	request.abort.target = *buf;
	length--;
	buf++;

	request.abort.status = *buf;
	length--;
	buf++;

	status = get_fw_config(fw_ptr, fw_ptr->facility_data->callback, IRL_FA_DOWNLOAD_ABORT, &request, NULL);

	if (status == IRL_STATUS_ERROR)
	{
		rc = IRL_DOWNLOAD_ERR;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;

}
int process_fw_complete(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 				rc = IRL_SUCCESS;
	IrlStatus_t			status;
	uint32_t			value;
	IrlFirmwareReq_t	request;
	IrlFirmwareRsp_t	response;
	struct e_packet		pkt;
	uint8_t				* buf;
	uint16_t			length;
	IrlSetting_t 		* irl_ptr = fw_ptr->irl_ptr;

	DEBUG_PRINTF("Process Firmware Download Complete...\n");

	/* firmware downlaod complete
	 *  ------------------------------------------------------
	 * |      0     |     1      |    2 - 5    |     6 - 9       |
	 *  ------------------------------------------------------
	 * | opcode | target | code size | checksum |
	 *  ------------------------------------------------------
	 *
	 */

	buf = p->buf + 1;
	length = p->length-1;

	request.download_complete.target = *buf;
	length--;
	buf++;

	value = *((uint32_t *)buf);
	request.download_complete.code_size = FROM_BE32(value);
	length -= sizeof value;
	buf += sizeof value;

	value = *((uint32_t *)buf);
	request.download_complete.checksum = FROM_BE32(value);
	length -= sizeof value;
	buf += sizeof value;

	status = get_fw_config(fw_ptr, fw_ptr->facility_data->callback, IRL_FA_DOWNLOAD_DONE, &request, &response);

	if (status == IRL_STATUS_CONTINUE)
	{
		/* send firmware download complete response
		 *  ---------------------------------------------------------------
		 * |      0     |     1     |   2 - 5    |     6 - 9      |    10    |
		 *  ---------------------------------------------------------------
		 * | opcode | target | version | calculated | status |
		 * |              |            |              | checksum |            |
		 *  ---------------------------------------------------------------
		 *
		 *  Firmware ID string: [descr]0xa[file name spec]
		 */
		uint8_t * ptr;

		irl_send_packet_init(irl_ptr, &pkt, PKT_PRE_FACILITY);

		/* now add this target to the target list message */
		ptr = pkt.buf;
		*ptr++ = IRL_FW_DOWNLOAD_COMPLETE_RESPONSE_OPCODE;
		*ptr++ = request.download_complete.target;  /* target number */
		value = TO_BE32(response.download_complete.version);
		memcpy((void *)ptr, (void *)&value, sizeof value);
		p->length += sizeof value;
		ptr += sizeof value;

		value = TO_BE32(response.download_complete.calculated_checksum);
		memcpy((void *)ptr, (void *)&value, sizeof value);
		p->length += sizeof value;
		ptr += sizeof value;

		*ptr++ = response.download_complete.status;

		pkt.length = ptr - pkt.buf;

		rc = fw_send(fw_ptr, &pkt);
	}
	else if (status == IRL_STATUS_ERROR)
	{
		send_fw_abort(fw_ptr, request.download_request.target, response.error_status);
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}
	else
	{
		rc = IRL_DOWNLOAD_ERR;
	}

	return rc;

}

int process_target_reset(struct irl_firmware_data_t * fw_ptr, struct e_packet * p)
{
	int 				rc = IRL_SUCCESS;
	IrlStatus_t			status;
	IrlFirmwareReq_t	request;
	uint8_t				* buf;
	uint16_t			length;

	DEBUG_PRINTF("Process Firmware Reset\n");

	/* firmware target reset
	 *  -----------------------
	 * |      0     |     1     |
	 *  ----------------------
	 * | opcode | target |
	 *  -----------------------
	 *
	 */
	buf = p->buf + 1;
	length = p->length;

	request.target = *buf;
	length--;
	buf++;

	status = get_fw_config(fw_ptr, fw_ptr->facility_data->callback, IRL_FA_DOWNLOAD_RESET, &request, NULL);

	if (status == IRL_STATUS_ERROR)
	{
		rc = IRL_DOWNLOAD_ERR;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;
}

int irl_fw_process(IrlSetting_t * irl_ptr, IrlFacilityHandle_t * fac_ptr, struct e_packet * p)
{
	int 												rc = IRL_SUCCESS;
	uint8_t										opcode;
	unsigned									i;
	struct irl_firmware_data_t		* fw_ptr = (struct irl_firmware_data_t *)fac_ptr->facility_data;


	DEBUG_PRINTF("irl_fw_process...\n");

	if (p == NULL)
	{
		rc = fw_discovery_layer_init(fw_ptr, fw_ptr->facility_data->callback);

	}
	else if (fw_ptr->keepalive)
	{
		rc = fw_discovery_layer_init(fw_ptr, fw_ptr->facility_data->callback);
		if (rc != IRL_BUSY)
		{
			if (irl_get_system_time(irl_ptr, &fw_ptr->ka_time) != IRL_SUCCESS)
			{
				rc = IRL_CONFIG_ERR;
			}

			fw_ptr->keepalive = 0;
			goto _cont;
		}
	}
	else
	{

_cont:
		opcode = p->buf[0];

//		if (opcode == IRL_FW_BINARY_BLOCK_OPCODE || opcode == IRL_FW_DOWNLOAD_COMPLETE_OPCODE)
		if (opcode == IRL_FW_DOWNLOAD_COMPLETE_OPCODE)
		{
			if (fw_ptr->ka_time == 0)
			{
				fw_ptr->ka_time = irl_ptr->tx_ka_time;
			}
		}

		for (i=0; i < gIrlFirmwareOpcodeHandleCount; i++)
		{
			if (gIrlFirmwareOpcodeHandle[i].opcode == opcode)
			{

				rc = gIrlFirmwareOpcodeHandle[i].callback(fw_ptr, p);
				break;
			}
		}

	}


	return rc;
}

int irlEnable_FirmwareFacility(unsigned long irl_handle, void * firmware_data)
{
	IrlSetting_t 					* irl_ptr = (IrlSetting_t *) irl_handle;
	int 							rc = IRL_BUSY;

	DEBUG_PRINTF("Enable Firmware Facility\n");
	if (gIrlFirmwareFacilityData == NULL)
	{
		DEBUG_PRINTF("malloc firmware facility data\n");
		rc = irl_malloc(irl_ptr, sizeof(struct irl_firmware_data_t), (void **)&gIrlFirmwareFacilityData);
		if (rc != IRL_SUCCESS)
		{
			goto _ret;
		}
		if (gIrlFirmwareFacilityData == NULL)
		{
			rc = IRL_MALLOC_ERR;
			goto _ret;
		}
		gIrlFirmwareFacilityData->idx = 0;
		gIrlFirmwareFacilityData->target = 0;
		gIrlFirmwareFacilityData->target_count = 0;
		gIrlFirmwareFacilityData->facility_data = firmware_data;
		gIrlFirmwareFacilityData->irl_ptr = irl_ptr;
		gIrlFirmwareFacilityData->keepalive = 0;

	}


	if (gIrlFirmwareFacilityData != NULL)
	{
		/*  Add firmware facility to the IRL setting facility list.
		 *  [IRL setting]--->[facility_list]--->[facility_data]                    --->[user_data]
		 *  [IRL setting]--->[facility_list]--->[gIrlfirmwareFacilityData]--->[firmware_data]
		 */
		rc = irl_add_facility(irl_ptr, firmware_data, E_MSG_FAC_FW_NUM, gIrlFirmwareFacilityData, (irl_facility_process_cb_t)irl_fw_process);
		if (rc == IRL_SUCCESS)
		{
			gIrlFirmwareFacilityData->idx = 0;
			gIrlFirmwareFacilityData->target = 0;
			gIrlFirmwareFacilityData->keepalive = 0;
		}
	}

_ret:
	return rc;
}
