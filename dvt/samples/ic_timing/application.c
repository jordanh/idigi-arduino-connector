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
#include <time.h>
#include <errno.h>
#include "idigi_api.h"
#include "platform.h"
#include "application.h"
#include "idigi_config.h"

idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void const * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    struct timespec start_time;
    struct timespec end_time;

    get_time(&start_time);
    callback_count++;

//    APP_DEBUG("app_idigi_callback: class %d id %d\n", class_id, request_id.config_request);

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

    get_time(&end_time);

    total_elapsed_time(&callback_time, start_time, end_time);

    return status;
}

int application_step(idigi_handle_t handle)
{
#if defined IDIGI_DATA_SERVICE
    send_put_request(handle);
#else
    UNUSED_ARGUMENT(handle);
#endif
    return 0;
}

