
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


/**
 * IIK version number
 *
 * Note current version number denotes:  1.1.0.0
 *                                       | | | |
 *                                       v v v v
 *                                    0x01010000UL
 */
#define IDIGI_VERSION_1100   0x01010000UL

#include "idigi_types.h"


 /**
 * @defgroup idigi_port_numbers iDigi Port Numbers
 * @{ 
 */
/**
 * iDigi server connect port number
 *
 * This port number is for idigi_network_connect callback to make
 * a connection to iDigi server.
 *
 * @see IDIGI_SSL_PORT
 * @see idigi_network_connect
 */
#define IDIGI_PORT       3197   /**< Port number used to connect to iDigi server. */

/**
 * iDigi server secured connect port number
 *
 * This port number is for idigi_network_connect callback to make
 * a secured connection to iDigi server.
 *
 * @see IDIGI_PORT
 * @see idigi_network_connect
 */
#define IDIGI_SSL_PORT   3199   /**< Secured port number used to connect to iDigi server. */
/**
* @}
*/

 /**
 * @defgroup idigi_status_t idigi Status values
 * @{ 
 */
 /** 
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
   idigi_invalid_data,          /**< Callback returned invalid data. Callback may return a NULL data. */
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
 * @defgroup idigi_class_t Class IDs
 * @{
 */
/**
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
* @defgroup idigi_service_supported_status_t Service Support
* @{
*/
/**
* Service supported status which is used in the application's callback
* telling IIK whether application supports a service or not.
* @see @ref firmware_support
* @see @ref data_service_support
*/
typedef enum {
    idigi_service_unsupported,  /**< Service is supported */
    idigi_service_supported     /**< Service is not supported */
} idigi_service_supported_status_t;
/**
* @}
*/

/**
* @defgroup idigi_config_request_t Configuration Requests
* @{
*/
/**
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
* @defgroup idigi_network_request_t Network Request IDs
* @{
*/
/**
* Network Request ID passed to the application's callback for network interface.
* The class id for this idigi_network_request_t is idigi_class_network.
*/
typedef enum {
    idigi_network_connect,                  /**< Requesting callback to set up and make connection to iDigi server */
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
 * @defgroup idigi_os_request_t OS Request IDs
 * @{ 
 */
 /** 
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
* @defgroup idigi_firmware_request_t Firmware Requests
* @{
*/
/**
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
* @defgroup idigi_data_service_request_t Data Service Request IDs
* @{
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
* @}
*/

/**
* @defgroup idigi_initiate_request_t Initiate action enumeration
* @{
*/
/**
* Request IDs used in idigi_initiate_action()
*/
typedef enum {
    idigi_initiate_terminate,               /**< Terminates and stops IIK from running. */
    idigi_initiate_data_service            /**< Initiates the action to send data to device cloud, the data will be stored in a file on the cloud. */
} idigi_initiate_request_t;
/**
* @}
*/

/**
* @defgroup idigi_connection_type_t iDigi connection types
* @{
*/
/**
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
* @defgroup.idigi_callback_status_t Callback return status
* @{
*/
/**
* Return status from IIK callback
*/
typedef enum  {
    idigi_callback_continue,        /**< Continues with no error */
    idigi_callback_busy,            /**< Callback is busy */
    idigi_callback_abort,           /**< Aborts IIK. IIK will try reconnecting to iDigi Cloud
                                         if @ref idigi_step or @ref idigi_run is called again. */
    idigi_callback_unrecognized     /**< Unsupported callback request */
} idigi_callback_status_t;
/**
* @}
*/

/**
* @defgroup idigi_fw_status_t Firmware download return status
* @{
*/
/**
* Return status code for firmware update. These status codes are used for @ref idigi_firmware_download_request,
* @see @ref idigi_firmware_binary_block and @ref idigi_firmware_download_abort callbacks.
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
* @defgroup idigi_fw_download_complete_status_t Firmware complete status codes
* @{
*/
/**
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
* @defgroup idigi_request_t Request IDs
* @{
*/
/**
* Request IDs passed to callback identifying the type of request
* @see idigi_class_t
*/
typedef union {
   idigi_config_request_t config_request;               /**< Configuration request ID for configuration class */
   idigi_network_request_t network_request;             /**< Network request ID for network class */
   idigi_os_request_t os_request;                       /**< Operating system request ID for operating system class */
   idigi_firmware_request_t firmware_request;           /**< Firmware Request ID for firmware facility class */
   idigi_data_service_request_t data_service_request;   /**< Data service request ID for data service class */
} idigi_request_t;
/**
* @}
*/

/**
* @defgroup idigi_handle_t iDigi Handle
* @{
*/
/**
*
* IIK Handle type that is used throughout IIK APIs, this is used by the application
* to store context information about a connections, for example this could
* be used to store a file descriptor or a pointer to a structure.
*/
#define idigi_handle_t void *
/**
* @}
*/

/**
* @defgroup idigi_error_status_t Error Status
* @{
*/
/**
* Error status structure for @ref idigi_config_error_status callback which
* is called when IIK encounters an error.
* @see idigi_config_request_t
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
* @defgroup idigi_write_request_t Network Write Request
* @{
*/
/**
* Write request structure for @ref idigi_network_send callback which is called to send data to iDigi server.
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
* @defgroup idigi_read_request_t Network Read Request
* @{
*/
/**
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
* @defgroup idigi_fw_config_t Firmware configuration
* @{
*/
/**
* Firmware configuration structure for @ref idigi_firmware_version, @ref idigi_firmware_code_size,
* @ref idigi_firmware_description, @ref idigi_firmware_name_spec, and @ref idigi_firmware_target_reset callbacks.
*/
typedef struct {
    unsigned timeout;    /**< Timeout value which callback must return control back to IIK in seconds */
    uint8_t target;      /**< Target number */
} idigi_fw_config_t;
/**
* @}
*/

/**
* @defgroup idigi_fw_download_request_t Download Request
* @{
*/
/**
* Firmware download request structure for @ref idigi_firmware_download_request callback which
* is called when server requests firmware download.
*/
typedef struct {
    unsigned timeout;           /**< Timeout value which callback must return control back to IIK in seconds */
    uint8_t target;             /**< Target number of whcih firmware download request for */
    uint32_t version;           /**< Reserved */
    uint32_t code_size;         /**< size of the code that is ready to be sent to the target */
    char * desc_string;         /**< Reserved */
    char * file_name_spec;      /**< Reserved */
    char * filename;            /**< Pointer to filename of the image to be downloaded */
} idigi_fw_download_request_t;
/**
* @}
*/

/**
* @defgroup idigi_fw_image_data_t Image Data
* @{
*/
/**
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
* @defgroup idigi_fw_download_complete_request_t Download complete
* @{
*/
/**
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
* @defgroup idigi_fw_download_complete_response_t Download complete response
* @{
*/
/**
* Firmware download complete response structure for idigi_firmware_download_complete callback which
* writes information and status of the download completion when iDigi server is done sending all image data.
*/
typedef struct {
    uint32_t version;                               /**< Version number of the downloaded image */
    uint32_t calculated_checksum;                   /**< Reserved */
    idigi_fw_download_complete_status_t status;     /**< Status code regarding the download completion */
} idigi_fw_download_complete_response_t;
/**
* @}
*/

/**
* @defgroup idigi_fw_download_abort_t Download Abort
* @{
*/
/**
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
* @defgroup idigi_msg_error_t iDigi Error Codes
* @{
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
* @}
*/

/**
* @defgroup idigi_data_status_t Data Status
* @{
*/
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
/**
* @}
*/

#if (IDIGI_VERSION < IDIGI_VERSION_1100)

/* Deprecated Data Service API */
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
* @defgroup put_flags Data service put flags
* @{
*/
/**
 * Message archive bit flag
 *
 * This flag is used in idigi_data_service_put_request_t indicating
 * server needs to archive the message file.
 *
 * @see idigi_data_service_put_request_t
 * @see idigi_data_service_put_request callback
 */
#define IDIGI_DATA_PUT_ARCHIVE   0x0001 /**< This flag is used in idigi_data_service_put_request callback
                                           telling server should archive the message file. */


/**
 * Message append bit flag
 *
 * This flag is used in idigi_data_service_put_request_t indicating
 * server need to append to existing data if applicable.
 *
 * @see idigi_data_service_put_request_t
 * @see idigi_data_service_put_request callback
 */
#define IDIGI_DATA_PUT_APPEND    0x0002 /**< This flag is used in idigi_data_service_put_request callback
                                            telling server should append to existing data if applicable. */
/**
* @}
*/

/**
* @defgroup idigi_data_service_put_request_t idigi_data_service_put_request_t
* @{
*/
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
    unsigned int flags; /**< Indicates whether server should archive and/or append, one of the following @ref put_flags */
    void const * context; /**< To hold the user context */
} idigi_data_service_put_request_t;
/**
* @}
*/

/**
* @defgroup idigi_data_service_type_t Data service types
* @{
*/
/**
* Data service types which is used in idigi_data_service_device_request and
* idigi_data_service_put_request callbacks indicating the type of message. 
*/
typedef enum 
{
    idigi_data_service_type_need_data,      /**< Indicating callback needs to write data onto specified buffer which will be sent to server */
    idigi_data_service_type_have_data,      /**< Indicating a message contains data from server that needs callback to process it. */
    idigi_data_service_type_error           /**< Indicating error is encountered. Message will be terminated */
} idigi_data_service_type_t;
/**
* @}
*/


/**
* @defgroup data_service_flags Data Service Flags
* @{
*/
/**
 * This flag is used in to indicate that this is the first message for this
 * data transfer.
 *
 * @see idigi_data_service_put_request callback
 * @see idigi_data_service_device_request callback
 */
#define IDIGI_MSG_FIRST_DATA            0x0001  /**< First chunk of data */

/**
 * This flag is to indicate that this is the last message for this transfer.
 *
 * @see idigi_data_service_put_request callback
 * @see idigi_data_service_device_request callback
 */
#define IDIGI_MSG_LAST_DATA             0x0002  /**< Last chunk of data */

/**
 * This flag is used to indicate tha the message was not processed.
 *
 * @see idigi_data_service_device_request callback
 */
#define IDIGI_MSG_DATA_NOT_PROCESSED    0x0010  /**< This flag is used in idigi_data_service_device_request callback
                                                     telling server that callback did not process the message */
/**
 * Message success bit flag
 * This flag is used in idigi_data_service_block_t indicating
 * message successfully handled.
 *
 * @see idigi_data_service_put_request callback
 */
#define IDIGI_MSG_RESP_SUCCESS          0x0100  /**< This flag is used in idigi_data_service_put_request callback
                                                     telling the callback that server successfully received the message. */
/**
 * This flag is used to indicate that the message was invalid.
 *
 * @see idigi_data_service_put_request callback
 */
#define IDIGI_MSG_BAD_REQUEST           0x0200  /**< This flag is used in idigi_data_service_put_request callback
                                                     from server telling the callback that some portion of the data was invalid. */
/**
 * This flag is used to indicate that the service is unavailable to process the message.
 *
 * @see idigi_data_service_put_request callback
 */
#define IDIGI_MSG_UNAVAILABLE           0x0400  /**< This flag is used in idigi_data_service_put_request callback
                                                     from server telling the callback that service is unavailable due to overload
                                                     or other issues. Callback may try to resend the message later. */
/**
 * This flag is used to indicate that the server encountered an error handling the message.
 *
 * @see idigi_data_service_put_request callback
 */
#define IDIGI_MSG_SERVER_ERROR          0x0800  /**< This flag is used in idigi_data_service_put_request callback
                                                     from server telling the callback that server encountered an error handling the message. */
/**
* @}
*/

/**
* @defgroup.idigi_data_service_block_t Data Service Block
* @{
*/
/**
* Data service block structure is used to send data between iDigi server and client.
* This structure is used in idigi_data_service_put_request and idigi_data_service_device_request callbacks.
*
* When message type is idigi_data_service_type_need_data in idigi_data_service_msg_request_t,
* callback needs to update this structure with data to be sent to server.
*
* When message type is idigi_data_service_type_have_data in idigi_data_service_msg_request_t,
* this structure contains data for callback to process.
*
* @see idigi_data_service_request_t
* @see idigi_initiate_action
* @see idigi_data_service_put_request_t
* @see idigi_data_service_device_request_t
* @see idigi_data_service_type_t

*/
typedef struct
{
    void * data;                /**< Pointer to data */
    size_t length_in_bytes;     /**< Number of bytes in data */
    unsigned int flags;         /**< Bit mask flags. See each callback for specified bit mask flags, defined in @ref data_service_flags*/
} idigi_data_service_block_t;
/**
* @}
*/

/**
* @defgroup idigi_data_service_msg_request_t Data Service Message Request
* @{
*/
/**
* Data service message request structure is used to tell the callback to process data from
* iDigi server, to return data to be sent to iDigi server, or to cancel an active message.
*
* This structure is used in idigi_data_service_put_request and idigi_data_service_device_request callbacks.
*
* @see idigi_data_service_request_t
* @see idigi_initiate_action
* @see idigi_data_service_put_request_t
* @see idigi_data_service_device_request_t
* @see idigi_data_service_type_t
*/
typedef struct
{
    void * service_context;                     /**< Service context is pointer to idigi_data_service_put_request_t for idigi_data_service_put_request callback
                                                    or to idigi_data_service_device_request_t for idigi_data_service_device_request callback. */
    idigi_data_service_type_t message_type;     /**< It contains idigi_data_service_type_need_data to request callback for data to be sent to server,
                                                   idigi_data_service_type_have_data to tell callback to process data from server, or
                                                   idigi_data_service_type_error to tell callback to cancel the message since error is encountered. */
    idigi_data_service_block_t * server_data;   /**< It's pointer to data from server to be processed for idigi_data_service_type_have_data message type or
                                                     pointer to error status for idigi_data_service_type_error message type. */
} idigi_data_service_msg_request_t;
/**
* @}
*/

/**
* @defgroup idigi_data_service_msg_response_t Data service response message
* @{
*/
/**
* Data service message response structure is used in idigi_data_service_put_request
* and idigi_data_service_device_request callbacks to return data to be sent to
* iDigi server, or to cancel the message.
*
* @see idigi_data_service_request_t
* @see idigi_data_service_type_t
*/
typedef struct
{
    void * user_context;                        /**< Used for idigi_data_service_device_request callback's context which will
                                                    be returned on subsequent callbacks for its reference.
                                                    For idigi_data_service_put_request callback, it's not used. */
    idigi_msg_error_t message_status;           /**< Callback writes error status when it encounters error and cancels the message */
    idigi_data_service_block_t * client_data;   /**< Pointer to memory where callback writes data to for idigi_data_service_type_need_data message type */
} idigi_data_service_msg_response_t;
/**
* @}
*/


/**
* @defgroup idigi_data_service_device_request_t Data service device request
* @{
*/
/**
* Data service device request structure is used in @see idigi_data_service_device_request
* callback to process device request. idigi_data_service_device_request callback is passed with
* @see idigi_sevice_msg_request_t where service_context is pointing to this structure.
*
* @see idigi_data_service_type_t
*/
typedef struct
{
    void * device_handle;       /**< Device handle for current device request */
    char const * target;        /**< Contains nul terminated target name. The target name is only provided on
                                    the first chunk of data (IDIGI_MSG_FIRST_DATA bit is set on flags. @see idigi_data_service_block_t).
                                    Otherwise, it’s null. */
} idigi_data_service_device_request_t;
/**
* @}
*/

 /**
 * @defgroup idigi_callback_t Application-defined callback
 *@{ 
 * idigi_callback_t: IIK Application-defined callback, this is the general purpose
 * callback used throughout the IIK.
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
 * @defgroup idigi_init Initialize the IIK.
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
 * @defgroup idigi_step Step Routine
 * @{ 
 * @b Include: idigi_api.h
 */
/**
 * @brief   Run a portion of the IIK.
 *
 * This function is called to start and run the IIK. This
 * function performs a sequence of operations or events and 
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
 * @retval idigi_success                No error. 
 * @retval idigi_init_error             IIK was not initialized. 
 * @retval idigi_configuration_error    IIK was aborted by callback function. 
 * @retval idigi_invalid_data_size      Callback returned configuration with invalid size.
 * @retval idigi_invalid_data_range     Callback returned configuration that is out of range.
 * @retval idigi_invalid_payload_packet IIK received invalid payload message. 
 * @retval idigi_keepalive_error        IIK did not receive keepalive messages. Server may be offline. 
 * @retval idigi_server_overload        Server overload. 
 * @retval idigi_bad_version            Server rejected version number. 
 * @retval idigi_invalid_packet         IIK received unrecognized or unexpected message. 
 * @retval idigi_exceed_timeout         Callback exceeded timeout value before it returned. 
 * @retval idigi_unsupported_security   IIK received a packet with unsupported security. 
 * @retval idigi_invalid_data           Callback returned invalid data. Callback may return a NULL data. 
 * @retval idigi_server_disconnected    Server disconnected IIK. 
 * @retval idigi_connect_error          IIK was unable to connect to the iDigi server. The callback for connect failed. 
 * @retval idigi_receive_error          Unable to receive message from the iDigi server. The callback for receive failed. 
 * @retval idigi_send_error             Unable to send message to the iDigi server. The callback for send failed. 
 * @retval idigi_close_error            Unable to disconnect the connection. The callback for close failed. 
 * @retval idigi_device_terminated      IIK was terminated by user via idigi_initiate_action call. 
 *
 * Example Usage:
 * @code
 *     status = idigi_step(idigi_handle);
 * @endcode 
 *  
 * @see idigi_init
 * @see idigi_handle_t
 * @see idigi_callback_t
 * @see idigi_status_t
 */
idigi_status_t idigi_step(idigi_handle_t const handle);
/**
* @}
*/


 /**
 * @defgroup idigi_run Run routine
 * @{ 
 * @b Include: idigi_api.h
 */
/**
 * @brief   Run the IIK, this function does not return and is used in a 
 * multithreaded environment.
 *
 * This function is similar to idigi_step except it doesn't 
 * return control back to caller unless IIK encounters an error.
 * This function should be executed as a separate thread.
 * 
 * @param [in] handle  Handle returned from idigi_init
 *
 * @retval idigi_success                No error. 
 * @retval idigi_init_error             IIK was not initialized. 
 * @retval idigi_configuration_error    IIK was aborted by callback function. 
 * @retval idigi_invalid_data_size      Callback returned configuration with invalid size.
 * @retval idigi_invalid_data_range     Callback returned configuration that is out of range.
 * @retval idigi_invalid_payload_packet IIK received invalid payload message. 
 * @retval idigi_keepalive_error        IIK did not receive keepalive messages. Server may be offline. 
 * @retval idigi_server_overload        Server overload. 
 * @retval idigi_bad_version            Server rejected version number. 
 * @retval idigi_invalid_packet         IIK received unrecognized or unexpected message. 
 * @retval idigi_exceed_timeout         Callback exceeded timeout value before it returned. 
 * @retval idigi_unsupported_security   IIK received a packet with unsupported security. 
 * @retval idigi_invalid_data           Callback returned invalid data. Callback may return a NULL data. 
 * @retval idigi_server_disconnected    Server disconnected IIK. 
 * @retval idigi_connect_error          IIK was unable to connect to the iDigi server. The callback for connect failed. 
 * @retval idigi_receive_error          Unable to receive message from the iDigi server. The callback for receive failed. 
 * @retval idigi_send_error             Unable to send message to the iDigi server. The callback for send failed. 
 * @retval idigi_close_error            Unable to disconnect the connection. The callback for close failed. 
 * @retval idigi_device_terminated      IIK was terminated by user via idigi_initiate_action call. 
 *
 * Example Usage:
 * @code
 *     status = idigi_run(idigi_handle);
 * @endcode 
 *  
 * @see idigi_init
 * @see idigi_step
 * @see idigi_handle_t
 * @see idigi_callback_t
 * @see idigi_status_t
 */
idigi_status_t idigi_run(idigi_handle_t const handle);
/**
* @}
*/


 /**
 * @defgroup idigi_initiate_action Initiate Action
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
 *                      @li @b idigi_initiate_terminate:
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
 * @param [in] request_data  Pointer to Request data 
 *                      @li @b idigi_initiate_terminate:
 *                          This is NULL.
 *                      @li @b idigi_initiate_data_service:
 *                          Pointer to idigi_data_service_put_request_t.
 * @param [out] response_data  Pointer to Response data
 *                      @li @b idigi_initiate_terminate:
 *                          This is NULL.
 *                      @li @b idigi_initiate_data_service:
 *                          This is NULL.
 *
 * @retval idigi_success  No error
 * @retval idigi_init_error           IIK was not initialized or not connected to the iDigi.
 * @retval idigi_configuration_error  Callback aborted IIK.
 * @retval idigi_invalid_data         Invalid parameter
 * @retval idigi_no_resource          Insufficient memory
 * @retval idigi_service_busy         IIK is busy
 *
 * Example Usage:
 * @code
 *     idigi_data_service_put_request_t  file_info;
 *     :
 *     status = idigi_initiate_action(handle, idigi_initiate_data_service, &file_info, NULL);
 *     :
 *     :
 *     status = idigi_initiate_action(idigi_handle, idigi_initiate_terminate, NULL, NULL);
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
