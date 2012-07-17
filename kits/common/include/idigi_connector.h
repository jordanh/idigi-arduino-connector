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
 * the existing file on the iDigi Device Cloud.
 */
#define IDIGI_FLAG_OVERWRITE_DATA 0x00

/**
 * This flag can be used with idigi_send_data(). The archive flag tells the iDigi Device Cloud to keep a history 
 * of changes to the file being pushed. So even if it is written over or deleted the user can 
 * query for old versions of the file.
 */
#define IDIGI_FLAG_ARCHIVE_DATA 0x01

/**
 * This flag can be used with idigi_send_data(). The append flag is used to append the data to an 
 * existing file on the iDigi Device Cloud.
 */
#define IDIGI_FLAG_APPEND_DATA  0x02
/**
* @}
*/

/**
 * The last data flag is used to indicate the last chunk of the request/response data. The connector 
 * will use this flag to indicate the last request chunk and application should use this to 
 * indicate the last response.
 */
#define IDIGI_FLAG_LAST_DATA	0x10

/** 
 * General purpose data structure used to hold the application or cloud data. 
 * When calling idigi_send_data() this structure contains the data to be sent to the 
 * iDigi Device cloud; when this is passed to @ref idigi_device_request_callback_t it 
 * contains the data received from the iDigi Device Cloud. The length_in_bytes field depends on the 
 * context in which this structure is used, when sending a @ref device_response_callback "response", 
 * the length indicates the size of the buffer passed into the function. 
 */
typedef struct idigi_connector_data_t
{
    void * data_ptr;               /**< Pointer to application/cloud data */
    size_t length_in_bytes;        /**< Number of bytes filled in (@ref idigi_device_request_callback_t "request"), number of bytes available to fill (@ref device_response_callback "response") or the number of bytes available to @ref idigi_send_data "send" */
    unsigned int flags;            /**< Indicates archive/append in idigi_send_data, otherwise it indicates the last chunk @see IDIGI_FLAG_OVERWRITE_DATA @see IDIGI_FLAG_ARCHIVE_DATA @see IDIGI_FLAG_APPEND_DATA @see IDIGI_FLAG_LAST_DATA */
    void * app_context;            /**< Pointer to hold application specific context, passed into subsequent calls */
    idigi_connector_error_t error; /**< Error encountered, the application has to check this value before handling the data */
}idigi_connector_data_t;

/**
 * @defgroup device_request_callback User defined device request callback function.
 * @{
 * @b Include: idigi_connector.h
 */
/**
 *  
 * This routine is called when a device request is received. 
 * This function needs to determine if the target and the data associated with that target is valid.
 * If both are valid, this function should take the appropriate actions with the data received.
 *
 * @param target       Null-terminated device request target name
 * @param request_data Pointer to the @ref idigi_connector_data_t "request data" received from the iDigi Device Cloud.
 *
 * @retval idigi_app_success        Success
 * @retval idigi_app_busy           Application is not ready to receive this request
 * @retval idigi_app_unknown_target Target not supported
 * @retval idigi_app_resource_error Failed to allocate required resources
 *
 * Example Usage:
 * @code 
 *  
 * idigi_app_error_t device_request_callback(char const * const target, idigi_connector_data_t * const request_data)
 * {
 *   static char buffer[BUFFER_SIZE];
 *   idigi_app_error_t status=idigi_app_invalid_parameter;
 *
 *    if (request_data->error != idigi_connector_success)
 *    {
 *        APP_DEBUG("device_request_callback: error [%d]\n", request_data->error);
 *        goto error;
 *    }
 *
 *    if (request_data->length_in_bytes < sizeof buffer)
 *    {
 *        // Copy the request received into our own local buffer 
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
 * }
 * @endcode
 *  
 *  
 * @see idigi_connector_data_t
 * @see idigi_app_error_t
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
 * @param target        null-terminated target name
 * @param response_data pointer to the response info, which contain pointer to a buffer where user
 *                      can write the response, maximum buffer length, flag to indicate last response
 *                      data, connector error if any and the application context provided in the
 *                      first request callback.
 *
 * @retval -1 indicates error
 * @retval 0  busy if last flag is not set
 * @retval 0> indicates number of bytes copied to the response buffer, cannot be more than the available 
 * bytes specified in @ref idigi_connector_data_t.
 *
 * Example Usage:
 * @code
 *    static char rsp_string[] = "iDigi Connector device response\n";
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
 * @see  idigi_connector_data_t
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
 * This function registers device request and response callback functions.  The device 
 * request callback is called when data is received from the iDigi device cloud, the 
 * device response callback is called to retrieve the response to the previous request. 
 * 
 * @param request_callback Called with request data from the iDigi Device Cloud
 * @param response_callback Called to get the response data to the previous request
 *
 * @retval idigi_connector_success Success
 * @retval idigi_connector_invalid_parameter NULL callback function
 * @retval idigi_connector_already_registered Callback has already been registered
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
 *  
 * @see idigi_device_request_callback_t
 * @see idigi_device_response_callback_t
 * @see idigi_connector_error_t
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
 * This function is used to write data to the iDigi Device cloud to the file specified in path. 
 * The @ref IDIGI_FLAG_OVERWRITE_DATA "flags" are used to specify how the data
 * will be written to the file.  @note This call will block call sending data over the network.
 *
 * @param path null-terminated file path where user wants to store the data on the iDigi Device Cloud
 * @param device_data Contains pointer to @ref idigi_connector_data_t which contains the data to be written to the file.
 *  The @ref IDIGI_FLAG_OVERWRITE_DATA "flags" field indicates whether to overwrite, append or archive the data
 * @param content_type null-terminated content type (text/plain, text/xml, application/json, etc). Pass NULL to let the iDigi Device Cloud determine
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
 *    char path[] = 'test.txt";             // Name of the file to be created in the cloud
 *    char buffer[] = "This is a test";     // Data to be written to the file
 *    unsigned int flags = IDIGI_FLAG_OVERWRITE_DATA;
 *  
 *    // This is called in a loop, an idigi_connector_init_error error may be returned if the initialization is not
 *    // complete
 *    do
 *    {
 *        static idigi_connector_data_t device_data = {0};
 *
 *        device_data_data.data_ptr = buffer;
 *        device_data_data.length_in_bytes = strlen(buffer);
 *        device_data_data.flags = flags;
 *  
 *        // Call the API to write the file
 *        ret = idigi_send_data(path, &device_data, content_type);
 *        if (ret == idigi_connector_init_error)
 *        {
 *            #define WAIT_FOR_A_SECOND  1
 *            app_os_sleep(WAIT_FOR_A_SECOND);
 *        }
 *
 *    } while (ret == idigi_connector_init_error);
 * @endcode 
 *  
 *  
 * @see idigi_connector_error_t
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
 * @param status iDigi connector @ref idigi_connector_error_t "error value"
 * @param status_message String containing error message describing the failure.
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
 * This function will start the iDigi Connector, this will connect to the iDigi Device Cloud.  Any errors 
 * encountered during initialization will invoke the provided @ref idigi_status_callback_t "callback routine".  
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
 * @see idigi_connector_error_t
 */
idigi_connector_error_t idigi_connector_start(idigi_status_callback_t status_callback);
/**
* @}
*/

/**
* @}.
*/
#endif

