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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "time.h"
#include "idigi_data.h"

static void initialize_request(idigi_data_request_t * request)
{
    static uint8_t test_data[] = "Welcome to iDigi Data Service sample test!";
    static uint8_t path[]      = "test/sample.txt";
    static uint8_t type[]      = "text/plain";

    request->session                = NULL;
    request->flag                   = IDIGI_DATA_REQUEST_START | IDIGI_DATA_REQUEST_LAST | IDIGI_DATA_REQUEST_COMPRESSED;
    request->path.size              = sizeof path - 1;
    request->path.value             = path;
    request->content_type.size      = sizeof type - 1;
    request->content_type.value     = type;
    request->payload.size           = sizeof test_data - 1;
    request->payload.data           = test_data;
}

idigi_status_t initiate_data_service(idigi_handle_t handle) 
{
    idigi_status_t status = idigi_success;
    static time_t last_time = 0;
    time_t current_time;
    static idigi_data_request_t request;
    
    time(&current_time);
    if (last_time == 0) 
    {
        initialize_request(&request);
        last_time = current_time;
        goto done;
    }
    else
        request.session = NULL;

#define DATA_LOG_INTERVAL_IN_SECONDS   300
    if ((current_time - last_time) >= DATA_LOG_INTERVAL_IN_SECONDS) 
    {
        last_time = current_time;
        status = idigi_initiate_action(handle, idigi_initiate_data_service, &request, &request.session);

        DEBUG_PRINTF("Status: %d, Handle: %p\n", status, request.session);
    }

done:
    return status;
}

#define add_node(head, node) \
    do { \
        ASSERT(node != NULL); \
        ASSERT(head != NULL); \
        if (*head != NULL) \
        {\
            *head->prev = node;\
        }\
        node->next = *head;\
        node->prev = NULL;\
        *head = node;\
   } while (0)

#define free_node(head, node) \
    do { \
        ASSERT(node != NULL); \
        ASSERT(head != NULL); \
        if (node->next != NULL) \
        {\
            node->next->prev = node->prev;\
        }\
        if (node->prev != NULL) \
        {\
            node->prev->next = node->next;\
        }\
        if (node == *head)\
        {\
            *head = node->next;\
        }\
        free(node);\
    } while (0)

#define DEVICE_REQUEST_TARGET "iik_target"

static char *device_response_data[] = {"<description>\nThe iDigi Integration kit (IIK) is a software development package used "
"to communicate and exchange information between a device and the iDigi Device Cloud. iDigi supports application to "
"device data interaction (messaging), application and device data storage, and remote management of devices. Devices "
"are associated with the server through the Internet or other wide area network connection, which allows for "
"communication between the device, server, and customer applications. An important part of this communication "
"is the transfer of data from a device to the server.\n",

"iDigi is based upon a cloud computing model that provides on-demand scalability "
"so you can rest assured that when you need additional computing and storage iDigi will scale to meet "
"your needs. The iDigi Device Cloud is designed using a high-availability architecture, with redundancy and failover "
"characteristics in mind.\n"
"Using iDigi, customers can now easily develop cloud connected devices and applications that quickly scale from dozens "
"to hundreds, thousands or even millions of endpoints.\n</description>"

};

static size_t device_response_count = sizeof device_response_data/ sizeof *device_response_data; /* number of chunk data */

static char *device_response_error_data = "<error> Unknown target</error>";

typedef struct device_request_handle {
    size_t total_length;
    idigi_data_service_device_response_t response_info;
    size_t response_data_index;
    bool send_response;
    struct device_request_handle * prev;
    struct device_request_handle * next;
} device_request_handle_t;

/* TODO: need semaphore lock for this device_request_list
 * for multi-threaded system.
 */
device_request_handle_t * device_request_list = NULL;

static void initiate_data_service_response(device_request_handle_t * const device_request, char * response_data, size_t response_length)
{
    idigi_status_t status;

    /* setup a new data for the response */
    device_request->response_info.data = (uint8_t *)response_data;
    device_request->response_info.data_length = response_length;
    device_request->response_info.flag = (device_request->response_data_index == device_response_count)? IDIGI_DATA_REQUEST_LAST : 0;

    DEBUG_PRINTF("initiate_data_service_response: idigi_initiate_data_service_response calling idigi_initiate_action for session %p to send %lu bytes\n",
            device_request->response_info.session, (unsigned long int)device_request->response_info.data_length);
    status = idigi_initiate_action(device_data.idigi_handle, idigi_initiate_data_service_response, &device_request->response_info, NULL);
    if (status != idigi_success && status != idigi_service_busy)
    {
        DEBUG_PRINTF("initiate_data_service_response: idigi_initiate_action for idigi_data_service_device_response for session %p fails with error %d\n",
                device_request->response_info.session, status);
        free_node(&device_request_list, device_request);
    }
    return;
}

static idigi_callback_status_t process_data_service_request(idigi_data_service_device_request_t * const request_data, void ** response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * handle;

    if ((request_data->flag & IDIGI_DATA_REQUEST_START) == IDIGI_DATA_REQUEST_START)
    {
        void * ptr;

        ptr = malloc(sizeof *handle);
        if (ptr == NULL)
        {
            /* no memeory stop IIK */
            DEBUG_PRINTF("process_data_service_request: malloc fails for device request on session %p\n", request_data->session);
            status = idigi_callback_abort;
            goto done;
        }

        handle = ptr;
        handle->total_length = 0;
        handle->response_info.session = request_data->session;
        handle->response_info.data_length = 0;
        handle->response_info.flag = 0;
        /* set to 0 so process_request_response will not start sending response */
        handle->response_data_index = 0;
        handle->send_response = false;
        add_node(&device_request_list, handle);
    }
    else
    {
        ASSERT(request_data->user_context != NULL);
        handle = request_data->user_context;
        ASSERT(handle->response_info.session == request_data->session);
    }


    handle->total_length += request_data->data_length;

    *response_data = handle;
    DEBUG_PRINTF("process_data_service_request: session %p local handle %p target = \"%s\" flag 0x%x data length = %lu total length = %lu\n",
                                request_data->session, handle,
                                request_data->target,
                                request_data->flag,
                                (unsigned long int)request_data->data_length,
                                (unsigned long int)handle->total_length);

    if ((request_data->flag & IDIGI_DATA_REQUEST_LAST) == IDIGI_DATA_REQUEST_LAST)
    {   /* No more chunk so let's send a response */
        char * data;
        size_t length;

        if (strcmp(request_data->target, DEVICE_REQUEST_TARGET) == 0)
        {
            /* we ignore the context request_data->data of the target; just send the response */
            handle->response_info.status = idigi_data_service_success;
            handle->response_data_index++;
            data = device_response_data[0]; /* initial chunk */
            length = strlen(device_response_data[0]);
        }
        else
        {
            handle->response_info.status = idigi_data_service_not_handled;
            handle->response_data_index = device_response_count; /* no more chnunk; send error response */
            data = device_response_error_data;
            length = strlen(device_response_error_data);

        }
        initiate_data_service_response(handle, data, length);
    }

done:
    return status;
}
static idigi_callback_status_t process_data_service_complete(idigi_data_send_t * const complete_info)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * request_handle = NULL;
    bool delete_session = false;

    for (request_handle = device_request_list; request_handle != NULL; request_handle = request_handle->next)
    {
        if (request_handle->response_info.session == complete_info->session)
        {
            request_handle->response_info.data_length -= complete_info->bytes_sent;

            if (complete_info->status != idigi_success)
            {   /* something's wrong */
                 delete_session = true;
            }
            else
            {
                ASSERT(request_handle->response_info.data_length == 0);

                if (request_handle->response_data_index < device_response_count)
                {  /* send more chunk */
                    int const i = request_handle->response_data_index++;
                    initiate_data_service_response(request_handle, device_response_data[i], strlen(device_response_data[i]));
                }
                else
                {   /* done with the session */
                    delete_session = true;
                }
            }
            break;
        }
    }

    if (delete_session)
    {
        free_node(&device_request_list, request_handle);
    }

    return status;
}

static idigi_callback_status_t process_data_service_error(idigi_data_error_t * const error_info)
{
    idigi_callback_status_t status = idigi_callback_continue;
    device_request_handle_t * request_handle;

    for (request_handle = device_request_list; request_handle != NULL; request_handle = request_handle->next)
    {
        if (request_handle->response_info.session == error_info->session)
        {
            DEBUG_PRINTF("process_data_service_error: Got idigi_data_service_error on session %p with error %d\n",
                                                                request_handle->response_info.session, error_info->error);
            free_node(&device_request_list, request_handle);
            break;
        }
    }

    return status;
}


idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_data);
    UNUSED_PARAMETER(response_length);

    switch (request)
    {
    case idigi_data_service_send_complete:
    {
        idigi_data_send_t const * send_info = request_data;

        UNUSED_PARAMETER(send_info);
        DEBUG_PRINTF("Handle: %p, status: %d, sent: %d bytes\n", send_info->session, send_info->status, send_info->bytes_sent);
        status = process_data_service_complete(request_data);
        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;

        UNUSED_PARAMETER(error_block);
        DEBUG_PRINTF("Handle: %p, error: %d\n", error_block->session, error_block->error);
        status = process_data_service_error(request_data);
        break;
    }

    case idigi_data_service_response:
    {
        idigi_data_response_t const * response = request_data;
        char * data = (char *)response->message.value;

        UNUSED_PARAMETER(response);
        data[response->message.size] = '\0';
        DEBUG_PRINTF("Handle: %p, status: %d, message: %s\n", response->session, response->status, data);
        break;
    }
    case idigi_data_service_request:
        status = process_data_service_request(request_data, response_data);
        break;

    default:
        break;

    }

    return status;
}
