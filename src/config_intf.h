/*
 * config_intf.h
 *
 *  Created on: Mar 24, 2011
 *      Author: mlchan
 */

#ifndef CONFIG_INTF_H_
#define CONFIG_INTF_H_

#include "irl_def.h"

IrlStatus_t irl_error_status(irl_callback_t callback, unsigned config_id, int status);
IrlStatus_t irl_get_config(struct irl_setting_t * irl_ptr, unsigned config_id, void * data);

#if 0
int irl_get_device_id(struct irl_setting_t * irl_ptr, uint8_t * device_id);
int irl_get_vendor_id(struct irl_setting_t * irl_ptr, uint8_t * vendor_id);
int irl_get_device_type(struct irl_setting_t * irl_ptr, char ** device_type);
int irl_get_server_url(struct irl_setting_t * irl_ptr, char ** server_url);
int irl_get_tx_keepalive(struct irl_setting_t * irl_ptr, uint16_t * keepalive);
int irl_get_rx_keepalive(struct irl_setting_t * irl_ptr, uint16_t * keepalive);
int irl_get_wait_count(struct irl_setting_t * irl_ptr, uint8_t * wait_count);
int irl_get_password(struct irl_setting_t * irl_ptr, char ** password);

#endif

#ifdef __cplusplus
}
#endif


#endif /* CONFIG_INTF_H_ */
