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
#ifndef APP_DEF_H_
#define APP_DEF_H_

#include <netinet/in.h>
#include <stdio.h>
#include "iik_api.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

#define NETWORK_READ_SET      0x01 << 0
#define NETWORK_WRITE_SET     0x01 << 1
#define NETWORK_TIMEOUT_SET   0x01 << 2

typedef struct {
    uint8_t         vendor_id[VENDOR_ID_LENGTH];
    uint16_t        tx_keepalive;
    uint16_t        rx_keepalive;
    uint8_t         wait_count;
    char            * device_type;
    char            * server_url;
    char            * password;
    uint8_t         * phone_number;
    uint32_t        link_speed;
    iik_connection_type_t connection_type;
    uint8_t         mac_addr[MAC_ADDR_LENGTH];

    uint32_t        ip_addr; /* only IPv4 */

    iik_handle_t             iik_handle;
    iik_network_handle_t     socket_fd;
    uint8_t                  select_data;
} idigi_data_t;

extern idigi_data_t iDigiSetting;
extern time_t  deviceSystemUpStartTime;

iik_callback_status_t idigi_firmware_callback(iik_firmware_request_t request,
                                              void const * request_data, size_t request_length,
                                              void * response_data, size_t * response_length);

iik_callback_status_t idigi_config_callback(iik_config_request_t request,
                                            void const * request_data, size_t request_length,
                                            void * response_data, size_t * response_length);

iik_callback_status_t idigi_network_callback(iik_network_request_t request,
                                            void const * request_data, size_t request_length,
                                            void * response_data, size_t * response_length);
uint8_t network_select(iik_network_handle_t fd, uint8_t select_set, unsigned wait_time);

iik_callback_status_t idigi_os_callback(iik_os_request_t request,
                                        void const * request_data, size_t request_length,
                                        void * response_data, size_t * response_length);


#ifdef __cplusplus
}
#endif

#endif /* APP_DEF_H_ */
