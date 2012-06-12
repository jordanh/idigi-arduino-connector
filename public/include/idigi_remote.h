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
/**
 * @file
 *  @brief iDigi connector remote configuration definitions and structures
 *
 */
#ifndef _IDIGI_REMOTE_H
#define _IDIGI_REMOTE_H

#include "idigi_types.h"

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
                                               than one line. A "text area" is shown in the iDigi Device
                                               Cloud that allows a user to form multiple lines by pressing enter. */
    idigi_element_type_password,            /**< This Password type is same as string type except it’s shown "*"
                                                when a user types in characters in the iDigi Device Cloud. */
    idigi_element_type_int32,               /**< 32-bit signed integer value */
    idigi_element_type_uint32,              /**< 32-bit unsigned integer value */
    idigi_element_type_hex32,               /**< 32-bit unsigned hexadecimal */
    idigi_element_type_0xhex,               /**< 32-bit unsigned hexadecimal with 0x prefix shown in iDigi Device Cloud.*/
    idigi_element_type_float,               /**< Floating value. float.h is included. */
    idigi_element_type_enum,                /**< enum is used to define a set of allowed values for an element.
                                                This is a pull-down menu shown in iDigi Device Cloud. */
    idigi_element_type_on_off,              /**< "on" or "off" value. An applciation's callback should use
                                                 idigi_on value for "on"  or idigi_off value for "off". See @ref idigi_on_off_t*/
    idigi_element_type_boolean,             /**< true or false value. An application's callback should use  idigi_boolean_true
                                                value for true  or idigi_boolean_false value for false. See @ref idigi_boolean_t */

    idigi_element_type_ipv4,                /**< Valid IPv4 address (32-bit value) which is shown aaa.bbb.ccc.ddd in iDigi Device Cloud */
    idigi_element_type_fqdnv4,              /**< This type accepts either ipv4 or DNS name. */
    idigi_element_type_fqdnv6,              /**< This type accepts either IPv6 address, IPv4 address, or DNS name. */
    idigi_element_type_datetime             /**< Date and time type. It’s string which contains the ISO 8601 standard
                                                 for date and time representation. The format is: YYYY-MM-DDTHH:MM:SStz
                                                 where
                                                     @htmlonly
                                                     <table>
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


/**
* @defgroup idigi_remote_group_t iDigi remote configuration group
* @{
*/
/**
* Remote configuration group
*/
typedef struct {
    idigi_remote_group_type_t type; /**< Type of the remote configuration group:
                                         @htmlonly
                                         <ul>
                                         <li>@endhtmlonly @ref idigi_remote_group_setting @htmlonly for setting configuration</li>
                                         <li>@endhtmlonly @ref idigi_remote_group_state @htmlonly for state configuration</li>
                                         </ul>
                                         @endhtmlonly
                                       */
    unsigned int id;                /**< An enumeration value of the remote configuration group */
    unsigned int index;             /**< Index of the remote configuration group */
} idigi_remote_group_t;
/**
* @}
*/

/**
* @defgroup idigi_remote_element_t iDigi remote configuration element
* @{
*/
/**
* Remote configuration element
*/
typedef struct {
    unsigned int id;                /**< An enumeration value of the element */
    idigi_element_value_type_t type;/**< Value type of the element */
    idigi_element_value_t * value;  /**< Pointer to memory which contains the element value to be
                                         set for @ref idigi_remote_action_set action */
} idigi_remote_element_t;
/**
* @}
*/

/**
* @defgroup idigi_remote_group_request_t iDigi remote configuration request structure
* @{
*/
/**
* Remote configuration request
*/
typedef struct {
    idigi_remote_action_t action;   /**< @htmlonly <ul><li> @endhtmlonly @ref idigi_remote_action_set @htmlonly for setting remote configuration or</li>
                                         <li> @endhtmlonly @ref idigi_remote_action_query @htmlonly for querying remote configuration </li></ul> @endhtmlonly */
    idigi_remote_group_t group;     /**< Group configuration to be accessed */
    idigi_remote_element_t element; /**< Element of the group configuration */
} idigi_remote_group_request_t;
/**
* @}
*/

/**
* @defgroup idigi_remote_element_response_t iDigi remote configuration element response union
* @{
*/
/**
* Remote configuration element response
*/
typedef union {
    char const * error_hint;    /**< Callback returns a pointer to a constant nul-terminated string which will be sent
                                     to the iDigi Cloud if error is encountered.
                                     Note: this string cannot be altered until next callback call.
                                  */
    idigi_element_value_t * element_value; /**< Pointer to memory where callback write the element value */
} idigi_remote_element_response_t;
/**
* @}
*/

/**
* @defgroup idigi_remote_group_response_t iDigi remote configuration response structure
* @{
*/
/**
* Remote configuration response
*/
typedef struct {
    void * user_context;        /**< Pointer to callback's context returned from previous callback call.
                                     Callback may write its own context which will be passed back to subsequential callback.*/
    unsigned int error_id;      /**< Callback writes error enumeration value if error is encountered.
                                     Error description is sent if it's provided for the given error_id. */
    idigi_remote_element_response_t element_data;  /**< Element response data */
} idigi_remote_group_response_t;
/**
* @}
*/

typedef struct {
    idigi_group_t const * groups;
    size_t count;
} idigi_group_table_t;

#endif /* idigi_element_H_ */
