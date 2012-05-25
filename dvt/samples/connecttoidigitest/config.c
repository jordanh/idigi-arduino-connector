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
#include <unistd.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "idigi_api.h"
#include "platform.h"

/* iDigi Connector Configuration routines */

#define IDIGI_DEVICE_TYPE                          "iDigi Connector Linux Sample"
#define IDIGI_CLOUD_URL                            "developer.idigi.com"
#define IDIGI_TX_KEEPALIVE_IN_SECONDS              75
#define IDIGI_RX_KEEPALIVE_IN_SECONDS              75
#define IDIGI_WAIT_COUNT                           5
#define IDIGI_VENDOR_ID                            0x00000000

/*
 * Routine to get the IP address, you will need to modify this routine for your 
 * platform.
 */
static int app_get_ip_address(uint8_t ** ip_address, size_t *size)
{
#define MAX_INTERFACES      128

    int             fd = -1;
    int            status=-1;
    char            *buf = malloc(MAX_INTERFACES*sizeof(struct ifreq));
    struct ifconf   conf;
    unsigned int    entries = 0;
    unsigned int    i;
    static struct in_addr  ip_addr; /* Used to store the IP address */

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


static int app_get_mac_addr(uint8_t ** addr, size_t * size)
{

#define MAC_ADDR_LENGTH     6

    /* MAC address used in this sample */
    static uint8_t device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x40, 0x9d, 0x43, 0x23, 0x17};

    *addr = device_mac_addr;
    *size = sizeof device_mac_addr;

    return 0;
}

static int app_get_device_id(uint8_t ** id, size_t * size)
{
#define DEVICE_ID_LENGTH    16

    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};
    uint8_t * mac_addr;
    size_t mac_size;

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
static int app_get_vendor_id(uint8_t ** id, size_t * size)
{
#define VENDOR_ID_LENGTH    4

    static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x01, 0x00, 0x00, 0x1A};


    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return 0;
}

static int app_get_device_type(char ** type, size_t * size)
{
    static char const device_type[] = "connect to idigi DVT";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = sizeof device_type -1;

    return 0;
}

static int app_get_server_url(char ** url, size_t * size)
{
    static char const idigi_server_url[] = "idigi-e2e.sa.digi.com";

    /* Return pointer to device type. */
    *url = (char *)idigi_server_url;
    *size = sizeof idigi_server_url -1;

    return 0;
}

static int app_get_connection_type(idigi_connection_type_t ** type)
{
    /* Return pointer to connection type */
    static idigi_connection_type_t  device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return 0;
}

static int app_get_link_speed(uint32_t **speed, size_t * size)
{
    UNUSED_ARGUMENT(speed);
    UNUSED_ARGUMENT(size);

    return 0;
}

static int app_get_phone_number(uint8_t ** number, size_t * size)
{
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
#define DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS     60
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = &device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return 0;
}

static int app_get_wait_count(uint16_t **count, size_t * size)
{
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

static idigi_service_supported_status_t app_get_remote_configuration_support(void)
{
    return idigi_service_supported;
}

static unsigned int app_get_max_message_transactions(void)
{
#define IDIGI_MAX_MSG_TRANSACTIONS   1

    return IDIGI_MAX_MSG_TRANSACTIONS;
}

static idigi_callback_status_t app_get_device_id_method(idigi_device_id_method_t * const method)
{

    *method = idigi_auto_device_id_method;

    return idigi_callback_continue;
}

/* End of iDigi Connector configuration routines */

/*
 * This routine is called when a configuration error is encountered by the iDigi Connector.
 * This is currently used as a debug tool for finding configuration errors.
 */
void app_config_error(idigi_error_status_t * const error_data)
{

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
        ret = app_get_device_id((uint8_t **)response_data, response_length);
        break;

    case idigi_config_mac_addr:
        ret = app_get_mac_addr((uint8_t **)response_data, response_length);
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

    case idigi_config_remote_configuration:
        *((idigi_service_supported_status_t *)response_data) = app_get_remote_configuration_support();
        ret = 0;
        break;

    case idigi_config_max_transaction:
        *((unsigned int *)response_data) = app_get_max_message_transactions();
        ret = 0;
        break;

    case idigi_config_device_id_method:
        status = app_get_device_id_method(response_data);
        break;

    default:
        APP_DEBUG("idigi_config_callback: unknown configuration request= %d\n", request);
        break;
    }

    status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}

