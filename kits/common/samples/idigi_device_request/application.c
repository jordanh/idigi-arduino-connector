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
#include "idigi_connector.h"

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    printf("idigi_status: status update %d [%s]\n", status, status_message);
}

#define BUFFER_SIZE 256
idigi_app_error_t device_request_callback(char const * const target, idigi_connector_data_t * const request_data)
{
    idigi_app_error_t status=idigi_app_invalid_parameter;
    char buffer[BUFFER_SIZE];

    if (request_data->error != idigi_connector_success)
    {
        printf("devcie_request_callback: error [%d]\n", request_data->error);
        goto error;
    }

    printf("device_request_callback: target [%s]\n", target);
    printf("device_request_callback: length [%d]\n", request_data->length_in_bytes);

    if (request_data->length_in_bytes > (BUFFER_SIZE-1))
    {
        printf("devcie_request_callback: received more data than expected [%d]", request_data->length_in_bytes);
        goto error;
    }

    memcpy(buffer, request_data->data_ptr, request_data->length_in_bytes);
    buffer[request_data->length_in_bytes] = 0;

    printf("device_request_callback received [%s]\n", buffer);
    status = idigi_app_success;

error:
    return status;
}

size_t device_response_callback(char const * const target, idigi_connector_data_t * const response_data)
{
    static char *rsp_string="0123456789012345678901234567890123456789012345678901234567890123456789";
    int len=strlen(rsp_string), bytes_to_copy=0;

    if (response_data->error != idigi_connector_success)
    {
        printf("devcie_response_callback: error [%d]\n", response_data->error);
        goto error;
    }

    printf("devcie_response_callback: target [%s]\n", target);

    bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
    memcpy(response_data->data_ptr, rsp_string, bytes_to_copy);

    response_data->flags = IDIGI_FLAG_LAST_DATA;

    printf("device_response_callback sending response [%s]\n",  (char *)response_data->data_ptr);

error:
    return bytes_to_copy;
}


int application_start(void)
{
    idigi_connector_data_t ic_data;
    idigi_connector_error_t ret;
    int status=-1;

    printf("main: calling idigi_connector_start\n");
    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        printf("idigi_send_data failed [%d]", ret);
        goto error;
    }

    printf("main: calling idigi_register_device_request_callbacks\n");
    ret = idigi_register_device_request_callbacks(device_request_callback, device_response_callback);
    if (ret != idigi_connector_success)
    {
        printf("idigi_register_device_request_callbacks failed [%d]", ret);
        goto error;
    }
    
    status = 0;
    
error:
    return status;
    
}
