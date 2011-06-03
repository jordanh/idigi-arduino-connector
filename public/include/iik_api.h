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
#ifndef _IIK_API_H
#define _IIK_API_H


#include "iik_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* IDK return status
 *
 */
typedef enum {
   iik_success,
   iik_init_error,
   iik_configuration_error,
   iik_invalid_data_size,
   iik_invalid_data_range,
   iik_invalid_payload_packet,
   iik_keepalive_error,
   iik_server_overload,
   iik_bad_version,
   iik_invalid_packet,
   iik_exceed_timeout,
   iik_unsupported_security,
   iik_invalid_data,
   iik_server_disconnected,
   iik_connect_error,
   iik_receive_error,
   iik_send_error,
   iik_close_error,
   iik_device_terminated,
} iik_status_t;

typedef enum {
    iik_class_config,
    iik_class_network,
    iik_class_operating_system,
    iik_class_firmware,
    iik_class_rci,
    iik_class_messaging
} iik_class_t;

typedef enum {
    /* Callback request ID to return device ID
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_device_id
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the device ID
     *  response_length = pointer to memory where callback writes the size of the device id (must 16 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = device ID is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_device_id,
    /* Callback request ID to return vendor ID
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_vendor_id
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the vendor ID
     *  response_length = pointer to memory where callback writes the size of the vendor id (must be 4 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = vendor ID is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_vendor_id,
    /* Callback request ID to return device type
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_device_type
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the ASCII device type string
     *  response_length = pointer to memory where callback writes the length of the device type (must < 32 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = device type is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_device_type,

    /* Callback request ID to return server URL
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_server_url
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the FQDN of server URL
     *  response_length = pointer to memory where callback writes the length of the server URL (must < 255 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = server URL is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_server_url,

    /* Callback request ID to return password for identity verification
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_password
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the ASCII password string.
     *                             Returns NULL for no identity verification.
     *  response_length = pointer to memory where callback writes the length of the password
     *
     * Callback returns:
     *  iik_callback_continue = password is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_password,

    /* Callback request ID to return connection type
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_connection_type
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to iik_connection_type_t of connection type
     *  response_length = ignore
     *
     * Callback returns:
     *  iik_callback_continue = Connection type  is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_connection_type,
    /* Callback request ID to return MAC address
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_mac_addr
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to MAC address
     *  response_length = pointer to memory where callback writes the size of MAC address (must be 6 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = MAC address is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
    */
    iik_config_mac_addr,
    /* Callback request ID to return link speed for WAN connection type
     * IDK will not call the callback for link speed if connection type is LAN.
      *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_link_speed
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 4 octet integer link speed
     *  response_length = pointer to memory where callback writes the size of link speed (must be 4 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = Link speed is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_link_speed,
    /* Callback request ID to return phone number dialed for WAN connection type
     * IDK will not call the callback for phone number dialed if connection type is LAN.
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_phone number
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to ASCII phone number string. The phone number string contains
     *                  the phone number dialed + any dialing prefixes.
     *  response_length = pointer to memory where callback writes the length of the phone number
     *
     * Callback returns:
     *  iik_callback_continue = phone number dialed is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_phone_number,

    /* Callback request ID to return TX keepalive interval
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_tx_keepalive
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 2 octet integer of TX keepalive interval in seconds.
     *                  it must be between 5 and 7200 seconds.
     *  response_length = pointer to memory where callback writes the size of TX keepalive (must be 2 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = Tx keepalive interval is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_tx_keepalive,

    /* Callback request ID to return RX keepalive interval
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_rx_keepalive
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 2 octet integer of RX keepalive interval in seconds.
     *                  it must be between 5 and 7200 seconds.
     *  response_length = pointer to memory where callback writes the size of RX keepalive (must be 2 bytes)
     *
     * Callback returns:
     *  iik_callback_continue = Rx keepalive interval is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_rx_keepalive,

    /* Callback request ID to return wait count (the number of intervals of not receiving a keepalive
     * message after which a connection should be considered lost.
     *
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_wait_count
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 1 octet integer of wait count.
     *                  it must be between 2 and 64.
     *  response_length = pointer to memory where callback writes the size of wait count (must be 1 byte)
     *
     * Callback returns:
     *  iik_callback_continue = Wait count is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_wait_count,

    /* Callback request ID to return IP address
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_ip_addr
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to IP address.
     *  response_length = pointer to memory where callback writes the size of IP addresss.
     *                    16 bytes for IPv6 or 4 bytes for IPv4 address.
     *
     * Callback returns:
     *  iik_callback_continue = IP address is successfully returned
     *  not iik_callback_continue = error is encountered and abort IDK.
     */
    iik_config_ip_addr,

    /* Callback request ID for error notification.
     * This error status callback is be called to notify caller that IDK has encountered error.
     *
     * The callback parameters:
     *  class = iik_class_config
     *  request = iik_config_error_status
     *  request_data = pointer to iik_error_status_t which error is encountered with
     *  request_length = size of iik_error_status_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = IDK will continue. If this error status callback is called because IDK
     *                                       finds error in a  previous callback, IDK will call the previous callback
     *                                       to correct the error if iik_callback_continue is returned.
     *  not iik_callbcak_continue = Abort  and exit IDK.
     *
     */
    iik_config_error_status,

    /* Callback request ID for server disconnected notification. This is used to tell the callback
     * that server disconnects IDK.
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_disconnected
     *  request_data = NULL
     *  request_length = 0
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback acknowledges it.
     *  iik_callback_abort =  abort IDK.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_config_disconnected,


    /* callback request ID to enable firmware facility
     *
     * The callback parameters:
     *  class = iik_class_config
     *  request = iik_config_firmware_facility
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to memory in boolean (bool) type where callback writes true to support firmware facility
     *  response_length = ignore
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully indicated facility support..
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_config_firmware_facility,

} iik_config_request_t;

typedef enum {

    /* Callback request ID to connect iDigi server
     *
     * Callback parameters:
     *  class = iik_class_network
     *  request = iik_config_connect
     *  request_data = pointer to iik_connect_request_t containing server FQDN and
     *                 port which callback makes connection to.
     *  request_length = size of iik_connect_request_t
     *  response_data = callback returns pointer to iik_network_handle_t.
     *                              (This is used to for send, receive, & close callbacks)
     *  response_length = pointer to memory where callback writes the size of iik_network_handle_t.
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully established the connection.
     *  iik_callback_abort =  abort IDK.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_network_connect,

    /* Callback request ID to send data to iDigi server
     *
     * Callback parameters:
     *  class = iik_class_config
     *  request = iik_config_send
     *  request_data = pointer to iik_write_request_t
     *  request_length = size of iik_write_request_t
     *  response_data = pointer to memory where callback writes the number of bytes sent to iDigi server.
     *                  This is size_t type.
     *  response_length = ignore
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully sent all data.
     *  iik_callback_abort =  abort IDK.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_network_send,

    /* Callback request ID to receive data from iDigi server
     *
     * Callback parameters:
     *  class = iik_class_network
     *  request = iik_config_receive
     *  request_data = pointer to iik_read_request_t
     *  request_length = size of iik_read_request_t
     *  response_data = pointer to memory where callback writes the number of bytes received from iDigi server.
     *                  This is size_t type.
     *  response_length = ignore
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully received number of requested bytes.
     *  iik_callback_abort =  abort IDK.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_network_receive,

    /* Callback request ID to close the connection
     *
     * Callback parameters:
     *  class = iik_class_network
     *  request = iik_config_close
     *  request_data = pointer to iik_network_handle_t associated with a connection through
     *                 the iik_config_connect callback.
     *  request_length = size of iik_network_handle_t pointer
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully sent all data.
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_network_close,

} iik_network_request_t;

typedef enum {

    /* Callback request ID to allocate memory
     *
     * Callback parameters:
     *  class = iik_class_operating_system
     *  request = iik_config_malloc
     *  request_data = pointer to size_t containing number of bytes to be allocated
     *  request_length = size of size_t
     *  response_data = pointer to allocated address
     *  response_length = ignore
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully allocated memory..
     *  iik_callback_abort =  abort and exit IDK.
     *  iik_callback_busy = memory is not available and callback needs to be called again.
     */
    iik_os_malloc,

    /* Callback request ID to free memory that is returned from iik_config_malloc callback
     *
     * Callback parameters:
     *  class = iik_class_operating_system
     *  request = iik_config_free
     *  request_data = address to be freed
     *  request_length = size of void *
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully freed memory.
     *  not iik_callback_continue =  abort and exit IDK.
     *
     */
    iik_os_free,

    /* callback request ID to return system time in millisecond
     *
     * The callback parameters:
     *  class = iik_class_operating_system
     *  request = iik_config_system_time
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to 4 octet integer memory where callback writes the system time in millisecond.
     *  response_length = ignore
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully returns the system time..
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_os_system_up_time,

} iik_os_request_t;


typedef enum {
    /* callback request ID to return number of supported firmware download target
     *
     * The callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_target_count
     *  request_data = Pointer to timeout value in seconds.
     *  request_length = size of unsigned
     *  response_data = pointer to 2 octet integer memory where callback writes number of supported targets.
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully returns number of target supported.
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_firmware_target_count,

    /* callback request ID to return the target version number
     *
     * The callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_version
     *  request_data = pointer to iik_fw_config_t
     *  request_length = sizeof iik_fw_config_t
     *  response_data = pointer to 4 octet integer memory where callback writes the version.
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully returns version number.
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_firmware_version,

    /* callback request ID to return the target code size
     *
     * The callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_code_code
     *  request_data = pointer to iik_fw_config_t
     *  request_length = sizeof iik_fw_config_t
     *  response_data = pointer to 4 octet integer memory where callback writes the code size.
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully returns the code size.
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_firmware_code_size,

    /* callback request ID to return the target description
     *
     * The callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_description
     *  request_data = pointer to iik_fw_config_t
     *  request_length = sizeof iik_fw_config_t
     *  response_data = pointer to address of ASCII description string.
     *  response_length = pointer to memory where callback writes the length of the description string.
     *                    The total length of description and name spec must not exceed 127 bytes.
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully returns the description.
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_firmware_description,

    /* callback request ID to return the file name spec in regular expression.
     *
     * The callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_name_spec
     *  request_data = pointer to iik_fw_config_t
     *  request_length = sizeof iik_fw_config_t
     *  response_data = pointer to address of regular expression string.
     *  response_length = pointer to memory where callback writes the length of the name spec.
     *                    The total length of description and name spec must not exceed 127 bytes.
     *
     * Callback returns:
     *  iik_callback_continue = Callback successfully returns the name spec.
     *  not iik_callback_continue =  abort and exit IDK.
     */
    iik_firmware_name_spec,

    /* Callback request ID to request firmware download from server
     *
     * Callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_download_request
     *  request_data = pointer to iik_fw_download_request_t
     *  request_length = sizeof iik_fw_download_request_t
     *  response_data = pointer to memory where callback writes the iik_fw_status_t status to.
     *                  IDK will send iik_fw_device_error error status to abort downlaod request
     *                  * for unknown returned status.
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue = Callback has set appropriate status in response_data.
     *  iik_callback_abort =  abort and exit IDK.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     *
     */
    iik_firmware_download_request,

    /* Callback request ID to pass image data for firmware download.
     *
     * Callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_binary_block
     *  request_data = pointer to iik_fw_image_data_t
     *  request_length = sizeof iik_fw_image_data_t
     *  response_data = pointer to memory where callback writes the iik_fw_status_t status to.
     *                  IDK will send error status to abort firmware downlaod process.
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue =  Callback has acknowledged.
     *  iik_callback_abort =  IDK will send iik_fw_user_abort to abort firmware download and exit.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_firmware_binary_block,

    /* Callback request ID to complete firmware download. This is used when server completes sending
     * all image binary data.
     *
     * Callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_download_complete
     *  request_data = pointer to iik_fw_download_complete_request_t
     *  request_length = sizeof iik_fw_download_complete_request_t
     *  response_data = pointer to iik_fw_download_complete_request_t where callback writes response to
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue =  Callback has successfully returned response_data..
     *  iik_callback_abort =  IDK will send iik_fw_user_abort to abort firmware download and exit.
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_firmware_download_complete,

    /* callback request ID to abort firmware download. This is called when server abort firmware download.
     *
     * Callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_download_abort
     *  request_data = pointer to iik_fw_download_abort_t
     *  request_length = sizeof iik_fw_download_abort_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue =  Callback has acknowledged that server has aborted the download.
     *  iik_callback_abort = abort and exit IDK
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_firmware_download_abort,

    /* callback request ID to reset target. This is called when server resets target. The callback should
     * not return if it's resetting itself. It may return and continue. However, server may disconnect
     * the device and IDK will process the iik_config_disconnected callback.
     *
     * Callback parameters:
     *  class = iik_class_firmware
     *  request = iik_firmware_target_reset
     *  request_data = pointer to iik_fw_config_t
     *  request_length = sizeof iik_fw_config_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  iik_callback_continue =  Callback has acknowledged that server has reseted the target..
     *  iik_callback_abort = abort and exit IDK
     *  iik_callback_busy = Callback is busy and needs to be called again.
     */
    iik_firmware_target_reset,
} iik_firmware_request_t;

typedef enum {
    iik_dispatch_terminate,
    iik_dispatch_put_service
} iik_dispatch_request_t;

typedef enum {
   iik_lan_connection_type,
   iik_wan_connection_type
} iik_connection_type_t;

/* Callback return status
 *
 */
typedef enum  {
    /* successfully and continue */
    iik_callback_continue,
    /* busy and IDK will call the callback again */
    iik_callback_busy,
    /* abort and exit IDK */
    iik_callback_abort,
    /* unrecognized or not implemented request */
    iik_callback_unrecognized

} iik_callback_status_t;

typedef enum {
   iik_fw_success,
   iik_fw_download_denied,
   iik_fw_download_invalid_size,
   iik_fw_download_invalid_version,
   iik_fw_download_unauthenticated,
   iik_fw_download_not_allowed,
   iik_fw_download_configured_to_reject,
   iik_fw_encountered_error,
   /* abort status */
   iik_fw_user_abort,
   iik_fw_device_error,
   iik_fw_invalid_offset,
   iik_fw_invalid_data,
   iik_fw_hardware_error
} iik_fw_status_t;

/* download complete status */
typedef enum {
   iik_fw_download_success,
   iik_fw_download_checksum_mismatch,
   iik_fw_download_not_complete
} iik_fw_download_complete_status_t;


typedef union {
   iik_config_request_t config_request;
   iik_network_request_t network_request;
   iik_os_request_t os_request;
   iik_firmware_request_t firmware_request;
} iik_request_t;

#define iik_handle_t void *

/* error status structure
 *
 * This structure is used in callback for iik_config_error_status indicating
 * IDK has encountered error.
 *
 *
 * @param class         Class number for request ID
 * @param request       Callback request ID of which error is found.
 * @param status        Error status of the error.
 *
 */
typedef struct  {
    iik_class_t class;
    iik_request_t request;
 
    iik_status_t status;
} iik_error_status_t;

/* network connect structure
 *
 * This structure is used in a callback for iik_config_connect request ID to
 * establish connection between a device and iDigi server.
 *
 * @param host_name     Pointer to FQDN of iDigi server to connect to.
 * @param port          Port number to connect to.
 *
 */
typedef struct  {
    char * host_name;
    unsigned    port;
} iik_connect_request_t;

/* Network write structure
 *
 * This structure is used in a callback as request parameter for iik_config_send to send
 * data to iDigi server.
 *
 * @param network_handle    Pointer to network handle associated with a connection
 *                          through the iik_config_connect callback.
 * @param buffer            Pointer to data to be sent to iDigi server.
 * @param length            Length of the data in bytes
 * @param timeout           timeout value in second. Callback must return within this
 *                          timeout. This allows IDK to maintenance keepalive process.
 *                          The connection may be disconnected if keepalive fails.
 */
typedef struct  {
   iik_network_handle_t * network_handle;
    uint8_t     * buffer;
    size_t      length;
    unsigned    timeout;
} iik_write_request_t;

/* Network read structure
 *
 * This structure is used in a callback as request parameter for iik_config_receive to receive
 * data from iDigi server.
 *
 * @param network_handle    Pointer to network handle associated with a connection through
 *                          the iik_config_connect callback.
 * @param buffer            Pointer to memory where received data will be written to.
 * @param length            length buffer in bytes to be received.
 * @param timeout           timeout value in second. Callback must return within this
 *                          timeout. This allows IDK to maintenance keepalive process.
 *                          The connection may be disconnected if keepalive fails.
 */
typedef struct  {
   iik_network_handle_t * network_handle;
    uint8_t     * buffer;
    size_t      length;
    unsigned    timeout;
} iik_read_request_t;

/* Firmware facility configurations structure
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows IDK to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number
 *
 */
typedef struct {
   unsigned timeout;
   uint8_t target;
} iik_fw_config_t;

/* Firmware Download Request structure
 *
 * This is used when server requests firmware dowloand.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows IDK to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number of which firmware download request for
 * @param version   Version number of the download target. It uses 0xFFFFFFFF for unknown version number.
 * @param code_size Code size that is ready to be sent.
 * @param desc_string Pointer to description string.
 * @param file_name_spec Pointer to name spec in regular expression.
 * @param filename       Pointer to filename of the image to be downloaded.
 *
 */
typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t    version;
    uint32_t    code_size;
    char        * desc_string;
    char        * file_name_spec;
    char        * filename;
} iik_fw_download_request_t;

/* Firmware Image Binary Data structure
 *
 * This is used after firmware download request from the server for firmware image data.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows IDK to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number of which image data for
 * @param offset    Offset location
 * @param data      Pointer to image data that is positioned at offset from the base position.
 * @param length    Length of the image data
 *
 */
typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t offset;
    uint8_t * data;
    size_t length;
} iik_fw_image_data_t;

/* Firmware Download Complete Request structure
 *
 * This is used when the server completes sending all firmware image data.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows IDK to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number
 * @param code_size This should be the same code size as sent in the Firmware Downlaod request.
 * @param checksum  Currently not used
 *
 */
typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t    code_size;
    uint32_t    checksum;
} iik_fw_download_complete_request_t;

/* Firmware Download Complete Response structure
 *
 * @param version   Version number of the image
 * @param calculated_checksum  Currently it's used for error code sent to server.
 * @param status    Download complete status
 *
 */
typedef struct {
    uint32_t    version;
    uint32_t    calculated_checksum;
    iik_fw_download_complete_status_t   status;
} iik_fw_download_complete_response_t;

/* Firmware Download Abort structure
 *
 * This is used when server aborts firmware downlaod.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows IDK to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number of which image data.
  * @param status   Status reason why server aborts firmware download.
 *
 */
typedef struct {
    unsigned timeout;
    uint8_t target;
    iik_fw_status_t status;
} iik_fw_download_abort_t;


/*
 * IDK callback.
 *
 * @param class     Class number of the request
 * @param request   Request ID of the callback
 * @param request_data Pointer to request data
 * @param request_length Length of request_data.
 * @param response_data  Pointer to requested data for the request ID.
 * @param response_length Pointer to memory where the length of the response data will be written.
 *
 * @return iik_callback_continue  IDK will continue
 * @return iik_callback_busy        IDK will call the callback again
 * @return iik_callback_abort       IDK will stop and exit
 */
typedef iik_callback_status_t (* iik_callback_t) (iik_class_t class, iik_request_t request,
                                                  void const * request_data, size_t request_length, 
                                                  void * response_data, size_t * response_length);

/*
 * device type configurations. This function returns IDK handle if IDK is ready.
 *
 * @param callback      callback function to communicate with IDK
 *
 * @return 0x0          Unable to initialize IRL or error is encountered.
 * @return handler      IRL Handler used throughout IRL API.
 *
 */
iik_handle_t iik_init(iik_callback_t const callback);

/* Starts and runs IDK. This function performs IDK process and returns control to a caller.
 * This function starts establishing connection with iDigi server, waits for any
 * request from iDigi server, parses a request and invokes appropriate facility to
 * handle the request. Caller must constantly call this function to process IDK.
 *
 * IDK only process one request at a time.
 *
 * @param handler       handler returned from iik_init.
 *
 * @return iik_success      No error is encountered and it allows caller to gain back the system control.
 *                          Caller must call this function again to continue IDK process.
 * @return not iik_success  Error is encountered and IDK has closed the connection. Applciation
 *                          may call this function to restart IRL or iik_dispatch to terminated IRL.
 *
 *
 */
iik_status_t iik_step(iik_handle_t const handle);

/* Starts and run IDK. This function is similar to iik_step except it does not
 * return control to caller unless it encounters error.
 *
 * This function is recommended to be executed as a thread.
 *
 * @param handler       handler returned from iik_init.
 *
  * @return not iik_success  Error is encountered and IDK has closed the connection. Applciation
 *                          may call this function to restart IRL or iik_dispatch to terminated IRL.
 */
iik_status_t iik_run(iik_handle_t const handle);

/* Dispatch IDK to perform requested action.
 *
 * @param handler       handler returned from iik_init.
 * @param request       Request ID:
 *                       iik_dispatch_terminate = terminates IDK. It closes the connection and frees
 *                       all allocated memory. If caller calls iik_step, caller must call iik_step again
 *                       for IDK to be terminated. If caller is using iik_run, iik_run will be terminated
 *                       and return. Once IDK is terminated, IDK cannot restart unless iik_init is called again.
 *@param request_data   Pointer to requested data.
                        For Request ID:
                            iik_dispatch_termiated: data is not used
 *@param response_data  Pointer to response data.
                        For Request ID:
                            iik_dispatch_termiated: data is not used
 */
iik_status_t iik_initiate_action(iik_handle_t handle, iik_dispatch_request_t request, void const * request_data, void * response_data);

#ifdef __cplusplus
}
#endif

#endif /* _IIK_API_H */
