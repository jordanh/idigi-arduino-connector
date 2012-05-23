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

#include "idigi_api.h"
#include "platform.h"


extern void clear_stack_size(void);

extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle, int index);

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

    case idigi_class_data_service:
        status = app_data_service_handler(request_id.data_service_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_firmware:
        status = app_firmware_handler(request_id.firmware_request, request_data, request_length, response_data, response_length);

    default:
        /* not supported */
        break;
    }
    clear_stack_size();

    return status;
}

int application_run(idigi_handle_t handle)
{
    int index = 0;
    int stop_calling = 0;

    while (!stop_calling)
    {
        idigi_status_t const status = send_put_request(handle, index);

        switch (status)
        {
        case idigi_init_error:
        case idigi_service_busy:
           #define SLEEP_ONE_SECOND  1
            app_os_sleep(SLEEP_ONE_SECOND);
            break;

        case idigi_success:
            index++;
            break;

        case idigi_invalid_data_range:
            index = 0;

            #define SLEEP_BETWEEN_TESTS   5
            app_os_sleep(SLEEP_BETWEEN_TESTS);
            break;
    
        default:
            stop_calling = 1;
            break;
        }
    }

    return 0;
}

