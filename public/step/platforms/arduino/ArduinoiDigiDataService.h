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

#ifndef __ARDUINO_IDIGI_DATA_SERVICE_H__
#define __ARDUINO_IDIGI_DATA_SERVICE_H__

extern "C" {
#include <stddef.h>
#include "idigi_api.h"
}

#define IDIGI_DS_TARGET_LEN  64


class iDigiPutFileRequest {
  friend class ArduinoiDigiDataService;
  
public:
  iDigiPutFileRequest():active_flag(false), finished_flag(false) { };
  
  char *buffer;
  size_t length;
  size_t written;
  
  void finished() { finished_flag = true; }
  bool isFinished() { return finished_flag; }
  const char *getFilePath() { return header.path; }
  const char *getMimeType() { return header.content_type; }
  const unsigned int getError() { return (unsigned int) error; }
  
private:
  bool active_flag;
  bool finished_flag;
  idigi_data_service_put_request_t header;
  idigi_msg_error_t error;
};

typedef void (* iDigiPutFileHandler)(iDigiPutFileRequest *);

class iDigiDataServiceRequest {
  friend class ArduinoiDigiDataService;
  
public:
  char *getTarget() { return target; }
  char *getBuffer() { return buffer; }
  size_t getLength() { return length; }
  bool isFirst() { return flags & IDIGI_MSG_FIRST_DATA ? 1 : 0; }
  bool isFinished() { return flags & IDIGI_MSG_LAST_DATA ? 1: 0; }
  
  void setResponse(char *response, size_t length) { buffer = response;
                                                    this->length = length;
                                                    is_response = true; }
  
private:
  void *device_handle;
  char target[IDIGI_DS_TARGET_LEN];
  char *buffer;
  size_t length;
  unsigned int flags;
  bool is_response;
};

typedef void (* iDigiDataServiceHandler)(iDigiDataServiceRequest *);

class ArduinoiDigiDataService {
  friend class ArduinoiDigiInterfaceClass;
  
public:
  ArduinoiDigiDataService():_putFileCallback(NULL), _putFileContext(),
                            _requestCallback(NULL), _requestContext() { };
  
  size_t putFile(char *filePath, char *mimeType, char *buffer, size_t length);
  size_t putFile(char *filePath, char *mimeType, char *buffer, size_t length, unsigned int flags);
  unsigned int putFileAsync(char *filePath, char *mimeType, iDigiPutFileHandler handler);
  unsigned int putFileAsync(char *filePath, char *mimeType, iDigiPutFileHandler handler, unsigned int flags);
  bool putFileAsyncBusy() { return _putFileContext.active_flag; }
  void registerHandler(iDigiDataServiceHandler callback);
  bool sendResponse(iDigiDataServiceRequest *request, char *buffer, size_t length);

private:
  iDigiPutFileHandler     _putFileCallback;
  iDigiPutFileRequest     _putFileContext;
  iDigiDataServiceHandler _requestCallback;
  iDigiDataServiceRequest _requestContext;

  static void putFileSyncHandler(iDigiPutFileRequest *request) { }
  
  idigi_callback_status_t appReqHandler(idigi_data_service_request_t const request,
                                        void const * request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length);  
  
  idigi_callback_status_t appPutReqHandlerNeedData(idigi_data_service_msg_request_t const * const request_data,
                                                   idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appPutReqHandlerHaveData(idigi_data_service_msg_request_t const * const request_data,
                                                   idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appPutReqHandlerError(idigi_data_service_msg_request_t const * const request_data,
                                                idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appDeviceReqHandlerNeedData(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appDeviceReqHandlerHaveData(idigi_data_service_msg_request_t const * const request_data,
                                                      idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appDeviceReqHandlerError(idigi_data_service_msg_request_t const * const request_data,
                                                   idigi_data_service_msg_response_t * const response_data);
};



#endif /* __ARDUINO_IDIGI_DATA_SERVICE_H__ */

