/*
 * Copyright (c) 1996-2012 Digi International Inc.,
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
  *  @brief Configuration routines for the IIK.
  *
  */
#include <stdio.h>
#include "idigi_api.h"
#include "platform.h"

#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

/**
 * @brief   Get the IP address of the device
 *
 * This routine assigns a pointer to the IP address of the device in *ip_address 
 * along with the size of the IP address which must be either a 4-octet value for 
 * IPv4 or a 6-octet value for IPv6. This routine returns fail if an error occurred, 
 * otherwise true.
 *
 * @param [out] ip_address  Pointer to memory containing IP address
 * @param [out] size Size of the IP address in bytes
 *  
 * @retval 0  IP address was returned OK
 * @retval -1  Could not get IP address
 *
 * @see @ref ip_address API Configuration Callback
 */
static int app_get_ip_address(uint8_t ** ip_address, size_t * size)
{
    UNUSED_ARGUMENT(ip_address);
    UNUSED_ARGUMENT(size);

    return 0;
}

/**
 * @brief   Get the MAC address of the device
 *
 * This routine assigns a pointer to the MAC address of the device in *mac_address along 
 * with the size. This routine returns fail if an error occurred, otherwise true.
 *
 * @param [out] addr  Pointer to memory containing IP address
 * @param [out] size Size of the MAC address in bytes (6 bytes).
 *  
 * @retval 0  MAC address was returned OK
 * @retval -1  Could not get the MAC address
 * 
 * @see @ref mac_address API Configuration Callback
 */
static int app_get_mac_addr(uint8_t ** addr, size_t * size)
{
    /* MAC address used in this sample */
    static uint8_t device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#error "Specify device MAC address for LAN connection"

    *addr = device_mac_addr;
    *size = sizeof device_mac_addr;

    return 0;
}

/**
 * @brief   Get the iDigi device ID
 *
 * This routine is called to get a unique device ID which is used to identify the device. 
 * The pointer ID is filled in with the address of the memory location which contains the 
 * device ID, size is filled in with the size of the device ID.
. *
 * @param [out] id  Pointer to memory containing the device ID
 * @param [out] size Size of the device ID in bytes (16 bytes)
 *  
 * @retval 0  Device ID was returned OK
 * @retval -1  Could not get the device ID
 * 
 * @see @ref device_id API Configuration Callback
 */
static int app_get_device_id(uint8_t ** id, size_t * size)
{
    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};
    uint8_t * mac_addr;
    size_t mac_size;

#error  "Specify device id"

    /* This sample uses the MAC address to format the device ID */
    app_get_mac_addr(&mac_addr, &mac_size);

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

    return 0;
}

/**
 * @brief   Get the iDigi vendor ID
 *
 * This routine assigns a pointer to the vendor ID which is a unique code identifying 
 * the manufacturer of a device. Vendor IDs are assigned to manufacturers by iDigi. 
 * This routine returns fail if an error occurred, otherwise true.
 *
 * @param [out] id  Pointer to memory containing the device ID
 * @param [out] size Size of the vendor ID in bytes (4 bytes)
 *  
 * @retval 0  Vendor ID was returned OK
 * @retval -1  Could not get the vendor ID
 * 
 * @see @ref vendor_id API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_VENDOR_ID configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_vendor_id(uint8_t ** id, size_t * size)
{
#error  "Specify vendor id"
    static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return 0;
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
 * @retval 0  Device type was returned OK
 * @retval -1  Could not get the device type
 *
 * @see @ref device_type API Configuration Callback
 * @see @ref IDIGI_DEVICE_TYPE
 * 
 * @note This routine is not needed if you define @ref IDIGI_DEVICE_TYPE configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_device_type(char ** type, size_t * size)
{
#error "Specify device type"
    static const char const *device_type = "IIK Linux Sample";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = strlen(device_type);

    return 0;
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
 * @retval 0  The URL type was returned OK
 * @retval -1  Could not get the URL
 * 
 * @see @ref server_url API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_CLOUD_URL configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_server_url(char ** url, size_t * size)
{
#error "Specify iDigi Server URL"
    static const char const *idigi_server_url = "developer.idigi.com";

    /* Return pointer to device type. */
    *url = (char *)idigi_server_url;
    *size = strlen(idigi_server_url);

    return 0;
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
 * @retval 0  The connection type was returned OK
 * @retval -1  Could not get connection type
 * 
 * @see @ref connection_type API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_CONNECTION_TYPE configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_connection_type(idigi_connection_type_t ** type)
{
#error "Specify LAN or WAN connection type"

    /* Return pointer to connection type */
    static idigi_connection_type_t  device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return 0;
}

/**
 * @brief   Get the link speed
 *
 * This routine assigns the link speed for WAN connection type. If connection type is LAN, 
 * IIK will not request link speed configuration.
 *
 * @param [out] speed Pointer to memory containing the link speed
 * @param [out] size Size of the link speed in bytes
 *  
 * @retval 0  The link speed was returned OK
 * @retval -1  Could not get the link speed
 * 
 * @see @ref link_speed API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_link_speed(uint32_t ** speed, size_t * size)
{
#error "Specify link speed for WAN connection type"

    UNUSED_ARGUMENT(speed);
    UNUSED_ARGUMENT(size);
    return 0;
}

/**
 * @brief   Get the WAN phone number
 *
 * This routine assigns the phone number dialed for WAN connection type,
 * including any dialing prefixes. It's a variable length, non null-terminated string.
 * If connection type is LAN, IIK will not request phone number.
 *
 * @param [out] number  Pointer to memory containing the phone number
 * @param [out] size Size of the phone number in bytes
 *  
 * @retval 0  The phone number was returned OK
 * @retval -1  Could not get the phone number
 * 
 * @see @ref phone_number API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_WAN_PHONE_NUMBER_DIALED configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_phone_number(uint8_t ** number, size_t * size)
{
#error "Specify phone number dialed for WAN connection type"
    /* 
     * Return pointer to phone number for WAN connection type.
     */
    UNUSED_ARGUMENT(number);
    UNUSED_ARGUMENT(size);
    return 0;
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
 * @retval 0  The keep alive interval was returned OK
 * @retval -1  Could not get the keep alive interval
 * 
 * @see @ref tx_keepalive API Configuration Callback
 *
 * @note This routine is not needed if you define @ref IDIGI_TX_KEEPALIVE_IN_SECONDS configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_tx_keepalive_interval(uint16_t ** interval, size_t * size)
{
#error "Specify server to device TX keepalive interval in seconds"

#define DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS     90
    /* Return pointer to Tx keepalive interval in seconds */
    static uint16_t device_tx_keepalive_interval = DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = (uint16_t *)&device_tx_keepalive_interval;
    *size = sizeof device_tx_keepalive_interval;

    return 0;
}

/**
 * @brief   Get the RX keepalive interval
 *
 * This routine assigns the RX keepalive interval in seconds. This indicates how 
 * often the IIK device sends keepalive messages to the iDigi Device Cloud. Keepalive
 * messages are from the prospective of the cloud, this keepalive is sent from the 
 * device to the cloud. The value must be between 5 and 7200 seconds.
 *
 * @param [out] interval  Pointer to memory containing the keep alive interval
 * @param [out] size Size of memory buffer, containing the keep alive interval in bytes (this must be 2 bytes).
 *  
 * @retval 0  The keep alive interval was returned OK
 * @retval -1  Could not get the keep alive interval
 * 
 * @see @ref rx_keepalive API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_RX_KEEPALIVE_IN_SECONDS configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_rx_keepalive_interval(uint16_t ** interval, size_t * size)
{
#error "Specify server to device RX keepalive interval in seconds"
#define DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS     60
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = (uint16_t *)&device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return 0;
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
 * @retval 0  The wait count was returned OK
 * @retval -1  Could not get the wait count
 * 
 * @see @ref wait_count API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_WAIT_COUNT configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static int app_get_wait_count(uint16_t ** count, size_t * size)
{
#error "Specify the number of times that not receiving keepalive messages from server is allowed"
#define DEVICE_WAIT_COUNT     5
    /* 
     * Return pointer to wait count (number of times not receiving Tx keepalive 
     * from server is allowed).
     */
    static uint16_t device_wait_count = DEVICE_WAIT_COUNT;
    *count = (uint16_t *)&device_wait_count;
    *size = sizeof device_wait_count;

    return 0;
}

/**
 * @brief   Return true if firmware download is supported
 *
 * This routine tells IIK whether firmware access facility is supported or not. 
 * If firmware access facility is not supported, callback for idigi_class_firmware 
 * class will not be executed.
 *
 * @retval idigi_service_supported  Firmware download is supported
 * @retval idigi_service_unsupported  Firmware download is not supported
 *
 * @see @ref firmware_support API Configuration Callback
 * 
 * @note This routine is not called if you define @b IDIGI_FIRMWARE_SUPPORT configuration in @ref idigi_config.h.
 * @note This IDIGI_FIRMWARE_SUPPORT indicates application supports firmware download. See @ref idigi_config_data_options
 *
 * @note See @ref IDIGI_FIRMWARE_SERVICE to include firmware access facility code in IIK.
 */
static idigi_service_supported_status_t app_get_firmware_support(void)
{
    return idigi_service_supported;
}

/**
 * @brief   Return true if the data service is supported
 *
 * This routine tells IIK whether the data service facility is supported or not. 
 * If you plan on sending data to/from the iDigi server set this to idigi_service_supported.
 *
 * @retval idigi_service_supported  Data service is supported
 * @retval idigi_service_unsupported  Data service is not supported
 *
 * @see @ref data_service_support API Configuration Callback
 *
 * @note This routine is not called if you define @b IDIGI_DATA_SERVICE_SUPPORT configuration in @ref idigi_config.h.
 * @note This IDIGI_DATA_SERVICE_SUPPORT indicates application supports data service. See @ref idigi_config_data_options
 *
 * @note See @ref IDIGI_DATA_SERVICE to include data service code in IIK.
 * @note See @ref IDIGI_COMPRESSION for data service transferring compressed data.
 */
static idigi_service_supported_status_t app_get_data_service_support(void)
{
    return idigi_service_supported;
}

/**
 * @brief   Return maximum transactions
 *
 * This routine tells IIK the maximum simultaneous transactions for data service
 * to receive messages from iDigi Cloud.
 *
 * @retval 0  unlimited transactions
 * @retval >0  maximum transactions
 * 
 * @see @ref max_msg_transactions API Configuration Callback
 *
 * @note This routine is not needed if you define @b IDIGI_MSG_MAX_TRANSACTION configuration in @ref idigi_config.h.
 * See @ref idigi_config_data_options
 */
static unsigned int app_get_max_message_transactions(void)
{
#define IDIGI_MAX_MSG_TRANSACTIONS   1

    return IDIGI_MAX_MSG_TRANSACTIONS;
}

/**
 * @brief   Error status notification
 *
 * This routine is called when IIK encounters an error. This is used as
 * a debug tool for finding configuration or keepalive error.
 *
 * The error_data argument contains class id, request id, and error status.
 *
 * @note If @ref IDIGI_DEBUG is not defined in idigi_config.h, IIK will
 * not call this callback to notify any error encountered.
 *
 * @retval None
 *
 * @see @ref error_status API Configuration Callback
 */
static void app_config_error(idigi_error_status_t * const error_data)
{

    /* do not arrange the strings. The strings are corresponding to enum list */
    char const * error_status_string[] = {"idigi_success", "idigi_init_error",
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

    char const * config_request_string[] = { "idigi_config_device_id",
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
                                              "idigi_config_max_transaction"};

    char const * network_request_string[] = { "idigi_network_connect",
                                              "idigi_network_send",
                                              "idigi_network_receive",
                                              "idigi_network_close"
                                              "idigi_network_disconnected",
                                              "idigi_network_reboot"};

    char const * os_request_string[] = { "idigi_os_malloc",
                                         "idigi_os_free",
                                         "idigi_os_system_up_time",
                                         "idigi_os_sleep"};

    char const * firmware_request_string[] = {"idigi_firmware_target_count",
                                              "idigi_firmware_version",
                                              "idigi_firmware_code_size",
                                              "idigi_firmware_description",
                                              "idigi_firmware_name_spec",
                                              "idigi_firmware_download_request",
                                              "idigi_firmware_binary_block",
                                              "idigi_firmware_download_complete",
                                              "idigi_firmware_download_abort",
                                              "idigi_firmware_target_reset"};

    char const * data_service_string[] = {"idigi_data_service_put_request",
                                          "idigi_data_service_device_request"};

    switch (error_data->class_id)
    {
    case idigi_class_config:
        APP_DEBUG("idigi_error_status: Config - %s (%d)  status = %s (%d)\n", config_request_string[error_data->request_id.config_request],
                     error_data->request_id.config_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_network:
        APP_DEBUG("idigi_error_status: Network - %s (%d)  status = %s (%d)\n", network_request_string[error_data->request_id.network_request],
                     error_data->request_id.network_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_operating_system:
        APP_DEBUG("idigi_error_status: Operating System - %s (%d)  status = %s (%d)\n", os_request_string[error_data->request_id.os_request],
                     error_data->request_id.os_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_firmware:
        APP_DEBUG("idigi_error_status: Firmware facility - %s (%d)  status = %s (%d)\n",
                     firmware_request_string[error_data->request_id.firmware_request],
                     error_data->request_id.firmware_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_data_service:
        APP_DEBUG("idigi_error_status: Data service - %s (%d)  status = %s (%d)\n",
                     data_service_string[error_data->request_id.data_service_request],
                     error_data->request_id.data_service_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    default:
        APP_DEBUG("idigi_error_status: unsupport class_id = %d status = %d\n", error_data->class_id, error_data->status);
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
                                              void * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * const response_length)
{
    idigi_callback_status_t status;
    int ret = -1;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
    case idigi_config_device_id:
        ret = app_get_device_id((uint8_t **)response_data, response_length);
        break;

    case idigi_config_vendor_id:
        ret = app_get_vendor_id((uint8_t **)response_data, response_length);
        break;

    case idigi_config_device_type:
        ret = app_get_device_type((char **)response_data, response_length);
        break;

    case idigi_config_server_url:
        ret = app_get_server_url((char **)response_data, response_length);
        break;

    case idigi_config_connection_type:
        ret = app_get_connection_type((idigi_connection_type_t **)response_data);
        break;

    case idigi_config_mac_addr:
        ret = app_get_mac_addr((uint8_t **)response_data, response_length);
        break;

    case idigi_config_link_speed:
        ret = app_get_link_speed((uint32_t **)response_data, response_length);
        break;

    case idigi_config_phone_number:
        ret = app_get_phone_number((uint8_t **)response_data, response_length);
       break;

    case idigi_config_tx_keepalive:
        ret = app_get_tx_keepalive_interval((uint16_t **)response_data, response_length);
        break;

    case idigi_config_rx_keepalive:
        ret = app_get_rx_keepalive_interval((uint16_t **)response_data, response_length);
        break;

    case idigi_config_wait_count:
        ret = app_get_wait_count((uint16_t **)response_data, response_length);
        break;

    case idigi_config_ip_addr:
        ret = app_get_ip_address((uint8_t **)response_data, response_length);
        break;

    case idigi_config_error_status:
        app_config_error((idigi_error_status_t *)request_data);
        ret = 0;
        break;

    case idigi_config_firmware_facility:
        *((idigi_service_supported_status_t *)response_data) = app_get_firmware_support();
        ret = 0;
        break;

    case idigi_config_data_service:
        *((idigi_service_supported_status_t *)response_data) = app_get_data_service_support();
        ret = 0;
        break;

    case idigi_config_max_transaction:
        *((unsigned int *)response_data) = app_get_max_message_transactions();
         ret = 0;
        break;

    }

    status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}
/**
 * @endcond
 */
