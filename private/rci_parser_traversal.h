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

static void rci_traverse_data(rci_t * const rci)
{
    idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
    idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
    idigi_group_element_t const * const group_element = (group->elements.data + rci->shared.request.element.id);
    
    switch (rci->traversal.state)
    {
    case rci_traversal_state_none:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        goto done;
        break;
        
    case rci_traversal_state_all_groups_start:
        ASSERT(rci->shared.request.group.id == INVALID_ID);
        ASSERT(rci->shared.request.group.index == INVALID_INDEX);
        ASSERT(rci->shared.request.element.id == INVALID_ID);
        /* no break; */
                
    case rci_traversal_state_all_groups_group_start:
        rci_callback(rci, idigi_remote_config_group_start);

        rci->output.type = rci_output_type_start_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group->name, &rci->traversal.tag);
        rci->traversal.state = rci_traversal_state_all_groups_element_start;
        break;
                    
    case rci_traversal_state_all_groups_element_start:
        rci->output.type = rci_output_type_start_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group_element->name, &rci->traversal.tag);
        rci->traversal.state = rci_traversal_state_all_groups_element_data;
        break;
                    
    case rci_traversal_state_all_groups_element_data:
        if (!rci_callback(rci, idigi_remote_config_group_process))
            goto done;
            
        rci_output_content(rci);
        rci->traversal.state = rci_traversal_state_all_groups_element_end;
        break;
        
    case rci_traversal_state_all_groups_element_end:
        rci->output.type = rci_output_type_end_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group_element->name, &rci->traversal.tag);
        
        rci->shared.request.element.id++;
        rci->traversal.state = (rci->shared.request.element.id == group->elements.count) ? rci_traversal_state_all_groups_group_end : rci_traversal_state_all_groups_element_start;
        break;

    case rci_traversal_state_all_groups_group_end:
        rci_callback(rci, idigi_remote_config_group_end);

        rci->output.type = rci_output_type_end_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group->name, &rci->traversal.tag);

        rci->shared.request.element.id = INVALID_ID;
        rci->shared.request.group.index++;
        rci->traversal.state = (rci->shared.request.group.index == group->instances) ? rci_traversal_state_all_groups_end : rci_traversal_state_all_groups_element_start;
        break;

    case rci_traversal_state_all_groups_end:
        rci->shared.request.group.id = INVALID_ID;
        rci->shared.request.group.index = INVALID_INDEX;
        rci->shared.request.element.id = INVALID_ID;
        
        rci->traversal.state = rci_traversal_state_none;
        state_call_return(rci, rci_parser_state_output, rci_parser_state_input);
        goto done;
        break;
        
    case rci_traversal_state_one_group_start:
        rci_callback(rci, idigi_remote_config_group_start);

        rci->output.type = rci_output_type_start_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group->name, &rci->traversal.tag);
        rci->traversal.state = rci_traversal_state_one_group_element_start;
        break;
        
    case rci_traversal_state_one_group_element_start:
        rci->output.type = rci_output_type_start_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group_element->name, &rci->traversal.tag);
        rci->traversal.state = rci_traversal_state_one_group_element_data;
        break;
                    
    case rci_traversal_state_one_group_element_data:
        if (!rci_callback(rci, idigi_remote_config_group_process))
            goto done;
            
        rci_output_content(rci);
        rci->traversal.state = rci_traversal_state_one_group_element_end;
        break;
        
    case rci_traversal_state_one_group_element_end:
        rci->output.type = rci_output_type_end_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group_element->name, &rci->traversal.tag);
        
        rci->shared.request.element.id++;
        rci->traversal.state = (rci->shared.request.element.id == group->elements.count) ? rci_traversal_state_one_group_end : rci_traversal_state_one_group_element_start;
        break;
        
    case rci_traversal_state_one_group_end:
        rci_callback(rci, idigi_remote_config_group_end);

        rci->output.type = rci_output_type_end_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group->name, &rci->traversal.tag);

        rci->shared.request.group.id = INVALID_ID;
        rci->shared.request.group.index = INVALID_INDEX;
        rci->shared.request.element.id = INVALID_ID;
        
        rci->traversal.state = rci_traversal_state_none;
        state_call_return(rci, rci_parser_state_output, rci_parser_state_input);
        goto done;
        break;
        
    case rci_traversal_state_one_element_start:
        rci->output.type = rci_output_type_start_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group_element->name, &rci->traversal.tag);
        rci->traversal.state = rci_traversal_state_one_element_data;
        break;
                    
    case rci_traversal_state_one_element_data:
        if (!rci_callback(rci, idigi_remote_config_group_process))
            goto done;
            
        rci_output_content(rci);
        rci->traversal.state = rci_traversal_state_one_element_end;
        break;
        
    case rci_traversal_state_one_element_end:
        rci->output.type = rci_output_type_end_tag;
        rci->output.tag = &rci->traversal.tag;
        cstr_to_rci_string(group_element->name, &rci->traversal.tag);

        rci->shared.request.element.id = INVALID_ID;
        
        rci->traversal.state = rci_traversal_state_none;
        state_call_return(rci, rci_parser_state_output, rci_parser_state_input);
        goto done;
        break;
    }

    state_call(rci, rci_parser_state_output);
    
done:
    ;
}


