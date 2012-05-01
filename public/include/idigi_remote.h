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

 /**
 * @defgroup idigi_remote_action_t iDigi remote configuration action types
 * @{
 */
 /**
 * Remote request action types
 */
typedef enum {
    idigi_remote_action_set,    /**< Set remote configuration */
    idigi_remote_action_query   /**< Query remote configuration */
} idigi_remote_action_t;
/**
* @}
*/

/**
* @defgroup idigi_remote_group_type_t iDigi remote configuration types
* @{
*/
/**
* Remote request group types.
*/
typedef enum {
    idigi_remote_group_setting, /**< Setting configuration */
    idigi_remote_group_state    /**< State configuration */
} idigi_remote_group_type_t;

/**
* @}
*/

/**
* @defgroup idigi_element_value_type_t idigi remote configuration element types
* @{
*/
/**
* Remote configuration element value types
*/
typedef enum {
    idigi_element_type_string,              /**< Single line string */
    idigi_element_type_multiline_string,    /**< This type is used for a string that may be more
                                               than one line. A “text area” is shown in the iDigi Device
                                               Cloud that allows a user to form multiple lines by pressing enter. */
    idigi_element_type_password,            /**< This Password type is same as string type except it’s shown “*”
                                                when a user types in characters in the iDigi Device Cloud. */
    idigi_element_type_int32,               /**< 32-bit signed integer value */
    idigi_element_type_uint32,              /**< 32-bit unsigned integer value */
    idigi_element_type_hex32,               /**< 32-bit unsigned hexadecimal */
    idigi_element_type_0xhex,               /**< 32-bit unsigned hexadecimal with 0x prefix shown in iDigi Device Cloud.*/
    idigi_element_type_float,               /** Floating value. float.h is included. */
    idigi_element_type_enum,                /**< enum is used to define a set of allowed values for an element.
                                                This is a pull-down menu shown in iDigi Device Cloud. */
    idigi_element_type_on_off,              /**< “on” or “off “ value. An applciation's callback should use
                                                 idigi_on value for “on”  or idigi_off value for “off”. */
    idigi_element_type_boolean,             /**< true or false.An application's callback should use  idigi_true
                                                value for true  or idigi_false value for false. */

    idigi_element_type_ipv4,                /**< Valid IPv4 address (32-bit value) which is shown aaa.bbb.ccc.ddd in iDigi Device Cloud */
    idigi_element_type_fqdnv4,              /**< This type accepts either ipv4 or DNS name. */
    idigi_element_type_fqdnv6,              /**< This type accepts either IPv6 address, IPv4 address, or DNS name. */
    idigi_element_type_datetime             /**< Date and time type. It’s string which contains the ISO 8601 standard
                                                 for date and time representation. The format is: YYYY-MM-DDTHH:MM:SStz
                                                 where
                                                     @htmlonly
                                                     <table class="apitable">
                                                     <tr><td>YYYY:</td> <td>Year<td></tr>
                                                     <tr><td>MM:</td> <td>Month<td></tr>
                                                     <tr><td>DD:</td> <td>Day<td></tr>
                                                     <tr><td>T:</td> <td>The separator between date and time<td></tr>
                                                     <tr><td>HH:</td> <td>Hours in 24-hour clock<td></tr>
                                                     <tr><td>MM:</td> <td>Minutes<td></tr>
                                                     <tr><td>SS:</td> <td>Seconds<td></tr>
                                                     <tr><td>tz:</td> <td>Time zone, specified either in the
                                                         form [+,-]HHMM or Z for Coordinated Universal Time (UTC) <td></tr>
                                                     </table>
                                                    @endhtmlonly */
} idigi_element_value_type_t;
/**
* @}
*/

/**
* @defgroup idigi_on_off_t  iDigi on or off values
* @{
*/
/**
* iDigi ON and OFF value types
*/
typedef enum {
    idigi_off,  /**< OFF */
    idigi_on    /**< ON */
} idigi_on_off_t;
/**
* @}
*/

/**
* @defgroup idigi_boolean_t iDigi boolean type
* @{
*/
/**
* Boolean type
*/
typedef enum {
    idigi_boolean_false,    /**< False */
    idigi_boolean_true      /**< True */
} idigi_boolean_t;
/**
* @}
*/

/**
* @defgroup idigi_element_access_t iDigi remote configuration access types
* @{
*/
/**
* Remote Configuration Element Access types
*/
typedef enum {
    idigi_element_access_read_only,     /**< Read only */
    idigi_element_access_write_only,    /**< Write only */
    idigi_element_access_read_write     /**< Read and write */
} idigi_element_access_t;
/**
* @}
*/

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

#if defined(RCI_PARSER_USES_ENUMERATIONS)
typedef struct {
    size_t count;
    char const * const * value;
} idigi_element_value_enum_t;
#endif

#if defined(RCI_PARSER_USES_FLOATING_POINT)
typedef struct {
    double min_value;
    double max_value;
} idigi_element_value_float_t;
#endif

typedef union {
    idigi_element_value_unsigned_integer_t integer_unsigned_value;
    idigi_element_value_signed_integer_t integer_signed_value;
    idigi_element_value_string_t string_value;
#if defined(RCI_PARSER_USES_ENUMERATIONS)
    idigi_element_value_enum_t enum_value;
#endif
#if defined(RCI_PARSER_USES_FLOATING_POINT)
    idigi_element_value_float_t float_value;
#endif
} idigi_element_value_limit_t;

typedef struct {
    char const * name;
    idigi_element_access_t access;
    idigi_element_value_type_t type;
    idigi_element_value_limit_t const * const value_limit;
} idigi_group_element_t;

typedef struct {
    char const * name;
    size_t instances;
    struct {
        size_t count;
        idigi_group_element_t const * const data;
    } elements;

    struct {
        size_t count;       /**< Number of errors in a remote configuration */
        char const * const * description;
    } errors;
} idigi_group_t;

#if ENABLE_COMPILE_TIME_DATA_PASSING
/**
* @defgroup idigi_element_value_t iDigi remote configuration element value union
* @{
*/
/**
* Remote Configuration Element value
*/
typedef union {
    char const * string_value;          /**< Pointer to a nul-terminated string for the following value types:
                                             @htmlonly
                                             <ul>
                                             <li>idigi_element_type_string</li>
                                             <li>idigi_element_type_multiline_string</li>
                                             <li>idigi_element_type_password</li>
                                             <li>idigi_element_type_ipv4</li>
                                             <li>idigi_element_type_fqdnv4</li>
                                             <li>idigi_element_type_fqdnv6</li>
                                             <li>idigi_element_type_datetime</li>
                                             </ul>
                                             @endhtmlonly
                                             Note: An application's callback must return the address of the pointer
                                             to the string for idigi_remote_action_set.
                                         */
    int32_t integer_signed_value;       /**< Signed integer value for idigi_element_type_int32 value type */
    uint32_t integer_unsigned_value;    /**< Unsigned integer value for the following value types:
                                             @htmlonly
                                             <ul>
                                             <li>idigi_element_type_uint32</li>
                                             <li>idigi_element_type_hex32</li>
                                             <li>idigi_element_type_0xhex32</li>
                                             </ul>
                                             @endhtmlonly
                                           */
    double float_value;                 /**< Floating point value for idigi_element_type_float value type */
    unsigned int enum_value;            /**< Enumeration value for idigi_element_type_enum value type */
    idigi_on_off_t  on_off_value;       /**< idigi_on for on or idigi_off for Off value for idigi_element_type_on_off value type */
    idigi_boolean_t boolean_value;      /**< idigi_true for true or idigi_false for false value for idigi_element_type_boolean value type */
} idigi_element_value_t;
/**
* @}
*/

#endif /* #if ENABLE_COMPILE_TIME_DATA_PASSING */

typedef union {
#if defined(RCI_PARSER_USES_STRING)
    char const * string_value;
#endif

#if defined(RCI_PARSER_USES_SIGNED_INTEGER)
    int32_t integer_signed_value;
#endif

#if defined(RCI_PARSER_USES_UNSIGNED_INTEGER)
    uint32_t integer_unsigned_value;
#endif

#if defined(RCI_PARSER_USES_FLOATING_POINT)
    double float_value;
#endif

#if defined(RCI_PARSER_USES_ENUMERATIONS)
    unsigned int enum_value;
#endif

    idigi_on_off_t  on_off_value;
    idigi_boolean_t boolean_value;
} idigi_element_value_t;

/**
* @defgroup idigi_remote_group_request_t iDigi remote configuration request structure
* @{
*/
/**
* Remote configuration request
*/
typedef struct {
    idigi_remote_action_t action;   /**< idigi_remote_action_set for setting remote configuration or
                                         idigi_remote_action_query for querying remote configuration */
    struct {
        idigi_remote_group_type_t type; /**< Type of the remote configuration group:
                                             @htmlonly
                                             <ul>
                                             <li>idigi_remote_group_setting for setting configuration</li>
                                             <li>idigi_remote_group_state for state configuration</li>
                                             </ul>
                                             @endhtmlonly
                                           */
        unsigned int id;                /**< An enumeration value of the remote configuration group */
        unsigned int index;             /**< Index of the remote configuration group */
    } group;
    struct {
        unsigned int id;                /**< An enumeration value of the element */
        idigi_element_value_type_t type;/**< Value type of the element */
        idigi_element_value_t * value;  /**< Pointer to memory which contains the element value to be
                                             set for idigi_remote_action_set action */
    } element;
} idigi_remote_group_request_t;
/**
* @}
*/

/**
* @defgroup idigi_remote_group_response_t iDigi remote configuration response structure
* @{
*/
/**
* Remote configuration reponse
*/
typedef struct {
    void * user_context;        /**< Pointer to callback's context returned from previous callback call.
                                     Callback may write its own context which will be passed back to subsequential callback.*/
    unsigned int error_id;      /**< Callback writes error enumeration value if error is encountered */
    union {
        char const * error_hint;    /**< Callback returns a pointer to a constant nul-terminated hit string which will be sent
                                         to the iDigi Cloud if error is encountered. Error description is sent if it's provided
                                         for the given error_id.
                                         Note: this string cannot be altered until next callback call.
                                      */
        idigi_element_value_t * element_value; /**< Pointer to memory where callback write the element value */
    } element_data;
} idigi_remote_group_response_t;
/**
* @}
*/

typedef struct {
    idigi_group_t const * groups;
    size_t count;
} idigi_group_table_t;

extern idigi_group_table_t const idigi_group_table[];
extern char const * const idigi_rci_errors[];


#endif /* idigi_element_H_ */
