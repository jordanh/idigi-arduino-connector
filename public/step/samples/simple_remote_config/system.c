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

idigi_callback_status_t app_system_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;
    system_data_t * system_ptr;

    void * ptr;

    UNUSED_ARGUMENT(request);

    ptr = malloc(sizeof *system_ptr);
    if (ptr == NULL)
    {
        response->error_id = idigi_global_error_memory_fail;
        goto done;
    }

    system_ptr = ptr;
    *system_ptr = system_config_data;  /* load data */
    session_ptr->group_context = system_ptr;

done:
    return status;
}

idigi_callback_status_t app_system_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;
    system_data_t * const system_ptr = session_ptr->group_context;

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
    remote_group_session_t * const session_ptr = response->user_context;
    system_data_t * const system_ptr = session_ptr->group_context;

    char * src_ptr = NULL;

    UNUSED_ARGUMENT(response);

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
        size_t const length = strlen(request->element.value->string_value);
        memcpy(src_ptr, request->element.value->string_value, length);
        src_ptr[length] = '\0';
    }
    return status;
}

idigi_callback_status_t app_system_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;
    system_data_t * const system_ptr = session_ptr->group_context;

    if (request->action == idigi_remote_action_set)
    {
        /* save data */
        system_config_data = *system_ptr;
    }

    free(system_ptr);

    return status;
}

void app_system_group_cancel(void * const context)
{
    remote_group_session_t * const session_ptr = context;
    system_data_t * const system_ptr = session_ptr->group_context;

    if (system_ptr != NULL)
    {
        free(system_ptr);
    }

}

