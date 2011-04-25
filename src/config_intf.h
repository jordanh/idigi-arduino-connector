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

#ifdef __cplusplus
extern "C"
{
#endif


IrlStatus_t irl_error_status(irl_callback_t callback, unsigned config_id, int status);
IrlStatus_t irl_get_config(IrlSetting_t * irl_ptr, unsigned config_id, void * data);
int  irl_check_config_null(IrlSetting_t *irl_ptr, unsigned config_id);

int irl_add_facility(IrlSetting_t * irl_ptr, void * user_data, unsigned facility_id, void * facility_data, irl_facility_process_cb_t process_cb);
//IrlFacilityHandle_t * irl_get_facility_handle(IrlSetting_t * irl_ptr, unsigned facility_id);
//int irl_del_facility_handle(IrlSetting_t * irl_ptr, unsigned facility_id);
//int irl_add_facility_handle(IrlSetting_t * irl_ptr, unsigned facility_id, IrlFacilityHandle_t ** fac_handle);


#ifdef __cplusplus
}
#endif


#endif /* CONFIG_INTF_H_ */
