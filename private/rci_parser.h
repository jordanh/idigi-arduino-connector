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

#include "rci_parser_support.h"
#include "rci_parser_debug.h"
#include "rci_parser_buffer.h"
#include "rci_parser_string.h"
#include "rci_parser_command.h"
#include "rci_parser_attribute.h"
#include "rci_parser_group.h"
#include "rci_parser_element.h"
#include "rci_parser_entity.h"
#include "rci_parser_callback.h"
#include "rci_parser_input.h"
#include "rci_parser_traversal.h"
#include "rci_parser_output.h"
#include "rci_parser_error.h"

static idigi_bool_t rci_action_session_start(rci_t * const rci, rci_service_data_t * service_data)
{
    ASSERT(rci->service_data == NULL);
    rci->service_data = service_data;
    ASSERT(rci->service_data != NULL);

    rci_set_buffer(&rci->buffer.input, &rci->service_data->input);
    rci_set_buffer(&rci->buffer.output, &rci->service_data->output);
#if defined RCI_DEBUG
    memset(rci->service_data->output.data, 0, rci->service_data->output.bytes);
#endif

    rci->input.destination = rci_buffer_position(&rci->buffer.input);

    invalidate_group_id(rci);
    invalidate_group_index(rci);
    invalidate_element_id(rci);

    rci->shared.response.element_data.element_value = &rci->shared.value;

    rci->status = rci_status_busy;

    rci->input.state = rci_input_state_element_tag_open;
    cstr_to_rcistr(RCI_VERSION, attribute_name(&rci->input.attribute.match));

    output_debug_info(rci, RCI_DEBUG_SHOW_ALL);

    return idigi_true;
}

static idigi_bool_t rci_action_session_active(rci_t * const rci)
{
    idigi_bool_t success = idigi_true;

    switch (rci->status)
    {
        case rci_status_error:
        case rci_status_complete:
        {
            rci->status = rci_status_internal_error;
            /* no break; */
        }

        case rci_status_internal_error:
        {
            success = idigi_false;
            break;
        }

        case rci_status_busy:
        {
            break;
        }

        case rci_status_more_input:
        {
            rci_set_buffer(&rci->buffer.input, &rci->service_data->input);
            /* TODO: what about destination? */
            rci->status = rci_status_busy;
            break;
        }

        case rci_status_flush_output:
        {
            rci_set_buffer(&rci->buffer.output, &rci->service_data->output);
#if defined RCI_DEBUG
            memset(rci->service_data->output.data, 0, rci->service_data->output.bytes);
#endif
            rci->status = rci_status_busy;
            break;
        }
    }

    return success;
}

static idigi_bool_t rci_action_session_lost(rci_t * const rci)
{
    trigger_rci_callback(rci, idigi_remote_config_session_cancel);
    {
        idigi_bool_t const success = rci_callback(rci);
        ASSERT(success); UNUSED_VARIABLE(success);
    }

    rci->service_data = NULL;
    rci->status = rci_status_complete;

    return idigi_false;
}


static rci_status_t rci_parser(rci_session_t const action, ...)
{
    static rci_t rci;

    {
        idigi_bool_t success;
        va_list ap;

        switch (action)
        {
        case rci_session_start:
            va_start(ap, action);
            success = rci_action_session_start(&rci, va_arg(ap, rci_service_data_t *));
            va_end(ap);
            break;

        case rci_session_active:
            success = rci_action_session_active(&rci);
            break;

        case rci_session_lost:
            success = rci_action_session_lost(&rci);
            break;
#if !defined RCI_DEBUG
        default:
            success = idigi_false;
            break;
#endif
        }

        if (!success) goto done;
    }

    switch (rci.parser.state.current)
    {
    case rci_parser_state_input:
        rci_parse_input(&rci);
        break;

    case rci_parser_state_traversal:
        rci_traverse_data(&rci);
        break;

    case rci_parser_state_output:
        rci_generate_output(&rci);
        break;

    case rci_parser_state_error:
        rci_generate_error(&rci);
        break;
    }

done:

    switch (rci.status)
    {
    case rci_status_busy:
    case rci_status_more_input:
        break;
    case rci_status_flush_output:
        rci.service_data->output.bytes = rci_buffer_used(&rci.buffer.output);
        output_debug_info(&rci, RCI_DEBUG_SHOW_ALL);
        break;
    case rci_status_complete:
        rci.service_data->output.bytes = rci_buffer_used(&rci.buffer.output);
        /* no break; */
    case rci_status_internal_error:
    case rci_status_error:
        output_debug_info(&rci, RCI_DEBUG_SHOW_ALL);
        rci.service_data = NULL;
        break;
    }

    return rci.status;
}

