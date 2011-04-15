/*
 * config_intf.c
 *
 *  Created on: Mar 24, 2011
 *      Author: mlchan
 */
#include "config_intf.h"
#include "network_intf.h"
#include  "os_intf.h"

#define IS_NETWORK_BUSY_CONFIG(x)	(((x) == IRL_CONFIG_CLOSE) || ((x) == IRL_DISCONNECTED) || ((x) == IRL_REDIRECT))
#define NETWORK_BUSY_FLAG(x)		((((x) == IRL_CONFIG_CLOSE) ? 0x01 : 0) | (((x) == IRL_DISCONNECTED) ? 0x02 : 0) | (((x) == IRL_REDIRECT) ? 0x04 : 0))

/* Error Status callback */
IrlStatus_t irl_error_status(irl_callback_t callback, unsigned config_id, int status)
{
    struct irl_error_status_t err;

    err.config_id = config_id;
    err.status = status;

    return callback(IRL_ERROR_STATUS, &err);
}

IrlStatus_t irl_get_config(struct irl_setting_t * irl_ptr, unsigned config_id, void * data)
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

unsigned irl_edp_init_config_ids[] = {
		IRL_CONFIG_SERVER_URL, IRL_CONFIG_TX_KEEPALIVE, IRL_CONFIG_RX_KEEPALIVE, IRL_CONFIG_WAIT_COUNT,
};

int irl_edp_configuration_layer(struct irl_setting_t * irl_ptr)
{
	int 		rc = IRL_SUCCESS;
	unsigned	config_id;
	IrlStatus_t	status;

	DEBUG_TRACE("irl_edp_init: INIT\n");
	if (irl_ptr->layer_state == IRL_LAYER_INIT)
	{
		DEBUG_TRACE("--- getting config id\n");
		while(irl_ptr->config.id < (sizeof irl_edp_init_config_ids/ sizeof irl_edp_init_config_ids[0]))
		{
			config_id = irl_edp_init_config_ids[irl_ptr->config.id];

			status = irl_get_config(irl_ptr, config_id, &irl_ptr->config.data[config_id]);

			if (status == IRL_STATUS_CONTINUE)
			{
				irl_ptr->config.id++;
			}
			else if (status != IRL_STATUS_BUSY)
			{
				rc = IRL_CONFIG_ERR;
				goto _ret;
			}
			/* Should be wait here */
		}
		irl_ptr->layer_state = IRL_LAYER_CONNECT;
	}

	if (irl_ptr->layer_state == IRL_LAYER_CONNECT)
	{
		char * server_url;

		config_id = IRL_CONFIG_SERVER_URL;
		server_url = irl_ptr->config.data[config_id];

		DEBUG_TRACE("--- connecting server = %s\n", server_url);
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

unsigned irl_get_select_set(struct irl_setting_t * irl_ptr)
{
	unsigned set = irl_network_read_t_SET | IRL_NETWORK_TIMEOUT_SET; /* always receive (waiting data from server) */

	if (irl_ptr->send_packet.total_length > 0)
	{
		set |= irl_network_write_t_SET;
	}
	if (irl_ptr->callback_event)
	{
		set |= IRL_NETWORK_CALLBACK_SET;
	}

	return set;
}

struct irl_facility_handle_t * irl_get_facility_handle(struct irl_setting_t * irl_ptr, void * user_data)
{
	struct irl_facility_handle_t	* fac_ptr, * rc_fac = NULL;

	for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
	{
		if (fac_ptr->user_data == user_data)
		{
			rc_fac = fac_ptr;
			break;
		}
	}

	return rc_fac;
}

int irl_del_facility_handle(struct irl_setting_t * irl_ptr, void * user_data)
{
	int								rc = IRL_SUCCESS;
	struct irl_facility_handle_t	* fac_ptr, * fac_ptr1 = NULL, * next_ptr;

	for (fac_ptr = irl_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
	{
		if (fac_ptr->user_data == user_data)
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

int irl_add_facility_handle(struct irl_setting_t * irl_ptr, void * user_data, struct irl_facility_handle_t ** fac_handle)
{
	int								rc = IRL_SUCCESS;
	struct irl_facility_handle_t	* fac_ptr;

	fac_ptr = irl_get_facility_handle(irl_ptr, user_data);

	if (fac_ptr == NULL)
	{

		rc = irl_malloc(irl_ptr, sizeof(struct irl_facility_handle_t), (void **)&fac_ptr);
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
		fac_ptr->user_data 					= user_data;
		fac_ptr->facility_data				= NULL;
		fac_ptr->process_function 			= NULL;
		fac_ptr->state						= IRL_FACILITY_INIT;
		fac_ptr->next 						= irl_ptr->facility_list;

		irl_ptr->facility_list = fac_ptr;
	}

	*fac_handle = fac_ptr;

_ret:
	return rc;
}

int irl_add_facility(struct irl_setting_t * irl_ptr, void * user_data, unsigned facility_id, void * facility_data, irl_facility_process_cb_t process_cb)
{
	int 							rc = IRL_INIT_ERR;
	struct irl_facility_handle_t	* fac_ptr;

	fac_ptr = irl_get_facility_handle(irl_ptr, user_data);

	if (fac_ptr != NULL)
	{
		DEBUG_TRACE("irl_add_facility: add facility 0x%x\n", facility_id);
		fac_ptr->facility_id = facility_id;
		fac_ptr->facility_data = facility_data;
		fac_ptr->process_function = process_cb;
		rc = IRL_SUCCESS;
	}

	return rc;
}


#if 0
int irl_get_device_id(struct irl_setting_t * irl_ptr, uint8_t * device_id)
{
    unsigned    	config_id;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_DEVICE_ID;
	if (irl_get_config(irl_ptr, config_id, device_id) == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;

		DEBUG_TRACE("irl_get_device_id: device_id = %02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x\n",
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

int irl_get_vendor_id(struct irl_setting_t * irl_ptr, uint8_t * vendor_id)
{
    unsigned    	config_id;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_VENDOR_ID;
	if (irl_get_config(irl_ptr, config_id, vendor_id) == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_TRACE("irl_get_vendor_id: vendor_id = %02x%02x%02x%02x\n",
				(unsigned)vendor_id[0], (unsigned)vendor_id[1],
				(unsigned)vendor_id[2], (unsigned)vendor_id[3]);
	}

	return rc;
}

int irl_get_device_type(struct irl_setting_t * irl_ptr, char ** device_type)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_DEVICE_TYPE;
	status = irl_get_config(irl_ptr, config_id, device_type);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;

		DEBUG_TRACE("irl_get_device_type: device_type = %s\n", *device_type);
	}

	return rc;
}

int irl_get_server_url(struct irl_setting_t * irl_ptr, char ** server_url)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_SERVER_URL;
	status = irl_get_config(irl_ptr, config_id, server_url);

	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;

		DEBUG_TRACE("irl_get_server_url: server_url = %s\n", *server_url);
	}

	return rc;
}

int irl_get_tx_keepalive(struct irl_setting_t * irl_ptr, uint16_t * keepalive)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_TX_KEEPALIVE;
	status = irl_get_config(irl_ptr, config_id, keepalive);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_TRACE("irl_get_tx_keepalive: tx_keepalive interval = %d\n", *keepalive);
	}

	return rc;
}

int irl_get_rx_keepalive(struct irl_setting_t * irl_ptr, uint16_t * keepalive)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_RX_KEEPALIVE;
	status = irl_get_config(irl_ptr, config_id, keepalive);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_TRACE("irl_get_rx_keepalive: rx_keepalive interval = %d\n", *keepalive);
	}

	return rc;
}

int irl_get_wait_count(struct irl_setting_t * irl_ptr, uint8_t * wait_count)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_WAIT_COUNT;
	status = irl_get_config(irl_ptr, config_id, wait_count);

	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_TRACE("irl_get_wait_count: wait_count = %d\n", *wait_count);
	}

	return rc;
}

int irl_get_password(struct irl_setting_t * irl_ptr, char ** password)
{
    unsigned    	config_id;
    IrlStatus_t 	status = IRL_STATUS_CONTINUE;
    int				rc = IRL_CONFIG_ERR;

	config_id = IRL_CONFIG_PASSWORD;
	status = irl_get_config(irl_ptr, config_id, password);

	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
		DEBUG_TRACE("irl_get_password: password = %s\n", *password);
	}

	return rc;
}
#endif


