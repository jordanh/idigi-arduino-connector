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

#include <psptypes.h>
#include <ipcfg.h>
#include <idigi_api.h>
#include <platform.h>
#include <bele.h>

/* IIK Configuration routines */

#define MAX_INTERFACES      128
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

/*
 * Routine to get the IP address, you will need to modify this routine for your 
 * platform.
 */
static int app_get_ip_address(uint8_t ** ip_address, size_t *size)
{
    IPCFG_IP_ADDRESS_DATA   ip_data;
    static uint32_t val;
    
    ipcfg_get_ip(IPCFG_default_enet_device, &ip_data);

    APP_DEBUG("get_ip_address: IP Address: %d.%d.%d.%d\n\n",IPBYTES(ip_data.ip));

    *size       = sizeof ip_data.ip;

    StoreBE32((void * const)&val, ip_data.ip);
    *ip_address = (uint8_t *)&val;
    
    return 0;
}

static int app_get_mac_addr(uint8_t ** addr, size_t * size)
{
    static _enet_address mac_address = {0};
    idigi_callback_status_t const status = ipcfg_get_mac(IPCFG_default_enet_device, mac_address) ? idigi_callback_continue : idigi_callback_abort;

    APP_DEBUG("get_mac_addr: MAC Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\r\n",
            mac_address[0], mac_address[1], mac_address[2],
            mac_address[3], mac_address[4], mac_address[5]);

    *addr = (uint8_t *)&mac_address;
    *size = sizeof mac_address;

    return status;
}

static int app_get_device_id(uint8_t ** id, size_t * size)
{
    #define DEVICE_ID_LENGTH    16
    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};
    _enet_address mac_address = {0};
    idigi_callback_status_t const status = ipcfg_get_mac(IPCFG_default_enet_device, mac_address) ? idigi_callback_continue : idigi_callback_abort;

    APP_DEBUG("get_device_id: MAC Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\r\n",
            mac_address[0], mac_address[1], mac_address[2],
            mac_address[3], mac_address[4], mac_address[5]);

    /* This sample uses the MAC address to format the device ID */
    device_id[8] = mac_address[0];
    device_id[9] = mac_address[1];
    device_id[10] = mac_address[2];
    device_id[11] = 0xFF;
    device_id[12] = 0xFF;
    device_id[13] = mac_address[3];
    device_id[14] = mac_address[4];
    device_id[15] = mac_address[5];

    *id = device_id;
    *size = sizeof device_id;

    return status;
}
static int app_get_vendor_id(uint8_t ** id, size_t * size)
{
    //static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x01, 0x00, 0x00, 0x20};
	static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x01, 0x00, 0x01, 0x64};
	
    APP_DEBUG("get_vendor_id Vendor ID: %2.2X,%2.2X,%2.2X,%2.2X\r\n", 
              device_vendor_id[0], device_vendor_id[1], device_vendor_id[2], device_vendor_id[3]);

    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return 0;
}

static int app_get_device_type(char ** type, size_t * size)
{
#ifdef TWR_K53N512
    static char const *const device_type = "K53N512";
#else
    static char const *const device_type = "K60N512";
#endif
	
    APP_DEBUG("get_device_type: Device Type %s\r\n", device_type);

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = strlen(device_type);

    return 0;
}

static int app_get_server_url(char ** url, size_t * size)
{
    static char const *const idigi_server_url = "developer.idigi.com";
	
    APP_DEBUG("get_server_url: Server URL %s\r\n", idigi_server_url);
    
    /* Return pointer to device type. */
    *url = (char *)idigi_server_url;
    *size = strlen(idigi_server_url);

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
    return idigi_service_unsupported;
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

/* End of IIK configuration routines */

/*
 * This routine is called when a configuration error is encountered by the IIK.
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
                                       "idigi_file_system_error",
                                       "idigi_file_system_hash"};    

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
                                              void * const response_data,
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

