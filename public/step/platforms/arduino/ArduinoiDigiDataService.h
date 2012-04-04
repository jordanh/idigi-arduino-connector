
#ifndef __ARDUINO_IDIGI_DATA_SERVICE_H__
#define __ARDUINO_IDIGI_DATA_SERVICE_H__

extern "C" {
#include <stddef.h>
#include "idigi_api.h"
}

#define IDIGI_DS_TARGET_LEN  64

typedef enum {
  arids_type_simple_buffer,
  arids_type_async_callback,
} arids_putfile_context_types_t;

typedef unsigned int (* arids_callback_t)(char *buffer, size_t *length);

typedef struct {
  char *buffer;
  size_t length;
} arids_data_block_t;

typedef enum {
  arids_context_state_init,
  arids_context_state_done,
} arids_context_state_t;

typedef struct {
  arids_putfile_context_types_t type;
  union {
    arids_callback_t callback;
    arids_data_block_t data;
  } action;
  size_t written;
  arids_context_state_t state;
} arids_putfile_context_t;


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
  ArduinoiDigiDataService():_requestCallback(NULL), _requestContext() { };
  
  size_t putFile(char *filePath, char *mimeType, char *buffer, size_t length);
  void registerHandler(iDigiDataServiceHandler callback);
  bool sendResponse(iDigiDataServiceRequest *request, char *buffer, size_t length);

private: 
  iDigiDataServiceHandler _requestCallback;
  iDigiDataServiceRequest _requestContext;
  
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

