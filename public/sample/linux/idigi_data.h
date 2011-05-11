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
#include "idk_api.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

#define DEBUG_PRINTF(...)       printf(__VA_ARGS__)

#define NETWORK_READ_SET        0x01 << 0
#define NETWORK_WRITE_SET       0x01 << 1
#define NETWORK_TIMEOUT_SET   0x01 << 2

typedef struct {
    uint8_t         device_id[DEVICE_ID_LENGTH];
    uint8_t         vendor_id[VENDOR_ID_LENGTH];
    uint16_t        tx_keepalive;
    uint16_t        rx_keepalive;
    uint8_t         wait_count;
    char            * device_type;
    char            * server_url;
    char            * password;
    idk_connection_type_t         connection_type;
    uint8_t         mac[MAC_ADDR_LENGTH];

    struct in_addr  ip_addr;

    idk_handle_t                   idk_handle;
    idk_network_handle_t     socket_fd;
    uint8_t                            select_data;
} idigi_data_t;

extern idigi_data_t giDigiSetting;

#ifdef __cplusplus
}
#endif

#endif /* APP_DEF_H_ */
