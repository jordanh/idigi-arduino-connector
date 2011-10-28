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

#include "idigi_api.h"
#include "platform.h"

extern idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle);


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
    case idigi_class_data_service:
        status = idigi_data_service_callback(request_id.firmware_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}



int main (void)
{
    idigi_status_t status;
    idigi_handle_t handle;

    APP_DEBUG("Starting iDigi\n");
    handle = idigi_init((idigi_callback_t)idigi_callback);

    if (handle != NULL)
    {
        static bool first_time = true;

        while (status != idigi_success) 
        {
            status = idigi_step(handle);
            if (first_time) 
            {
                if (is_initialization_complete()) 
                {
                    status = send_put_request(handle);
                    if (status != idigi_service_busy) 
                        first_time = false;
                    else
                        status = idigi_success;
                }
            }
        }
    }

    APP_DEBUG("iDigi stopped running status = [%d]\n", status);
    return 0;
}
