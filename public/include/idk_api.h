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
#ifndef _IDK_API_H
#define _IDK_API_H


#include "idk_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* IDK return status
 *
 */
typedef enum {
   idk_success,
   idk_init_error,
   idk_invalid_parameter,
   idk_configuration_error,
   idk_invalid_data_size,
   idk_invalid_data_range,  
   idk_invalid_payload_packet,
   idk_keepalive_error,
   idk_server_overload,
   idk_bad_version,
   idk_invalid_packet,
   idk_exceed_timeout,
   idk_unsupported_security,
   idk_invalid_data,
   idk_firmware_error,      
   idk_server_disconnected,
   idk_firwmare_download_error,
   idk_facility_init_error,
   idk_connect_error,
   idk_receive_error,
   idk_send_error,
   idk_close_error,
   idk_device_terminated,
   idk_redirect_error
} idk_status_t;

typedef enum {
    idk_class_base,
    idk_class_firmware,
    idk_class_rci,
    idk_class_messaging
} idk_class_t;

typedef enum {
    /* Callback request ID to return device ID
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_device_id
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the device ID
     *  response_length = pointer to memory where callback writes the size of the device id (must 16 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = device ID is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_device_id,
    /* Callback request ID to return vendor ID
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_vendor_id
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the vendor ID
     *  response_length = pointer to memory where callback writes the size of the vendor id (must be 4 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = vendor ID is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_vendor_id,
    /* Callback request ID to return device type
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_device_type
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the ASCII device type string
     *  response_length = pointer to memory where callback writes the length of the device type (must < 32 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = device type is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_device_type,

    /* Callback request ID to return server URL
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_server_url
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the FQDN of server URL
     *  response_length = pointer to memory where callback writes the length of the server URL (must < 255 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = server URL is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_server_url,

    /* Callback request ID to return password for identity verification
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_password
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the ASCII password string.
     *                             Returns NULL for no identity verification.
     *  response_length = pointer to memory where callback writes the length of the password
     *
     * Callback returns:
     *  idk_callback_continue = password is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_password,

    /* Callback request ID to return connection type
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_connection_type
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to idk_connection_type_t of connection type
     *  response_length = ignore
     *
     * Callback returns:
     *  idk_callback_continue = Connection type  is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_connection_type,
    /* Callback request ID to return MAC address
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_mac_addr
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to MAC address
     *  response_length = pointer to memory where callback writes the size of MAC address (must be 6 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = MAC address is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
    */
    idk_base_mac_addr,
    /* Callback request ID to return link speed for WAN connection type
     * IDK will not call the callback for link speed if connection type is LAN.
      *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_link_speed
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 4 octet integer link speed
     *  response_length = pointer to memory where callback writes the size of link speed (must be 4 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = Link speed is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_link_speed,
    /* Callback request ID to return phone number dialed for WAN connection type
     * IDK will not call the callback for phone number dialed if connection type is LAN.
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_phone number
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to ASCII phone number string. The phone number string contains
     *                  the phone number dialed + any dialing prefixes.
     *  response_length = pointer to memory where callback writes the length of the phone number
     *
     * Callback returns:
     *  idk_callback_continue = phone number dialed is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_phone_number,

    /* Callback request ID to return TX keepalive interval
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_tx_keepalive
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 2 octet integer of TX keepalive interval in seconds.
     *                  it must be between 5 and 7200 seconds.
     *  response_length = pointer to memory where callback writes the size of TX keepalive (must be 2 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = Tx keepalive interval is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_tx_keepalive,

    /* Callback request ID to return RX keepalive interval
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_rx_keepalive
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 2 octet integer of RX keepalive interval in seconds.
     *                  it must be between 5 and 7200 seconds.
     *  response_length = pointer to memory where callback writes the size of RX keepalive (must be 2 bytes)
     *
     * Callback returns:
     *  idk_callback_continue = Rx keepalive interval is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_rx_keepalive,

    /* Callback request ID to return wait count (the number of intervals of not receiving a keepalive
     * message after which a connection should be considered lost.
     *
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_wait_count
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 1 octet integer of wait count.
     *                  it must be between 2 and 64.
     *  response_length = pointer to memory where callback writes the size of wait count (must be 1 byte)
     *
     * Callback returns:
     *  idk_callback_continue = Wait count is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_wait_count,

    /* Callback request ID to return IP address
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_ip_addr
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to IP address.
     *  response_length = pointer to memory where callback writes the size of IP addresss.
     *                    16 bytes for IPv6 or 4 bytes for IPv4 address.
     *
     * Callback returns:
     *  idk_callback_continue = IP address is successfully returned
     *  not idk_callback_continue = error is encountered and abort IDK.
     */
    idk_base_ip_addr,

    /* Callback request ID for error notification.
     * This error status callback is be called to notify caller that IDK has encountered error.
     *
     * The callback parameters:
     *  class = idk_class_base
     *  request = idk_base_error_status
     *  request_data = pointer to idk_error_status_t which error is encountered with
     *  request_length = size of idk_error_status_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = IDK will continue. If this error status callback is called because IDK
     *                                       finds error in a  previous callback, IDK will call the previous callback
     *                                       to correct the error if idk_callback_continue is returned.
     *  not idk_callbcak_continue = Abort  and exit IDK.
     *
     */
    idk_base_error_status,

    /* Callback request ID for server disconnected notification. This is used to tell the callback
     * that server disconnects IDK.
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_disconnected
     *  request_data = NULL
     *  request_length = 0
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback acknowledges it.
     *  idk_callback_abort =  abort IDK.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_base_disconnected,

    /* Callback request ID to connect iDigi server
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_connect
     *  request_data = pointer to idk_connect_request_t containing server FQDN and
     *                 port which callback makes connection to.
     *  request_length = size of idk_connect_request_t
     *  response_data = callback returns pointer to idk_network_handle_t.
     *                              (This is used to for send, receive, & close callbacks)
     *  response_length = pointer to memory where callback writes the size of idk_network_handle_t.
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully established the connection.
     *  idk_callback_abort =  abort IDK.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_base_connect,

    /* Callback request ID to send data to iDigi server
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_send
     *  request_data = pointer to idk_write_request_t
     *  request_length = size of idk_write_request_t
     *  response_data = pointer to memory where callback writes the number of bytes sent to iDigi server.
     *                  This is size_t type.
     *  response_length = ignore
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully sent all data.
     *  idk_callback_abort =  abort IDK.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_base_send,

    /* Callback request ID to receive data from iDigi server
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_receive
     *  request_data = pointer to idk_read_request_t
     *  request_length = size of idk_read_request_t
     *  response_data = pointer to memory where callback writes the number of bytes received from iDigi server.
     *                  This is size_t type.
     *  response_length = ignore
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully received number of requested bytes.
     *  idk_callback_abort =  abort IDK.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_base_receive,

    /* Callback request ID to close the connection
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_close
     *  request_data = pointer to idk_network_handle_t associated with a connection through
     *                 the idk_base_connect callback.
     *  request_length = size of idk_network_handle_t pointer
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully sent all data.
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_base_close,

    /* Callback request ID to allocate memory
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_malloc
     *  request_data = pointer to size_t containing number of bytes to be allocated
     *  request_length = size of size_t
     *  response_data = pointer to allocated address
     *  response_length = ignore
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully allocated memory..
     *  idk_callback_abort =  abort and exit IDK.
     *  idk_callback_busy = memory is not available and callback needs to be called again.
     */
    idk_base_malloc,

    /* Callback request ID to free memory that is returned from idk_base_malloc callback
     *
     * Callback parameters:
     *  class = idk_class_base
     *  request = idk_base_free
     *  request_data = address to be freed
     *  request_length = size of void *
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully freed memory.
     *  not idk_callback_continue =  abort and exit IDK.
     *
     */
    idk_base_free,

    /* callback request ID to return system time in millisecond
     *
     * The callback parameters:
     *  class = idk_class_base
     *  request = idk_base_system_time
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to 4 octet integer memory where callback writes the system time in millisecond.
     *  response_length = ignore
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully returns the system time..
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_base_system_time,

    /* callback request ID to enable firmware facility
     *
     * The callback parameters:
     *  class = idk_class_base
     *  request = idk_base_firmware_facility
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to memory in boolean (bool) type where callback writes true to support firmware facility
     *  response_length = ignore
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully indicated facility support..
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_base_firmware_facility,

//  idk_base_rci_facility,
//  idk_base_messaging_facility
} idk_base_request_t;

typedef enum {
    /* callback request ID to return number of supported firmware download target
     *
     * The callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_target_count
     *  request_data = Pointer to timeout value in seconds.
     *  request_length = size of unsigned
     *  response_data = pointer to 2 octet integer memory where callback writes number of supported targets.
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully returns number of target supported.
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_firmware_target_count,

    /* callback request ID to return the target version number
     *
     * The callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_version
     *  request_data = pointer to idk_fw_config_t
     *  request_length = sizeof idk_fw_config_t
     *  response_data = pointer to 4 octet integer memory where callback writes the version.
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully returns version number.
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_firmware_version,

    /* callback request ID to return the target code size
     *
     * The callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_code_code
     *  request_data = pointer to idk_fw_config_t
     *  request_length = sizeof idk_fw_config_t
     *  response_data = pointer to 4 octet integer memory where callback writes the code size.
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully returns the code size.
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_firmware_code_size,

    /* callback request ID to return the target description
     *
     * The callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_description
     *  request_data = pointer to idk_fw_config_t
     *  request_length = sizeof idk_fw_config_t
     *  response_data = pointer to address of ASCII description string.
     *  response_length = pointer to memory where callback writes the length of the description string.
     *                    The total length of description and name spec must not exceed 127 bytes.
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully returns the description.
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_firmware_description,

    /* callback request ID to return the file name spec in regular expression.
     *
     * The callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_name_spec
     *  request_data = pointer to idk_fw_config_t
     *  request_length = sizeof idk_fw_config_t
     *  response_data = pointer to address of regular expression string.
     *  response_length = pointer to memory where callback writes the length of the name spec.
     *                    The total length of description and name spec must not exceed 127 bytes.
     *
     * Callback returns:
     *  idk_callback_continue = Callback successfully returns the name spec.
     *  not idk_callback_continue =  abort and exit IDK.
     */
    idk_firmware_name_spec,

    /* Callback request ID to request firmware download from server
     *
     * Callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_download_request
     *  request_data = pointer to idk_fw_download_request_t
     *  request_length = sizeof idk_fw_download_request_t
     *  response_data = pointer to memory where callback writes the idk_fw_status_t status to.
     *                  IDK will send idk_fw_device_error error status to abort downlaod request
     *                  * for unknown returned status.
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue = Callback has set appropriate status in response_data.
     *  idk_callback_abort =  abort and exit IDK.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     *
     */
    idk_firmware_download_request,

    /* Callback request ID to pass image data for firmware download.
     *
     * Callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_binary_block
     *  request_data = pointer to idk_fw_image_data_t
     *  request_length = sizeof idk_fw_image_data_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue =  Callback has acknowledged.
     *  idk_callback_abort =  IDK will send idk_fw_user_abort to abort firmware download and exit.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_firmware_binary_block,

    /* Callback request ID to complete firmware download. This is used when server completes sending
     * all image binary data.
     *
     * Callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_download_complete
     *  request_data = pointer to idk_fw_download_complete_request_t
     *  request_length = sizeof idk_fw_download_complete_request_t
     *  response_data = pointer to idk_fw_download_complete_request_t where callback writes response to
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue =  Callback has successfully returned response_data..
     *  idk_callback_abort =  IDK will send idk_fw_user_abort to abort firmware download and exit.
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_firmware_download_complete,

    /* callback request ID to abort firmware download. This is called when server abort firmware download.
     *
     * Callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_download_abort
     *  request_data = pointer to idk_fw_download_abort_t
     *  request_length = sizeof idk_fw_download_abort_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue =  Callback has acknowledged that server has aborted the download.
     *  idk_callback_abort = abort and exit IDK
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_firmware_download_abort,

    /* callback request ID to reset target. This is called when server resets target. The callback should
     * not return if it's resetting itself. It may return and continue. However, server may disconnect
     * the device and IDK will process the idk_base_disconnected callback.
     *
     * Callback parameters:
     *  class = idk_class_firmware
     *  request = idk_firmware_target_reset
     *  request_data = pointer to idk_fw_config_t
     *  request_length = sizeof idk_fw_config_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idk_callback_continue =  Callback has acknowledged that server has reseted the target..
     *  idk_callback_abort = abort and exit IDK
     *  idk_callback_busy = Callback is busy and needs to be called again.
     */
    idk_firmware_target_reset,
} idk_firmware_request_t;

typedef enum {
    idk_dispatch_terminate,
    idk_dispatch_stop
} idk_dispatch_request_t;

typedef enum {
   idk_lan_connection_type,
   idk_wan_connection_type
} idk_connection_type_t;

/* Callback return status
 *
 */
typedef enum  {
    /* successfully and continue */
    idk_callback_continue,
    /* busy and IDK will call the callback again */
    idk_callback_busy,
    /* abort and exit IDK */
    idk_callback_abort
} idk_callback_status_t;

typedef enum {
   idk_fw_success,
   idk_fw_download_denied,
   idk_fw_download_invalid_size,
   idk_fw_download_invalid_version,
   idk_fw_download_unauthenticated,
   idk_fw_download_not_allowed,
   idk_fw_download_reject,
   idk_fw_encounterted_error,
   /* abort status */
   idk_fw_user_abort,
   idk_fw_device_error,
   idk_fw_invalid_offset,
   idk_fw_invalid_data,
   idk_fw_hardware_error
} idk_fw_status_t;

/* download complete status */
typedef enum {
   idk_fw_download_success,
   idk_fw_download_checksum_mismatch,
   idk_fw_download_not_complete
} idk_fw_download_complete_status_t;


typedef union {
   idk_base_request_t base_request;
   idk_firmware_request_t firmware_request;
//   idk_rci_request_t rci_request;
//   idk_messaging_request_t messaging_request;
} idk_request_t;

#define idk_handle_t void *

/* error status structure
 *
 * This structure is used in callback for idk_base_error_status indicating
 * IDK has encountered error.
 *
 *
 * @param class         Class number for request ID
 * @param request       Callback request ID of which error is found.
 * @param status        Error status of the error.
 *
 */
typedef struct  {
    idk_class_t class;
    idk_request_t request;
 
    idk_status_t status;
} idk_error_status_t;

/* network connect structure
 *
 * This structure is used in a callback for idk_base_connect request ID to
 * establish connection between a device and iDigi server.
 *
 * @param host_name     Pointer to FQDN of iDigi server to connect to.
 * @param port          Port number to connect to.
 *
 */
typedef struct  {
    char * host_name;
    unsigned    port;
} idk_connect_request_t;

/* Network write structure
 *
 * This structure is used in a callback as request parameter for idk_base_send to send
 * data to iDigi server.
 *
 * @param network_handle    Pointer to network handle associated with a connection
 *                          through the idk_base_connect callback.
 * @param buffer            Pointer to data to be sent to iDigi server.
 * @param length            Length of the data in bytes
 * @param timeout           timeout value in second. Callback must return within this
 *                          timeout. This allows IDK to maintenance keepalive process.
 *                          The connection may be disconnected if keepalive fails.
 */
typedef struct  {
   idk_network_handle_t * network_handle;
    uint8_t     * buffer;
    size_t      length;
    unsigned    timeout;
} idk_write_request_t;

/* Network read structure
 *
 * This structure is used in a callback as request parameter for idk_base_receive to receive
 * data from iDigi server.
 *
 * @param network_handle    Pointer to network handle associated with a connection through
 *                          the idk_base_connect callback.
 * @param buffer            Pointer to memory where received data will be written to.
 * @param length            length buffer in bytes to be received.
 * @param timeout           timeout value in second. Callback must return within this
 *                          timeout. This allows IDK to maintenance keepalive process.
 *                          The connection may be disconnected if keepalive fails.
 */
typedef struct  {
   idk_network_handle_t * network_handle;
    uint8_t     * buffer;
    size_t      length;
    unsigned    timeout;
} idk_read_request_t;

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
} idk_fw_config_t;

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
} idk_fw_download_request_t;

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
} idk_fw_image_data_t;

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
} idk_fw_download_complete_request_t;

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
    idk_fw_download_complete_status_t   status;
} idk_fw_download_complete_response_t;

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
    idk_fw_status_t status;
} idk_fw_download_abort_t;


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
 * @return idk_callback_continue  IDK will continue
 * @return idk_callback_busy        IDK will call the callback again
 * @return idk_callback_abort       IDK will stop and exit
 */
typedef idk_callback_status_t (* idk_callback_t) (idk_class_t class, idk_request_t request, 
                                                  void const * request_data, size_t request_length, 
                                                  void * response_data, size_t * response_length);

/*
 * Allocates and initializes IDK. It will call the callback to get device ID, vendor ID and
 * device type configurations. This function returns IDK handle if IDK is ready.
 *
 * @param callback      callback function to communicate with IDK
 *
 * @return 0x0          Unable to initialize IRL or error is encountered.
 * @return handler      IRL Handler used throughout IRL API.
 *
 */
idk_handle_t idk_init(idk_callback_t callback);

/* Starts and runs IDK. This function performs IDK process and returns control to a caller.
 * This function starts establishing connection with iDigi server, waits for any
 * request from iDigi server, parses a request and invokes appropriate facility to
 * handle the request. Caller must constantly call this function to process IDK.
 *
 * IDK only process one request at a time.
 *
 * @param handler       handler returned from idk_init.
 *
 * @return idk_success      No error is encountered and it allows caller to gain back the system control.
 *                          Caller must call this function again to continue IDK process.
 * @return not idk_success  Error is encountered and IDK has closed the connection. Applciation
 *                          may call this function to restart IRL or idk_stop to terminated IRL.
 *
 *
 */
idk_status_t idk_step(idk_handle_t handle);

//idk_status_t idk_run(idk_handle_t handle);
//idk_status_t idk_dispatch(idk_handle_t handle, idk_dispatch_request_t request);

#ifdef __cplusplus
}
#endif

#endif /* _IDK_API_H */
