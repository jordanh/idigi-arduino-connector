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
#include <malloc.h>
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"

#if !defined IDIGI_RCI_SERVICE
#error "Must define IDIGI_RCI_SERVICE in idigi_config.h to run this sample"
#endif

typedef idigi_callback_status_t(* remote_group_cb_t) (idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
typedef void (* remote_group_cancel_cb_t) (void * const context);

typedef struct remote_group_table {
    remote_group_cb_t init_cb;
    remote_group_cb_t set_cb;
    remote_group_cb_t get_cb;
    remote_group_cb_t end_cb;
    remote_group_cancel_cb_t cancel_cb;
} remote_group_table_t;


remote_group_table_t remote_setting_table[] = {
    {app_serial_group_init,         app_serial_group_set,       app_serial_group_get,       app_serial_group_end,       app_serial_group_cancel},
    {app_ethernet_group_init,       app_ethernet_group_set,     app_ethernet_group_get,     app_ethernet_group_end,     app_ethernet_group_cancel},
    {app_device_stats_group_init,   app_device_stats_group_set, app_device_stats_group_get, NULL, NULL},
    {app_device_info_group_init,    app_device_info_group_set,  app_device_info_group_get,  app_device_info_group_end,  app_device_info_group_cancel},
    {NULL,                          app_system_group_set,          app_system_group_get,          NULL,  NULL},
    {NULL,                          app_device_security_group_set, app_device_security_group_get, NULL,  NULL}
};


remote_group_table_t remote_state_table[] = {
    {NULL, NULL, app_debug_info_group_get, NULL, NULL},
    {NULL, NULL, app_gps_stats_group_get, NULL, NULL}
};


static idigi_callback_status_t app_process_session_start(idigi_remote_group_response_t * const response)
{
    void * ptr;
    remote_group_session_t * session_ptr;

    APP_DEBUG("app_process_session_start\n");

    ptr = malloc(sizeof *session_ptr);
    if (ptr == NULL)
    {
        response->error_id = idigi_global_error_memory_fail;
        goto done;
    }

    session_ptr = ptr;
    session_ptr->group_context = NULL;

done:
    response->user_context = ptr;
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_session_end(idigi_remote_group_response_t * const response)
{
    APP_DEBUG("app_process_session_end\n");

    if (response->user_context != NULL)
    {
        free(response->user_context);
    }
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_action_start(idigi_remote_group_request_t const * const request,
                                                        idigi_remote_group_response_t * const response)
{
    APP_DEBUG("app_process_action_start\n");

    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_action_end(idigi_remote_group_request_t const * const request,
                                                      idigi_remote_group_response_t * const response)
{
    APP_DEBUG("app_process_action_end\n");

    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_group(idigi_remote_config_request_t const request_id,
                                                 idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_table_t * group_ptr = NULL;
    remote_group_cb_t callback;

    switch (request->group.type)
    {
    case idigi_remote_group_setting:

        if (request->group.id >= asizeof(remote_setting_table))
        {
            ASSERT(0);
            goto done;
        }

        group_ptr = &remote_setting_table[request->group.id];
        break;
    case idigi_remote_group_state:
        if (request->group.id >= asizeof(remote_state_table))
        {
            ASSERT(0);
            goto done;
        }
        group_ptr = &remote_state_table[request->group.id];
        break;
    }

    switch (request_id)
    {
    case idigi_remote_config_group_start:
        APP_DEBUG("app_process_group_start group_id = %d\n", request->group.id);
        callback = group_ptr->init_cb;
        break;
    case idigi_remote_config_group_process:
        APP_DEBUG("app_process_group_process group_id = %d\n", request->group.id);
        callback = (request->action == idigi_remote_action_set) ? group_ptr->set_cb : group_ptr->get_cb;
        break;
    case idigi_remote_config_group_end:
        APP_DEBUG("app_process_group_end group_id = %d\n", request->group.id);
        callback = group_ptr->end_cb;
        break;
    default:
        ASSERT(0);
        goto done;
    }

    if (callback)
    {
        status = callback(request, response);
    }

done:
    return status;
}


static idigi_callback_status_t app_process_session_cancel(void const * const context)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = (remote_group_session_t *)context;

    APP_DEBUG("app_process_session_cancel\n");
    if (session_ptr != NULL)
    {
        remote_group_table_t * const group_ptr = session_ptr->group_context;
        remote_group_cancel_cb_t callback = group_ptr->cancel_cb;

        callback(session_ptr);
        free(session_ptr);
    }
    return status;
}

idigi_callback_status_t app_remote_config_handler(idigi_remote_config_request_t const request,
                                                      void const * const request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_remote_config_session_start:
        status = app_process_session_start(response_data);
        break;
    case idigi_remote_config_session_end:
        status = app_process_session_end(response_data);
        break;
    case idigi_remote_config_action_start:
        status = app_process_action_start(request_data, response_data);
        break;
    case idigi_remote_config_action_end:
        status = app_process_action_end(request_data, response_data);
        break;
    case idigi_remote_config_group_start:
    case idigi_remote_config_group_end:
    case idigi_remote_config_group_process:
        status = app_process_group(request, request_data, response_data);
        break;
    case idigi_remote_config_session_cancel:
        status = app_process_session_cancel(request_data);
        break;
    default:
        APP_DEBUG("app_remote_config_handler: unknown request id %d\n", request);
        break;
    }

    return status;
}

