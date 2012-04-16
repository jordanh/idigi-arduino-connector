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

#if ! defined RCI_DEBUG

#define output_debug_info(rci)  ((void) rci)

#else

#if 0
typedef enum
{
    rci_session_start,
    rci_session_active,
    rci_session_lost
} rci_session_t;

typedef enum
{
    rci_status_internal_error,  /* bad code path */
    rci_status_complete,        /* all done */
    rci_status_busy,            /* user callback returned busy */
    rci_status_more_input,      /* have output buffer space to process more input */
    rci_status_flush_output,    /* need more output space, so send out the current buffer */
    rci_status_error            /* error occured, RCI service should inform messaging layer to cancel the session */
} rci_status_t;

typedef struct
{
    char * data;
    size_t bytes;
} rci_service_buffer_t;

typedef struct
{
    rci_service_buffer_t input;
    rci_service_buffer_t output;
} rci_service_data_t;

typedef struct
{
    char * start;
    char * end;
    char * current;
} rci_buffer_t;

typedef enum
{
    rci_parser_state_input,
    rci_parser_state_traversal,
    rci_parser_state_output,
    rci_parser_state_error
} rci_parser_state_t;

typedef enum
{
    rci_input_state_element_tag_open,
    rci_input_state_element_tag_name,
    rci_input_state_element_tag_close,
    rci_input_state_element_start_name,
	rci_input_state_element_end_name,
	rci_input_state_element_param_name,
	rci_input_state_element_param_quote,
	rci_input_state_element_param_value,
	rci_input_state_element_param_value_escaping,
	rci_input_state_content,
	rci_input_state_content_escaping
} rci_input_state_t;

typedef enum
{
    rci_traversal_state_all_groups_start,
    rci_traversal_state_all_groups_group_start,
    rci_traversal_state_one_group,
    rci_traversal_state_one_element
} rci_traversal_state_t;

typedef enum
{
    rci_output_state_none,
    rci_output_state_element_tag_open,
	rci_output_state_element_tag_slash,
    rci_output_state_element_tag_name,
    rci_output_state_element_tag_close,
	rci_output_state_element_param_space,
	rci_output_state_element_param_name,
	rci_output_state_element_param_equal_sign,
	rci_output_state_element_param_start_quote,
	rci_output_state_element_param_value,
	rci_output_state_element_param_value_scan,
	rci_output_state_element_param_value_entity,
	rci_output_state_element_param_value_semicolon,
	rci_output_state_element_param_end_quote,
	rci_output_state_content,
	rci_output_state_content_scan,
	rci_output_state_content_entity,
	rci_output_state_content_semicolon
} rci_output_state_t;

typedef enum
{
    rci_error_state_none,
    rci_error_state_complete
} rci_error_state_t;

typedef enum
{
    rci_output_type_start_tag,
    rci_output_type_content,
    rci_output_type_end_tag,
    rci_output_type_unary,
    rci_output_type_three_tuple
} rci_output_type_t;    

typedef enum
{
    rci_command_set_setting,
    rci_command_set_state,
    rci_command_query_setting,
    rci_command_query_state,
    rci_command_unseen,
    rci_command_header,
    rci_command_unknown
} rci_command_t;

typedef struct
{
    char const * data;
    size_t length;
} rci_string_t;

typedef struct
{
    rci_string_t name;
    rci_string_t value;
} rci_attribute_t;

typedef struct
{
    size_t count;
    rci_attribute_t pair[2];
} rci_attribute_list_t;

typedef struct
{
    rci_service_data_t * service_data;
    struct {
        rci_error_state_t state;
        idigi_remote_group_response_t response;
        char const * description;
    } error;
} rci_t;
#endif

static void reassign_service_buffer(rci_service_buffer_t * const dst, rci_service_buffer_t * const src)
{
    if (dst->data != NULL)
    {
        free(dst->data);
        dst->data = NULL;
        dst->bytes = 0;
    }
    
    if ((src->data != NULL) && (src->bytes != 0))
    {
        dst->bytes = src->bytes;
        dst->data = calloc(1, dst->bytes);
        ASSERT(dst->data != NULL);
    }
}

#define enum_to_case(name)  case name:  result = #name;             break

static char const * rci_status_t_as_string(rci_status_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_status_internal_error);
        enum_to_case(rci_status_complete);
        enum_to_case(rci_status_busy);
        enum_to_case(rci_status_more_input);
        enum_to_case(rci_status_flush_output);
        enum_to_case(rci_status_error);
    }
    return result;
}

static char const * rci_parser_state_t_as_string(rci_parser_state_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_parser_state_input);
        enum_to_case(rci_parser_state_traversal);
        enum_to_case(rci_parser_state_output);
        enum_to_case(rci_parser_state_error);
    }
    return result;
}

static char const * rci_input_state_t_as_string(rci_input_state_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_input_state_element_tag_open);
        enum_to_case(rci_input_state_element_tag_name);
        enum_to_case(rci_input_state_element_tag_close);
        enum_to_case(rci_input_state_element_start_name);
	    enum_to_case(rci_input_state_element_end_name);
	    enum_to_case(rci_input_state_element_param_name);
	    enum_to_case(rci_input_state_element_param_quote);
	    enum_to_case(rci_input_state_element_param_value);
	    enum_to_case(rci_input_state_element_param_value_escaping);
	    enum_to_case(rci_input_state_content);
	    enum_to_case(rci_input_state_content_escaping);
    }
    return result;
}

static char const * rci_traversal_state_t_as_string(rci_traversal_state_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_traversal_state_none);
        enum_to_case(rci_traversal_state_all_groups_start);
        enum_to_case(rci_traversal_state_all_groups_group_start);
        enum_to_case(rci_traversal_state_all_groups_element_start);
        enum_to_case(rci_traversal_state_all_groups_element_data);
        enum_to_case(rci_traversal_state_all_groups_element_end);
        enum_to_case(rci_traversal_state_all_groups_group_end);
        enum_to_case(rci_traversal_state_all_groups_end);
        enum_to_case(rci_traversal_state_one_group_start);
        enum_to_case(rci_traversal_state_one_group_element_start);
        enum_to_case(rci_traversal_state_one_group_element_data);
        enum_to_case(rci_traversal_state_one_group_element_end);
        enum_to_case(rci_traversal_state_one_group_end);
        enum_to_case(rci_traversal_state_one_element_start);
        enum_to_case(rci_traversal_state_one_element_data);
        enum_to_case(rci_traversal_state_one_element_end);
    }
    return result;
}

static char const * rci_command_t_as_string(rci_command_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_command_set_setting);
        enum_to_case(rci_command_set_state);
        enum_to_case(rci_command_query_setting);
        enum_to_case(rci_command_query_state);
        enum_to_case(rci_command_unseen);
        enum_to_case(rci_command_header);
        enum_to_case(rci_command_unknown);
    }
    return result;
}

static char const * idigi_remote_action_t_as_string(idigi_remote_action_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(idigi_remote_action_set);
        enum_to_case(idigi_remote_action_query);
    }
    return result;
}

static char const * idigi_remote_group_type_t_as_string(idigi_remote_group_type_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(idigi_remote_group_setting);
        enum_to_case(idigi_remote_group_state);
    }
    return result;
}

static char const * rci_output_state_t_as_string(rci_output_state_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_output_state_none);
        enum_to_case(rci_output_state_element_tag_open);
	    enum_to_case(rci_output_state_element_tag_slash);
        enum_to_case(rci_output_state_element_tag_name);
        enum_to_case(rci_output_state_element_tag_close);
	    enum_to_case(rci_output_state_element_param_space);
	    enum_to_case(rci_output_state_element_param_name);
	    enum_to_case(rci_output_state_element_param_equal_sign);
	    enum_to_case(rci_output_state_element_param_start_quote);
	    enum_to_case(rci_output_state_element_param_value);
	    enum_to_case(rci_output_state_element_param_value_scan);
	    enum_to_case(rci_output_state_element_param_value_entity);
	    enum_to_case(rci_output_state_element_param_value_semicolon);
	    enum_to_case(rci_output_state_element_param_end_quote);
	    enum_to_case(rci_output_state_content);
	    enum_to_case(rci_output_state_content_scan);
	    enum_to_case(rci_output_state_content_entity);
	    enum_to_case(rci_output_state_content_semicolon);
    }
    return result;
}

static char const * rci_output_type_t_as_string(rci_output_type_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_output_type_start_tag);
        enum_to_case(rci_output_type_content);
        enum_to_case(rci_output_type_end_tag);
        enum_to_case(rci_output_type_unary);
        enum_to_case(rci_output_type_three_tuple);
    }
    return result;
}

static char const * idigi_element_value_type_t_as_string(idigi_element_value_type_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(idigi_element_type_string);
        enum_to_case(idigi_element_type_multiline_string);
        enum_to_case(idigi_element_type_password);
        enum_to_case(idigi_element_type_int32);
        enum_to_case(idigi_element_type_uint32);
        enum_to_case(idigi_element_type_hex32);
        enum_to_case(idigi_element_type_0xhex);
        enum_to_case(idigi_element_type_float);
        enum_to_case(idigi_element_type_enum);
        enum_to_case(idigi_element_type_on_off);
        enum_to_case(idigi_element_type_boolean);
        enum_to_case(idigi_element_type_ipv4);
        enum_to_case(idigi_element_type_fqdnv4);
        enum_to_case(idigi_element_type_fqdnv6);
        enum_to_case(idigi_element_type_datetime);
    }
    return result;
}

static char const * rci_error_state_t_as_string(rci_error_state_t const value)
{
    char const * result;
    switch (value)
    {
        enum_to_case(rci_error_state_none);
	    enum_to_case(rci_error_state_complete);
    }
    return result;
}

static void output_buffer_diff(char const * const name, void const * const current, void * const previous, size_t const bytes)
{
    if (memcmp(current, previous, bytes) != 0)
    {
        char const * current_buffer = current;
        size_t remaining = bytes;
        
        printf("%s\n", name);
        while (remaining != 0)
        {
            size_t const bytes_per_line = 16;
            size_t const bytes_this_line = (remaining < bytes_per_line) ? remaining : bytes_per_line;
            size_t i;
            
            printf("%p: ", current_buffer);
            for (i = 0; i < bytes_this_line; i++)
            {
                int const byte = current_buffer[i];
                
                printf("%02x ", byte);
            }
            for (i = 0; i < bytes_this_line; i++)
            {
                int const byte = current_buffer[i];
                int const replacement = '.';
                
                printf("%c ", (isprint(byte) && !iscntrl(byte)) ? byte : replacement);
            }
            
            remaining -= bytes_this_line;
            current_buffer += bytes_this_line;
            
            putchar('\n');
        }
        
        memcpy(previous, current, bytes);             
    }
}

#define output_diff(format, operation, member)  do {if (previous.member != current->member) printf("%s(%p): " format " -> " format "\n", #member, (void *)&(current->member), operation(previous.member), operation(current->member)); previous.member = current->member; } while (0)

#define output_enum(name, member)               output_diff("%s", name ## _as_string, member)
#define output_pointer(member)                  output_diff("%p", (void *), member)
#define output_unsigned_int(member)             output_diff("%u", (unsigned int), member)
#define output_signed_int(member)               output_diff("%d", (signed int), member)
#define output_size(member)                     output_diff("%zu", (size_t), member)

#define output_service_data_buffer(name)        do { output_pointer(name.data); output_unsigned_int(name.bytes); } while (0)
#define output_rci_buffer(name)                 do { output_pointer(name.start); output_pointer(name.end); output_pointer(name.current); } while (0)
#define output_rci_string(name)                 do { output_pointer(name.data); output_size(name.length); } while (0)
#define output_rci_attribute(pair)              do { output_rci_string(pair.name); output_rci_string(pair.value); } while (0)
#define output_rci_attribute_list(name)         do { output_size(name.count); assert(asizeof(current->name.pair) == 2); output_rci_attribute(name.pair[0]); output_rci_attribute(name.pair[1]); } while (0)

#define output_service_data_buffer_diff(name)   output_buffer_diff("service_data." #name, current->service_data->name.data, service_data.name.data, current->service_data->name.bytes)

static void output_debug_info(rci_t const * const current)
{
    static rci_t previous;
    static rci_service_data_t service_data;
    static long unsigned int step;
    
    /* have we started a new session? */
    if ((previous.service_data == NULL) && (current->service_data != NULL))
    {
        reassign_service_buffer(&service_data.input, &current->service_data->input);
        reassign_service_buffer(&service_data.output, &current->service_data->output);
    }

    printf("STEP %lu:\n", step);
    
    output_pointer(service_data);

    output_service_data_buffer(service_data->input);
    output_service_data_buffer(service_data->output);

    output_enum(rci_status_t, status);

    output_rci_buffer(buffer.input);
    output_rci_buffer(buffer.output);
    
    output_enum(rci_parser_state_t, parser.state.current);
    output_enum(rci_parser_state_t, parser.state.previous);

    output_enum(rci_input_state_t, input.state);
    output_signed_int(input.value);
    output_pointer(input.position);
    output_rci_string(input.string.generic);
    output_rci_string(input.entity);
    output_rci_attribute_list(input.attribute);

    output_enum(rci_traversal_state_t, traversal.state);
    output_enum(rci_command_t, traversal.state);
    output_enum(idigi_remote_group_type_t, traversal.type);
    output_enum(idigi_remote_action_t, traversal.action);
    output_signed_int(traversal.id.group);
    output_signed_int(traversal.id.element);
    output_unsigned_int(traversal.index);
    output_rci_string(traversal.tag);
        
    output_enum(rci_output_state_t, output.state);
    output_enum(rci_output_type_t, output.type);
    output_pointer(output.tag);
    output_pointer(output.attribute);
    output_enum(idigi_element_value_type_t, output.content.type);
    output_pointer(output.content.data.generic);
    output_size(output.attribute_pair_index);
    output_size(output.entity_scan_index);

    output_enum(rci_error_state_t, error.state);
    output_pointer(error.response.user_context);
    output_unsigned_int(error.response.error_id);
    if (current->error.response.error_id == 0)
    {
        output_pointer(error.response.element_data.element_value);
    }
    else
    {
        output_pointer(error.response.element_data.error_hint);
    }
    output_pointer(error.description);

    output_service_data_buffer_diff(input);
    output_service_data_buffer_diff(output);

    /* TODO: output temporary transfer buffer */
    
    previous = *current;
    step++;
}

#endif
