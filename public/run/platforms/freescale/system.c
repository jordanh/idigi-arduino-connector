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
#include <time.h>
#include <stdlib.h>
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"
#include "idigi_debug.h"

#define SYSTEM_STRING_LENGTH 64


typedef struct {
    char contact[SYSTEM_STRING_LENGTH];
    char location[SYSTEM_STRING_LENGTH];
    char description[SYSTEM_STRING_LENGTH];
} system_data_t;

system_data_t nvram_system_data = {"\0", "\0", "iDigi connector sample"};

idigi_callback_status_t app_system_session_start(idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(response);
    APP_DEBUG("app_system_session_start\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_system_session_end(idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(response);
    APP_DEBUG("app_system_session_end\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_system_action_start(idigi_remote_group_request_t const * const request,
                                                        idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    APP_DEBUG("app_system_action_start\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_system_action_end(idigi_remote_group_request_t const * const request,
                                                      idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    APP_DEBUG("app_system_action_end\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_system_group_init(idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    void * ptr;
    system_data_t * system;

    UNUSED_ARGUMENT(request);

    APP_DEBUG("app_system_group_init\n");

    ptr = malloc(sizeof *system);
    if (ptr == NULL)
    {
        response->error_id = idigi_setting_system_error_no_memory;
    }
    else
    {
        system = ptr;
        *system = nvram_system_data;
        response->user_context = system;
    }

    return status;
}

idigi_callback_status_t app_system_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    system_data_t * const system_ptr = response->user_context;

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

    system_data_t * const system_ptr = response->user_context;
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

idigi_callback_status_t app_system_group_end(idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response)
{
   system_data_t * const system = response->user_context;

    UNUSED_ARGUMENT(request);

    ASSERT(system != NULL);

    APP_DEBUG("app_system_group_end\n");

    /* should start writing onto NvRam */
    nvram_system_data = *system;

    free(system);

    return idigi_callback_continue;

}

void app_system_session_cancel(void * const context)
{
    ASSERT(context != NULL);

    APP_DEBUG("app_system_group_cancel\n");

    free(context);

}


