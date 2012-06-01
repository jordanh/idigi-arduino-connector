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

#include "idigi_api.h"
#include "platform.h"
#include "application.h"
#include "idigi_config.h"


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
        break;

    case idigi_class_file_system:
        status = app_file_system_handler(request_id.file_system_request, request_data, request_length, response_data, response_length);
        break;

    default:
        /* not supported */
        break;
    }
    clear_stack_size();

    return status;
}

int application_run(idigi_handle_t handle)
{
#if defined IDIGI_DATA_SERVICE && DS_MAX_USER > 0
    int index = 0;
    int stop_calling = 0;

    while (!stop_calling)
    {
        idigi_status_t const status = send_put_request(handle, index);

        switch (status)
        {
        case idigi_init_error:
            if (idigi_run_thread_status == idigi_device_terminated) 
            {
                stop_calling = 1;
                break;
            }
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

        case idigi_device_terminated:
        default:
            stop_calling = 1;
            break;
        }
    }
#else
    UNUSED_ARGUMENT(handle);
#endif
    return 0;
}

