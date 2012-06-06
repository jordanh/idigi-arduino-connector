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

static idigi_bool_t rci_output_data(rci_buffer_t * const output, char const * const data, size_t const bytes)
{
    idigi_bool_t const overflow = (rci_buffer_remaining(output) < bytes);
    
    if (!overflow)
    {
        memcpy(rci_buffer_position(output), data, bytes);
        rci_buffer_advance(output, bytes);
    }
    
    return overflow;
}

static idigi_bool_t rci_output_rcistr(rci_buffer_t * const output, rci_string_t const * const rcistr)
{
    return rci_output_data(output, rcistr->data, rcistr->length);
}

static idigi_bool_t rci_output_cstr(rci_buffer_t * const output, char const * const cstr)
{
    ASSERT(cstr != NULL);
    
    return rci_output_data(output, CSTR_DATA(cstr), CSTR_LEN(cstr));
}

static idigi_bool_t rci_output_formatted(rci_buffer_t * const output, char const * const format, ...)
{
    idigi_bool_t overflow;
    
    va_list ap;
        
    va_start(ap, format);
    {
        size_t const have = rci_buffer_remaining(output);
        int const result = vsnprintf(rci_buffer_position(output), have, format, ap);
        size_t const need = (size_t) result;
        
        ASSERT(result >= 0);
        overflow = (need > have);
        if (!overflow)
        {
            size_t const used = (size_t) result;
            
            rci_buffer_advance(output, used);
        }   
    }
    va_end(ap);

    return overflow;
}

static void rci_output_character(rci_buffer_t * const output, int const value)
{
    size_t const bytes = 1;
    ASSERT(rci_buffer_remaining(output) != 0);
    
    rci_buffer_write(output, value);
    rci_buffer_advance(output, bytes);
}

static idigi_bool_t rci_output_non_entity_character(rci_buffer_t * const output, int const value)
{
    idigi_bool_t const non_entity_character = (rci_entity_name(value) == NULL);
    
    if (non_entity_character)
    {
        rci_output_character(output, value);
    }
    
    return non_entity_character;
}

static idigi_bool_t rci_output_entity_name(rci_buffer_t * const output, int const value)
{
    char const * const name = rci_entity_name(value);
    
    ASSERT(name != NULL);
    
    return rci_output_cstr(output, name);
}

static cstr_t const * enum_value_to_cstr(rci_t * const rci, unsigned int const value)
{
    unsigned int const index = value;
    idigi_group_element_t const * const element = get_current_element(rci);
    idigi_element_value_limit_t const * const value_limit = element->value_limit;
    idigi_element_value_enum_t const * const enum_value = &value_limit->enum_value;
    cstr_t const * result = NULL;
    
    if (index < enum_value->count)
    {
        result = enum_value->value[index];
    }
        
    return result;
}

static cstr_t const * on_off_value_to_cstr(idigi_on_off_t const value)
{
    cstr_t const * result = NULL;
    
    switch (value)
    {
    UNHANDLED_CASES_ARE_NEEDED
    case idigi_off: result = RCI_OFF;   break;
    case idigi_on:  result = RCI_ON;    break;
    }
    
    return result;
}

static cstr_t const * boolean_value_to_cstr(idigi_boolean_t const value)
{
    cstr_t const * result = NULL;
    
    switch (value)
    {
    UNHANDLED_CASES_ARE_NEEDED
    case idigi_false:   result = RCI_FALSE; break;
    case idigi_true:    result = RCI_TRUE;  break;
    }
    
    return result;
}

static void rci_generate_output(rci_t * const rci)
{
    rci_buffer_t * const output = &rci->buffer.output;
    idigi_bool_t overflow = idigi_false;
    
    while ((rci_buffer_remaining(output) != 0) && !overflow)
    {
        output_debug_info(rci, RCI_DEBUG_SHOW_DIFFS);

        switch (rci->output.state)
        {
        UNHANDLED_CASES_ARE_NEEDED
        case rci_output_state_none:
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_NEEDED
            case rci_output_type_start_tag:
            case rci_output_type_end_tag:
            case rci_output_type_unary:
            case rci_output_type_three_tuple:
                rci->output.state = rci_output_state_element_tag_open;
                break;
            case rci_output_type_content:
                rci->output.state = rci_output_state_content;
                break;
            }
            break;
            
        case rci_output_state_element_tag_open:
            rci_output_character(output, '<');
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_end_tag:
                rci->output.state = rci_output_state_element_tag_slash;
                break;
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                rci->output.state = rci_output_state_element_tag_name;
                break;
            }
            break;
            
        case rci_output_state_element_tag_slash:
            rci_output_character(output, '/');
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_end_tag:
                rci->output.state = rci_output_state_element_tag_name;
                break;
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                rci->output.state = rci_output_state_element_tag_close;
                break;
            }
            break;

        case rci_output_state_element_tag_name:
            overflow = rci_output_rcistr(output, &rci->output.tag);
            if (overflow) break;
            
            clear_rcistr(&rci->output.tag);
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_end_tag:
                rci->output.state = rci_output_state_element_tag_close;
                break;
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                if (have_attributes(&rci->output.attribute))
                {
                    rci->output.state = rci_output_state_element_param_space;
                    rci->output.attribute_pair_index = 0;
                }
                else
                {
                    rci->output.state = (rci->output.type == rci_output_type_start_tag) ? rci_output_state_element_tag_close : rci_output_state_element_tag_slash;
                }
                break;
            }
            break;

        case rci_output_state_element_tag_close:
            rci_output_character(output, '>');
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_end_tag:
                if (rci->input.command == rci_command_unseen)
                {
                    rci->status = rci_status_complete;
                }
                /* no break; */
                
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                rci->output.state = rci_output_state_none;
                break;
            }
            break;

        case rci_output_state_element_param_space:
            if (rci->output.attribute_pair_index < rci->output.attribute.count)
            {
                rci_output_character(output, ' ');
                
                switch (rci->output.type)
                {
                UNHANDLED_CASES_ARE_INVALID
                case rci_output_type_start_tag:
                case rci_output_type_unary:
                    rci->output.state = rci_output_state_element_param_name;
                    break;
                }
            }
            else
            {
                clear_attributes(&rci->output.attribute);
                
                switch (rci->output.type)
                {
                UNHANDLED_CASES_ARE_INVALID
                case rci_output_type_start_tag:
                case rci_output_type_unary:
                    rci->output.state = rci_output_state_element_tag_close;
                    break;
                }
            }
            break;

        case rci_output_state_element_param_name:
            overflow = rci_output_rcistr(output, attribute_name(&rci->output.attribute, rci->output.attribute_pair_index));
            if (overflow) break;
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_end_tag:
                rci->output.state = rci_output_state_element_tag_close;
                break;
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                rci->output.state = rci_output_state_element_param_equal_sign;
                break;
            }
            break;

        case rci_output_state_element_param_equal_sign:
            rci_output_character(output, '=');
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                rci->output.state = rci_output_state_element_param_start_quote;
                break;
            }
            break;
        
        case rci_output_state_element_param_start_quote:
        case rci_output_state_element_param_end_quote:
            rci_output_character(output, '"');
            
            switch (rci->output.type)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_output_type_start_tag:
            case rci_output_type_unary:
                if (rci->output.state == rci_output_state_element_param_start_quote)
                {
                    rci->output.state = rci_output_state_element_param_value;
                }
                else
                {
                    ASSERT(rci->output.state == rci_output_state_element_param_end_quote);
                    rci->output.attribute_pair_index++;
                    rci->output.state = rci_output_state_element_param_space;
                }
                break;
            }
            break;
        case rci_output_state_element_param_value:
            rci->output.entity_scan_index = 0;
            rci->output.state = rci_output_state_element_param_value_scan;
            /* no break; */
        case rci_output_state_element_param_value_scan:
            {
                rci_string_t const * const value = attribute_value(&rci->output.attribute, rci->output.attribute_pair_index);
                char const * const data = rcistr_data(value);
                
                if (rci_output_non_entity_character(output, data[rci->output.entity_scan_index]))
                {
                    rci->output.entity_scan_index++;
                    if (rci->output.entity_scan_index == rcistr_length(value))
                    {
                            rci->output.state = rci_output_state_element_param_end_quote;
                    }
                }
                else
                {
                    rci_output_character(output, '&');
                    rci->output.state = rci_output_state_element_param_value_entity;
                }
            }
            break;
        case rci_output_state_element_param_value_entity:
            {
                rci_string_t const * const value = attribute_value(&rci->output.attribute, rci->output.attribute_pair_index);
                char const * const data = rcistr_data(value);

                overflow = rci_output_entity_name(output, data[rci->output.entity_scan_index]);
                if (overflow) break;
                
                rci->output.state = rci_output_state_element_param_value_semicolon;
            }
            break;
        case rci_output_state_element_param_value_semicolon:
            rci_output_character(output, ';');

            rci->output.entity_scan_index++;
            rci->output.state = rci_output_state_element_param_value_scan;
            break;
        case rci_output_state_content:
            switch (rci->shared.request.element.type)
            {
            UNHANDLED_CASES_ARE_NEEDED
            case idigi_element_type_string:
            case idigi_element_type_multiline_string:
            case idigi_element_type_password:
                rci->output.state = rci_output_state_content_scan;
                rci->output.entity_scan_index = 0;
                break;
            case idigi_element_type_ipv4:
            case idigi_element_type_fqdnv4:
            case idigi_element_type_fqdnv6:
            case idigi_element_type_datetime:
                overflow = rci_output_data(output, rci->shared.value.string_value, strlen(rci->shared.value.string_value));
                break;
            case idigi_element_type_enum:
                overflow = rci_output_cstr(output, enum_value_to_cstr(rci, rci->shared.value.enum_value));
                break;
            case idigi_element_type_on_off:
                overflow = rci_output_cstr(output, on_off_value_to_cstr(rci->shared.value.on_off_value));
                break;
            case idigi_element_type_boolean:
                overflow = rci_output_cstr(output, boolean_value_to_cstr(rci->shared.value.boolean_value));
                break;
            case idigi_element_type_int32:
                overflow = rci_output_formatted(output, "%ld", rci->shared.value.integer_signed_value);
                break;
            case idigi_element_type_uint32:
                overflow = rci_output_formatted(output, "%lu", rci->shared.value.integer_signed_value);
                break;
            case idigi_element_type_hex32:
                overflow = rci_output_formatted(output, "%lx", rci->shared.value.integer_signed_value);
                break;
            case idigi_element_type_0xhex:
                overflow = rci_output_formatted(output, "0x%lx", rci->shared.value.integer_signed_value);
                break;
            case idigi_element_type_float:
                overflow = rci_output_formatted(output, "%f", rci->shared.value.float_value);
                break;
            }
            if (overflow) break;

            if (rci->output.state == rci_output_state_content)
            {
                switch (rci->output.type)
                {
                UNHANDLED_CASES_ARE_INVALID;
                case rci_output_type_content:
                    rci->output.state = rci_output_state_none;
                    break;
                case rci_output_type_three_tuple:
                    rci->output.state = rci_output_state_element_tag_open;
                    rci->output.type = rci_output_type_end_tag;
                    break;
                }
            }
            break;
        case rci_output_state_content_scan:
            {
                int const character = rci->shared.value.string_value[rci->output.entity_scan_index];
                
                if (character == '\0')
                {
                    switch (rci->output.type)
                    {
                    UNHANDLED_CASES_ARE_INVALID;
                    case rci_output_type_content:
                        rci->output.state = rci_output_state_none;
                        break;
                    case rci_output_type_three_tuple:
                        rci->output.state = rci_output_state_element_tag_open;
                        rci->output.type = rci_output_type_end_tag;
                        break;
                    }
                }
                else if (rci_output_non_entity_character(output, character))
                {
                    rci->output.entity_scan_index++;
                }
                else
                {
                    rci_output_character(output, '&');
                    rci->output.state = rci_output_state_content_entity;
                }
            }
            break;
        case rci_output_state_content_entity:
            {
                int const entity = rci->shared.value.string_value[rci->output.entity_scan_index];
                
                overflow = rci_output_entity_name(output, entity);
                if (overflow) break;
                
                rci->output.entity_scan_index++;
                rci->output.state = rci_output_state_content_semicolon;
             }
            break;
        case rci_output_state_content_semicolon:
            rci_output_character(output, ';');

            rci->output.state = rci_output_state_content_scan;
            break;
        }

        if (rci->output.state == rci_output_state_none)
        {
            rci->parser.state.current = rci->parser.state.previous;
            break;
        }
    }

    if (rci->output.state != rci_output_state_none)
    {
        rci->status = rci_status_flush_output;
    }
    
    output_debug_info(rci, RCI_DEBUG_SHOW_DIFFS);
}

