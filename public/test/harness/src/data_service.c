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
#include <errno.h>
#include "time.h"

#include "idigi_struct.h"

idigi_data_request_t * data_service_request;
bool data_service_ready;

idigi_status_t initiate_data_service(idigi_handle_t handle) 
{
    idigi_status_t status = idigi_success;

    if(data_service_request != NULL){
        if(data_service_ready == true){
            status = idigi_initiate_action(handle, idigi_initiate_data_service, data_service_request, &data_service_request->session);
            DEBUG_PRINTF("Status: %d, Session: %p\n", status, data_service_request->session);

            data_service_request = NULL;   
            data_service_ready = false;
        }
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
        DEBUG_PRINTF("Handle: %p, status: %d, sent: %d bytes\n", send_info->session, send_info->status, send_info->bytes_sent);
        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;

        UNUSED_PARAMETER(error_block);
        DEBUG_PRINTF("Handle: %p, error: %d\n", error_block->session, error_block->error);
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

    default:
        break;

    }

    return status;
}
