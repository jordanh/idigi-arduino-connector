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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "idigi_data.h"

#define STEP_DELAY_IN_MS  100

device_data_t device_data;


idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
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
    case idigi_class_firmware:
        status = idigi_firmware_callback(request_id.firmware_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_data_service:
        status = idigi_data_service_callback(request_id.data_service_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}


idigi_status_t data_service_process_device_response(void);

int main (void)
{
    idigi_status_t status = idigi_success;

    time(&device_data.start_system_up_time);
    device_data.idigi_handle = NULL;
    device_data.select_data = 0;
    device_data.socket_fd = INADDR_NONE;

    DEBUG_PRINTF("Starting iDigi\n");
    device_data.idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
    if (device_data.idigi_handle != 0)
    {

        while (status == idigi_success)
        {
            struct timeval delay = { .tv_sec = 0, .tv_usec = STEP_DELAY_IN_MS };
            status = idigi_step(device_data.idigi_handle);
            device_data.select_data = 0;

            if (status == idigi_success)
            {
                if (device_data.socket_fd != (int)INADDR_NONE)
                {
                    device_data.select_data |= NETWORK_TIMEOUT_SET | NETWORK_READ_SET;
                    network_select(device_data.socket_fd, device_data.select_data, &delay);
#if (defined IDIGI_DATA_SERVICE)
                    if (device_data.connected)
                    {
                        status = initiate_data_service(device_data.idigi_handle);
                        if (status != idigi_success)
                        {
                            DEBUG_PRINTF("initiate_data_service fails %d\n", status);
                            status = idigi_success; /* continue to run IIK */
                        }
                    }
#endif
                }
                else
                {
                    sleep(1);
                }
            }
            else if (status != idigi_device_terminated)
            {
                /* let's terminate IIK. Must execute idigi_step to terminate itself. */
                status = idigi_initiate_action(device_data.idigi_handle, idigi_initiate_terminate, NULL, NULL);

            }
        }
        DEBUG_PRINTF("idigi status = %d\n", status);
    }

    DEBUG_PRINTF("iDigi stopped running!\n");
    return 0;
}
