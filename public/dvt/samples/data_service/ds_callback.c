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
#include "idigi_api.h"
#include "platform.h"

#define DS_MAX_USER   8
#define DS_FILE_NAME_LEN  20
#define DS_DATA_SIZE  (1024 * 16)

static char ds_buffer[DS_DATA_SIZE];

typedef struct
{
    idigi_data_put_header_t header;
    void const * handle;
    char file_path[DS_FILE_NAME_LEN];
    size_t bytes_sent;
} ds_record_t;

static ds_record_t ds_user_data[DS_MAX_USER];

idigi_status_t send_put_request(idigi_handle_t handle, int index) 
{
    idigi_status_t status = idigi_success;
    static char file_type[] = "text/plain";
    static bool first_time = true;
    ds_record_t * user = &ds_user_data[index];
    idigi_data_put_header_t * const header = &user->header;
    void const * session_handle = NULL;

    if (index >= DS_MAX_USER) 
    {
        status = idigi_invalid_data_range;
        goto done;
    }

    if (first_time)
    {
        int i;
    
        for (i = 0; i < DS_DATA_SIZE; i++) 
            ds_buffer[i] = 0x20 + (i % 0x60);
        first_time = false;
    }

    sprintf(user->file_path, "test/dvt%d.txt", index);
    header->flags = 0;
    header->path  = user->file_path;
    header->content_type = file_type;
    header->context = user;
    user->bytes_sent = 0;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, header, &session_handle);
    APP_DEBUG("Status: %d, Session: %p\n", status, session_handle);
    user->handle = session_handle;

done:
    return status;
}

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_put_request_t const * const put_request = request_data;
    idigi_data_put_response_t * const put_response = response_data;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    if ((put_request == NULL) || (put_response == NULL))
    {
         APP_DEBUG("Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
         goto done;
    }

    if (request == idigi_data_service_put_request)
    {
        idigi_data_put_header_t const * const header = put_request->header_context;
        ds_record_t * const user = (ds_record_t * const)header->context;

        switch (put_request->request_type)
        {
        case idigi_data_service_type_need_data:
            {
                char * dptr = put_response->data;
                size_t const bytes_available = put_response->length_in_bytes;
                size_t const bytes_to_send = DS_DATA_SIZE - user->bytes_sent;
                size_t bytes_copy = (bytes_to_send > bytes_available) ? bytes_available : bytes_to_send;

                memcpy(dptr, &ds_buffer[user->bytes_sent], bytes_copy);
                printf("Copying %d bytes\n", bytes_copy);
                put_response->length_in_bytes = bytes_copy;
                put_response->flags = 0;
                if (user->bytes_sent == 0)
                    put_response->flags |= IDIGI_MSG_FIRST_DATA;

                user->bytes_sent += bytes_copy;
                if (user->bytes_sent == DS_DATA_SIZE)
                    put_response->flags |= IDIGI_MSG_LAST_DATA;
            }
            break;

        case idigi_data_service_type_have_data:
            {
                uint8_t const * data = put_response->data;
                uint8_t status = *data;

                APP_DEBUG("Received %s response for %p\n", (status == 0) ? "success" : "error", put_request->session_handle);
                if (put_response->length_in_bytes > 1) 
                {
                    APP_DEBUG("Server response %s\n", (char *)&data[1]);
                }
            }
            break;

        case idigi_data_service_type_error:
            {
                idigi_msg_error_t const * const error_value = put_response->data;

                APP_DEBUG("Data service error for %p: %d\n", put_request->session_handle, *error_value);
            }
            break;

        default:            
            APP_DEBUG("Unexpected command: %d\n", request);
            break;
        }
    }
    else
    {
        APP_DEBUG("Request not supported in this sample: %d\n", request);
    }

done:
    return status;
}

