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
#include <unistd.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "idigi_api.h"
#include "platform.h"

/* iDigi Connector Configuration routines */

#define MAX_INTERFACES      128
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6
#define MAX_URL_LENGTH      256

/* Configuration file */
#define MAXLEN 80
#define CONFIG_FILE "/etc/idigi.conf"
#define MAX_CONFIG_NAME_LEN 64

/* Parameters in configuration file (need to be statically allocted */
static struct idigi_parameters_t
{
    uint8_t vendor_id[VENDOR_ID_LENGTH];
    uint8_t mac_addr[MAC_ADDR_LENGTH];
    char    server_url[MAX_URL_LENGTH];
} idigi_params;

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

    *addr = idigi_params.mac_addr;
    *size = sizeof idigi_params.mac_addr;

    return 0;
}

static int app_get_device_id(uint8_t ** id, size_t * size)
{
    uint8_t * mac_addr;
    size_t mac_size;
    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};


    /* This sample uses the MAC address to format the device ID */
    app_get_mac_addr(&mac_addr, &mac_size);

    device_id[8] = idigi_params.mac_addr[0];
    device_id[9] = idigi_params.mac_addr[1];
    device_id[10] = idigi_params.mac_addr[2];
    device_id[11] = 0xFF;
    device_id[12] = 0xFF;
    device_id[13] = idigi_params.mac_addr[3];
    device_id[14] = idigi_params.mac_addr[4];
    device_id[15] = idigi_params.mac_addr[5];

    *id   = device_id;
    *size = sizeof device_id;

    return 0;
}


static int app_get_vendor_id(uint8_t ** id, size_t * size)
{
    *id   = (uint8_t *)idigi_params.vendor_id;
    *size = sizeof idigi_params.vendor_id;

    return 0;
}

static int app_get_device_type(char ** type, size_t * size)
{
    static const char const *device_type = "Linux Application";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = strlen(device_type);

    return 0;
}

static int app_get_server_url(char ** url, size_t * size)
{
    /* Return pointer to device type. */
    *url = (char *)idigi_params.server_url;
    *size = strlen(idigi_params.server_url);

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
                                             "idigi_config_file_system",
                                             "idigi_config_remote_configuration",
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
    char const * file_system_string[] = {"idigi_file_system_open",
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

    char const * remote_config_string[]= { "idigi_remote_config_session_start",
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
    case idigi_class_file_system:
           APP_DEBUG("idigi_error_status: File system - %s (%d)  status = %s (%d)\n",
                        file_system_string[error_data->request_id.file_system_request],
                        error_data->request_id.file_system_request,
                        error_status_string[error_data->status],error_data->status);
           break;
    case idigi_class_remote_config_service:
           APP_DEBUG("idigi_error_status: Remote configuration - %s (%d)  status = %s (%d)\n",
                   remote_config_string[error_data->request_id.remote_config_request],
                        error_data->request_id.remote_config_request,
                        error_status_string[error_data->status],error_data->status);
           break;
    default:
        APP_DEBUG("idigi_error_status: unsupport class_id = %d status = %d\n", error_data->class_id, error_data->status);
        break;
    }
}

/*
 * Initialize configuration parameters to default values
 */
static void init_parameters (struct idigi_parameters_t *parms)
{
  memset(parms->vendor_id, 0, VENDOR_ID_LENGTH);
  memset(parms->mac_addr, 0, MAC_ADDR_LENGTH);
  strncpy (parms->server_url, "developer.idigi.com", MAX_URL_LENGTH);
}

static char *trim(char *s)
{
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];

  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';

  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;

  /* Copy finished string */
  strcpy (s, s1);
  return s;
}
#define BAD_HEX_DIGIT 0xff
static unsigned char convertHexDigit(char hexDigit)
{
    unsigned char returnValue = BAD_HEX_DIGIT;
    static char const table[]="0123456789ABCDEF";
    unsigned int idx;

    for (idx = 0; idx < strlen(table); idx++)
    {
        if (table[idx] == toupper(hexDigit))
        {
            returnValue = idx;
            break;
        }
    }

    return returnValue;
}
static int convert_address_string(unsigned char * dest, char * source, int length)
{
    int idx;
    unsigned char hi, low;

    for (idx = 0; idx < length; idx++)
    {
        hi  = convertHexDigit(*source++);
        low = convertHexDigit(*source++);
        if ((hi == BAD_HEX_DIGIT) || (low == BAD_HEX_DIGIT))
        {
            return -1;
        }
        dest[idx] = (hi << 4) | low;
    }
    return 0;
}


static void parse_config(struct idigi_parameters_t *parms)
{
  char *s, buff[256];
  FILE *fp = fopen (CONFIG_FILE, "r");

  if (fp == NULL)
  {
    APP_DEBUG("could not open config file [%s]\n", CONFIG_FILE);
    return;
  }

  APP_DEBUG("--->parsing config file [%s]<----\n", CONFIG_FILE);
  /* Read next line */
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    int ret;

    /* Skip blank lines and comments */
    if (buff[0] == '\n' || buff[0] == '#')
      continue;

    /* Parse name/value pair from line */
    char name[MAXLEN], value[MAXLEN];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else
      strncpy (name, s, MAXLEN);
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, MAXLEN);
    trim (value);

    APP_DEBUG("config found pair: %s=%s\n", name, value);

    /* Copy into correct entry in parameters struct */
    if (strcmp(name, "vendor_id")==0)
    {
       ret = convert_address_string(parms->vendor_id, value, VENDOR_ID_LENGTH);
       if (ret < 0)
       {
           APP_DEBUG("config: Vendor ID invalid\n");
           continue;
       }
       APP_DEBUG("config: Vendor ID: [%02X:%02X:%02X:%02X]\n", parms->vendor_id[0], parms->vendor_id[1],
                                                               parms->vendor_id[2],parms->vendor_id[3]);
    }
    else if (strcmp(name, "mac_addr")==0)
    {
       ret = convert_address_string(parms->mac_addr, value, MAC_ADDR_LENGTH);
       if (ret < 0)
       {
           APP_DEBUG("config: MAC Address invalid\n");
           continue;
       }
       APP_DEBUG("config: MAC Address: [%02X:%02X:%02X:%02X:%02X:%02X]\n", parms->mac_addr[0], parms->mac_addr[1],
                parms->mac_addr[2],parms->mac_addr[3], parms->mac_addr[4],parms->mac_addr[5]);
    }
    else if (strcmp(name, "server_url")==0)
    {
      strncpy (parms->server_url, value, MAXLEN);
      APP_DEBUG("config: server_url [%s]\n", parms->server_url);
    }
    else
    {
      APP_DEBUG("WARNING: %s/%s: Unknown name/value pair!\n", name, value);
    }
  }

  /* Close file */
  fclose (fp);
}

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
    static int first=1;

    UNUSED_ARGUMENT(request_length);

    if (first)
    {
        init_parameters(&idigi_params);
        parse_config(&idigi_params);
        first = 0;
    }

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

    case idigi_config_file_system:
        *((idigi_service_supported_status_t *)response_data) = app_get_file_system_support();
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

    default:
        APP_DEBUG("idigi_config_callback: unknown configuration request= %d\n", request);
        break;
    }

    status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}

