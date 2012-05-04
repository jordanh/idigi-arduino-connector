/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 *
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 * =======================================================================
 *
 */
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"


idigi_callback_status_t app_keepalive_session_start(idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_session_end(idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_action_start(idigi_remote_group_request_t * const request,
                                                        idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_action_end(idigi_remote_group_request_t * const request,
                                                      idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_group_init(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_group_set(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_group_get(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response);
idigi_callback_status_t app_keepalive_group_end(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response);
void app_keepalive_session_cancel(void * context);

idigi_callback_status_t app_remote_config_handler(idigi_remote_config_request_t const request,
                                                      void * const request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_remote_config_session_start:
        status = app_keepalive_session_start(response_data);
        break;
    case idigi_remote_config_session_end:
        status = app_keepalive_session_end(response_data);
        break;
    case idigi_remote_config_action_start:
        status = app_keepalive_action_start(request_data, response_data);
        break;
    case idigi_remote_config_action_end:
        status = app_keepalive_action_end(request_data, response_data);
        break;
    case idigi_remote_config_group_start:
        status = app_keepalive_group_init(request_data, response_data);
        break;
    case idigi_remote_config_group_end:
        status = app_keepalive_group_end(request_data, response_data);
        break;
    case idigi_remote_config_group_process:
    {
        idigi_remote_group_request_t * const remote_request = request_data;

        if (remote_request->action == idigi_remote_action_set)
        {
            status = app_keepalive_group_set(request_data, response_data);
        }
        else
        {
            status = app_keepalive_group_get(request_data, response_data);
        }
        break;
    }
    case idigi_remote_config_session_cancel:
        app_keepalive_session_cancel(request_data);
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

