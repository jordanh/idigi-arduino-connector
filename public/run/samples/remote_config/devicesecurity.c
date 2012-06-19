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

typedef struct {
    idigi_setting_devicesecurity_identityVerificationForm_id_t identity_verification_form;
    char password[DEVICESECURITY_PASSWORD_MAX_LENGTH];
} device_security_data_t;

device_security_data_t device_security_data = {idigi_setting_devicesecurity_identityVerificationForm_simple, "\0"};


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

    ASSERT(request->element.value != NULL);

    switch (request->element.id)
    {
    case idigi_setting_devicesecurity_identityVerificationForm:
    {
        ASSERT(request->element.type == idigi_element_type_enum);
        device_security_ptr->identity_verification_form = (idigi_setting_devicesecurity_identityVerificationForm_id_t)request->element.value->enum_value;
        break;
    }
    case idigi_setting_devicesecurity_password:
    {
        size_t string_length = strlen(request->element.value->string_value);
        ASSERT(request->element.type == idigi_element_type_password);

        if (strlen(request->element.value->string_value) >= sizeof device_security_ptr->password)
        {
            ASSERT(0);
            goto done;
        }

        memcpy(device_security_ptr->password, request->element.value->string_value, string_length);
        device_security_ptr->password[string_length] = '\0';
        break;
    }
    default:
        ASSERT(0);
        break;
    }

done:
    return status;
}

