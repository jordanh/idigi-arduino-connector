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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "idigi_api.h"
#include "platform.h"

extern idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle);


idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void const * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    switch (class_id)
    {
    case idigi_class_config:
        status = idigi_config_callback(request_id.config_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_operating_system:
        status = idigi_os_callback(request_id.os_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_network:
        status = idigi_network_callback(request_id.network_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_data_service:
        status = idigi_data_service_callback(request_id.firmware_request, request_data, request_length, response_data, response_length);
        break;

    default:
        /* not supported */
        break;
    }
    return status;
}

/* Frequency to send data to the iDigi device cloud */
static const unsigned sample_rate=5;

int application_run(idigi_handle_t handle)
{
    int stop_calling = 0;

    for (;;)
    {
        os_sleep(sample_rate);
        idigi_status_t const status = send_put_request(handle);

        if (status == idigi_init_error)
        {
            break;
        }
    };

    return stop_calling;
}

