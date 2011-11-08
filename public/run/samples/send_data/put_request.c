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

#define MSG_DATA_SIZE  (1024 * 16)
static char ds_buffer[MSG_DATA_SIZE];

idigi_status_t send_put_request(idigi_handle_t handle) 
{
    idigi_status_t status = idigi_success;
    static idigi_data_put_header_t header;
    static char file_path[] = "test/test.txt";
    static char file_type[] = "text/plain";
    int i;

    for (i = 0; i < MSG_DATA_SIZE; i++) 
        ds_buffer[i] = 0x20 + (i % 0x60);

    header.flags = 0; // IDIGI_DATA_PUT_APPEND;
    header.path  = file_path;
    header.content_type = file_type;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, &header, NULL);
    APP_DEBUG("Status: %d, file: %s\n", status, file_path);

    return status;
}

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_put_request_t const * const put_request = request_data;
    idigi_data_put_response_t * const put_response = response_data;
    static size_t bytes_sent = 0;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    if ((put_request == NULL) || (put_response == NULL))
    {
         APP_DEBUG("Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
         goto done;
    }

    if (request == idigi_data_service_put_request)
    {
        switch (put_request->request_type)
        {
        case idigi_data_service_type_need_data:
            {
                char * dptr = put_response->data;
                size_t const bytes_available = put_response->length_in_bytes;
                size_t const bytes_to_send = MSG_DATA_SIZE - bytes_sent;
                size_t bytes_copy = (bytes_to_send > bytes_available) ? bytes_available : bytes_to_send;

                memcpy(dptr, &ds_buffer[bytes_sent], bytes_copy);
                printf("Copying %d bytes\n", bytes_copy);
                put_response->length_in_bytes = bytes_copy;
                put_response->flags = 0;
                if (bytes_sent == 0)
                    put_response->flags |= IDIGI_MSG_FIRST_DATA;

                bytes_sent += bytes_copy;
                if (bytes_sent == MSG_DATA_SIZE)
                    put_response->flags |= IDIGI_MSG_LAST_DATA;
            }
            break;

        case idigi_data_service_type_have_data:
            {
                uint8_t const * data = put_response->data;
                uint8_t status = *data;

                APP_DEBUG("Received %s response from server\n", (status == 0) ? "success" : "error");
                if (put_response->length_in_bytes > 1) 
                {
                    APP_DEBUG("Server response %s\n", (char *)&data[1]);
                }
            }
            break;

        case idigi_data_service_type_error:
            {
                idigi_msg_error_t const * const error_value = put_response->data;

                APP_DEBUG("Data service error: %d\n", *error_value);
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

