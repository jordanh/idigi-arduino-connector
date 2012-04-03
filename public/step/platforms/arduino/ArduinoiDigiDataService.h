
#ifndef __ARDUINO_IDIGI_DATA_SERVICE_H__
#define __ARDUINO_IDIGI_DATA_SERVICE_H__

extern "C" {
#include <stddef.h>
#include "idigi_api.h"
}



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

class ArduinoiDigiDataService {
public:
  size_t putFile(char *filePath, char *mimeType, char *buffer, size_t length);
  size_t putFileAsync(char *filePath, char *mimeType, char *buffer, size_t length);

  idigi_callback_status_t appReqHandler(idigi_data_service_request_t const request,
                                        void const * request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length);
private:  
  idigi_callback_status_t appReqHandlerNeedData(idigi_data_service_request_t const request,
                                                idigi_data_service_msg_request_t const * const put_request,
                                                size_t const request_length,
                                                idigi_data_service_msg_response_t * const put_response,
                                                size_t * const response_length);
  
  idigi_callback_status_t appReqHandlerHaveData(idigi_data_service_request_t const request,
                                                idigi_data_service_msg_request_t const * const put_request,
                                                size_t const request_length,
                                                idigi_data_service_msg_response_t * const put_response,
                                                size_t * const response_length);
  
  idigi_callback_status_t appReqHandlerError(idigi_data_service_request_t const request,
                                             idigi_data_service_msg_request_t const * const put_request,
                                             size_t const request_length,
                                             idigi_data_service_msg_response_t * const put_response,
                                             size_t * const response_length);
};



#endif /* __ARDUINO_IDIGI_DATA_SERVICE_H__ */

