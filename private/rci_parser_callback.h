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

static void trigger_rci_callback(rci_t * const rci, idigi_remote_config_request_t const remote_config_request)
{
    rci->callback.request.remote_config_request = remote_config_request;
    rci->callback.status = idigi_callback_busy;
}

static idigi_bool_t rci_callback(rci_t * const rci)
{
    idigi_bool_t callback_complete;
    idigi_remote_group_response_t * response_data = &rci->shared.response;
    size_t response_length = sizeof *response_data;
    idigi_remote_group_request_t * request_data = NULL;
    size_t request_length = 0;

    response_data->error_id = idigi_success;

    switch (rci->callback.request.remote_config_request)
    {
    UNHANDLED_CASES_ARE_NEEDED;
    case idigi_remote_config_session_start:
    case idigi_remote_config_session_end:
        break;
        
    case idigi_remote_config_action_start:
    case idigi_remote_config_action_end:
    case idigi_remote_config_group_start:
    case idigi_remote_config_group_end:
    case idigi_remote_config_group_process:
        request_data = &rci->shared.request;
        request_length = sizeof *request_data;
        break;

    case idigi_remote_config_session_cancel:
        response_data = NULL;
        response_length = 0;
        break;
    }

    rci->callback.status = idigi_callback(rci->service_data->idigi_ptr->callback, idigi_class_remote_config_service, rci->callback.request, request_data, request_length, response_data, &response_length);

    switch (rci->callback.status)
    {
    case idigi_callback_abort:
        callback_complete = idigi_true;
        rci->status = rci_status_error;
        break;
        
    case idigi_callback_continue:
        callback_complete = idigi_true;
        break;
        
    case idigi_callback_busy:
        callback_complete = idigi_false;
        break;
        
    default:
        callback_complete = idigi_true;
        rci->status = rci_status_internal_error;
        ASSERT(idigi_false);
        break;
    }

    return callback_complete;
}


