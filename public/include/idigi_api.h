
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

#define IDIGI_VERSION_1100   0x01010000UL

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
   idigi_no_resource            /**< Lack of resource */
} idigi_status_t;
/**
* @}
*/

 /**
 * @defgroup idigi_class_t idigi_class_t
 * @{
 */
/**
* idigi_class_t.
* Class Id for which class is used in the application's callback.
* It tells class id for the request id passed to the application's callback.
*/
typedef enum {
    idigi_class_config,             /**< Configuration Class Id */
    idigi_class_network,            /**< Network Class Id */
    idigi_class_operating_system,   /**< Operating System Class Id */
    idigi_class_firmware,           /**< Firmware Facility Class Id */
    idigi_class_data_service        /**< Data Service Class Id */
} idigi_class_t;
/**
* @}
*/

/**
* @defgroup idigi_service_supported_status_t idigi_service_supported_status_t.
* @{
*/
/**
* idigi_service_supported_status_t.
* Service supported status which is used in the application's callback
* telling IIK whether application supports a service or not.
* @See idigi_config_firmware_facility
* @See idigi_config_data_service
*/
typedef enum {
    idigi_service_unsupported,
    idigi_service_supported
} idigi_service_supported_status_t;
/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_config_request_t.
* Configuration Request Id passed to the application's callback for requesting configuration data.
* The class id for this idigi_config_request_t is idigi_class_config.
*/
typedef enum {
    idigi_config_device_id,         /**< Requesting callback to return a unique device id which is used to identify the device.*/
    idigi_config_vendor_id,         /**< Requesting callback to return a unique vendor id identifying the manufacturer of a device. */
    idigi_config_device_type,       /**< Requesting callback to return device type that identifies the type of the device  */
    idigi_config_server_url,        /**< Requesting callback to return iDigi server FQDN which will be connected to */
    idigi_config_connection_type,   /**< Requesting callback to return LAN or WAN connection type */
    idigi_config_mac_addr,          /**< Requesting callback to return device's MAC addresss */
    idigi_config_link_speed,        /**< Requesting callback to return link speed for WAN connection type */
    idigi_config_phone_number,      /**< Requesting callback to return phone number dialed for WAN connection type */
    idigi_config_tx_keepalive,      /**< Requesting callback to return server's Tx keepAlive interval in seconds */
    idigi_config_rx_keepalive,      /**< Requesting callback to return server's Rx keepAlive interval in seconds */
    idigi_config_wait_count,        /**< Requesting callback to return the number of intervals of not receiving a keepAlive message after which a connection should be considered lost. */
    idigi_config_ip_addr,           /**< Requesting callback to return device's IP address */
    idigi_config_error_status,      /**< Error status notification which tells callback that error is encountered. */
    idigi_config_firmware_facility, /**< Requesting callback to return whether firmware facility is supported or not. */
    idigi_config_data_service,      /**< Requesting callback to return whether data service is supported or not. */
#if (IDIGI_VERSION >= IDIGI_VERSION_1100)
    idigi_config_max_transaction    /**< Requesting callback to obtain maximum messaging sessions supported by client. */
#endif
} idigi_config_request_t;
/**
* @}
*/


/**
* @defgroup.
* @{
*/
/**
* idigi_network_request_t.
* Network Request Id passed to the application's callback for network interface.
* The class id for this idigi_network_request_t is idigi_class_network.
*/
typedef enum {
    idigi_network_connect,                  /**< Requesting callback to setup and make connection to iDigi server */
    idigi_network_send,                     /**< Requesting callback to send data to iDigi server */
    idigi_network_receive,                  /**< Requesting callback to receive data from iDigi server */
    idigi_network_close,                    /**< Requesting callback to close iDigi server connection */
    idigi_network_disconnected,             /**< iDigi server disconnected notification. iDigi server requests to disconnect the connection */
    idigi_network_reboot                   /**< Requesting callback to reboot the system */
} idigi_network_request_t;
/**
* @}
*/


 /**
 * @defgroup idigi_os_request_t idigi_os_request_t: OS request ID's
 * @{ 
 */
 /** 
 * idigi_os_request_t.
 * Operating System Request ID passed to the application's callback for operating system interface.
 * The class id for this idigi_os_request_t is idigi_class_operating_system.
 */
typedef enum {
    idigi_os_malloc,            /**< Callback used to dynamically allocate memory.. */
    idigi_os_free,              /**< Callback is called to free previous allocated memory. */
    idigi_os_system_up_time,    /**< Callback is called to return system up time in seconds. It is the time that a device has been up and running. */
    idigi_os_sleep              /**< Callback is called to sleep or relinquish so that other tasks can be executed when @see idigi_run is used. */
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

/**
* @defgroup.
* @{
*/
/**
* idigi_firmware_request_t.
* Firmware Facility Request ID passed to the application's callback for firmware update interface.
* The class id for this idigi_firmware_request_t is idigi_class_firmware.
*/
typedef enum {
    idigi_firmware_target_count,            /**< Requesting callback to return number of supported target for firmware update */
    idigi_firmware_version,                 /**< Requesting callback to return the version number for specific target */
    idigi_firmware_code_size,               /**< Requesting callback to return max code size of specific target */
    idigi_firmware_description,             /**< Requesting callback to return description of specific target */
    idigi_firmware_name_spec,               /**< Requesting callback to return the regular expression for firmware update image name of specific target. */
    idigi_firmware_download_request,        /**< Requesting callback to start firmware update of specific target */
    idigi_firmware_binary_block,            /**< Callback is passed with image data for firmware update. This is called for each chunk of image data */
    idigi_firmware_download_complete,       /**< Callback is called to complete firmware update. */
    idigi_firmware_download_abort,          /**< Requesting callback to abort firmware update */
    idigi_firmware_target_reset             /**< Requesting callback to reset the target */
} idigi_firmware_request_t;
/**
* @}
*/


/**
 * Data service request ID, passed to the application callback 
 * to request the data, to pass the response, and to pass the 
 * error.
 */
typedef enum {
#if (IDIGI_VERSION < IDIGI_VERSION_1100)
    idigi_data_service_send_complete, /**< Deprecated. Used to indicate the completion of earlier initiate_action() call to send data to the cloud */
    idigi_data_service_response,  /**< Deprecated. Used to indicate the device cloud response for the current operation */
    idigi_data_service_error, /**< Deprecated. Used to indicate the error either from the device cloud or from the IIK while processing the message */
#endif
    idigi_data_service_put_request, /**< Indicates data service request related to send data to the device cloud */
    idigi_data_service_device_request /**< Indicates data service request related to receive data from device cloud */
} idigi_data_service_request_t;

/**
* @defgroup.
* @{
*/
/**
* idigi_initiate_request_t.
* Request ID used in idigi_initiate_action.
*/
typedef enum {
    idigi_initiate_terminate,               /**< Terminates and stops IIK from running. */
    idigi_initiate_data_service            /**< Initiates the action to send data to device cloud, the data will be stored in a file on the cloud. */
} idigi_initiate_request_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_connection_type_t.
* Connection type for @see idigi_config_connection_type callback
*/
typedef enum {
   idigi_lan_connection_type,   /**< LAN connection type for Ethernet or WiFi */
   idigi_wan_connection_type    /**< WAN connection type for PPP over Modem */
} idigi_connection_type_t;
/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_callback_status_t.
* Return status from callback function.
*/
typedef enum  {
    idigi_callback_continue,        /**< Continues with no error */
    idigi_callback_busy,            /**< Callback is busy */
    idigi_callback_abort,           /**< Aborts IIK */
    idigi_callback_unrecognized     /**< Unsupported callback request */
} idigi_callback_status_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_status_t.
* Return status code for firmware update. These status codes are used for @see idigi_firmware_download_request,
* @see idigi_firmware_binary_block and @see idigi_firmware_abort callbacks.
*/
typedef enum {
   idigi_fw_success,                        /**< No error */
   idigi_fw_download_denied,                /**< Callback denied firmware update */
   idigi_fw_download_invalid_size,          /**< Callback returns invalid size */
   idigi_fw_download_invalid_version,       /**< Callback returns invalid version */
   idigi_fw_download_unauthenticated,       /**< The server has not been authenticated */
   idigi_fw_download_not_allowed,           /**< The server is not allowed to provided updates */
   idigi_fw_download_configured_to_reject,  /**< Callback rejects firmware update */
   idigi_fw_encountered_error,              /**< Callback encountered an error that precludes the firmware update */
   idigi_fw_user_abort,                     /**< User aborted firmware update */
   idigi_fw_device_error,                   /**< Device or server encountered an error in the download data */
   idigi_fw_invalid_offset,                 /**< idigi_firmware_binary_block callback found invalid offset. */
   idigi_fw_invalid_data,                   /**< idigi_firmware_binary_block callback found invalid data block.*/
   idigi_fw_hardware_error                  /**< Callback found permanent hardware error */
} idigi_fw_status_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_download_complete_status_t.
* Firmware Update Complete status. These status codes are used in @see idigi_firmware_download_complete callback.
*/
typedef enum {
   idigi_fw_download_success,               /**< Callback returns this for firmware download finished successfully and calculated checksum matched the checksum sent in the callback */
   idigi_fw_download_checksum_mismatch,     /**< Callback returns this for download completed successfully, but the calculated checksum did not match the checksum sent in the callback */
   idigi_fw_download_not_complete           /**< Callback did not complete download successfully */
} idigi_fw_download_complete_status_t;
/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_request_t.
* Request ID structure passed to callback identifying the request id of specific class @see idigi_class_t
*/
typedef union {
   idigi_config_request_t config_request;               /**< Configuration request ID for configuration class */
   idigi_network_request_t network_request;             /**< Network request ID for network class */
   idigi_os_request_t os_request;                       /**< Operating system request ID for operating system class */
   idigi_firmware_request_t firmware_request;           /**< Firmware request ID for firmware facility class */
   idigi_data_service_request_t data_service_request;   /**< Data service request ID for data service class */
} idigi_request_t;
/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_handle_t.
* IIK Handle type that is used throughout IIK APIs.
*/
#define idigi_handle_t void *


/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_error_status_t.
* Error status structure for idigi_config_error_status callback (@see idigi_config_request_t) which
* is called when IIK encounters an error.
*/
typedef struct  {
    idigi_class_t class_id;         /**< Class ID which IIK encounters error with */
    idigi_request_t request_id;     /**< Request ID which IIK encounters error with */
    idigi_status_t status;          /**< Error status */
} idigi_error_status_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_write_request_t.
* Write request structure for idigi_network_send callback which is called to send data to iDigi server.
*/
typedef struct  {
    idigi_network_handle_t * network_handle;    /**< Pointer to network handle associated with a connection through the idigi_network_connect callback */
    uint8_t const * buffer;                     /**< Pointer to data to be sent */
    size_t length;                              /**< Number of bytes of data to be sent */
    unsigned timeout;                           /**< Timeout value in seconds which callback must return. This allows IIK to maintenance keepalive process and send process. */
} idigi_write_request_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_read_request_t.
* Read request structure for idigi_network_receive callback which is called to receive
* a specified number of bytes data from the iDigi server.
*/
typedef struct  {
    idigi_network_handle_t * network_handle;    /**< Pointer to network handle associated with a connection through the idigi_network_connect callback */
    uint8_t * buffer;                           /**< Pointer to memory where callback writes recieved data to */
    size_t length;                              /**< Number of bytes to be received */
    unsigned timeout;                           /**< Timeout value in seconds which callback must return. This allows IIK to maintenance keepalive process and send process. */
} idigi_read_request_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_config_t.
* Firmware configuration structure for idigi_firmware_version,  idigi_firmware_code_size,
* idigi_firmware_description, idigi_firmware_name_spec, and idigi_firmware_target_reset callbacks
*/
typedef struct {
   unsigned timeout;    /**< Timeout value which callback must return control back to IIK in seconds */
   uint8_t target;      /**< Target number */
} idigi_fw_config_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_download_request_t.
* Firmware download request structure for idigi_firmware_download_request callback which
* is called when server requests firmware download.
*/
typedef struct {
    unsigned timeout;           /**< Timeout value which callback must return control back to IIK in seconds */
    uint8_t target;             /**< Target number of whcih firmware download request for */
    uint32_t version;           /**< Version number of the download target. It's set to 0xFFFFFFFF for unknown version number */
    uint32_t code_size;         /**< size of the code that is ready to be sent to the target */
    char * desc_string;         /**< Pointer to target description */
    char * file_name_spec;      /**< Pointer to file name in regular expression */
    char * filename;            /**< Pointer to filename of the image to be downloaded */
} idigi_fw_download_request_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_image_data_t.
* Firmware download image data structure for idigi_firmware_binary_block callback which
* is called when IIK receives a block of image data for firmware download.
*/
typedef struct {
    unsigned timeout;       /**< Timeout value which callback must return control back to IIK in seconds */
    uint8_t target;         /**< Target number of which image data for */
    uint32_t offset;        /**< Offset value where this particular block of image data fits into the download */
    uint8_t * data;         /**< Pointer binary image data */
    size_t length;          /**< Length of binary image data in bytes */
} idigi_fw_image_data_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_download_complete_request_t.
* Firmware download complete request structure containing information about firmware image data
* for idigi_firmware_download_complete callback which is called when iDigi server is done
* sending all image data.
*/
typedef struct {
    unsigned timeout;       /**< Timeout value which callback must return control back to IIK in seconds */
    uint8_t target;         /**< Target number of which firmware download complete for */
    uint32_t code_size;     /**< Code size of the entire image data sent */
    uint32_t checksum;      /**< CRC-32 value computed from offset 0 to code size. If it's 0, no checksum is required */
} idigi_fw_download_complete_request_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_download_complete_response_t.
* Firmware download complete response structure for idigi_firmware_download_complete callback which
* writes information and status of the download completion when iDigi server is done sending all image data.
*/
typedef struct {
    uint32_t version;                               /**< Version number of the downloaded image */
    uint32_t calculated_checksum;                   /**< It's currently used for error code that is used by server. Set 0 for no error. */
    idigi_fw_download_complete_status_t status;     /**< Status code regarding the download completion */
} idigi_fw_download_complete_response_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_fw_download_abort_t.
* Firmware download abort structure for idigi_firmware_abort callback which
* is called when server aborts firmware download process.
*/
typedef struct {
    unsigned timeout;           /**< Timeout value which callback must return control back to IIK in seconds */
    uint8_t target;             /**< Target number of which firmware download abort for */
    idigi_fw_status_t status;   /**< Abort reason or status */
} idigi_fw_download_abort_t;
/**
* @}
*/

/**
 * Error values returned either from the remote device cloud or 
 * from the local iDigi client. These are errors originated from
 * messaging layer, where compression/decompression, resource
 * allocation and state handling take place. 
 */
typedef enum 
{
    idigi_msg_error_none, /**< Success */
    idigi_msg_error_fatal, /**< Generally represents internal, unexpected error */
    idigi_msg_error_invalid_opcode, /**< Opcode used in the message is invalid/unsupported */
    idigi_msg_error_format, /**< Packet is framed incorrectly */
    idigi_msg_error_session_in_use, /**< Session with same ID is already in use */
    idigi_msg_error_unknown_session, /**< Session is not opened or already closed */
    idigi_msg_error_compression_failure, /**< Failed during compression of the data to send */
    idigi_msg_error_decompression_failure, /**< Failed during decompression of the received data */
    idigi_msg_error_memory, /**< Malloc failed, try to restrict the number of active sessions */
    idigi_msg_error_send, /**< Send socket error */
    idigi_msg_error_cancel, /**< Used to force termination of a session */
    idigi_msg_error_busy, /**< Either device cloud or iDigi client is busy processing */
    idigi_msg_error_ack, /**< Invalid ack count */
    idigi_msg_error_timeout, /**< Session timed out */
    idigi_msg_error_no_service, /**< Requested service is not supported */
    idigi_msg_error_count  /**< Maximum error count value, new value goes before this element */
} idigi_msg_error_t;

/**
 * Possible response status returned from device cloud for the 
 * data send request (put request). 
 */
typedef enum
{
    idigi_data_success, /**< Data transferred successfully */
    idigi_data_bad_request, /**< Either path or content type is invalid */
    idigi_data_service_unavailable, /**< The requested service is not supported */
    idigi_data_server_error /**< Device cloud error */
} idigi_data_status_t;

#if (IDIGI_VERSION < IDIGI_VERSION_1100)

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
} idigi_data_send_t;

typedef struct
{
    void * session;
    idigi_msg_error_t error;
} idigi_data_error_t;

#endif

/**
 * We need to figure this out.
 */
#define IDIGI_DATA_PUT_ARCHIVE   0x0001

#define IDIGI_DATA_PUT_APPEND    0x0002

/**
 * Put request header information. Used as initiate_action() 
 * request parameter to initiate the send operation. Subsequent 
 * handling is done via callback functions. This header 
 * information is returned as user_context in each callback 
 * request. 
 */
typedef struct
{
    char const * path;  /**< NUL terminated file path where user wants to store the data on device cloud */
    char const * content_type;  /**< NUL terminated content type (text/plain, text/xml, application/json, etc. */
    unsigned int flags; /**< Indicates whether server should archive and/or append, one of the following @ref IDIGI_DATA_PUT_ARCHIVE */
    void const * context; /**< To hold the user context */
} idigi_data_put_header_t;

#define IDIGI_MSG_FIRST_DATA     0x0001
#define IDIGI_MSG_LAST_DATA      0x0002

/**
* @defgroup.
* @{
*/
/**
* idigi_data_service_type_t.
* Data service types which is used in idigi_data_service_device_request and
* idigi_data_service_put_request callbacks indicating the type of message. */
typedef enum 
{
    idigi_data_service_type_need_data,      /**< Indicating callback needs to write data onto specified buffer which will be sent to server */
    idigi_data_service_type_have_data,      /**< Indicating a message contains data from server that needs callback to process it. */
    idigi_data_service_type_error           /**< Indicating error is encountered. Needs to terminate */
} idigi_data_service_type_t;

/**
 * Data service put request application callback updates this 
 * response based on the request type. If the request is for 
 * need data then user has to fill the data and if the request 
 * is for have data then user has to copy the data to their 
 * space. 
 */
typedef struct idigi_data_put_response_t
{
    idigi_data_service_type_t response_type; /**< Determines the what data field contains */
    size_t length_in_bytes; /**< Number of bytes in data */
    void * data; /**< Content of this field is decided on response_type */
    unsigned int flags; /**< Determines whether it is first and/or last data */
} idigi_data_put_response_t;

/**
 * Data service put request application callback receives this 
 * as a request. User can use header_context to determine which
 * session is sending this request. 
 */
typedef struct idigi_data_put_request_t
{
    idigi_data_service_type_t request_type; /**< Decides whether the request is for need data, have data, or error */
    void const * header_context;  /**< Holds idigi_data_put_header_t * provided in idigi_initiate_action() */
} idigi_data_put_request_t;

/**
* @defgroup.
* @{
*/
/**
* idigi_data_service_device_response_status_t.
* Data service device response status. These status codes are used in
* @see idigi_data_service_device_request callback.
*
*/
typedef enum {
    idigi_data_service_device_success,          /**< Callback has no error and successfully processed the device request data */
    idigi_data_service_device_not_handled       /**< Callback didn't not processed the device request data */
} idigi_data_service_device_response_status_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_data_service_device_data_t.
* Data service device data structure which is used for device request and response data for
* idigi_data_service_device_request callback */
typedef struct {
    void * data;                    /**< Buffer which device request or response data  */
    size_t length_in_bytes;         /**< Number of bytes in data */
    unsigned int flag;              /**< Bit field indicating first or last chunk of data */
} idigi_data_service_device_data_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_data_service_device_request_t.
* Data service device request structure which is passed to @see idigi_data_service_device_request
* callback to process device request */
typedef struct {
    void * user_context;                                    /**< User context from previous idigi_data_service_device_request callback  */
    void * session;                                         /**< session handle for current device request */
    char const * target;                                    /**< Nul terminated target name */
    idigi_data_service_type_t message_type;                 /**< Meesage type of message data @see idigi_data_service_type_t */
    union {
        idigi_data_service_device_data_t * request_data;    /**< Pointer to device request which contains request data for idigi_data_service_type_have_data message type */
        idigi_msg_error_t                error_code;        /**< Error code which IIK encounters for idigi_data_service_type_error message type */
    } message_data;
} idigi_data_service_device_request_t;

/**
* @}
*/

/**
* @defgroup.
* @{
*/
/**
* idigi_data_service_device_response_t.
* Data service device response structure which is passed to @see idigi_data_service_device_request
* callback to return device response */
typedef struct
{
    void * user_context;                                            /**< User context which will be passed to @see idigi_data_service_device_request_t on next idigi_data_service_device_request callback */
    idigi_data_service_device_response_status_t status;             /**< Status of device request */
    idigi_data_service_device_data_t            * response_data;    /**< Pointer to memory where callback writes device response data */
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
 * This is used to initiate the send data from the device to the
 * iDigi Device Cloud and to terminate the IIK library. 
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
 *                           This is used to trigger the send
 *                           data to the iDigi server. Only the
 *                           header information is passed by
 *                           this method. The actual data is
 *                           transferred through callbacks. The
 *                           data is stored in a specified file
 *                           on the server.
 *
 * 
 * @retval idigi_success  No error
 * 
 * @retval idigi_configuration_error  IIK was not initialized
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
