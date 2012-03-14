/*
 *  Copyright (c) 2011 Digi International Inc., All Rights Reserved
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
    rci_parser_data_t * parser_data;
    msg_session_t * session;

    ASSERT_GOTO(idigi_ptr != NULL, done);
    ASSERT_GOTO(service_request != NULL, done);
    ASSERT_GOTO(service_request->session != NULL, done);

    session = service_request->session;

    if (session->service_context == NULL)
    {
        /* 1st time here so let's allocate service context memory for rci parser */
        void * ptr;

        status = malloc_data(idigi_ptr, sizeof *parser_data, &ptr);
        if (status != idigi_callback_continue)
        {
            if (status != idigi_callback_busy)
            {
                error_status = idigi_msg_error_cancel;
            }
            goto done;
        }

        session->service_context = ptr;
        MsgSetStart(service_request->need_data->flags);
    }
    parser_data = session->service_context;

    ASSERT(parser_data != NULL);
    ASSERT(service_request->have_data != NULL);
    ASSERT(service_request->need_data != NULL);

    switch (service_request->service_type)
    {
    case msg_service_type_need_data:
    case msg_service_type_have_data:
    {
        rci_parser_status_t ccode;
        parser_data->input_buf = service_request->have_data->data_ptr;
        parser_data->input_bytes = service_request->have_data->length_in_bytes;

        parser_data->output_buf = service_request->need_data->data_ptr;
        parser_data->output_bytes = service_request->need_data->length_in_bytes;

        ccode = rci_parser(parser_data);

        switch (ccode)
        {
        case rci_parser_complete:
            MsgSetLastData(service_request->need_data->flags);
            MsgSetSendNow(service_request->need_data->flags);
            break;

        case rci_parser_busy:
            status = idigi_callback_busy;
            break;

        case rci_parser_more_input:
            break;

        case rci_parser_flush_output:
            MsgSetSendNow(service_request->need_data->flags);
            status = idigi_callback_busy;
            break;

        case rci_parser_error:
            error_status = idigi_msg_error_cancel;
            break;
        }
        break;
    }
    case msg_service_type_error:
        status = rci_parser(NULL);
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
