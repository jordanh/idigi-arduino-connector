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

#include "iDigiDataService.h"
#include "iDigiConnector.h"

extern iDigiConnectorClass iDigi;

size_t iDigiDataService::sendFile(const char *filePath, const char *mimeType, 
                                        const char *buffer, size_t length)
{
  return sendFile(filePath, mimeType, buffer, length, 0);
}

size_t iDigiDataService::sendFile(const char *filePath, const char *mimeType, 
                                        const char *buffer, size_t length, unsigned int flags)
{
  unsigned int result = sendFileAsync(filePath, mimeType, &sendFileSyncHandler, flags);
  
  if (result != idigi_success)
  {
    // error occured
    return -result;
  }
  
  _sendFileContext.buffer = buffer;
  _sendFileContext.length = length;
  _sendFileContext.finished_flag = true;
  
  while (!sendFileAsyncFinished())
  {
    iDigi.update();
  }
  
  return _sendFileContext.written;
}

size_t iDigiDataService::sendDataset(iDigiDataset *dataset)
{
  sendDatasetContext context(dataset);

  //APP_DEBUG("iDigiDataService::sendDataset: About to upload dataset \"%s\" (%d samples) \r\n", dataset->name, dataset->size());
  unsigned int result = sendFileAsync("diaData.xml", "text/xml",
                                     &sendDatasetHandler, 0, (void *) &context);

  if (result != idigi_success)
  {
    // error occured
    return -result;
  }
  
  while (!sendFileAsyncFinished())
  {
    iDigi.update(); 
  }

  return _sendFileContext.written;
}

void iDigiDataService::sendDatasetHandler(iDigiSendFileRequest *request)
{
    sendDatasetContext *context = (sendDatasetContext *) request->userContext;

    context->formatBuffer = "";

    if (context->it == context->dataset->begin())
    {
      //APP_DEBUG("iDigiDataService::sendDatasetHandler: begin\r\n");
      // We need to begin the document:
      context->formatBuffer += "<idigi_data compact=\"True\">";
    }

    if (context->it != context->dataset->end())
    {
      // We're in the middle of the vector of samples:
      //APP_DEBUG("iDigiDataService::sendDatasetHandler: middle\r\n");
      context->formatBuffer += "<sample name=\"";
      context->formatBuffer += context->dataset->name;
      context->formatBuffer += ".";
      context->formatBuffer += (*context->it)->name;
      context->formatBuffer += "\" ";
      context->formatBuffer += "value=\"";
      context->formatBuffer += (*context->it)->value;
      context->formatBuffer += "\" ";
      context->formatBuffer += "unit=\"";
      context->formatBuffer += (*context->it)->unit;
      context->formatBuffer += "\" ";
      if (strlen((*context->it)->isoTimestamp) > 0)
      {
        context->formatBuffer += "timestamp=\"";
        context->formatBuffer += (*context->it)->isoTimestamp;
        context->formatBuffer += "\" ";
      }
      context->formatBuffer += "/>";
      context->it++; // increment iterator
    } else
    {
      // We've processed the last sample:
      //APP_DEBUG("iDigiDataService::sendDatasetHandler: end\r\n");
      context->formatBuffer += "</idigi_data>";
      request->finished();
    }

    //APP_DEBUG("iDigiDataService::sendDatasetHandler: %s\r\n", context->formatBuffer.c_str());

    request->length = context->formatBuffer.length();
    request->buffer = context->formatBuffer.c_str();
}

unsigned int iDigiDataService::sendFileAsync(const char *filePath, const char *mimeType,
                                          iDigiSendFileHandler handler)
{
  return sendFileAsync(filePath, mimeType, handler, 0);
}

unsigned int iDigiDataService::sendFileAsync(const char *filePath, const char *mimeType,
                                          iDigiSendFileHandler handler, unsigned int flags)
{
  return sendFileAsync(filePath, mimeType, handler, 0, NULL);
}

unsigned int iDigiDataService::sendFileAsync(const char *filePath, const char *mimeType,
                                          iDigiSendFileHandler handler, unsigned int flags,
                                          void *userContext)
{
  idigi_status_t status = idigi_success;
  
  if (_sendFileContext.active_flag)
  {
    // Another request is already active, do not create a new request
    // APP_DEBUG("iDigiDataService::sendFileAsync(): another request in progress");
    return (unsigned int) idigi_service_busy;
  }

  _sendFileCallback = handler;
  _sendFileContext.written = 0;
  _sendFileContext.userContext = userContext;
  _sendFileContext.finished_flag = 0;
  _sendFileContext.header.flags = flags;
  _sendFileContext.header.path = filePath;
  _sendFileContext.header.content_type = mimeType;
  _sendFileContext.header.context = (void *) &_sendFileContext;
  
  status = idigi_initiate_action(iDigi.getHandle(), idigi_initiate_data_service,
                                 &(_sendFileContext.header), NULL);
  
  if (status == idigi_success)
  {
    _sendFileContext.active_flag = true;
  }

  return (unsigned int) status;
}

void iDigiDataService::attachInterrupt(iDigiDataServiceInterrupt handler)
{
  _requestCallback = handler;
}

bool iDigiDataService::sendResponse(iDigiDataServiceRequest *request,
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
idigi_callback_status_t iDigiDataService::appReqHandler(idigi_data_service_request_t const request,
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
        // APP_DEBUG("iDigiDataService::appReqHandler(): put request not supported: %d\n", put_request->message_type);
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
        // APP_DEBUG("iDigiDataService::appReqHandler(): device request not supported: %d\n", device_request->message_type);
        break;
    } 
  } else
  {
    // APP_DEBUG("iDigiDataService::appReqHandler(): unhandled request %d (not %d or %d)\n", request, idigi_data_service_put_request, idigi_data_service_device_request);
  }
  
  return status;
}

idigi_callback_status_t iDigiDataService::appPutReqHandlerNeedData(idigi_data_service_msg_request_t const * const request_data,
                                                                            idigi_data_service_msg_response_t * const response_data)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = (idigi_data_service_block_t *) response_data->client_data;

  
  size_t copylen = _sendFileContext.length < message->length_in_bytes ? _sendFileContext.length :
                                                                      message->length_in_bytes;
  
  memcpy(message->data, (void *)_sendFileContext.buffer, copylen);
  message->length_in_bytes = copylen;
  _sendFileContext.buffer += copylen;
  _sendFileContext.length -= copylen;
  response_data->message_status = idigi_msg_error_none;
  
  if (_sendFileContext.written == 0)
  {
    message->flags |= IDIGI_MSG_FIRST_DATA;
  }
  _sendFileContext.written += copylen;

  if (_sendFileContext.length == 0)
  {
    if (_sendFileContext.finished_flag)
    {
      message->flags |= IDIGI_MSG_LAST_DATA;
      _sendFileContext.active_flag = false;
    } else
    {
      // not finished, we need more data from the user
      _sendFileCallback(&_sendFileContext);
    }
  }
  
  return status;
}


idigi_callback_status_t iDigiDataService::appPutReqHandlerHaveData(idigi_data_service_msg_request_t const *request_data,
                                                                            idigi_data_service_msg_response_t * const response_data)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = request_data->server_data;

  // APP_DEBUG("Received %s response from server\n", ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
  if (message->length_in_bytes > 0)
  {
    // APP_DEBUG("Server response %s\n", (char *) message->data);
  }
  
  
  return status;
}

idigi_callback_status_t iDigiDataService::appPutReqHandlerError(idigi_data_service_msg_request_t const *request_data,
                                                                         idigi_data_service_msg_response_t * const response_data)
{
  idigi_callback_status_t status = idigi_callback_continue;
  idigi_data_service_block_t * message = request_data->server_data;
  
  _sendFileContext.active_flag = false;
  _sendFileContext.finished_flag = true;
  _sendFileContext.error = *((idigi_msg_error_t *) message->data);
  // APP_DEBUG("Put data service error: %u\n", _sendFileContext.error);
  
  return status;
}

  
idigi_callback_status_t iDigiDataService::appDeviceReqHandlerNeedData(idigi_data_service_msg_request_t const *request_data,
                                                                               idigi_data_service_msg_response_t * const response_data)
{
  iDigiDataServiceRequest *context = (iDigiDataServiceRequest *) response_data->user_context;
  
  if ((response_data->client_data == NULL) || (context == NULL))
  {
    // APP_DEBUG("invalid client data or context is NULL!\n");
    
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


idigi_callback_status_t iDigiDataService::appDeviceReqHandlerHaveData(idigi_data_service_msg_request_t const *request_data,
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
    // APP_DEBUG("device data NULL context!\n");
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

idigi_callback_status_t iDigiDataService::appDeviceReqHandlerError(idigi_data_service_msg_request_t const *request_data,
                                                                            idigi_data_service_msg_response_t * const response_data)
{
  idigi_data_service_block_t * error_data = request_data->server_data;
  idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);
  
  // APP_DEBUG("Device data service error: %d\n", error_code);
  
  return idigi_callback_continue;
}

