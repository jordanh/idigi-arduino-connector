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

#include "idigi_data.h"


uint8_t phone_number[] = {8,8,8,5,5,5,1,0,0,0};

idigi_data_t iDigiSetting = {

     /* vendor id */
    {0x00, 0x00, 0x00, 0x00},

    /* tx keepalive interval in seconds */
    40,
    /* rx keepalive interval in seconds */
    60,
    /* wait count */
    5,

    /* device type */
    "Linux Application",

    /* server_url */
//    "developer.idigi.com\0",
    "10.52.18.85",

    /* password */
    NULL,

    /* phone number */
    phone_number,
    /* link speed */
    19200,

    /* connection type */
    iik_lan_connection_type,

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


#define REDIRECT_SUCCESSFUL_MESSAGE "Redirect"
#define REDIRECT_ERROR_MESSAGE      "Error in redirect"


iik_callback_status_t idigi_error_status(iik_error_status_t * error_data)
{

    iik_callback_status_t   status = iik_callback_continue;
    char const * error_status_string[] = {"iik_success", "iik_init_error",
                                           "iik_configuration_error",
                                                       "iik_invalid_data_size",
                                                      "iik_invalid_data_range", "iik_invalid_payload_packet",
                                                      "iik_keepalive_error", "iik_server_overload",
                                                      "iik_bad_version", "iik_invalid_packet",
                                                      "iik_exceed_timeout", "iik_unsupported_security",
                                                      "iik_invalid_data", "iik_server_disconnected",
                                                       "iik_connect_error", "iik_receive_error",
                                                      "iik_send_error", "iik_close_error",
                                                      "iik_device_terminated"};


    char const * config_request_string[] = { "iik_config_device_id", "iik_config_vendor_id",
                                             "iik_config_device_type", "iik_config_server_url",
                                             "iik_config_password", "iik_config_connection_type",
                                             "iik_config_mac_addr", "iik_config_link_speed",
                                             "iik_config_phone_number", "iik_config_tx_keepalive",
                                             "iik_config_rx_keepalive", "iik_config_wait_count",
                                             "iik_config_ip_addr", "iik_config_error_status",
                                              "iik_config_disconnected", "iik_config_firmware_facility" };

    char const * network_request_string[] = { "iik_config_connect", "iik_config_send",
                                             "iik_config_receive", "iik_config_close",};

    char const * os_request_string[] = { "iik_config_malloc", "iik_config_free",
                                              "iik_config_system_up_time"};

    char const * firmware_request_string[] = {"iik_firmware_target_count", "iik_firmware_version",
                                              "iik_firmware_code_size", "iik_firmware_description",
                                              "iik_firmware_name_spec", "iik_firmware_download_request",
                                              "iik_firmware_binary_block", "iik_firmware_download_complete",
                                              "iik_firmware_download_abort", "iik_firmware_target_reset"};



    switch (error_data->class)
    {
    case iik_class_config:
        DEBUG_PRINTF("idigi_error_status: Config - %s (%d)  status = %s (%d)\n", config_request_string[error_data->request.config_request],
                     error_data->request.config_request, error_status_string[error_data->status],error_data->status);
        break;
    case iik_class_network:
        DEBUG_PRINTF("idigi_error_status: Network - %s (%d)  status = %s (%d)\n", network_request_string[error_data->request.network_request],
                     error_data->request.network_request, error_status_string[error_data->status],error_data->status);
        break;
    case iik_class_operating_system:
        DEBUG_PRINTF("idigi_error_status: Operating System - %s (%d)  status = %s (%d)\n", os_request_string[error_data->request.os_request],
                     error_data->request.os_request, error_status_string[error_data->status],error_data->status);
        break;
    case iik_class_firmware:
        DEBUG_PRINTF("idigi_error_status: Firmware facility - %s (%d)  status = %s (%d)\n",
                     firmware_request_string[error_data->request.firmware_request],
                     error_data->request.firmware_request,
                     error_status_string[error_data->status],error_data->status);
        break;
    default:
        DEBUG_PRINTF("idigi_error_status: unsupport class = %d status = %d\n", error_data->class, error_data->status);
        break;
    }
    return status;
}
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
        goto done;
    }
    conf.ifc_len = 128*sizeof(struct ifreq);
    conf.ifc_buf = buf;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == sock){
        perror("socket");
        goto done;
    }

    if( ioctl(sock,SIOCGIFCONF , &conf) == -1)
    {
        DEBUG_PRINTF("Error using ioctl SIOCGIFCONF.\n");
        goto done;
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

done:
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

static void get_device_id(uint8_t ** id, size_t * size)
{
    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};
    /* Return pointer to device_id.
     *
     * Use MAC addr to generate device ID.
     *
     */
    device_id[8] = iDigiSetting.mac_addr[0];
    device_id[9] = iDigiSetting.mac_addr[1];
    device_id[10] = iDigiSetting.mac_addr[2];
    device_id[11] = 0xFF;
    device_id[12] = 0xFF;
    device_id[13] = iDigiSetting.mac_addr[3];
    device_id[14] = iDigiSetting.mac_addr[4];
    device_id[15] = iDigiSetting.mac_addr[5];

    *id = device_id;
    *size = sizeof device_id;
}

iik_callback_status_t idigi_config_callback(iik_config_request_t request,
                                            void const * request_data, size_t request_length,
                                            void * response_data, size_t * response_length)
{
    iik_callback_status_t status = iik_callback_continue;

    switch (request)
    {
    case iik_config_device_id:
        /* Return pointer to device ID */
#error  "Must specify device id";
        get_device_id((uint8_t **)response_data, response_length);
        break;

    case iik_config_vendor_id:
    {
        /* Return pointer to vendor id */
        uint8_t ** data = (uint8_t **) response_data;

#error "Must specify vendor id";
        *data = (uint8_t *)iDigiSetting.vendor_id;
        *response_length = sizeof iDigiSetting.vendor_id;
        break;
    }

    case iik_config_device_type:
    {
        /* Return pointer to device type */
        char ** data = (char **) response_data;
#error "Specify device type";
        * data = (char *)iDigiSetting.device_type;
        *response_length = strlen(iDigiSetting.device_type);
         break;
    }

    case iik_config_server_url:
    {
        /* Return pointer to server url to be connected */
        char ** data = (char **) response_data;
#error "Specify iDigi server url";
        *data = (char *)iDigiSetting.server_url;
        *response_length = strlen(iDigiSetting.server_url);
        break;
    }
    case iik_config_password:
    {
        /* Return pointer to password */
        char ** data = (char **) response_data;
#error "Specify password for identity verification";
        *data = (char *)iDigiSetting.password;
        if (iDigiSetting.password != NULL)
        {
            *response_length = strlen(iDigiSetting.password);
        }
        break;
    }
        ;
    case iik_config_connection_type:
    {
        /* Return pointer to connection type */
        iik_connection_type_t  ** data = (iik_connection_type_t **) response_data;

#error "Specify LAN or WAN connection type";
        *data = (iik_connection_type_t *)&iDigiSetting.connection_type;
        break;
    };

    case iik_config_mac_addr:
    {
        /* Return pointer to device MAC address */
        uint8_t ** data = (uint8_t **) response_data;
#error "Specify device MAC address for LAN connection type";
        *data = (uint8_t *)iDigiSetting.mac_addr;
        *response_length = sizeof iDigiSetting.mac_addr;
        break;
    }

    case iik_config_link_speed:
    {
        /* Return pointer to link speed for WAN connection
         * type.
         */
        uint32_t ** data = (uint32_t **) response_data;
#error "Specify LINK SPEED for WAN connection type";
        *data = (uint32_t *)&iDigiSetting.link_speed;
        *response_length = sizeof iDigiSetting.link_speed;
        break;
    }
    case iik_config_phone_number:
    {
        /* Return pointer to phone number dialed for WAN
         * connection type.
         */
        uint8_t ** data = (uint8_t **) response_data;
#error "Specify phone number dialed for WAN connection type";
        *data = (uint8_t *)iDigiSetting.phone_number;
        if (iDigiSetting.phone_number != NULL)
        {
            *response_length = sizeof phone_number;
        }
        break;
    }
    case iik_config_tx_keepalive:
    {
        /* Return pointer to Tx keepalive interval in seconds */
        uint16_t ** data = (uint16_t **) response_data;
#error "Specify TX keepalive interval in seconds";
        *data = (uint16_t *)&iDigiSetting.tx_keepalive;
        *response_length = sizeof iDigiSetting.tx_keepalive;
        break;
    }
    case iik_config_rx_keepalive:
    {
        /* Return pointer to Rx keepalive interval in seconds */
        uint16_t ** data = (uint16_t **) response_data;

#error "Specify RX keepalive interval in seconds";
        *data = (uint16_t *)&iDigiSetting.rx_keepalive;
        *response_length = sizeof iDigiSetting.rx_keepalive;
        break;
    }

    case iik_config_wait_count:
    {
        /* Return pointer to wait count (number of times
         * not receiving Tx keepalive from server.
         */
        uint8_t ** data = (uint8_t **) response_data;

#error "Specify wait count for not receiving TX keepalive";
        *data = (uint8_t *)&iDigiSetting.wait_count;
        *response_length = sizeof iDigiSetting.wait_count;
        break;
    }

    case iik_config_ip_addr:
    {
        /* return pointer to device IP address */
        uint8_t ** data = (uint8_t **) response_data;
        struct in_addr  ip_addr;

#error "Specify Device IP address. Return the response_length to 4-byte for IPv4 or 16-byte for IPv6";

        if (get_device_address(&ip_addr) < 0)
        {
             DEBUG_PRINTF("Error: unable to get device ip address\n");
             status = iik_callback_abort;
        }
        else
        {
            /* IPv4 address */
            iDigiSetting.ip_addr = (uint32_t)ip_addr.s_addr;
            *data = (uint8_t *)&iDigiSetting.ip_addr;
            *response_length = sizeof iDigiSetting.ip_addr;
        }
        break;
    }
    case iik_config_error_status:
        /* IIK encounters some kind of error */
        if (request_length != sizeof(iik_error_status_t))
        {
            DEBUG_PRINTF("Error Status from IIK: given incompatible iik_config_error_status size\n");
        }
        else
        {
            status = idigi_error_status((iik_error_status_t *)request_data);
        }
        break;

    case iik_config_disconnected:
       DEBUG_PRINTF("Disconnected from server\n");
       /* set status to idk_callback_abort to abort IIK or
        * idk_callback_continue to reconnect to server.
        */
        break;
    case iik_config_firmware_facility:
        /* enable Firmware update facility */
        *((bool *)response_data)= true;

        break;
    }

    return status;
}

