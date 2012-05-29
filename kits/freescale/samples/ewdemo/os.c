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

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include "idigi_api.h"
#include "platform.h"

int idigi_malloc_failures = 0;

int app_os_malloc(size_t  const size, void ** ptr)
{
    int status = -1;

    *ptr = _mem_alloc(size);
    
    if (*ptr != NULL)
    {
#ifdef DEBUG_MALLOC
        APP_DEBUG ("os_malloc: ptr = [%x] size = [%d]\n", *ptr, size);
#endif
        status = 0;
    }
    else
    {    
        APP_DEBUG ("os_malloc: failed\n");
        idigi_malloc_failures ++;
    }

    return status;
}

void app_os_free(void * const ptr)
{
	unsigned int result;
    ASSERT(ptr != NULL);

#ifdef DEBUG_MALLOC
    APP_DEBUG ("os_free: ptr = [%x]\n", ptr);
#endif
    
    if (ptr != NULL)
    {
    	result = _mem_free(ptr);
    	if (result)
    	{
    		APP_DEBUG("os_free: _mem_free failed [%d]\n", result);
    	}
    }
    return;
}

int app_os_get_system_time(unsigned long * const uptime)
{
    TIME_STRUCT curtime;
    static start_system_up_time = -1;
    _time_get(&curtime);
    *uptime = curtime.SECONDS;

    if (start_system_up_time == -1)
    {
    	start_system_up_time = curtime.SECONDS;
    }
    
    /* Up time in seconds */
    *uptime = (uint32_t)(curtime.SECONDS - start_system_up_time);

    return 0;
}

void app_os_sleep(unsigned int const timeout_in_seconds)
{
	if (timeout_in_seconds == 0)	
		_sched_yield();
	else	
        _time_delay(timeout_in_seconds * 1000);

    return;
}

idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                                        void const * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int ret;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        ret    = app_os_malloc(*((size_t *)request_data), (void **)response_data);
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_busy;
        break;

    case idigi_os_free:
        app_os_free((void *)request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        ret    = app_os_get_system_time((unsigned long *)response_data);
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
        break;

    case idigi_os_sleep:
        app_os_sleep(*((unsigned int *)request_data));
        status = idigi_callback_continue;
        break;

    default:
        APP_DEBUG("idigi_os_callback: unrecognized request [%d]\n", request);
        break;
    }

    return status;
}


