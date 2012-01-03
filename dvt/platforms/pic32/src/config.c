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

#include "idigi.h"
#include <tcpip_bsd/tcpip_bsd.h>

/* IIK Configuration routines */

#define MAX_INTERFACES 128

static bool get_ip_address(uint8_t ** ip_address, size_t *size){
    IP_ADDR ip_addr;
    ip_addr.Val = TCPIPGetIPAddr();
    
    *size       = sizeof ip_addr.v;
    *ip_address = (uint8_t *)&ip_addr.v;
    
    return true;
}

/* MAC address used in this sample */
static uint8_t device_mac_addr[MAC_ADDR_LENGTH] = {0x00, 0x40, 0x9F, 0x00, 0x00, 0x00};

static bool get_mac_addr(uint8_t ** addr, size_t * size)
{

    *addr = device_mac_addr;
    *size = sizeof device_mac_addr;

    return true;
}

static bool get_device_id(uint8_t ** id, size_t * size)
{
    static uint8_t device_id[DEVICE_ID_LENGTH] = {0};


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
    static const uint8_t device_vendor_id[VENDOR_ID_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    *id   = (uint8_t *)device_vendor_id;
    *size = sizeof device_vendor_id;

    return true;
}

static bool get_device_type(char ** type, size_t * size)
{
    static const char const *device_type = "PIC32 Ethernet Starter Kit";

    /* Return pointer to device type. */
    *type = (char *)device_type;
    *size = strlen(device_type);

    return true;
}

static bool get_server_url(char ** url, size_t * size)
{
    static const char const *idigi_server_url = "10.0.10.105";

    /* Return pointer to device type. */
    *url = (char *)idigi_server_url;
    *size = strlen(idigi_server_url);

    return true;
}

static bool get_connection_type(idigi_connection_type_t ** type)
{
    /* Return pointer to connection type */
    static idigi_connection_type_t  device_connection_type = idigi_lan_connection_type;

    *type = &device_connection_type;

    return true;
}

static bool get_link_speed(uint32_t ** speed, size_t * size)
{
    UNUSED_ARGUMENT(speed);
    UNUSED_ARGUMENT(size);

    return true;
}

static bool get_phone_number(uint8_t ** number, size_t * size)
{
    /* 
     * Return pointer to phone number for WAN connection type.
     */
    UNUSED_ARGUMENT(number);
    UNUSED_ARGUMENT(size);

    return true;
}

/* Keep alives are from the prospective of the server */
/* This keep alive is sent from the server to the device */
static bool get_tx_keepalive_interval(uint16_t ** interval, size_t * size)
{

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
#define DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS     60
    /* Return pointer to Rx keepalive interval in seconds */
    static uint16_t device_rx_keepalive_interval = DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS;
    *interval = (uint16_t *)&device_rx_keepalive_interval;
    *size = sizeof device_rx_keepalive_interval;

    return true;
}

static bool get_wait_count(uint8_t ** count, size_t * size)
{
#define DEVICE_WAIT_COUNT     5
    /* 
     * Return pointer to wait count (number of times not receiving Tx keepalive 
     * from server is allowed).
     */
    static uint8_t device_wait_count = DEVICE_WAIT_COUNT;
    *count = (uint8_t *)&device_wait_count;
    *size = sizeof device_wait_count;

    return true;
}

static bool get_firmware_support(void)
{
    return true;
}

static bool get_data_service_support(void)
{
#if (defined _DATA_SERVICE)
    return true;
#else
    return false;
#endif
}

static bool get_rci_support(void)
{
    return true;
}

/*
 * Configuration callback routine.
 */
idigi_callback_status_t idigi_config_callback(idigi_config_request_t const request,
                                              void * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * response_length)
{
    idigi_callback_status_t status;
    bool ret = false;

    UNUSED_ARGUMENT(request_length);

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
        ret = get_wait_count((uint8_t **)response_data, response_length);
        break;

    case idigi_config_ip_addr:
        ret = get_ip_address((uint8_t **)response_data, response_length);
        break;

    case idigi_config_error_status:
        ret = true;
        break;

    case idigi_config_firmware_facility:
        *((bool *)response_data) = get_firmware_support();
        ret = true;
        break;

    case idigi_config_data_service:
        *((bool *)response_data) = get_data_service_support();
        ret = true;
        break;

    case idigi_config_rci_facility:
        *((bool *)response_data) = get_rci_support();
        ret = true;
        break;

    }

    status = (ret == true) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}

