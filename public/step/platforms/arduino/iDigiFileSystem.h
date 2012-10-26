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

#ifndef __IDIGI_FILE_SYSTEM_H__
#define __IDIGI_FILE_SYSTEM_H__

#include <Arduino.h>
#include <SD.h>

#include "DigiSD.h"

extern "C" {
#include <stddef.h>
#include "idigi_api.h"
}


class iDigiFileSystem {
  friend class iDigiConnectorClass;
  
  class FileContext {
  public:
  	uint8_t mode;
  };

public:

  iDigiFileSystem() { };

  void enableSharing(uint8_t ethernetChipSelect, uint8_t ssPin);
  void disableSharing();

private:
  uint8_t sdCS, ssPin;

  idigi_callback_status_t appReqHandler(idigi_file_system_request_t const request,
                                        void const * request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length); 

  idigi_callback_status_t app_process_file_open(idigi_file_open_request_t * const request_data,
                                                     idigi_file_open_response_t * const response_data);
  idigi_callback_status_t app_process_file_ftruncate(idigi_file_ftruncate_request_t * const request_data,
                                                          idigi_file_response_t * const response_data);
  idigi_callback_status_t app_process_file_rm(idigi_file_path_request_t * const request_data,
                                                   idigi_file_response_t * const response_data);
  idigi_callback_status_t app_process_file_read(idigi_file_request_t * const request_data,
                                                     idigi_file_data_response_t * const response_data);
  idigi_callback_status_t app_process_file_write(idigi_file_write_request_t * const request_data,
                                                      idigi_file_write_response_t * const response_data);
  idigi_callback_status_t app_process_file_lseek(idigi_file_lseek_request_t * const request_data,
                                                      idigi_file_lseek_response_t * const response_data);
  idigi_callback_status_t app_process_file_close(idigi_file_request_t * const request_data,
                                                      idigi_file_response_t * const response_data);
  idigi_callback_status_t app_process_file_opendir(idigi_file_path_request_t * const request_data,
                                                        idigi_file_open_response_t * const response_data);
  idigi_callback_status_t app_process_file_closedir(idigi_file_request_t * const request_data,
                                                         idigi_file_response_t * const response_data);
  idigi_callback_status_t app_process_file_readdir(idigi_file_request_t * const request_data,
                                                        idigi_file_data_response_t * const response_data);
  idigi_callback_status_t app_process_file_stat(idigi_file_stat_request_t * const request_data,
                                                     idigi_file_stat_response_t * const response_data);
  idigi_callback_status_t app_process_file_strerror(idigi_file_data_response_t * response_data); 
  idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t * const request_data,
                                                          idigi_file_response_t * const response_data);
};

#endif