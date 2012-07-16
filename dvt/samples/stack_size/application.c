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
#include "application.h"
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"


idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void const * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status;

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
        break;

    case idigi_class_file_system:
        status = app_file_system_handler(request_id.file_system_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_remote_config_service:
        status = app_remote_config_handler(request_id.remote_config_request, request_data, request_length, response_data, response_length);
        break;

    default:
        status = idigi_callback_unrecognized;
        /* not supported */
        break;
    }

    clear_stack_size();

    return status;
}

int application_run(idigi_handle_t handle)
{
    idigi_status_t status;

#if 0
    size_t file_length = file_length = strlen(terminate_file_content);

     app_os_sleep(5);

     while (terminate_flag == device_request_terminate_done  && file_length > 0)
     {

         status = send_file(handle, 255, (char * const)TERMINATE_TEST_FILE, (char * const)terminate_file_content, file_length);
         if (status != idigi_success)
         {
             APP_DEBUG("application_run: unable to send terminate_test.txt %d\n", status);
             app_os_sleep(2);
         }
         else
         {
             /* assume it's done sending */
             APP_DEBUG("application_run: sent terminate_test.txt\n");
             break;
         }
     }
#endif

    for (;idigi_run_thread_status != idigi_device_terminated;)
    {
        status = send_put_request(handle);

        switch (status)
        {
        case idigi_init_error:
        case idigi_service_busy:
           #define SLEEP_ONE_SECOND  1
            app_os_sleep(SLEEP_ONE_SECOND);
            break;

        case idigi_success:
            break;

        case idigi_invalid_data_range:
             #define SLEEP_BETWEEN_TESTS   1
            app_os_sleep(SLEEP_BETWEEN_TESTS);
            break;
        case idigi_device_terminated:
            goto done;
        default:
            break;
        }
    }
    APP_DEBUG("application_run: done!\n");
done:
    return 0;
}


