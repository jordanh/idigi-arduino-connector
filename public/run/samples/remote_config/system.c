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
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"

#define SYSTEM_STRING_LENGTH 64


typedef struct {
    char contact[SYSTEM_STRING_LENGTH];
    char location[SYSTEM_STRING_LENGTH];
    char description[SYSTEM_STRING_LENGTH];
} system_data_t;

system_data_t system_config_data = {"\0", "\0", "\0"};


idigi_callback_status_t app_system_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    system_data_t * const system_ptr = &system_config_data;

    ASSERT(request->element.type == idigi_element_type_string);

    switch (request->element.id)
    {
    case idigi_setting_system_contact:
        response->element_data.element_value->string_value = system_ptr->contact;
        break;
    case idigi_setting_system_location:
        response->element_data.element_value->string_value = system_ptr->location;
        break;
    case idigi_setting_system_description:
        response->element_data.element_value->string_value = system_ptr->description;
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_system_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    system_data_t * const system_ptr = &system_config_data;
    char * src_ptr = NULL;

    UNUSED_ARGUMENT(response);

    ASSERT(request->element.type == idigi_element_type_string);
    ASSERT(strlen(request->element.value->string_value) < sizeof system_ptr->contact);

    switch (request->element.id)
    {
    case idigi_setting_system_contact:
        src_ptr = system_ptr->contact;
        break;
    case idigi_setting_system_location:
        src_ptr = system_ptr->location;
        break;
    case idigi_setting_system_description:
        src_ptr = system_ptr->description;
        break;
    default:
        ASSERT(0);
        break;
    }

    if (src_ptr != NULL)
    {
        memcpy(src_ptr, request->element.value->string_value, strlen(request->element.value->string_value));
    }
    return status;
}

