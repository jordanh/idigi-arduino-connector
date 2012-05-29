/*
 * Copyright (c) 2012 Digi International Inc.,
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

#include "idigi_api.h"

#define APP_DEBUG   _io_printf

#define UNUSED_ARGUMENT(x)     ((void)x)

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
idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds);

#endif /* _PLATFORM_H */
