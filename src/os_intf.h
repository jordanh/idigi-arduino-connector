/*
 * os_cb.h
 *
 *  Created on: Mar 24, 2011
 *      Author: mlchan
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
