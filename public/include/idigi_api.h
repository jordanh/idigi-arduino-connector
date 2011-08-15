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
#ifndef _IDIGI_API_H
#define _IDIGI_API_H


#include "idigi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IDIGI_PORT          3197
#define IDIGI_SSL_PORT   3199
/* iDigi return status
 *
 */
typedef enum {
   idigi_success,
   idigi_init_error,
   idigi_configuration_error,
   idigi_invalid_data_size,
   idigi_invalid_data_range,
   idigi_invalid_payload_packet,
   idigi_keepalive_error,
   idigi_server_overload,
   idigi_bad_version,
   idigi_invalid_packet,
   idigi_exceed_timeout,
   idigi_unsupported_security,
   idigi_invalid_data,
   idigi_server_disconnected,
   idigi_connect_error,
   idigi_receive_error,
   idigi_send_error,
   idigi_close_error,
   idigi_device_terminated,
   idigi_service_busy,
   idigi_invalid_response,
} idigi_status_t;

typedef enum {
    idigi_class_config,
    idigi_class_network,
    idigi_class_operating_system,
    idigi_class_firmware,
    idigi_class_rci,
    idigi_class_data_service
} idigi_class_t;

typedef enum {
    /* Request ID to return device ID
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_device_id
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the device ID
     *  response_length = pointer to memory where callback writes the size of the device id (must 16 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = device ID is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_device_id,
    /* Request ID to return vendor ID
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_vendor_id
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the vendor ID
     *  response_length = pointer to memory where callback writes the size of the vendor id (must be 4 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = vendor ID is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_vendor_id,
    /* Request ID to return device type
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_device_type
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the ASCII device type string
     *  response_length = pointer to memory where callback writes the length of the device type (must < 32 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = device type is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_device_type,

    /* Request ID to return server URL
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_server_url
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to the FQDN of server URL
     *  response_length = pointer to memory where callback writes the length of the server URL (must < 255 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = server URL is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_server_url,

    /* Request ID to return connection type
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_connection_type
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to idigi_connection_type_t of connection type
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Connection type  is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_connection_type,
    /* Request ID to return MAC address
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_mac_addr
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to MAC address
     *  response_length = pointer to memory where callback writes the size of MAC address (must be 6 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = MAC address is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
    */
    idigi_config_mac_addr,
    /* Request ID to return link speed for WAN connection type
     * iDigi will not call the callback for link speed if connection type is LAN.
      *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_link_speed
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 4 octet integer link speed
     *  response_length = pointer to memory where callback writes the size of link speed (must be 4 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = Link speed is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_link_speed,
    /* Request ID to return phone number dialed for WAN connection type
     * iDigi will not call the callback for phone number dialed if connection type is LAN.
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_phone number
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to ASCII phone number string. The phone number string contains
     *                  the phone number dialed + any dialing prefixes.
     *  response_length = pointer to memory where callback writes the length of the phone number
     *
     * Callback returns:
     *  idigi_callback_continue = phone number dialed is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_phone_number,

    /* Request ID to return TX keepalive interval
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_tx_keepalive
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 2 octet integer of TX keepalive interval in seconds.
     *                  it must be between 5 and 7200 seconds.
     *  response_length = pointer to memory where callback writes the size of TX keepalive (must be 2 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = Tx keepalive interval is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_tx_keepalive,

    /* Request ID to return RX keepalive interval
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_rx_keepalive
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 2 octet integer of RX keepalive interval in seconds.
     *                  it must be between 5 and 7200 seconds.
     *  response_length = pointer to memory where callback writes the size of RX keepalive (must be 2 bytes)
     *
     * Callback returns:
     *  idigi_callback_continue = Rx keepalive interval is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_rx_keepalive,

    /* Request ID to return wait count (the number of intervals of not receiving a keepalive
     * message after which a connection should be considered lost.
     *
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_wait_count
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to 1 octet integer of wait count.
     *                  it must be between 2 and 64.
     *  response_length = pointer to memory where callback writes the size of wait count (must be 1 byte)
     *
     * Callback returns:
     *  idigi_callback_continue = Wait count is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_wait_count,

    /* Request ID to return IP address
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_ip_addr
     *  request_data = NULL
     *  request_length = 0;
     *  response_data = callback returns pointer to IP address.
     *  response_length = pointer to memory where callback writes the size of IP addresss.
     *                    16 bytes for IPv6 or 4 bytes for IPv4 address.
     *
     * Callback returns:
     *  idigi_callback_continue = IP address is successfully returned
     *  not idigi_callback_continue = error is encountered and abort iDigi.
     */
    idigi_config_ip_addr,

    /* Request ID for error notification.
     * This error status callback is be called to notify caller that iDigi has encountered error.
     *
     * The callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_error_status
     *  request_data = pointer to idigi_error_status_t which error is encountered with
     *  request_length = size of idigi_error_status_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  ignore
     */
    idigi_config_error_status,

    /* Request ID to enable firmware facility
     *
     * The callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_firmware_facility
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to memory in boolean (bool) type where callback writes true to support firmware facility
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully indicated facility support..
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_config_firmware_facility,

    /* Request ID to enable data service over messaging facility
     *
     * The callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_data_service
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to memory in boolean (bool) type where callback writes true to support data service over messaging facility
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully indicated data service support..
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_config_data_service,
    
    /* Request ID to enable data service over messaging facility
     *
     * The callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_rci_service
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to memory in boolean (bool) type where callback writes true to support RCI
     *                  which allows user to remotely configure, control, and exchange device's information
     *                  via iDigi server.
     *
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully indicated RCI support..
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_config_rci_facility

} idigi_config_request_t;

typedef enum {

    /* Request ID to connect iDigi server
     *
     * Callback parameters:
     *  class_id = idigi_class_network
     *  request_id = idigi_config_connect
     *  request_data = pointer to server FQDN which callback will make connection to.
     *                 Callback uses IDIGI_PORT or IDIGI_SSL_PORT port number to establish
     *                 non-secure or secure connection respectively.
     *  request_length = length of the server
     *  response_data = callback returns pointer to idigi_network_handle_t.
     *                              (This is used to for send, receive, & close callbacks)
     *  response_length = pointer to memory where callback writes the size of idigi_network_handle_t.
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully established the connection.
     *  idigi_callback_abort =  abort iDigi.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_network_connect,

    /* Request ID to send data to iDigi server
     *
     * Callback parameters:
     *  class_id = idigi_class_config
     *  request_id = idigi_config_send
     *  request_data = pointer to idigi_write_request_t
     *  request_length = size of idigi_write_request_t
     *  response_data = pointer to memory where callback writes the number of bytes sent to iDigi server.
     *                  This is size_t type.
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully sent all data.
     *  idigi_callback_abort =  abort iDigi.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_network_send,

    /* Request ID to receive data from iDigi server
     *
     * Callback parameters:
     *  class_id = idigi_class_network
     *  request_id = idigi_config_receive
     *  request_data = pointer to idigi_read_request_t
     *  request_length = size of idigi_read_request_t
     *  response_data = pointer to memory where callback writes the number of bytes received from iDigi server.
     *                  This is size_t type.
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully received number of requested bytes.
     *  idigi_callback_abort =  abort iDigi.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_network_receive,

    /* Request ID to close the connection
     *
     * Callback parameters:
     *  class_id = idigi_class_network
     *  request_id = idigi_config_close
     *  request_data = pointer to idigi_network_handle_t associated with a connection through
     *                 the idigi_config_connect callback.
     *  request_length = size of idigi_network_handle_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully sent all data.
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_network_close,

    /* Request ID for server disconnected notification. This is used to tell the callback
     * that server disconnects iDigi.
     *
     * Callback parameters:
     *  class_id = idigi_class_network
     *  request_id = idigi_network_disconnected
     *  request_data = NULL
     *  request_length = 0
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback acknowledges it.
     *  idigi_callback_abort =  abort iDigi.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_network_disconnected,

    /* Request ID for reboot  This is called when server requests to reboot
     * the device.
     *
     * Callback parameters:
     *  class_id = idigi_class_network
     *  request_id = idigi_network_reboot
     *  request_data = NULL
     *  request_length = 0
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback acknowledges it.
     *  idigi_callback_abort =  abort iDigi.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_network_reboot

} idigi_network_request_t;

typedef enum {

    /* Request ID to allocate memory
     *
     * Callback parameters:
     *  class_id = idigi_class_operating_system
     *  request_id = idigi_config_malloc
     *  request_data = pointer to size_t containing number of bytes to be allocated
     *  request_length = size of size_t
     *  response_data = pointer to allocated address
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully allocated memory..
     *  idigi_callback_abort =  abort and exit iDigi.
     *  idigi_callback_busy = memory is not available and callback needs to be called again.
     */
    idigi_os_malloc,

    /* Request ID to free memory that is returned from idigi_config_malloc callback
     *
     * Callback parameters:
     *  class_id = idigi_class_operating_system
     *  request_id = idigi_config_free
     *  request_data = address to be freed
     *  request_length = size of void *
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *    ignore.
     *
     */
    idigi_os_free,

    /* Request ID to return system time in seconds
     *
     * The callback parameters:
     *  class_id = idigi_class_operating_system
     *  request_id = idigi_config_system_time
     *  request_data = NULL
     *  request_length = 0
     *  response_data = pointer to 4 octet integer memory where callback writes the system time in seconds.
     *  response_length = ignore
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully returns the system time..
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_os_system_up_time,

} idigi_os_request_t;


typedef enum {
    /* Request ID to return number of supported firmware download target
     *
     * The callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_target_count
     *  request_data = Pointer to timeout value in seconds.
     *  request_length = size of unsigned
     *  response_data = pointer to 2 octet integer memory where callback writes number of supported targets.
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully returns number of target supported.
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_firmware_target_count,

    /* Request ID to return the target version number
     *
     * The callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_version
     *  request_data = pointer to idigi_fw_config_t
     *  request_length = sizeof idigi_fw_config_t
     *  response_data = pointer to 4 octet integer memory where callback writes the version.
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully returns version number.
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_firmware_version,

    /* Request ID to return the target code size
     *
     * The callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_code_code
     *  request_data = pointer to idigi_fw_config_t
     *  request_length = sizeof idigi_fw_config_t
     *  response_data = pointer to 4 octet integer memory where callback writes the code size.
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully returns the code size.
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_firmware_code_size,

    /* Request ID to return the target description
     *
     * The callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_description
     *  request_data = pointer to idigi_fw_config_t
     *  request_length = sizeof idigi_fw_config_t
     *  response_data = pointer to address of ASCII description string.
     *  response_length = pointer to memory where callback writes the length of the description string.
     *                    The total length of description and name spec must not exceed 127 bytes.
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully returns the description.
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_firmware_description,

    /* Request ID to return the file name spec in regular expression.
     *
     * The callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_name_spec
     *  request_data = pointer to idigi_fw_config_t
     *  request_length = sizeof idigi_fw_config_t
     *  response_data = pointer to address of regular expression string.
     *  response_length = pointer to memory where callback writes the length of the name spec.
     *                    The total length of description and name spec must not exceed 127 bytes.
     *
     * Callback returns:
     *  idigi_callback_continue = Callback successfully returns the name spec.
     *  not idigi_callback_continue =  abort and exit iDigi.
     */
    idigi_firmware_name_spec,

    /* Request ID to request firmware download from server
     *
     * Callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_download_request
     *  request_data = pointer to idigi_fw_download_request_t
     *  request_length = sizeof idigi_fw_download_request_t
     *  response_data = pointer to memory where callback writes the idigi_fw_status_t status to.
     *                  iDigi will send idigi_fw_device_error error status to abort downlaod request
     *                  * for unknown returned status.
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue = Callback has set appropriate status in response_data.
     *  idigi_callback_abort =  abort and exit iDigi.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     *
     */
    idigi_firmware_download_request,

    /* Request ID to pass image data for firmware download.
     *
     * Callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_binary_block
     *  request_data = pointer to idigi_fw_image_data_t
     *  request_length = sizeof idigi_fw_image_data_t
     *  response_data = pointer to memory where callback writes the idigi_fw_status_t status to.
     *                  iDigi will send error status to abort firmware downlaod process.
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue =  Callback has acknowledged.
     *  idigi_callback_abort =  iDigi will send idigi_fw_user_abort to abort firmware download and exit.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_firmware_binary_block,

    /* Request ID to complete firmware download. This is used when server completes sending
     * all image binary data.
     *
     * Callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_download_complete
     *  request_data = pointer to idigi_fw_download_complete_request_t
     *  request_length = sizeof idigi_fw_download_complete_request_t
     *  response_data = pointer to idigi_fw_download_complete_request_t where callback writes response to
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue =  Callback has successfully returned response_data..
     *  idigi_callback_abort =  iDigi will send idigi_fw_user_abort to abort firmware download and exit.
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_firmware_download_complete,

    /* Request ID to abort firmware download. This is called when server abort firmware download.
     *
     * Callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_download_abort
     *  request_data = pointer to idigi_fw_download_abort_t
     *  request_length = sizeof idigi_fw_download_abort_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue =  Callback has acknowledged that server has aborted the download.
     *  idigi_callback_abort = abort and exit iDigi
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_firmware_download_abort,

    /* Request ID to reset target. This is called when server resets target. The callback should
     * not return if it's resetting itself. It may return and continue. However, server may disconnect
     * the device and iDigi will process the idigi_config_disconnected callback.
     *
     * Callback parameters:
     *  class_id = idigi_class_firmware
     *  request_id = idigi_firmware_target_reset
     *  request_data = pointer to idigi_fw_config_t
     *  request_length = sizeof idigi_fw_config_t
     *  response_data = NULL
     *  response_length = NULL
     *
     * Callback returns:
     *  idigi_callback_continue =  Callback has acknowledged that server has reseted the target..
     *  idigi_callback_abort = abort and exit iDigi
     *  idigi_callback_busy = Callback is busy and needs to be called again.
     */
    idigi_firmware_target_reset,
} idigi_firmware_request_t;

typedef enum {
    idigi_data_service_send_complete,
    idigi_data_service_response,
    idigi_data_service_error    
} idigi_data_service_request_t;

typedef enum {
    idigi_initiate_terminate,
    idigi_initiate_data_service
} idigi_initiate_request_t;

typedef enum {
   idigi_lan_connection_type,
   idigi_wan_connection_type
} idigi_connection_type_t;

/* Callback return status
 *
 */
typedef enum  {
    /* successfully and continue */
    idigi_callback_continue,
    /* busy and iDigi will call the callback again */
    idigi_callback_busy,
    /* abort and exit iDigi */
    idigi_callback_abort,
    /* unrecognized or not implemented request */
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
   /* abort status */
   idigi_fw_user_abort,
   idigi_fw_device_error,
   idigi_fw_invalid_offset,
   idigi_fw_invalid_data,
   idigi_fw_hardware_error
} idigi_fw_status_t;

/* download complete status */
typedef enum {
   idigi_fw_download_success,
   idigi_fw_download_checksum_mismatch,
   idigi_fw_download_not_complete
} idigi_fw_download_complete_status_t;

typedef enum {
    idigi_rci_zlib_compression,
    idigi_rci_query_setting,
    idigi_rci_query_state,
    idigi_rci_set_setting,
    idigi_rci_set_state,
    idigi_rci_set_default,
    idigi_rci_do_command,
    idigi_rci_get_setting_descriptor,
    idigi_rci_get_state_descriptor,
    idigi_rci_compress_data,
    idigi_rci_compress_data_done,
    idigi_rci_decompress_data,
    idigi_rci_decompress_data_done
} idigi_rci_request_t;

typedef union {
   idigi_config_request_t config_request;
   idigi_network_request_t network_request;
   idigi_os_request_t os_request;
   idigi_firmware_request_t firmware_request;
   idigi_data_service_request_t data_service_request;
   idigi_rci_request_t rci_request;
} idigi_request_t;

#define idigi_handle_t void *

/* error status structure
 *
 * This structure is used in callback for idigi_config_error_status indicating
 * iDigi has encountered error.
 *
 *
 * @param class_id      Class number for request ID
 * @param request_id    Request ID of which error is found.
 * @param status        Error status of the error.
 *
 */
typedef struct  {
    idigi_class_t class_id;
    idigi_request_t request_id;
 
    idigi_status_t status;
} idigi_error_status_t;


/* Network write structure
 *
 * This structure is used in a callback as request parameter for idigi_config_send to send
 * data to iDigi server.
 *
 * @param network_handle    Pointer to network handle associated with a connection
 *                          through the idigi_config_connect callback.
 * @param buffer            Pointer to data to be sent to iDigi server.
 * @param length            Length of the data in bytes
 * @param timeout           timeout value in second. Callback must return within this
 *                          timeout. This allows iDigi to maintenance keepalive process.
 *                          The connection may be disconnected if keepalive fails.
 */
typedef struct  {
   idigi_network_handle_t *  network_handle;
    uint8_t  const *  buffer;
    size_t      length;
    unsigned    timeout;
} idigi_write_request_t;

/* Network read structure
 *
 * This structure is used in a callback as request parameter for idigi_config_receive to receive
 * data from iDigi server.
 *
 * @param network_handle    Pointer to network handle associated with a connection through
 *                          the idigi_config_connect callback.
 * @param buffer            Pointer to memory where received data will be written to.
 * @param length            length buffer in bytes to be received.
 * @param timeout           timeout value in second. Callback must return within this
 *                          timeout. This allows iDigi to maintenance keepalive process.
 *                          The connection may be disconnected if keepalive fails.
 */
typedef struct  {
   idigi_network_handle_t * network_handle;
    uint8_t  * buffer;
    size_t      length;
    unsigned    timeout;
} idigi_read_request_t;

/* Firmware facility configurations structure
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows iDigi to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number
 *
 */
typedef struct {
   unsigned timeout;
   uint8_t target;
} idigi_fw_config_t;

/* Firmware Download Request structure
 *
 * This is used when server requests firmware dowloand.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows iDigi to maintenance keepalive process. The connection may be
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
} idigi_fw_download_request_t;

/* Firmware Image Binary Data structure
 *
 * This is used after firmware download request from the server for firmware image data.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows iDigi to maintenance keepalive process. The connection may be
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
} idigi_fw_image_data_t;

/* Firmware Download Complete Request structure
 *
 * This is used when the server completes sending all firmware image data.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows iDigi to maintenance keepalive process. The connection may be
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
} idigi_fw_download_complete_request_t;

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
    idigi_fw_download_complete_status_t   status;
} idigi_fw_download_complete_response_t;

/* Firmware Download Abort structure
 *
 * This is used when server aborts firmware downlaod.
 *
 * @param timeout   Timeout value in second which callback must return with this timeout value.
 *                  This allows iDigi to maintenance keepalive process. The connection may be
 *                  disconnected if keepalvie fails.
 * @param target    Target number of which image data.
  * @param status   Status reason why server aborts firmware download.
 *
 */
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

typedef struct
{
    uint16_t session_id;
    uint8_t  status;
    idigi_data_block_t message;
} idigi_data_response_t;

typedef struct
{
    uint16_t session_id;
    idigi_status_t status;
    size_t bytes_sent;
} idigi_data_send_t;

typedef struct
{
    uint16_t session_id;
} idigi_data_session_t;

#define IDIGI_DATA_GET_SESSION_ID(session)    ((idigi_data_session_t *)session)->session_id

typedef struct
{
    uint16_t session_id;
    uint8_t error;
} idigi_data_error_t;

typedef struct {
    unsigned timeout;
    idigi_rci_request_t compression;
    uint8_t * data;
    uint32_t length;
} idigi_rci_data_t;
/*
 * iDigi callback.
 *
 * @param class_id     Class number of the request
 * @param request_id   Request ID of the callback
 * @param request_data Pointer to request data
 * @param request_length Length of request_data.
 * @param response_data  Pointer to requested data for the request ID.
 * @param response_length Pointer to memory where the length of the response data will be written.
 *
 * @return idigi_callback_continue  iDigi will continue
 * @return idigi_callback_busy        iDigi will call the callback again
 * @return idigi_callback_abort       iDigi will stop and exit
 */
typedef idigi_callback_status_t (* idigi_callback_t) (idigi_class_t const class_id, idigi_request_t const request_id,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

/*
 * Initializes iDigi and returns iDigi handle.
 *
 * @param callback      callback function to communicate with iDigi
 *
 * @return 0x0          Unable to initialize iDigi or error is encountered.
 * @return handle      iDigi Handle used throughout iDigi APIs.
 *
 */
idigi_handle_t idigi_init(idigi_callback_t const callback);

/* Starts and runs iDigi. This function performs iDigi process and returns control to a caller.
 * This function starts establishing connection with iDigi server, waits for any
 * request from iDigi server, parses a request and invokes appropriate facility to
 * handle the request. Caller must constantly call this function to process iDigi.
 *
 * iDigi only process one request at a time.
 *
 * @param handle       handle returned from idigi_init.
 *
 * @return idigi_success      No error is encountered and it allows caller to gain back the system control.
 *                          Caller must call this function again to continue iDigi process.
 * @return not idigi_success  Error is encountered and iDigi has closed the connection. Applciation
 *                          may call this function to restart iDigi or idigi_initiate_action to terminated iDigi.
 *
 *
 */
idigi_status_t idigi_step(idigi_handle_t const handle);

/* Starts and runs iDigi. This function is similar to idigi_step except it does not
 * return control to caller unless it encounters error.
 *
 * This function is recommended to be executed as a thread.
 *
 * @param handler       handler returned from idigi_init.
 *
  * @return not idigi_success  Error is encountered and iDigi has closed the connection. Applciation
 *                          may call this function to restart iDigi or idigi_initiate_action to terminated iDigi.
 */
idigi_status_t idigi_run(idigi_handle_t const handle);

/* Dispatch iDigi to perform requested action.
 *
 * @param handler       handler returned from idigi_init.
 * @param request       Request ID:
 *                       idigi_dispatch_terminate = terminates iDigi. It closes the connection and frees
 *                       all allocated memory. If caller calls idigi_step, caller must call idigi_step again
 *                       for iDigi to be terminated. If caller is using idigi_run, idigi_run will be terminated
 *                       and return. Once iDigi is terminated, iDigi cannot restart unless idigi_init is called again.
 *
 *                      idigi_initiate_terminate: terminates and stops iDigi from running. It closes
 *                                                           the connection to the iDigi server and frees all allocated memory.
 *                                                           If the application is using idigi_step , the next call to idigi_step will
 *                                                           terminate the iDigi. If caller is using idigi_run, idigi_run will terminate
 *                                                           and return. Once iDigi is terminated, iDigi cannot restart unless idigi_init is called again.
 *                      idigi_initiate_data_service: this is used to send data to the iDigi server, the data is stored in a file on the server.
 *
 *@param request_data   Pointer to requested data.
 *                         idigi_initiate_terminate: NULL
 *                         idigi_initiate_data_service: Pointer to idigi_data_request_t
 *
 *@param response_data  Pointer to response data.
 *                          idigi_initiate_termiate: NULL
 *                          idigi_initiate_data_service: pointer to the session handle passed in idigi_data_request_t for subsequent calls
 */
idigi_status_t idigi_initiate_action(idigi_handle_t const handle, idigi_initiate_request_t const request, void const * const request_data, void * const response_data);

#ifdef __cplusplus
}
#endif

#endif /* _IDIGI_API_H */
