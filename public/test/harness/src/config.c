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

#include "idigi_struct.h"
#include "idigi_data.h"
uint8_t phone_number[] = {8,8,8,5,5,5,1,0,0,0};

#define REDIRECT_SUCCESSFUL_MESSAGE "Redirect"
#define REDIRECT_ERROR_MESSAGE      "Error in redirect"

idigi_data_t iDigiSetting;

idigi_callback_status_t idigi_error_status(idigi_error_status_t * error_data)
{

    idigi_callback_status_t   status = idigi_callback_continue;

#if defined(DEBUG)
    char const * error_status_string[] = {"idigi_success", "idigi_init_error",
                                           "idigi_configuration_error",
                                                       "idigi_invalid_data_size",
                                                      "idigi_invalid_data_range", "idigi_invalid_payload_packet",
                                                      "idigi_keepalive_error", "idigi_server_overload",
                                                      "idigi_bad_version", "idigi_invalid_packet",
                                                      "idigi_exceed_timeout", "idigi_unsupported_security",
                                                      "idigi_invalid_data", "idigi_server_disconnected",
                                                       "idigi_connect_error", "idigi_receive_error",
                                                      "idigi_send_error", "idigi_close_error",
                                                      "idigi_device_terminated"};


    char const * config_request_string[] = { "idigi_config_device_id", "idigi_config_vendor_id",
                                             "idigi_config_device_type", "idigi_config_server_url",
                                             "idigi_config_connection_type", "idigi_config_mac_addr",
                                             "idigi_config_link_speed", "idigi_config_phone_number",
                                             "idigi_config_tx_keepalive", "idigi_config_rx_keepalive",
                                             "idigi_config_wait_count", "idigi_config_ip_addr",
                                             "idigi_config_error_status", "idigi_config_disconnected",
                                             "idigi_config_firmware_facility", "idigi_config_data_service"};

    char const * network_request_string[] = { "idigi_config_connect", "idigi_config_send",
                                             "idigi_config_receive", "idigi_config_close"};

    char const * os_request_string[] = { "idigi_config_malloc", "idigi_config_free",
                                              "idigi_config_system_up_time"};

    char const * firmware_request_string[] = {"idigi_firmware_target_count", "idigi_firmware_version",
                                              "idigi_firmware_code_size", "idigi_firmware_description",
                                              "idigi_firmware_name_spec", "idigi_firmware_download_request",
                                              "idigi_firmware_binary_block", "idigi_firmware_download_complete",
                                              "idigi_firmware_download_abort", "idigi_firmware_target_reset"};

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
#endif
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

idigi_callback_status_t idigi_config_callback(idigi_config_request_t request,
                                            void * const request_data, size_t request_length,
                                            void * response_data, size_t * response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    switch (request)
    {
    case idigi_config_device_id:
        /* Return pointer to device ID */
        get_device_id((uint8_t **)response_data, response_length);
        break;

    case idigi_config_vendor_id:
    {
        /* Return pointer to vendor id */
        uint8_t ** data = (uint8_t **) response_data;

        *data = (uint8_t *)iDigiSetting.vendor_id;
        *response_length = sizeof iDigiSetting.vendor_id;
        break;
    }

    case idigi_config_device_type:
    {
        /* Return pointer to device type */
        char ** data = (char **) response_data;
        * data = (char *)iDigiSetting.device_type;
        *response_length = strlen(iDigiSetting.device_type);
         break;
    }

    case idigi_config_server_url:
    {
        /* Return pointer to server url to be connected */
        char ** data = (char **) response_data;
        *data = (char *)iDigiSetting.server_url;
        *response_length = strlen(iDigiSetting.server_url);
        break;
    }

    case idigi_config_connection_type:
    {
        /* Return pointer to connection type */
        idigi_connection_type_t  ** data = (idigi_connection_type_t **) response_data;

        *data = (idigi_connection_type_t *)&iDigiSetting.connection_type;
        break;
    }

    case idigi_config_mac_addr:
    {
        /* Return pointer to device MAC address */
        uint8_t ** data = (uint8_t **) response_data;
        *data = (uint8_t *)iDigiSetting.mac_addr;
	*response_length = sizeof iDigiSetting.mac_addr;
        break;
    }

    case idigi_config_link_speed:
    {
        /* Return pointer to link speed for WAN connection
         * type.
         */
        uint32_t ** data = (uint32_t **) response_data;
        *data = (uint32_t *)&iDigiSetting.link_speed;
        *response_length = sizeof iDigiSetting.link_speed;
        break;
    }
    case idigi_config_phone_number:
    {
        /* Return pointer to phone number dialed for WAN
         * connection type.
         */
        uint8_t ** data = (uint8_t **) response_data;
        *data = (uint8_t *)iDigiSetting.phone_number;
        if (iDigiSetting.phone_number != NULL)
        {
            *response_length = sizeof phone_number;
        }
        break;
    }
    case idigi_config_tx_keepalive:
    {
        /* Return pointer to Tx keepalive interval in seconds */
        uint16_t ** data = (uint16_t **) response_data;
        *data = (uint16_t *)&iDigiSetting.tx_keepalive;
        *response_length = sizeof iDigiSetting.tx_keepalive;
        break;
    }
    case idigi_config_rx_keepalive:
    {
        /* Return pointer to Rx keepalive interval in seconds */
        uint16_t ** data = (uint16_t **) response_data;
	*data = (uint16_t *)&iDigiSetting.rx_keepalive;
        *response_length = sizeof iDigiSetting.rx_keepalive;
        break;
    }

    case idigi_config_wait_count:
    {
        /* Return pointer to wait count (number of times
         * not receiving Tx keepalive from server.
         */
        uint8_t ** data = (uint8_t **) response_data;

        *data = (uint8_t *)&iDigiSetting.wait_count;
        *response_length = sizeof iDigiSetting.wait_count;
        break;
    }

    case idigi_config_ip_addr:
    {
        /* return pointer to device IP address */
        uint8_t ** data = (uint8_t **) response_data;
        struct in_addr  ip_addr = {0};


        if (get_device_address(&ip_addr) < 0)
        {
             DEBUG_PRINTF("Error: unable to get device ip address\n");
             status = idigi_callback_abort;
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
    case idigi_config_error_status:
        /* iDigi encounters some kind of error */
        if (request_length != sizeof(idigi_error_status_t))
        {
            DEBUG_PRINTF("Error Status from iDigi: given incompatible idigi_config_error_status size\n");
        }
        else
        {
            status = idigi_error_status((idigi_error_status_t *)request_data);
        }
        break;
	
    case idigi_config_firmware_facility:
        /* enable Firmware update facility */
        *((bool *)response_data)= true;

        break;

    case idigi_config_data_service:
        *((bool *)response_data) = true;
        
        break;

    }

    return status;
}

