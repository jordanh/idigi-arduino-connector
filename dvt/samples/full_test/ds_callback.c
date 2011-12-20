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
#include "idigi_dvt.h"

extern int os_malloc(size_t const size, void ** ptr);
extern void os_free(void * const ptr);

idigi_status_t send_put_request(idigi_handle_t handle)
{
    idigi_status_t status = idigi_success;
    static char file_path[sizeof "test/dvt/" + DVT_FILE_NAME_SIZE];
    static char file_type[] = "text/plain";
    idigi_data_service_put_request_t * header = &dvt_current_ptr->ds_info->header;

    if (dvt_current_ptr->state != dvt_state_fw_download_complete)
        goto done;

    sprintf(file_path, "test/dvt/%s", dvt_current_ptr->file_name);

    switch (dvt_current_ptr->target)
    {
    case dvt_case_put_request_no_flag:
        header->flags = 0;
        break;

    case dvt_case_put_request_append:
        header->flags = IDIGI_DATA_PUT_APPEND;
        break;

    case dvt_case_put_request_archive:
        header->flags = IDIGI_DATA_PUT_ARCHIVE;
        break;

    case dvt_case_put_request_both:
        header->flags = IDIGI_DATA_PUT_APPEND | IDIGI_DATA_PUT_ARCHIVE;
        break;

    default:
        header->flags = 0;
        break;
    }

    header->path  = file_path;
    header->content_type = file_type;
    header->context = dvt_current_ptr->ds_info;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, header, NULL);
    APP_DEBUG("send_put_request: %s status  %d total file length = %d\n", file_path, status, dvt_current_ptr->file_size);
    if (status != idigi_success)
        cleanup_dvt_data();
    else
        dvt_current_ptr->state = dvt_state_request_progress;

done:
    return status;
}

idigi_callback_status_t idigi_put_request_callback(void const * request_data, size_t const request_length,
                                                   void * response_data, size_t * const response_length)
{
    idigi_data_service_msg_request_t const * const put_request = request_data;
    idigi_data_service_msg_response_t * const put_response = response_data;
    dvt_ds_t * const ds_info = put_request->service_context;
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    if ((put_request == NULL) || (put_response == NULL))
    {
         APP_DEBUG("idigi_put_request_callback: Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
         goto done;
    }

    switch (put_request->message_type) 
    {
    case idigi_data_service_type_need_data:
        switch (dvt_current_ptr->target)
        {
        case dvt_case_put_request_busy:
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

        case dvt_case_put_request_cancel_at_start:
            goto error;

        case dvt_case_put_request_cancel_at_middle:
            if (ds_info->bytes_sent > 0)
                goto error;
            break;

        case dvt_case_put_request_timeout:
            #if !defined (IDIGI_COMPRESSION)
            if (ds_info->bytes_sent > 0)
            {
                status = idigi_callback_busy;
                goto done;
            }
            #endif
            break;

        default:
            break;
        }

        {
            idigi_data_service_block_t * message = put_response->client_data;
            char * dptr = message->data;
            size_t const bytes_available = message->length_in_bytes;
            size_t const bytes_to_send = dvt_current_ptr->file_size - ds_info->bytes_sent;
            size_t bytes_copy = (bytes_to_send > bytes_available) ? bytes_available : bytes_to_send;

            memcpy(dptr, &dvt_current_ptr->file_content[ds_info->bytes_sent], bytes_copy);
            message->length_in_bytes = bytes_copy;
            message->flags = 0;
            if (ds_info->bytes_sent == 0)
                message->flags |= IDIGI_MSG_FIRST_DATA;

            ds_info->bytes_sent += bytes_copy;
            if (ds_info->bytes_sent == dvt_current_ptr->file_size)
            {
                message->flags |= IDIGI_MSG_LAST_DATA;
                dvt_current_ptr->state = dvt_state_fw_download_complete;
            }
        }
        break;

    case idigi_data_service_type_have_data:
        {
            idigi_data_service_block_t * message = put_request->server_data;
            uint8_t const * data = message->data;

            if (dvt_current_ptr->state != dvt_state_fw_download_complete)
            {
                APP_DEBUG("idigi_put_request_callback: got response before complete\n");
                goto error;
            }

            APP_DEBUG("idigi_put_request_callback (have_data): Received %s (0x%x) response from server\n",
                                        (((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error"), (unsigned)message->flags);
            if (message->length_in_bytes > 0)
            {
                APP_DEBUG("idigi_put_request_callback: server response %s\n", (char *)data);
            }

            goto cleanup;
        }

    case idigi_data_service_type_error:
        {
            idigi_data_service_block_t * message = put_request->server_data;
            idigi_msg_error_t const * const error_value = message->data;

            if (error_value == NULL)
                goto error;

            APP_DEBUG("idigi_put_request_callback: Data service error %d\n", *error_value);
            goto cleanup;
        }
    
    default:
        APP_DEBUG("idigi_put_request_callback: Unexpected message type: %d\n", put_request->message_type);
        break;
    }
    goto done;

error:
    APP_DEBUG("idigi_put_request_callback error: target[%d] cancel this session\n", dvt_current_ptr->target);
    put_response->message_status = idigi_msg_error_cancel;

cleanup:
    cleanup_dvt_data();

done:
    return status;
}

/* supported targets */
static char const device_request_target[] = "iik_target";
static char const device_request_not_handle_target[] = "iik_not_handle_target";
static char const device_request_cancel_target[] = "iik_cancel_target";
static char const device_request_invalid_response_target[] = "iik_cancel_response_target";
static char const device_request_invalid_response_target1[] = "iik_cancel_response_target1";

#define DEVICE_REPONSE_COUNT    2

typedef struct device_request_handle {
    void * session;
    char * response_data;
    size_t length_in_bytes;
    char * target;
    unsigned count;
} device_request_handle_t;

static unsigned int device_request_active_count = 0;

static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t * const server_device_request = request_data->service_context;
    device_request_handle_t * client_device_request = response_data->user_context;
    idigi_data_service_block_t * server_data = request_data->server_data;

    ASSERT(server_data != NULL);
    ASSERT(service_device_request != NULL);

    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        void * ptr;

        int const ccode = os_malloc(sizeof *client_device_request, &ptr);
        if (ccode != 0 || ptr == NULL)
        {
            /* no memeory stop IIK */
            APP_DEBUG("process_device_request: malloc fails for device request on session %p\n", server_device_request->device_handle);
            response_data->message_status = idigi_msg_error_memory;
            goto done;
        }

        client_device_request = ptr;
        client_device_request->length_in_bytes = 0;
        client_device_request->response_data = NULL;
        client_device_request->count = 0;
        device_request_active_count++;
        response_data->user_context = client_device_request;

        ASSERT(server_device_request->target != NULL);
        if (strcmp(server_device_request->target, device_request_target) == 0)
        {
            /* testing regular process target */
            client_device_request->target = (char *)device_request_target;
        }
        else if (strcmp(server_device_request->target, device_request_not_handle_target) == 0)
        {
            /* testing to return not processed message */
            client_device_request->target = (char *)device_request_not_handle_target;
        }
        else if (strcmp(server_device_request->target, device_request_invalid_response_target) == 0)
        {
            /* testing to return error in response callback */
            client_device_request->target = (char *)device_request_invalid_response_target;
        }
        else if (strcmp(server_device_request->target, device_request_invalid_response_target1) == 0)
        {
            /* testing to return error in response callback */
            client_device_request->target = (char *)device_request_invalid_response_target1;
        }
        else if (strcmp(server_device_request->target, device_request_cancel_target) == 0)
        {
            /* testing to return cancel message status */
            APP_DEBUG("process_device_request: handle %p cancel\n", server_device_request->device_handle);
            response_data->message_status = idigi_msg_error_cancel;
            os_free(ptr);
            device_request_active_count--;
            goto done;

        }
        else
        {
            /* testing to return unrecognized status */
            APP_DEBUG("process_device_request: unrecognized target = %s\n", server_device_request->target);
            os_free(ptr);
            device_request_active_count--;
            status = idigi_callback_unrecognized;
            goto done;
        }

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
        client_device_request->response_data = NULL; /* echo back the data */
        client_device_request->length_in_bytes = 0;
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
    ASSERT(server_device_request->device_handle != client_device_request->device_handle);

    if (client_device_request->target == device_request_invalid_response_target)
    {
        /* cancel before sending any response data */
        APP_DEBUG("process_device_response: handle %p cancel\n", server_device_request->device_handle);
        response_data->message_status = idigi_msg_error_cancel;
        goto error;
    }

    {
        idigi_data_service_block_t * const client_data = response_data->client_data;
        size_t const bytes = (client_device_request->length_in_bytes < client_data->length_in_bytes) ? client_device_request->length_in_bytes : client_data->length_in_bytes;

        if ((client_device_request->target == device_request_invalid_response_target1) &&
            (client_device_request->count == 1))
        {
            /* cancel client response after sending some response data */
            APP_DEBUG("process_device_response: handle %p cancel\n", server_device_request->device_handle);
            response_data->message_status = idigi_msg_error_cancel;
            goto error;
        }

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
        if (client_device_request->target == device_request_not_handle_target)
        {
            client_data->flags |= IDIGI_MSG_DATA_NOT_PROCESSED;
        }
    }

    response_data->message_status = idigi_msg_error_none;

    if (client_device_request->length_in_bytes == 0)
    {
        client_device_request->count--;
        if (client_device_request->count > 0)
        {
            /* setup more data to be sent */
            client_device_request->response_data = NULL;
            client_device_request->length_in_bytes = 0;
        }
        else
        {
            /* done. let free memory */
            goto error;
        }
    }
    goto done;

error:
    /* done */
    device_request_active_count--;
    os_free(client_device_request);

done:
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
    os_free(client_device_request);

    return status;
}

idigi_callback_status_t idigi_device_request_callback(void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_msg_request_t const * const service_device_request = request_data;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

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
        APP_DEBUG("idigi_put_request_callback: unknown message type %d for idigi_data_service_device_request\n", service_device_request->message_type);
        break;
    }

    return status;
}

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    switch (request)
    {
    case idigi_data_service_put_request:
        status = idigi_put_request_callback(request_data, request_length, response_data, response_length);
        break;
    case idigi_data_service_device_request:
        status = idigi_device_request_callback(request_data, request_length, response_data, response_length);
        break;
    default:
        APP_DEBUG("idigi_data_service_callback: Request not supported: %d\n", request);
        break;
    }
    return status;
}

