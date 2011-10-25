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

#define MSG_MAX_RECV_PACKET_SIZE 1600
#define MSG_MAX_SEND_PACKET_SIZE 512

#define ON_FALSE_DO_(cond, code)        do { if (!(cond)) {code;} } while (0)

#if defined(IDIGI_DEBUG)
#define ON_ASSERT_DO_(cond, code, output)   ON_FALSE_DO_((cond), {ASSERT(cond); code;})
#else
#define ON_ASSERT_DO_(cond, code, output)   ON_FALSE_DO_((cond), {code})
#endif

#define ASSERT_GOTO(cond, label)    ON_ASSERT_DO_((cond), {goto label;}, {})

#define UNUSED_PARAMETER(x)     ((void)x)

#define EDP_MT_VERSION      2

#define DEVICE_TYPE_LENGTH  32
#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define SERVER_URL_LENGTH   64
#define MAC_ADDR_LENGTH     6

/* these are limits for Tx and Rx keepalive
 * interval in seconds.
 */
#define MIN_TX_KEEPALIVE_INTERVAL_IN_SECONDS     5
#define MAX_TX_KEEPALIVE_INTERVAL_IN_SECONDS     7200
#define MIN_RX_KEEPALIVE_INTERVAL_IN_SECONDS     5
#define MAX_RX_KEEPALIVE_INTERVAL_IN_SECONDS     7200
/* Limits for wait count (number of
 * keepalive packets)
 */
#define WAIT_COUNT_MIN      2
#define WAIT_COUNT_MAX      64

#define MIN_VALUE(x,y)        (((x) < (y))? (x): (y))
#define MAX_VALUE(x,y)        (((x) > (y))? (x): (y))

#define URL_PREFIX  "en://"
#define GET_PACKET_DATA_POINTER(p, s)   (uint8_t *)(((uint8_t *)p) + (s))
#define IS_SEND_PENDING(idigi_ptr)      (idigi_ptr->network_connected && idigi_ptr->send_packet.total_length > 0)

#define asizeof(array)  (sizeof array/sizeof array[0])

#define idigi_callback_no_response(callback, class_id, request_id, request_data, request_length) idigi_callback((callback), (class_id), (request_id), (request_data), (request_length), NULL, NULL)
#define idigi_callback_no_request_data(callback, class_id, request_id, response_data, response_length) idigi_callback((callback), (class_id), (request_id), NULL, 0, (response_data), (response_length))

#define MutexLock(idigi_ptr, lock_handle) mutex_data(idigi_ptr, lock_handle, idigi_os_lock)
#define MutexUnlock(idigi_ptr, lock_handle) mutex_data(idigi_ptr, lock_handle, idigi_os_unlock)
#define MutexLockDelete(idigi_ptr, lock_handle) mutex_data(idigi_ptr, lock_handle, idigi_os_lock_delete)


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


#define add_node(head, node) \
    do { \
        ASSERT(node != NULL); \
        ASSERT(head != NULL); \
        if (*head != NULL) \
        {\
            *head->prev = node;\
        }\
        node->next = *head;\
        node->prev = NULL;\
        *head = node;\
   } while (0)

#define remove_node(head, node) \
    do { \
        ASSERT(node != NULL); \
        ASSERT(head != NULL); \
        if (node->next != NULL) \
        {\
            node->next->prev = node->prev;\
        }\
        if (node->prev != NULL) \
        {\
            node->prev->next = node->next;\
        }\
        if (node == *head)\
        {\
            *head = node->next;\
        }\
    } while (0)

struct idigi_data;
struct idigi_facility;

typedef idigi_callback_status_t (* idigi_facility_process_cb_t )(struct idigi_data * const idigi_ptr, void * const facility_data, uint8_t * const packet);
typedef void (* send_complete_cb_t)(struct idigi_data * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data);

typedef struct idigi_facility {
    uint16_t facility_num;
    size_t size;
    idigi_facility_process_cb_t discovery_cb;
    idigi_facility_process_cb_t process_cb;
    uint8_t * packet;
    void * facility_data;
    struct idigi_facility * next;
    struct idigi_facility * prev;
} idigi_facility_t;

typedef struct idigi_buffer {
    uint8_t buffer[MSG_MAX_RECV_PACKET_SIZE];
    struct idigi_buffer * next;
    bool    in_use;
} idigi_buffer_t;

typedef struct idigi_data {

    uint8_t * device_id;
    uint8_t * vendor_id;
    char * device_type;
    size_t device_type_length;
    uint16_t tx_keepalive_interval;
    uint16_t rx_keepalive_interval;
    uint16_t wait_count;
    uint32_t last_rx_keepalive_sent_time;
    uint32_t last_tx_keepalive_received_time;

    idigi_facility_t * active_facility;
    idigi_facility_t * facility_list;

    idigi_network_handle_t network_handle;

    idigi_callback_t callback;

    idigi_active_state_t active_state;
    idigi_edp_state_t edp_state;
    idigi_status_t error_code;

    unsigned layer_state;
    unsigned request_id;
    uint16_t facilities;
    bool network_connected;
    bool network_busy;
    bool edp_connected;

    char server_url[SERVER_URL_LENGTH];
    size_t server_url_length;
    uint8_t rx_keepalive_packet[PACKET_EDP_HEADER_SIZE];
    struct {
        struct {
            uint8_t buffer[MSG_MAX_SEND_PACKET_SIZE];
            bool in_use;
        } packet_buffer;
        uint8_t * ptr;
        size_t bytes_sent;
        size_t total_length;
        send_complete_cb_t complete_cb;
        void * user_data;
    } send_packet;

    struct {
        idigi_buffer_t * free_packet_buffer;
        idigi_buffer_t packet_buffer;
        uint8_t * data_packet;
        uint8_t * ptr;
        int index;
        uint16_t  packet_type;
        uint16_t  packet_length;
        size_t bytes_received;
        size_t total_length;
    } receive_packet;

} idigi_data_t;

#endif /* IDIGI_DEF_H_ */
