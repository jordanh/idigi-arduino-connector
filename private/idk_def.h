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

#ifndef IDK_DEF_H_
#define IDK_DEF_H_

//#include "idk_api.h"
#include "ei_packet.h"

#ifdef __cplusplus
extern "C"
{
#endif
//#define DEBUG_PRINTF(...)

#define DEBUG_PRINTF(...)       printf(__VA_ARGS__)


#define IDK_MT_VERSION           2
#define IDK_MT_PORT              3197
#define IDK_MSG_MAX_PACKET_SIZE 1600

#define IDK_DEVICE_TYPE_LENGTH  32
#define IDK_DEVICE_ID_LENGTH    16
#define IDK_VENDOR_ID_LENGTH    4
#define IDK_SERVER_URL_LENGTH   255
#define IDK_MAC_ADDR_LENGTH     6
#define IDK_LINK_SPEED_LENGTH   4

#define IDK_RX_INTERVAL_MIN     5
#define IDK_RX_INTERVAL_MAX     7200
#define IDK_TX_INTERVAL_MIN     5
#define IDK_TX_INTERVAL_MAX     7200
#define IDK_WAIT_COUNT_MIN      2
#define IDK_WAIT_COUNT_MAX      64

#define IDK_MIN(x,y)        (((x) < (y))? (x): (y))
#define IDK_MAX(x,y)        (((x) > (y))? (x): (y))
#define IDK_MILLISECONDS            1000

#define  IDK_IS_SELECT_SET(x, y)        (x & y)
#define  IDK_PACKET_DATA_POINTER(p, s)  (uint8_t *)((uint8_t *)p + s)

/* IRL EDP States */
typedef enum {
    edp_init_layer,
    edp_communication_layer,
    edp_initialization_layer,
    edp_security_layer,
    edp_discovery_layer,
    edp_facility_layer
} idk_edp_state_t;

/* layer states */
typedef enum {
    layer_init_state,
    layer_connect_state,
    layer_redirect_state,
    layer_send_version_state,
    layer_send_ka_params_state,
    layer_process_packet_state,
    layer_security_device_id_state,
    layer_security_server_url_state,
    layer_security_password_state,
    layer_discovery_device_type_state,
    layer_discovery_facility_init_state,
    layer_discovery_facility_state,
    layer_discovery_complete_state,
    layer_done_state
} idk_layer_state_t;

struct idk_data;
struct idk_facility;

typedef idk_callback_status_t (* idk_facility_process_cb_t )(struct idk_data * idk_ptr, struct idk_facility * fac_ptr);

typedef struct idk_facility {
    uint16_t facility_num;
    size_t size;
    idk_facility_process_cb_t discovery_cb;
    idk_facility_process_cb_t process_cb;
    idk_facility_packet_t * packet;
    uint8_t buffer[IDK_MSG_MAX_PACKET_SIZE];
    struct idk_facility * next;
} idk_facility_t;

typedef struct idk_data {
    int active_state;

    idk_callback_t callback;

    uint8_t * device_id;
    uint8_t * vendor_id;
    char * device_type;
    char * server_url;
    char * password;
    int  request_id;

    uint16_t facilities;
    idk_network_handle_t * network_handle;

    uint16_t    * tx_keepalive;
    uint16_t    * rx_keepalive;
    uint8_t  * wait_count;
    uint32_t    rx_ka_time;
    uint32_t    tx_ka_time;


//  uint8_t         current_wait_count;
    uint8_t security_form;

   idk_edp_state_t edp_state;
   idk_layer_state_t layer_state;
   idk_status_t error_code;
   bool network_busy;
   bool edp_connected;

   idk_facility_t * active_facility;
   idk_facility_t * facility_list;

   struct {
        uint8_t buffer[IDK_MSG_MAX_PACKET_SIZE];
        uint8_t * ptr;
        size_t length;
        size_t total_length;
    } send_packet;

    struct {
        int index;
        uint8_t buffer[IDK_MSG_MAX_PACKET_SIZE];
        uint8_t * ptr;
        uint16_t    packet_type;
        uint16_t    packet_length;
        size_t length;
        size_t total_length;
        idk_packet_t * data_packet;
    } receive_packet;

} idk_data_t;



#ifdef __cplusplus
extern "C"
}
#endif

#endif /* IDK_DEF_H_ */
