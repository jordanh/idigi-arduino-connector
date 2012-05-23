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

#include "idigi_api.h"
#include "platform.h"
#include "os_support.h"
#include "data_service.h"

typedef struct
{
    char * target;
    void * context;
} idigi_connector_request_t;

static void free_connector_request(idigi_connector_request_t * request)
{
    if (request != NULL)
    {
        if (request->target != NULL)
            ic_free(request->target);
        ic_free(request);
    }
}

static idigi_connector_error_t map_msg_error_to_user_error(idigi_msg_error_t const msg_error)
{
    idigi_connector_error_t user_error;

    APP_DEBUG("Received connector msg error: %d\n", msg_error);
    switch (msg_error)
    {
        case idigi_msg_error_none:
            user_error = idigi_connector_success;
            break;

        case idigi_msg_error_fatal:
        case idigi_msg_error_memory:
        case idigi_msg_error_no_service:
            user_error = idigi_connector_resource_error;
            break;

        case idigi_msg_error_invalid_opcode:
        case idigi_msg_error_format:
            user_error = idigi_connector_invalid_parameter;
            break;

        case idigi_msg_error_session_in_use:
        case idigi_msg_error_unknown_session:
        case idigi_msg_error_cancel:
            user_error = idigi_connector_session_error;
            break;

        case idigi_msg_error_compression_failure:
        case idigi_msg_error_decompression_failure:
            user_error = idigi_connector_compression_error;
            break;

        case idigi_msg_error_send:
        case idigi_msg_error_ack:
            user_error = idigi_connector_network_error;
            break;

        case idigi_msg_error_busy:
        case idigi_msg_error_timeout:
            user_error = idigi_connector_timeout;
            break;

        default:
            user_error = idigi_connector_init_error;
            break;
    }

    return user_error;
}

static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_data_service_device_request_t * const server_device_request = request_data->service_context;
    idigi_data_service_block_t * const server_data = request_data->server_data;
    idigi_connector_callbacks_t * const app_callbacks = idigi_get_app_callbacks();

    if (app_callbacks->device_request != NULL)
    {
        idigi_app_error_t result;
        idigi_connector_data_t app_data;
        idigi_connector_request_t * connector_request;

        if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
        {
            connector_request = ic_malloc(sizeof(idigi_connector_request_t));
            ASSERT_GOTO(connector_request != NULL, error);

            connector_request->target = ic_malloc(strlen(server_device_request->target) + 1);
            ASSERT_GOTO(connector_request->target != NULL, error);

            strcpy(connector_request->target, server_device_request->target);
            connector_request->context = NULL;
            response_data->user_context = connector_request;
        }
        else
            connector_request = response_data->user_context;

        app_data.error = idigi_connector_success;
        app_data.data_ptr = server_data->data;
        app_data.length_in_bytes = server_data->length_in_bytes;
        app_data.flags = ((server_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA) ? IDIGI_FLAG_LAST_DATA : 0;
        app_data.app_context = connector_request->context;

        result = app_callbacks->device_request(connector_request->target, &app_data);
        switch (result)
        {
            case idigi_app_success:
                connector_request->context = app_data.app_context;
                status = idigi_callback_continue;
                break;

            case idigi_app_busy:
                status = idigi_callback_busy;
                break;

            case idigi_app_unknown_target:
                response_data->message_status = idigi_msg_error_cancel;
                status = idigi_callback_continue;
                break;

            default:
                APP_DEBUG("process_device_request: application error [%d]\n", result);
                break;
        }
    }
    else
    {
        APP_DEBUG("process_device_request: callback is not registered\n");
        response_data->message_status = idigi_msg_error_cancel;
        status = idigi_callback_continue;
    }

error:
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_service_msg_request_t const * const request_data,
                                                       idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_block_t * const client_data = response_data->client_data;
    idigi_connector_callbacks_t * const app_callbacks = idigi_get_app_callbacks();

    UNUSED_ARGUMENT(request_data);
    if (app_callbacks->device_response != NULL)
    {
        size_t response_bytes;
        idigi_connector_data_t app_data;
        idigi_connector_request_t * const connector_request = response_data->user_context;

        app_data.error = idigi_connector_success;
        app_data.data_ptr = client_data->data;
        app_data.length_in_bytes = client_data->length_in_bytes;
        app_data.flags = ((client_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA) ? IDIGI_FLAG_LAST_DATA : 0;
        app_data.app_context = connector_request->context;

        response_bytes = app_callbacks->device_response(connector_request->target, &app_data);
        if (app_data.error == idigi_connector_success)
        {
            client_data->length_in_bytes = response_bytes;
            if ((app_data.flags & IDIGI_FLAG_LAST_DATA) == IDIGI_FLAG_LAST_DATA)
            {
                client_data->flags = IDIGI_MSG_LAST_DATA;
                free_connector_request(connector_request);
            }
            else
                status = (response_bytes == 0) ? idigi_callback_busy : idigi_callback_continue;
        }
        else
        {
            APP_DEBUG("process_device_response: app returned error\n", app_data.error);
            client_data->flags = IDIGI_MSG_DATA_NOT_PROCESSED;
            free_connector_request(connector_request);
        }
    }
    else
    {
        APP_DEBUG("process_device_response: callback is not registered\n");
        client_data->flags = IDIGI_MSG_DATA_NOT_PROCESSED;
        status = idigi_callback_continue;
    }

    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
                                                    idigi_data_service_msg_response_t * const response_data)
{
    idigi_data_service_block_t * error_data = request_data->server_data;
    idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);
    idigi_connector_request_t * const connector_request = response_data->user_context;
    idigi_connector_callbacks_t * const app_callbacks = idigi_get_app_callbacks();

    /*APP_DEBUG("process_device_error: target %s error %d from server\n", request_data->server_data, error_code);*/
    APP_DEBUG("process_device_error\n");

    if (app_callbacks->device_response != NULL)
    {
        size_t response_bytes;
        idigi_connector_data_t app_data;

        app_data.error = map_msg_error_to_user_error(error_code);
        app_data.data_ptr = NULL;
        response_bytes = app_callbacks->device_response(connector_request->target, &app_data);
        ASSERT(response_bytes == 0);
    }

    free_connector_request(connector_request);
    return idigi_callback_continue;
}


static void send_data_completed(idigi_app_send_data_t * const app_dptr, idigi_connector_error_t const error_code)
{
    app_dptr->error = error_code;
    ic_set_event(IC_SEND_DATA_EVENT);
}

static idigi_callback_status_t process_send_data_request(idigi_data_service_msg_request_t const * const request_data,
                                                         idigi_data_service_msg_response_t * const response_data)
{
    idigi_data_service_msg_request_t const * const put_request = request_data;
    idigi_data_service_msg_response_t * const put_response = response_data;
    idigi_data_service_put_request_t const * const header = put_request->service_context;
    idigi_app_send_data_t * const app_dptr = (idigi_app_send_data_t * const)header->context;

    if (app_dptr != NULL)
    {
        idigi_data_service_block_t * const message = put_response->client_data;

        if (app_dptr->bytes_remaining < message->length_in_bytes)
        {
            message->length_in_bytes = app_dptr->bytes_remaining;
            message->flags = IDIGI_MSG_LAST_DATA;
        }

        memcpy(message->data, app_dptr->next_data, message->length_in_bytes);
        app_dptr->bytes_remaining -= message->length_in_bytes;
        app_dptr->next_data = ((char *)app_dptr->next_data) + message->length_in_bytes;
        put_response->message_status = idigi_msg_error_none;
    }
    else
    {
        APP_DEBUG("process_send_data_request: no app data set to send\n");
        put_response->message_status = idigi_msg_error_unknown_session;
    }

    return idigi_callback_continue;
}

static idigi_callback_status_t process_send_data_response(idigi_data_service_msg_request_t const * const request_data)
{
    idigi_data_service_msg_request_t const * const put_request = request_data;
    idigi_data_service_block_t * const message = put_request->server_data;
    idigi_data_service_put_request_t const * const header = put_request->service_context;
    idigi_app_send_data_t * const app_dptr = (idigi_app_send_data_t * const)header->context;
    idigi_connector_error_t error_code;
    
    if ((message->flags & IDIGI_MSG_RESP_SUCCESS) == IDIGI_MSG_RESP_SUCCESS)
    {
        error_code = idigi_connector_success;
    }
    else
    {
        error_code = ((message->flags & IDIGI_MSG_BAD_REQUEST) == IDIGI_MSG_BAD_REQUEST) ? idigi_connector_invalid_parameter : idigi_connector_service_unavailable;
        if (message->length_in_bytes > 0)
        {
            char * const data = message->data;

            data[message->length_in_bytes] = '\0';
            APP_DEBUG("Cloud response %s\n", data);
        }
    }

    send_data_completed(app_dptr, error_code);
    return idigi_callback_continue;
}

static idigi_callback_status_t process_send_data_error(idigi_data_service_msg_request_t const * const request_data)
{
    idigi_data_service_msg_request_t const * const put_request = request_data;
    idigi_data_service_put_request_t const * const header = put_request->service_context;
    idigi_app_send_data_t * const app_dptr = (idigi_app_send_data_t * const)header->context;
    idigi_data_service_block_t const * const message = put_request->server_data;
    idigi_msg_error_t const * const error_value = message->data;
    idigi_connector_error_t const error_code = map_msg_error_to_user_error(*error_value);

    APP_DEBUG("Send data error: %d\n", *error_value);
    send_data_completed(app_dptr, error_code);
    
    return idigi_callback_continue;
}

idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                 void const * const request_data, size_t const request_length,
                                                 void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_msg_request_t const * const data_service_request = request_data;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
        case idigi_data_service_device_request:
            switch (data_service_request->message_type)
            {
                case idigi_data_service_type_have_data:
                    status = process_device_request(request_data, response_data);
                    break;

                case idigi_data_service_type_need_data:
                    status = process_device_response(request_data, response_data);
                    break;

                case idigi_data_service_type_error:
                    status = process_device_error(request_data, response_data);
                    break;

                default:
                    APP_DEBUG("app_data_service_handler: unknown message type %d for idigi_data_service_device_request\n", data_service_request->message_type);
                    break;
            }
            break;

        case idigi_data_service_put_request:
            switch (data_service_request->message_type)
            {
                case idigi_data_service_type_need_data:
                    status = process_send_data_request(request_data, response_data);
                    break;

                case idigi_data_service_type_have_data:
                    status = process_send_data_response(request_data);
                    break;

                case idigi_data_service_type_error:
                    status = process_send_data_error(request_data);
                    break;

                default:
                    APP_DEBUG("app_data_service_handler: unknown message type %d for idigi_data_service_put_request\n", data_service_request->message_type);
                    break;
            }
            break;

        default:
            APP_DEBUG("app_data_service_handler: unknown request %d\n", request);
    }

    return status;
}

