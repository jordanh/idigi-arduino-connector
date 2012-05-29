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

#include <errno.h>
#include "idigi_api.h"
#include "platform.h"
#include "idigi_connector.h"

idigi_data_service_put_request_t put_request_header;
int idigi_put_request_in_progress = 0;
int idigi_put_request_sent = 0;
unsigned int idigi_put_request_response;

static char *put_request_data;
static size_t put_request_size;
static int put_request_first_buffer;

char put_request_buffer[PUT_REQUEST_BUFFER_SIZE];

#ifndef USE_NEW_DEVICE_REQUEST_CALLBACK
idigi_status_t  idigi_get_put_request_data(size_t *size, unsigned int *flags)
{	
    idigi_status_t status = idigi_success;
    
	if (put_request_first_buffer == 1)
	{
		put_request_first_buffer = 0;
        *flags = IDIGI_MSG_FIRST_DATA;
	}
    
    if (put_request_size < PUT_REQUEST_BUFFER_SIZE)
    {
    	*flags |= IDIGI_MSG_LAST_DATA;
    	*size = put_request_size;        
        memcpy(put_request_buffer, put_request_data, put_request_size);
    }
    else
    {
    	*size = PUT_REQUEST_BUFFER_SIZE;
    	memcpy(put_request_buffer, put_request_data, PUT_REQUEST_BUFFER_SIZE);
    	put_request_data += PUT_REQUEST_BUFFER_SIZE;
    	put_request_size -= PUT_REQUEST_BUFFER_SIZE;
    }

    return status;
}
#endif

/*
 *  This function will initiate a put request to the iDigi cloud.
 *
 *  Parameters:
 *      path            -- NUL terminated file path where user wants to store the data on the iDigi cloud.
 *      data            -- Data to write to file on iDigi cloud.
 *      content_type    -- NUL terminated content type (text/plain, text/xml, application/json, etc.
 *      length_in_bytes -- Data length in put_request
 *      flags           -- Indicates whether server should archive and/or append.
 *
 *  Return Value:
 *      idigi_success
 *      idigi_invalid_data      -- Indicates bad parameters
 *      idigi_invalid_response  -- Indicates error response from iDigi cloud
 */
idigi_status_t idigi_initiate_put_request(char const * const path, char const * const data, char const * const content_type,
		                                  size_t const length_in_bytes, unsigned int const flags) 
{
#ifdef USE_NEW_DEVICE_REQUEST_CALLBACK
    idigi_connector_error_t ret;
    int status=-1;
    
    do
    {
        static idigi_connector_data_t ic_data = {0};
        //static char buffer[] = "iDigi Device application data!\n";

        #define WAIT_FOR_A_SECOND  1
        app_os_sleep(WAIT_FOR_A_SECOND);

        ic_data.data_ptr = (char *)data;
        ic_data.length_in_bytes = length_in_bytes;
        ic_data.flags = flags;
        ret = idigi_send_data(path, &ic_data, content_type);

    } while (ret == idigi_connector_init_error);

    if (ret != idigi_connector_success)
    {
        APP_DEBUG("\nSend failed [%d]\n", ret);
        goto error;
    }

#ifdef DEBUG_PUT_REQUEST
    APP_DEBUG("\nSend completed\n");
#endif
    
    status = 0;

error:
    
    return status;
#else
	idigi_status_t status;
	size_t length;
    
    status = idigi_success;
    length = length_in_bytes;
    
    put_request_header.flags = flags;
    put_request_header.content_type = content_type;

    put_request_header.path  = path;
    
    put_request_data = (char *)data;
    put_request_size = length_in_bytes;
    
    /*
     * A call to idigi_initiate_action() must be made from the IDIGI_RUN_TASK because MQX
     * requires the same task that malloc's the data must free it.  If a different task tries
     * to free the data, _mem_free() will fail.   
     */
    
    idigi_put_request_in_progress = 1;
	idigi_put_request_sent = 0;
    
    while (idigi_put_request_in_progress == 1)
        _time_delay(10);
    
    if ((idigi_put_request_response & IDIGI_MSG_RESP_SUCCESS) == 0)
    	status = idigi_invalid_response;
    
    put_request_first_buffer = 1;

    return status;
#endif
}

#ifndef USE_NEW_DEVICE_REQUEST_CALLBACK
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
                idigi_data_service_block_t * message = put_response->client_data;
                size_t size;
                unsigned int flags;
                idigi_data_service_put_request_t const * const header = put_request->service_context;
                
#ifdef DEBUG_PUT_REQUEST
                APP_DEBUG("idigi_data_service_callback: File Path [%s]\n", header->path);
#endif
                
                idigi_get_put_request_data((size_t *)&size, (unsigned int *)&flags);
                
                if (size > PUT_REQUEST_BUFFER_SIZE)
                	size = PUT_REQUEST_BUFFER_SIZE;

                message->length_in_bytes = size;

                memcpy(message->data, put_request_buffer, message->length_in_bytes);
                message->flags = flags;
                put_response->message_status = idigi_msg_error_none;
            }
            break;

        case idigi_data_service_type_have_data:
            {
                idigi_data_service_block_t * const message = put_request->server_data;
                
                idigi_put_request_in_progress = 0;
                
                idigi_put_request_response = message->flags;

#ifdef DEBUG_PUT_REQUEST
                APP_DEBUG("Received %s response from server\n", ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
#endif
                
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
    else if (request == idigi_data_service_device_request)
    {
        idigi_data_service_msg_request_t const * const service_device_request = request_data;
        
        switch (service_device_request->message_type)
        {
            case idigi_data_service_type_have_data:
                status = app_process_device_request(request_data, response_data);
                break;
            case idigi_data_service_type_need_data:
                status = app_process_device_response(request_data, response_data);
                break;
            case idigi_data_service_type_error:
                status = app_process_device_error(request_data, response_data);
                break;
            default:
                APP_DEBUG("app_data_service_handler: unknown message type %d for idigi_data_service_device_request\n", service_device_request->message_type);
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
#endif
