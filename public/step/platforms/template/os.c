/*
 * Copyright (c) 2011, 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include "idigi_api.h"
#include "platform.h"

idigi_callback_status_t app_os_malloc(size_t const size, void ** ptr)
{
    UNUSED_ARGUMENT(size);
    UNUSED_ARGUMENT(ptr);
    return idigi_callback_continue;
}

void app_os_free(void * const ptr)
{
    UNUSED_ARGUMENT(ptr);
    return;
}

idigi_callback_status_t app_os_get_system_time(unsigned long * const uptime)
{
    UNUSED_ARGUMENT(uptime);
    return idigi_callback_continue;
}

idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds)
{
    UNUSED_ARGUMENT(timeout_in_seconds);
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

            status    = app_os_malloc(*bytes, response_data);
        }
        break;

    case idigi_os_free:
        app_os_free((void *)request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        status    = app_os_get_system_time(response_data);
        break;

    case idigi_os_sleep:
        {
            unsigned int const * const seconds = request;

            status = app_os_sleep(*seconds);
        }
        break;

    default:
        status = idigi_callback_unrecognized;
        break;
    }

    return status;
}
/**
 * @endcond
 */


