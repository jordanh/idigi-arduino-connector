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
#include "application.h"

extern void clear_stack_size(void);

extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle, char * const filename, char * const content);

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
    idigi_status_t status;
    static char reboot_result[32];

    for (;;)
    {
        int send_reboot_result = 0;

        switch (reboot_state)
        {
        case reboot_received:
            strcpy(reboot_result, "reboot_ok");
            send_reboot_result = 1;
            break;
        case receive_reset:
            strcpy(reboot_result, "reboot_reset");
            send_reboot_result = 1;
            break;
        case receive_error:
            sprintf(reboot_result, "reboot_receive_error = %d", reboot_errno);
            send_reboot_result = 1;
            break;
        case send_error:
            sprintf(reboot_result, "reboot_send_error = %d", reboot_errno);
            send_reboot_result = 1;
            break;
        case reboot_timeout:
            sprintf(reboot_result, "reboot_timeout");
            send_reboot_result = 1;
            break;
        default:
            app_os_sleep(5);
            break;
        }

        if (send_reboot_result)
        {
            status = send_put_request(handle, "reboot_test.txt", reboot_result);
            if (status != idigi_success)
            {
                app_os_sleep(2);
            }
            else
            {
                APP_DEBUG("send_put_request for reboot_test.txt %d\n", status);
            }
        }

    }
    return 0;
}

