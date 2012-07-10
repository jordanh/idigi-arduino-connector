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
  *  @brief Functions and prototypes for iDigi Connector
  *         user APIs for easier access to iDigi functionalities.
  *
  */
#ifndef IDIGI_CONNECTOR_H
#define IDIGI_CONNECTOR_H

#include <idigi_types.h>

typedef enum
{
    idigi_app_success,
    idigi_app_busy,
    idigi_app_unknown_target,
    idigi_app_invalid_parameter,
    idigi_app_resource_error
} idigi_app_error_t;

typedef enum
{
    idigi_connector_success,
    idigi_connector_reset,
    idigi_connector_init_error,
    idigi_connector_invalid_parameter,
    idigi_connector_already_registered,
    idigi_connector_resource_error,
    idigi_connector_event_error,
    idigi_connector_failed_to_create_thread,
    idigi_connector_network_error,
    idigi_connector_compression_error,
    idigi_connector_timeout,
    idigi_connector_session_error,
    idigi_connector_service_unavailable
} idigi_connector_error_t;

/**
 * This flag can be used with idigi_send_data(). The archive flag tells the iDigi Device Cloud to keep a history 
 * of changes to the file being pushed. So even if it is written over or deleted the user can 
 * query for old versions of the file.
 */
#define IDIGI_FLAG_ARCHIVE_DATA 0x01

/**
 * This flag is used in idigi_send_data(). The append flag is used to append the data to an 
 * existing file on the iDigi Device Cloud.
 */
#define IDIGI_FLAG_APPEND_DATA  0x02

/**
 * The last data flag is used to indicate the last chunk of the request/response data. The connector 
 * will use this flag to indicate the last reequest chunk and application should use this to 
 * indicate the last response.
 */
#define IDIGI_FLAG_LAST_DATA	0x10

/** 
 * Data structure to hold the application/cloud data along with the user context.
 *
 * Parameters:
 *   data_ptr -- pointer to application/cloud data
 *   length_in_bytes -- number of bytes filled/available to fill in data_ptr
 *   flags -- Indicates archive/append in idigi_send_data, otherwise it indicates the last chunk.
 *   app_context -- pointer to hold application specific context
 *   error -- encountered connector error if any, application has to check this value before 
 *            handling the data
 */
typedef struct idigi_connector_data_t
{
    void * data_ptr;
    size_t length_in_bytes;
    unsigned int flags;
    void * app_context;
    idigi_connector_error_t error;
}idigi_connector_data_t;

/**
 * Device request callback function.  It is the callbacks responsibility to determine if the 
 * device request target is valid. Application need to check the error value in the request_data 
 * before processing the request.
 *
 * Parameters:
 *    target -- NUL-terminated device request target name
 *    request_data -- pointer to the request info, which contain requested data, data length,
 *                    flag to indicate last request data, connector error if any,
 *                    and application context. The value updated in the application context
 *                    will be retained in subsequent callbacks for the same target.
 *
 * Return Value:
 *    idigi_app_success -- for success
 *    idigi_app_busy -- application is not ready to receive this request
 *    idigi_app_unknown_target -- target not supported
 *    idigi_app_resource_error -- failed to allocate required resource
 *
 */
typedef idigi_app_error_t (* idigi_device_request_callback_t)(char const * const target, idigi_connector_data_t * const request_data);

/**
 * Device response callback function.  It is the callbacks responsibility to determine if the 
 * target is valid. Application need to check the error value in the response_data before 
 * returning the response. 
 *  
 * Parameters:
 *    target -- NUL-terminated target name
 *    response_data -- pointer to the response info, which contain pointer to a buffer where user
 *                     can write the response, maximum buffer length, flag to indicate last response
 *                     data, connector error if any and the application context provided in the
 *                     first request callback.
 *
 * Return Value:
 *    -1 -- indicates error
 *    0  -- busy if last flag is not set
 *    0> -- indicates number of bytes copied to the response buffer, not more than max_response_bytes.
 *          The default max_response_bytes will be around 1590 bytes.
 *
 */
typedef size_t (* idigi_device_response_callback_t)(char const * const target, idigi_connector_data_t * const response_data);

/**
 * Registers device request and response callback functions to deliver the request
 * from the iDigi Device Cloud and to return the response to the iDigi Device Cloud.
 *
 * Parameters:
 *    request_callback -- called with request data from the iDigi Device Cloud
 *    response_callback -- called to get the response data to the iDigi Device Cloud
 *
 * Return Value:
 *    idigi_connector_success -- success
 *    idigi_connector_invalid_parameter -- NULL callback function
 *    idigi_connector_already_registered -- callback has already been registered 
 */
idigi_connector_error_t idigi_register_device_request_callbacks(idigi_device_request_callback_t request_callback, idigi_device_response_callback_t response_callback);

/**
 * This function will send data to the iDigi Device Cloud.  Note, this is a network blocking call.
 *
 * Parameters:
 *      path -- NUL-terminated file path where user wants to store the data on the iDigi Device Cloud
 *      device_data -- Will contain pointer to application data, data length in bytes, flags to indicate
 *                     whether append or archive is needed and application context (will be returned in subsequent callbacks)
 *      content_type -- NUL-terminated content type (text/plain, text/xml, application/json, etc). Pass NULL to let the iDigi Device Cloud determine
 *                      the type based on the file extension. In that case unsupported extensions will be treated as a binary data.
 *
 * Return Value:
 *      idigi_connector_success -- success
 *      idigi_connector_invalid_parameter -- Indicates bad parameters
 *      idigi_connector_cloud_error -- Indicates error response from the iDigi Device Cloud
 */
idigi_connector_error_t idigi_send_data(char const * const path, idigi_connector_data_t * const device_data, char const * const content_type);

/**
 * Callback routine data type, which is called when there is any asynchronous error or 
 * reset message from the cloud/connector. After returning this callback, the 
 * application can restart the iDigi connector calling idigi_connector_start() again.
 *
 * Parameters:
 *   status -- iDigi connector error value or reset
 *   status_message -- if available, this string explains more about the error
 *
 * Return value:
 *   none
 */
typedef void (* idigi_status_callback_t)(idigi_connector_error_t const status, char const * const status_message);

/**
 * Routine to start the iDigi connector. Any error during init 
 * may invoke the provided callback routine.  
 * 
 * Parameters: 
 *   status_callback -- to provide asynchronous status from iDigi connector
 * 
 * Return value: 
 *   idigi_connector_success -- iDigi connector started successfully
 *   idigi_connector_init_error -- Failed to initialize iDigi connector
 *   idigi_connector_resource_error -- Failed to create iDigi connector thread
 *
 */
idigi_connector_error_t idigi_connector_start(idigi_status_callback_t status_callback);

/**
* @}.
*/
#endif

