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
#include <unistd.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "idigi_api.h"

/* IIK Configuration routines */

#define MAX_INTERFACES 128
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

/*
 * Routine to get the IP address, you will need to modify this routine for your 
 * platform.
 */
static bool get_ip_address(uint8_t ** ip_address, size_t *size)
{
    int             fd = -1;
    bool            status=false;
    char            *buf = malloc(MAX_INTERFACES*sizeof(struct ifreq));
    struct ifconf   conf;
    unsigned int    entries = 0;
    unsigned int    i;
    static struct in_addr  ip_addr; /* Used to store the IP address */

/* Remove this //#error statement once you modify this routine to return the correct IP address */
//#error "Specify device IP address. Set size to 4 (bytes) for IPv4 or 16 (bytes) for IPv6"

    if (buf == NULL)
    {
        DEBUG_PRINTF("get_ip_address malloc failed\n");
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
        DEBUG_PRINTF("get_ip_address: Error using ioctl SIOCGIFCONF.\n");
        goto error;
    }

    entries = conf.ifc_len / sizeof(struct ifreq);

    DEBUG_PRINTF("get_ip_address: Looking for current device IP address: found [%d] entries\n", entries);

    if (entries == 0)
    {
        goto error;
    }
    for( i = 0; i < entries; i++)
    {
        struct ifreq * req = &conf.ifc_req[i];
        struct sockaddr_in * sa = (struct sockaddr_in *) &req->ifr_addr;

        DEBUG_PRINTF("get_ip_address: %d: Interface name [%s]\tIP Address [%s]\n", i+1, req->ifr_name, inet_ntoa(sa->sin_addr));
        if (sa->sin_addr.s_addr != htonl(INADDR_LOOPBACK))
        {
            ip_addr = sa->sin_addr;
            break;
        }
    }

    /* Fill in the size and IP address */
    *size       = sizeof ip_addr.s_addr;
    *ip_address = (uint8_t *)&ip_addr.s_addr;
    status = true;

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

/* MAC address used in this sample */
static uint8_t device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static bool get_mac_addr(uint8_t ** addr, size_t * size)
{
//#error "Specify device MAC address for LAN connection"

    *addr = device_mac_addr;
    *size = sizeof device_mac_addr;

    return true;
}

static bool get_device_id(uint8_t ** id, size_t * size)
{
    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};

//#error  "Specify device id"

    /* This sample uses the MAC address to format the device ID */
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

    return true;
}

static bool get_vendor_id(uint8_t ** id, size_t * size)
{
//#error  "Specify vendor id"
    static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return true;
}

static bool get_device_type(char ** type, size_t * size)
{
//#error "Specify device type"
    static const char const *device_type = "Linux Application";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = strlen(device_type);

    return true;
}

static bool get_server_url(char ** url, size_t * size)
{
//#error "Specify iDigi Server URL"
    static const char const *idigi_server_url = "developer.idigi.com";

    /* Return pointer to device type. */
    *url = (char *)idigi_server_url;
    *size = strlen(idigi_server_url);

    return true;
}

static bool get_connection_type(idigi_connection_type_t ** type)
{
//#error "Specify LAN or WAN connection type"

    /* Return pointer to connection type */
    static idigi_connection_type_t  device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return true;
}

static bool get_link_speed(uint32_t ** speed, size_t * size)
{
//#error "Specify link speed for WAN connection type"
    UNUSED_PARAMETER(speed);
    UNUSED_PARAMETER(size);

    return true;
}

static bool get_phone_number(uint8_t ** number, size_t * size)
{
//#error "Specify phone number dialed for WAN connection type"
    /* 
     * Return pointer to phone number for WAN connection type.
     */
    UNUSED_PARAMETER(number);
    UNUSED_PARAMETER(size);

    return true;
}

/* Keep alives are from the prospective of the server */
/* This keep alive is sent from the server to the device */
static bool get_tx_keepalive_interval(uint16_t ** interval, size_t * size)
{
//#error "Specify server to device TX keepalive interval in seconds"

#define DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS     90
    /* Return pointer to Tx keepalive interval in seconds */
    static uint16_t device_tx_keepalive_interval = DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = (uint16_t *)&device_tx_keepalive_interval;
    *size = sizeof device_tx_keepalive_interval;

    return true;
}


/* This keep alive is sent from the device to the server  */
static bool get_rx_keepalive_interval(uint16_t ** interval, size_t * size)
{
//#error "Specify server to device RX keepalive interval in seconds"
#define DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS     60
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = (uint16_t *)&device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return true;
}

static bool get_wait_count(uint16_t ** count, size_t * size)
{
//#error "Specify the number of times that not receiving keepalive messages from server is allowed"
#define DEVICE_WAIT_COUNT     5
    /* 
     * Return pointer to wait count (number of times not receiving Tx keepalive 
     * from server is allowed).
     */
    static uint16_t device_wait_count = DEVICE_WAIT_COUNT;
    *count = (uint16_t *)&device_wait_count;
    *size = sizeof device_wait_count;

    return true;
}

static bool get_firmware_support(void)
{
    return true;
}

static bool get_data_service_support(void)
{
#if (defined IDIGI_DATA_SERVICE)
    return true;
#else
    return false;
#endif
}


/* End of IIK configuration routines */

/*
 * This routine is called when a configuration error is encountered by the IIK.
 * This is currently used as a debug tool for finding configuration errors.
 */
static bool idigi_config_error(idigi_error_status_t * const error_data)
{

    bool status = true;

#if defined(DEBUG)
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
                                             "idigi_config_disconnected",
                                             "idigi_config_firmware_facility",
                                             "idigi_config_data_service"};

    char const * network_request_string[] = { "idigi_config_connect",
                                              "idigi_config_send",
                                              "idigi_config_receive",
                                              "idigi_config_close"};

    char const * os_request_string[] = { "idigi_config_malloc",
                                         "idigi_config_free",
                                         "idigi_config_system_up_time"};

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

    char const * data_service_string[] = {"idigi_data_service_send_complete",
                                          "idigi_data_service_response",
                                          "idigi_data_service_error"};
    switch (error_data->class_id)
    {
    case idigi_class_config:
        DEBUG_PRINTF("idigi_error_status: Config - %s (%d)  status = %s (%d)\n", config_request_string[error_data->request_id.config_request],
                     error_data->request_id.config_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_network:
        DEBUG_PRINTF("idigi_error_status: Network - %s (%d)  status = %s (%d)\n", network_request_string[error_data->request_id.network_request],
                     error_data->request_id.network_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_operating_system:
        DEBUG_PRINTF("idigi_error_status: Operating System - %s (%d)  status = %s (%d)\n", os_request_string[error_data->request_id.os_request],
                     error_data->request_id.os_request, error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_firmware:
        DEBUG_PRINTF("idigi_error_status: Firmware facility - %s (%d)  status = %s (%d)\n",
                     firmware_request_string[error_data->request_id.firmware_request],
                     error_data->request_id.firmware_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    case idigi_class_data_service:
        DEBUG_PRINTF("idigi_error_status: Data service - %s (%d)  status = %s (%d)\n",
                     data_service_string[error_data->request_id.data_service_request],
                     error_data->request_id.data_service_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    default:
        DEBUG_PRINTF("idigi_error_status: unsupport class_id = %d status = %d\n", error_data->class_id, error_data->status);
        break;
    }
#else
    UNUSED_PARAMETER(error_data);
#endif

    return status;
}

/*
 * Configuration callback routine.
 */
idigi_callback_status_t idigi_config_callback(idigi_config_request_t const request,
                                              void * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * const response_length)
{
    idigi_callback_status_t status;
    bool ret = false;

    UNUSED_PARAMETER(request_length);

    switch (request)
    {
    case idigi_config_device_id:
        ret = get_device_id((uint8_t **)response_data, response_length);
        break;

    case idigi_config_vendor_id:
        ret = get_vendor_id((uint8_t **)response_data, response_length);
        break;

    case idigi_config_device_type:
        ret = get_device_type((char **)response_data, response_length);
        break;

    case idigi_config_server_url:
        ret = get_server_url((char **)response_data, response_length);
        break;

    case idigi_config_connection_type:
        ret = get_connection_type((idigi_connection_type_t **)response_data);
        break;

    case idigi_config_mac_addr:
        ret = get_mac_addr((uint8_t **)response_data, response_length);
        break;

    case idigi_config_link_speed:
        ret = get_link_speed((uint32_t **)response_data, response_length);
        break;

    case idigi_config_phone_number:
        ret = get_phone_number((uint8_t **)response_data, response_length);
       break;

    case idigi_config_tx_keepalive:
        ret = get_tx_keepalive_interval((uint16_t **)response_data, response_length);
        break;

    case idigi_config_rx_keepalive:
        ret = get_rx_keepalive_interval((uint16_t **)response_data, response_length);
        break;

    case idigi_config_wait_count:
        ret = get_wait_count((uint16_t **)response_data, response_length);
        break;

    case idigi_config_ip_addr:
        ret = get_ip_address((uint8_t **)response_data, response_length);
        break;

    case idigi_config_error_status:
        ret = idigi_config_error((idigi_error_status_t *)request_data);
        break;

    case idigi_config_firmware_facility:
        *((bool *)response_data) = get_firmware_support();
        ret = true;
        break;

    case idigi_config_data_service:
        *((bool *)response_data) = get_data_service_support();
        ret = true;
        break;

    }

    status = (ret == true) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}

