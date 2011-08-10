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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "time.h"

#include "idigi_data.h"

static void initialize_request(idigi_data_request_t * request)
{
    static uint8_t test_data[] = "Welcome to iDigi Data Service sample test!";
    static uint8_t path[] = "test/sample.txt";
    static uint8_t type[] = "text/plain";

    request->flag                   = IDIGI_DATA_REQUEST_START | IDIGI_DATA_REQUEST_LAST | IDIGI_DATA_REQUEST_COMPRESSED;
    request->path.size              = sizeof path - 1;
    request->path.value             = path;
    request->content_type.size      = sizeof type - 1;
    request->content_type.value     = type;
    request->payload.size           = sizeof test_data - 1;
    request->payload.data           = test_data;
}

idigi_status_t initiate_data_service(idigi_handle_t handle) 
{
    idigi_status_t status = idigi_success;
    static time_t last_time = 0;
    time_t current_time;
    static idigi_data_request_t request;
    
    time(&current_time);
    if (last_time == 0) 
    {
        initialize_request(&request);
        last_time = current_time;
        goto done;
    }

#define DATA_LOG_INTERVAL_IN_SECONDS   30
    if ((current_time - last_time) >= DATA_LOG_INTERVAL_IN_SECONDS) 
    {
        last_time = current_time;
        status = idigi_initiate_action(handle, idigi_initiate_data_service, &request, &request.session);

        DEBUG_PRINTF("Status: %d, Session: %p\n", status, request.session);
    }

done:
    return status;
}

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t request,
                                                  void const * request_data, size_t request_length,
                                                  void * response_data, size_t * response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_data);
    UNUSED_PARAMETER(response_length);

    switch (request)
    {
    case idigi_data_service_send_complete:
    {
        idigi_data_send_t const * send_info = request_data;

        UNUSED_PARAMETER(send_info);
        DEBUG_PRINTF("Handle: %d, status: %d, sent: %d bytes\n", send_info->session_id, send_info->status, send_info->bytes_sent);
        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;

        UNUSED_PARAMETER(error_block);
        DEBUG_PRINTF("Handle: %d, error: %d\n", error_block->session_id, error_block->error);
        break;
    }

    case idigi_data_service_response:
    {
        idigi_data_response_t const * response = request_data;
        char * data = (char *)response->message.value;

        UNUSED_PARAMETER(response);
        data[response->message.size] = '\0';
        DEBUG_PRINTF("Handle: %d, status: %d, message: %s\n", response->session_id, response->status, data);
        break;
    }

    default:
        break;

    }

    return status;
}
