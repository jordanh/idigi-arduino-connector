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

#include <stdio.h>

#define APP_DEBUG  printf

#define UNUSED_ARGUMENT(x)     ((void)x)

/* Callbacks for this platform */
extern idigi_callback_status_t idigi_os_callback(idigi_os_request_t const request,
                           void * const request_data, size_t const request_length,
                           void * response_data, size_t * const response_length);


idigi_callback_status_t idigi_network_callback(idigi_network_request_t const request,
                         void * const request_data, size_t const request_length,
                         void * response_data, size_t * const response_length);

idigi_callback_status_t idigi_config_callback(idigi_config_request_t const request,
                                              void * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * const response_length);

#endif /* _PLATFORM_H */
