/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include <mqx.h>
#include "idigi_api.h"
#include "platform.h"
#include "idigi_debug.h"

idigi_callback_status_t app_os_malloc(size_t const size, void ** ptr)
{
    idigi_callback_status_t status = idigi_callback_abort;

    *ptr = _mem_alloc(size);
    if (*ptr != NULL)
    {
        status = idigi_callback_continue;
    }
    else
    {
        APP_DEBUG ("os_malloc: failed\n");
    }

    return status;
}

void app_os_free(void * const ptr)
{
    ASSERT(ptr != NULL);
    if (ptr != NULL)
    {
        unsigned int const result = _mem_free(ptr);

        if (result)
        {
            APP_DEBUG("os_free: _mem_free failed [%d]\n");
        }
    }

    return;
}

idigi_callback_status_t app_os_get_system_time(unsigned long * const uptime)
{
    TIME_STRUCT curtime;
    static unsigned long start_system_up_time = 0;

    _time_get(&curtime);
    if (start_system_up_time == 0)
        start_system_up_time = curtime.SECONDS;
 
    /* Up time in seconds */
    *uptime = curtime.SECONDS - start_system_up_time;

    return idigi_callback_continue;
}

idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds)
{
    if (timeout_in_seconds == 0)
        _sched_yield();
    else
        _time_delay(timeout_in_seconds * 1000);

    return idigi_callback_continue;
}

idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                                        void const * const request_data, size_t const request_length,
                                        void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        status = app_os_malloc(*((size_t *)request_data), response_data);
        break;

    case idigi_os_free:
        app_os_free((void * const)request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        status = app_os_get_system_time(response_data);
        break;

    case idigi_os_sleep:
        status = app_os_sleep(*((unsigned int *)request_data));
        break;

    default:
        APP_DEBUG("app_os_handler: unrecognized request [%d]\n", request);
        status = idigi_callback_unrecognized;
        break;
    }

    return status;
}




