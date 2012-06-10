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
    
    switch (rci->traversal.state)
    {
    UNHANDLED_CASES_ARE_NEEDED

    case rci_traversal_state_none:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        goto done;
        break;
        
    case rci_traversal_state_all_groups_start:
        ASSERT(rci->shared.request.group.id == INVALID_ID);
        ASSERT(rci->shared.request.group.index == INVALID_INDEX);
        ASSERT(rci->shared.request.element.id == INVALID_ID);
        
        rci->shared.request.group.id = 0;
        rci->shared.request.group.index = 1;
        
        rci->output.type = rci_output_type_start_tag;
        set_rci_command_tag(rci->input.command, &rci->output.tag);
        rci->traversal.state = rci_traversal_state_all_groups_group_start;
        break;
                
    case rci_traversal_state_all_groups_group_start:
    case rci_traversal_state_one_group_start:
    case rci_traversal_state_indexed_group_start:
        trigger_rci_callback(rci, idigi_remote_config_group_start);
        
        rci->shared.request.element.id = 0;

        {
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
            
            rci->output.type = rci_output_type_start_tag;
            cstr_to_rci_string(group->name, &rci->output.tag);
            add_numeric_attribute(&rci->output.attribute, RCI_INDEX, rci->shared.request.group.index);
        }
        
        switch (rci->traversal.state)
        {
        UNHANDLED_CASES_ARE_INVALID
        case rci_traversal_state_all_groups_group_start:    rci->traversal.state = rci_traversal_state_all_groups_element_start;    break;
        case rci_traversal_state_one_group_start:           rci->traversal.state = rci_traversal_state_one_group_element_start;     break;
        case rci_traversal_state_indexed_group_start:       rci->traversal.state = rci_traversal_state_indexed_group_element_start; break;
        }
        break;
                    
    case rci_traversal_state_all_groups_element_start:
    case rci_traversal_state_one_group_element_start:
    case rci_traversal_state_indexed_group_element_start:
    case rci_traversal_state_one_element_start:
        {
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
            idigi_group_element_t const * const element = (group->elements.data + rci->shared.request.element.id);
            
            rci->output.type = rci_output_type_start_tag;
            cstr_to_rci_string(element->name, &rci->output.tag);
            
            rci->shared.request.element.type = element->type;
            rci->shared.request.element.value = NULL;
        }
        
        trigger_rci_callback(rci, idigi_remote_config_group_process);
        
        switch (rci->traversal.state)
        {
        UNHANDLED_CASES_ARE_INVALID
        case rci_traversal_state_all_groups_element_start:      rci->traversal.state = rci_traversal_state_all_groups_element_data;     break;
        case rci_traversal_state_one_group_element_start:       rci->traversal.state = rci_traversal_state_one_group_element_data;      break;
        case rci_traversal_state_indexed_group_element_start:   rci->traversal.state = rci_traversal_state_indexed_group_element_data;  break;
        case rci_traversal_state_one_element_start:             rci->traversal.state = rci_traversal_state_one_element_data;            break;
        }
        break;

    case rci_traversal_state_all_groups_element_data:
    case rci_traversal_state_one_group_element_data:
    case rci_traversal_state_indexed_group_element_data:
    case rci_traversal_state_one_element_data:
        switch (rci->traversal.state)
        {
        UNHANDLED_CASES_ARE_INVALID
        case rci_traversal_state_all_groups_element_data:       rci->traversal.state = rci_traversal_state_all_groups_element_end;      break;
        case rci_traversal_state_one_group_element_data:        rci->traversal.state = rci_traversal_state_one_group_element_end;       break;
        case rci_traversal_state_indexed_group_element_data:    rci->traversal.state = rci_traversal_state_indexed_group_element_end;   break;
        case rci_traversal_state_one_element_data:              rci->traversal.state = rci_traversal_state_one_element_end;             break;
        }

        rci_output_content(rci);
        goto done;
        break;

    case rci_traversal_state_all_groups_element_end:
    case rci_traversal_state_one_group_element_end:
    case rci_traversal_state_indexed_group_element_end:
    case rci_traversal_state_one_element_end:
        {
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
            idigi_group_element_t const * const element = (group->elements.data + rci->shared.request.element.id);

            rci->output.type = rci_output_type_end_tag;
            cstr_to_rci_string(element->name, &rci->output.tag);
        }
        
        switch (rci->traversal.state)
        {
        UNHANDLED_CASES_ARE_INVALID
        case rci_traversal_state_all_groups_element_end:
        case rci_traversal_state_one_group_element_end:
        case rci_traversal_state_indexed_group_element_end:
            {
                idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
                unsigned int const next_element = (rci->shared.request.element.id + 1);
                
                if (next_element == group->elements.count)
                {
                    rci->shared.request.element.id = INVALID_ID;
                    
                    switch (rci->traversal.state)
                    {
                    UNHANDLED_CASES_ARE_INVALID
                    case rci_traversal_state_all_groups_element_end:    rci->traversal.state = rci_traversal_state_all_groups_group_end;    break;
                    case rci_traversal_state_one_group_element_end:     rci->traversal.state = rci_traversal_state_one_group_end;           break;
                    case rci_traversal_state_indexed_group_element_end: rci->traversal.state = rci_traversal_state_indexed_group_end;       break;
                    }
                }
                else
                {
                    rci->shared.request.element.id = next_element;
                    
                    switch (rci->traversal.state)
                    {
                    UNHANDLED_CASES_ARE_INVALID
                    case rci_traversal_state_all_groups_element_end:    rci->traversal.state = rci_traversal_state_all_groups_element_start;    break;
                    case rci_traversal_state_one_group_element_end:     rci->traversal.state = rci_traversal_state_one_group_element_start;     break;
                    case rci_traversal_state_indexed_group_element_end: rci->traversal.state = rci_traversal_state_indexed_group_element_start; break;
                    }
                }
            }
            break;                  
        case rci_traversal_state_one_element_end:
            rci->shared.request.element.id = INVALID_ID;
            goto complete;
            break;
        }
        break;

    case rci_traversal_state_all_groups_group_end:
    case rci_traversal_state_one_group_end:
    case rci_traversal_state_indexed_group_end:
        trigger_rci_callback(rci, idigi_remote_config_group_end);

        {
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);

            rci->output.type = rci_output_type_end_tag;
            cstr_to_rci_string(group->name, &rci->output.tag);
        }

        switch (rci->traversal.state)
        {
        UNHANDLED_CASES_ARE_INVALID
        case rci_traversal_state_all_groups_group_end:
        case rci_traversal_state_one_group_end:
            {
                idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
                
                /* group indicies are 1-based */
                if (rci->shared.request.group.index == group->instances)
                {
                    if (rci->traversal.state == rci_traversal_state_one_group_end)
                    {
                        goto group_complete;
                    }

                    ASSERT(rci->traversal.state == rci_traversal_state_all_groups_group_end);
                    {
                        unsigned int const next_group = (rci->shared.request.group.id + 1);
                        
                        if (next_group == table->count)
                        {
                            rci->traversal.state = rci_traversal_state_all_groups_end;
                        }   
                        else
                        {
                            rci->shared.request.group.id = next_group;
                            rci->shared.request.group.index = 1;
                            rci->traversal.state = rci_traversal_state_all_groups_group_start;
                        }
                    }
                }
                else
                {
                    rci->shared.request.group.index++;
                    rci->traversal.state = (rci->traversal.state == rci_traversal_state_all_groups_group_end) ? rci_traversal_state_all_groups_group_start : rci_traversal_state_one_group_start;
                }
            }
            break;
                   
        case rci_traversal_state_indexed_group_end:
            goto group_complete;
            break;
        }
        break;
        
    case rci_traversal_state_all_groups_end:
        rci->output.type = rci_output_type_end_tag;
        set_rci_command_tag(rci->input.command, &rci->output.tag);
        
        rci->input.command = rci_command_header;
        goto group_complete;
        break;
    }

    state_call(rci, rci_parser_state_output);
    goto done;

group_complete:
    rci->shared.request.group.id = INVALID_ID;
    rci->shared.request.group.index = INVALID_INDEX;
    rci->shared.request.element.id = INVALID_ID;

complete:    
    rci->traversal.state = rci_traversal_state_none;
    state_call_return(rci, rci_parser_state_output, rci_parser_state_input);

    ASSERT(rci->shared.request.group.id == INVALID_ID);
    ASSERT(rci->shared.request.group.index == INVALID_INDEX);
    ASSERT(rci->shared.request.element.id == INVALID_ID);
    
done:
    ;
}


