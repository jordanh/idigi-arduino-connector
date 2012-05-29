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

static void rci_generate_error(rci_t * const rci)
{
    switch (rci->error.state)
    {
    case rci_error_state_none:
        /* unlikely case - have it be the one to come back around */
        if (rci->input.command == rci_command_unseen)
        {
            rci_prep_reply(rci, &rci->error.tag, &rci->shared.attribute);
            rci->error.state = rci_error_state_error_open;
            break;
        }
        else
        {
            rci->output.tag = &rci->error.tag;
            rci->output.attribute = &rci->shared.attribute;
            /* no break; */
        }
        
    case rci_error_state_error_open:
        cstr_to_rci_string(RCI_ERROR, &rci->error.tag);
        rci->output.attribute->count = 1;
        cstr_to_rci_string(RCI_INDEX, &rci->output.attribute->pair[0].name);
        rci->output.attribute->pair[0].value.data = rci->input.storage;
        rci->output.attribute->pair[0].value.length = sprintf(rci->input.storage, "%d", rci->shared.response.error_id);
        
        rci->output.type = rci_output_type_start_tag;
        
#if defined RCI_PARSER_USES_DESCRIPTIONS
        rci->error.state = rci_error_state_error_content;
#else
        rci->error.state = rci_error_state_error_close;
#endif        
        break;
            
    case rci_error_state_error_content:
#if defined RCI_PARSER_USES_DESCRIPTIONS
        output.content.data.counted_string = rci->output.description;
        rci->output.type = rci_output_type_content;
        rci->error.state = rci_error_state_error_close;
#endif
        break;
        
    case rci_error_state_error_close:
        cstr_to_rci_string(RCI_ERROR, &rci->error.tag);
        rci->output.attribute = NULL;
        rci->output.type = rci_output_type_end_tag;
        
        switch (rci->input.command)
        {
        case rci_command_unseen:
        case rci_command_unknown:
            rci->error.state = rci_error_state_reply_close;
            break;
        default:
            if (rci->shared.request.element.id != INVALID_ID)
                rci->error.state = rci_error_state_element_close;
            else if (rci->shared.request.group.id != INVALID_ID)
                rci->error.state = rci_error_state_group_close;
            else
                rci->error.state = rci_error_state_command_close;
        }    
        break;
        
    case rci_error_state_element_close:
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
            idigi_group_element_t const * const group_element = (group->elements.data + rci->shared.request.element.id);

            cstr_to_rci_string(group_element->name, &rci->error.tag);
            rci->shared.request.element.id = INVALID_ID;

            rci->error.state = rci_error_state_group_close;
        }
        break;
        
    case rci_error_state_group_close:
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);

            rci_callback(rci, idigi_remote_config_group_end);

            cstr_to_rci_string(group->name, &rci->error.tag);
            rci->shared.request.group.id = INVALID_ID;

            rci->error.state = rci_error_state_command_close;
        }
        break;

    case rci_error_state_command_close:
        {
            char const * tag = NULL;

            switch (rci->input.command)
            {
            case rci_command_set_setting:   tag = RCI_SET_SETTING;      break;
            case rci_command_set_state:     tag = RCI_SET_STATE;        break;
            case rci_command_query_setting: tag = RCI_QUERY_SETTING;    break;
            case rci_command_query_state:   tag = RCI_QUERY_STATE;      break;
            default:                        ASSERT(idigi_false);        break;
            }
             
            rci_callback(rci, idigi_remote_config_action_end);

            cstr_to_rci_string(tag, &rci->error.tag);
            rci->error.state = rci_error_state_reply_close;
        }
        break;

    case rci_error_state_reply_close:
        rci_callback(rci, idigi_remote_config_session_end);

        cstr_to_rci_string(RCI_REQUEST, &rci->error.tag);
        rci->error.state = rci_error_state_complete;
        break;

    case rci_error_state_complete:
        rci->status = rci_status_complete;
        goto done;
        break;
    }

    state_call(rci, rci_parser_state_output);
    
done:
    ;
}


