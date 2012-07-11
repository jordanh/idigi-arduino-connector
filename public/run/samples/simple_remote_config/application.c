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
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"


extern idigi_callback_status_t app_remote_config_handler(idigi_remote_config_request_t const request,
                                                      void const * const request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length);


idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void const * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    switch (class_id)
    {
    case idigi_class_config:
        status = app_config_handler(request_id.config_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_operating_system:
        status = app_os_handler(request_id.os_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_network:
        status = app_network_handler(request_id.network_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_remote_config_service:
        status = app_remote_config_handler(request_id.remote_config_request, request_data, request_length, response_data, response_length);
        break;
    default:
        status = idigi_callback_unrecognized;
        /* not supported */
        break;
    }
    return status;
}

int application_run(idigi_handle_t handle)
{
    UNUSED_ARGUMENT(handle);

    /* No application's thread here.
     * Application has no other process.
     * main() will start idigi_run() as a separate thread.
     */

    return 0;
}

