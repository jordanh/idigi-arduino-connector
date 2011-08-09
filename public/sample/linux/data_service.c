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
#include "time.h"
#include <stdlib.h>

#include "idigi_data.h"

#define DATA_LOG_INTERVAL   30
#define DATA_BLOCK_SIZE     4563
#define MAX_USER            4
#define MAX_BLOCKS          8

typedef struct
{
    uint8_t cur_index;
    uint8_t cur_block;
    bool in_process;
    idigi_data_request_t user_data[MAX_USER];
    uint8_t data[DATA_BLOCK_SIZE];
} data_service_test_t;

static data_service_test_t data_service; 

/* TODO: Findout how we can extract the handle in callback function? */
static idigi_handle_t local_handle = NULL;

static idigi_status_t send_data_request(idigi_handle_t handle)
{
    idigi_status_t status = idigi_success;
    static bool first_time = true;

    if (first_time)
    {
        data_service.cur_block = 0;
        data_service.cur_index = 0;
        first_time = false;
    }

    {
        int i;

        for (i = 0; i < DATA_BLOCK_SIZE; i++) 
            data_service.data[i] = (rand()%0x5B)+0x20;        
    }

    if (data_service.cur_index == MAX_USER) 
    {
        data_service.cur_index = 0;
        data_service.cur_block++;
        if (data_service.cur_block == MAX_BLOCKS)
        {
            data_service.cur_block = 0;
            data_service.in_process = false;
            goto done;
        }
    }

    {
        idigi_data_request_t * request = &data_service.user_data[data_service.cur_index];

        if (data_service.cur_block == 0)
        {
            static char path[16];
            static char type[] = "text/plain";
    
            sprintf(path, "test/data%d.txt", data_service.cur_index);
            {
                request->flag = IDIGI_DATA_REQUEST_START | IDIGI_DATA_REQUEST_COMPRESSED;
                request->path.size = strlen(path);
                request->path.value = (uint8_t *)path;
                request->content_type.size = strlen(type);
                request->content_type.value = (uint8_t *)type;
                request->payload.size = DATA_BLOCK_SIZE;
                request->payload.data = data_service.data;
            }
        }
        else
            request->flag = IDIGI_DATA_REQUEST_COMPRESSED;

        if (data_service.cur_block == (MAX_BLOCKS - 1)) 
            request->flag |=  IDIGI_DATA_REQUEST_LAST;

        data_service.cur_index++;
        status = idigi_initiate_action(handle, idigi_initiate_data_service, request, &request->session);
    }

done:
    return status;
}

idigi_status_t initiate_data_service(idigi_handle_t handle)
{
    idigi_status_t status = idigi_success;
    static time_t last_time = 0;
    time_t current_time;
    
    time(&current_time);
    if (last_time == 0) 
    {
        local_handle = handle;
        last_time = current_time;
        data_service.in_process = false;
        goto done;
    }

    if ((current_time - last_time) >= DATA_LOG_INTERVAL) 
    {
        last_time = current_time;
        if (data_service.in_process) 
        {
            DEBUG_PRINTF("Last data service request is not complete yet!!\n");
            goto done;
        }

        data_service.in_process = true;
        status = send_data_request(handle);
    }

done:
    return status;
}


idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t request,
                                                  void const * request_data, size_t request_length,
                                                  void * response_data, size_t * response_length)
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
        DEBUG_PRINTF("Handle: %d, status: %d, sent: %d bytes\n", send_info->session_id, send_info->status, send_info->bytes_sent);
        if (data_service.in_process) 
        {
            idigi_status_t send_status = send_data_request(local_handle);

            if (send_status != idigi_success) 
                status = idigi_callback_abort;
        }

        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;

        UNUSED_PARAMETER(error_block);
        DEBUG_PRINTF("Handle: %d, error: %d\n", error_block->session_id, error_block->error);
        break;
    }

    case idigi_data_service_response:
    {
        idigi_data_response_t const * response = request_data;

        UNUSED_PARAMETER(response);
        response->message.value[response->message.size] = '\0';
        DEBUG_PRINTF("Handle: %d, status: %d, message: %s\n", response->session_id, response->status, response->message.value);
        break;
    }

    default:
        break;

    }

    return status;
}

