/*
 * Copyright (c) 2011, 2012 Digi International Inc.,
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
  *  @brief Configuration routines for the iDigi Connector.
  *
  */
#include <stdio.h>
#include <ctype.h>

#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"

/**
 * @brief   Get the IP address of the device
 *
 * This routine assigns a pointer to the IP address of the device in *ip_address
 * along with the size of the IP address which must be either a 4-octet value for
 * IPv4 or a 6-octet value for IPv6.
 *
 * @param [out] ip_address  Pointer to memory containing IP address
 * @param [out] size Size of the IP address in bytes
 *
 * @retval idigi_callback_continue  IP address was successfully returned
 * @retval idigi_callback_abort     Could not get IP address and abort iDigi connector.
 *
 * @see @ref ip_address API Configuration Callback
 */
static idigi_callback_status_t app_get_ip_address(uint8_t const ** ip_address, size_t * const size)
{
    /* Remove this #error statement once you modify this routine to return the correct IP address */
#error "Specify device IP address. Set size to 4 (bytes) for IPv4 or 16 (bytes) for IPv6"

    UNUSED_ARGUMENT(ip_address);
    UNUSED_ARGUMENT(size);

    return idigi_callback_continue;
}

/**
 * @brief   Get the MAC address of the device
 *
 * This routine assigns a pointer to the MAC address of the device in *mac_address along
 * with the size.
 *
 * @param [out] mac_address  Pointer to memory containing IP address
 * @param [out] size Size of the MAC address in bytes (6 bytes).
 *
 * @retval idigi_callback_continue  MAC address was successfully returned
 * @retval idigi_callback_abort     Could not get the MAC address and abort iDigi connector.
 *
 * @see @ref mac_address API Configuration Callback
 */
static idigi_callback_status_t app_get_mac_addr(uint8_t const ** mac_address, size_t * const size)
{
    #define MAC_ADDR_LENGTH     6

    /* MAC address used in this sample */
    static uint8_t const device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#error "Specify device MAC address for LAN connection"

    *mac_address = device_mac_addr;
    *size = sizeof device_mac_addr;

    return idigi_callback_continue;
}

/**
 * @brief   Get the iDigi device ID
 *
 * This routine is called to get a unique device ID which is used to identify the device.
 *
 * Device IDs are a globally unique identifier for iDigi clients.  The Device ID is a
 * 16-octet value derived from the MAC address of a network interface on the client.
 * The mapping from MAC address to Device ID consists of inserting "FFFF" in the middle
 * of the MAC and setting all other bytes of the Device ID to 0.
 * For Example:
 * MAC Address 12:34:56:78:9A:BC, would map to a Device ID: 00000000-123456FF-FF789ABC.
 * If a client has more than one network interface, it does not matter to iDigi which
 * network interface MAC is used for the basis of the Device ID.  If the MAC is read
 * directly from the network interface to generate the client's Device ID, care must be
 * taken to always use the same network interface's MAC since there is a unique mapping
 * between a device and a Device ID.
 *
 * The pointer ID is filled in with the address of the memory location which contains the
 * device ID, size is filled in with the size of the device ID.
. *
 * @param [out] id  Pointer to memory containing the device ID
 * @param [out] size Size of the device ID in bytes (16 bytes)
 *
 * @retval idigi_callback_continue  Device ID was successfully returned.
 * @retval idigi_callback_abort     Could not get the device ID and abort iDigi connector.
 *
 * @see @ref device_id API Configuration Callback
 */
static idigi_callback_status_t app_get_device_id(uint8_t const ** id, size_t * const size)
{
    #define DEVICE_ID_LENGTH    16

    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};

    uint8_t const * mac_addr;
    size_t mac_size;
    idigi_callback_status_t status;

#error  "Specify device id"

    /* This sample uses the MAC address to format the device ID */
    status  = app_get_mac_addr(&mac_addr, &mac_size);

    if (status == idigi_callback_continue)
    {
        device_id[8] = mac_addr[0];
        device_id[9] = mac_addr[1];
        device_id[10] = mac_addr[2];
        device_id[11] = 0xFF;
        device_id[12] = 0xFF;
        device_id[13] = mac_addr[3];
        device_id[14] = mac_addr[4];
        device_id[15] = mac_addr[5];

        *id   = device_id;
        *size = sizeof device_id;
    }

    return status;
}

/**
 * @brief   Get the iDigi vendor ID
 *
 * This routine assigns a pointer to the vendor ID which is a unique code identifying
 * the manufacturer of a device. Vendor IDs are assigned to manufacturers by iDigi.
 *
 * @param [out] id  Pointer to memory containing the device ID
 * @param [out] size Size of the vendor ID in bytes (4 bytes)
 *
 * @retval idigi_callback_continue  Vendor ID was successfully returned.
 * @retval idigi_callback_abort     Could not get the vendor ID and abort iDigi connector.
 *
 * @see @ref vendor_id API Configuration Callback
 * @see @ref idigi_config_vendor_id
 * @see @ref idigi_vendor_id "Obtaining an iDigi Vendor ID"
 *
 * @note This routine is not needed if you define @b IDIGI_VENDOR_ID configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_vendor_id(uint8_t const ** id, size_t * const size)
{
#error  "Specify vendor id"

    #define VENDOR_ID_LENGTH    4

    static const uint8_t const device_vendor_id[VENDOR_ID_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    *id   = device_vendor_id;
    *size = sizeof device_vendor_id;

    return idigi_callback_continue;
}

/**
 * @brief   Get the device type
 *
 * This routine returns a pointer to the device type which is an iso-8859-1 encoded string.
 * This string should be chosen by the device manufacture as a name that uniquely
 * identifies this model of device  to the server. When the server finds two devices
 * with the same device type, it can infer that they are the same product and
 * product scoped data may be shared among all devices with this device type.
 * A device's type cannot be an empty string, nor contain only whitespace.
 *
 * @param [out] type  Pointer to memory containing the device type
 * @param [out] size Size of the device type in bytes (Maximum is 63 bytes)
 *
 * @retval idigi_callback_continue  Device type was successfully returned.
 * @retval idigi_callback_abort     Could not get the device type and abort iDigi connector.
 *
 * @see @ref device_type API Configuration Callback
 * @see @ref IDIGI_DEVICE_TYPE
 *
 * @note This routine is not needed if you define @ref IDIGI_DEVICE_TYPE configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_device_type(char const ** type, size_t * const size)
{
#error "Specify device type"
    static char const device_type[] = "Linux Sample";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = sizeof device_type -1;

    return idigi_callback_continue;
}

/**
 * @brief   Get the iDigi server URL
 *
 * This routine assigns a pointer to the ASCII null-terminated string of the iDigi
 * Device Cloud FQDN, this is typically developer.idig.com.
 *
 * @param [out] url  Pointer to memory containing the URL
 * @param [out] size Size of the server URL in bytes (Maximum is 63 bytes)
 *
 * @retval idigi_callback_continue  The URL type was successfully returned.
 * @retval idigi_callback_abort     Could not get the URL and abort iDigi connector.
 *
 * @see @ref server_url API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_CLOUD_URL configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_server_url(char const ** url, size_t * const size)
{
#error "Specify iDigi Server URL"
    static char const idigi_server_url[] = "developer.idigi.com";

    /* Return pointer to device type. */
    *url = idigi_server_url;
    *size = sizeof idigi_server_url -1;

    return idigi_callback_continue;
}

/**
 * @brief   Get the connection type
 *
 * This routine specifies the connection type as @ref idigi_lan_connection_type or
 * @ref idigi_wan_connection_type. Fill in the type parameter with the address of the
 * idigi_connection_type_t.
 *
 * @param [out] type  Pointer to memory containing the @ref idigi_connection_type_t
 *
 * @retval idigi_callback_continue  The connection type was successfully returned.
 * @retval idigi_callback_abort     Could not get connection type and abort iDigi connector.
 *
 * @see @ref connection_type API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_CONNECTION_TYPE configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_connection_type(idigi_connection_type_t const ** type)
{
#error "Specify LAN or WAN connection type"

    /* Return pointer to connection type */
    static idigi_connection_type_t const device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return idigi_callback_continue;
}

/**
 * @brief   Get the link speed
 *
 * This routine assigns the link speed for WAN connection type. If connection type is LAN,
 * iDigi Connector will not request link speed configuration.
 *
 * @param [out] speed Pointer to memory containing the link speed
 * @param [out] size Size of the link speed in bytes
 *
 * @retval idigi_callback_continue  The link speed was successfully returned.
 * @retval idigi_callback_abort     Could not get the link speed and abort iDigi connector
 *
 * @see @ref link_speed API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_link_speed(uint32_t const ** speed, size_t * const size)
{
#error "Specify link speed for WAN connection type"

    UNUSED_ARGUMENT(speed);
    UNUSED_ARGUMENT(size);
    return idigi_callback_continue;
}

/**
 * @brief   Get the WAN phone number
 *
 * This routine assigns the phone number dialed for WAN connection type,
 * including any dialing prefixes. It's a variable length, non null-terminated string.
 * If connection type is LAN, iDigi Connector will not request phone number.
 *
 * @param [out] number  Pointer to memory containing the phone number
 * @param [out] size Size of the phone number in bytes
 *
 * @retval idigi_callback_continue  The phone number was successfully returned.
 * @retval idigi_callback_abort     Could not get the phone number and abort iDigi connector.
 *
 * @see @ref phone_number API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_WAN_PHONE_NUMBER_DIALED configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_phone_number(char const ** number, size_t * const size)
{
#error "Specify phone number dialed for WAN connection type"
    /*
     * Return pointer to phone number for WAN connection type.
     */
    UNUSED_ARGUMENT(number);
    UNUSED_ARGUMENT(size);
    return idigi_callback_continue;
}

/**
 * @brief   Get the TX keepalive interval
 *
 * This routine assigns the TX keepalive interval in seconds. This indicates how
 * often the iDigi Device Cloud sends a keepalive message to the device to verify the
 * device is still operational. Keepalive messages are from the prospective of the cloud,
 * this keepalive is sent from the cloud to the device. The value must be between 5 and 7200 seconds.
 *
 * @param [out] interval  Pointer to memory containing the keep alive interval
 * @param [out] size Size of memory buffer, containing the keep alive interval in bytes (this must be 2 bytes).
 *
 * @retval idigi_callback_continue  The keep alive interval was successfully returned.
 * @retval idigi_callback_abort     Could not get the keep alive interval and abort iDigi connector.
 *
 * @see @ref tx_keepalive API Configuration Callback
 *
 * @note This routine is not needed if you define @ref IDIGI_TX_KEEPALIVE_IN_SECONDS configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_tx_keepalive_interval(uint16_t const ** interval, size_t * const size)
{
#error "Specify server to device TX keepalive interval in seconds"

#define    DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS    45
    /* Return pointer to Tx keepalive interval in seconds */
    static uint16_t const device_tx_keepalive_interval = DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS;

    *interval = &device_tx_keepalive_interval;
    *size = sizeof device_tx_keepalive_interval;

    return idigi_callback_continue;
}

/**
 * @brief   Get the RX keepalive interval
 *
 * This routine assigns the RX keepalive interval in seconds. This indicates how
 * often the iDigi Connector device sends keepalive messages to the iDigi Device Cloud. Keepalive
 * messages are from the prospective of the cloud, this keepalive is sent from the
 * device to the cloud. The value must be between 5 and 7200 seconds.
 *
 * @param [out] interval  Pointer to memory containing the keep alive interval
 * @param [out] size Size of memory buffer, containing the keep alive interval in bytes (this must be 2 bytes).
 *
 * @retval idigi_callback_continue  The keep alive interval was successfully returned.
 * @retval idigi_callback_abort     Could not get the keep alive interval and abort iDigi connector.
 *
 * @see @ref rx_keepalive API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_RX_KEEPALIVE_IN_SECONDS configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_rx_keepalive_interval(uint16_t const ** interval, size_t * const size)
{
#error "Specify server to device RX keepalive interval in seconds"
#define    DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS    45
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t const device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;

    *interval = &device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return idigi_callback_continue;
}

/**
 * @brief   Get the wait count
 *
 * This routine assigns the number of times that not receiving a keepalive message
 * from the iDigi Device Cloud will indicate that the connection is considered lost.
 * This must be a 2-octet integer value between 2 to 64 counts.
 *
 * @param [out] count  Pointer to memory containing the wait count
 * @param [out] size Size of memory buffer, containing the wait count in bytes (this must be 2 bytes).
 *
 * @retval idigi_callback_continue  The wait count was successfully returned.
 * @retval idigi_callback_abort     Could not get the wait count and abort iDigi connector.
 *
 * @see @ref wait_count API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_WAIT_COUNT configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_wait_count(uint16_t const ** count, size_t * const size)
{
#error "Specify the number of times that not receiving keepalive messages from server is allowed"
#define    DEVICE_WAIT_COUNT    3
    /*
     * Return pointer to wait count (number of times not receiving Tx keepalive
     * from server is allowed).
     */
    static uint16_t const device_wait_count = DEVICE_WAIT_COUNT;

    *count = &device_wait_count;
    *size = sizeof device_wait_count;

    return idigi_callback_continue;
}

/**
 * @brief   Get firmware update support
 *
 * This routine tells iDigi Connector whether firmware update capability is supported or not.
 * If firmware update is not supported, callback for idigi_class_firmware
 * class will not be executed.
 *
 * @param [out] isSupported  Pointer memory where callback writes idigi_service_supported if firmware update is supported or
 *                            idigi_service_unsupported  if firmware update is not supported.
 *
 * @retval idigi_callback_continue  The firmware update support was successfully returned.
 * @retval idigi_callback_abort     Could not get the firmware update support and abort iDigi connector.
 *
 * @see @ref firmware_support API Configuration Callback
 *
 * @note This routine is not called if you define @b IDIGI_FIRMWARE_SUPPORT configuration in @ref idigi_config.h.
 * @note This IDIGI_FIRMWARE_SUPPORT indicates application supports firmware download. See @ref idigi_config_data_options
 *
 * @note See @ref IDIGI_FIRMWARE_SERVICE to include firmware access facility code in iDigi Connector.
 */
static idigi_callback_status_t app_get_firmware_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

/**
 * @brief  Get data service support
 *
 * This routine tells iDigi Connector whether the data service facility is supported or not.
 * If you plan on sending data to/from the iDigi server set this to idigi_service_supported.
 *
 * @param [out] isSupported  Pointer memory where callback writes idigi_service_supported if data service is supported or
 *                            idigi_service_unsupported  if data service is not supported.
 *
 * @retval idigi_callback_continue  The data service support was successfully returned.
 * @retval idigi_callback_abort     Could not get the data service support and abort iDigi connector.
 *
 * @see @ref data_service_support API Configuration Callback
 *
 * @note This routine is not called if you define @b IDIGI_DATA_SERVICE_SUPPORT configuration in @ref idigi_config.h.
 * @note This IDIGI_DATA_SERVICE_SUPPORT indicates application supports data service. See @ref idigi_config_data_options
 *
 * @note See @ref IDIGI_DATA_SERVICE to include data service code in iDigi Connector.
 * @note See @ref IDIGI_COMPRESSION for data service transferring compressed data.
 */
static idigi_callback_status_t app_get_data_service_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

/**
 * @brief   Get file system support
 *
 * This routine tells iDigi Connector whether the file system facility is supported or not.
 * If you plan to access device files from the iDigi server set this to idigi_service_supported.
 *
 * @param [out] isSupported  Pointer memory where callback writes idigi_service_supported if file system is supported or
 *                            idigi_service_unsupported  if file system is not supported.
 *
 * @retval idigi_callback_continue  The file system support was successfully returned.
 * @retval idigi_callback_abort     Could not get the file system support and abort iDigi connector.
 *
 * @see @ref file_system_support API Configuration Callback
 *
 * @note This routine is not called if you define @b IDIGI_FILE_SYSTEM_SUPPORT configuration in @ref idigi_config.h.
 * @note This IDIGI_FILE_SYSTEM_SUPPORT indicates application supports file system. See @ref idigi_config_data_options
 *
 * @note See @ref IDIGI_FILE_SYSTEM to include file system code in iDigi Connector.
 * @note See @ref IDIGI_COMPRESSION for file system transferring compressed data.
 */
static idigi_callback_status_t app_get_file_system_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

/**
 * @brief   Get the remote configuration support
 *
 * This routine tells iDigi connector whether the remote configuration service is supported or not.
 * If you plan on accessing device data configurations through iDigi server set
 * this to idigi_service_supported.
 *
 * @param [out] isSupported  Pointer memory where callback writes idigi_service_supported if the remote configuration is supported or
 *                            idigi_service_unsupported  if the remote configuration is not supported.
 *
 * @retval idigi_callback_continue  The remote configuration support was successfully returned.
 * @retval idigi_callback_abort     Could not get the remote configuration support and abort iDigi connector.
 *
 * @note @b IDIGI_RCI_MAXIMUM_CONTENT_LENGTH must be defined a non-zero length in @ref idigi_config.h idigi_remote.h.
 * @note This routine is not called if you define @b IDIGI_REMOTE_CONFIGURATION_SUPPORT configuration in @ref idigi_config.h.
 * @note This IDIGI_REMOTE_CONFIGURATION_SUPPORT indicates application supports remote configuration. See @ref idigi_config_data_options
 *
 */
static idigi_service_supported_status_t app_get_remote_configuration_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

/**
 * @brief   Get maximum transactions
 *
 * This routine tells iDigi Connector the maximum simultaneous transactions for data service
 * to receive messages from iDigi Cloud.
 *
 * @param [out] transCount  Pointer memory where callback writes the maximum simultaneous transaction.
 *                           Writes 0 for unlimited transactions.
 *
 * @retval idigi_callback_continue  The maximum simultaneous transactions was successfully returned.
 * @retval idigi_callback_abort     Could not get the maximum simultaneous transactions and abort iDigi connector.
 *
 * @see @ref max_msg_transactions API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_MSG_MAX_TRANSACTION configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_max_message_transactions(unsigned int * const transCount)
{
	/**
	 */
#define    IDIGI_MAX_MSG_TRANSACTIONS    1

    *transCount = IDIGI_MAX_MSG_TRANSACTIONS;

    return idigi_callback_continue;
}

/**
 * @brief   Get device id method
 *
 * This routine tells iDigi Connector how to obtain a device ID.
 *
 * @param [out] method  Pointer memory where callback writes:
 *                      @li @a @b digi_auto_device_id_method: to generate device ID from
 *                             - @ref mac_address callback for @ref idigi_lan_connection_type connection type or
 *                             - @ref imei_number callback for @ref idigi_wan_connection_type connection type.
 *                      @li @a @b idigi_manual_device_id_method: to obtain device ID from @ref device_id callback.
 *
 * @retval idigi_callback_continue  The device ID method was successfully returned.
 * @retval idigi_callback_abort     Could not get the device ID method and abort iDigi connector.
 *
 * @see @ref connection_type API Callback
 *
 * @note This routine is not needed if you define @b IDIGI_DEVICE_ID_METHOD configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static idigi_callback_status_t app_get_device_id_method(idigi_device_id_method_t * const method)
{

    *method = idigi_auto_device_id_method;

    return idigi_callback_continue;
}

/**
 * @brief   Get IMEI number
 *
 * This routine returns IMEI number. This routine is called when @ref device_id_method callback returns
 * @ref idigi_auto_device_id_method for WAN connection type.
 *
 * @param [out] imei_number  Pointer memory where callback writes 14 IMEI decimal digits plus one check digit.
 *                           Each nibble corresponds a decimal digit and most upper nibble must be 0.
 * @param [out] size         Size of the imei_number in bytes
 *
 * @retval idigi_callback_continue  The IMEI number was successfully returned.
 * @retval idigi_callback_abort     Could not get the IMEI number and abort iDigi connector.
 *
 * @see @ref device_id_method API Callback
 * @see @ref connection_type API Callback
 *
 */
static idigi_callback_status_t app_get_imei_number(uint8_t * const imei_number, size_t * size)
{
#error "Specify the IMEI number for WAN connection type if app_get_device_id_method returns idigi_auto_device_id_method"
    /* Each nibble corresponds a decimal digit.
     * Most upper nibble must be 0.
     */
    char  const app_imei_number[] = "000000-00-000000-0";
    int i = sizeof app_imei_number -1;
    int index = *size -1;

    while (i > 0)
    {
        int n = 0;

        imei_number[index] = 0;

        while (n < 2 && i > 0)
        {
            i--;
            if (app_imei_number[i] != '-')
            {
                ASSERT(isdigit(app_imei_number[i]));
                imei_number[index] += ((app_imei_number[i] - '0') << (n * 4));
                n++;
            }
        }
        index--;
    }
    return idigi_callback_continue;
}

/**
 * @brief   Error status notification
 *
 * This routine is called when iDigi Connector encounters an error. This is used as
 * a debug tool for finding configuration or keepalive error.
 *
 * The error_data argument contains class id, request id, and error status.
 *
 * @note If @ref IDIGI_DEBUG is not defined in idigi_config.h, iDigi Connector will
 * not call this callback to notify any error encountered.
 *
 * @retval None
 *
 * @see @ref error_status API Configuration Callback
 */
static void app_config_error(idigi_error_status_t const * const error_data)
{

    /* do not arrange the strings. The strings are corresponding to enum list */
    static char const * error_status_string[] = {"idigi_success", "idigi_init_error",
                                          "idigi_configuration_error",
                                          "idigi_invalid_data_size",
                                          "idigi_invalid_data_range",
                                          "idigi_invalid_payload_packet",
                                          "idigi_keepalive_error",
                                          "idigi_server_overload",
                                          "idigi_bad_version",
                                          "idigi_invalid_packet",
                                          "idigi_exceed_timeout",
                                          "idigi_unsupported_security",
                                          "idigi_invalid_data",
                                          "idigi_server_disconnected",
                                          "idigi_connect_error",
                                          "idigi_receive_error",
                                          "idigi_send_error",
                                          "idigi_close_error",
                                          "idigi_device_terminated",
                                          "idigi_service_busy",
                                          "idigi_invalid_response",
                                          "idigi_no_resource"};

    static char const * config_request_string[] = { "idigi_config_device_id",
                                             "idigi_config_vendor_id",
                                             "idigi_config_device_type",
                                             "idigi_config_server_url",
                                             "idigi_config_connection_type",
                                             "idigi_config_mac_addr",
                                             "idigi_config_link_speed",
                                             "idigi_config_phone_number",
                                             "idigi_config_tx_keepalive",
                                             "idigi_config_rx_keepalive",
                                             "idigi_config_wait_count",
                                             "idigi_config_ip_addr",
                                             "idigi_config_error_status",
                                             "idigi_config_firmware_facility",
                                             "idigi_config_data_service",
                                             "idigi_config_file_system",
                                             "idigi_config_remote_configuration",
                                             "idigi_config_max_transaction",
                                             "idigi_config_device_id_method",
                                             "idigi_config_imei_number"};

    static char const * network_request_string[] = { "idigi_network_connect",
                                              "idigi_network_send",
                                              "idigi_network_receive",
                                              "idigi_network_close"
                                              "idigi_network_disconnected",
                                              "idigi_network_reboot"};

    static char const * os_request_string[] = { "idigi_os_malloc",
                                             "idigi_os_free",
                                             "idigi_os_system_up_time",
                                             "idigi_os_sleep"};

    static char const * firmware_request_string[] = {"idigi_firmware_target_count",
                                              "idigi_firmware_version",
                                              "idigi_firmware_code_size",
                                              "idigi_firmware_description",
                                              "idigi_firmware_name_spec",
                                              "idigi_firmware_download_request",
                                              "idigi_firmware_binary_block",
                                              "idigi_firmware_download_complete",
                                              "idigi_firmware_download_abort",
                                              "idigi_firmware_target_reset"};

    static char const * data_service_string[] = {"idigi_data_service_put_request",
                                                "idigi_data_service_device_request"};

    static char const * file_system_string[] = {"idigi_file_system_open",
                                         "idigi_file_system_read",
                                         "idigi_file_system_write",
                                         "idigi_file_system_lseek",
                                         "idigi_file_system_ftruncate",
                                         "idigi_file_system_close",
                                         "idigi_file_system_rm",
                                         "idigi_file_system_stat",
                                         "idigi_file_system_opendir",
                                         "idigi_file_system_readdir",
                                         "idigi_file_system_closedir",
                                         "idigi_file_system_strerror",
                                         "idigi_file_system_msg_error",
                                         "idigi_file_system_hash"};

    static char const * remote_config_string[]= { "idigi_remote_config_session_start",
                                            "idigi_remote_config_session_end",
                                            "idigi_remote_config_action_start",
                                            "idigi_remote_config_action_end",
                                            "idigi_remote_config_group_start",
                                            "idigi_remote_config_group_end",
                                            "idigi_remote_config_group_process",
                                            "idigi_remote_config_session_cancel"};

    switch (error_data->class_id)
    {
    case idigi_class_config:
        APP_DEBUG("app_config_error: Config - %s (%d)  status = %s (%d)\n", config_request_string[error_data->request_id.config_request],
                     error_data->request_id.config_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_network:
        APP_DEBUG("app_config_error: Network - %s (%d)  status = %s (%d)\n", network_request_string[error_data->request_id.network_request],
                     error_data->request_id.network_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_operating_system:
        APP_DEBUG("app_config_error: Operating System - %s (%d)  status = %s (%d)\n", os_request_string[error_data->request_id.os_request],
                     error_data->request_id.os_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_firmware:
        APP_DEBUG("app_config_error: Firmware facility - %s (%d)  status = %s (%d)\n",
                     firmware_request_string[error_data->request_id.firmware_request],
                     error_data->request_id.firmware_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_data_service:
        APP_DEBUG("app_config_error: Data service - %s (%d)  status = %s (%d)\n",
                     data_service_string[error_data->request_id.data_service_request],
                     error_data->request_id.data_service_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_file_system:
        APP_DEBUG("app_config_error: File system - %s (%d)  status = %s (%d)\n",
                     file_system_string[error_data->request_id.file_system_request],
                     error_data->request_id.file_system_request,
                     error_status_string[error_data->status],error_data->status);
        break;
 case idigi_class_remote_config_service:
        APP_DEBUG("app_config_error: Remote configuration - %s (%d)  status = %s (%d)\n",
                remote_config_string[error_data->request_id.remote_config_request],
                     error_data->request_id.remote_config_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    default:
        APP_DEBUG("app_config_error: unsupport class_id = %d status = %d\n", error_data->class_id, error_data->status);
        break;
    }

}


/**
 * @cond DEV
 */
/*
 * Configuration callback routine.
 */
idigi_callback_status_t app_config_handler(idigi_config_request_t const request,
                                              void const * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * const response_length)
{
    idigi_callback_status_t status;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
    case idigi_config_device_id:
        status = app_get_device_id(response_data, response_length);
        break;

    case idigi_config_vendor_id:
        status = app_get_vendor_id(response_data, response_length);
        break;

    case idigi_config_device_type:
        status = app_get_device_type(response_data, response_length);
        break;

    case idigi_config_server_url:
        status = app_get_server_url(response_data, response_length);
        break;

    case idigi_config_connection_type:
        status = app_get_connection_type(response_data);
        break;

    case idigi_config_mac_addr:
        status = app_get_mac_addr(response_data, response_length);
        break;

    case idigi_config_link_speed:
        status = app_get_link_speed(response_data, response_length);
        break;

    case idigi_config_phone_number:
        status = app_get_phone_number(response_data, response_length);
       break;

    case idigi_config_tx_keepalive:
        status = app_get_tx_keepalive_interval(response_data, response_length);
        break;

    case idigi_config_rx_keepalive:
        status = app_get_rx_keepalive_interval(response_data, response_length);
        break;

    case idigi_config_wait_count:
        status = app_get_wait_count(response_data, response_length);
        break;

    case idigi_config_ip_addr:
        status = app_get_ip_address(response_data, response_length);
        break;

    case idigi_config_error_status:
        app_config_error(request_data);
        status = idigi_callback_continue;
        break;

    case idigi_config_firmware_facility:
        status = app_get_firmware_support(response_data);
        break;

    case idigi_config_data_service:
        status = app_get_data_service_support(response_data);
        break;

    case idigi_config_max_transaction:
        status = app_get_max_message_transactions(response_data);
        break;

    case idigi_config_remote_configuration:
        status = app_get_remote_configuration_support(response_data);
        break;

    case idigi_config_file_system:
        status = app_get_file_system_support(response_data);
        break;

    case idigi_config_device_id_method:
        status = app_get_device_id_method(response_data);
        break;

     case idigi_config_imei_number:
         status = app_get_imei_number(response_data, response_length);
         break;

    default:
        status = idigi_callback_unrecognized;
        break;

    }
    return status;
}
/**
 * @endcond
 */
