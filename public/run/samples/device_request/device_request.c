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

#include "idigi_api.h"
#include "platform.h"

extern bool os_malloc(size_t const size, void ** ptr);
extern void os_free(void * const ptr);

/* supported targets */
static char const device_request_target[] = "iik_target";
static char const device_request_not_handle_target[] = "iik_not_handle_target";

static char device_response_data[] = "The iDigi Integration kit (IIK) is a software development package used "
"to communicate and exchange information between a device and the iDigi Device Cloud. iDigi supports application to "
"device data interaction (messaging), application and device data storage, and remote management of devices. Devices "
"are associated with the server through the Internet or other wide area network connection, which allows for "
"communication between the device, server, and customer applications. An important part of this communication "
"is the transfer of data from a device to the server.\n"
"iDigi is based upon a cloud computing model that provides on-demand scalability "
"so you can rest assured that when you need additional computing and storage iDigi will scale to meet "
"your needs. The iDigi Device Cloud is designed using a high-availability architecture, with redundancy and failover "
"characteristics in mind.\n"
"Using iDigi, customers can now easily develop cloud connected devices and applications that quickly scale from dozens "
"to hundreds, thousands or even millions of endpoints.\n";

static char *device_response_error_data = "Not handle target";

typedef struct device_request_handle {
    void * device_handle;
    char * response_data;
    char * target;
    size_t length_in_bytes;
} device_request_handle_t;

static unsigned int device_request_active_count = 0;

static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t * server_device_request = request_data->service_context;
    idigi_data_service_block_t * server_data = request_data->server_data;

    device_request_handle_t * client_device_request = response_data->user_context;

    ASSERT(server_data != NULL);

    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        /* 1st chunk of device request so let's allocate memory for it
         * and setup user_context for the client_device_request.
         */
        void * ptr;

        bool const is_ok = os_malloc(sizeof *client_device_request, &ptr);
        if (!is_ok || ptr == NULL)
        {
            /* no memeory so cancel this request */
            APP_DEBUG("process_device_request: malloc fails for device request on session %p\n", server_device_request->device_handle);
            response_data->message_status = idigi_msg_error_memory;
            goto done;
        }

        client_device_request = ptr;
        client_device_request->length_in_bytes = 0;
        client_device_request->response_data = NULL;
        client_device_request->device_handle = server_device_request->device_handle;
        device_request_active_count++;

        response_data->user_context = client_device_request;  /* setup the user_context */

        ASSERT(server_device_request->target != NULL);
        if (strcmp(server_device_request->target, device_request_target) == 0)
        {
            client_device_request->target = (char *)device_request_target;
            client_device_request->response_data = device_response_data;
        }
        else if (strcmp(server_device_request->target, device_request_not_handle_target) == 0)
        {
            client_device_request->target = (char *)device_request_not_handle_target;
            client_device_request->response_data = device_response_error_data;
        }
        else
        {
            /* unrecognized target so let's cancel it */
            os_free(ptr);
            device_request_active_count--;
            response_data->message_status = idigi_msg_error_cancel;
            goto done;
        }

    }
    else
    {
        /* device request should be our user_context */
        ASSERT(client_device_request != NULL);
    }


    client_device_request->length_in_bytes += server_data->length_in_bytes;

    if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        APP_DEBUG("process_device_request: handle %p target = \"%s\" data length = %lu total length = %lu\n",
                                 server_device_request->device_handle,
                                 server_device_request->target,
                                 (unsigned long int)server_data->length_in_bytes,
                                 (unsigned long int)client_device_request->length_in_bytes);
    }
    else
    {
        APP_DEBUG("process_device_request: handle %p data length = %lu total length = %lu\n",
                                 server_device_request->device_handle,
                                 (unsigned long int)server_data->length_in_bytes,
                                 (unsigned long int)client_device_request->length_in_bytes);
    }

    if ((server_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
    {   /* No more chunk. */
        client_device_request->length_in_bytes = strlen(client_device_request->response_data);
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
        client_data->flags = (client_device_request->length_in_bytes == 0) ? IDIGI_MSG_LAST_DATA : 0;
        if (client_device_request->target == device_request_not_handle_target)
        {
            client_data->flags |= IDIGI_MSG_DATA_NOT_PROCESSED;
        }
    }

    response_data->message_status = idigi_msg_error_none;

    if (client_device_request->length_in_bytes == 0)
    {   /* done */
        device_request_active_count--;
        os_free(client_device_request);
    }
    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
                                                    idigi_data_service_msg_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * const client_device_request = response_data->user_context;
    idigi_data_service_block_t * error_data = request_data->server_data;
    idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);


    APP_DEBUG("process_device_error: handle %p error %d from server\n",
                client_device_request->device_handle, error_code);

    device_request_active_count--;
    os_free(client_device_request);

    return status;
}

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                      void const * request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_msg_request_t const * const service_device_request = request_data;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    if (request == idigi_data_service_device_request)
    {
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
    }
    else
    {
        APP_DEBUG("Unsupported %d  (Only support idigi_data_service_device_request)\n", request);
    }

    return status;
}

