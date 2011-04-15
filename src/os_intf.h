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
#ifndef OS_INTF_H_
#define OS_INTF_H_

#include "e_types.h"

// #include "irl_api.h"
#include "config_intf.h"


#ifdef __cplusplus
extern "C"
{
#endif


int irl_wait(struct irl_setting_t * irl_ptr, uint32_t wait_time);
int irl_get_system_time(struct irl_setting_t * irl_ptr, uint32_t * mstime);
int irl_malloc(struct irl_setting_t * irl_ptr, size_t length, void ** ptr);
int irl_free(struct irl_setting_t * irl_ptr, void * ptr);


#ifdef __cplusplus
}
#endif

#endif /* OS_INTF_H_ */
