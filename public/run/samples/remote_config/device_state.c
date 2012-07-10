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
#include <stdlib.h>

#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"

typedef struct {
    int32_t signed_integer;
} device_state_config_data_t;

device_state_config_data_t device_state_config_data = {-10};

idigi_callback_status_t app_device_state_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{

    remote_group_session_t * const session_ptr = response->user_context;

    UNUSED_ARGUMENT(request);

    ASSERT(session_ptr != NULL);

    session_ptr->group_context = &device_state_config_data;

    return idigi_callback_continue;
}

idigi_callback_status_t app_device_state_group_get(idigi_remote_group_request_t const * const  request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;

    device_state_config_data_t * device_state_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_state_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_state_device_state_system_up_time:
    {
        unsigned long uptime;

        ASSERT(request->element.type == idigi_element_type_uint32);

        status = app_os_get_system_time(&uptime);
        response->element_data.element_value->unsigned_integer_value = (uint32_t)uptime;

        break;
    }
    case idigi_state_device_state_signed_integer:
        ASSERT(request->element.type == idigi_element_type_int32);
        response->element_data.element_value->signed_integer_value = device_state_ptr->signed_integer;
        break;

    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_device_state_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    remote_group_session_t * const session_ptr = response->user_context;
    device_state_config_data_t * device_state_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_state_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_state_device_state_signed_integer:
        ASSERT(request->element.type == idigi_element_type_int32);
        device_state_ptr->signed_integer= request->element.value->signed_integer_value;
        break;

    default:
        ASSERT(0);
        break;
    }

    return status;
}

