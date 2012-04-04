
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

void ArduinoiDigiDataService::registerHandler(iDigiDataServiceHandler handler)
{
  _requestCallback = handler;
}

bool ArduinoiDigiDataService::sendResponse(iDigiDataServiceRequest *request,
                                               char *buffer, size_t length)
{
  if (!request->isFinished())
  {
    return false;
  }
  
  request->setResponse(buffer, length);
  
  return true;
}

idigi_callback_status_t ArduinoiDigiDataService::appReqHandler(idigi_data_service_request_t const request,
                                                 void const * request_data, size_t const request_length,
                                                 void * response_data, size_t * const response_length)
{
  idigi_callback_status_t status = idigi_callback_continue;
  UNUSED_ARGUMENT(request_length);
  UNUSED_ARGUMENT(response_length);

  if ((request_data == NULL) || (response_data == NULL))
    return status;
  
  if (request == idigi_data_service_put_request)
  {
    idigi_data_service_msg_request_t const * const put_request = (idigi_data_service_msg_request_t *) request_data;
    
    switch (put_request->message_type)
    {
    case idigi_data_service_type_need_data:
        status = appPutReqHandlerNeedData((idigi_data_service_msg_request_t *)request_data,
                                          (idigi_data_service_msg_response_t *)response_data);
        break;
      case idigi_data_service_type_have_data:
        status = appPutReqHandlerHaveData((idigi_data_service_msg_request_t *)request_data,
                                           (idigi_data_service_msg_response_t *)response_data);
        break;        
    case idigi_data_service_type_error:
        status = appPutReqHandlerError((idigi_data_service_msg_request_t *)request_data,
                                       (idigi_data_service_msg_response_t *)response_data);
        break;                
    default:
        AR_DEBUG_PRINTF("ArduinoiDigiDataService::appReqHandler(): put request not supported: %d\n", put_request->message_type);
        break;
    }
  } else if (request == idigi_data_service_device_request)
  {
    idigi_data_service_msg_request_t const * const device_request = (idigi_data_service_msg_request_t *) request_data;
    
    switch (device_request->message_type)
    {
      case idigi_data_service_type_have_data:
        status = appDeviceReqHandlerHaveData((idigi_data_service_msg_request_t *)request_data,
                                             (idigi_data_service_msg_response_t *)response_data);
        break;
      case idigi_data_service_type_need_data:
        status = appDeviceReqHandlerNeedData((idigi_data_service_msg_request_t *)request_data,
                                             (idigi_data_service_msg_response_t *)response_data);
        break;
      case idigi_data_service_type_error:
        status = appDeviceReqHandlerError((idigi_data_service_msg_request_t *)request_data,
                                          (idigi_data_service_msg_response_t *)response_data);
        break;
      default:
        AR_DEBUG_PRINTF("ArduinoiDigiDataService::appReqHandler(): device request not supported: %d\n", device_request->message_type);
        break;
    } 
  } else
  {
    AR_DEBUG_PRINTF("ArduinoiDigiDataService::appReqHandler(): unhandled request %d\n", request);
  }
  
  return status;
}

idigi_callback_status_t ArduinoiDigiDataService::appPutReqHandlerNeedData(idigi_data_service_msg_request_t const * const request_data,
                                                                            idigi_data_service_msg_response_t * const response_data)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_put_request_t *request_header = (idigi_data_service_put_request_t *) request_data->service_context;
  idigi_data_service_block_t * message = (idigi_data_service_block_t *) response_data->client_data;
  arids_putfile_context_t *putfile_context = (arids_putfile_context_t *) request_header->context;
  
  size_t maxwrite = putfile_context->action.data.length - putfile_context->written;
  size_t copylen =  maxwrite < message->length_in_bytes ? maxwrite : message->length_in_bytes;
  
  switch(putfile_context->type)
  {
    case arids_type_simple_buffer:
      memcpy(message->data, (void *) (putfile_context->action.data.buffer+putfile_context->written), copylen);
      message->length_in_bytes = copylen;
      response_data->message_status = idigi_msg_error_none;
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


idigi_callback_status_t ArduinoiDigiDataService::appPutReqHandlerHaveData(idigi_data_service_msg_request_t const * const request_data,
                                                                            idigi_data_service_msg_response_t * const response_data)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = request_data->server_data;

  AR_DEBUG_PRINTF("Received %s response from server\n", ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
  if (message->length_in_bytes > 0)
  {
    AR_DEBUG_PRINTF("Server response %s\n", (char *) message->data);
  }
  
  
  return status;
}

idigi_callback_status_t ArduinoiDigiDataService::appPutReqHandlerError(idigi_data_service_msg_request_t const * const request_data,
                                                                         idigi_data_service_msg_response_t * const response_data)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = request_data->server_data;
  idigi_msg_error_t const * const error_value = (idigi_msg_error_t *) message->data;
  
  AR_DEBUG_PRINTF("put data service error: %d\n", *error_value);
  
  return status;
}

  
idigi_callback_status_t ArduinoiDigiDataService::appDeviceReqHandlerNeedData(idigi_data_service_msg_request_t const * const request_data,
                                                                               idigi_data_service_msg_response_t * const response_data)
{
  iDigiDataServiceRequest *context = (iDigiDataServiceRequest *) response_data->user_context;
  
  if ((response_data->client_data == NULL) || (context == NULL))
  {
    AR_DEBUG_PRINTF("invalid client data or context is NULL!\n");
    
    return idigi_callback_continue;
  }
  
  idigi_data_service_block_t * const client_data = (idigi_data_service_block_t * const) response_data->client_data;
  size_t const copylen = (context->length < client_data->length_in_bytes) ? 
                            context->length : client_data->length_in_bytes;
  
  memcpy(client_data->data, context->buffer, copylen);
  context->buffer += copylen;
  context->length -= copylen;
  client_data->length_in_bytes = copylen;
  client_data->flags = (context->length == 0) ? IDIGI_MSG_LAST_DATA : 0;
  if (context->target == NULL)
  {
    client_data->flags |= IDIGI_MSG_DATA_NOT_PROCESSED;
  }
  
  response_data->message_status = idigi_msg_error_none;
  
  return idigi_callback_continue;
}


idigi_callback_status_t ArduinoiDigiDataService::appDeviceReqHandlerHaveData(idigi_data_service_msg_request_t const * const request_data,
                                                                               idigi_data_service_msg_response_t * const response_data)
{
  idigi_data_service_device_request_t * const server_device_request = (idigi_data_service_device_request_t * const) request_data->service_context;
  idigi_data_service_block_t * const server_data = (idigi_data_service_block_t * const) request_data->server_data;  
  
  if (server_data == NULL)
  {
    return idigi_callback_continue;
  }
  
  if (server_data->flags & IDIGI_MSG_FIRST_DATA)
  {
    if (server_device_request->target == NULL)
    {
      return idigi_callback_continue;
    }

    _requestContext.device_handle = server_device_request->device_handle;
    strlcpy(_requestContext.target, server_device_request->target, IDIGI_DS_TARGET_LEN);
    _requestContext.buffer = NULL;
    _requestContext.length = 0;
    _requestContext.flags = 0;
    _requestContext.is_response = false;
    
    response_data->user_context = &_requestContext;
  }
  
  iDigiDataServiceRequest *context = (iDigiDataServiceRequest *) response_data->user_context;
  
  if (context == NULL)
  {
    AR_DEBUG_PRINTF("device data NULL context!\n");
    return idigi_callback_continue;
  }
  
  if (_requestCallback == NULL)
  {
    response_data->message_status = idigi_msg_error_cancel;
    return idigi_callback_continue;
  }
  
  context->buffer = (char *) server_data->data;
  context->length = server_data->length_in_bytes;
  context->flags = server_data->flags;
  (_requestCallback)(context);
  
  return idigi_callback_continue;
}

idigi_callback_status_t ArduinoiDigiDataService::appDeviceReqHandlerError(idigi_data_service_msg_request_t const * const request_data,
                                                                            idigi_data_service_msg_response_t * const response_data)
{
  idigi_data_service_block_t * error_data = request_data->server_data;
  idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);
  
  AR_DEBUG_PRINTF("device data service error: %d\n", error_code);
  
  return idigi_callback_continue;
}
