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
        /* callback */
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
        /* callback */
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
        /* callback */
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


