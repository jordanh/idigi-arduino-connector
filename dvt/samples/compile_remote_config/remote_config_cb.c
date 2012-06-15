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
#include "idigi_api.h"
#include "platform.h"
#include "idigi_debug.h"
#include "remote_config_cb.h"

#if !defined IDIGI_RCI_SERVICE
#error "Must define IDIGI_RCI_SERVICE in idigi_config.h to run this sample"
#endif

idigi_callback_status_t app_remote_config_handler(idigi_remote_config_request_t const request,
                                                      void const * const request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(response_data);

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_remote_config_session_start:
    case idigi_remote_config_session_end:
    case idigi_remote_config_action_start:
    case idigi_remote_config_action_end:
    case idigi_remote_config_group_start:
    case idigi_remote_config_group_end:
    case idigi_remote_config_group_process:
    case idigi_remote_config_session_cancel:
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

