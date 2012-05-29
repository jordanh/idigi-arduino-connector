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

#ifndef DATA_SERVICE_H
#define DATA_SERVICE_H

#include <stdlib.h>
#include "idigi_api.h"

typedef struct
{
    void * next_data;
    size_t bytes_remaining;
    unsigned long event_bit;
    idigi_connector_error_t error;
} idigi_app_send_data_t;

idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                 void const * const request_data, size_t const request_length,
                                                 void * const response_data, size_t * const response_length);
#endif



