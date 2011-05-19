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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include <malloc.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <arpa/inet.h>


#include "idigi_data.h"
#include "os.h"
#include "network.h"
#include "firmware.h"

bool gAbort = false;
idigi_data_t giDigiSetting = {
    /* device id */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* vendor id */
    {0x00, 0x00, 0x00, 0x00},

    /* tx keepalive */
    30,
    /* rx keepalive */
    30,
    /* wait count */
    3,

    /* device type */
    "Linux Application",

    /* server_url */
    "developer.idigi.com",

    /* password */
    NULL,

    /* phone number */
    NULL,
    /* link speed */
    0,

    /* connection type */
    idk_lan_connection_type,

    /* mac address */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* ip address */
    0,

    /* idk handle  (application) */
   NULL,

    /* socket fd */
    -1,

    /* select data */
    0
};

time_t  gSystemTime;


#define REDIRECT_SUCCESSFUL_MESSAGE "Redirect"
#define REDIRECT_ERROR_MESSAGE      "Error in redirect"

static int get_device_address(struct in_addr * eth_addr)
{
    int             sock = -1;
    char            * buf = malloc(128* sizeof(struct ifreq));
    struct ifconf   conf;
    unsigned int    entries = 0;
    int             i;
    int             rc = -1;

    if (buf == NULL)
    {
        goto _ret;
    }
    conf.ifc_len = 128*sizeof(struct ifreq);
    conf.ifc_buf = buf;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == sock){
        perror("socket");
        goto _ret;
    }

    if( ioctl(sock,SIOCGIFCONF , &conf) == -1)
    {
        DEBUG_PRINTF("Error using ioctl SIOCGIFCONF.\n");
        goto _ret;
    }

    entries = conf.ifc_len / sizeof(struct ifreq);

    DEBUG_PRINTF("Looking current device IP address: found %d entries\n", entries);

    for( i = 0; i<entries; i++)
    {
        struct ifreq * req = &conf.ifc_req[i];
        struct sockaddr_in * sa = (struct sockaddr_in *) &req->ifr_addr;

        DEBUG_PRINTF("%d: %s, IP: %s\n", i+1, req->ifr_name, inet_ntoa(sa->sin_addr));
        if (sa->sin_addr.s_addr != htonl(INADDR_LOOPBACK))
        {
            *eth_addr = sa->sin_addr;
            break;
        }
    }

    rc = 0;

_ret:
    if (sock != -1)
    {
        close(sock);
    }
    if (buf != NULL)
    {
        free(buf);
    }
    return rc;
}

idk_callback_status_t idigi_error_status(idk_error_status_t * error_data)
{

    idk_callback_status_t   status = idk_callback_abort;
    char * error_status_string[] = {"idk_success", "idk_init_error",
                                                       "idk_configuration_error",
                                                       "idk_invalid_data_size",
                                                      "idk_invalid_data_range", "idk_invalid_payload_packet",
                                                      "idk_keepalive_error", "idk_server_overload",
                                                      "idk_bad_version", "idk_invalid_packet",
                                                      "idk_exceed_timeout", "idk_unsupported_security",
                                                      "idk_invalid_data", "idk_server_disconnected",
                                                       "idk_connect_error", "idk_receive_error",
                                                      "idk_send_error", "idk_close_error",
                                                      "idk_device_terminated"};

    char * base_request_string[] = { "idk_base_device_id", "idk_base_vendor_id",
                                                         "idk_base_device_type", "idk_base_server_url",
                                                         "idk_base_password", "idk_base_connection_type",
                                                         "idk_base_mac_addr", "idk_base_link_speed",
                                                         "idk_base_phone_number", "idk_base_tx_keepalive",
                                                         "idk_base_rx_keepalive", "idk_base_wait_count",
                                                         "idk_base_ip_addr", "idk_base_error_status",
                                                         "idk_base_disconnected", "idk_base_connect",
                                                         "idk_base_send", "idk_base_receive",
                                                         "idk_base_close", "idk_base_malloc",
                                                         "idk_base_free", "idk_base_system_time",
                                                         "idk_base_firmware_facility" };
    char * firmware_request_string[] = {"idk_firmware_target_count", "idk_firmware_version",
                                                              "idk_firmware_code_size", "idk_firmware_description",
                                                              "idk_firmware_name_spec", "idk_firmware_download_request",
                                                              "idk_firmware_binary_block", "idk_firmware_download_complete",
                                                              "idk_firmware_download_abort", "idk_firmware_target_reset"};



    switch (error_data->class)
    {
    case idk_class_base:
        DEBUG_PRINTF("idigi_error_status: BASE - %s (%d)  status = %s (%d)\n", base_request_string[error_data->request.base_request], error_data->request.base_request,
                                   error_status_string[error_data->status],error_data->status);
        break;
    case idk_class_firmware:
        DEBUG_PRINTF("idigi_error_status: Firmware facility - %s (%d)  status = %s (%d)\n", firmware_request_string[error_data->request.firmware_request], error_data->request.firmware_request,
                                   error_status_string[error_data->status],error_data->status);
        break;
    default:
        DEBUG_PRINTF("idigi_error_status: unsupport class = %d status = %d\n", error_data->class, error_data->status);
        break;
    }
    return status;
}

idk_callback_status_t idigi_base_callback(idk_base_request_t request,
                                                  void const * request_data, size_t request_length,
                                                  void * response_data, size_t * response_length)
{
    idk_callback_status_t status = idk_callback_continue;

    switch (request)
    {
    case idk_base_device_id:
    {
        uint8_t ** data = (uint8_t **) response_data;

#error  "Must specify device id";
        *data = (uint8_t *)giDigiSetting.device_id;
        *response_length = sizeof giDigiSetting.device_id;
        break;
    }

    case idk_base_vendor_id:
    {
        uint8_t ** data = (uint8_t **) response_data;

#error "Must specify vendor id";
        *data = (uint8_t *)giDigiSetting.vendor_id;
        *response_length = sizeof giDigiSetting.vendor_id;
        break;
    }

    case idk_base_device_type:
    {
        char ** data = (char **) response_data;
#error "Specify device type";
        * data = (char *)giDigiSetting.device_type;
        *response_length = sizeof giDigiSetting.device_type;
         break;
    }

    case idk_base_server_url:
    {
        char ** data = (char **) response_data;
#error "Specify iDigi server url";
        *data = (char *)giDigiSetting.server_url;
        *response_length = strlen(giDigiSetting.server_url);
        break;
    }
    case idk_base_password:
    {
        char ** data = (char **) response_data;
#error "Specify password for identity verification";
        *data = (char *)giDigiSetting.password;
        if (giDigiSetting.password != NULL)
        {
            *response_length = strlen(giDigiSetting.password);
        }
        break;
    }
        ;
    case idk_base_connection_type:
    {
        idk_connection_type_t  ** data = (idk_connection_type_t **) response_data;

#error "Specify LAN or WAN connection type";
        *data = (idk_connection_type_t *)&giDigiSetting.connection_type;
        break;
    };

    case idk_base_mac_addr:
    {
        uint8_t ** data = (uint8_t **) response_data;
#error "Specify device MAC address for LAN connection type";
        *data = (uint8_t *)giDigiSetting.mac_addr;
        *response_length = sizeof giDigiSetting.mac_addr;
        break;
    }

    case idk_base_link_speed:
    {
        uint16_t ** data = (uint16_t **) response_data;
#error "Specify LINK SPEED for WAN connection type";
        *data = (uint16_t *)&giDigiSetting.link_speed;
        *response_length = sizeof giDigiSetting.link_speed;
        break;
    }
    case idk_base_phone_number:
    {
        char ** data = (char **) response_data;
#error "Specify phone number dialed for WAN connection type";
        *data = (char *)giDigiSetting.phone_number;
        if (giDigiSetting.phone_number != NULL)
        {
            *response_length = strlen(giDigiSetting.phone_number);
        }
        break;
    }
    case idk_base_tx_keepalive:
    {
        uint16_t ** data = (uint16_t **) response_data;
#error "Specify TX keepalive interval in seconds";
        *data = (uint16_t *)&giDigiSetting.tx_keepalive;
        *response_length = sizeof giDigiSetting.tx_keepalive;
        break;
    }
    case idk_base_rx_keepalive:
    {
        uint16_t ** data = (uint16_t **) response_data;

#error "Specify RX keepalive interval in seconds";
        *data = (uint16_t *)&giDigiSetting.rx_keepalive;
        *response_length = sizeof giDigiSetting.rx_keepalive;
        break;
    }

    case idk_base_wait_count:
    {
        uint8_t ** data = (uint8_t **) response_data;

#error "Specify wait count for not receiving TX keepalive";
        *data = (uint8_t *)&giDigiSetting.wait_count;
        *response_length = sizeof giDigiSetting.wait_count;
        break;
    }

    case idk_base_ip_addr:
    {
        uint8_t ** data = (uint8_t **) response_data;

#error "Specify Device IP address. Return the response_length to4-byte for IPv4 or 16-byte for IPv6";
        *data = (uint8_t *)&giDigiSetting.ip_addr;
        *response_length = sizeof giDigiSetting.ip_addr;
    }
        break;
    case idk_base_error_status:
        if (request_length != sizeof(idk_error_status_t))
        {
            DEBUG_PRINTF("idigi_base_callback: given incompatible idk_base_error_status size\n");
        }
        else
        {
            status = idigi_error_status((idk_error_status_t *)request_data);
        }
        break;

    case idk_base_disconnected:
#error "Disconnect server"
        giDigiSetting.socket_fd = -1;
        break;
    case idk_base_connect:
        status = network_connect((idk_connect_request_t *)request_data);
        if (status == idk_callback_continue)
        {
            idk_network_handle_t ** data = (idk_network_handle_t **)response_data;
            *data = (idk_network_handle_t *)&giDigiSetting.socket_fd;
            *response_length = sizeof giDigiSetting.socket_fd;
        }
        break;
    case idk_base_send:
        status = network_send((idk_write_request_t *)request_data, (size_t *)response_data);
        break;
    case idk_base_receive:
        status = network_receive((idk_read_request_t *)request_data, (size_t *)response_data);
        break;

    case idk_base_close:
        status = network_close((idk_network_handle_t *)request_data);
        break;

    case idk_base_malloc:
        status = os_malloc(*((size_t *)request_data), (void **)response_data);
        break;

    case idk_base_free:
        status = os_free((void *)request_data);
        break;

    case idk_base_system_time:
        status = os_get_system_time((uint32_t *)response_data);
        break;

    case idk_base_firmware_facility:
        *((bool *)response_data)= true;

        break;
    }

    return status;
}


idk_callback_status_t idigi_callbak(idk_class_t class, idk_request_t request,
                                    void const * request_data, size_t request_length,
                                    void * response_data, size_t * response_length)
{
    idk_callback_status_t   status = idk_callback_continue;

    switch (class)
    {
    case idk_class_base:
        status = idigi_base_callback(request.base_request, request_data, request_length, response_data, response_length);
        break;
    case idk_class_firmware:
        status = idigi_firmware_callback(request.firmware_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}


int main (void)
{
    idk_status_t status = idk_success;
    struct in_addr  ip_addr;

    time(&gSystemTime);

    if (get_device_address(&ip_addr) < 0)
    {
         DEBUG_PRINTF("main: unable to get device ip address\n");
         goto _ret;
    }

    /* IPv4 address */
    giDigiSetting.ip_addr = (uint32_t)ip_addr.s_addr;

    giDigiSetting.idk_handle = idk_init((idk_callback_t) idigi_callbak);
    if (giDigiSetting.idk_handle != 0)
    {

        while (status == idk_success)
        {
            status = idk_step(giDigiSetting.idk_handle);
             giDigiSetting.select_data = 0;

            if (status != idk_success)
            {
                printf("main: idk_task returns error %d\n", status);
            }
            else
            {
                giDigiSetting.select_data |= NETWORK_TIMEOUT_SET | NETWORK_READ_SET;
                network_select(giDigiSetting.socket_fd, giDigiSetting.select_data, 1);
            }
        }

    }
_ret:
    return 0;
}
