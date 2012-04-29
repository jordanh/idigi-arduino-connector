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

static void rci_generate_error(rci_t * const rci)
{
    switch (rci->error.state)
    {
    case rci_error_state_none:
        /* unlikely case - have it be the one to come back around */
        if (rci->input.command == rci_command_unseen)
        {
            rci_prep_reply(rci, &rci->error.tag, &rci->error.attribute);
            rci->error.state = rci_error_state_error_open;
            break;
        }
        else
        {
            rci->output.tag = &rci->error.tag;
            rci->output.attribute = &rci->error.attribute;
            /* no break; */
        }
        
    case rci_error_state_error_open:
        cstr_to_rci_string(RCI_ERROR, &rci->error.tag);
        rci->error.attribute.count = 0;
        rci->output.type = rci_output_type_start_tag;
        
        rci->error.state = rci_error_state_error_content;
        break;
            
    case rci_error_state_error_content:
#if defined RCI_PARSER_USES_DESCRIPTIONS
        output.content.data.counted_string = rci->output.description;
        rci->output.type = rci_output_type_content;
#endif        
        rci->error.state = rci_error_state_error_close;
        break;
        
    case rci_error_state_error_close:
        rci->output.type = rci_output_type_end_tag;
        
        rci->error.state = (rci->input.command == rci_command_unseen) ? rci_error_state_reply_close : rci_error_state_element_close;
        break;
        
    case rci_error_state_element_close:
        if (rci->shared.request.element.id != INVALID_ID)
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
            idigi_group_element_t const * const group_element = (group->elements.data + rci->shared.request.element.id);

            cstr_to_rci_string(group_element->name, &rci->error.tag);
            rci->shared.request.element.id = INVALID_ID;

            rci->error.state = rci_error_state_group_close;
            break;
        }
        else
        {
            /* no break; */
        }
        
    case rci_error_state_group_close:
        if (rci->shared.request.group.id != INVALID_ID)
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);

            rci_callback(rci, idigi_remote_config_group_end);

            cstr_to_rci_string(group->name, &rci->error.tag);
            rci->shared.request.group.id = INVALID_ID;

            rci->error.state = rci_error_state_command_close;
            break;
        }
        else
        {
            /* no break; */
        }
        
    
    case rci_error_state_command_close:
        {
            idigi_bool_t const is_set = (rci->shared.request.action == idigi_remote_action_set);
            idigi_bool_t const is_setting = (rci->shared.request.group.type == idigi_remote_group_setting);
            char const * const tag = is_set ? (is_setting ? RCI_SET_SETTING : RCI_SET_STATE) : (is_setting ? RCI_QUERY_SETTING : RCI_QUERY_STATE);
 
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


