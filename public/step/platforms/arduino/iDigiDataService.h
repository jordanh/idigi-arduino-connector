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

#ifndef __IDIGI_DATA_SERVICE_H__
#define __IDIGI_DATA_SERVICE_H__

#include <WString.h>
#include "Vector.h"

extern "C" {
#include <stddef.h>
#include "idigi_api.h"
}

#define IDIGI_DS_TARGET_LEN  32

class DigiString : public String
{
  // Extension of Arduino Wiring string to have access to internal buffer
public:
  DigiString & operator= (const char *cstr) {
        return (DigiString &) String::operator=(cstr);
  };
  char *c_str() { return buffer; };
};


class iDigiPutFileRequest {
  friend class iDigiDataService;
  
public:
  iDigiPutFileRequest():active_flag(false), finished_flag(false) { };
  
  const char *buffer;
  size_t length;
  size_t written;
  void *userContext;
  
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
  friend class iDigiDataService;
  
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


class iDigiDiaSample {
public:
  char *name, *unit, *value, *isoTimestamp;
  iDigiDiaSample() { name = unit = value = isoTimestamp = NULL; };
  iDigiDiaSample(const char *name, const char *value, const char *unit, const char *isoTimestamp) { 
      this->name = strdup(name);
      this->value = strdup(value);
      this->unit = strdup(unit);
      this->isoTimestamp = strdup(isoTimestamp);
  };
  ~iDigiDiaSample() { 
      free(this->name);
      free(this->value);
      free(this->unit);
      free(this->isoTimestamp);
  };
};

class iDigiDiaDataset {
public:
    char *name;
    typedef iDigiDiaSample *iterator;
    typedef iDigiDiaSample * const const_iterator;

    iDigiDiaDataset(size_t maxNumSamples, const char *name) {
      this->name = strdup(name);
      this->_maxNumSamples = maxNumSamples;
      _samples = new Vector<iDigiDiaSample *>();
    };
    ~iDigiDiaDataset() {
      dealloc();
      free(this->name);
    };
    bool add(const char *name, const char *value, const char *unit) {
      return add(name, value, unit, "");
    };
    bool add(const char *name, const char *value, const char *unit, const char *isoTimestamp) {
      if (_samples->size() >= _maxNumSamples)
        return false;
      iDigiDiaSample *sample = new iDigiDiaSample(name, value, unit, isoTimestamp);
      _samples->push_back(sample);
      return true;
    };

    iterator operator[](size_t idx) const { return (*_samples)[idx]; };
    size_t size() { return _samples->size(); }
    iterator begin() { return (*this)[0]; }
    iterator end() { return (*this)[this->size()-1]; }
    bool spaceAvailable() { return _samples->size() < _maxNumSamples; }
    
    void clear() { dealloc(); _samples = new Vector<iDigiDiaSample *>(); };

private:
    Vector <iDigiDiaSample *> *_samples;
    size_t _maxNumSamples;

    void dealloc() {
      for (size_t i=0; i < _samples->size(); i++)
        delete (*_samples)[i];
      delete _samples;
    }
};

class putDiaDatasetContext {
public:
  DigiString formatBuffer;
  iDigiDiaDataset *dataset;
  iDigiDiaDataset::iterator it;

  putDiaDatasetContext():dataset(NULL), it(NULL) {};
  putDiaDatasetContext(iDigiDiaDataset *ds) { dataset = ds; it = ds->begin(); }
};

class iDigiDataService {
  friend class iDigiConnectorClass;
  
public:
  iDigiDataService():_putFileCallback(NULL), _putFileContext(),
                            _requestCallback(NULL), _requestContext() { };
  
  size_t putFile(const char *filePath, const char *mimeType, const char *buffer, size_t length);
  size_t putFile(const char *filePath, const char *mimeType, const char *buffer, size_t length, unsigned int flags);
  size_t putDiaDataset(iDigiDiaDataset *dataset);
  unsigned int putFileAsync(const char *filePath, const char *mimeType, iDigiPutFileHandler handler);
  unsigned int putFileAsync(const char *filePath, const char *mimeType, iDigiPutFileHandler handler, unsigned int flags);
  unsigned int putFileAsync(const char *filePath, const char *mimeType, iDigiPutFileHandler handler, unsigned int flags,
                            void *userContext);
  bool putFileAsyncBusy() { return _putFileContext.active_flag; }

  void registerHandler(iDigiDataServiceHandler callback);
  bool sendResponse(iDigiDataServiceRequest *request, char *buffer, size_t length);



private:
  iDigiPutFileHandler     _putFileCallback;
  iDigiPutFileRequest     _putFileContext;
  iDigiDataServiceHandler _requestCallback;
  iDigiDataServiceRequest _requestContext;

  static void putFileSyncHandler(iDigiPutFileRequest *request) { }
  static void putDiaDatasetHandler(iDigiPutFileRequest *request);
  
  idigi_callback_status_t appReqHandler(idigi_data_service_request_t const request,
                                        void const * request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length);  
  
  idigi_callback_status_t appPutReqHandlerNeedData(idigi_data_service_msg_request_t const *request_data,
                                                   idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appPutReqHandlerHaveData(idigi_data_service_msg_request_t const *request_data,
                                                   idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appPutReqHandlerError(idigi_data_service_msg_request_t const *request_data,
                                                idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appDeviceReqHandlerNeedData(idigi_data_service_msg_request_t const *request_data,
                                                      idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appDeviceReqHandlerHaveData(idigi_data_service_msg_request_t const *request_data,
                                                      idigi_data_service_msg_response_t * const response_data);
  
  idigi_callback_status_t appDeviceReqHandlerError(idigi_data_service_msg_request_t const *request_data,
                                                   idigi_data_service_msg_response_t * const response_data);
};


#endif /* __ARDUINO_IDIGI_DATA_SERVICE_H__ */

