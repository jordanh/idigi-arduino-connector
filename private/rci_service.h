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

#include "rci_parser.h"

static void set_rci_service_error(msg_service_request_t * const service_request, idigi_msg_error_t const error_code)
{
    service_request->error_value = error_code;
    service_request->service_type = msg_service_type_error;
}

static idigi_callback_status_t rci_service_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_error_t error_status = idigi_msg_error_none;
    msg_session_t * session;
    rci_service_data_t * service_data;

    ASSERT_GOTO(idigi_ptr != NULL, done);
    ASSERT_GOTO(service_request != NULL, done);

    session = service_request->session;
    ASSERT_GOTO(session != NULL, done);

    service_data = session->service_context;

    /* idigi_debug_printf("rci_service: service type: %d\n", service_request->service_type); */

    switch (service_request->service_type)
    {
    case msg_service_type_need_data:
    case msg_service_type_have_data:
    {
        rci_status_t rci_status;
        rci_session_t parser_action = rci_session_active;
        void * parser_data = NULL;

        ASSERT_GOTO(service_request->have_data != NULL, done);
        ASSERT_GOTO(service_request->need_data != NULL, done);

        if (session->service_context == NULL)
        {
            /* 1st time here so let's allocate service context memory for rci parser */
            void * ptr;

            status = malloc_data(idigi_ptr, sizeof *service_data, &ptr);
            if (status != idigi_callback_continue)
            {
                if (status != idigi_callback_busy)
                {
                    error_status = idigi_msg_error_cancel;
                }
                goto done;
            }

            service_data = ptr;
            session->service_context = service_data;
            
            MsgSetStart(service_request->need_data->flags);
            
            idigi_debug_printf("rci_service.request: %d %.*s\n", service_request->have_data->length_in_bytes, service_request->have_data->length_in_bytes, service_request->have_data->data_ptr);

            parser_action = rci_session_start;
            parser_data = service_data;
        }

        ASSERT(service_data != NULL);

        service_data->idigi_ptr = idigi_ptr;

        service_data->input.data = service_request->have_data->data_ptr;
        service_data->input.bytes = service_request->have_data->length_in_bytes;

        service_data->output.data = service_request->need_data->data_ptr;
        service_data->output.bytes = service_request->need_data->length_in_bytes;

        rci_status = rci_parser(parser_action, parser_data);

        switch (rci_status)
        {
        case rci_status_complete:
            MsgSetLastData(service_request->need_data->flags);
            MsgSetSendNow(service_request->need_data->flags);
            idigi_debug_printf("rci_service.response: %d %.*s\n", service_data->output.bytes, service_data->output.bytes, service_data->output.data);
            service_request->need_data->length_in_bytes = service_data->output.bytes;
            break;

        case rci_status_busy:
            status = idigi_callback_busy;
            break;

        case rci_status_more_input:
            break;

        case rci_status_flush_output:
            MsgSetSendNow(service_request->need_data->flags);
            idigi_debug_printf("rci_service.response: %d %.*s\n", service_data->output.bytes, service_data->output.bytes, service_data->output.data);
            service_request->need_data->length_in_bytes = service_data->output.bytes;
            status = idigi_callback_busy;
            break;

        case rci_status_internal_error:
            /* no break; */
        case rci_status_error:
            error_status = idigi_msg_error_cancel;
            break;
        }
        break;
    }
    case msg_service_type_error:
         rci_parser(rci_session_lost);
        break;

    case msg_service_type_free:
        free_data(idigi_ptr, session->service_context);
        break;

    default:
        status = idigi_callback_unrecognized;
        ASSERT(idigi_false);
        break;
    }

done:
    if (error_status != idigi_msg_error_none)
    {
        set_rci_service_error(service_request, error_status);
    }

    return status;
}

static idigi_callback_status_t idigi_facility_rci_service_cleanup(idigi_data_t * const idigi_ptr)
{
    return msg_cleanup_all_sessions(idigi_ptr, msg_service_id_rci);
}

static idigi_callback_status_t idigi_facility_rci_service_delete(idigi_data_t * const data_ptr)
{
    return msg_delete_facility(data_ptr, msg_service_id_rci);
}

static idigi_callback_status_t idigi_facility_rci_service_init(idigi_data_t * const data_ptr, unsigned int const facility_index)
{
    return msg_init_facility(data_ptr, facility_index, msg_service_id_rci, rci_service_callback);
}
