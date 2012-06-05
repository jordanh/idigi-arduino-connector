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

idigi_callback_status_t app_os_malloc(size_t const size, void ** ptr)
{
    idigi_callback_status_t status = idigi_callback_abort;

    *ptr = malloc(size);
    if (*ptr != NULL)
    {
        status = idigi_callback_continue;
    }
    else
    {
        APP_DEBUG("app_os_malloc: Failed to malloc\n");
    }

    return status;
}

void app_os_free(void * const ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
    else
    {
        APP_DEBUG("app_os_free: called with NULL\n");
    }

    return;
}


idigi_callback_status_t app_os_get_system_time(unsigned long * const uptime)
{
    time((time_t *)uptime);

    return idigi_callback_continue;
}

idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds)
{
    unsigned int const timeout_in_milliseconds = timeout_in_seconds * 1000;

    tx_thread_sleep (NS_MILLISECONDS_TO_TICKS (timeout_in_milliseconds));

    return idigi_callback_continue;
}

idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                                        void const * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        {
            size_t const * const bytes = request_data;

            status = app_os_malloc(*bytes, response_data);
        }
        break;

    case idigi_os_free:
        app_os_free((void *)request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        status = app_os_get_system_time(response_data);
        break;

    case idigi_os_sleep:
        {
            unsigned int const * const seconds = request_data;

            status = app_os_sleep(*seconds);
        }
        break;

    default:
        APP_DEBUG("app_os_handler: unrecognized request [%d]\n", request);
        status = idigi_callback_unrecognized;
        break;
    }

    return status;
}


