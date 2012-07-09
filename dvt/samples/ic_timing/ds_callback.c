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
#include <stdbool.h>
#include "idigi_api.h"
#include "platform.h"
#include "application.h"

#define DS_FILE_NAME_LEN  20
#define DS_DATA_SIZE  (1024 * 8)

#define PUT_FILE_MAX    1

static char ds_buffer[DS_DATA_SIZE];

typedef struct
{
    idigi_data_service_put_request_t header;
    char file_path[DS_FILE_NAME_LEN];
    size_t bytes_sent;
    size_t file_length_in_bytes;
    char * file_data;
    int index;
} ds_record_t;


static unsigned int put_file_active_count = 0;
static unsigned int put_file_count = 0;
static bool first_time = true;
bool start_put_file = true;

static idigi_status_t send_file(idigi_handle_t handle, int index, char * const filename, char * const content, size_t content_length)
{

    idigi_status_t status = idigi_success;
    static char file_type[] = "text/plain";

    ds_record_t * user = malloc(sizeof *user);

    if (user == NULL)
    {
        /* no memeory stop IIK */
        APP_DEBUG("send_put_request: malloc fails\n");
        status = idigi_no_resource;
        goto done;
    }

    sprintf(user->file_path, "%s", filename);
    user->header.flags = 0;
    user->header.path  = user->file_path;
    user->header.content_type = file_type;
    user->header.context = user;
    user->bytes_sent = 0;
    user->file_data = content;
    user->file_length_in_bytes = content_length;
    user->index = index;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, &user->header, NULL);
    if (status != idigi_success)
    {
        if (status != idigi_init_error)
            APP_DEBUG("send_file: idigi_initiate_action failed %d\n", status);

        free(user);
    }
    else
    {
        if (put_file_count == 0)
        {
            startWritingCount = 0;

        }

        {
            #define SIZE_TEXT "Size = "

            static char ds_complete_text[sizeof user->file_path + sizeof (SIZE_TEXT MACRO_TO_STRING(SIZE_MAX)) + 1];

            sprintf(ds_complete_text, "%s %s%zu", user->file_path, SIZE_TEXT, user->file_length_in_bytes);

            open_timing_table("Initiate put file: ", ds_complete_text);
        }
    }

done:
    return status;
}


void send_put_request(idigi_handle_t handle)
{


    if (start_put_file && put_file_active_count < PUT_FILE_MAX)
    {

        if (first_time)
        {
            int i;

            for (i = 0; i < DS_DATA_SIZE; i++)
                ds_buffer[i] = 0x41 + (rand() % 0x3B);
            first_time = false;
        }

        {
            char filename[DS_FILE_NAME_LEN];

            sprintf(filename, "test/dvt%d.txt", put_file_active_count);
            if (send_file(handle, put_file_active_count, filename, ds_buffer, DS_DATA_SIZE) == idigi_success)
            {
                put_file_active_count++;
                put_file_count++;
                if (put_file_active_count == PUT_FILE_MAX) start_put_file = false;
            }
        }
    }

    return;
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

                memcpy(dptr, &ds_buffer[user->bytes_sent], bytes_copy);
                message->length_in_bytes = bytes_copy;
                message->flags = 0;
                if (user->bytes_sent == 0)
                {
                    message->flags |= IDIGI_MSG_FIRST_DATA;
                    APP_DEBUG("app_put_request_handler: (need data) %s %p\n", user->file_path, (void *)user);
                    writing_timing_description("start put data: ", user->file_path);
                }

                user->bytes_sent += bytes_copy;
                if (user->bytes_sent == user->file_length_in_bytes)
                {
                    message->flags |= IDIGI_MSG_LAST_DATA;
                    writing_timing_description("End put data: ", user->file_path);
                }
            }
            break;

        case idigi_data_service_type_have_data:
            {
                idigi_data_service_block_t * message = put_request->server_data;
                char * data = message->data;

                if (message->length_in_bytes > 0)
                {
                    data[message->length_in_bytes] = '\0';
                    APP_DEBUG("app_put_request_handler: server response (%zu) %s\n", message->length_in_bytes, data);
                }

                close_timing_table("Got response for put data: ", user->file_path);

                APP_DEBUG("app_put_request_handler (have_data): status = 0x%x %s done this session %p\n",
                   message->flags, user->file_path, (void *)user);
              /* should be done now */
                ASSERT(user != NULL);
                free(user);
                put_file_active_count--;
            }
            break;

        case idigi_data_service_type_error:
            {

                APP_DEBUG("app_put_request_handler (type_error): %s cancel this session %p\n", user->file_path, (void *)user);
                ASSERT(user != NULL);
                free(user);
                put_file_active_count--;
            }
            break;

        default:
            APP_DEBUG("app_put_request_handler: Unexpected message type: %d\n", put_request->message_type);
            break;
        }
        goto done;
    }

done:
    return status;
}

/* supported targets */
static char const device_request_target[] = "ds_timing_test";
static char const put_device_request_target[] = "put_ds_timing_test";
static char const request_terminate_target[] = "request_terminate";


#define DEVICE_REPONSE_COUNT    1

typedef struct device_request_handle {
    void * session;
    char * response_data;
    size_t length_in_bytes;
    char * target;
    unsigned int count;
} device_request_handle_t;

static unsigned int device_request_active_count = 0;

static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t * const server_device_request = request_data->service_context;
    device_request_handle_t * client_device_request = response_data->user_context;
    idigi_data_service_block_t * server_data = request_data->server_data;

    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        open_timing_table("Start device request", server_device_request->target);

        APP_DEBUG("process_device_request: Start device request: %s\n", server_device_request->target);

        if (strcmp(server_device_request->target, request_terminate_target) == 0)
        {
            idigi_status_t action_status;

            APP_DEBUG("process_device_request: terminate IC (active session = %d)\n", put_file_active_count);
            action_status = idigi_initiate_action(idigi_handle, idigi_initiate_terminate, NULL, NULL);
            if (action_status != idigi_success)
            {
                APP_DEBUG("process_device_request: idigi_initiate_terminate error %d\n", action_status);
            }
            response_data->message_status = idigi_msg_error_cancel;
            goto done;
        }
#if 0
        if (strcmp(server_device_request->target, put_device_request_target) == 0)
        {
            /* trigger put file */
            start_put_file = true;
        }
#endif
        client_device_request = malloc(sizeof *client_device_request);
        if (client_device_request == NULL)
        {
            /* no memeory stop IIK */
            APP_DEBUG("process_device_request: malloc fails for device request on session %p\n", server_device_request->device_handle);
            response_data->message_status = idigi_msg_error_memory;
            goto done;
        }

        client_device_request->target = (char *)device_request_target;
        client_device_request->length_in_bytes = 0;
        client_device_request->response_data = NULL;
        client_device_request->count = 0;
        device_request_active_count++;
        response_data->user_context = client_device_request;

        ASSERT(server_device_request->target != NULL);
        

    }
    else
    {
        ASSERT(client_device_request != NULL);
    }


    client_device_request->length_in_bytes += server_data->length_in_bytes;
    APP_DEBUG("process_device_request: handle %p target = \"%s\" data length = %lu total length = %lu\n",
                                 server_device_request->device_handle,
                                 client_device_request->target,
                                 (unsigned long int)server_data->length_in_bytes,
                                 (unsigned long int)client_device_request->length_in_bytes);

    if ((server_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
    {   /* No more chunk. let's setup response data */
        /* don't care about what target in here */
        if (first_time)
        {
            int i;

            for (i = 0; i < DS_DATA_SIZE; i++)
                ds_buffer[i] = 0x41 + (rand() % 0x3B);
            first_time = false;
        }
        client_device_request->response_data = ds_buffer;
        client_device_request->length_in_bytes = DS_DATA_SIZE;
        client_device_request->count = DEVICE_REPONSE_COUNT;
    }

    response_data->message_status = idigi_msg_error_none;

done:
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_service_msg_request_t const * const request_data,
                                                       idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t * const server_device_request = request_data->service_context;
    device_request_handle_t * const client_device_request = response_data->user_context;

    ASSERT(response_data->client_data != NULL);
    ASSERT(client_device_request != NULL); /* we use user_context for our client_device_request */
 
   {
        idigi_data_service_block_t * const client_data = response_data->client_data;
        size_t const bytes = (client_device_request->length_in_bytes < client_data->length_in_bytes) ? client_device_request->length_in_bytes : client_data->length_in_bytes;

        APP_DEBUG("process_device_response: handle %p total length = %lu send_byte %lu\n",
                                    server_device_request->device_handle,
                                    (unsigned long int)client_device_request->length_in_bytes,
                                    (unsigned long int)bytes);

        /* let's copy the response data to service_response buffer */
        memcpy(client_data->data, client_device_request->response_data, bytes);
        client_device_request->response_data += bytes;
        client_device_request->length_in_bytes -= bytes;

        client_data->length_in_bytes = bytes;
        client_data->flags = (client_device_request->length_in_bytes == 0 && client_device_request->count == 1) ? IDIGI_MSG_LAST_DATA : 0;
   }

    response_data->message_status = idigi_msg_error_none;

    if (client_device_request->length_in_bytes == 0)
    {
        client_device_request->count--;
        if (client_device_request->count > 0)
        {
            /* setup more data to be sent */
            client_device_request->response_data = ds_buffer;
            client_device_request->length_in_bytes = (rand() % (DS_DATA_SIZE +1));
        }
        else
        {
            close_timing_table("End device request ", client_device_request->target);
            APP_DEBUG("process_device_response: End device request\n");

            device_request_active_count--;
            free(client_device_request);
        }
    }

    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
                                                    idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * const client_device_request = response_data->user_context;
    idigi_data_service_device_request_t * const server_device_request = request_data->service_context;
    idigi_data_service_block_t * error_data = request_data->server_data;
    idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);


    APP_DEBUG("process_device_error: handle %p error %d from server\n",
                server_device_request->device_handle, error_code);

    device_request_active_count--;
    free(client_device_request);

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

