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
#include "idigi_remote.h"
#include "remote_config.h"
#include "remote_config_cb.h"

extern idigi_callback_status_t app_keepalive_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response, void ** group_context);
extern idigi_callback_status_t app_keepalive_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response, void * group_context);
extern idigi_callback_status_t app_keepalive_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response, void * group_context);
extern idigi_callback_status_t app_keepalive_group_end(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response, void * group_context);
extern void app_keepalive_group_cancel(void * group_context);

extern idigi_callback_status_t app_debug_info_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);

remote_group_table_t remote_setting_table[] = {
    {app_keepalive_group_init, app_keepalive_group_set, app_keepalive_group_get, app_keepalive_group_end, app_keepalive_group_cancel},
};

static idigi_callback_status_t app_process_session_start(idigi_remote_group_response_t * const response)
{
    void * ptr;
    remote_config_session_t * session_ptr;

    printf("process_session_start\n");

    if (app_os_malloc(sizeof *session_ptr, &ptr) != 0)
    {
        response->error_id = idigi_global_error_no_memory;
        goto done;
    }

    session_ptr = ptr;
    session_ptr->session_context = NULL;
    session_ptr->group_context = NULL;
    session_ptr->group = NULL;

done:
    response->user_context = ptr;
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_session_end(idigi_remote_group_response_t * const response)
{
    printf("process_session_end\n");
    if (response->user_context != NULL)
    {
        app_os_free(response->user_context);
    }
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_action_start(idigi_remote_group_request_t * const request,
                                                        idigi_remote_group_response_t * const response)
{
    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    printf("process_action_start\n");
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_action_end(idigi_remote_group_request_t * const request,
                                                      idigi_remote_group_response_t * const response)
{
    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    printf("process_action_end\n");
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_group_init(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_table_t * group_ptr = NULL;
    remote_config_session_t * session_ptr = response->user_context;

    ASSERT(session_ptr != NULL);

    printf("process_group_init\n");

    switch (request->group.type)
    {
    case idigi_remote_group_setting:
        if (request->group.id <= sizeof (remote_setting_table))
        {
            group_ptr = &remote_setting_table[request->group.id];
            session_ptr->group = group_ptr;
        }
        else
        {
            ASSERT(0);
        }
        break;
    case idigi_remote_group_state:
        ASSERT(0);
        break;
    }

    if (group_ptr->init_cb)
    {
        status = group_ptr->init_cb(request, response, &session_ptr->group_context);
    }

    return status;
}

static idigi_callback_status_t app_process_group_set(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_config_session_t * session_ptr = response->user_context;
    remote_group_table_t * group_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group != NULL);

    printf("process_group_set\n");

    group_ptr = session_ptr->group;

    if (group_ptr->set_cb)
    {
        status = group_ptr->set_cb(request, response, session_ptr->group_context);
    }
    return status;
}


static idigi_callback_status_t app_process_group_get(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_config_session_t * session_ptr = response->user_context;
    remote_group_table_t * group_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group != NULL);

    printf("process_group_get\n");

    group_ptr = session_ptr->group;

    if (group_ptr->get_cb)
    {
        status = group_ptr->get_cb(request, response, session_ptr->group_context);
    }
    return status;

}

static idigi_callback_status_t app_process_group_end(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_config_session_t * session_ptr = response->user_context;
    remote_group_table_t * group_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group != NULL);

    printf("process_group_end\n");

    group_ptr = session_ptr->group;

    if (group_ptr->end_cb)
    {
        status = group_ptr->end_cb(request, response, session_ptr->group_context);
    }
    return status;
}

static idigi_callback_status_t app_process_session_cancel(void * context)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_config_session_t * session_ptr = context;

    printf("process_session_cancel\n");
    if (session_ptr != NULL && session_ptr->group != NULL)
    {
        remote_group_table_t * const group_ptr = session_ptr->group;

        if (group_ptr->cancel_cb)
        {
            group_ptr->cancel_cb(session_ptr->group_context);
        }

        if (session_ptr->group_context != NULL)
        {
            app_os_free(session_ptr->group_context);
        }
    }

    return status;
}

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
        status = app_process_group_init(request_data, response_data);
        break;
    case idigi_remote_config_group_end:
        status = app_process_group_end(request_data, response_data);
        break;
    case idigi_remote_config_group_process:
    {
        idigi_remote_group_request_t * const remote_request = request_data;

        if (remote_request->action == idigi_remote_action_set)
        {
            status = app_process_group_set(request_data, response_data);
        }
        else
        {
            status = app_process_group_get(request_data, response_data);
        }
        break;
    }
    case idigi_remote_config_session_cancel:
        status = app_process_session_cancel(request_data);
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

