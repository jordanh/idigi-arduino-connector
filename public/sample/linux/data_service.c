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
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "time.h"
#include "idigi_data.h"

static void initialize_request(idigi_data_request_t * request)
{
    static uint8_t test_data[] = "Welcome to iDigi Data Service sample test!";
    static uint8_t path[]      = "test/sample.txt";
    static uint8_t type[]      = "text/plain";

    request->session                = NULL;
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
    else
        request.session = NULL;

#define DATA_LOG_INTERVAL_IN_SECONDS   20
    if ((current_time - last_time) >= DATA_LOG_INTERVAL_IN_SECONDS) 
    {
        last_time = current_time;
        status = idigi_initiate_action(handle, idigi_initiate_data_service, &request, &request.session);

        DEBUG_PRINTF("Status: %d, Session id: %d\n", status, IDIGI_DATA_GET_SESSION_ID(request.session));
    }

done:
    return status;
}

static uint8_t device_response_data[] = "Data Service Device Response";
static size_t device_response_data_length = sizeof device_response_data -1;
static size_t device_response_count = 2;

typedef struct device_request_handle {
    size_t total_length;
    uint16_t session_id;
    idigi_ds_device_response_t response_info;
    size_t response_data_count;
    bool send_response;
    struct device_request_handle * prev;
    struct device_request_handle * next;
} device_request_handle_t;

device_request_handle_t * device_request_list = NULL;

void delete_device_session(device_request_handle_t * request_handle)
{
    if (request_handle == device_request_list)
    {
        device_request_list = request_handle->next;
    }

    if (request_handle->prev != NULL)
    {
        request_handle->prev->next = request_handle->next;
    }

    if (request_handle->next != NULL)
    {
        request_handle->next->prev = request_handle->prev;
    }
    free(request_handle);
}

idigi_status_t process_device_response(idigi_data_service_request_t const request, void * const request_data)
{
    idigi_status_t status = idigi_success;
    device_request_handle_t * request_handle = NULL;
    bool delete_session = false;

    switch (request)
    {
    case idigi_data_service_device_response:
        for (request_handle = device_request_list; request_handle != NULL; request_handle = request_handle->next)
        {
            if (request_handle->response_data_count > 0 && request_handle->response_info.data_length == 0)
            {   /* setup the response */
                request_handle->response_info.data = (device_response_data_length == 0) ? NULL : device_response_data;
                request_handle->response_info.data_length = device_response_data_length;
                request_handle->response_data_count--;
                request_handle->response_info.flag = (request_handle->response_data_count > 0)? 0 : IDIGI_DATA_REQUEST_LAST;
                request_handle->send_response = true;
            }

            if (request_handle->send_response)
            {
                DEBUG_PRINTF("process_request_response: idigi_data_service_device_response calling idigi_initiate_action for session %d\n",
                                        request_handle->response_info.session_id);
                status = idigi_initiate_action(device_data.idigi_handle, idigi_data_service_device_response, &request_handle->response_info, NULL);
                if (status != idigi_success)
                {
                    DEBUG_PRINTF("process_request_response: idigi_initiate_action for idigi_data_service_device_response for session %d fails with error %d\n",
                            request_handle->response_info.session_id, status);
                    delete_session = true;
                }
                else
                {
                    request_handle->send_response = false;
                }
            }
        }
        break;
    case idigi_data_service_send_complete:
    {
        idigi_data_send_t const * send_info = request_data;
        for (request_handle = device_request_list; request_handle != NULL; request_handle = request_handle->next)
        {
            if (request_handle->session_id == send_info->session_id)
            {
                request_handle->response_info.data_length -= send_info->bytes_sent;
                DEBUG_PRINTF("process_request_response: idigi_data_service_send_complete for session %d (remaining bytes = %lu bytes sent %lu)\n",
                        request_handle->response_info.session_id, (unsigned long int)request_handle->response_info.data_length,
                        (unsigned long int) send_info->bytes_sent);

                if (send_info->status != idigi_success)
                {   /* something's wrong */
                    DEBUG_PRINTF("process_request_response: idigi_data_service_send_complete for session %d fails %d\n", request_handle->session_id, send_info->status);
                    delete_session = true;
                }
                else if (request_handle->response_info.data_length > 0)
                {
                    /* send remaining data */
                    request_handle->response_info.data = device_response_data + send_info->bytes_sent;
                    request_handle->response_info.data_length = device_response_data_length - send_info->bytes_sent;
                    request_handle->response_info.flag = (request_handle->response_data_count > 0) ? 0 : IDIGI_DATA_REQUEST_LAST;
                    request_handle->send_response = true;

                }
                else if (request_handle->response_data_count == 0)
                {   /* done with the session */
                    delete_session = true;
                }
                break;
            }
        }
        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;
        for (request_handle = device_request_list; request_handle != NULL; request_handle = request_handle->next)
        {
            if (request_handle->session_id == error_block->session_id)
            {
                DEBUG_PRINTF("process_request_response: Got idigi_data_service_error on session %d with error %d\n", request_handle->session_id, error_block->error);
                delete_session = true;
            }
        }
        break;
    }
    default:
        DEBUG_PRINTF("process_request_response: invalid process for request %d\n", request);
        break;
    }

    if (delete_session)
    {
        delete_device_session(request_handle);
    }


    return status;
}

int gWait = 0;

static idigi_callback_status_t process_device_request(idigi_ds_device_request_t * const request_data, void ** response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * handle;

    if ((request_data->flag & IDIGI_DATA_REQUEST_START) == IDIGI_DATA_REQUEST_START)
    {
        /* we know this is a new device request since we will return the user_context for more data */
        void * ptr;

        ptr = malloc(sizeof *handle);
        if (ptr == NULL)
        {
            /* no memeory stop IIK */
            status = idigi_callback_abort;
            goto done;
        }

        handle = ptr;
        handle->total_length = 0;
        handle->response_info.session_id = request_data->session_id;
        handle->response_info.data_length = 0;
        /* set to 0 so process_request_response will not start sending response */
        handle->response_data_count = 0;
        handle->next = device_request_list;
        device_request_list = handle;
    }
    else
    {
        ASSERT(request_data->user_context != NULL);
        handle = request_data->user_context;
    }


    ASSERT(handle->response_info.session_id == request_data->session_id);
    handle->total_length += request_data->data_length;

    *response_data = handle;
    DEBUG_PRINTF("process_device_request: data length = %u total length = %u\n", request_data->data_length, handle->total_length);

    if ((request_data->flag & IDIGI_DATA_REQUEST_LAST) == IDIGI_DATA_REQUEST_LAST)
    {   /* No more data (last message) so let's setup to send a response.
         * Set here so process_request_response() will send response
         */
        if (gWait < 3)
        {
            DEBUG_PRINTF("TIMEOUT = %d\n", request_data->timeout);
            usleep(request_data->timeout * 1000 * 1000);
            gWait++;
            status = idigi_callback_busy;
            goto done;
        }
        else
        {
        ASSERT(device_response_count > 0);
        handle->response_data_count = (device_response_data_length) ? device_response_count : 1;
        handle->response_info.status = idigi_data_service_success;
        }
    }

done:
    return status;
}

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
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
        status = (process_device_response(request, request_data) == idigi_success) ? idigi_callback_continue : idigi_callback_abort;
        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;

        UNUSED_PARAMETER(error_block);
        DEBUG_PRINTF("Handle: %d, error: %d\n", error_block->session_id, error_block->error);
        status = (process_device_response(request, request_data) == idigi_success) ? idigi_callback_continue : idigi_callback_abort;
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
    case idigi_data_service_device_request:
        status = process_device_request(request_data, response_data);
        break;

    default:
        break;

    }

    return status;
}
