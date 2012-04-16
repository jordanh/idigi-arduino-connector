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
#include "idigi_types.h"

#ifndef _IDIGI_REMOTE_H
#define _IDIGI_REMOTE_H

typedef enum {
    idigi_remote_action_set,
    idigi_remote_action_query
} idigi_remote_action_t;


typedef enum {
    idigi_remote_group_setting,
    idigi_remote_group_state
} idigi_remote_group_type_t;


typedef enum {
    idigi_element_type_string,
    idigi_element_type_multiline_string,
    idigi_element_type_password,
    idigi_element_type_int32,
    idigi_element_type_uint32,
    idigi_element_type_hex32,
    idigi_element_type_0xhex,
    idigi_element_type_float,
    idigi_element_type_enum,
    idigi_element_type_on_off,
    idigi_element_type_boolean,
    idigi_element_type_ipv4,
    idigi_element_type_fqdnv4,
    idigi_element_type_fqdnv6,
    idigi_element_type_datetime
} idigi_element_value_type_t;

typedef enum {
    idigi_off,
    idigi_on
} idigi_on_off_t;

typedef enum {
    idigi_boolean_false,
    idigi_boolean_true,
    idigi_boolean_integer_width = INT_MAX
} idigi_boolean_t;

typedef enum {
    idigi_element_access_read_only,
    idigi_element_access_write_only,
    idigi_element_access_read_write
} idigi_element_access_t;

typedef struct {
    int32_t min_value;
    int32_t max_value;
} idigi_element_value_signed_integer_t;


typedef struct {
    uint32_t min_value;
    uint32_t max_value;
} idigi_element_value_unsigned_integer_t;


typedef struct {
    size_t min_length_in_bytes;
    size_t max_length_in_bytes;
} idigi_element_value_string_t;

typedef struct {
    size_t count;
    char const * const * value;
} idigi_element_value_enum_t;

typedef struct {
    float min_value;
    float max_value;
} idigi_element_value_float_t;

typedef union {
    idigi_element_value_unsigned_integer_t integer_unsigned_value;
    idigi_element_value_signed_integer_t integer_signed_value;
    idigi_element_value_string_t string_value;
    idigi_element_value_enum_t enum_value;
    idigi_element_value_float_t float_value;
} idigi_element_value_limit_t;

typedef struct {
    char const * name;
    idigi_element_access_t access;
    idigi_element_value_type_t type;
    idigi_element_value_limit_t const * const value_limit;
} idigi_group_element_t;

/* group structure */
typedef struct {
    char const * name;
    size_t instances;
    struct {
        size_t count;
        idigi_group_element_t const * const data;
    } elements;

    struct {
        size_t count;
        char const * const * description;
    } errors;
} idigi_group_t;


typedef union {
    struct {
        char * buffer;
        size_t length_in_bytes;
    } string_value;
    int32_t integer_signed_value;
    uint32_t integer_unsigned_value;
    float float_value;
    unsigned int enum_value;
    idigi_on_off_t  on_off_value;
    idigi_boolean_t boolean_value;
} idigi_element_value_t;

typedef struct {
    idigi_remote_action_t action;
    struct {
        idigi_remote_group_type_t type;
        unsigned int id;
        unsigned int index;
    } group;
    struct {
        unsigned int id;
        idigi_element_value_type_t type;
        idigi_element_value_t * value;
    } element;
} idigi_remote_group_request_t;

typedef struct {
    void * user_context;
    unsigned int error_id;
    union {
        char const * error_hint;
        idigi_element_value_t * element_value;
    } element_data;
} idigi_remote_group_response_t;

typedef struct {
    idigi_group_t const * groups;
    size_t count;
} idigi_group_table_t;

extern idigi_group_table_t const idigi_group_table[];
extern char const * const idigi_rci_errors[];


#endif /* idigi_element_H_ */
