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

static rci_command_t find_rci_command(rci_t * const rci)
{
    static char const * const command[] = {
        INDEXED_CSTR(IDIGI_SET_SETTING_STRING_INDEX),
        INDEXED_CSTR(IDIGI_SET_STATE_STRING_INDEX),
        INDEXED_CSTR(IDIGI_QUERY_SETTING_STRING_INDEX),
        INDEXED_CSTR(IDIGI_QUERY_STATE_STRING_INDEX)
    };
    size_t i;
    rci_command_t result = rci_command_unknown;
      
    CONFIRM(rci_command_set_setting == 0);
    CONFIRM(rci_command_set_state == 1);
    CONFIRM(rci_command_query_setting == 2);
    CONFIRM(rci_command_query_state == 3);
    
    for (i = 0; i < asizeof(command); i++)
    {
        if (cstr_equals_rcistr(command[i], &rci->input.string.tag))
        {
            result = (rci_command_t) i;
            break;
        }
    }
    
    return result;
}

static idigi_bool_t rci_handle_unary_tag(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;

    switch (rci->traversal.command)
    {
    case rci_command_unseen:
        assert(idigi_false);
        /* clean up, something very bad happened */
        break;
        
    case rci_command_header:
        rci->traversal.command = find_rci_command(rci);
        if (rci->traversal.command == rci_command_unknown)
        {
            /* report error: unknown tag */
        }
        else
        {
            /* traverse all groups */
        }
        break;
        
    case rci_command_set_setting:
    case rci_command_set_state:
        assert(idigi_false);
        /* clean up - can't have unary tag on sets */
        break;
            
    case rci_command_query_setting:
    case rci_command_query_state:
        if (rci->traversal.id.group == -1)
        {
            /* search for group name */
            /* traverse single group */
        }
        else
        {
            assert(rci->traversal.id.element == -1);
            /* search for element name */
            /* return single item */
        }   
        break;
        
    case rci_command_unknown:
        assert(idigi_false);
        break;
    }
    
    assert(continue_parsing || (rci->parser.state.current != rci_parser_state_input));
    return continue_parsing;
}

static idigi_bool_t rci_handle_start_tag(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->traversal.command)
    {
    case rci_command_unseen:
        if (cstr_equals_rcistr(INDEXED_CSTR(IDIGI_RCI_REQUEST_STRING_INDEX), &rci->input.string.tag) &&
            (rci->input.attribute.count == 1) &&
            cstr_equals_rcistr(INDEXED_CSTR(IDIGI_VERSION_STRING_INDEX), &rci->input.attribute.pair[0].name) &&
            cstr_equals_rcistr(INDEXED_CSTR(IDIGI_RCI_VERSION_STRING_INDEX), &rci->input.attribute.pair[0].value))
        {
            rci->traversal.command = rci_command_header;
            /* output reply tag */
        }
        else
        {
            /* return error indicating version failed */
        }
        break;
        
    case rci_command_header:
        rci->traversal.command = find_rci_command(rci);
        if (rci->traversal.command == rci_command_unknown)
        {
            /* report unknown tag */
        }
        else
        {
            /* output command tag */
        }
        break;
        
    case rci_command_set_setting:
    case rci_command_set_state:
        if (rci->traversal.id.group == -1)
        {
            /* search for group name */
            /* output group tag */
        }
        else
        {
            assert(rci->traversal.id.element == -1);
            /* search for element name */
            /* output element tag */
        }   
        break;
        
    case rci_command_query_setting:
    case rci_command_query_state:
        if (rci->traversal.id.group == -1)
        {
            /* search for group name */
            /* output group tag */
        }
        else
        {
            assert(idigi_false);
            /* clean up, something very bad happened */
            break;
        }   
        break;
        
    case rci_command_unknown:
        assert(idigi_false);
        break;
    }

    assert(continue_parsing || (rci->parser.state.current != rci_parser_state_input));
    return continue_parsing;
}

static idigi_bool_t rci_handle_content(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    /* parse ints and enums (and conditionally floats) */
    /* callback with content */
    /* if callback returns busy, retard the pointer */
    
    (void) rci;

    assert(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_handle_end_tag(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    /* output end tag for wherever we are at */
    (void) rci;

    assert(continue_parsing);
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
        if (rci->traversal.id.element == -1)
        {
            /* NUL-terminate and validate content contains only whitespace */
#if 0 /* TODO: need debug specific code here - can't call str*() methods as the contant is not NUL-terminated */
            assert((rci_buffer_write(&rci->buffer.input, nul), (strspn(rci->input.string.content, " \t\n\r\f") == strlen(rci->input.string.content))));
#endif
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

    assert(continue_parsing);
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

    assert(continue_parsing);
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

    assert(continue_parsing);
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

    assert(continue_parsing);
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

    assert(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_whitespace(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value_escaping:
	case rci_input_state_content_escaping:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;

    default:
        break;
    }

    assert(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_ampersand(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value:
        rci->input.state = rci_input_state_element_param_value_escaping;
        break;

    case rci_input_state_content:
        rci->input.state = rci_input_state_content_escaping;
        break;
        
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    assert(continue_parsing);
    return continue_parsing;
}

static idigi_bool_t rci_parse_input_semicolon(rci_t * const rci)
{
    idigi_bool_t continue_parsing = idigi_true;
    
    switch (rci->input.state)
    {
    case rci_input_state_element_param_value_escaping:
    case rci_input_state_content_escaping:
        /* find entity */
        /* use write pointer - tricky stuff here */
        break;

    case rci_input_state_element_param_value:
    case rci_input_state_content:
        break;
        
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    assert(continue_parsing);
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
        if (rci->input.string.tag.data == NULL)
            rci->input.string.tag.data = rci->input.position;
        break;
                
    case rci_input_state_element_param_quote:
    default:
        rci->status = rci_status_internal_error;
        continue_parsing = idigi_false;
        break;
    }

    assert(continue_parsing);
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
        
        rci->input.value = rci_buffer_read(input);
        switch (rci->input.value)
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
        
        /* find out if we are compressing entities */
        if (rci->input.position != rci_buffer_position(&rci->buffer.input))
        {
            *(rci->input.position) = rci->input.value;
            rci->input.position++;
        }
        
        rci_buffer_advance(input, 1);
    }
    
    if (rci_buffer_remaining(input) == 0)
    {
        /* if preservation is needed, copy data and adjust pointers */
        /* including the position pointer - more tricky stuff */
    }
    
done:
    ;
}

