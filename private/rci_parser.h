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
#include "rci_parser_entity.h"
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

    rci->input.destination = rci_buffer_position(&rci->buffer.input);

    rci->shared.request.group.id = INVALID_ID;
    rci->shared.request.group.index = INVALID_INDEX;
    rci->shared.request.element.id = INVALID_ID;
                       
    rci->status = rci_status_busy;

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
            goto done;
            break;
        }
        
        case rci_status_busy:
        {
            /* assert state is valid */
            break;
        }
        
        case rci_status_more_input:
        {
            /* reset input buffer pointers */
            rci->status = rci_status_busy;
            break;
        }
        
        case rci_status_flush_output:
        {
            /* reset output buffer pointers */
            rci->status = rci_status_busy;
            break;
        }
    }
    
done:
    return success;
}

static idigi_bool_t rci_action_session_lost(rci_t * const rci)
{
    /* call cancel */
    /* clean up */
    
    rci->service_data = NULL;
    rci->status = rci_status_complete;

    return idigi_false;
}


static rci_status_t rci_parser(idigi_data_t idigi_ptr, rci_session_t const action, ...)
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
        }
        
        ASSERT(success);
        if (!success) goto done;
    }

    if (rci.callback.status == idigi_callback_busy)
    {
        if (!rci_callback(&rci, rci.callback.config_request))
            goto done;
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
    case rci_status_flush_output:
        break;
        
    case rci_status_complete:
    case rci_status_internal_error:
    case rci_status_error:
        output_debug_info(&rci, RCI_DEBUG_SHOW_ALL);
        break;        
    }
       
    return rci.status;
}

