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

static void state_call_return(rci_t * const rci, rci_parser_state_t const call_state, rci_parser_state_t const return_state)
{
    rci->parser.state.previous = return_state;
    rci->parser.state.current = call_state;
}

static void state_call(rci_t * const rci, rci_parser_state_t const call_state)
{
    state_call_return(rci, call_state, rci->parser.state.current);
}

static rci_command_t find_rci_command(rci_string_t const * const tag)
{
    static char const * const rci_command[] = {
        RCI_SET_SETTING,
        RCI_SET_STATE,
        RCI_QUERY_SETTING,
        RCI_QUERY_STATE
    };
    size_t i;
    rci_command_t result = rci_command_unknown;
      
    CONFIRM(rci_command_set_setting == 0);
    CONFIRM(rci_command_set_state == 1);
    CONFIRM(rci_command_query_setting == 2);
    CONFIRM(rci_command_query_state == 3);
    
    for (i = 0; i < asizeof(rci_command); i++)
    {
        if (cstr_equals_rcistr(rci_command[i], tag))
        {
            result = (rci_command_t) i;
            break;
        }
    }
    
    return result;
}

static rci_string_t const * find_attribute_value(rci_attribute_list_t const * const attribute, char const * const name)
{
    rci_string_t const * result = NULL;
    size_t i;
        
    for (i = 0; i < attribute->count; i++)
    {
        rci_attribute_t const * const pair = &attribute->pair[i];
        
        if (cstr_equals_rcistr(name, &pair->name))
        {
            result = &pair->value;
            break;
        }
    }
    
    return result;
}

static unsigned int find_group(idigi_group_t const * const group, size_t const count, rci_string_t const * const tag)
{
    int result = INVALID_ID;
    size_t i;
    
    for (i = 0; i < count; i++)
    {
        if (cstr_equals_rcistr(group[i].name, tag))
        {
            result = i;
            break;
        }
    }
    
    return result;
}

static unsigned int find_element(idigi_group_t const * const group, rci_string_t const * const tag)
{
    int result = INVALID_ID;
    size_t i;
    
    for (i = 0; i < group->elements.count; i++)
    {
        if (cstr_equals_rcistr(group->elements.data[i].name, tag))
        {
            result = i;
            break;
        }
    }
    
    return result;
}

static void rci_error(rci_t * const rci, unsigned int const id, char const * const description, char const * const hint)
{
    rci->shared.response.error_id = id;
    rci->shared.response.element_data.error_hint = (char *)hint;
    
    rci->error.description = description;

    state_call(rci, rci_parser_state_error);
}

#if defined RCI_PARSER_USES_DESCRIPTIONS
#define get_rci_global_error(id)   idigi_rci_errors[(id) - idigi_rci_error_OFFSET]
#else
#define get_rci_global_error(id)   ((void) (id), NULL)
#endif

static void rci_global_error(rci_t * const rci, unsigned int const id, char const * const hint)
{
    char const * const description = get_rci_global_error(id);
    
    rci_error(rci, id, description, hint);
}

static void rci_group_error(rci_t * const rci, unsigned int const id, char const * const hint)
{
    if (id < idigi_global_error_COUNT)
    {
        rci_global_error(rci, id, hint);
    }
    else
    {
        char const * const description = NULL;
    
        /* subtract offset to find index */
        rci_error(rci, id, description, hint);
    }
}

static idigi_callback_status_t rci_callback_data(rci_t * const rci, idigi_remote_config_request_t const config_request_id)
{
    /* TODO: This is not the actual routine to call, this is only for local testing -ASK */
    extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id, void * const request_data, size_t const request_length, void * response_data, size_t * const response_length);
    idigi_remote_group_response_t * response_data = &rci->shared.response;
    size_t response_length = sizeof *response_data;
    idigi_remote_group_request_t * request_data;
    size_t request_length;

    switch (config_request_id)
    {
    case idigi_remote_config_session_start:
    case idigi_remote_config_session_end:
        request_data = NULL;
        request_length = 0;
        break;
        
    case idigi_remote_config_action_start:
    case idigi_remote_config_action_end:
    case idigi_remote_config_group_start:
    case idigi_remote_config_group_end:
    case idigi_remote_config_group_process:
        request_data = &rci->shared.request;
        request_length = sizeof *request_data;
        break;
        
    case idigi_remote_config_session_cancel:
        request_data = NULL;
        request_length = 0;
        response_data = NULL;
        response_length = 0;
        break;
    }
        
    {
        idigi_request_t const request_id = {config_request_id};
        idigi_callback_status_t const result = app_idigi_callback(idigi_class_remote_config_service, request_id, request_data, request_length, response_data, &response_length);

        switch (result)
        {
        case idigi_callback_abort:
            rci->status = rci_status_error;
            break;
            
        case idigi_callback_continue:
        case idigi_callback_busy:
            break;
            
        default:
            ASSERT(idigi_false);
            break;
        }

        return result;
    }
}

static idigi_bool_t rci_handle_unary_tag(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;

    switch (rci->input.command)
    {
    case rci_command_unseen:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        goto error;
        break;
        
    case rci_command_header:
        rci->input.command = find_rci_command(&rci->shared.string.tag);
        switch (rci->input.command)
        {
        case rci_command_unknown:
            rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
            goto error;
            break;
            
        case rci_command_set_setting:
        case rci_command_set_state:
            rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
            goto error;
            break;

        case rci_command_query_setting:
        case rci_command_query_state:
            rci->traversal.state = rci_traversal_state_all_groups_start;
            state_call(rci, rci_parser_state_traversal);
            break;
            
        case rci_command_unseen:
        case rci_command_header:
            ASSERT_GOTO(idigi_false, error);
            break;
        }
        break;
        
    case rci_command_set_setting:
    case rci_command_set_state:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        goto error;
        break;
            
    case rci_command_query_setting:
    case rci_command_query_state:
        if (rci->shared.request.group.id == INVALID_ID)
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            size_t const count = table->count;
            
            rci->shared.request.group.id = find_group(table->groups, count, &rci->shared.string.tag);
            if (rci->shared.request.group.id == INVALID_ID)
            {
                rci_global_error(rci, idigi_rci_error_bad_group, RCI_NO_HINT);
                goto error;
            }

            {
                rci_string_t const * const index = find_attribute_value(&rci->input.attribute, RCI_INDEX);
                idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
                
                if (index == NULL)
                {
                    rci->shared.request.group.index = 1;
                }
                else
                {
                    if (!rcistr_to_uint(index, &rci->shared.request.group.index) || (rci->shared.request.group.index > group->instances))
                    {
                        rci_global_error(rci, idigi_rci_error_bad_index, RCI_NO_HINT);
                        goto error;
                    }
                }
                    
                rci->traversal.state = rci_traversal_state_one_group_start;
                state_call(rci, rci_parser_state_traversal);
            }
        }
        else
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);

            ASSERT(rci->shared.request.element.id == INVALID_ID);
            rci->shared.request.element.id = find_element(group, &rci->shared.string.tag);
            if (rci->shared.request.element.id == INVALID_ID)
            {
                rci_global_error(rci, idigi_rci_error_bad_element, RCI_NO_HINT);
                goto error;
            }

            rci->traversal.state = rci_traversal_state_one_element_start;
            state_call(rci, rci_parser_state_traversal);
        }   
        break;
        
    case rci_command_unknown:
        rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
        goto error;
        break;
    }
    goto done;
    
error:
    continue_parsing = idigi_false;
    
done:   
    ASSERT(continue_parsing || (rci->parser.state.current != rci_parser_state_input));
    return continue_parsing;
}

static idigi_bool_t rci_handle_start_tag(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.command)
    {
    case rci_command_unseen:
        if (cstr_equals_rcistr(RCI_REQUEST, &rci->shared.string.tag))
        {
            rci_string_t const * const version = find_attribute_value(&rci->input.attribute, RCI_VERSION);
            
            if ((version == NULL) || (cstr_equals_rcistr(RCI_VERSION_SUPPORTED, version)))
            {
                rci->input.command = rci_command_header;
                
                rci->output.tag = &rci->shared.string.tag;

                rci->input.attribute.count = 1;
                cstr_to_rci_string(RCI_VERSION, &rci->input.attribute.pair[0].name);
                cstr_to_rci_string(RCI_VERSION_SUPPORTED, &rci->input.attribute.pair[0].value);
                rci->output.attribute = &rci->input.attribute;

                rci->output.type = rci_output_type_start_tag;
                state_call(rci, rci_parser_state_output);
            }
            else
            {
                rci_global_error(rci, idigi_rci_error_invalid_version, RCI_NO_HINT);
                goto error;
            }
        }
        else
        {
            rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
            goto error;
        }
        break;
        
    case rci_command_header:
        rci->input.command = find_rci_command(&rci->shared.string.tag);
        if (rci->input.command == rci_command_unknown)
        {
            rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
            goto error;
        }
        else
        {
            switch (rci->input.command)
            {
            case rci_command_set_setting:   rci->shared.request.action = idigi_remote_action_set;   rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_set_state:     rci->shared.request.action = idigi_remote_action_set;   rci->shared.request.group.type = idigi_remote_group_state;      break;
            case rci_command_query_setting: rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_query_state:   rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_state;      break;
            default:                        ASSERT(idigi_false);                                                                                                    break;
            }

            rci->output.tag = &rci->shared.string.tag;
            rci->output.attribute = NULL;

            rci->output.type = rci_output_type_start_tag;
            state_call(rci, rci_parser_state_output);
        }
        break;
        
    case rci_command_set_setting:
    case rci_command_set_state:
    case rci_command_query_setting:
    case rci_command_query_state:
        if (rci->shared.request.group.id == INVALID_ID)
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            size_t const count = table->count;
            
            rci->shared.request.group.id = find_group(table->groups, count, &rci->shared.string.tag);
            if (rci->shared.request.group.id == INVALID_ID)
            {
                rci_global_error(rci, idigi_rci_error_bad_group, RCI_NO_HINT);
                goto error;
            }
            else
            {
                rci_string_t const * const index = find_attribute_value(&rci->input.attribute, RCI_INDEX);
                idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
                
                if (index == NULL)
                {
                    rci->shared.request.group.index = 1;
                }
                else
                {
                    if (!rcistr_to_uint(index, &rci->shared.request.group.index) || (rci->shared.request.group.index > group->instances))
                    {
                        rci_global_error(rci, idigi_rci_error_bad_index, RCI_NO_HINT);
                        goto error;
                    }
                }
                    
                rci->output.tag = &rci->shared.string.tag;
                rci->output.attribute = NULL;

                rci->output.type = rci_output_type_start_tag;
                state_call(rci, rci_parser_state_output);
            }
        }
        else
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);

            ASSERT(rci->shared.request.element.id == INVALID_ID);
            rci->shared.request.element.id = find_element(group, &rci->shared.string.tag);
            if (rci->shared.request.element.id == INVALID_ID)
            {
                rci_global_error(rci, idigi_rci_error_bad_element, RCI_NO_HINT);
                goto error;
            }
            else
            {
                rci->output.tag = &rci->shared.string.tag;
                rci->output.attribute = NULL;

                rci->output.type = rci_output_type_start_tag;
                state_call(rci, rci_parser_state_output);
            }
        }   
        break;

    case rci_command_unknown:
        rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
        goto error;
        break;
    }
    goto done;
    
error:
    continue_parsing = idigi_false;

done:
    ASSERT(continue_parsing || (rci->parser.state.current != rci_parser_state_input));
    return continue_parsing;
}

static idigi_bool_t rci_handle_content(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    /* parse ints and enums (and conditionally floats) */
    /* callback with content */
    /* if callback returns busy, retard the pointer */
    
    (void) rci;

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_handle_end_tag(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    rci->output.tag = &rci->shared.string.tag;

    rci->output.type = rci_output_type_end_tag;
    state_call(rci, rci_parser_state_output);

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_less_than_sign(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_tag_open:
        rci->input.state = rci_input_state_element_tag_name;
        break;
        
    case rci_input_state_content:
        if (rci->shared.request.element.id == INVALID_ID)
        {
            ASSERT(strspn(rci->shared.string.content.data, " \t\n\r\f") == ((size_t) (rci_buffer_position(&rci->buffer.input) - rci->shared.string.content.data)));
        }
        else
        {
            continue_parsing = rci_handle_content(rci);
        }
        rci->input.state = rci_input_state_element_tag_name;
        break;
    
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_greater_than_sign(rci_t * const rci)
{
    idigi_bool_t continue_parsing;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_start_name:
    case rci_input_state_element_param_name:
        rci->input.state = rci_input_state_content;
        continue_parsing = rci_handle_start_tag(rci);
        break;
        
    case rci_input_state_element_end_name:
        rci->input.state = rci_input_state_element_tag_open;
        continue_parsing = rci_handle_end_tag(rci);
        break;
    
    case rci_input_state_element_tag_close:
        rci->input.state = rci_input_state_element_tag_open;
        continue_parsing = rci_handle_unary_tag(rci);
        break;
    
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_equals_sign(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_name:
        rci->input.state = rci_input_state_element_param_quote;
        /* save param pointer */
        break;
        
    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
    
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_slash(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_tag_name:
        rci->input.state = rci_input_state_element_end_name;
        break;
        
    case rci_input_state_element_start_name:
    case rci_input_state_element_param_name:
        rci->input.state = rci_input_state_element_tag_close;
        break;
    
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_quote(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_quote:
        rci->input.state = rci_input_state_element_param_value;
        break;
        
    case rci_input_state_element_param_value:
        /* at end of value */
        rci->input.state = rci_input_state_element_param_name;
        break;

     case rci_input_state_content:
        break;
   
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_whitespace(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_start_name:
        rci->input.state = rci_input_state_element_param_name;
        break;
        
    case rci_input_state_element_param_value_escaping:
	case rci_input_state_content_escaping:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;

    default:
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_ampersand(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value:
        rci->input.state = rci_input_state_element_param_value_escaping;
        rci->input.character = nul;
        ASSERT(rci->input.entity.data == NULL);
        break;

    case rci_input_state_content:
        rci->input.state = rci_input_state_content_escaping;
        rci->input.character = nul;
        ASSERT(rci->input.entity.data == NULL);
        break;
        
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_semicolon(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value_escaping:
    case rci_input_state_content_escaping:
        ASSERT(rci->input.entity.data != NULL);
        rci->input.entity.length = (rci->input.destination - rci->input.entity.data);
        rci->input.character = rci_entity_value(rci->input.entity.data, rci->input.entity.length);
        ASSERT(rci->input.character != nul);
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
        
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_other(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_tag_name:
        rci->input.state = rci_input_state_element_start_name;
        /* no break; */
    case rci_input_state_element_end_name:
        if (rci->shared.string.tag.data == NULL)
            rci->shared.string.tag.data = rci->input.destination;
        break;

    case rci_input_state_element_param_value_escaping:
    case rci_input_state_content_escaping:
        if (rci->input.entity.data == NULL)
            rci->input.entity.data = rci->input.destination;
        rci->input.character = nul;
        break;
               
    case rci_input_state_element_param_quote:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
        
    default:
        break;
    }

    ASSERT(continue_parsing);
    return continue_parsing;
}

static void rci_parse_input(rci_t * const rci)
{
#if 0 /* TODO: Still not sure the best place for this yet -ASK */
    static char internal_buffer_storage[IDIGI_RCI_MAXIMUM_CONTENT_LENGTH + sizeof nul];
#endif

    rci_buffer_t * const input = &rci->buffer.input;

    while ((rci_buffer_remaining(input) != 0) && (rci->parser.state.current == rci_parser_state_input))
    {
        idigi_bool_t continue_parsing;

        output_debug_info(rci);
                
        rci->input.character = rci_buffer_read(input);
        switch (rci->input.character)
        {
        case '<':
            continue_parsing = rci_parse_input_less_than_sign(rci);
            break;
            
        case '>':
            continue_parsing = rci_parse_input_greater_than_sign(rci);
            break;
        
        case '=':
            continue_parsing = rci_parse_input_equals_sign(rci);
            break;
        
        case '/':
            continue_parsing = rci_parse_input_slash(rci);
            break;
        
        case '"':
            continue_parsing = rci_parse_input_quote(rci);
            break;
        
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\f':
            continue_parsing = rci_parse_input_whitespace(rci);
            break;
        
        case '&':
            continue_parsing = rci_parse_input_ampersand(rci);
            break;
        
        case ';':
            continue_parsing = rci_parse_input_semicolon(rci);
            break;
        
        default:
            continue_parsing = rci_parse_input_other(rci);
            break;
        }
        
        /* callback is busy */
        if (!continue_parsing)
            goto done;
        
        if (rci->input.character != nul)
        {
            /* find out if we are compressing entities */
            if (rci->input.destination != rci_buffer_position(&rci->buffer.input))
            {
                *(rci->input.destination) = rci->input.character;
            }
            rci->input.destination++;
        }    
        rci_buffer_advance(input, 1);
    }
    
    if (rci_buffer_remaining(input) == 0)
    {
        /* if preservation is needed, copy data and adjust pointers */
        /* including the position pointer - more tricky stuff */
    }
    
done:
    output_debug_info(rci);
}

