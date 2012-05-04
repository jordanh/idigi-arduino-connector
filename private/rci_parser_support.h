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
#if defined IDIGI_DEBUG
#include <ctype.h>
#endif
#define IDIGI_RCI_PARSER_INTERNAL_DATA
#include "remote_config.h"

#define ROUND_UP(value, interval)   ((value) + -(value) % (interval))

static char const nul = '\0';

static char const * const on_off_strings[] = {RCI_OFF, RCI_ON};
static idigi_element_value_enum_t on_off_enum = { asizeof(on_off_strings), on_off_strings};

static char const * const boolean_strings[] = {RCI_FALSE, RCI_TRUE};
static idigi_element_value_enum_t boolean_enum = { asizeof(boolean_strings), boolean_strings};

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
	rci_input_state_element_end_name_first,
	rci_input_state_element_end_name,
	rci_input_state_element_param_name,
	rci_input_state_element_param_equals,
	rci_input_state_element_param_quote,
	rci_input_state_element_param_value_first,
	rci_input_state_element_param_value,
	rci_input_state_element_param_value_escaping,
	rci_input_state_content_first,
	rci_input_state_content,
	rci_input_state_content_escaping
} rci_input_state_t;

typedef enum
{
    rci_traversal_state_none,
    rci_traversal_state_all_groups_start,
    rci_traversal_state_all_groups_group_start,
    rci_traversal_state_all_groups_element_start,
    rci_traversal_state_all_groups_element_data,
    rci_traversal_state_all_groups_element_end,
    rci_traversal_state_all_groups_group_end,
    rci_traversal_state_all_groups_end,
    rci_traversal_state_one_group_start,
    rci_traversal_state_one_group_element_start,
    rci_traversal_state_one_group_element_data,
    rci_traversal_state_one_group_element_end,
    rci_traversal_state_one_group_end,
    rci_traversal_state_one_element_start,
    rci_traversal_state_one_element_data,
    rci_traversal_state_one_element_end
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
    rci_error_state_error_open,
    rci_error_state_error_content,
    rci_error_state_error_close,
    rci_error_state_element_close,
    rci_error_state_group_close,
    rci_error_state_command_close,
    rci_error_state_reply_close,
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
    rci_command_unseen,
    rci_command_header,
    rci_command_set_setting,
    rci_command_set_state,
    rci_command_query_setting,
    rci_command_query_state,
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
        idigi_remote_config_request_t config_request;
        idigi_callback_status_t status;
    } callback;
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
        int character;
        char * destination;
        idigi_bool_t send_content;
        rci_command_t command;
        rci_string_t entity;
        char storage[ROUND_UP(IDIGI_RCI_MAXIMUM_CONTENT_LENGTH + sizeof nul, sizeof (int))];
    } input;
    struct {
        rci_traversal_state_t state;
        rci_string_t tag;
    } traversal;
    struct {
        rci_output_state_t state;
        rci_output_type_t type;
        rci_string_t * tag;
        rci_attribute_list_t * attribute;
        size_t attribute_pair_index;
        size_t entity_scan_index;
    } output;
    struct {
        rci_error_state_t state;
        rci_string_t tag;
        char const * description;
    } error;
    struct {
        union {
            rci_string_t generic;
            rci_string_t tag;
            rci_string_t content;
        } string;
        idigi_element_value_t value;
        idigi_remote_group_request_t request;
        idigi_remote_group_response_t response;
        rci_attribute_list_t attribute;
    } shared;
} rci_t;

#define RCI_NO_HINT             NULL
#define INVALID_ID              UINT_MAX
#define INVALID_INDEX           UINT_MAX

#define CSTR_LEN(p)     ((size_t) *(p))
#define CSTR_DATA(p)    ((p) + 1)

static void cstr_to_rci_string(char const * const cstr, rci_string_t * const rcistr)
{
    rcistr->data = CSTR_DATA(cstr);
    rcistr->length = CSTR_LEN(cstr);
}

static idigi_bool_t buffer_equals_buffer(char const * const str1, size_t const len1, char const * const str2, size_t const len2)
{
    ASSERT(len1 != 0);
    ASSERT(len2 != 0);
    
    return ((len1 == len2) && (memcmp(str1, str2, len1) == 0)) ? idigi_true : idigi_false;
}

static idigi_bool_t cstr_equals_buffer(char const * const cstr, char const * const str2, unsigned int const len2)
{
    return buffer_equals_buffer(CSTR_DATA(cstr), CSTR_LEN(cstr), str2, len2);
}

#if 0 /* currently unused - remove before shipping */
static idigi_bool_t cstr_equals_cstr(char const * const cstr1, char const * const cstr2)
{
    return buffer_equals_buffer(CSTR_DATA(cstr1), CSTR_LEN(cstr1), CSTR_DATA(cstr2), CSTR_LEN(cstr2));
}
#endif

static idigi_bool_t cstr_equals_rcistr(char const * const cstr, rci_string_t const * const rcistr)
{
    return cstr_equals_buffer(cstr, rcistr->data, rcistr->length);
}

static idigi_bool_t cstr_equals_str(char const * const cstr, char const * const str)
{
    return cstr_equals_buffer(cstr, str, strlen(str));
}

static idigi_bool_t rcistr_to_uint(rci_string_t const * const rcistr, unsigned int * const value)
{
    ASSERT(!isdigit(rcistr->data[rcistr->length]));

    return (sscanf(rcistr->data, "%u", value) == 1);
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
    ASSERT((buffer->current + amount) <= buffer->end);
    buffer->current += amount;
}

static int rci_buffer_read(rci_buffer_t const * const buffer)
{
    ASSERT(rci_buffer_remaining(buffer) != 0);
    
    return *(buffer->current);
}

static void rci_buffer_write(rci_buffer_t const * const buffer, int const value)
{
    ASSERT(rci_buffer_remaining(buffer) != 0);
    
    *(buffer->current) = value;
}

static idigi_bool_t ptr_in_buffer(rci_buffer_t const * const buffer, char const * const pointer)
{
    return ((pointer >= buffer->start) && (pointer <= buffer->end));
}

static void set_rcistr_length(rci_t const * const rci, rci_string_t * const string)
{
    ASSERT(string->length == 0);
    string->length = (rci->input.destination - string->data);
}

static void clear_rcistr(rci_string_t * const string)
{
    string->data = NULL;
    string->length = 0;
}

static void adjust_char_pointer(rci_t const * const rci, char const * const old_base, char * * const pointer)
{
    size_t const offset = (*pointer - old_base);
    char * new_base = (char *) rci->input.storage;
     
    *pointer = (new_base + offset);
}

static void adjust_rci_string(rci_t const * const rci, char const * const base, rci_string_t * const string)
{
    char * pointer = (char *) string->data;
    
    adjust_char_pointer(rci, base, &pointer);
    string->data = pointer;
}

static void rci_prep_reply(rci_t * const rci, rci_string_t * const tag, rci_attribute_list_t * const attribute)
{
    cstr_to_rci_string(RCI_REPLY, tag);

    attribute->count = 1;
    cstr_to_rci_string(RCI_VERSION, &attribute->pair[0].name);
    cstr_to_rci_string(RCI_VERSION_SUPPORTED, &attribute->pair[0].value);

    rci->output.tag = tag;
    rci->output.attribute = attribute;
    rci->output.type = rci_output_type_start_tag;
}


