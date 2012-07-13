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
  *  @brief Functions and prototypes for iDigi Connector Kits.
  *
  */
#ifndef IDIGI_CONNECTOR_H
#define IDIGI_CONNECTOR_H

#include <idigi_types.h>


 /** 
 * Status values returned by application to iDigi connector API 
 * calls. 
 */
typedef enum
{
    idigi_app_success,              /**< No error. */
    idigi_app_busy,                 /**< Application busy. */
    idigi_app_unknown_target,       /**< Invalid target. */
    idigi_app_invalid_parameter,    /**< Invalid parameter. */
    idigi_app_resource_error        /**< Lack of resources */
} idigi_app_error_t;


 /** 
 * Status returned by iDigi connector API calls.
 */
typedef enum
{
    idigi_connector_success,                    /**< No error. */
    idigi_connector_reset,                      /**< Connection reset. */
    idigi_connector_init_error,                 /**< Initialization error. */
    idigi_connector_invalid_parameter,          /**< Invalid parameter. */
    idigi_connector_already_registered,         /**< Device already registered. */
    idigi_connector_resource_error,             /**< Lack of resources. */
    idigi_connector_event_error,                /**< OS event error. */
    idigi_connector_failed_to_create_thread,    /**< OS could not create thread. */
    idigi_connector_network_error,              /**< General network error. */
    idigi_connector_compression_error,          /**< Error compressing data. */
    idigi_connector_timeout,                    /**< Connection timeout. */
    idigi_connector_session_error,              /**< iDigi session error. */
    idigi_connector_service_unavailable         /**< Session unavailable. */
} idigi_connector_error_t;

 /**
 * @defgroup idigi_send_data_flag_definitions iDigi Send Data Flag Definitions
 * @{ 
 */
/**
 * This flag can be used with idigi_send_data(). The overwrite flag tells the iDigi Device Cloud to overwrite 
 * the existing file on the iDigi Device Cloud..
 */
#define IDIGI_FLAG_OVERWRITE_DATA 0x00

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
* @}
*/

/**
 * The last data flag is used to indicate the last chunk of the request/response data. The connector 
 * will use this flag to indicate the last reequest chunk and application should use this to 
 * indicate the last response.
 */
#define IDIGI_FLAG_LAST_DATA	0x10

/** 
 * Data structure to hold the application/cloud data along with the user context.            
 */
typedef struct idigi_connector_data_t
{
    void * data_ptr;               /**< pointer to application/cloud data */
    size_t length_in_bytes;        /**< number of bytes filled/available to fill in data_ptr */
    unsigned int flags;            /**< Indicates archive/append in idigi_send_data, otherwise it indicates the last chunk */
    void * app_context;            /**< pointer to hold application specific context */
    idigi_connector_error_t error; /**< encountered connector error if any, application has to check this value before handling the data */
}idigi_connector_data_t;

/**
 * @defgroup device_request_callback User defined device request callback function.
 * @{
 * @b Include: idigi_connector.h
 */
/**
 *
 * This function needs to determine if the target and the data associated with that target is valid.
 * If both are valid, this function should take the appropriate actions.
 *
 * @param target       NUL-terminated device request target name
 * @param request_data pointer to the request info, which contain requested data, data length,
 *                     flag to indicate last request data, connector error if any,
 *                     and application context. The value updated in the application context
 *                     will be retained in subsequent callbacks for the same target.
 *
 * @retval idigi_app_success        for success
 * @retval idigi_app_busy           application is not ready to receive this request
 * @retval idigi_app_unknown_target target not supported
 * @retval idigi_app_resource_error failed to allocate required resource
 *
 * Example Usage:
 * @code
 *    #define BUFFER_SIZE 256
 *    static char buffer[BUFFER_SIZE];
 *    idigi_app_error_t status=idigi_app_invalid_parameter;
 *
 *    if (request_data->error != idigi_connector_success)
 *    {
 *        APP_DEBUG("device_request_callback: error [%d]\n", request_data->error);
 *        goto error;
 *    }
 *
 *    if (request_data->length_in_bytes < sizeof buffer)
 *    {
 *        memcpy(buffer, request_data->data_ptr, request_data->length_in_bytes);
 *        buffer[request_data->length_in_bytes] = 0;
 *    }
 *    else
 *    {
 *        APP_DEBUG("device_request_callback: received more data than expected [%d]\n", request_data->length_in_bytes);
 *        goto error;
 *    }
 *
 *    APP_DEBUG("device_request_callback: received [%s] on target [%s]\n", buffer, target);
 *    status = idigi_app_success;
 *
 * error:
 *    return status;
 * @endcode
 *
 */
typedef idigi_app_error_t (* idigi_device_request_callback_t)(char const * const target, idigi_connector_data_t * const request_data);
/**
* @}
*/

/**
 * @defgroup device_response_callback User defined device response callback function.
 * @{
 * @b Include: idigi_connector.h
 */
/**
 * 
 * This function checks for the result of the device request callback function, and sets up the
 * response_data based on that result.  The Application needs to check the error value in the
 * response_data before returning the response.
 *
 * @param target        NUL-terminated target name
 * @param response_data pointer to the response info, which contain pointer to a buffer where user
 *                      can write the response, maximum buffer length, flag to indicate last response
 *                      data, connector error if any and the application context provided in the
 *                      first request callback.
 *
 * @retval -1 indicates error
 * @retval 0  busy if last flag is not set
 * @retval 0> indicates number of bytes copied to the response buffer, not more than max_response_bytes.
 *            The default max_response_bytes will be around 1590 bytes.
 *
 * Example Usage:
 * @code
 *    static char rsp_string[] = "iDigi Connector device response!\n";
 *    size_t const len = sizeof rsp_string - 1;
 *    size_t const bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
 *
 *    if (response_data->error != idigi_connector_success)
 *    {
 *        APP_DEBUG("device_response_callback: error [%d]\n", response_data->error);
 *        goto error;
 *    }
 *
 *    memcpy(response_data->data_ptr, rsp_string, bytes_to_copy);
 *    response_data->flags = IDIGI_FLAG_LAST_DATA;
 *
 *    APP_DEBUG("device_response_callback: target [%s], data- %s\n", target, rsp_string);
 *
 * error:
 *    return bytes_to_copy;
 * @endcode
 *
 */
typedef size_t (* idigi_device_response_callback_t)(char const * const target, idigi_connector_data_t * const response_data);
/**
* @}
*/

/**
 * @defgroup idigi_register_device_request_callbacks Registers the device request callbacks.
 * @{ 
 * @b Include: idigi_connector.h
 */
/**
 *
 * This function Registers device request and response callback functions to deliver the
 * request from the iDigi Device Cloud and to return the response to the iDigi Device Cloud.
 *
 * @param request_callback called with request data from the iDigi Device Cloud
 * @param response_callback called to get the response data to the iDigi Device Cloud
 *
 * @retval idigi_connector_success success
 * @retval idigi_connector_invalid_parameter NULL callback function
 * @retval idigi_connector_already_registered callback has already been registered
 *
 * Example Usage:
 * @code
 *    idigi_connector_error_t ret;
 *
 *    APP_DEBUG("application_start: calling idigi_register_device_request_callbacks\n");
 *    ret = idigi_register_device_request_callbacks(device_request_callback, device_response_callback);
 *    if (ret != idigi_connector_success)
 *    {
 *        APP_DEBUG("idigi_register_device_request_callbacks failed [%d]\n", ret);
 *    }
 * @endcode 
 */
idigi_connector_error_t idigi_register_device_request_callbacks(idigi_device_request_callback_t request_callback, idigi_device_response_callback_t response_callback);
/**
* @}
*/

/**
 * @defgroup idigi_send_data Send data to a file on the iDigi Device Cloud.
 * @{ 
 * @b Include: idigi_connector.h
 */
/** 
 *
 * This function will attempt to send data to a file on the iDigi Device Cloud.  Note, this is a network blocking call.
 *
 * @param path NUL terminated file path where user wants to store the data on the iDigi Device Cloud
 * @param device_data Will contain pointer to application data, data length in bytes, flags to indicate
 *                    whether append or archive is needed and application context (will be returned in subsequent callbacks)
 * @param content_type NUL-terminated content type (text/plain, text/xml, application/json, etc). Pass NULL to let the iDigi Device Cloud determine
 *                     the type based on the file extension. In that case unsupported extensions will be treated as a binary data.
 *
 * @retval idigi_connector_success success
 * @retval idigi_connector_invalid_parameter Indicates bad parameters
 * @retval idigi_connector_cloud_error Indicates error response from the iDigi Device Cloud
 *
 * Example Usage:
 * @code
 *    idigi_connector_error_t ret;
 *    char content_type[] = "text/plain";
 *    char path[] = 'test.txt";
 *    char buffer[] = "This is a test";
 *    unsigned int flags = 0; // 0 = overwrite file
 *
 *    do
 *    {
 *        static idigi_connector_data_t device_data = {0};
 *
 *        device_data_data.data_ptr = buffer;
 *        device_data_data.length_in_bytes = strlen(buffer);
 *        device_data_data.flags = flags;
 *
 *        ret = idigi_send_data(path, &device_data, content_type);
 *        if (ret == idigi_connector_init_error)
 *        {
 *            #define WAIT_FOR_A_SECOND  1
 *            app_os_sleep(WAIT_FOR_A_SECOND);
 *        }
 *
 *    } while (ret == idigi_connector_init_error);
 * @endcode
 */
idigi_connector_error_t idigi_send_data(char const * const path, idigi_connector_data_t * const device_data, char const * const content_type);
/**
* @}
*/

/**
 * @defgroup idigi_status_callback_t Application-defined status_callback
 *@{ 
 * idigi_status_callback_t: iDigi connector Application-defined status_callback, this is the 
 * callback used when there is any asynchronous error or reset message from
 * the cloud/connector. After returning this callback, the application can restart the
 * iDigi connector by calling idigi_connector_start().
 *
 */
/** 
 * idigi_status_callback_t. 
 *
 * @param status iDigi connector error value or reset
 * @param status_message if available, this string explains more about the error
 *
 * @retval none
 */
typedef void (* idigi_status_callback_t)(idigi_connector_error_t const status, char const * const status_message);
/**
* @}
*/

/**
 * @defgroup idigi_connector_start Start the iDigi connector.
 * @{ 
 * @b Include: idigi_connector.h
 */
/** 
 *
 * This function will to try to start the iDigi connector.  Any error during init may
 * invoke the provided callback routine.  
 *  
 * @param status_callback to provide asynchronous status from iDigi connector
 *  
 * @retval idigi_connector_success                 iDigi connector started successfully
 * @retval idigi_connector_failed_to_create_thread Failed to create iDigi connector thread
 * @retval idigi_connector_event_error             Failed to create iDigi connector event
 *
 * Example Usage:
 * @code
 *    idigi_connector_error_t result = idigi_connector_start(status_callback);
 * @endcode
 *  
 * @see idigi_status_callback_t
 *
 */
idigi_connector_error_t idigi_connector_start(idigi_status_callback_t status_callback);
/**
* @}
*/

/**
* @}.
*/
#endif

