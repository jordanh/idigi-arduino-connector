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
#include "application.h"

#define PUT_REQUEST_TEST_ERROR

#define INITIAL_WAIT_COUNT      4

#define DS_FILE_NAME_LEN  20

typedef struct
{
    idigi_data_service_put_request_t header;
    char file_path[DS_FILE_NAME_LEN];
    size_t bytes_sent;
    size_t file_length_in_bytes;
    char * file_data;
    int index;
} ds_record_t;

unsigned int put_file_active_count = 0;

idigi_status_t send_put_request(idigi_handle_t handle, char * const filename, char * const content)
{

    idigi_status_t status = idigi_success;
    static char file_type[] = "text/plain";
    ds_record_t * user;

    if (put_file_active_count != 0)
    {
        status = idigi_service_busy;
        goto done;
    }
    {
        void * ptr;

        ptr = malloc(sizeof *user);
        if (ptr == NULL)
        {
            /* no memeory stop iDigi Connector */
            APP_DEBUG("send_put_request: malloc fails\n");
            status = idigi_no_resource;
            goto done;
        }
        user = ptr;
    }

    sprintf(user->file_path, "%s", filename);
    user->header.flags = 0;
    user->header.path  = user->file_path;
    user->header.content_type = file_type;
    user->header.context = user;
    user->bytes_sent = 0;
    user->file_data = content;
    user->file_length_in_bytes = strlen(content);

    status = idigi_initiate_action(handle, idigi_initiate_data_service, &user->header, NULL);
    if (status == idigi_success)
    {
        put_file_active_count++;
    }
    else
    {
        free(user);
    }

done:
    return status;
}

idigi_callback_status_t app_put_request_handler(void const * request_data, size_t const request_length,
                                                   void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    {
        idigi_data_service_msg_request_t const * const put_request = request_data;
        idigi_data_service_msg_response_t * const put_response = response_data;

        idigi_data_service_put_request_t const * const header = put_request->service_context;
        ds_record_t * const user = (ds_record_t * const)header->context;

        if ((put_request == NULL) || (put_response == NULL))
        {
             APP_DEBUG("app_put_request_handler: Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
             goto done;
        }

        switch (put_request->message_type)
        {
        case idigi_data_service_type_need_data:
             {

                idigi_data_service_block_t * message = put_response->client_data;
                char * dptr = message->data;
                size_t const bytes_available = message->length_in_bytes;
                size_t const bytes_to_send = user->file_length_in_bytes - user->bytes_sent;
                size_t bytes_copy = (bytes_to_send > bytes_available) ? bytes_available : bytes_to_send;

                memcpy(dptr, &user->file_data[user->bytes_sent], bytes_copy);
                message->length_in_bytes = bytes_copy;
                message->flags = 0;
                if (user->bytes_sent == 0)
                    message->flags |= IDIGI_MSG_FIRST_DATA;

                user->bytes_sent += bytes_copy;
                if (user->bytes_sent == user->file_length_in_bytes)
                {
                    message->flags |= IDIGI_MSG_LAST_DATA;
                    APP_DEBUG("app_put_request_handle: (need_data) done sending %s file\n", user->file_path);
                }

            }
            break;

        case idigi_data_service_type_have_data:
            {
                idigi_data_service_block_t * message = put_request->server_data;
                uint8_t const * data = message->data;

                ASSERT(user != NULL);

                if (message->length_in_bytes > 0)
                {
                    APP_DEBUG("app_put_request_handler: (have_data) server response for %s file: %s\n",
                            user->file_path, (char *)data);
                }
                if ((message->flags & IDIGI_MSG_RESP_SUCCESS) != IDIGI_MSG_RESP_SUCCESS)
                {
                    APP_DEBUG("app_put_request_handler: (have data) server response for %s file with an error 0x%X\n",
                                user->file_path, message->flags);
                }

                /* should be done now */
                free(user);
                put_file_active_count--;
                reboot_state = no_reboot_received;
                delay_receive_state = no_delay_receive;
            }
            break;

        case idigi_data_service_type_error:
            {

                ASSERT(user != NULL);
                APP_DEBUG("app_put_request_handler: type_error for putting %s file\n", user->file_path);
                free(user);
                put_file_active_count--;
            }
            break;

        default:
            APP_DEBUG("app_put_request_handler: Unexpected message type: %d\n", put_request->message_type);
            break;
        }
    }
done:
    return status;
}

/* we only supported 1 target */
static char const request_reboot_ready[] = "request_reboot_ready";

static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t * const server_device_request = request_data->service_context;
    idigi_data_service_block_t * server_data = request_data->server_data;

    ASSERT(server_data != NULL);
    ASSERT(service_device_request != NULL);


    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        if (strcmp(server_device_request->target, request_reboot_ready) == 0)
        {
            /* cause to delay calling receive */
            if (delay_receive_state == no_delay_receive)
                delay_receive_state = start_delay_receive;
            else
                APP_DEBUG("process_device_request: %s already started\n", request_reboot_ready);
        }
        else
        {
            /* testing to return unrecognized status */
            APP_DEBUG("process_device_request: unrecognized target = \"%s\"\n", server_device_request->target);
            response_data->message_status = idigi_msg_error_cancel;
        }
    }
    /* don't care any data in the request */
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_service_msg_request_t const * const request_data,
                                                       idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_block_t * const client_data = response_data->client_data;
    /* user just lets us know that reboot request is about to start.
     * just respond so he knows we are connected and ready.
     */
    UNUSED_ARGUMENT(request_data);

    ASSERT(client_data != NULL);
    client_data->length_in_bytes = 0; /* no data */
    client_data->flags = IDIGI_MSG_LAST_DATA;

    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
                                                    idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_block_t * error_data = request_data->server_data;
    idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);

    UNUSED_ARGUMENT(response_data);

    APP_DEBUG("process_device_error: error %d from server\n", error_code);

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
        status = process_device_response(request_data, response_data);
        break;
    case idigi_data_service_type_error:
        status = process_device_error(request_data, response_data);
        break;
    default:
        APP_DEBUG("app_device_request_handler: unknown message type %d for idigi_data_service_device_request\n", service_device_request->message_type);
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
        APP_DEBUG("app_data_service_handler: Request not supported: %d\n", request);
        break;
    }
    return status;
}

