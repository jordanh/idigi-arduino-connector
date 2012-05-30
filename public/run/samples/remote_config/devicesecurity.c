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
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"

#define DEVICESECURITY_PASSWORD_MAX_LENGTH 135

typedef enum {
    device_security_simple_type,
    device_security_password_type,
} device_security_type_t;

typedef struct {
    device_security_type_t identity_verification_form;
    char password[DEVICESECURITY_PASSWORD_MAX_LENGTH];
} device_security_data_t;

device_security_data_t device_security_data = {device_security_simple_type, "\0"};


idigi_callback_status_t app_device_security_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    device_security_data_t * const device_security_ptr = &device_security_data;

     switch (request->element.id)
    {
    case idigi_setting_devicesecurity_identityVerificationForm:
    {
        ASSERT(request->element.type == idigi_element_type_enum);
        response->element_data.element_value->enum_value = device_security_ptr->identity_verification_form;
        break;
    }
    case idigi_setting_devicesecurity_password:
        ASSERT(request->element.type == idigi_element_type_password);
        response->element_data.element_value->string_value = device_security_ptr->password;
        break;

    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_device_security_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    device_security_data_t * const device_security_ptr = &device_security_data;

    UNUSED_ARGUMENT(response);

    switch (request->element.id)
    {
    case idigi_setting_devicesecurity_identityVerificationForm:
    {
        ASSERT(request->element.type == idigi_element_type_enum);
        ASSERT(request->element.value != NULL);
        device_security_ptr->identity_verification_form = request->element.value->enum_value;
        break;
    }
    case idigi_setting_devicesecurity_password:
        ASSERT(request->element.type == idigi_element_type_password);

        ASSERT(strlen(request->element.value->string_value) < sizeof device_security_ptr->password);
        memcpy(device_security_ptr->password, request->element.value->string_value, strlen(request->element.value->string_value));
        break;

    default:
        ASSERT(0);
        break;
    }

    return status;
}

