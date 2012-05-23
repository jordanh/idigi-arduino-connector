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

#include <malloc.h>
#include <time.h>
#include <unistd.h>
#include "idigi_api.h"
#include "platform.h"

int os_malloc(size_t const size, void ** ptr)
{
    int status=-1;

    *ptr = malloc(size);
    ASSERT(*ptr != NULL);
    if (*ptr != NULL)
    {
        status = 0;
    }

    return status;
}

void os_free(void * const ptr)
{
    ASSERT(ptr != NULL);

    if (ptr != NULL)
    {
        free(ptr);
    }
    return;
}


int os_get_system_time(uint32_t * const uptime)
{
    time((time_t *)uptime);

    return 0;
}

void os_sleep(unsigned int const timeout_in_seconds)
{
    unsigned int const timeout_in_milliseconds = timeout_in_seconds * 1000;

    tx_thread_sleep (NS_MILLISECONDS_TO_TICKS (timeout_in_milliseconds));
    return;
}

idigi_callback_status_t idigi_os_callback(idigi_os_request_t const request,
                                        void * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int ret;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        ret    = os_malloc(*((size_t *)request_data), (void **)response_data);
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_busy;
        break;

    case idigi_os_free:
        os_free(request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        ret    = os_get_system_time((uint32_t *)response_data);
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
        break;

    case idigi_os_sleep:
        os_sleep(*((unsigned int *)request_data));
        status = idigi_callback_continue;
        break;

    default:
        APP_DEBUG("idigi_os_callback: unrecognized request [%d]\n", request);
        break;
    }

    return status;
}


