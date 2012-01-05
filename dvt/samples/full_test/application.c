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
#include "idigi_dvt.h"

extern void check_stack_size(void);
extern void clear_stack_size(void);

extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle);

idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    check_stack_size();

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
    #define SLEEP_IN_SECONDS  1
    int stop_calling = 0;

    while (!stop_calling)
    {
        if (dvt_current_ptr != NULL)
        {
            switch (dvt_current_ptr->state)
            {
                case dvt_state_request_start:
                {
                    idigi_status_t const status = send_put_request(handle);
        
                    switch (status)
                    {
                        case idigi_success:
                        case idigi_init_error:
                            break;
        
                        default:
                            APP_DEBUG("Exiting main loop because of error [%d]\n", status);
                            stop_calling = 1;
                            break;
                    }

                    break;
                }

                case dvt_state_reset_called:
                    dvt_current_ptr->state = dvt_state_request_start;
                    app_os_sleep(SLEEP_IN_SECONDS); /* let reset complete */
                    break;

                case dvt_state_stop:
                    cleanup_dvt_data();
                    stop_calling = 1;
                    break;

                default:
                    break;
            }
        }

        app_os_sleep(SLEEP_IN_SECONDS);
    }

    return 0;
}

