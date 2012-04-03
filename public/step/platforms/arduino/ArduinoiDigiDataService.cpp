
#include "ArduinoiDigiDataService.h"
#include "ArduinoiDigiInterface.h"

extern ArduinoiDigiInterfaceClass iDigi;

size_t ArduinoiDigiDataService::putFile(char *filePath, char *mimeType, char *buffer, size_t length)
{
  idigi_status_t status = idigi_success;
  idigi_data_service_put_request_t header;
  arids_putfile_context_t context;
  
  context.type = arids_type_simple_buffer;
  context.action.data.buffer = buffer;
  context.action.data.length = length;
  context.state = arids_context_state_init;
  context.written = 0;
  
  header.flags = IDIGI_DATA_PUT_APPEND;
  header.path = filePath;
  header.content_type = mimeType;
  header.context = (void *) &context;
  
  status = idigi_initiate_action(iDigi.getHandle(), idigi_initiate_data_service,
                                 &header, NULL);
  
  if (status != idigi_success)
    return -status;
  
  while (context.state != arids_context_state_done)
  {
    iDigi.step();
  }
  
  return context.written;
}

idigi_callback_status_t ArduinoiDigiDataService::appReqHandler(idigi_data_service_request_t const request,
                                                 void const * request_data, size_t const request_length,
                                                 void * response_data, size_t * const response_length)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_msg_request_t const * const put_request = (idigi_data_service_msg_request_t *) request_data;
  idigi_data_service_msg_response_t * const put_response = (idigi_data_service_msg_response_t *) response_data;
  
  UNUSED_ARGUMENT(request_length);
  UNUSED_ARGUMENT(response_length);
  
  if ((put_request == NULL) || (put_response == NULL))
    return status;
  
  if (request == idigi_data_service_put_request)
  {
    switch (put_request->message_type)
    {
    case idigi_data_service_type_need_data:
        status = appReqHandlerNeedData(request, put_request, request_length,
                              put_response, response_length);
        break;
    case idigi_data_service_type_have_data:
        status = appReqHandlerHaveData(request, put_request, request_length,
                                       put_response, response_length);
        break;        
    case idigi_data_service_type_error:
        status = appReqHandlerError(request, put_request, request_length,
                                       put_response, response_length);
        break;                
    default:
        AR_DEBUG_PRINTF("ArduinoiDigiDataService::appReqHandler(): request not supported: %d\n", request);
        break;
    }
  }
  
  return status;
}

idigi_callback_status_t ArduinoiDigiDataService::appReqHandlerNeedData(idigi_data_service_request_t const request,
                                              idigi_data_service_msg_request_t const * const put_request, size_t const request_length,
                                              idigi_data_service_msg_response_t * const put_response, size_t * const response_length)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_put_request_t *request_header = (idigi_data_service_put_request_t *) put_request->service_context;
  idigi_data_service_block_t * message = (idigi_data_service_block_t *) put_response->client_data;
  arids_putfile_context_t *putfile_context = (arids_putfile_context_t *) request_header->context;
  
  size_t maxwrite = putfile_context->action.data.length - putfile_context->written;
  size_t copylen =  maxwrite < message->length_in_bytes ? maxwrite : message->length_in_bytes;
  
  switch(putfile_context->type)
  {
    case arids_type_simple_buffer:
      memcpy(message->data, (void *) (putfile_context->action.data.buffer+putfile_context->written), copylen);
      message->length_in_bytes = copylen;
      put_response->message_status = idigi_msg_error_none;
      if (putfile_context->written == 0)
      {
        message->flags |= IDIGI_MSG_FIRST_DATA;
      }
      
      putfile_context->written += copylen;
      
      if (putfile_context->written >= putfile_context->action.data.length)
      {
        message->flags |= IDIGI_MSG_LAST_DATA;
        putfile_context->state = arids_context_state_done;
      }    
      break;
    case arids_type_async_callback:
      message->flags = (putfile_context->action.callback)((char *) message->data, &(message->length_in_bytes));
      putfile_context->written += message->length_in_bytes;
      if (message->flags & IDIGI_MSG_LAST_DATA)
        putfile_context->state = arids_context_state_done;
      break;
    default:
      break;
  }
  
  return status;
}


idigi_callback_status_t ArduinoiDigiDataService::appReqHandlerHaveData(idigi_data_service_request_t const request,
                                              idigi_data_service_msg_request_t const * const put_request, size_t const request_length,
                                              idigi_data_service_msg_response_t * const put_response, size_t * const response_length)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = put_request->server_data;

  AR_DEBUG_PRINTF("Received %s response from server\n", ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
  if (message->length_in_bytes > 0)
  {
    AR_DEBUG_PRINTF("Server response %s\n", (char *) message->data);
  }
  
  
  return status;
}

idigi_callback_status_t ArduinoiDigiDataService::appReqHandlerError(idigi_data_service_request_t const request,
                                              idigi_data_service_msg_request_t const * const put_request, size_t const request_length,
                                              idigi_data_service_msg_response_t * const put_response, size_t * const response_length)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = put_request->server_data;
  idigi_msg_error_t const * const error_value = (idigi_msg_error_t *) message->data;
  
  AR_DEBUG_PRINTF("Data service error: %d\n", *error_value);
  
  return status;
}


