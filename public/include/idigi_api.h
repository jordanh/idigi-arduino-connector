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
 /**
  * @file
  *  @brief Functions and prototypes for iDigi Integration kit
  *         public API
  *
  */
#ifndef _IDIGI_API_H
#define _IDIGI_API_H

#include "idigi_types.h"

#define IDIGI_PORT       3197
#define IDIGI_SSL_PORT   3199


 /**
 * @defgroup idigi_status_t idigi_status_t: Status returned by IIK API calls.
 * @{ 
 */
 /** 
 * idigi_status_t.
 * Status returned by IIK API calls.
 */
typedef enum {
   idigi_success,               /**< No error. */
   idigi_init_error,            /**< IIK was not initialized. */
   idigi_configuration_error,   /**< IIK was aborted by callback function. */
   idigi_invalid_data_size,     /**< Callback returned configuration with invalid size. */
   idigi_invalid_data_range,    /**< Callback returned configuration that is out of range. */
   idigi_invalid_payload_packet, /**< IIK received invalid payload message. */
   idigi_keepalive_error,       /**< IIK did not receive keepalive messages. Server may be offline. */
   idigi_server_overload,       /**< Server overload. */
   idigi_bad_version,           /**< Server rejected version number. */
   idigi_invalid_packet,        /**< IIK received unrecognized or unexpected message. */
   idigi_exceed_timeout,        /**< Callback exceeded timeout value before it returned. */
   idigi_unsupported_security,  /**< IIK received a packet with unsupported security. */
   idigi_invalid_data,          /**< Callback returned invalid data. Callback may return a NULL data.. */
   idigi_server_disconnected,   /**< Server disconnected IIK. */
   idigi_connect_error,         /**< IIK was unable to connect to the iDigi server. The callback for connect failed. */
   idigi_receive_error,         /**< Unable to receive message from the iDigi server. The callback for receive failed. */
   idigi_send_error,            /**< Unable to send message to the iDigi server. The callback for send failed. */
   idigi_close_error,           /**< Unable to disconnect the connection. The callback for close failed. */
   idigi_device_terminated,     /**< IIK was terminated by user via idigi_initiate_action call. */
   idigi_service_busy,          /**< Someone else is using the same service or the device is busy. */
   idigi_invalid_response,      /**< Received invalid response from the server. */
   idigi_no_resource
} idigi_status_t;
/**
* @}
*/

typedef enum {
    idigi_class_config,
    idigi_class_network,
    idigi_class_operating_system,
    idigi_class_firmware,
    idigi_class_data_service
} idigi_class_t;

typedef enum {
    idigi_config_device_id,
    idigi_config_vendor_id,
    idigi_config_device_type,
    idigi_config_server_url,
    idigi_config_connection_type,
    idigi_config_mac_addr,
    idigi_config_link_speed,
    idigi_config_phone_number,
    idigi_config_tx_keepalive,
    idigi_config_rx_keepalive,
    idigi_config_wait_count,
    idigi_config_ip_addr,
    idigi_config_error_status,
    idigi_config_firmware_facility,
    idigi_config_data_service
} idigi_config_request_t;

typedef enum {
    idigi_network_connect,
    idigi_network_send,
    idigi_network_receive,
    idigi_network_close,
    idigi_network_disconnected,
    idigi_network_reboot,
    idigi_network_initialization_done

} idigi_network_request_t;


 /**
 * @defgroup idigi_os_request_t idigi_os_request_t: OS request ID's
 * @{ 
 */
 /** 
 * idigi_status_t.
 * Status returned by IIK API calls.
 */
typedef enum {
    idigi_os_malloc, /**< Callback used to dynamically allocate memory.. */
    idigi_os_free, /**< Callback is called to free previous allocated memory. */
    idigi_os_system_up_time, /**< This callback is called to return system up time in seconds. It is the time that a device has been up and running. */
    idigi_os_lock_init,
    idigi_os_lock,
    idigi_os_unlock,
    idigi_os_lock_delete
} idigi_os_request_t;
/**
* @}
*/

 /**
 * @defgroup idigi_os_malloc idigi_os_malloc: Dynamically allocate memory
 *  @ref idigi_callback_t "Callback" used to dynamically allocate memory
 *  @param class_id: idigi_class_operating_system class ID
 *  @param request_id idigi_os_malloc request ID
 *  @param request_data Pointer to number of bytes to be allocated
 *  @param request_length Specifies the size of *request_data which is size of size_t.
 *  @param response_data Callback returns a pointer to memory for allocated address
 *  @param response_length ignore
 *
 * @return idigi_callback_continue Callback successfully allocated memory
 * @return idigi_callback_abort Callback was unable to allocate memory and callback aborts IIK
 * @return idigi_callback_busy Memory is not available at this time and needs to be called back again
 * @see idigi_callback_t
 * @{ 
 */

/**
* @}
*/

 /**
 * @defgroup idigi_os_system_up_time idigi_os_system_up_time: System up time
 *  @ref idigi_callback_t "Callback" used to to return system up time in seconds. It is the time that a device has been up and running.
 *  @param class_id: idigi_class_operating_system class ID
 *  @param request_id idigi_os_system_up_time request ID
 *  @param request_data NULL
 *  @param request_length 0
 *  @param response_data Pointer to uint32_t integer memory where callback writes the system up time to (in seconds)
 *  @param response_length NULL Ignore
 *
 * @return idigi_callback_continue Callback returned system up time
 * @return idigi_callback_abort Callback was unable to get system time
 * @see idigi_callback_t
 * @{ 
 */

/**
* @}
*/

 /**
 * @defgroup idigi_os_malloc idigi_os_malloc: Dynamically allocate memory
 *  @ref idigi_callback_t "Callback" used to dynamically allocate memory
 *  @param class_id: idigi_class_operating_system class ID
 *  @param request_id idigi_os_malloc request ID
 *  @param request_data Pointer to number of bytes to be allocated
 *  @param request_length Specifies the size of *request_data which is size of size_t.
 *  @param response_data Callback returns a pointer to memory for allocated address
 *  @param response_length ignore
 *
 * @return idigi_callback_continue Callback successfully allocated memory
 * @return idigi_callback_abort Callback was unable to allocate memory and callback aborts IIK
 * @return idigi_callback_busy Memory is not available at this time and needs to be called back again
 * @see idigi_callback_t
 * @{ 
 */

/**
* @}
*/





typedef enum {
    idigi_firmware_target_count,
    idigi_firmware_version,
    idigi_firmware_code_size,
    idigi_firmware_description,
    idigi_firmware_name_spec,
    idigi_firmware_download_request,
    idigi_firmware_binary_block,
    idigi_firmware_download_complete,
    idigi_firmware_download_abort,
    idigi_firmware_target_reset
} idigi_firmware_request_t;

typedef enum {
    idigi_data_service_send_complete,
    idigi_data_service_response,
    idigi_data_service_error,
    idigi_data_service_request,
    idigi_data_service_max_transactions
} idigi_data_service_request_t;

typedef enum {
    idigi_initiate_terminate,
    idigi_initiate_data_service,
    idigi_initiate_data_service_response
} idigi_initiate_request_t;

typedef enum {
   idigi_lan_connection_type,
   idigi_wan_connection_type
} idigi_connection_type_t;

typedef enum  {
    idigi_callback_continue,
    idigi_callback_busy,
    idigi_callback_abort,
    idigi_callback_unrecognized
} idigi_callback_status_t;

typedef enum {
   idigi_fw_success,
   idigi_fw_download_denied,
   idigi_fw_download_invalid_size,
   idigi_fw_download_invalid_version,
   idigi_fw_download_unauthenticated,
   idigi_fw_download_not_allowed,
   idigi_fw_download_configured_to_reject,
   idigi_fw_encountered_error,
   idigi_fw_user_abort,
   idigi_fw_device_error,
   idigi_fw_invalid_offset,
   idigi_fw_invalid_data,
   idigi_fw_hardware_error
} idigi_fw_status_t;

typedef enum {
   idigi_fw_download_success,
   idigi_fw_download_checksum_mismatch,
   idigi_fw_download_not_complete
} idigi_fw_download_complete_status_t;

typedef union {
   idigi_config_request_t config_request;
   idigi_network_request_t network_request;
   idigi_os_request_t os_request;
   idigi_firmware_request_t firmware_request;
   idigi_data_service_request_t data_service_request;
} idigi_request_t;

#define idigi_handle_t void *

typedef struct  {
    idigi_class_t class_id;
    idigi_request_t request_id;
    idigi_status_t status;
} idigi_error_status_t;

typedef struct  {
   idigi_network_handle_t *  network_handle;
    uint8_t const * buffer;
    size_t length;
    unsigned timeout;
} idigi_write_request_t;

typedef struct  {
   idigi_network_handle_t * network_handle;
    uint8_t * buffer;
    size_t length;
    unsigned timeout;
} idigi_read_request_t;

typedef struct {
   unsigned timeout;
   uint8_t target;
} idigi_fw_config_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t version;
    uint32_t code_size;
    char * desc_string;
    char * file_name_spec;
    char * filename;
} idigi_fw_download_request_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t offset;
    uint8_t * data;
    size_t length;
} idigi_fw_image_data_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t code_size;
    uint32_t checksum;
} idigi_fw_download_complete_request_t;

typedef struct {
    uint32_t version;
    uint32_t calculated_checksum;
    idigi_fw_download_complete_status_t status;
} idigi_fw_download_complete_response_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    idigi_fw_status_t status;
} idigi_fw_download_abort_t;

#define IDIGI_DATA_REQUEST_START        0x0001
#define IDIGI_DATA_REQUEST_LAST         0x0002
#define IDIGI_DATA_REQUEST_ARCHIVE      0x0004
#define IDIGI_DATA_REQUEST_COMPRESSED   0x0008
#define IDIGI_DATA_REQUEST_APPEND       0x0010

typedef struct
{
    uint8_t const * value;
    uint8_t size;
} idigi_data_block_t;

typedef struct
{
    uint8_t * data;
    size_t size;
} idigi_data_payload_t;

typedef struct
{
    void * session;
    uint16_t flag;
    idigi_data_block_t path;
    idigi_data_block_t content_type;
    idigi_data_payload_t payload;
} idigi_data_request_t;

typedef enum
{
    idigi_data_success,
    idigi_data_bad_request,
    idigi_data_service_unavailable,
    idigi_data_server_error
} idigi_data_status_t;

typedef struct
{
    void * session;
    idigi_data_status_t  status;
    idigi_data_block_t message;
} idigi_data_response_t;

typedef struct
{
    void * session;
    idigi_status_t status;
    size_t bytes_sent;
    void const * payload;
} idigi_data_send_t;

typedef enum 
{
    idigi_msg_error_none,
    idigi_msg_error_fatal,
    idigi_msg_error_invalid_opcode,
    idigi_msg_error_format,
    idigi_msg_error_session_in_use,
    idigi_msg_error_unknown_session,
    idigi_msg_error_compression_failure,
    idigi_msg_error_decompression_failure,
    idigi_msg_error_memory,
    idigi_msg_error_send,
    idigi_msg_error_cancel,
    idigi_msg_error_busy,
    idigi_msg_error_ack,
    idigi_msg_error_timeout,
    idigi_msg_error_no_service,
    idigi_msg_error_count
} idigi_msg_error_t;

typedef struct
{
    void * session;
    idigi_msg_error_t error;
} idigi_data_error_t;


typedef struct {
    void * session;
    char const * target;
    void const * data;
    size_t data_length;
    unsigned int flag;
    void * user_context;
} idigi_data_service_device_request_t;

typedef enum {
    idigi_data_service_success,
    idigi_data_service_not_handled
} idigi_data_service_device_response_status_t;

typedef struct
{
    void * session;
    idigi_data_service_device_response_status_t status;
    void const * data;
    size_t data_length;
    unsigned int flag;
} idigi_data_service_device_response_t;

 /**
 * @defgroup idigi_callback_t idigi_callback_t: IIK application defined callback.
 *@{ 
 * idigi_callback_t: IIK application callback.
 *
 */
 /** 
 * idigi_callback_t.
 *
 * @param class_id class ID for this request
 * @param request_id request ID
 * @param request_data Data for this request
 * @param request_length Number of bytes in the request
 * @param response_data Pointer to returned response
 * @param response_length Length of the response in bytes
 * 
 * 
 * @retval idigi_callback_status_t  Callback status.
 */
typedef idigi_callback_status_t (* idigi_callback_t) (idigi_class_t const class_id, idigi_request_t const request_id,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
/**
* @}
*/
 /**
 * @defgroup idigi_init idigi_init(): Initialize the IIK.
 *@{ 
 * @b Include: idigi_api.h
 */
/**
 *
 * This API is called once upon startup to allocate and 
 * initialize the IIK. It takes the application-defined callback 
 * function as an argument; this callback is used by the IIK to 
 * communicate with the application. This function must be 
 * called before all other IIK APIs. The idigi_callback_t is 
 * defined below. 
 *
 * @param [in] callback  Callback function that is used to 
 *        interface between the application and the IIK.
 *
 * @retval NULL   Error was found and the IIK is unable to 
 *         initialize.
 * @retval "Not NULL"  Handle used for subsequent IIK calls.
 *
 * Example Usage:
 * @code
 *    idigi_handle = idigi_init(application_callback);
 * @endcode
 *  
 * @see idigi_callback_t 
 */
idigi_handle_t idigi_init(idigi_callback_t const callback);
/**
* @}
*/

 /**
 * @defgroup idigi_step idigi_step(): Run a portion of the IIK
 * @{ 
 * @b Include: idigi_api.h
 */

/**
 * @brief   Run a portion of the IIK.
 *
 * This function is called to start and run the IIK. This
 *function performs a sequence of operations or events and 
 * returns control back to the caller. This allows a caller to
 * perform other tasks, especially in single-threaded system.
 * A caller must call this API again to continue IIK operations.
 * The connection is already terminated when idigi_step returns
 * an error, idigi_step will try reconnecting to the iDigi
 * Device Cloud if it's called again. The idigi_step performs
 * the following operations:
 * @li Establish a connection with the iDigi Device Cloud.
 * 
 * @li Wait for incoming messages from the iDigi Device Cloud.
 * 
 * @li Invoke and pass message to the appropriate process (such
 * as firmware access facility).
 *
 * @param [in] handle  Handle returned from idigi_init
 *
 * @retval idigi_success  No error
 * 
 * @retval idigi_success  IIK was not initialized
 *
 * Example Usage:
 * @code
 *     status = idigi_step(idigi_handle);
 * @endcode 
 *  
 * @see idigi_handle_t
 * @see idigi_callback_t
 */
idigi_status_t idigi_step(idigi_handle_t const handle);
/**
* @}
*/


 /**
 * @defgroup idigi_run idigi_run(): Run a portion of the IIK.
 * @{ 
 * @b Include: idigi_api.h
 */
/**
 * @brief   Run a portion of the IIK.
 *
 * This function is similar to idigi_step except it doesn't 
 * return control back to caller unless IIK encounters an error.
 * This function should be executed as a separated thread. 
 * 
 * @param [in] handle  Handle returned from idigi_init
 *
 * @retval idigi_success  Status code
 * 
 *
 * Example Usage:
 * @code
 *     status = idigi_run(idigi_handle);
 * @endcode 
 *  
 * @see idigi_status_t
 */
idigi_status_t idigi_run(idigi_handle_t const handle);
/**
* @}
*/


 /**
 * @defgroup idigi_initiate_action idigi_initiate_action(): Request IIK to perform an action.
 * @{
 * @b Include: idigi_api.h
 */
/** 
 * @brief   Request IIK to perform an action.
 *
 * This function is called to request IIK to perform an action. 
 * This is used to send data from the device to the iDigi Device 
 * Cloud and to terminate the IIK library. 
 *  
 * @param [in] handle  Handle returned from idigi_init 
 *  
 * @param [in] request  Request action (one of the following):
                        @li @b idigi_initiate_terminate:
 *                          Terminates and stops IIK from running. It closes the connection to the
 *                          iDigi server and frees all allocated
 *                          memory. If the application is using
 *                          idigi_step, the next call to
 *                          idigi_step will terminate the IIK.
 *                          If caller is using idigi_run,
 *                          idigi_run will terminate and return.
 *                           Once IIK is terminated, IIK cannot
 *                           restart unless idigi_init is
 *                           called  again.
 * 
 *                      @li @b idigi_initiate_data_service: 
 *                           This is used to send data to the
 *                           iDigi server, the data is stored in
 *                           a file on * the server.
 *
 * 
 * @retval idigi_success  No error
 * 
 * @retval idigi_success  IIK was not initialized
 *
 * Example Usage:
 * @code
 *     status = idigi_initiate_action(idigi_handle);
 * @endcode 
 *  
 * @see idigi_handle_t
 * @see idigi_callback_t
 */
idigi_status_t idigi_initiate_action(idigi_handle_t const handle, idigi_initiate_request_t const request,
                                     void const * const request_data, void * const response_data);
/**
* @}.
*/
#endif /* _IDIGI_API_H */
