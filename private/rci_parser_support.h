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

#include <stdarg.h>
#include "idigi_strings.h"

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
    rci_parser_state_error,
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
	rci_input_state_content_escaping,
} rci_input_state_t;

typedef enum
{
    rci_traversal_state_all_groups_start,
    rci_traversal_state_all_groups_group_start,
    rci_traversal_state_one_group,
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
	rci_output_state_content_semicolon,
} rci_output_state_t;

typedef enum
{
    rci_error_state_none,
    rci_error_state_complete,
} rci_error_state_t;

typedef enum
{
    rci_output_type_start_tag,
    rci_output_type_content,
    rci_output_type_end_tag,
    rci_output_type_unary,
    rci_output_type_three_tuple,
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
    rci_status_t status;
    struct {
        rci_buffer_t input;
        rci_buffer_t output;
    } buffer;
    struct {
        struct {
            rci_parser_state_t current;
            rci_parser_state_t previous;
        } state;
    } parser;
    struct {
        rci_input_state_t state;
        int value;
        char * position;
        union {
            rci_string_t tag;
            rci_string_t content;
        } string;
        rci_attribute_list_t attribute;
    } input;
    struct {
        rci_command_t command;
        struct {
            int group;
            int element;
        } id;
    } traversal;
    struct {
        rci_string_t const * tag;
        rci_attribute_list_t * attribute;
        rci_output_type_t type;
        rci_output_state_t state;
        struct {
            idigi_element_value_type_t type;
            union {
                unsigned int * unsigned_integer;
                signed int * signed_integer;
                char const * counted_string;
                char const * nul_terminated_string;
                float * float_precision;
            } data;
        } content;
        size_t attribute_pair_index;
        size_t entity_scan_index;
    } output;
    struct {
        rci_error_state_t state;
        unsigned int error_id;
        char const * description;
        char const * hint;
    } error;
} rci_t;

static char const nul = '\0';

static idigi_bool_t cstr_equals_rcistr(char const * const cstr, rci_string_t const * const rcistr)
{
    return cstr_equals_buffer(cstr, rcistr->data, rcistr->length);
}

static void rci_set_buffer(rci_buffer_t * const dst, rci_service_buffer_t const * const src)
{
    char * const start = src->data;
    
    dst->start = start;
    dst->end = start + src->bytes;
    dst->current = start;
}

static size_t rci_buffer_remaining(rci_buffer_t const * const buffer)
{
    return (buffer->end - buffer->current);
}

static char * rci_buffer_position(rci_buffer_t const * const buffer)
{
    return buffer->current;
}

static void rci_buffer_advance(rci_buffer_t * const buffer, size_t const amount)
{
    assert((buffer->current + amount) <= buffer->end);
    buffer->current += amount;
}

static int rci_buffer_read(rci_buffer_t const * const buffer)
{
    assert(rci_buffer_remaining(buffer) != 0);
    
    return *(buffer->current);
}

static void rci_buffer_write(rci_buffer_t const * const buffer, int const value)
{
    assert(rci_buffer_remaining(buffer) != 0);
    
    *(buffer->current) = value;
}


