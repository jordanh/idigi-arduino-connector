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

#if defined RCI_PARSER_USES_ON_OFF
static char const * const on_off_strings[] = {RCI_OFF, RCI_ON};
static idigi_element_value_enum_t on_off_enum = { asizeof(on_off_strings), on_off_strings};
#endif

#if defined RCI_PARSER_USES_BOOLEAN
static char const * const boolean_strings[] = {RCI_FALSE, RCI_TRUE};
static idigi_element_value_enum_t boolean_enum = { asizeof(boolean_strings), boolean_strings};
#endif

static void rci_output_content(rci_t * const rci)
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
    (defined RCI_PARSER_USES_ENUM) || (defined RCI_PARSER_USES_ON_OFF) || (defined RCI_PARSER_USES_BOOLEAN) || \
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
    (defined RCI_PARSER_USES_ENUM) || (defined RCI_PARSER_USES_ON_OFF) || (defined RCI_PARSER_USES_BOOLEAN) || \
    (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UINT32) || (defined RCI_PARSER_USES_HEX32) || (defined RCI_PARSER_USES_0XHEX) || \
    (defined RCI_PARSER_USES_FLOAT) */
    }

    state_call(rci, rci_parser_state_output);
}

#if (defined RCI_PARSER_USES_INT32) || (defined RCI_PARSER_USES_UNSIGNED_INTEGER) || (defined RCI_PARSER_USES_FLOAT)
static idigi_bool_t rci_scan_formatted(char const * const input, char const * const format, ...)
{
    idigi_bool_t error;

    va_list ap;

    va_start(ap, format);
    error = idigi_bool(vsscanf(input, format, ap) != 1);
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


static void rci_handle_content(rci_t * const rci)
{
    idigi_group_element_t const * const element = get_current_element(rci);
    idigi_element_value_type_t const type = element->type;
    char const * const string_value = rcistr_data(&rci->shared.string.content);
    size_t const string_length = rcistr_length(&rci->shared.string.content);
    idigi_bool_t error = idigi_true;

    if (element->access == idigi_element_access_read_only)
    {
        goto done;
    }

    /* NUL-terminate the content as we know it is always followed by '<', that's how we got here. */
    ASSERT(string_value[string_length] == '<');
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
            error = idigi_bool((string_length < limit->min_length_in_bytes) || (string_length > limit->max_length_in_bytes));
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

                    error = idigi_bool((value < limit->min_value) || (value > limit->max_value));
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

                    error = idigi_bool((value < limit->min_value) || (value > limit->max_value));
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

done:

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

typedef void (*rci_action_t)(rci_t * const rci);

static void rci_action_unary_group(rci_t * const rci)
{
    rci->traversal.state = have_group_index(rci) ? rci_traversal_state_indexed_group_start : rci_traversal_state_one_group_start;
    state_call(rci, rci_parser_state_traversal);
}

static void rci_action_start_group(rci_t * const rci)
{
    rci->output.tag = rci->shared.string.tag;
    rci->output.type = rci_output_type_start_tag;

    if (!have_group_index(rci))
    {
        set_group_index(rci, 1);
    }
    set_numeric_attribute(&rci->output.attribute, RCI_INDEX, get_group_index(rci));

    trigger_rci_callback(rci, idigi_remote_config_group_start);

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
        rcistr_t const * const index = attribute_value(&rci->input.attribute.match);

        if (rcistr_valid(index))
        {
            idigi_group_t const * const group = get_current_group(rci);
            unsigned int group_index;

            if (!rcistr_to_uint(index, &group_index) || (group_index > group->instances))
            {
                invalidate_group_id(rci);
                rci_global_error(rci, idigi_rci_error_bad_index, RCI_NO_HINT);
                goto error;
            }

            set_group_index(rci, group_index);
        }
    }

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
    switch (rci->input.command)
    {
    UNHANDLED_CASES_ARE_INVALID
    case rci_command_set_setting:
    case rci_command_set_state:
        rci->output.tag = rci->shared.string.tag;
        rci->output.type = rci_output_type_start_tag;
        state_call(rci, rci_parser_state_output);

        break;

    case rci_command_query_setting:
    case rci_command_query_state:
        {
            idigi_group_element_t const * const element = get_current_element(rci);

            if (element->access != idigi_element_access_write_only)
            {
                rci->output.tag = rci->shared.string.tag;
                rci->output.type = rci_output_type_start_tag;
                state_call(rci, rci_parser_state_output);

                trigger_rci_callback(rci, idigi_remote_config_group_process);
            }
        }
        break;
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
            rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
            break;

        case rci_command_header:
        case rci_command_set_setting:
        case rci_command_set_state:
            rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
            break;

        case rci_command_query_setting:
        case rci_command_query_state:
            switch (rci->input.command)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_command_query_setting: rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_setting;    break;
            case rci_command_query_state:   rci->shared.request.action = idigi_remote_action_query; rci->shared.request.group.type = idigi_remote_group_state;      break;
            }

            cstr_to_rcistr(RCI_INDEX, attribute_name(&rci->input.attribute.match));

            rci->traversal.state = rci_traversal_state_all_groups_start;
            state_call(rci, rci_parser_state_traversal);
            break;
        }
        break;

    case rci_command_set_setting:
    case rci_command_set_state:
        if (have_group_id(rci))
        {
            rci_process_element_tag(rci, rci_action_unary_element);
        }
        else
        {
            rci_global_error(rci, idigi_rci_error_bad_element, RCI_NO_HINT);
        }
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
            rcistr_t const * const version = attribute_value(&rci->input.attribute.match);

            if (rcistr_empty(version) || (cstr_equals_rcistr(RCI_VERSION_SUPPORTED, version)))
            {
                prep_rci_reply_data(rci);
                state_call(rci, rci_parser_state_output);

                trigger_rci_callback(rci, idigi_remote_config_session_start);
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

            rci->output.tag = rci->shared.string.tag;
            rci->output.type = rci_output_type_start_tag;
            state_call(rci, rci_parser_state_output);

            cstr_to_rcistr(RCI_INDEX, attribute_name(&rci->input.attribute.match));

            trigger_rci_callback(rci, idigi_remote_config_action_start);
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

static void rci_handle_end_tag(rci_t * const rci)
{
    if (have_element_id(rci))
    {
        idigi_group_element_t const * const element = get_current_element(rci);

        if (element->access != idigi_element_access_write_only)
        {
            rci->output.tag = rci->shared.string.tag;

            rci->output.type = rci_output_type_end_tag;
            state_call(rci, rci_parser_state_output);
        }
        invalidate_element_id(rci);
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
        }
        else
        {
            config_request_id = idigi_remote_config_session_end;
        }

        trigger_rci_callback(rci, config_request_id);
    }
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
            end_rcistr(rci, &rci->shared.string.content);
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
        end_rcistr(rci, &rci->shared.string.tag);
        rci_handle_start_tag(rci);
        rci->input.state = rci_input_state_content_first;
        break;

    case rci_input_state_element_param_name:
        /* TODO: Why is comment here? */
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
        end_rcistr(rci, &rci->shared.string.tag);
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
            /* TODO: base state on previous state */
            rci->input.state = rci_input_state_content_first;
        }
        rci->input.hyphens = 0;
        break;

    default:
        rci_global_error(rci, idigi_rci_error_bad_xml, RCI_NO_HINT);
        break;
    }

    clear_rcistr(&rci->shared.string.tag);
    clear_rcistr(attribute_value(&rci->input.attribute.match));
}

static void rci_parse_input_equals_sign(rci_t * const rci)
{
    switch (rci->input.state)
    {
    case rci_input_state_element_param_equals:
        end_rcistr(rci, attribute_name(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_quote;
        break;

    case rci_input_state_element_param_value_first:
        begin_rcistr(rci, attribute_name(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
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
        end_rcistr(rci, &rci->shared.string.tag);
        /* no break; */

    case rci_input_state_element_param_name:
        rci->input.state = rci_input_state_element_tag_close;
        break;

    case rci_input_state_element_param_value_first:
        begin_rcistr(rci, attribute_value(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
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
        end_rcistr(rci, attribute_value(&rci->input.attribute.current));
        /* TODO: Error on same name match */
        if (rcistr_valid(attribute_name(&rci->input.attribute.match)) && rcistr_empty(attribute_value(&rci->input.attribute.match)))
        {
            if (rcistr_equals_rcistr(attribute_name(&rci->input.attribute.match), attribute_name(&rci->input.attribute.current)))
            {
                *attribute_value(&rci->input.attribute.match) = *attribute_value(&rci->input.attribute.current);
            }
        }
        rci->input.state = rci_input_state_element_param_name;
        break;

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
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
        end_rcistr(rci, &rci->shared.string.tag);
        rci->input.state = rci_input_state_element_param_name;
        break;

    case rci_input_state_element_param_name:
        /* TODO: check for having an attribute name */
        end_rcistr(rci, attribute_name(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_equals;
        break;

    case rci_input_state_element_param_value_first:
        begin_rcistr(rci, attribute_value(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;

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
    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
        /* no break; */

    case rci_input_state_content:
        rci->input.state = rci_input_state_content_escaping;
        rci->input.character = nul;
        ASSERT(rcistr_empty(&rci->input.entity));
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
    case rci_input_state_content_escaping:
        end_rcistr(rci, &rci->input.entity);
        rci->input.character = rci_entity_value(&rci->input.entity);
        ASSERT(rci->input.character != nul);
        rci->input.state = rci_input_state_content;
        break;

    case rci_input_state_element_param_value_first:
        begin_rcistr(rci, attribute_value(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
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

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
        rci->input.state = rci_input_state_content;
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
        begin_rcistr(rci, &rci->shared.string.tag);
        rci->input.state = rci_input_state_element_start_name;
        break;

    case rci_input_state_element_end_name_first:
        begin_rcistr(rci, &rci->shared.string.tag);
        rci->input.state = rci_input_state_element_end_name;
        break;

    case rci_input_state_content_escaping:
        if (rcistr_empty(&rci->input.entity))
            begin_rcistr(rci, &rci->input.entity);
        rci->input.character = nul;
        break;

    case rci_input_state_element_param_value_first:
        begin_rcistr(rci, attribute_value(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_value;
        break;

    case rci_input_state_element_param_name:
        begin_rcistr(rci, attribute_name(&rci->input.attribute.current));
        rci->input.state = rci_input_state_element_param_equals;
        break;

    case rci_input_state_content_first:
        begin_rcistr(rci, &rci->shared.string.content);
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

    if (pending_rci_callback(rci))
    {
        idigi_remote_group_response_t const * const response = &rci->shared.response;

        if (!rci_callback(rci))
            goto done;

        if (response->error_id != idigi_success)
        {
            rci_group_error(rci, response->error_id, response->element_data.error_hint);
            goto done;
        }

        {
            idigi_remote_config_request_t const remote_config_request = rci->callback.request.remote_config_request;

            switch (remote_config_request)
            {
            UNHANDLED_CASES_ARE_NEEDED;
            case idigi_remote_config_session_start:
            case idigi_remote_config_action_start:
            case idigi_remote_config_group_start:
                break;

            case idigi_remote_config_group_process:
                {
                    idigi_remote_group_request_t const * const request = &rci->shared.request;

                    switch (request->action)
                    {
                    UNHANDLED_CASES_ARE_NEEDED
                    case idigi_remote_action_set:
                        break;
                    case idigi_remote_action_query:
                        rci_output_content(rci);
                        goto done;
                        break;
                    }
                }
                break;
            case idigi_remote_config_session_cancel:
                rci->status = rci_status_error;
                goto done;
               break;

            case idigi_remote_config_group_end:
            case idigi_remote_config_action_end:
            case idigi_remote_config_session_end:
                switch (remote_config_request)
                {
                UNHANDLED_CASES_ARE_INVALID;
                case idigi_remote_config_group_end:
                {
                    idigi_group_t const * const group = get_current_group(rci);

                    cstr_to_rcistr(group->name, &rci->output.tag);

                    invalidate_group_id(rci);
                    invalidate_group_index(rci);
                    break;
                }
                case idigi_remote_config_action_end:
                    set_rci_command_tag(rci->input.command, &rci->output.tag);

                    rci->input.command = rci_command_header;
                    break;
                case idigi_remote_config_session_end:
                    cstr_to_rcistr(RCI_REPLY, &rci->output.tag);

                    rci->input.command = rci_command_unseen;
                    break;
                }

                rci->output.type = rci_output_type_end_tag;
                state_call(rci, rci_parser_state_output);
                goto done;
                break;
            }
        }
    }

    while ((rci->parser.state.current == rci_parser_state_input) && (rci_buffer_remaining(input) != 0))
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

        if (rci->callback.status == idigi_callback_busy)
        {
            goto done;
        }
    }

    if (rci_buffer_remaining(input) == 0)
    {
        if (rci->parser.state.current == rci_parser_state_input)
        {
            char const * const old_base = rcistr_data(&rci->shared.string.generic);
            char * const new_base = rci->input.storage;

            if (ptr_in_buffer(&rci->buffer.input, old_base))
            {
                size_t const bytes = (rci->buffer.input.end - old_base) + 1;

                if (bytes >= sizeof rci->input.storage)
                {
                    idigi_debug_printf("Maximum content size exceeded - wanted %u, had %y\n", bytes, sizeof rci->input.storage);
                    rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
                    goto done;
                }

                memcpy(new_base, old_base, bytes);

                adjust_rcistr(new_base, old_base, &rci->shared.string.generic);
                adjust_rcistr(new_base, old_base, attribute_name(&rci->input.attribute.current));
                adjust_rcistr(new_base, old_base, attribute_value(&rci->input.attribute.current));
                adjust_char_pointer(new_base, old_base, &rci->input.destination);
            }

            rci->status = rci_status_more_input;
        }
    }

done:
    output_debug_info(rci, RCI_DEBUG_SHOW_DIFFS);
}

