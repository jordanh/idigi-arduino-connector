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
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "errno.h"

#include "idigi_api.h"
#include "platform.h"
#include "application.h"

/* IIK Configuration routines */

#define MAX_INTERFACES      128
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

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
                                      "idigi_invalid_response"};

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

char python_file_buffer[256];
static char filename[64];

/*
 * Routine to get the IP address, you will need to modify this routine for your 
 * platform.
 */
static int app_get_ip_address(uint8_t ** ip_address, size_t *size)
{
    int             fd = -1;
    int            status=-1;
    char            *buf = malloc(MAX_INTERFACES*sizeof(struct ifreq));
    struct ifconf   conf;
    unsigned int    entries = 0;
    unsigned int    i;
    static struct in_addr  ip_addr; /* Used to store the IP address */

/* Remove this #error statement once you modify this routine to return the correct IP address */
#error "Specify device IP address. Set size to 4 (bytes) for IPv4 or 16 (bytes) for IPv6"


    APP_DEBUG("Entering app_get_ip_address\n");

    if (buf == NULL)
    {
        APP_DEBUG("get_ip_address malloc failed\n");
        goto error;
    }
    conf.ifc_len = MAX_INTERFACES*sizeof(struct ifreq);
    conf.ifc_buf = buf;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("socket");
        goto error;
    }

    if( ioctl(fd, SIOCGIFCONF , &conf) == -1)
    {
        APP_DEBUG("get_ip_address: Error using ioctl SIOCGIFCONF.\n");
        goto error;
    }

    entries = conf.ifc_len / sizeof(struct ifreq);

    APP_DEBUG("get_ip_address: Looking for current device IP address: found [%d] entries\n", entries);

    if (entries == 0)
    {
        goto error;
    }
    for( i = 0; i < entries; i++)
    {
        struct ifreq * req = &conf.ifc_req[i];
        struct sockaddr_in * sa = (struct sockaddr_in *) &req->ifr_addr;

        APP_DEBUG("get_ip_address: %d: Interface name [%s]\tIP Address [%s]\n", i+1, req->ifr_name, inet_ntoa(sa->sin_addr));
        if (sa->sin_addr.s_addr != htonl(INADDR_LOOPBACK))
        {
            ip_addr = sa->sin_addr;
            break;
        }
    }

    /* Fill in the size and IP address */
    *size       = sizeof ip_addr.s_addr;

    *ip_address = (uint8_t *)&ip_addr.s_addr;
    status = 0;

error:
    if (fd != -1)
    {
        close(fd);
    }
    if (buf != NULL)
    {
        free(buf);
    }
    return status;                                     
}

int app_get_forced_error_ip_address(void ** request_data, size_t request_length, void ** ip_address, size_t *size)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(request_length);

    APP_DEBUG("Entering app_get_forced_error_ip_address\n");
    app_get_ip_address((uint8_t **)ip_address, size); 
    *size = forcedErrorTable[idigi_config_ip_addr].size;

    return forcedErrorTable[idigi_config_ip_addr].status;
}

/* MAC address used in this sample */
// static uint8_t device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x40, 0x9d, 0x99, 0x55, 0x11};
static uint8_t device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static int app_get_mac_addr(uint8_t ** addr, size_t * size)
{
#error "Specify device MAC address for LAN connection"
       
    APP_DEBUG("entering app_get_mac_addr\n");

    *addr = device_mac_addr;
    *size = sizeof device_mac_addr;

    return 0;
}

static uint8_t device_id[DEVICE_ID_LENGTH] = {0};

static int app_get_device_id(uint8_t ** id, size_t * size)
{
    uint8_t * mac_addr;
    size_t mac_size;

#error  "Specify device id"

    APP_DEBUG("Entering app_get_device_id\n");

    /* This sample uses the MAC address to format the device ID */
    app_get_mac_addr(&mac_addr, &mac_size);

    device_id[8] = device_mac_addr[0];
    device_id[9] = device_mac_addr[1];
    device_id[10] = device_mac_addr[2];
    device_id[11] = 0xFF;
    device_id[12] = 0xFF;
    device_id[13] = device_mac_addr[3];
    device_id[14] = device_mac_addr[4];
    device_id[15] = device_mac_addr[5];

    *id   = device_id;
    *size = sizeof device_id;

    return 0;
}

int app_get_forced_error_device_id(void ** request_data, size_t request_length, void ** id, size_t * size)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(request_length);
    
    APP_DEBUG("Entering app_get_forced_error_device_id - returning [%d]\n", forcedErrorTable[idigi_config_device_id].status);

    app_get_device_id((uint8_t **)id, size);
    
    *size = forcedErrorTable[idigi_config_device_id].size;
    
    return forcedErrorTable[idigi_config_device_id].status;
}

static int app_get_vendor_id(uint8_t ** id, size_t * size)
{
#error  "Specify vendor id"
    // static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x01, 0x00, 0x00, 0x01};
    static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return 0;
}

int app_get_forced_error_vendor_id(void ** request_data, size_t request_length, void ** id, size_t * size)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(request_length);
    
    APP_DEBUG("Entering app_get_forced_error_vendor_id - returning [%d]\n", forcedErrorTable[idigi_config_vendor_id].status);
    
    app_get_vendor_id((uint8_t **)id, size);
    
    *size = forcedErrorTable[idigi_config_vendor_id].size;
    
    return forcedErrorTable[idigi_config_vendor_id].status;
}

static int app_get_device_type(char ** type, size_t * size)
{
#error "Specify device type"
    static char const device_type[] = "Linux Application";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = sizeof device_type -1;

    return 0;
}

int app_get_forced_error_device_type(void ** request_data, size_t request_length, void ** type, size_t * size)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(request_length);

    APP_DEBUG("Entering app_get_forced_error_device_type\n");

    app_get_device_type((char **)type, size);
     
    *size = forcedErrorTable[idigi_config_device_type].size;

    return forcedErrorTable[idigi_config_device_type].status;
}

static int app_get_server_url(char ** url, size_t * size)
{
#error "Specify iDigi Server URL"

    static char const idigi_server_url[] = "developer.idigi.com";

    /* Return pointer to device type. */
    *url = (char *)idigi_server_url;
    *size = sizeof idigi_server_url -1;

    return 0;
}

int app_get_forced_error_server_url(void ** request_data, size_t request_length, void **  url, size_t * size)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(request_length);
    
    APP_DEBUG("Entering app_get_forced_error_server_url - returning [%d]\n", forcedErrorTable[idigi_config_server_url].status);
    
    app_get_server_url((char **)url, size);
    
    *size = forcedErrorTable[idigi_config_server_url].size;
    
    return forcedErrorTable[idigi_config_server_url].status;
}

static int app_get_connection_type(idigi_connection_type_t ** type)
{
#error "Specify LAN or WAN connection type"

    /* Return pointer to connection type */
    static idigi_connection_type_t  device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return 0;
}

static int app_get_link_speed(uint32_t **speed, size_t * size)
{
#error "Specify link speed for WAN connection type"
    UNUSED_ARGUMENT(speed);
    UNUSED_ARGUMENT(size);

    return 0;
}

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

/* Keep alives are from the prospective of the server */
/* This keep alive is sent from the server to the device */
static int app_get_tx_keepalive_interval(uint16_t **interval, size_t * size)
{
#error "Specify server to device TX keepalive interval in seconds"

#define DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS     90
    /* Return pointer to Tx keepalive interval in seconds */
    static uint16_t device_tx_keepalive_interval = DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = &device_tx_keepalive_interval;
    *size = sizeof device_tx_keepalive_interval;

    return 0;
}

/* This keep alive is sent from the device to the server  */
static int app_get_rx_keepalive_interval(uint16_t **interval, size_t * size)
{
#error "Specify server to device RX keepalive interval in seconds"
#define DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS     60
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = &device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return 0;
}

int app_get_forced_error_rx_keepalive_interval(void ** request_data, size_t request_length, void **interval, size_t * size)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(request_length);

    app_get_rx_keepalive_interval((uint16_t **)interval, size);
 
    *size = forcedErrorTable[idigi_config_rx_keepalive].size;

    return forcedErrorTable[idigi_config_rx_keepalive].status;
}

static int app_get_wait_count(uint16_t **count, size_t * size)
{
#error "Specify the number of times that not receiving keepalive messages from server is allowed"
#define DEVICE_WAIT_COUNT     5
    /* 
     * Return pointer to wait count (number of times not receiving Tx keepalive 
     * from server is allowed).
     */
    static uint16_t device_wait_count = DEVICE_WAIT_COUNT;
    *count = &device_wait_count;
    *size = sizeof device_wait_count;

    return 0;
}

static idigi_service_supported_status_t app_get_firmware_support(void)
{
    return idigi_service_supported;
}

static idigi_service_supported_status_t app_get_data_service_support(void)
{
    return idigi_service_supported;
}

static idigi_service_supported_status_t app_get_file_system_support(void)
{
    return idigi_service_supported;
}

static idigi_service_supported_status_t app_get_remote_configuration_support(void)
{
    return idigi_service_supported;
}

static unsigned int app_get_max_message_transactions(void)
{
#define IDIGI_MAX_MSG_TRANSACTIONS   1

    return IDIGI_MAX_MSG_TRANSACTIONS;
}

void write_python_result_file(char *file_buffer)
{
    FILE * fp = NULL;
    uint8_t id;
    size_t size;

    app_get_device_id((uint8_t **)&id, &size);
    snprintf(filename, sizeof(filename), "%02X%02X%02X%02X-%02X%02X%02X%02X-%02X%02X%02X%02X-%02X%02X%02X%02X.txt",
             device_id[0], device_id[1], device_id[2], device_id[3], device_id[4], device_id[5], device_id[6], device_id[7],
             device_id[8], device_id[9], device_id[10], device_id[11], device_id[12], device_id[13], device_id[14], device_id[15]);
    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        APP_DEBUG("app_config_error: Failed to open %s\n", filename);
    }
    else
    {
        APP_DEBUG("app_config_error: writing file %s\n", filename);
        fwrite(file_buffer, 1, strlen(file_buffer), fp);
        fclose(fp);
    }
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
                imei_number[index] += ((app_imei_number[i] - '0') << (n * 4));
                n++;
            }
        }
        index--;
    }
    return idigi_callback_continue;
}

/* End of IIK configuration routines */

/*
 * This routine is called when a configuration error is encountered by the IIK.
 * This is currently used as a debug tool for finding configuration errors.
 */
void app_config_error(idigi_error_status_t const * const error_data)
{
    switch (error_data->class_id)
    {
    case idigi_class_config:
        APP_DEBUG("app_config_error: Config - %s (%d)  status = %s (%d)\n", config_request_string[error_data->request_id.config_request],
                   error_data->request_id.config_request, error_status_string[error_data->status],error_data->status);
        snprintf(python_file_buffer, sizeof(python_file_buffer), "%s,%s,",
                 config_request_string[error_data->request_id.config_request], 
                 error_status_string[error_data->status]);
        write_python_result_file(python_file_buffer);
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
    default:
        APP_DEBUG("app_config_error: unsupport class_id = %d status = %d\n", error_data->class_id, error_data->status);
        break;
    }
}

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
    int ret = -1;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
    case idigi_config_device_id:
        APP_DEBUG("app_config_handler:idigi_config_device_id\n");
        ret = app_get_device_id(response_data, response_length);
        break;

    case idigi_config_mac_addr:
        APP_DEBUG("app_config_handler:idigi_config_mac_addr\n");
        ret = app_get_mac_addr(response_data, response_length);
        break;

    case idigi_config_vendor_id:
        APP_DEBUG("app_config_handler:idigi_config_vendor_id\n");
        ret = app_get_vendor_id(response_data, response_length);
        break;

    case idigi_config_device_type:
        APP_DEBUG("app_config_handler:idigi_config_vendor_id\n"); 
        ret = app_get_device_type(response_data, response_length);
        break;

    case idigi_config_server_url:
        APP_DEBUG("app_config_handler:idigi_config_server_url\n");
        ret = app_get_server_url(response_data, response_length);
        break;

    case idigi_config_connection_type:
        APP_DEBUG("app_config_handler:idigi_config_connection_type\n");
        ret = app_get_connection_type(response_data);
        break;

    case idigi_config_link_speed:
        APP_DEBUG("app_config_handler:idigi_config_link_speed\n");
        ret = app_get_link_speed(response_data, response_length);
        break;

    case idigi_config_phone_number:
        APP_DEBUG("app_config_handler:idigi_config_phone_number\n");
        ret = app_get_phone_number(response_data, response_length);
        break;

    case idigi_config_tx_keepalive:
        APP_DEBUG("app_config_handler:idigi_config_tx_keepalive\n");
        ret = app_get_tx_keepalive_interval(response_data, response_length);
        break;

    case idigi_config_rx_keepalive:
        APP_DEBUG("app_config_handler:idigi_config_rx_keepalive\n");
        ret = app_get_rx_keepalive_interval(response_data, response_length);
        break;

    case idigi_config_wait_count:
        APP_DEBUG("app_config_handler:idigi_config_wait_count\n");
        ret = app_get_wait_count(response_data, response_length);
        break;

    case idigi_config_ip_addr:
        APP_DEBUG("app_config_handler:idigi_config_ip_addr\n");
        ret = app_get_ip_address(response_data, response_length);
        break;

    case idigi_config_error_status:
        APP_DEBUG("app_config_handler:idigi_config_error_status\n");
        app_config_error(request_data);
        ret = 0;
        break;

    case idigi_config_firmware_facility:
        APP_DEBUG("app_config_handler:idigi_config_firmware_support\n");
        *((idigi_service_supported_status_t *)response_data) = app_get_firmware_support();
        ret = 0;
        break;

    case idigi_config_data_service:
        APP_DEBUG("app_config_handler:idigi_config_data_service_support\n");
        *((idigi_service_supported_status_t *)response_data) = app_get_data_service_support();
        ret = 0;
        break;

    case idigi_config_file_system:
        *((idigi_service_supported_status_t *)response_data) = app_get_file_system_support();
        ret = 0;
        break;

    case idigi_config_remote_configuration:
        *((idigi_service_supported_status_t *)response_data) = app_get_remote_configuration_support();
        ret = 0;
        break;

    case idigi_config_max_transaction:
        APP_DEBUG("app_config_handler:idigi_config_max_message_transactions\n");
        *((unsigned int *)response_data) = app_get_max_message_transactions();
        ret = 0;
        break;

    case idigi_config_device_id_method:
        status = app_get_device_id_method(response_data);
        break;

     case idigi_config_imei_number:
         status = app_get_imei_number(response_data, response_length);
         break;

    default:
        APP_DEBUG("idigi_config_callback: unknown configuration request= %d\n", request);
        break;
    }

    status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}

