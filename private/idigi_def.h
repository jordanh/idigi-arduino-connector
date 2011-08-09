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

#define URL_PREFIX  "en://"
#define GET_PACKET_DATA_POINTER(p, s)   (uint8_t *)(((uint8_t *)p) + (s))
#define IS_SEND_PENDING(idigi_ptr)      (idigi_ptr->edp_connected && idigi_ptr->send_packet.total_length > 0)

#define asizeof(array)  (sizeof array/sizeof array[0])

#define idigi_callback_no_response(callback, class_id, request_id, request_data, request_length) idigi_callback((callback), (class_id), (request_id), (request_data), (request_length), NULL, NULL)



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
        if (*head != NULL) \
        {\
            *head->prev = node;\
        }\
        node->next = *head;\
        node->prev = NULL;\
        *head = node;\
   } while (0)

#define del_node(head, node) \
    do { \
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
    uint16_t facility_num;                          /* facility opcode */
    size_t size;                                    /* size of facility data */
    idigi_facility_process_cb_t discovery_cb;       /* function for discovery layer that allows facility to send any initialization message */
    idigi_facility_process_cb_t process_cb;         /* function to process message received from server */
    uint8_t * packet;                               /* message packet data */
    void * facility_data;                           /* pointer to facility data */
    struct idigi_facility * next;                   /* next facility */
    struct idigi_facility * prev;                   /* prev facility */
} idigi_facility_t;

typedef struct idigi_buffer {
    uint8_t buffer[MSG_MAX_PACKET_SIZE];        /* buffer for message */
    bool    in_used;                            /* active or inactive */
    uint16_t facility;                          /* facility that own the buffer */
    struct idigi_buffer * next;                 /* next buffer */
} idigi_buffer_t;

typedef struct idigi_data {

    uint8_t * device_id;                        /* pointer to device id */
    uint8_t * vendor_id;                        /* pointer to vendor id */
    char * device_type;                         /* pointer to device type */
    uint16_t * tx_keepalive;                    /* pointer to Tx keepalive configuration */
    uint16_t * rx_keepalive;                    /* pointer to Rx keepalive configuration */
    uint8_t  * wait_count;                      /* wait count for keepalive messages */
    uint32_t rx_ka_time;                        /* time sent last Rx keepalive message */
    uint32_t tx_ka_time;                        /* time received last Tx keepalive message */

    idigi_facility_t * active_facility;         /* current active facility */
    idigi_facility_t * facility_list;           /* list of supported facilities */

    idigi_network_handle_t * network_handle;    /* handler for network class callbacks */

    idigi_callback_t callback;                  /* user's callback */

    idigi_active_state_t active_state;          /* active state of the idigi */
   idigi_edp_state_t edp_state;                 /* which layer that EDP is in */
   idigi_status_t error_code;                   /* error code */

   unsigned layer_state;                        /* layer state of each layer */
   int  request_id;                             /* request id to callback back */
   uint16_t facilities;                         /* bit mask of supported facilities */
   bool network_busy;                           /* busy network */
   bool edp_connected;                          /* whether connection was established */

   char server_url[SERVER_URL_LENGTH];                  /* server URL */
   uint8_t rx_keepalive_packet[PACKET_EDP_HEADER_SIZE]; /* buffer to send Rx keepalive message */
   struct {
        idigi_buffer_t packet_buffer;           /* buffer for send message to server */
        uint8_t * ptr;                          /* current pointer to message to be sent */
        size_t length;                          /* length has been sent */
        size_t total_length;                    /* total length of data that needs to be sent */
        send_complete_cb_t complete_cb;         /* completion callback */
        void * user_data;                       /* user data for completion callback */
    } send_packet;

    struct {
        idigi_buffer_t * free_packet_buffer;  /* pointer to a list of message that is available for receive data */
        idigi_buffer_t packet_buffer;         /* active packet buffer that is being used */
        uint8_t * data_packet;                  /* current data message */
        uint8_t * ptr;                          /* pointer for receive data */
        int index;                              /* index used for receiving a complete message from server */
        uint16_t  packet_type;                  /* packet type of a received message */
        uint16_t  packet_length;                /* packet length of a received message */
        size_t length;                          /* length has been received */
        size_t total_length;                    /* total length that needs to be received */
    } receive_packet;

} idigi_data_t;

#endif /* IDIGI_DEF_H_ */
