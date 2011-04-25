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
#include <stdio.h>
#include "config_intf.h"
#include "network_intf.h"
#include  "os_intf.h"
#include "layer.h"

#define IS_NETWORK_BUSY_CONFIG(x)	(((x) == IRL_CONFIG_CLOSE) || ((x) == IRL_DISCONNECTED) || ((x) == IRL_REDIRECT))
#define NETWORK_BUSY_FLAG(x)		((((x) == IRL_CONFIG_CLOSE) ? 0x01 : 0) | (((x) == IRL_DISCONNECTED) ? 0x02 : 0) | (((x) == IRL_REDIRECT) ? 0x04 : 0))

/* Error Status callback */
IrlStatus_t irl_error_status(irl_callback_t callback, unsigned config_id, int status)
{
	IrlErrorStatus_t err;

	err.config_id = config_id;
	err.status = status;

	return callback(IRL_ERROR_STATUS, &err);
}

IrlStatus_t irl_get_config(IrlSetting_t * irl_ptr, unsigned config_id, void * data)
{

	IrlStatus_t	status;

	status = irl_ptr->callback(config_id, data);
	if (IS_NETWORK_BUSY_CONFIG(config_id))
	{
		if (status == IRL_STATUS_BUSY)
			irl_ptr->network_busy |= NETWORK_BUSY_FLAG(config_id);
		else
			irl_ptr->network_busy &= ~NETWORK_BUSY_FLAG(config_id);
	}
	return status;
}

int  irl_check_config_null(IrlSetting_t *irl_ptr, unsigned config_id)
{
	IrlStatus_t		status;
	int				rc = IRL_SUCCESS;

	if (irl_ptr->config.data[config_id] == NULL)
	{
		status =  irl_error_status(irl_ptr->callback, config_id, IRL_INVALID_DATA);
		if (status == IRL_STATUS_CONTINUE)
		{
			/* continue calling the callback */
			rc = IRL_BUSY;
		}
		else
		{
			rc = IRL_CONFIG_ERR;
		}
	}

	return rc;
}


unsigned irl_get_select_set(IrlSetting_t * irl_ptr)
{
	unsigned set = IRL_NETWORK_READ_SET | IRL_NETWORK_TIMEOUT_SET; /* always receive (waiting data from server) */

	if (irl_ptr->send_packet.total_length > 0)
	{
		set |= IRL_NETWORK_WRITE_SET;
	}
	if (irl_ptr->callback_event)
	{
		set |= IRL_NETWORK_CALLBACK_SET;
	}

	return set;
}

IrlFacilityHandle_t * irl_get_facility_handle(IrlSetting_t * irl_ptr, unsigned facility_id)
{
	IrlFacilityHandle_t	* fac_ptr, * rc_fac = NULL;

	for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
	{
		if (fac_ptr->facility_id == facility_id)
		{
			rc_fac = fac_ptr;
			break;
		}
	}

	return rc_fac;
}

int irl_del_facility_handle(IrlSetting_t * irl_ptr, unsigned facility_id)
{
	int								rc = IRL_SUCCESS;
	IrlFacilityHandle_t	* fac_ptr, * fac_ptr1 = NULL, * next_ptr;

	for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
	{
		if (fac_ptr->facility_id == facility_id)
		{
			next_ptr = fac_ptr->next;

			rc = irl_free(irl_ptr, (void *)fac_ptr);
			if (rc == IRL_SUCCESS)
			{
				if (fac_ptr1 != NULL)
				{
					fac_ptr1->next = next_ptr;
				}
				if (fac_ptr == irl_ptr->facility_list)
				{
					irl_ptr->facility_list = next_ptr;
				}
			}
			break;
		}
		fac_ptr1 = fac_ptr;
	}

	return rc;
}

int irl_add_facility_handle(IrlSetting_t * irl_ptr, unsigned facility_id, IrlFacilityHandle_t ** fac_handle)
{
	int								rc = IRL_SUCCESS;
	IrlFacilityHandle_t	* fac_ptr;

	fac_ptr = irl_get_facility_handle(irl_ptr, facility_id);

	if (fac_ptr == NULL)
	{

		rc = irl_malloc(irl_ptr, sizeof(IrlFacilityHandle_t), (void **)&fac_ptr);
		if (rc != IRL_SUCCESS)
		{
			goto _ret;
		}
		if (fac_ptr == NULL)
		{
			rc = IRL_MALLOC_ERR;
			goto _ret;
		}

		fac_ptr->facility_enalble_function	= NULL;
		fac_ptr->facility_id				            = facility_id;
		fac_ptr->facility_data				        = NULL;
		fac_ptr->process_function 		    	= NULL;
		fac_ptr->state						            = IRL_FACILITY_INIT;
		fac_ptr->packet                              = NULL;
		fac_ptr->next 						            = irl_ptr->facility_list;

		irl_ptr->facility_list = fac_ptr;
	}

	*fac_handle = fac_ptr;

_ret:
	return rc;
}

int irl_add_facility(IrlSetting_t * irl_ptr, void * user_data, unsigned facility_id, void * facility_data, irl_facility_process_cb_t process_cb)
{
	int 							rc = IRL_INIT_ERR;
	IrlFacilityHandle_t	* fac_ptr = NULL;

	rc = irl_add_facility_handle(irl_ptr, facility_id, &fac_ptr);

	if (fac_ptr != NULL)
	{
		DEBUG_PRINTF("irl_add_facility: add facility 0x%x\n", facility_id);
		fac_ptr->user_data = user_data;
		fac_ptr->facility_data = facility_data;
		fac_ptr->process_function = process_cb;
		rc = IRL_SUCCESS;
	}

	return rc;
}


#if 0
int irl_get_device_id(IrlSetting_t * irl_ptr, uint8_t * device_id)
{
	unsigned		config_id;
	int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_DEVICE_ID;
	if (irl_get_config(irl_ptr, config_id, device_id) == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;

		DEBUG_PRINTF("irl_get_device_id: device_id = %02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x\n",
				(unsigned)device_id[0], (unsigned)device_id[1],
				(unsigned)device_id[2], (unsigned)device_id[3],
				(unsigned)device_id[4], (unsigned)device_id[5],
				(unsigned)device_id[6], (unsigned)device_id[7],
				(unsigned)device_id[8], (unsigned)device_id[9],
				(unsigned)device_id[10], (unsigned)device_id[11],
				(unsigned)device_id[12], (unsigned)device_id[13],
				(unsigned)device_id[14], (unsigned)device_id[15]);
	}

	return rc;
}

int irl_get_vendor_id(IrlSetting_t * irl_ptr, uint8_t * vendor_id)
{
	unsigned		config_id;
	int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_VENDOR_ID;
	if (irl_get_config(irl_ptr, config_id, vendor_id) == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_PRINTF("irl_get_vendor_id: vendor_id = %02x%02x%02x%02x\n",
				(unsigned)vendor_id[0], (unsigned)vendor_id[1],
				(unsigned)vendor_id[2], (unsigned)vendor_id[3]);
	}

	return rc;
}

int irl_get_device_type(IrlSetting_t * irl_ptr, char ** device_type)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_DEVICE_TYPE;
	status = irl_get_config(irl_ptr, config_id, device_type);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;

		DEBUG_PRINTF("irl_get_device_type: device_type = %s\n", *device_type);
	}

	return rc;
}

int irl_get_server_url(IrlSetting_t * irl_ptr, char ** server_url)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_SERVER_URL;
	status = irl_get_config(irl_ptr, config_id, server_url);

	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;

		DEBUG_PRINTF("irl_get_server_url: server_url = %s\n", *server_url);
	}

	return rc;
}

int irl_get_tx_keepalive(IrlSetting_t * irl_ptr, uint16_t * keepalive)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_TX_KEEPALIVE;
	status = irl_get_config(irl_ptr, config_id, keepalive);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_PRINTF("irl_get_tx_keepalive: tx_keepalive interval = %d\n", *keepalive);
	}

	return rc;
}

int irl_get_rx_keepalive(IrlSetting_t * irl_ptr, uint16_t * keepalive)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_RX_KEEPALIVE;
	status = irl_get_config(irl_ptr, config_id, keepalive);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_PRINTF("irl_get_rx_keepalive: rx_keepalive interval = %d\n", *keepalive);
	}

	return rc;
}

int irl_get_wait_count(IrlSetting_t * irl_ptr, uint8_t * wait_count)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_WAIT_COUNT;
	status = irl_get_config(irl_ptr, config_id, wait_count);

	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_PRINTF("irl_get_wait_count: wait_count = %d\n", *wait_count);
	}

	return rc;
}

int irl_get_password(IrlSetting_t * irl_ptr, char ** password)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_PASSWORD;
	status = irl_get_config(irl_ptr, config_id, password);

	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_PRINTF("irl_get_password: password = %s\n", *password);
	}

	return rc;
}
#endif


