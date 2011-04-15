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
