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


#define DEVICE_REQUEST_TARGET "iik_target"
#define DEVICE_REQUEST_NOT_HANDLE_TARGET "iik_not_handle_target"

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
    void * session;
    char * response_data;
    size_t length_in_bytes;
    idigi_data_service_device_response_status_t status;
} device_request_handle_t;

static unsigned int device_request_count = 0;

static idigi_callback_status_t process_device_request(idigi_data_service_device_request_t const * const service_request,
                                                      idigi_data_service_device_response_t * const service_response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * device_request = service_request->user_context;
    idigi_data_service_device_data_t * request_data = service_request->message_data.request_data;

    ASSERT(service_response != NULL);
    ASSERT(request_data != NULL);

    if ((request_data->flag & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        void * ptr;

        bool const is_ok = os_malloc(sizeof *device_request, &ptr);
        if (!is_ok || ptr == NULL)
        {
            /* no memeory stop IIK */
            APP_DEBUG("process_device_request: malloc fails for device request on session %p\n", service_request->session);
            service_response->status = idigi_data_service_device_not_handled;
            goto done;
        }

        device_request = ptr;
        device_request->length_in_bytes = 0;
        device_request->response_data = NULL;
        device_request->session = service_request->session;
        device_request_count++;
        service_response->user_context = device_request;

        ASSERT(service_request->target != NULL);
        if (strcmp(service_request->target, DEVICE_REQUEST_TARGET) == 0)
        {
            device_request->status = idigi_data_service_device_success;
        }
        else if (strcmp(service_request->target, DEVICE_REQUEST_NOT_HANDLE_TARGET) == 0)
        {
            device_request->status = idigi_data_service_device_not_handled;
        }
        else
        {
            /* unrecognized target so let's cancel it */
            service_response->status = idigi_data_service_device_not_handled;
            goto done;
        }

    }
    else
    {
        ASSERT(device_request != NULL);
    }


    device_request->length_in_bytes += request_data->length_in_bytes;

    if ((request_data->flag & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
    {
        APP_DEBUG("process_device_request: session %p target = \"%s\" data length = %lu total length = %lu\n",
                                 service_request->session,
                                 service_request->target,
                                 (unsigned long int)request_data->length_in_bytes,
                                 (unsigned long int)device_request->length_in_bytes);
    }
    else
    {
        APP_DEBUG("process_device_request: session %p data length = %lu total length = %lu\n",
                                 service_request->session,
                                 (unsigned long int)request_data->length_in_bytes,
                                 (unsigned long int)device_request->length_in_bytes);
    }

    if ((request_data->flag & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
    {   /* No more chunk. let's setup response data */
        if (device_request->status == idigi_data_service_device_success)
        {
            device_request->response_data = device_response_data;
        }
        else if (device_request->status == idigi_data_service_device_not_handled)
        {
            device_request->response_data = device_response_error_data;
        }
        device_request->length_in_bytes = strlen(device_request->response_data);
    }

    service_response->status = idigi_data_service_device_success;


done:
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_service_device_request_t const * const service_request,
                                                       idigi_data_service_device_response_t * const service_response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * const device_request = service_request->user_context;

    ASSERT(device_response != NULL);
    ASSERT(service_response != NULL);
    ASSERT(service_response->response_data != NULL);
    {
        idigi_data_service_device_data_t * const response_data = service_response->response_data;
        size_t const bytes = (device_request->length_in_bytes < response_data->length_in_bytes) ? device_request->length_in_bytes : response_data->length_in_bytes;

        APP_DEBUG("process_device_response: session %p total length = %lu send_byte %lu\n",
                                    service_request->session,
                                    (unsigned long int)device_request->length_in_bytes,
                                    (unsigned long int)bytes);

        /* let's copy the response data to service_response buffer */
        memcpy(response_data->data, device_request->response_data, bytes);
        response_data->length_in_bytes = bytes;

        device_request->response_data += bytes;
        device_request->length_in_bytes -= bytes;
        response_data->flag = (device_request->length_in_bytes == 0) ? IDIGI_MSG_LAST_DATA : 0;
    }

    service_response->status = device_request->status;

    if (device_request->length_in_bytes == 0)
    {   /* done */
        device_request_count--;
        os_free(device_request);
    }
    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_service_device_request_t const * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * const device_request = service_request->user_context;
    idigi_msg_error_t const error_code = service_request->message_data.error_code;


    APP_DEBUG("process_device_error: session %p error %d from server\n",
                service_request->session, error_code);

    device_request_count--;
    os_free(device_request);

    return status;
}


idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t const * const service_request = request_data;
    idigi_data_service_device_response_t * const service_response = response_data;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    if (request == idigi_data_service_device_request)
    {
        switch (service_request->message_type)
        {
        case idigi_data_service_type_have_data:
            status = process_device_request(service_request, service_response);
            break;
        case idigi_data_service_type_need_data:
            status = process_device_response(service_request, service_response);
            break;
        case idigi_data_service_type_error:
            status = process_device_error(service_request);
            break;
        }
    }
    else
    {
        APP_DEBUG("Unsupported %d  (Only support idigi_data_service_device_request)\n", request);
    }
    return status;
}

