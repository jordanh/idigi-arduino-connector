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

static void state_call_return(rci_t * const rci, rci_parser_state_t const call_state, rci_parser_state_t const return_state)
{
    rci->parser.state.previous = return_state;
    rci->parser.state.current = call_state;
}

static void state_call(rci_t * const rci, rci_parser_state_t const call_state)
{
    state_call_return(rci, call_state, rci->parser.state.current);
}

static void rci_error(rci_t * const rci, unsigned int const id, char const * const description, char const * const hint)
{
    rci->shared.response.error_id = id;
    rci->shared.response.element_data.error_hint = hint;
    
    rci->error.description = description;

    state_call(rci, rci_parser_state_error);
}

#if defined RCI_PARSER_USES_ERROR_DESCRIPTIONS
#define get_rci_global_error(rci, id)   idigi_rci_errors[(id) - idigi_rci_error_OFFSET]
static char const * get_rci_group_error(rci_t * const rci, unsigned int const id)
{
    idigi_group_t const * const group = get_current_group(rci);
    unsigned int const index = (id - idigi_global_error_OFFSET);
    
    ASSERT(id >= idigi_global_error_OFFSET);
    ASSERT(index < group->errors.count);
        
    return group->errors.description[index];
}
#else
#define get_rci_global_error(rci, id)   ((void) (rci), (void) (id), NULL)
#define get_rci_group_error(rci, id)    ((void) (rci), (void) (id), NULL)
#endif

static void rci_global_error(rci_t * const rci, unsigned int const id, char const * const hint)
{
    char const * const description = get_rci_global_error(rci, id);
    
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
        char const * const description = get_rci_group_error(rci, id);
    
        rci_error(rci, id, description, hint);
    }
}

static void rci_output_content(rci_t * const rci)
{
    idigi_remote_group_response_t const * const response = &rci->shared.response;

    if (response->error_id != 0)
    {
        rci_group_error(rci, response->error_id, response->element_data.error_hint);
    }
    else
    {
        switch (rci->shared.request.element.type)
        {
        UNHANDLED_CASES_ARE_NEEDED
#if (defined RCI_PARSER_USES_STRING) || (defined RCI_PARSER_USES_MULTILINE_STRING) || (defined RCI_PARSER_USES_PASSWORD)

#if defined RCI_PARSER_USES_STRING
        case idigi_element_type_string:
#endif

#if defined RCI_PARSER_USES_MULTILINE_STRING
        case idigi_element_type_multiline_string:
#endif

#if defined RCI_PARSER_USES_PASSWORD
        case idigi_element_type_password:
#endif
            str_to_rcistr(rci->shared.value.string_value, &rci->output.content);
            rci->output.type = rci_output_type_content;
            break;
#endif /* (defined RCI_PARSER_USES_STRING) || (defined RCI_PARSER_USES_MULTILINE_STRING) || (defined RCI_PARSER_USES_PASSWORD) */

#if (defined RCI_PARSER_USES_IPV4) || (defined RCI_PARSER_USES_FQDNV4) || (defined RCI_PARSER_USES_FQDNV6) || \
    (defined RCI_PARSER_USES_DATETIME) || \
    (defined RCI_PARSER_USES_ENUM) || (defined RCI_PARSER_USES_ON_OFF) || (defined RCI_PARSER_BOOLEAN) || \
    (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UINT32) || (defined RCI_PARSER_USES_HEX32) || (defined RCI_PARSER_USES_0XHEX) || \
    (defined RCI_PARSER_USES_FLOAT)

#if defined RCI_PARSER_USES_IPV4
        case idigi_element_type_ipv4:
#endif

#if defined RCI_PARSER_USES_FQDNV4
        case idigi_element_type_fqdnv4:
#endif

#if defined RCI_PARSER_USES_FQDNV6
        case idigi_element_type_fqdnv6:
#endif

#if defined RCI_PARSER_USES_DATETIME
        case idigi_element_type_datetime:
#endif

#if defined RCI_PARSER_USES_ENUM
        case idigi_element_type_enum:
#endif

#if defined RCI_PARSER_USES_ON_OFF
        case idigi_element_type_on_off:
#endif

#if defined RCI_PARSER_USES_BOOLEAN
        case idigi_element_type_boolean:
#endif

#if defined RCI_PARSER_USES_INT32
        case idigi_element_type_int32:
#endif

#if defined RCI_PARSER_USES_UINT32
        case idigi_element_type_uint32:
#endif

#if defined RCI_PARSER_USES_HEX32
        case idigi_element_type_hex32:
#endif

#if defined RCI_PARSER_USES_0XHEX
        case idigi_element_type_0xhex:
#endif

#if defined RCI_PARSER_USES_FLOAT
        case idigi_element_type_float:
#endif
            rci->output.type = rci_output_type_content_formatted;
            break;
#endif /* (defined RCI_PARSER_USES_IPV4) || (defined RCI_PARSER_USES_FQDNV4) || (defined RCI_PARSER_USES_FQDNV6) || \
    (defined RCI_PARSER_USES_DATETIME) || \
    (defined RCI_PARSER_USES_ENUM) || (defined RCI_PARSER_USES_ON_OFF) || (defined RCI_PARSER_BOOLEAN) || \
    (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UINT32) || (defined RCI_PARSER_USES_HEX32) || (defined RCI_PARSER_USES_0XHEX) || \
    (defined RCI_PARSER_USES_FLOAT) */
        }

        state_call(rci, rci_parser_state_output);
    }
}

#if (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UNSIGNED_INTEGER) || (defined RCI_PARSER_USES_FLOAT)
static idigi_bool_t rci_scan_formatted(char const * const input, char const * const format, ...)
{
    idigi_bool_t error;
    
    va_list ap;
        
    va_start(ap, format);
    error = (vsscanf(input, format, ap) != 1);
    va_end(ap);

    return error;
}
#endif

#if (defined RCI_PARSER_USES_ENUM) || (defined RCI_PARSER_USES_ON_OFF) || (defined RCI_PARSER_USES_BOOLEAN)
static idigi_bool_t rci_scan_enum(char const * const input, idigi_element_value_enum_t const * const match, unsigned int * const result)
{
    idigi_bool_t error = idigi_true;
    size_t i;
    
    for (i = 0; i < match->count; i++)
    {
        if (cstr_equals_str(match->value[i], input))
        {
            *result = i;
            error = idigi_false;
            break;
        }
    }

    return error;
}
#endif

typedef void (*rci_action_t)(rci_t * const rci);

static void rci_action_unary_group(rci_t * const rci)
{
    rci->traversal.state = rci_traversal_state_one_group_start;
    state_call(rci, rci_parser_state_traversal);
}

static void rci_action_start_group(rci_t * const rci)
{
    rci->output.tag = rci->shared.string.tag;
    rci->output.type = rci_output_type_start_tag;
    
    add_numeric_attribute(&rci->output.attribute, RCI_INDEX, rci->shared.request.group.index);

    state_call(rci, rci_parser_state_output);
}

static void rci_process_group_tag(rci_t * const rci, rci_action_t const rci_action)
{
    ASSERT(!have_group_id(rci));
    assign_group_id(rci, &rci->shared.string.tag);
    if (!have_group_id(rci))
    {
        rci_global_error(rci, idigi_rci_error_bad_group, RCI_NO_HINT);
        goto error;
    }

    {
        rci_string_t const * const index = find_attribute_value(&rci->shared.attribute, RCI_INDEX);
        
        if (index == NULL)
        {
            rci->shared.request.group.index = 1;
        }
        else
        {
            idigi_group_t const * const group = get_current_group(rci);
            
            if (!rcistr_to_uint(index, &rci->shared.request.group.index) || (rci->shared.request.group.index > group->instances))
            {
                rci_global_error(rci, idigi_rci_error_bad_index, RCI_NO_HINT);
                goto error;
            }
        }
    }
        
    trigger_rci_callback(rci, idigi_remote_config_group_start);
   
    rci_action(rci);
    
error:
    return;
}

static void rci_action_unary_element(rci_t * const rci)
{
    rci->traversal.state = rci_traversal_state_one_element_start;
    state_call(rci, rci_parser_state_traversal);
}

static void rci_action_start_element(rci_t * const rci)
{
    rci->output.tag = rci->shared.string.tag;
    rci->output.type = rci_output_type_start_tag;
    state_call(rci, rci_parser_state_output);
    
    switch (rci->input.command)
    {
    UNHANDLED_CASES_ARE_INVALID
    case rci_command_set_setting:
    case rci_command_set_state:
        /* These are handled in the content parser */
        break;

    case rci_command_query_setting:
    case rci_command_query_state:
        trigger_rci_callback(rci, idigi_remote_config_group_process);
        rci->input.send_content = idigi_true;
    }
}

static void rci_process_element_tag(rci_t * const rci, rci_action_t const rci_action)
{
    ASSERT(!have_element_id(rci));
    assign_element_id(rci, &rci->shared.string.tag);
    if (!have_element_id(rci))
    {
        rci_global_error(rci, idigi_rci_error_bad_element, RCI_NO_HINT);
        goto error;
    }

    rci_action(rci);

error:
    return;    
}

static void rci_handle_unary_tag(rci_t * const rci)
{
    switch (rci->input.command)
    {
    UNHANDLED_CASES_ARE_NEEDED
    case rci_command_unseen:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        break;
        
    case rci_command_header:
        rci->input.command = find_rci_command(&rci->shared.string.tag);
        switch (rci->input.command)
        {
        UNHANDLED_CASES_ARE_NEEDED
        case rci_command_unknown:
            rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
            break;
            
        case rci_command_unseen:
        case rci_command_header:
        case rci_command_set_setting:
        case rci_command_set_state:
            rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
            break;

        case rci_command_query_setting:
        case rci_command_query_state:
            switch (rci->input.command)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_command_set_setting:   rci->shared.request.action = idigi_remote_action_set;   rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_set_state:     rci->shared.request.action = idigi_remote_action_set;   rci->shared.request.group.type = idigi_remote_group_state;      break;
            case rci_command_query_setting: rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_query_state:   rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_state;      break;
            }

            rci->traversal.state = rci_traversal_state_all_groups_start;
            state_call(rci, rci_parser_state_traversal);
            break;
        }
        break;
        
    case rci_command_set_setting:
    case rci_command_set_state:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        break;
            
    case rci_command_query_setting:
    case rci_command_query_state:
        if (have_group_id(rci))
            rci_process_element_tag(rci, rci_action_unary_element);
        else
            rci_process_group_tag(rci, rci_action_unary_group);
        break;
        
    case rci_command_unknown:
        rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
        break;
    }
}

static void rci_handle_start_tag(rci_t * const rci)
{
    switch (rci->input.command)
    {
    UNHANDLED_CASES_ARE_NEEDED
    case rci_command_unseen:
        if (cstr_equals_rcistr(RCI_REQUEST, &rci->shared.string.tag))
        {
            rci_string_t const * const version = find_attribute_value(&rci->shared.attribute, RCI_VERSION);
            
            if ((version == NULL) || (cstr_equals_rcistr(RCI_VERSION_SUPPORTED, version)))
            {
                trigger_rci_callback(rci, idigi_remote_config_session_start);
                
                rci->input.command = rci_command_header;
                
                prep_rci_reply_data(rci);
                state_call(rci, rci_parser_state_output);
            }
            else
            {
                rci_global_error(rci, idigi_rci_error_invalid_version, RCI_NO_HINT);
            }
        }
        else
        {
            rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
        }
        break;
        
    case rci_command_header:
        rci->input.command = find_rci_command(&rci->shared.string.tag);
        if (rci->input.command == rci_command_unknown)
        {
            rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
        }
        else
        {
            switch (rci->input.command)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_command_set_setting:   rci->shared.request.action = idigi_remote_action_set;   rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_set_state:     rci->shared.request.action = idigi_remote_action_set;   rci->shared.request.group.type = idigi_remote_group_state;      break;
            case rci_command_query_setting: rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_query_state:   rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_state;      break;
            }

            trigger_rci_callback(rci, idigi_remote_config_action_start);
            
            rci->output.tag = rci->shared.string.tag;
            rci->output.type = rci_output_type_start_tag;
            state_call(rci, rci_parser_state_output);
        }
        break;
        
    case rci_command_set_setting:
    case rci_command_set_state:
    case rci_command_query_setting:
    case rci_command_query_state:
        if (have_group_id(rci))
            rci_process_element_tag(rci, rci_action_start_element);
        else
            rci_process_group_tag(rci, rci_action_start_group);
        break;

    case rci_command_unknown:
        rci_global_error(rci, idigi_rci_error_bad_command, RCI_NO_HINT);
        break;
    }
}

static void rci_handle_content(rci_t * const rci)
{
    idigi_group_element_t const * const element = get_current_element(rci);
    idigi_element_value_type_t const type = element->type; 
    char const * const string_value = rci->shared.string.content.data;
    size_t const string_length = rci->shared.string.content.length;
    idigi_bool_t error = idigi_true;
    
    /* NUL-terminate the content as we know it is always followed by '<', that's how we got here. */
    {
        char * const writeable_string = (char *) string_value;
        
        writeable_string[string_length] = nul;
    }
    
    switch (type)
    {
    UNHANDLED_CASES_ARE_NEEDED

#if defined RCI_PARSER_USES_STRINGS

#if defined RCI_PARSER_USES_STRING
    case idigi_element_type_string:
#endif

#if defined RCI_PARSER_USES_MULTILINE_STRING
    case idigi_element_type_multiline_string:
#endif

#if defined RCI_PARSER_USES_PASSWORD
    case idigi_element_type_password:
#endif

#if defined RCI_PARSER_USES_IPV4
    case idigi_element_type_ipv4:
#endif

#if defined RCI_PARSER_USES_FQDNV4
    case idigi_element_type_fqdnv4:
#endif

#if defined RCI_PARSER_USES_FQDNV6
    case idigi_element_type_fqdnv6:
#endif

#if defined RCI_PARSER_USES_DATETIME
    case idigi_element_type_datetime:
#endif
        rci->shared.value.string_value = string_value;
        if (element->value_limit == NULL)
        {
            error = idigi_false;
        }
        else
        {
            idigi_element_value_string_t const * const limit = &element->value_limit->string_value;
            error = (string_length < limit->min_length_in_bytes) || (string_length > limit->max_length_in_bytes);
        }
        break;
#endif /* RCI_PARSER_USES_STRINGS */

#if (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UNSIGNED_INTEGER) || (defined RCI_PARSER_USES_FLOAT)

#if defined RCI_PARSER_USES_INT32
    case idigi_element_type_int32:
#endif

#if defined RCI_PARSER_USES_UINT32
    case idigi_element_type_uint32:
#endif

#if defined RCI_PARSER_USES_HEX32
    case idigi_element_type_hex32:
#endif

#if defined RCI_PARSER_USES_0XHEX
    case idigi_element_type_0xhex:
#endif

#if defined RCI_PARSER_USES_FLOAT
    case idigi_element_type_float:
#endif
        switch (type)
        {
        UNHANDLED_CASES_ARE_INVALID

#if defined RCI_PARSER_USES_INT32
        case idigi_element_type_int32:
            error = rci_scan_formatted(string_value, "%ld", &rci->shared.value.signed_integer_value);
            break;
#endif
            
#if defined RCI_PARSER_USES_UINT32
        case idigi_element_type_uint32:
            error = rci_scan_formatted(string_value, "%lu", &rci->shared.value.unsigned_integer_value);
            break;
#endif

#if defined RCI_PARSER_USES_HEX32
        case idigi_element_type_hex32:
            error = rci_scan_formatted(string_value, "%lx", &rci->shared.value.unsigned_integer_value);
            break;
#endif
            
#if defined RCI_PARSER_USES_0XHEX
        case idigi_element_type_0xhex:
            error = rci_scan_formatted(string_value, "0x%lx", &rci->shared.value.unsigned_integer_value);
            break;
#endif
            
#if defined RCI_PARSER_USES_FLOAT
        case idigi_element_type_float:
            error = rci_scan_formatted(string_value, "%f", &rci->shared.value.float_value);
            break;
#endif
        }

        
        if (!error && (element->value_limit != NULL))
        {
            switch (type)
            {
            UNHANDLED_CASES_ARE_INVALID
#if defined RCI_PARSER_USES_INT32
            case idigi_element_type_int32:
                {
                    idigi_element_value_signed_integer_t const * const limit = &element->value_limit->signed_integer_value;
                    int32_t const value = rci->shared.value.signed_integer_value;
                    
                    error = (value < limit->min_value) || (value > limit->max_value);
                }
                break;
#endif

#if defined RCI_PARSER_USES_UNSIGNED_INTEGER
#if defined RCI_PARSER_USES_UINT32
            case idigi_element_type_uint32:
#endif

#if defined RCI_PARSER_USES_HEX32
            case idigi_element_type_hex32:
#endif

#if defined RCI_PARSER_USES_0XHEX
            case idigi_element_type_0xhex:
#endif
                {
                    idigi_element_value_unsigned_integer_t const * const limit = &element->value_limit->unsigned_integer_value;
                    uint32_t const value = rci->shared.value.unsigned_integer_value;
                    
                    error = (value < limit->min_value) || (value > limit->max_value);
                }    
                break;
#endif /* RCI_PARSER_USES_UNSIGNED_INTEGER */

#if defined RCI_PARSER_USES_FLOAT
            case idigi_element_type_float:
                {
                    idigi_element_value_float_t const * const limit = &element->value_limit->float_value;
                    double const value = rci->shared.value.float_value;
                    
                    error = (value < limit->min_value) || (value > limit->max_value);
                }
                break;
#endif
            }
        }
        break;
#endif /* (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UNSIGNED_INTEGER) || (defined RCI_PARSER_USES_FLOAT) */
        
#if defined RCI_PARSER_USES_ENUM
    case idigi_element_type_enum:
        error = rci_scan_enum(string_value, &element->value_limit->enum_value, &rci->shared.value.enum_value);
        break;
#endif

#if defined RCI_PARSER_USES_ON_OFF
    case idigi_element_type_on_off:
        error = rci_scan_enum(string_value, &on_off_enum, &rci->shared.value.on_off_value);
        break;
#endif

#if defined RCI_PARSER_USES_BOOLEAN
    case idigi_element_type_boolean:
        error = rci_scan_enum(string_value, &boolean_enum, &rci->shared.value.boolean_value);
        break;
#endif
    }
    
    if (error)
    {
        rci_global_error(rci, idigi_rci_error_bad_value, RCI_NO_HINT);
    }
    else
    {
        rci->shared.request.element.type = element->type;
        rci->shared.request.element.value = &rci->shared.value;

        trigger_rci_callback(rci, idigi_remote_config_group_process);
    }
}

static void rci_handle_end_tag(rci_t * const rci)
{
    rci->output.tag = rci->shared.string.tag;
    if (have_element_id(rci))
    {
        set_element_id(rci, INVALID_ID);
    }
    else
    {
        idigi_remote_config_request_t config_request_id;
        
        if (have_group_id(rci))
        {
            config_request_id = idigi_remote_config_group_end;
        }
        else if (rci->input.command != rci_command_header)
        {
            config_request_id = idigi_remote_config_action_end;
            rci->input.command = rci_command_header;
        }
        else
        {
            cstr_to_rci_string(RCI_REPLY, &rci->output.tag);
            config_request_id = idigi_remote_config_session_end;
            rci->input.command = rci_command_unseen;
        }

        trigger_rci_callback(rci, config_request_id);
    }
                    
    rci->output.type = rci_output_type_end_tag;
    state_call(rci, rci_parser_state_output);
}

static void rci_parse_input_less_than_sign(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_tag_open:
        rci->input.state = rci_input_state_element_tag_name;
        break;
        
    case rci_input_state_content_first:
    case rci_input_state_content:
        if (have_element_id(rci))
        {
            set_rcistr_length(rci, &rci->shared.string.content);
            rci_handle_content(rci);
        }
        rci->input.state = rci_input_state_element_tag_name;
        break;
        
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }
}

static void rci_parse_input_greater_than_sign(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_start_name:
        set_rcistr_length(rci, &rci->shared.string.tag);
        /* no break; */
        
    case rci_input_state_element_param_name:
        if (cstr_equals_rcistr(RCI_COMMENT, &rci->shared.string.tag))
        {
            rci->input.state = rci_input_state_comment;
        }
        else
        {    
            rci_handle_start_tag(rci);
            rci->input.state = rci_input_state_content_first;
        }
        break;
        
    case rci_input_state_element_end_name:
        set_rcistr_length(rci, &rci->shared.string.tag);
        rci_handle_end_tag(rci);
        rci->input.state = rci_input_state_element_tag_open;
        break;
    
    case rci_input_state_element_tag_close:
        rci_handle_unary_tag(rci);
        rci->input.state = rci_input_state_element_tag_open;
        break;

    case rci_input_state_comment:
        if (rci->input.hyphens == 2)
        {
            rci->input.state = rci_input_state_content_first;
        }
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }

    clear_rcistr(&rci->shared.string.tag);
    clear_attributes(&rci->shared.attribute);
}

static void rci_parse_input_equals_sign(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_param_equals:
        set_rcistr_length(rci, &rci->shared.attribute.pair[rci->shared.attribute.count].name);
        rci->input.state = rci_input_state_element_param_quote;
        break;
        
    case rci_input_state_element_param_value_first:
        rci->shared.attribute.pair[rci->shared.attribute.count].name.data = rci->input.destination;
        rci->input.state = rci_input_state_element_param_value;
        break;
        
    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
    
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }
}

static void rci_parse_input_slash(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_tag_name:
        rci->input.state = rci_input_state_element_end_name_first;
        break;
        
    case rci_input_state_element_start_name:
        set_rcistr_length(rci, &rci->shared.string.tag);
        /* no break; */
        
    case rci_input_state_element_param_name:
        rci->input.state = rci_input_state_element_tag_close;
        break;
    
    case rci_input_state_element_param_value_first:
        rci->shared.attribute.pair[rci->shared.attribute.count].value.data = rci->input.destination;
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
    
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }
}

static void rci_parse_input_quote(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_param_quote:
        rci->input.state = rci_input_state_element_param_value_first;
        break;
        
    case rci_input_state_element_param_value:
        set_rcistr_length(rci, &rci->shared.attribute.pair[rci->shared.attribute.count].value);
        rci->shared.attribute.count++;
        rci->input.state = rci_input_state_element_param_name;
        break;
        
    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        rci->input.state = rci_input_state_content;
        break;

     case rci_input_state_content:
        break;
   
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }
}

static void rci_parse_input_whitespace(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_start_name:
        set_rcistr_length(rci, &rci->shared.string.tag);
        rci->input.state = rci_input_state_element_param_name;
        break;

    case rci_input_state_element_param_name:
        set_rcistr_length(rci, &rci->shared.attribute.pair[rci->shared.attribute.count].name);
        rci->input.state = rci_input_state_element_param_equals;
        break;
        
    case rci_input_state_element_param_value_first:
        rci->shared.attribute.pair[rci->shared.attribute.count].value.data = rci->input.destination;
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
    
    case rci_input_state_element_param_value_escaping:
	case rci_input_state_content_escaping:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;

    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        break;
    }
}

static void rci_parse_input_ampersand(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value_first:
        rci->shared.attribute.pair[rci->shared.attribute.count].value.data = rci->input.destination;
        /* no break; */
        
    case rci_input_state_element_param_value:
        rci->input.state = rci_input_state_element_param_value_escaping;
        rci->input.character = nul;
        ASSERT(rci->input.entity.data == NULL);
        break;

    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        /* no break; */
        
    case rci_input_state_content:
        rci->input.state = rci_input_state_content_escaping;
        rci->input.character = nul;
        ASSERT(rci->input.entity.data == NULL);
        break;
        
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }
}

static void rci_parse_input_semicolon(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value_escaping:
    case rci_input_state_content_escaping:
        ASSERT(rci->input.entity.data != NULL);
        rci->input.entity.length = (rci->input.destination - rci->input.entity.data);
        rci->input.character = rci_entity_value(rci->input.entity.data, rci->input.entity.length);
        ASSERT(rci->input.character != nul);
        break;

    case rci_input_state_element_param_value_first:
        rci->shared.attribute.pair[rci->shared.attribute.count].value.data = rci->input.destination;
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
    
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }
}

static void rci_parse_input_hyphen(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_comment:
        rci->input.hyphens++;
        break;

    default:
        break;
    }
}

static void rci_parse_input_other(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_tag_name:
        rci->shared.string.tag.data = rci->input.destination;
        rci->input.state = rci_input_state_element_start_name;
        break;
        
    case rci_input_state_element_end_name_first:
        rci->shared.string.tag.data = rci->input.destination;
        rci->input.state = rci_input_state_element_end_name;
        break;

    case rci_input_state_element_param_value_escaping:
    case rci_input_state_content_escaping:
        if (rci->input.entity.data == NULL)
            rci->input.entity.data = rci->input.destination;
        rci->input.character = nul;
        break;
               
    case rci_input_state_element_param_value_first:
        rci->shared.attribute.pair[rci->shared.attribute.count].value.data = rci->input.destination;
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_element_param_name:
        rci->shared.attribute.pair[rci->shared.attribute.count].name.data = rci->input.destination;
        rci->input.state = rci_input_state_element_param_equals;
        break;

    case rci_input_state_content_first:
        rci->shared.string.content.data = rci->input.destination;
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_quote:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
        
    case rci_input_state_comment:
        rci->input.hyphens = 0;
        break;

    default:
        break;
    }
}

static void rci_parse_input(rci_t * const rci)
{
    rci_buffer_t * const input = &rci->buffer.input;

    if (rci->callback.request.remote_config_request == idigi_remote_config_group_end)
    {
        set_group_id(rci, INVALID_ID);
        /* Set it to something else, so we don't overwrite it again */
        rci->callback.request.remote_config_request = idigi_remote_config_session_cancel;
    }
    
    if (rci->input.send_content)
    {
        rci_output_content(rci);
        rci->input.send_content = idigi_false;
        goto done;
    }
    
    while ((rci_buffer_remaining(input) != 0) && (rci->parser.state.current == rci_parser_state_input) && (rci->callback.status != idigi_callback_busy))
    {
        output_debug_info(rci, RCI_DEBUG_SHOW_DIFFS);
                
        rci->input.character = rci_buffer_read(input);
        switch (rci->input.character)
        {
        case '<':
            rci_parse_input_less_than_sign(rci);
            break;
        case '>':
            rci_parse_input_greater_than_sign(rci);
            break;
        case '=':
            rci_parse_input_equals_sign(rci);
            break;
        case '/':
            rci_parse_input_slash(rci);
            break;
        case '"':
            rci_parse_input_quote(rci);
            break;
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\f':
            rci_parse_input_whitespace(rci);
            break;
        case '&':
            rci_parse_input_ampersand(rci);
            break;
        case ';':
            rci_parse_input_semicolon(rci);
            break;
        case '-':
            rci_parse_input_hyphen(rci);
            break;
        default:
            rci_parse_input_other(rci);
            break;
        }
        
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
        if (rci->parser.state.current == rci_parser_state_input)
        {
            char const * const base = rcistr_data(&rci->shared.string.generic);

            if (ptr_in_buffer(&rci->buffer.input, base))
            {
                size_t const bytes = (rci->buffer.input.end - base) + 1;
                
                if (bytes >= sizeof rci->input.storage)
                {
                    rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
                    goto done;
                }

                memcpy(rci->input.storage, base, bytes);
                
                adjust_rci_string(rci, base, &rci->shared.string.generic);
                {
                    size_t i;
                    
                    for (i = 0; i < attribute_count(&rci->shared.attribute); i++)
                    {
                        adjust_rci_string(rci, base, attribute_name(&rci->shared.attribute, i));
                        adjust_rci_string(rci, base, attribute_value(&rci->shared.attribute, i));
                    }
                }
                adjust_char_pointer(rci, base, &rci->input.destination);
            }
                
            rci->status = rci_status_more_input;
        }
    }
    
done:
    output_debug_info(rci, RCI_DEBUG_SHOW_DIFFS);
}

