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
#include "idigi_api.h"
#include "platform.h"

/* #define PUT_REQUEST_RANDOM_LENGTH */
/* #define PUT_REQUEST_TEST_ERROR */

extern bool os_malloc(size_t const size, void ** ptr);
extern void os_free(void * const ptr);

#define DS_MAX_USER   10
#define DS_FILE_NAME_LEN  20
#define DS_DATA_SIZE  (1024 * 16)

static char ds_buffer[DS_DATA_SIZE];

typedef struct
{
    idigi_data_put_header_t header;
    char file_path[DS_FILE_NAME_LEN];
    size_t bytes_sent;
    size_t file_length_in_bytes;
} ds_record_t;


#if defined(PUT_REQUEST_RANDOM_LENGTH)
unsigned int put_file_active_count = 0;

idigi_status_t send_put_request(idigi_handle_t handle, int index)
{
    idigi_status_t status = idigi_success;
    static char file_type[] = "text/plain";
    static bool first_time = true;
    ds_record_t * user;

    if (index >= DS_MAX_USER)
    {
        status = idigi_invalid_data_range;
        goto done;
    }

    if (first_time)
    {
        int i;

        for (i = 0; i < DS_DATA_SIZE; i++)
            ds_buffer[i] = 0x30 + (rand() % 0x55);
        first_time = false;
    }

    {
        void * ptr;

        bool const is_ok = os_malloc(sizeof *user, &ptr);
        if (!is_ok || ptr == NULL)
        {
            /* no memeory stop IIK */
            APP_DEBUG("send_put_request: malloc fails\n");
            status = idigi_invalid_data_range;
            goto done;
        }
        user = ptr;
    }

    sprintf(user->file_path, "test/dvt%d.txt", index);
    user->header.flags = 0;
    user->header.path  = user->file_path;
    user->header.content_type = file_type;
    user->header.context = user;
    user->bytes_sent = 0;
    user->file_length_in_bytes = (rand() % (DS_DATA_SIZE +1));

    status = idigi_initiate_action(handle, idigi_initiate_data_service, &user->header, NULL);
    APP_DEBUG("send_put_request: %p %s status  %d total file length = %d\n", (void *)user, user->file_path, status, user->file_length_in_bytes);
    if (status == idigi_success)
    {
        put_file_active_count++;
    }
    else
    {
        os_free(user);
    }

done:
    return status;
}


idigi_callback_status_t idigi_put_request_callback(void const * request_data, size_t const request_length,
                                                   void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    {
        idigi_data_put_request_t const * const put_request = request_data;
        idigi_data_put_response_t * const put_response = response_data;

        idigi_data_put_header_t const * const header = put_request->header_context;
        ds_record_t * const user = (ds_record_t * const)header->context;

        if ((put_request == NULL) || (put_response == NULL))
        {
             APP_DEBUG("Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
             goto done;
        }

        switch (put_request->request_type)
        {
        case idigi_data_service_type_need_data:
            {
                char * dptr = put_response->data;
                size_t const bytes_available = put_response->length_in_bytes;
                size_t const bytes_to_send = user->file_length_in_bytes - user->bytes_sent;
                size_t bytes_copy = (bytes_to_send > bytes_available) ? bytes_available : bytes_to_send;

#if defined(PUT_REQUEST_TEST_ERROR)
                if (put_file_active_count > (DS_MAX_USER -2))
                {
                    idigi_msg_error_t * const error_value = put_response->data;

                    printf("need_data: %p cancel this session\n", (void *)user);
                    ASSERT(put_response->length_in_bytes < sizeof error_value);
                    *error_value = idigi_msg_error_cancel;
                    put_response->response_type = idigi_data_service_type_error;
                    put_response->length_in_bytes = sizeof error_value;
                    os_free(user);
                    put_file_active_count--;
                    goto done;
                }
#endif

                memcpy(dptr, &ds_buffer[user->bytes_sent], bytes_copy);
                put_response->length_in_bytes = bytes_copy;
                put_response->flags = 0;
                if (user->bytes_sent == 0)
                    put_response->flags |= IDIGI_MSG_FIRST_DATA;

                user->bytes_sent += bytes_copy;
                if (user->bytes_sent == user->file_length_in_bytes)
                    put_response->flags |= IDIGI_MSG_LAST_DATA;

                printf("need_data: %p copy %d bytes remaining bytes = %d\n", (void *)user, bytes_copy, user->file_length_in_bytes - user->bytes_sent);
            }
            break;

        case idigi_data_service_type_have_data:
            {
                uint8_t const * data = put_response->data;
                uint8_t status = *data;

                ASSERT(user != NULL);
                APP_DEBUG("have_data: %p Received %s response for %p\n", (void *)user, (status == 0) ? "success" : "error", (void *)user);
                if (put_response->length_in_bytes > 1)
                {
                    APP_DEBUG("Server response %s\n", (char *)&data[1]);
                }
                /* should be done now */
                os_free(user);
                put_file_active_count--;
            }
            break;

        case idigi_data_service_type_error:
            {
                idigi_msg_error_t const * const error_value = put_response->data;

                APP_DEBUG("Data service error %p: %d\n", (void *)user, *error_value);

                ASSERT(user != NULL);
                os_free(user);
                put_file_active_count--;
            }
            break;

        default:
            APP_DEBUG("Unexpected command: %d\n", put_request->request_type);
            break;
        }
    }

done:
    return status;
}
#else
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
            ds_buffer[i] = 0x30 + (rand() % 0x55);
        first_time = false;
    }

    sprintf(user->file_path, "test/dvt%d.txt", index);
    header->flags = 0;
    header->path  = user->file_path;
    header->content_type = file_type;
    header->context = user;
    user->bytes_sent = 0;

    status = idigi_initiate_action(handle, idigi_initiate_data_service, header, NULL);
    APP_DEBUG("Status: %d, Session: %p\n", status, session_handle);

done:
    return status;
}

idigi_callback_status_t idigi_put_request_callback(void const * request_data, size_t const request_length,
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

                APP_DEBUG("Received %s response for %p\n", (status == 0) ? "success" : "error", (void *)user);
                if (put_response->length_in_bytes > 1) 
                {
                    APP_DEBUG("Server response %s\n", (char *)&data[1]);
                }
            }
            break;

        case idigi_data_service_type_error:
            {
                idigi_msg_error_t const * const error_value = put_response->data;

                APP_DEBUG("Data service error for %p: %d\n", (void *)user, *error_value);
            }
            break;

        default:            
            APP_DEBUG("Unexpected command: %d\n", put_request->request_type);
            break;
        }
    }

done:
    return status;
}
#endif

#define DEVICE_REQUEST_TARGET "iik_target"
#define DEVICE_REQUEST_NOT_HANDLE_TARGET "iik_not_handle_target"

#if 0
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
#endif

static char *device_response_error_data = "Not handle target";

#define DEVICE_REPONSE_COUNT    1

typedef struct device_request_handle {
    void * session;
    char * response_data;
    size_t length_in_bytes;
    idigi_data_service_device_response_status_t status;
    unsigned count;
} device_request_handle_t;

static unsigned int device_request_count = 0;

static int gWait = 3;

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
        device_request->count = 0;
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
        if (gWait < 3)
        {
            printf("busy\n");
            status = idigi_callback_busy;
            gWait++;
            goto done;
        }
    }


    device_request->length_in_bytes += request_data->length_in_bytes;
    APP_DEBUG("process_device_request: session %p target = \"%s\" data length = %lu total length = %lu\n",
                                 service_request->session,
                                 service_request->target,
                                 (unsigned long int)request_data->length_in_bytes,
                                 (unsigned long int)device_request->length_in_bytes);

    if ((request_data->flag & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
    {   /* No more chunk. let's setup response data */

        if (device_request->status == idigi_data_service_device_success)
        {
            device_request->response_data = ds_buffer;
            device_request->length_in_bytes = (rand() % (DS_DATA_SIZE +1));
            device_request->count = DEVICE_REPONSE_COUNT;
        }
        else if (device_request->status == idigi_data_service_device_not_handled)
        {
            device_request->response_data = device_response_error_data;
            device_request->count = 1;
            device_request->length_in_bytes = strlen(device_request->response_data);
        }
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
        response_data->flag = (device_request->length_in_bytes == 0 && device_request->count == 1) ? IDIGI_MSG_LAST_DATA : 0;
    }

    service_response->status = device_request->status;

    if (device_request->length_in_bytes == 0)
    {   /* done */
        device_request->count--;
        if (device_request->count > 0 && device_request->status == idigi_data_service_device_success)
        {
            device_request->response_data = ds_buffer;
            device_request->length_in_bytes = (rand() % (DS_DATA_SIZE +1));
        }
        else
        {
            device_request_count--;
            os_free(device_request);
        }
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

idigi_callback_status_t idigi_device_request_callback(void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_device_request_t const * const service_request = request_data;
    idigi_data_service_device_response_t * const service_response = response_data;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

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
    default:
        APP_DEBUG("idigi_put_request_callback: unknown message type %d for idigi_data_service_device_request\n", service_request->message_type);
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

