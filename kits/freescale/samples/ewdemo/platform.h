/*
* Copyright (c) 2011, 2012 Digi International Inc.,
* All rights not expressly granted are reserved.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
* =======================================================================
*/
#ifndef _PLATFORM_H
#define _PLATFORM_H

// #define DEBUG_MALLOC

#define APP_DEBUG   _io_printf

#define PUT_REQUEST_BUFFER_SIZE 512
#define DEVICE_REQUEST_BUFFER_SIZE 512

#define USE_NEW_DEVICE_REQUEST_CALLBACK

#define UNUSED_ARGUMENT(x)     ((void)x)

typedef size_t (* device_request_callback_function_t)(char const * const target,
                                                      void const * const reqest_data, size_t const request_bytes,
                                                      void * const response_data, size_t const max_response_bytes);

/* Callbacks for this platform */
idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                           void const * const request_data, size_t const request_length,
                           void * const response_data, size_t * const response_length);

idigi_callback_status_t app_network_handler(idigi_network_request_t const request,
                         void const * const request_data, size_t const request_length,
                         void * const response_data, size_t * const response_length);

idigi_callback_status_t app_config_handler(idigi_config_request_t const request,
                                              void const * const request_data,
                                              size_t const request_length,
                                              void * const response_data,
                                              size_t * const response_length);

int application_run(idigi_handle_t handle);
void app_os_sleep(unsigned int const timeout_in_seconds);

#endif /* _PLATFORM_H */
