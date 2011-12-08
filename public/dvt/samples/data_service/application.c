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


extern void check_stack_size(void);

extern idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t idigi_firmware_callback(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle, int index);

idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    check_stack_size();

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
        status = idigi_data_service_callback(request_id.data_service_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_firmware:
        status = idigi_firmware_callback(request_id.firmware_request, request_data, request_length, response_data, response_length);

    default:
        /* not supported */
        break;
    }
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
            #define SLEEP_ONE_SECOND  1
            os_sleep(SLEEP_ONE_SECOND);
            break;

        case idigi_success:
            index++;
            break;

        case idigi_invalid_data_range:
            index = 0;

            #define SLEEP_BETWEEN_TESTS   20
            os_sleep(SLEEP_BETWEEN_TESTS);
            break;
    
        default:
            stop_calling = 1;
            break;
        }
    }
    return 0;
}

