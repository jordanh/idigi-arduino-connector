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
#include <stdio.h>
#include <ctype.h>

#include "idigi_config.h"
#include "idigi_api.h"
#include "idigi_debug.h"
#include "platform.h"

static idigi_callback_status_t app_get_ip_address(uint8_t const ** ip_address, size_t * const size)
{
    /* Remove this #error statement once you modify this routine to return the correct IP address */
#error "Specify device IP address. Set size to 4 (bytes) for IPv4 or 16 (bytes) for IPv6"

    UNUSED_ARGUMENT(ip_address);
    UNUSED_ARGUMENT(size);

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_mac_addr(uint8_t const ** addr, size_t * const size)
{
    #define MAC_ADDR_LENGTH     6

    /* MAC address used in this sample */
    static uint8_t const device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#error "Specify device MAC address for LAN connection"

    *addr = device_mac_addr;
    *size = sizeof device_mac_addr;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_device_id(uint8_t const ** id, size_t * const size)
{
    #define DEVICE_ID_LENGTH    16

    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};
    uint8_t const * mac_addr;
    size_t mac_size;
    idigi_callback_status_t status;

#error  "Specify device id"

    /* This sample uses the MAC address to format the device ID */
    status = app_get_mac_addr(&mac_addr, &mac_size);

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

static idigi_callback_status_t app_get_vendor_id(uint8_t const ** id, size_t * const size)
{
#error  "Specify vendor id"

    #define VENDOR_ID_LENGTH    4

    static uint8_t const device_vendor_id[VENDOR_ID_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_device_type(char const ** type, size_t * const size)
{
#error "Specify device type"
    static char const device_type[] = "iDigi connector Linux Sample";

    /* Return pointer to device type. */
    *type = device_type;
    *size = sizeof device_type -1;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_server_url(char const ** url, size_t * const size)
{
#error "Specify iDigi Device Cloud URL"
    static char const idigi_server_url[] = "developer.idigi.com";

    /* Return pointer to device type. */
    *url = idigi_server_url;
    *size = sizeof idigi_server_url -1;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_connection_type(idigi_connection_type_t const ** type)
{
#error "Specify LAN or WAN connection type"

    /* Return pointer to connection type */
    static idigi_connection_type_t const device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_link_speed(uint32_t const ** speed, size_t * const size)
{
#error "Specify link speed for WAN connection type"

    UNUSED_ARGUMENT(speed);
    UNUSED_ARGUMENT(size);
    return idigi_callback_continue;
}

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

static idigi_callback_status_t app_get_tx_keepalive_interval(uint16_t const ** interval, size_t * const size)
{
#error "Specify server to device TX keepalive interval in seconds"

#define DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS     90
    /* Return pointer to Tx keepalive interval in seconds */
    static uint16_t const device_tx_keepalive_interval = DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS;

    *interval = &device_tx_keepalive_interval;
    *size = sizeof device_tx_keepalive_interval;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_rx_keepalive_interval(uint16_t const ** interval, size_t * const size)
{
#error "Specify server to device RX keepalive interval in seconds"
#define DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS     60
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t const device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;

    *interval = &device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_wait_count(uint16_t const ** count, size_t * const size)
{
#error "Specify the number of times that not receiving keepalive messages from server is allowed"
#define DEVICE_WAIT_COUNT     5
    /* 
     * Return pointer to wait count (number of times not receiving Tx keepalive 
     * from server is allowed).
     */
    static uint16_t const device_wait_count = DEVICE_WAIT_COUNT;
    *count = &device_wait_count;
    *size = sizeof device_wait_count;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_firmware_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_data_service_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_file_system_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;

}

static idigi_callback_status_t app_get_remote_configuration_support(idigi_service_supported_status_t * const isSupported)
{
    *isSupported = idigi_service_supported;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_max_message_transactions(unsigned int * const transCount)
{
#define IDIGI_MAX_MSG_TRANSACTIONS   1

    *transCount = IDIGI_MAX_MSG_TRANSACTIONS;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_get_device_id_method(idigi_device_id_method_t * const method)
{

    *method = idigi_auto_device_id_method;

    return idigi_callback_continue;
}

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

    case idigi_config_remote_configuration:
        status = app_get_remote_configuration_support(response_data);
        break;

    case idigi_config_max_transaction:
        status = app_get_max_message_transactions(response_data);
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
