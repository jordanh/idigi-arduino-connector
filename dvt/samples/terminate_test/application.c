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

char terminate_file_content[32];

extern idigi_status_t idigi_run_thread_status;


extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle, int index);
extern idigi_status_t send_file(idigi_handle_t handle, int index, char * const filename,
                                char * const content, size_t content_length);

idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
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

    return status;
}

int application_run(idigi_handle_t handle)
{
    idigi_status_t status;
    int index = 0;

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

    for (;idigi_run_thread_status != idigi_device_terminated;)
    {
        status = send_put_request(handle, index);

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
        case idigi_device_terminated:
            goto done;
        default:
            break;
        }
    }

done:
    return 0;
}


