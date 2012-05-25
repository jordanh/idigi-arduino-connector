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
#include <stdlib.h>
#include <stdbool.h>
#include "idigi_api.h"
#include "platform.h"
#include "idigi_dvt.h"

idigi_status_t send_put_request(idigi_handle_t handle, dvt_ds_t * const ds_info)
{
    idigi_status_t status = idigi_success;
    static char file_type[] = "text/plain";
    idigi_data_service_put_request_t * header = NULL;

    ds_info->bytes_sent = 0;
    header = &ds_info->header;
    APP_DEBUG("Sending %s of length %zu\n", ds_info->file_path, (ds_info->test_case != dvt_case_put_request_ds_zero) ? ds_info->file_size : 0);
    switch (ds_info->test_case)
    {
    case dvt_case_put_request_ds_append:
        header->flags = IDIGI_DATA_PUT_APPEND;
        break;

    case dvt_case_put_request_ds_archive:
        header->flags = IDIGI_DATA_PUT_ARCHIVE;
        break;

    case dvt_case_put_request_ds_both:
        header->flags = IDIGI_DATA_PUT_APPEND | IDIGI_DATA_PUT_ARCHIVE;
        break;

    default:
        header->flags = 0;
        break;
    }

    header->path  = ds_info->file_path;
    header->content_type = file_type;
    header->context = ds_info;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, header, NULL);
    if (status == idigi_success)
        ds_info->state = dvt_state_request_progress;
    APP_DEBUG("send_put_request: %s status  %d\n", ds_info->file_path, status);

    return status;
}

idigi_callback_status_t app_put_request_handler(void const * request_data, size_t const request_length,
                                                   void * response_data, size_t * const response_length)
{
    idigi_data_service_msg_request_t const * const put_request = request_data;
    idigi_data_service_msg_response_t * const put_response = response_data;
    idigi_data_service_put_request_t const * const header = put_request->service_context;
    dvt_ds_t * const ds_info = (dvt_ds_t * const)header->context;
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    if ((put_request == NULL) || (put_response == NULL))
    {
         APP_DEBUG("idigi_put_request_callback: Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
         goto done;
    }

    switch (put_request->message_type) 
    {
    case idigi_data_service_type_need_data:
        switch (ds_info->test_case)
        {
        case dvt_case_put_request_ds_busy:
            {
                static bool first_time = true;

                if (first_time) 
                {
                    APP_DEBUG("busy\n");
                    status = idigi_callback_busy;
                    first_time = false;
                    goto done;
                }

                break;
            }

        case dvt_case_put_request_ds_zero:
            {
                idigi_data_service_block_t * message = put_response->client_data;

                message->flags = IDIGI_MSG_FIRST_DATA | IDIGI_MSG_LAST_DATA;
                message->length_in_bytes = 0;
                ds_info->state = dvt_state_request_complete;
                goto done;
            }

        case dvt_case_put_request_ds_cancel_start:
            goto error;

        default:
            break;
        }

        {
            idigi_data_service_block_t * message = put_response->client_data;
            char * dptr = message->data;
            size_t const bytes_available = message->length_in_bytes;
            size_t const bytes_to_send = ds_info->file_size - ds_info->bytes_sent;
            size_t bytes_copy = (bytes_to_send > bytes_available) ? bytes_available : bytes_to_send;

            memcpy(dptr, &ds_info->file_buf[ds_info->bytes_sent], bytes_copy);
            message->length_in_bytes = bytes_copy;
            message->flags = 0;
            if (ds_info->bytes_sent == 0)
            {
                message->flags |= IDIGI_MSG_FIRST_DATA;
            }

            ds_info->bytes_sent += bytes_copy;
            if (ds_info->bytes_sent >= ds_info->file_size)
            {
                switch (ds_info->test_case)
                {
                case dvt_case_put_request_ds_timeout:
                    status = idigi_callback_busy;
                    goto done;

                case dvt_case_put_request_ds_cancel_middle:
                    goto error;

                default:
                    break;
                }

                message->flags |= IDIGI_MSG_LAST_DATA;
                ds_info->state = dvt_state_request_complete;
            }
        }
        break;

    case idigi_data_service_type_have_data:
        {
            idigi_data_service_block_t * message = put_request->server_data;
            uint8_t * const data = message->data;

            if (ds_info->state != dvt_state_request_complete)
            {
                APP_DEBUG("idigi_put_request_callback: got response before complete\n");
                goto error;
            }

            APP_DEBUG("idigi_put_request_callback (have_data): Received %s (0x%x) response from server\n",
                                        (((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error"), (unsigned)message->flags);
            if (message->length_in_bytes > 0)
            {
                data[message->length_in_bytes] = '\0';
                APP_DEBUG("idigi_put_request_callback: server response %s\n", (char *)data);
            }

            ds_info->state = ((message->flags & IDIGI_MSG_RESP_SUCCESS) == IDIGI_MSG_RESP_SUCCESS) ? dvt_state_response_complete : dvt_state_request_error;
            goto done;
        }

    case idigi_data_service_type_error:
        {
            idigi_data_service_block_t * message = put_request->server_data;
            idigi_msg_error_t const * const error_value = message->data;

            if (error_value == NULL)
            {
                APP_DEBUG("idigi_put_request_callback: Data service error %d\n", *error_value);
            }

            goto error;
        }
    
    default:
        APP_DEBUG("idigi_put_request_callback: Unexpected message type: %d\n", put_request->message_type);
        break;
    }
    goto done;

error:
    APP_DEBUG("idigi_put_request_callback error: target[%s] cancel this session\n", ds_info->file_path);
    put_response->message_status = idigi_msg_error_cancel;
    ds_info->state = dvt_state_request_error;

done:
    return status;
}

/* supported targets */
typedef enum
{
    dvt_case_ds_valid_target,
    dvt_case_ds_invalid_target,
    dvt_case_ds_cancel_request,
    dvt_case_ds_cancel_in_middle,
    dvt_case_ds_cancel_response,
    dvt_case_ds_busy_response,
    dvt_case_ds_busy_request,
    dvt_case_ds_not_handle,
    dvt_case_ds_zero_byte,
    dvt_case_ds_timeout_response,
    dvt_case_ds_start_put_request,
    dvt_case_ds_query_put_request,
    dvt_case_ds_last
} dvt_case_ds_t;

typedef struct device_request_handle 
{
    dvt_case_ds_t test_case;
    char * target;
    size_t length_in_bytes;
    char * data;
    size_t bytes_sent;
} device_request_handle_t;

static device_request_handle_t device_request_targets[] =
{ 
    {dvt_case_ds_valid_target, "valid target", 0, NULL, 0},
    {dvt_case_ds_invalid_target, "invalid target", 0, NULL, 0},
    {dvt_case_ds_cancel_request, "cancel request", 0, NULL, 0},
    {dvt_case_ds_cancel_in_middle, "cancel in middle", 0, NULL, 0},
    {dvt_case_ds_cancel_response, "cancel response", 0, NULL, 0},
    {dvt_case_ds_busy_request, "busy request", 0, NULL, 0},
    {dvt_case_ds_busy_response, "busy response", 0, NULL, 0},
    {dvt_case_ds_not_handle, "not handle", 0, NULL, 0},
    {dvt_case_ds_zero_byte, "zero byte data", 0, NULL, 0},
    {dvt_case_ds_timeout_response, "timeout response", 0, NULL, 0},
    {dvt_case_ds_start_put_request, "start put request", 0, NULL, 0},
    {dvt_case_ds_query_put_request, "query put request", 0, NULL, 0}
};

#define DVT_DS_MAX_DATA_SIZE  0x8000

static void start_put_request(device_request_handle_t * target_info, idigi_data_service_block_t * server_data)
{
    dvt_ds_t * const ds_info = &data_service_info;

    if (ds_info->state != dvt_state_init)
    {
        APP_DEBUG("start_put_request: unexpected state %d for %s\n", ds_info->state, ds_info->file_path);
        goto done;
    }

    if (server_data->length_in_bytes > sizeof ds_info->file_path)
    {
        APP_DEBUG("start_put_request: unexpected data length %zu\n", server_data->length_in_bytes);
        goto done;
    }


    memcpy(ds_info->file_path, server_data->data, server_data->length_in_bytes);
    ds_info->file_path[server_data->length_in_bytes] = '\0';
    ds_info->test_case = atoi(&ds_info->file_path[strlen("test/dvt_ds")]);
    target_info->data = ds_info->file_path;

done:
    return;
}

static void prepare_put_request_response(device_request_handle_t * target_info, idigi_data_service_block_t * client_data)
{
    dvt_ds_t * const ds_info = &data_service_info;
    static char const ds_error[] = "error";
    char const * resp = ds_error;

    switch (target_info->test_case)
    {
    case dvt_case_ds_start_put_request:
        resp = ds_info->file_path;
        ds_info->state = dvt_state_request_start;
        break;

    case dvt_case_ds_query_put_request:
        {
            switch (ds_info->state)
            {
            case dvt_state_request_start:
            case dvt_state_request_progress:
            case dvt_state_request_complete:
                {
                    static char const ds_wait[] = "wait";

                    resp = ds_wait;
                }
                break;

            case dvt_state_response_complete:
                {
                    static char const ds_done[] = "done";

                    resp = ds_done;
                    ds_info->state = dvt_state_init;
                }
                break;

            default:
                ds_info->state = dvt_state_init;
                break;
            }
        }
        break;

    default:
        ds_info->state = dvt_state_init;
        break;
    }

    APP_DEBUG("Sending response [%s] state[%d]\n", resp, ds_info->state);
    client_data->length_in_bytes = strlen(resp);
    memcpy(client_data->data, resp, client_data->length_in_bytes);
}

static device_request_handle_t * get_test_case_record (char const * const target)
{
    dvt_case_ds_t test_case;
    device_request_handle_t * record = NULL; 

    for (test_case = dvt_case_ds_valid_target; test_case < dvt_case_ds_last; test_case++) 
    {
        if (!strcmp(target, device_request_targets[test_case].target))
        {
            record = &device_request_targets[test_case];
            break;
        }
    }

    return record;
}

static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t * const request_info = request_data->service_context;
    device_request_handle_t * target_info = response_data->user_context;
    idigi_data_service_block_t * server_data = request_data->server_data;

    ASSERT(server_data != NULL);
    ASSERT(service_device_request != NULL);

    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        target_info = get_test_case_record(request_info->target);

        if (target_info == NULL)
        {
            /* no memeory stop iDigi Connector */
            APP_DEBUG("process_device_request: unknown target %s\n", request_info->target);
            goto cancel;
        }

        target_info->bytes_sent = 0;
        target_info->length_in_bytes = 0;
        response_data->user_context = target_info;
    }

    switch (target_info->test_case) 
    {
    case dvt_case_ds_invalid_target:
        goto cancel;

    case dvt_case_ds_cancel_request:
        if (target_info->length_in_bytes > 0)
            goto cancel;
        break;

    case dvt_case_ds_busy_response:
        {
            static bool first_time = true;

            if (first_time && (target_info->length_in_bytes > 0))
            {
                APP_DEBUG("busy\n");
                status = idigi_callback_busy;
                first_time = false;
                goto done;
            }
        }
        break;

    case dvt_case_ds_start_put_request:
        {
            dvt_ds_t * const ds_info = &data_service_info;

            start_put_request(target_info, server_data);
            APP_DEBUG("Received start %s\n", ds_info->file_path);
            response_data->message_status = idigi_msg_error_none;
        }
        goto done;

    case dvt_case_ds_query_put_request:
        {
            dvt_ds_t * const ds_info = &data_service_info;

            APP_DEBUG("Received query %s\n", ds_info->file_path);
            if (strncmp(server_data->data, ds_info->file_path, server_data->length_in_bytes))
            {
                APP_DEBUG("Received query %s, mismatch \n", ds_info->file_path);
                ds_info->state = dvt_state_request_error;
            }

            response_data->message_status = idigi_msg_error_none;
        }
        goto done;

    default:
        break;
    }

    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        target_info->data = malloc(DVT_DS_MAX_DATA_SIZE);
        if (target_info->data == NULL) 
        {
            APP_DEBUG("process_device_request: malloc failed %s\n", request_info->target);
            response_data->message_status = idigi_msg_error_memory;
            goto clear;
        }
    }

    if ((server_data->length_in_bytes > 0) && ((target_info->length_in_bytes + server_data->length_in_bytes) < DVT_DS_MAX_DATA_SIZE))
    {
        memcpy(&target_info->data[target_info->length_in_bytes], server_data->data, server_data->length_in_bytes);
        target_info->length_in_bytes += server_data->length_in_bytes;
    }

    response_data->message_status = idigi_msg_error_none;
    goto done;

cancel:
    response_data->message_status = idigi_msg_error_cancel;

clear:
    if (target_info != NULL && target_info->data != NULL)
    {
        free(target_info->data);
        target_info->data = NULL;
    }

done:
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * const target_info = response_data->user_context;
    idigi_data_service_block_t * const client_data = response_data->client_data;

    ASSERT(response_data->client_data != NULL);
    ASSERT(target_info != NULL); /* we use user_context for our client_device_request */

    client_data->flags = (target_info->bytes_sent == 0) ? IDIGI_MSG_FIRST_DATA : 0;
    response_data->message_status = idigi_msg_error_none;
    switch (target_info->test_case) 
    {
    case dvt_case_ds_busy_response:
        {
            static bool first_time = true;

            if (first_time && (target_info->bytes_sent > 0))
            {
                APP_DEBUG("busy\n");
                status = idigi_callback_busy;
                first_time = false;
                goto done;
            }
        }
        break;

    case dvt_case_ds_cancel_in_middle:
        goto cancel;

    case dvt_case_ds_cancel_response:
        if (target_info->bytes_sent > target_info->length_in_bytes/2) 
            goto cancel;
        break;

    case dvt_case_ds_timeout_response:
        if (target_info->bytes_sent > target_info->length_in_bytes/2)
        {
            status = idigi_callback_busy;
            goto done;
        }
        break;

    case dvt_case_ds_not_handle:
        client_data->flags |= (IDIGI_MSG_LAST_DATA | IDIGI_MSG_DATA_NOT_PROCESSED);
        client_data->length_in_bytes = 0;
        goto clear;

    case dvt_case_ds_start_put_request:        
    case dvt_case_ds_query_put_request:
        prepare_put_request_response(target_info, client_data);
        client_data->flags |= IDIGI_MSG_LAST_DATA;
        goto done;

    default:
        break;
    }

    {
        size_t const bytes_to_send = target_info->length_in_bytes - target_info->bytes_sent;
        size_t const bytes = (bytes_to_send < client_data->length_in_bytes) ? bytes_to_send : client_data->length_in_bytes;

        /* let's copy the response data to service_response buffer */
        if (bytes_to_send > 0) 
        {
            memcpy(client_data->data, &target_info->data[target_info->bytes_sent], bytes);
            target_info->bytes_sent += bytes;
        }
        else
            APP_DEBUG("Sending 0 bytes\n");

        client_data->length_in_bytes = bytes;
        if (target_info->bytes_sent >= target_info->length_in_bytes)
        {
            client_data->flags |= IDIGI_MSG_LAST_DATA;
            goto clear;
        }
    }

    goto done;

cancel:
    response_data->message_status = idigi_msg_error_cancel;

clear:
    if (target_info != NULL && target_info->data != NULL)
    {
        free(target_info->data);
        target_info->data = NULL;
    }

done:
    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
                                                    idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * const target_info = response_data->user_context;
    idigi_data_service_block_t * error_data = request_data->server_data;
    idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);

    APP_DEBUG("process_device_error: target [%s] error [%d] from server\n", target_info->target, error_code);

    if (target_info != NULL && target_info->data != NULL)
    {
        free(target_info->data);
        target_info->data = NULL;
    }

    return status;
}

idigi_callback_status_t app_device_request_handler(void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_msg_request_t const * const service_device_request = request_data;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (service_device_request->message_type)
    {
    case idigi_data_service_type_have_data:
        status = process_device_request(request_data, response_data);
        break;

    case idigi_data_service_type_need_data:
        status = process_device_response(response_data);
        break;

    case idigi_data_service_type_error:
        status = process_device_error(request_data, response_data);
        break;

    default:
        APP_DEBUG("idigi_put_request_callback: unknown message type %d for idigi_data_service_device_request\n", service_device_request->message_type);
        break;
    }

    return status;
}

idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    switch (request)
    {
    case idigi_data_service_put_request:
        status = app_put_request_handler(request_data, request_length, response_data, response_length);
        break;

    case idigi_data_service_device_request:
        status = app_device_request_handler(request_data, request_length, response_data, response_length);
        break;

    default:
        APP_DEBUG("idigi_data_service_callback: Request not supported: %d\n", request);
        break;
    }

    return status;
}

