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

#include <stdio.h>
#include "platform.h"
#include "idigi_connector.h"

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    APP_DEBUG("idigi_status: status update %d [%s]\n", status, status_message);
}

idigi_app_error_t device_request_callback(char const * const target, idigi_connector_data_t * const request_data)
{
    #define BUFFER_SIZE 256
    static char buffer[BUFFER_SIZE];
    idigi_app_error_t status=idigi_app_invalid_parameter;

    if (request_data->error != idigi_connector_success)
    {
        APP_DEBUG("devcie_request_callback: error [%d]\n", request_data->error);
        goto error;
    }

    if (request_data->length_in_bytes < sizeof buffer)
    {
        memcpy(buffer, request_data->data_ptr, request_data->length_in_bytes);
        buffer[request_data->length_in_bytes] = 0;
    }
    else
    {
        APP_DEBUG("devcie_request_callback: received more data than expected [%d]\n", request_data->length_in_bytes);
        goto error;
    }

    APP_DEBUG("device_request_callback: received [%s] on target [%s]\n", buffer, target);
    status = idigi_app_success;

error:
    return status;
}

size_t device_response_callback(char const * const target, idigi_connector_data_t * const response_data)
{
    static char rsp_string[] = "iDigi Connector device response!\n";
    size_t const len = sizeof rsp_string - 1;
    size_t const bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;

    if (response_data->error != idigi_connector_success)
    {
        APP_DEBUG("devcie_response_callback: error [%d]\n", response_data->error);
        goto error;
    }

    memcpy(response_data->data_ptr, rsp_string, bytes_to_copy);
    response_data->flags = IDIGI_FLAG_LAST_DATA;

    APP_DEBUG("devcie_response_callback: target [%s], data- %s\n", target, rsp_string);

error:
    return bytes_to_copy;
}


int application_start(void)
{
    idigi_connector_data_t ic_data;
    idigi_connector_error_t ret;
    int status=-1;

    APP_DEBUG("application_start: calling idigi_connector_start\n");
    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_connector_start failed [%d]\n", ret);
        goto error;
    }

    APP_DEBUG("application_start: calling idigi_register_device_request_callbacks\n");
    ret = idigi_register_device_request_callbacks(device_request_callback, device_response_callback);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_register_device_request_callbacks failed [%d]\n", ret);
        goto error;
    }
 
    status = 0;

error:
    return status;
    
}
