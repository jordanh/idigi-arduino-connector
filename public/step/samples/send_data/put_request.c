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

idigi_status_t app_send_put_request(idigi_handle_t handle)
{
    idigi_status_t status = idigi_success;
    static idigi_data_service_put_request_t header;
    static char file_path[] = "test/test.txt";
    static char file_type[] = "text/plain";

    header.flags = IDIGI_DATA_PUT_APPEND;
    header.path  = file_path;
    header.content_type = file_type;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, &header, NULL);
    APP_DEBUG("Status: %d, file: %s\n", status, file_path);

    return status;
}

idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_msg_request_t const * const put_request = request_data;
    idigi_data_service_msg_response_t * const put_response = response_data;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    if ((put_request == NULL) || (put_response == NULL))
    {
         APP_DEBUG("Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
         goto done;
    }

    if (request == idigi_data_service_put_request)
    {
        switch (put_request->message_type)
        {
        case idigi_data_service_type_need_data:
            {
                idigi_data_service_block_t * const message = put_response->client_data;
                char const buffer[] = "iDigi data service sample\n";
                size_t const bytes = strlen(buffer);

                if (message->length_in_bytes > bytes)
                    message->length_in_bytes = bytes;

                memcpy(message->data, buffer, message->length_in_bytes);
                message->flags = IDIGI_MSG_LAST_DATA | IDIGI_MSG_FIRST_DATA;
                put_response->message_status = idigi_msg_error_none;
            }
            break;

        case idigi_data_service_type_have_data:
            {
                idigi_data_service_block_t * const message = put_request->server_data;

                APP_DEBUG("Received %s response from server\n", ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
                if (message->length_in_bytes > 0) 
                {
                    char * const data = message->data;

                    data[message->length_in_bytes] = '\0';
                    APP_DEBUG("Server response %s\n", data);
                }
            }
            break;

        case idigi_data_service_type_error:
            {
                idigi_data_service_block_t * const message = put_request->server_data;
                idigi_msg_error_t const * const error_value = message->data;

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

