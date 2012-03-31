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

#include "rci_parser_support.h"
#include "rci_parser_entity.h"
#include "rci_parser_input.h"
#include "rci_parser_traversal.h"
#include "rci_parser_output.h"
#include "rci_parser_error.h"

static idigi_bool_t rci_action_session_start(rci_t * const rci, rci_service_data_t * service_data)
{
    assert(rci->service_data == NULL);
    rci->service_data = service_data;
    assert(rci->service_data != NULL);

    rci_set_buffer(&rci->buffer.input, &rci->service_data->input);
    rci_set_buffer(&rci->buffer.output, &rci->service_data->output);

    rci->input.state = rci_input_state_element_tag_open;
    rci->input.position = rci_buffer_position(&rci->buffer.input);
    rci->input.string.tag.data = NULL;
    
    rci->traversal.command = rci_command_unseen;
                   
    rci->status = rci_status_busy;
    
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
        }
        
        assert(success);
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
    return rci.status;
}

