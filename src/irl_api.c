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
#include "irl_def.h"
#include "os_intf.h"
#include "network_intf.h"
#include "irl_cc.h"

#define IRL_MAX_IRL_HANDLE	1

IrlSetting_t 	gIrlData[IRL_MAX_IRL_HANDLE];
unsigned				gIrlDataCount = 0;

/* main edp state processes called by start function */
typedef int (* irl_edp_state_cb)(IrlSetting_t * irl_ptr);

irl_edp_state_cb gIrlEdpStateCallback[] = {
	irl_edp_configuration_layer,
	irl_communication_layer,
	irl_initialization_layer,
	irl_security_layer,
	irl_discovery_layer,
	irl_facility_layer
};


e_boolean_t irl_validate_handle(unsigned long handle)
{
	unsigned    i;
	e_boolean_t rc = FALSE;

	for (i=0; i < gIrlDataCount; i++)
	{
		if (handle == (unsigned)&gIrlData[i])
		{
			rc = TRUE;
			break;
		}
	}

	return rc;
}


void irl_init_setting(IrlSetting_t * irl_ptr)
{
   	irl_ptr->active_state = IRL_HANDLE_ACTIVE;
	irl_ptr->active_facility_idx = 0;
	irl_ptr->active_facility = NULL;
	irl_ptr->connection.socket_fd = -1;
	irl_ptr->edp_state = IRL_INIT;
	irl_ptr->config.id = 0;
	irl_ptr->layer_state = 0;

}

/* Starts and process IRL.
 *
 *
 *
 * @param handler		handler returned from irl_init.
 * @param data			Pointer to IRL data which includes all supported facilities.
 *
 * @return IRL_SUCCESS		Successfully process IRL. Application need to call this function
 * 							again to continue IRL.
 * @return not IRL_SUCCESS	Error is encountered and IRL has closed the connection. Applciation
 * 							may call this function to restart IRL or irl_stop to terminated IRL.
 *
 *
 */
int irl_start(unsigned long handle, IrlData_t const * data)
{
    int         	rc = IRL_SUCCESS;
    IrlSetting_t   	* irl_handle = (IrlSetting_t *)handle;
	int				sent_status = IRL_NETWORK_BUFFER_COMPLETE;
	unsigned		i;

	IrlFacilityHandle_t	* fac_ptr;
   irl_edp_state_cb	state_function;

	if (!irl_validate_handle(handle) ||
		irl_handle->active_state == IRL_HANDLE_INACTIVE)
	{
		rc = IRL_INIT_ERR;
		goto _ret;
	}

	if (irl_handle->edp_state == IRL_INIT)
	{

		irl_handle->edp_version = data->version;
		irl_handle->facility_count = data->facility_count;

		while (irl_handle->active_facility_idx < irl_handle->facility_count)
		{
			i = irl_handle->active_facility_idx;

			rc = irl_add_facility_handle(irl_handle, data->facility_list[i].user_data, &fac_ptr);

			if (rc != IRL_SUCCESS)
			{
				goto _ret;
			}

			if (fac_ptr->state == IRL_FACILITY_INIT)
			{
				fac_ptr->facility_enalble_function = data->facility_list[i].facility_enalble_function;

				if (fac_ptr->facility_enalble_function != NULL)
				{
					rc = fac_ptr->facility_enalble_function((unsigned long)irl_handle, fac_ptr->user_data);
					if (rc != IRL_SUCCESS)
					{
						goto _ret;
					}
				}
				fac_ptr->state = IRL_FACILITY_INIT_DONE;
			}
			irl_handle->active_facility_idx++;
		}

		if (irl_handle->active_facility_idx >= irl_handle->facility_count)
		{
			rc = irlEnable_ConnectionControl((unsigned long)irl_handle);
			if (rc != IRL_SUCCESS)
			{
				goto _ret;
			}
			irl_handle->active_facility_idx = 0;
		}
	}

	if (irl_handle->edp_state >= (int)(sizeof gIrlEdpStateCallback / sizeof gIrlEdpStateCallback[0]))
	{
		DEBUG_TRACE("irl_start: invalid state %d\n", irl_handle->edp_state);
		rc = IRL_STATE_ERR;
		goto _ret;
	}


	rc = irl_send_packet_status(irl_handle, &sent_status);

	if (sent_status != IRL_NETWORK_BUFFER_COMPLETE)
	{
		DEBUG_TRACE("irl_start: send pending\n");
		goto _ret;
	}

	state_function = gIrlEdpStateCallback[irl_handle->edp_state];
	rc = state_function(irl_handle);

_ret:
	if (rc == IRL_BUSY)
	{
		rc = IRL_SUCCESS;
	}
	else if (rc < 0)
	{
		if (irl_close(irl_handle) != IRL_SUCCESS)
		{
			rc = IRL_CLOSE_ERR;
		}
		if (irl_handle->active_state == IRL_HANDLE_TERMINATE)
		{
			DEBUG_TRACE("irl_start: IRL_HANDLE_TERMINATE\n");
			irl_handle->active_state = IRL_HANDLE_INACTIVE;
		}
		else
		{
			DEBUG_TRACE("irl_start: IRL_HANDLE_STOP\n");
			irl_handle->active_state = IRL_HANDLE_ACTIVE;
		}
	}
    return rc;
}

/*
 * Initializes IRL and get device ID, vendor ID and device type configurations
 * from callback. It returns IRL handle and IRL is ready to start.
 *
 * @param callback		callback for EDP configurations, network interface,
 * 						and operating system interface.
 *
 * @return 0x0			Unable to initialize IRL or error is found.
 * @return handler		IRL Handler used throughout IRL API.
 *
 */
unsigned long irl_init(irl_callback_t callback)
{
    IrlSetting_t   	* irl_handle = NULL;
    int 					status = IRL_SUCCESS;
    unsigned				i;

    unsigned irl_init_config_ids[] = {
    		IRL_CONFIG_DEVICE_ID, IRL_CONFIG_VENDOR_ID, IRL_CONFIG_DEVICE_TYPE
    };

    if (gIrlDataCount < IRL_MAX_IRL_HANDLE && callback != NULL)
    {
    	/* get the IRL setting */
    	irl_handle = &gIrlData[gIrlDataCount];
    	gIrlDataCount++;
    	irl_init_setting(irl_handle); /* set irl setting to initial values */


		irl_handle->callback = (irl_callback_t)callback;
		i = 0;

		while (i < (sizeof irl_init_config_ids/sizeof irl_init_config_ids[0]) && status == IRL_SUCCESS)
		{
			/* get initial configurations */
			do {
				status = irl_get_config(irl_handle, irl_init_config_ids[i], &irl_handle->config.data[irl_init_config_ids[i]]);
				/* wait here ??? */
			} while (status == IRL_STATUS_BUSY);

			i++;
		}
	}

    if (status != IRL_SUCCESS && irl_handle != NULL)
    {
    	irl_handle->active_state = IRL_HANDLE_INACTIVE;
		gIrlDataCount--;
		irl_handle = NULL;
    }
    return (unsigned long) irl_handle;
}


/* Stops or terminates IRL.
 *
 *
 *
 * @param handler		handler returned from irl_init.
 * @param stop_flag     IRL_STOP flag to stop IRL which IRL will disconnect iDigi server
 * 						and may be reconnect to iDigi server again.
 *						IRL_TERMINATE flag to terminate IRL which IRL will disconnect iDigi
 *						server and free all memory used. IRL cannot be restart again unless
 *						irl_init is called again.
 *
 * @return IRL_SUCCESS		IRL was successfully stopped or terminated.
 * @return IRL_INIT_ERR		Invalid handle
 *
 *
 */
int irl_stop(unsigned long handle, unsigned stop_flag)
{
	int 		rc = IRL_SUCCESS;
    IrlSetting_t   * irl_handle = (IrlSetting_t *)handle;

	if (!irl_validate_handle(handle))
	{
		rc = IRL_INIT_ERR;
		goto _ret;
	}

	if (stop_flag == IRL_TERMINATE)
	{
		irl_handle->active_state = IRL_HANDLE_TERMINATE;
	}
	else if (irl_handle->active_state == IRL_HANDLE_ACTIVE)
	{
		irl_handle->active_state = IRL_HANDLE_STOP;

	}
_ret:
	return rc;
}
