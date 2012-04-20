/*
 * Copyright (c) 1996-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include "ArduinoiDigiDataService.h"
#include "ArduinoiDigiInterface.h"

extern ArduinoiDigiInterfaceClass iDigi;

size_t ArduinoiDigiDataService::putFile(char *filePath, char *mimeType, 
                                        char *buffer, size_t length)
{
  return putFile(filePath, mimeType, buffer, length, 0);
}

size_t ArduinoiDigiDataService::putFile(char *filePath, char *mimeType, 
                                        char *buffer, size_t length, unsigned int flags)
{
  unsigned int result = putFileAsync(filePath, mimeType, &putFileSyncHandler, flags);
  
  if (result != idigi_success) {
    // error occured
    return -result;
  }
  
  _putFileContext.buffer = buffer;
  _putFileContext.length = length;
  _putFileContext.finished_flag = true;
  
  while (putFileAsyncBusy())
  {
    iDigi.step();
  }
  
  return _putFileContext.written;
}

unsigned int ArduinoiDigiDataService::putFileAsync(char *filePath, char *mimeType,
                                          iDigiPutFileHandler handler)
{
  return putFileAsync(filePath, mimeType, handler, 0);
}

unsigned int ArduinoiDigiDataService::putFileAsync(char *filePath, char *mimeType,
                                          iDigiPutFileHandler handler, unsigned int flags)
{
  idigi_status_t status = idigi_success;
  
  if (_putFileContext.active_flag)
  {
    // Another request is already active, do not create a new request
    return (unsigned int) idigi_service_busy;
  }

  _putFileCallback = handler;
  _putFileContext.written = 0;
  _putFileContext.finished_flag = 0;
  _putFileContext.header.flags = flags;
  _putFileContext.header.path = filePath;
  _putFileContext.header.content_type = mimeType;
  _putFileContext.header.context = (void *) &_putFileContext;
  
  status = idigi_initiate_action(iDigi.getHandle(), idigi_initiate_data_service,
                                 &(_putFileContext.header), NULL);
  
  if (status == idigi_success)
  {
    _putFileContext.active_flag = true;
  }
  
  return (unsigned int) status;
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

// private definitions
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
  idigi_data_service_block_t * message = (idigi_data_service_block_t *) response_data->client_data;

  
  size_t copylen = _putFileContext.length < message->length_in_bytes ? _putFileContext.length :
                                                                      message->length_in_bytes;
  
  memcpy(message->data, (void *)_putFileContext.buffer, copylen);
  message->length_in_bytes = copylen;
  _putFileContext.buffer += copylen;
  _putFileContext.length -= copylen;
  response_data->message_status = idigi_msg_error_none;
  
  if (_putFileContext.written == 0)
  {
    message->flags |= IDIGI_MSG_FIRST_DATA;
  }
  _putFileContext.written += copylen;

  if (_putFileContext.length == 0)
  {
    if (_putFileContext.finished_flag)
    {
      message->flags |= IDIGI_MSG_LAST_DATA;
      _putFileContext.active_flag = false;
    } else
    {
      // not finished, we need more data from the user
      _putFileCallback(&_putFileContext);
    }
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
  
  _putFileContext.active_flag = false;
  _putFileContext.finished_flag = true;
  _putFileContext.error = *((idigi_msg_error_t *) message->data);
  AR_DEBUG_PRINTF("put data service error: %u\n", _putFileContext.error);
  
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
