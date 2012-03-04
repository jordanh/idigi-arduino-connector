/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
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

#define asizeof(array)  (sizeof array/sizeof array[0])
/**
* @defgroup idigi_remote_request_t Remote Configuration Requests
* @{
*/
/**
* Remote Configuration Request Id passed to the application's callback to query or set remote configuration data.
* The class id for this idigi_remote_request_t is idigi_class_remote_config.
*/

typedef enum {
    idigi_remote_session_start,
    idigi_remote_session_end,
    idigi_remote_action_start,
    idigi_remote_action_end,
    idigi_remote_group_start,
    idigi_remote_group_end,
    idigi_remote_group_process,
    idigi_remote_session_cancel     /**< Requesting callback to abort and cancel any query or set remote configuration request.
                                     Callback should stop and release any resources used for this remote configuration request */
} idigi_remote_request_t;
/**
* @}
*/

typedef enum {
    idigi_remote_action_set,
    idigi_remote_action_query
} idigi_remote_action_t;

typedef enum {
    idigi_remote_group_config,
    idigi_remote_group_sysinfo
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
    char * name;
    idigi_element_access_t access;
    idigi_element_value_type_t type;
    idigi_element_value_limit_t * const value_limit;
} idigi_group_element_t;

/* group structure */
typedef struct {
    char * name;
    size_t start_index;
    size_t end_index;
    struct {
        size_t count;
        idigi_group_element_t * const data;
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
} idigi_element_value_t;

typedef struct {
    void * user_context;
    idigi_remote_action_t action;
    idigi_remote_group_type_t config_type;
    unsigned int group_id;
    unsigned int group_index;
    idigi_remote_request_t * request;
    unsigned int element_id;
    idigi_element_value_type_t element_type;
    idigi_element_value_t element_value;
} idigi_remote_group_request_t;

typedef struct {
    unsigned int error_id;
    union {
        char * error_hint;
        idigi_element_value_t element_value;
    } result;
} idigi_remote_group_response_t;

#endif /* idigi_element_H_ */
