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

#include <stdarg.h>
#if defined IDIGI_DEBUG
#include <ctype.h>
#endif
#define IDIGI_RCI_PARSER_INTERNAL_DATA
#include "remote_config.h"

#define RCI_NO_HINT             NULL
#define INVALID_ID              UINT_MAX
#define INVALID_INDEX           UINT_MAX

#define ROUND_UP(value, interval)   ((value) + -(value) % (interval))

#if defined IDIGI_DEBUG
#define UNHANDLED_CASES_ARE_NEEDED
#else
#define UNHANDLED_CASES_ARE_NEEDED  default: break;
#endif
#define UNHANDLED_CASES_ARE_INVALID default: ASSERT(idigi_false); break;

static char const nul = '\0';

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
    rci_status_error            /* error occurred, RCI service should inform messaging layer to cancel the session */
} rci_status_t;

typedef struct
{
    char * data;
    size_t bytes;
} rci_service_buffer_t;

typedef struct
{
    idigi_data_t * idigi_ptr;
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
	rci_input_state_content_first,
	rci_input_state_content,
	rci_input_state_content_escaping,
	rci_input_state_comment
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
    rci_traversal_state_all_groups_group_advance,
    rci_traversal_state_all_groups_end,
    rci_traversal_state_one_group_start,
    rci_traversal_state_one_group_element_start,
    rci_traversal_state_one_group_element_data,
    rci_traversal_state_one_group_element_end,
    rci_traversal_state_one_group_end,
    rci_traversal_state_one_group_advance,
    rci_traversal_state_indexed_group_start,
    rci_traversal_state_indexed_group_element_start,
    rci_traversal_state_indexed_group_element_data,
    rci_traversal_state_indexed_group_element_end,
    rci_traversal_state_indexed_group_end,
    rci_traversal_state_indexed_group_advance,
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
	rci_output_state_element_param_end_quote,
	rci_output_state_content_formatted,
	rci_output_state_content,
	rci_output_state_content_scan,
	rci_output_state_content_entity,
	rci_output_state_content_semicolon
} rci_output_state_t;

typedef enum
{
    rci_error_state_none,
    rci_error_state_error_open,
    rci_error_state_error_description,
    rci_error_state_error_hint,
    rci_error_state_error_close,
    rci_error_state_element_close,
    rci_error_state_group_close,
    rci_error_state_command_close,
    rci_error_state_reply_close,
} rci_error_state_t;

typedef enum
{
    rci_output_type_start_tag,
    rci_output_type_content,
    rci_output_type_content_formatted,
    rci_output_type_end_tag,
    rci_output_type_unary,
    rci_output_type_three_tuple,
    rci_output_type_three_tuple_formatted
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
} rcistr_t;

typedef struct
{
    rcistr_t name;
    rcistr_t value;
} rci_attribute_t;

typedef struct
{
    rci_service_data_t * service_data;
    rci_status_t status;
    struct {
        idigi_request_t request;
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
        unsigned int hyphens;
        int character;
        char * destination;
        rci_command_t command;
        struct {
            rci_attribute_t match;
            rci_attribute_t current;
        } attribute;
        rcistr_t entity;
        char storage[ROUND_UP(IDIGI_RCI_MAXIMUM_CONTENT_LENGTH + sizeof nul, sizeof (int))];
    } input;
    struct {
        rci_traversal_state_t state;
    } traversal;
    struct {
        rci_output_state_t state;
        rci_output_type_t type;
        rcistr_t tag;
        rci_attribute_t attribute;
        rcistr_t content;
        rci_output_type_t current;
        size_t entity_scan_index;
    } output;
    struct {
        rci_error_state_t state;
        char const * description;
    } error;
    struct {
        union {
            rcistr_t generic;
            rcistr_t tag;
            rcistr_t content;
        } string;
        struct {
            struct {
                unsigned int id;
                unsigned int index;
            } group;
            struct {
                unsigned int id;
            } element;
        } current;
        idigi_element_value_t value;
        idigi_remote_group_request_t request;
        idigi_remote_group_response_t response;
    } shared;
} rci_t;

