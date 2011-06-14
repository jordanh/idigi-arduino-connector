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

#ifndef IDIGI_DEF_H_
#define IDIGI_DEF_H_

#include "ei_packet.h"

#define ON_FALSE_DO_(cond, code)        do { if (!(cond)) {code;} } while (0)

#if defined(DEBUG)
#define ON_ASSERT_DO_(cond, code, output)   ON_FALSE_DO_((cond), {ASSERT(cond); code;})

#else
#define ON_ASSERT_DO_(cond, code, output)   ON_FALSE_DO_((cond), {code})
#endif

#define ASSERT_GOTO(cond, label)    ON_ASSERT_DO_((cond), {goto label;}, {})

#define UNUSED_PARAMETER(x)     ((void)x)

#define EDP_MT_VERSION      2
#define MSG_MAX_PACKET_SIZE 1600

#define DEVICE_TYPE_LENGTH  32
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define SERVER_URL_LENGTH   255
#define MAC_ADDR_LENGTH     6

/* these are limits for Tx and Rx keepalive
 * interval in seconds.
 */
#define MIN_RX_KEEPALIVE_INTERVAL_PER_SECOND     5
#define MAX_RX_KEEPALIVE_INTERVAL_PER_SECOND     7200
#define MIN_TX_KEEPALIVE_INTERVAL_PER_SECOND     5
#define MAX_TX_KEEPALIVE_INTERVAL_PER_SECOND     7200
/* Limits for wait count (number of
 * keepalive packets)
 */
#define WAIT_COUNT_MIN      2
#define WAIT_COUNT_MAX      64

#define MIN_VALUE(x,y)        (((x) < (y))? (x): (y))
#define MAX_VALUE(x,y)        (((x) > (y))? (x): (y))
#define MILLISECONDS_PER_SECOND            1000

#define URL_PREFIX  "en://"
#define GET_PACKET_DATA_POINTER(p, s)  (uint8_t *)(((uint8_t *)p) + (s))
#define IS_SEND_PENDING(idigi_ptr)       (idigi_ptr->edp_connected && idigi_ptr->send_packet.total_length > 0)
#define GET_FACILITY_POINTER(fac_ptr)   (fac_ptr->facility_data)
#define GET_FACILITY_PACKET(fac_ptr)    (fac_ptr->packet)
#define DONE_FACILITY_PACKET(fac_ptr)   (fac_ptr->packet = NULL)

#define asizeof(array)  (sizeof array/sizeof array[0])

/* IRL EDP States */
typedef enum {
    edp_init_layer,
    edp_communication_layer,
    edp_initialization_layer,
    edp_security_layer,
    edp_discovery_layer,
    edp_facility_layer
} idigi_edp_state_t;


typedef enum {
    idigi_device_started,
    idigi_device_stop,
    idigi_device_terminate
} idigi_active_state_t;

struct idigi_data;
struct idigi_facility;

typedef idigi_callback_status_t (* idigi_facility_process_cb_t )(struct idigi_data * idigi_ptr, void * facility_data, idigi_packet_t * packet);
typedef void (* send_complete_cb_t)(struct idigi_data * idigi_ptr, idigi_packet_t * packet, idigi_status_t  status);

typedef struct idigi_facility {
    uint16_t facility_num;
    size_t size;
    idigi_facility_process_cb_t discovery_cb;
    idigi_facility_process_cb_t process_cb;
    idigi_packet_t * packet;
    struct idigi_facility * next;
    void * facility_data;
} idigi_facility_t;

typedef struct idigi_buffer {
    uint8_t buffer[MSG_MAX_PACKET_SIZE];
    bool    in_used;
    uint16_t facility;
    struct idigi_buffer * next;
} idigi_buffer_t;

typedef struct idigi_data {
    idigi_active_state_t active_state;

    idigi_callback_t callback;

    uint8_t * device_id;
    uint8_t * vendor_id;
    char * device_type;
    int  request_id;
    char server_url[SERVER_URL_LENGTH];

    uint16_t facilities;
    idigi_network_handle_t * network_handle;

    uint16_t    * tx_keepalive;
    uint16_t    * rx_keepalive;
    uint8_t  * wait_count;
    uint32_t    rx_ka_time;
    uint32_t    tx_ka_time;

   idigi_edp_state_t edp_state;
   unsigned layer_state;
   idigi_status_t error_code;
   bool network_busy;
   bool edp_connected;

   idigi_facility_t * active_facility;
   idigi_facility_t * facility_list;

   idigi_packet_t     rx_keepalive_packet;
   struct {
        idigi_buffer_t packet_buffer;
        uint8_t * ptr;
        size_t length;
        size_t total_length;
        send_complete_cb_t complete_cb;
    } send_packet;

    struct {
        idigi_buffer_t   * free_packet_buffer;
        idigi_buffer_t   packet_buffer;
        int index;
        uint8_t * ptr;
        uint16_t    packet_type;
        uint16_t    packet_length;
        size_t length;
        size_t total_length;
        idigi_packet_t * data_packet;
    } receive_packet;

} idigi_data_t;

#endif /* IDIGI_DEF_H_ */
