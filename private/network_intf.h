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
//#include <string.h>

//#include "idk_def.h"
//#include "bele.c"
//#include "ei_security.h"
#include "ei_discover.h"

#define IDK_MT_VERSION_1            1

/*
 * MT version 2 message type defines.
 * Refer to EDP specification rev. 14.2 for a description of MT version 2.
 */
#define E_MSG_MT2_TYPE_VERSION                  0x0004 /* C -> S */
#define E_MSG_MT2_TYPE_LEGACY_EDP_VERSION       0x0004 /* C -> S */
#define E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP      0x0001 /* C <- S */
#define E_MSG_MT2_TYPE_VERSION_OK               0x0010 /* C <- S */
#define E_MSG_MT2_TYPE_VERSION_BAD              0x0011 /* C <- S */
#define E_MSG_MT2_TYPE_SERVER_OVERLOAD          0x0012 /* C <- S */
#define E_MSG_MT2_TYPE_KA_RX_INTERVAL           0x0020 /* C -> S */
#define E_MSG_MT2_TYPE_KA_TX_INTERVAL           0x0021 /* C -> S */
#define E_MSG_MT2_TYPE_KA_WAIT                  0x0022 /* C -> S */
#define E_MSG_MT2_TYPE_KA_KEEPALIVE             0x0030 /* bi-directional */
#define E_MSG_MT2_TYPE_PAYLOAD                  0x0040 /* bi-directional */


enum {
    IDK_NETWORK_BUFFER_COMPLETE,
    IDK_NETWORK_BUFFER_PENDING,
    IDK_NETWORK_RECEIVED_MESSAGE
};

enum {
    receive_packet_init,
    receive_packet_type,
    receive_packet_length,
    receive_packet_data,
    receive_packet_complete
};


static bool valid_interval_limit(idk_data_t * idk_ptr, uint32_t start, uint32_t limit)
{
    uint32_t elapsed;
    bool rc = false;

    if (get_system_time(idk_ptr, &elapsed) == idk_callback_continue)
    {
        elapsed -= start;
        rc = (elapsed < limit);
    }

    return rc;
}

static void set_idk_state(idk_data_t * idk_ptr, int state)
{
    idk_ptr->edp_state = state;
    idk_ptr->layer_state = layer_init_state;
    idk_ptr->request_id = 0;
}

#if 0
stataic int idk_select(idk_data_t * idk_ptr, unsigned set, unsigned * actual_set)
{
    IrlNetworkSelect_t select_data;
    unsigned                    config_id;
    idk_callback_status_t                   status;
    int                         rc = IDK_SUCCESS;
    uint16_t                    rx_keepalive;
    uint16_t                    tx_keepalive;
    uint32_t                    time_stamp;

    if (idk_ptr->connection.socket_fd < 0)
    {
        goto _ret;
    }
    if (idk_ptr->edp_state > edp_discovery_layer)
    {
        if (idk_get_system_time(idk_ptr, &time_stamp) != IDK_SUCCESS)
        {
            status = IDK_STATUS_ERROR;
            goto _ret;
        }

        rx_keepalive = (GET_RX_KEEPALIVE(idk_ptr) * IDK_MILLISECONDS) - (time_stamp - idk_ptr->rx_ka_time);
        tx_keepalive = (GET_TX_KEEPALIVE(idk_ptr) * IDK_MILLISECONDS) - (time_stamp - idk_ptr->tx_ka_time);

        select_data.wait_time = IDK_MIN(rx_keepalive, tx_keepalive);

    }
    else
    {
        select_data.wait_time = 0;
        select_data.wait_time = 2;
    }
    select_data.select_set = set;
    select_data.actual_set = 0;
    select_data.socket_fd = idk_ptr->connection.socket_fd;


    config_id = IDK_CONFIG_SELECT;
    status = idk_get_config(idk_ptr, config_id, &select_data);
    if (status != idk_callback_continue && status != IDK_STATUS_BUSY)
    {
        rc = IDK_CONFIG_ERR;
    }

    *actual_set = select_data.actual_set;
_ret:
    return rc;
}
#endif

static int send_buffer(idk_data_t * idk_ptr, uint8_t * buffer, size_t length)
{
    int bytes_sent = 0;

    idk_callback_status_t status;
    idk_write_request_t write_data;
    uint16_t tx_keepalive;
    uint16_t rx_keepalive;
    uint32_t time_stamp;
    idk_request_t request_id;

    size_t length_written, size;


    if (idk_ptr->network_busy || idk_ptr->network_handle == NULL)
    {
        /* don't do any network activity */
        goto _ret;
    }
    tx_keepalive = *idk_ptr->tx_keepalive;
    rx_keepalive = *idk_ptr->rx_keepalive;
    if (get_system_time(idk_ptr, &time_stamp) != idk_callback_continue)
    {
        bytes_sent = -idk_configuration_error;
        goto _ret;
    }

    /* rx keepalive timeout that we need to send a keepalive packet:
     *   = configured keepalive time  - (current time - last time we send a packet);
     *      
     * tx keepalive timeout that we expect a keepalive packet from server:
     *   = configured keepalive time  - (current time - last time we receive a packet);
     *
     * timeout value is in seconds.
     */
    write_data.timeout = (IDK_MIN(((rx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->rx_ka_time)),
                                 ((tx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->tx_ka_time)))/
                                 IDK_MILLISECONDS);

    write_data.buffer = buffer;
    write_data.length = length;
    write_data.network_handle = idk_ptr->network_handle;
    printf("idk_send: timeout = %d\n", write_data.timeout);

    size = sizeof length_written;
    request_id.base_request = idk_base_send;
    status = idk_ptr->callback(idk_class_base, request_id, &write_data, sizeof write_data, &length_written, &size);
    if (status == idk_callback_continue && length_written > 0) 
    {
        bytes_sent = length_written;

        /* Retain the "last (RX) message send" time. */
        status = get_system_time(idk_ptr, &idk_ptr->rx_ka_time);
        if (status == idk_callback_abort)
        {
            bytes_sent = -idk_configuration_error;
        }
    }
    else if (status == idk_callback_abort)
    {
        bytes_sent = -idk_network_error;
    }

_ret:
    return bytes_sent;
}


static idk_status_t net_send_packet(idk_data_t * idk_ptr, int * sent_status)
{
    uint8_t * buf;
    size_t  length;
    int bytes_sent;
    idk_status_t rc = idk_success;

    *sent_status = IDK_NETWORK_BUFFER_COMPLETE;

    if (idk_ptr->send_packet.total_length > 0)
    {
        buf = idk_ptr->send_packet.ptr + idk_ptr->send_packet.length;
        length = idk_ptr->send_packet.total_length;

        bytes_sent = send_buffer(idk_ptr, buf, length);
        if (bytes_sent > 0)
        {
            idk_ptr->send_packet.total_length -= bytes_sent;
            idk_ptr->send_packet.length += bytes_sent;
        }
        else
        {
            rc = -bytes_sent;
        }
    }

    if (rc == idk_success && idk_ptr->send_packet.total_length > 0)
    {
        *sent_status = IDK_NETWORK_BUFFER_PENDING;
    }
    return rc;

}


static idk_status_t net_enable_send_packet(idk_data_t * idk_ptr)
{
    idk_callback_status_t status = idk_success;
    idk_packet_t * p;
    uint16_t length;
    uint16_t msg_type;

    if (idk_ptr->send_packet.total_length > 0)
    {
        DEBUG_PRINTF("net_enable_send_packet: unable to enable another send since previous data is still pending\n");
        status = idk_callback_busy;
        goto _ret;
    }

    p = (idk_packet_t *)idk_ptr->send_packet.buffer;
    /* Adjust the packet header fields. */

    idk_ptr->send_packet.total_length = p->length;
    idk_ptr->send_packet.ptr = (uint8_t *)p;
    length = TO_BE16(p->length);

    /*
     * MTv2 (and later)...
     * MT version 2 has a 2-octet type field before the 2-octet length.
     *   | length | type | data |
     */

    msg_type = TO_BE16(p->type);

    p->type = msg_type;
    p->length = length;
    idk_ptr->send_packet.total_length += sizeof(p->type) + sizeof(p->length);

    idk_ptr->send_packet.length = 0;

_ret:
    return status;
}

static idk_callback_status_t net_enable_facility_packet(idk_data_t * idk_ptr, uint16_t facility, uint8_t security_code)
{
    idk_facility_packet_t   * p;

    p = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;

    p->type = E_MSG_MT2_TYPE_PAYLOAD;

    p->sec_coding = security_code;
    p->disc_payload = DISC_OP_PAYLOAD;
    p->facility = TO_BE16(facility);
    p->length += sizeof(p->sec_coding) + sizeof(p->disc_payload) + sizeof(p->facility);
    return net_enable_send_packet(idk_ptr);
}

static idk_status_t net_send_rx_keepalive(idk_data_t * idk_ptr)
{
#define IDK_MTV2_VERSION            2

    idk_status_t rc = idk_success;
    idk_callback_status_t status;
    idk_packet_t * p;
    uint16_t rx_keepalive;
    uint8_t * ptr;
    idk_request_t request_id;
    uint32_t version;

//  rx_keepalive = (uint16_t *)idk_ptr->config.data[IDK_CONFIG_RX_KEEPALIVE];
//  wait_count = (uint8_t *)idk_ptr->config.data[IDK_CONFIG_WAIT_COUNT];
    rx_keepalive = *idk_ptr->rx_keepalive;

    if (valid_interval_limit(idk_ptr, idk_ptr->rx_ka_time, (rx_keepalive * IDK_MILLISECONDS)))
    {
        /* not expired yet. no need to send rx keepalive */
        rc = idk_success;
        goto _ret;
    }

    if (idk_ptr->send_packet.total_length > 0)
    {
        /* time to send rx keepalive but send is still pending */
        request_id.base_request = idk_base_send;
        status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_keepalive_error);
        if (status == idk_callback_abort)
        {
            rc = idk_keepalive_error;
        }
        goto _ret;
    }
#if 0

    rc = idk_get_system_time(idk_ptr, &cur_time);
    if (rc != IDK_SUCCESS) goto _ret;

    if ((cur_time - idk_ptr->rx_ka_time) < (idk_ptr->rx_keepalive * 1000 * idk_ptr->wait_count)) {
        /* not expired yet */
        rc = IDK_SUCCESS;
        goto _ret;
    }
#endif

    DEBUG_PRINTF("net_send_rx_keepalive: time to send Rx keepalive\n");

    p = (idk_packet_t *)idk_ptr->send_packet.buffer;
    ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_packet_t));

    p->length = 0;

    /*
     * MTv2 (and later)...
     * MT version 2 has a 2-octet type field before the 2-octet length.
     */
    p->length = 4;
    p->type = E_MSG_MT2_TYPE_KA_KEEPALIVE;
    version = TO_BE32(IDK_MTV2_VERSION);
    memcpy(ptr, &version, p->length);

    rc = net_enable_send_packet(idk_ptr);

_ret:
    return rc;
}


unsigned receive_timeout = 0;

static int receive_data(idk_data_t * idk_ptr, uint8_t * buffer, size_t length)
{
    int bytes_received = 0;

    idk_callback_status_t status;
    idk_read_request_t read_data;
    uint16_t tx_keepalive;
    uint16_t rx_keepalive;
    uint8_t wait_count;
    uint32_t time_stamp;
    size_t  length_read, size;
    idk_request_t request_id;

    tx_keepalive = *idk_ptr->tx_keepalive;
    rx_keepalive = *idk_ptr->rx_keepalive;
    wait_count = *idk_ptr->wait_count;

    if (idk_ptr->network_busy || idk_ptr->network_handle == NULL)
    {
        goto _ka_check;
    }

    if (get_system_time(idk_ptr, &time_stamp) != idk_callback_continue)
    {
        bytes_received = -1;
        goto _ret;
    }

    /* rx keepalive timeout that we need to send a keepalive packet:
     *   = configured keepalive time  - (current time - last time we send a packet);
     *      
     * tx keepalive timeout that we expect a keepalive packet from server:
     *   = configured keepalive time  - (current time - last time we receive a packet);
     *
     * timeout value is in seconds.
     */
    read_data.timeout = (IDK_MIN(((rx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->rx_ka_time)),
                                 ((tx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->tx_ka_time)))/
                                 IDK_MILLISECONDS);

    if (receive_timeout != read_data.timeout)
    {
//      printf("idk_receive: timeout = %d count %d\n", read_data.timeout, receive_count);
        receive_timeout = read_data.timeout;
    }

    read_data.network_handle = idk_ptr->network_handle;
    read_data.buffer = buffer;
    read_data.length = length;

    size = sizeof length_read;

    request_id.base_request = idk_base_receive;
    status = idk_ptr->callback(idk_class_base, request_id, &read_data, sizeof read_data, &length_read, &size);
    if (status != idk_callback_abort)
    {
        if (status == idk_callback_continue && length_read > 0)
        {
            bytes_received = (int)length_read;
            /* Retain the "last (TX) message send" time. */
            if (get_system_time(idk_ptr, &idk_ptr->tx_ka_time) != idk_callback_continue)
            {
                bytes_received = -idk_configuration_error;
                goto _ret;
            }

        }
        else
        {
_ka_check:
            bytes_received = 0;
            if (tx_keepalive > 0)
            {
                if (!valid_interval_limit(idk_ptr, idk_ptr->tx_ka_time, (tx_keepalive * IDK_MILLISECONDS * wait_count)))
                {
                    /*
                     * We haven't received a message
                     * of any kind for the configured maximum interval, so we must
                     * mark this connection in error and return that status.
                     *
                     * Note: this inactivity check applies only for MTv2 and later.
                     * For MTv1, the client sends keep-alives, but the server does
                     * not send them (nor must the client expect them). For MTv1,
                     * the data member tx_keepalive_ms is always 0 (zero), so this
                     * keep-alive failure check never triggers.
                     *
                     */
                    bytes_received = -idk_keepalive_error;
                    request_id.base_request = idk_base_receive;
                    notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_keepalive_error);
                    DEBUG_PRINTF("idk_receive: keepalive fail\n");
                }
            }
        }
    }
    else
    {
        bytes_received = -idk_network_error;
    }
_ret:
    return bytes_received;
}


static idk_status_t receive_data_status(idk_data_t * idk_ptr, int * receive_status)
{
    uint8_t * buf;
    size_t  length;
    idk_status_t rc = idk_success;
    int     read_length;

    *receive_status = IDK_NETWORK_BUFFER_COMPLETE;
    if (idk_ptr->receive_packet.length < idk_ptr->receive_packet.total_length)
    {
        buf = idk_ptr->receive_packet.ptr + idk_ptr->receive_packet.length;
        length = idk_ptr->receive_packet.total_length - idk_ptr->receive_packet.length;

        read_length = receive_data(idk_ptr, buf, length);
        if (read_length > 0)
        {
            DEBUG_PRINTF("idk_receive_status: read_length = %d\n", read_length);
            idk_ptr->receive_packet.length += read_length;
        }
        else if (read_length < 0)
        {
            rc = -read_length;
        }
    }

    if (idk_ptr->receive_packet.total_length > 0)
    {
        if (idk_ptr->receive_packet.length < idk_ptr->receive_packet.total_length)
        {
            *receive_status = IDK_NETWORK_BUFFER_PENDING;
        }
        else if (idk_ptr->receive_packet.length > idk_ptr->receive_packet.total_length)
        {
            /* something's wrong */
            DEBUG_PRINTF("get_receive_packet_status: length receive > request length !!!\n");
        }

    }
//  DEBUG_PRINTF("idk_receive_status: length = %d total_length %d status %d\n", idk_ptr->receive_packet.length,
//          idk_ptr->receive_packet.total_length, *receive_status);
    return rc;
}


static idk_status_t net_get_receive_packet(idk_data_t * idk_ptr, idk_packet_t ** p)
{
    idk_status_t rc = idk_success;
    uint16_t type_val;
    int receive_status;
    idk_request_t request_id;

    *p = NULL;
    /*
     * Read the MT message type.
     *
     * For MT version 2, there are numerous message types. Some of these
     * messages require special handling, in that they may be legacy EDP
     * version response message varieties. These messages are sent by
     * servers that do not support MTv2. Since the client doesn't support
     * both MTv1 and MTv2 concurrently, an MTv2 client must terminate its
     * MT connection if it finds the server to be incompatible insofar as
     * the MT version is concerned.
     *
     * We only accept messages of the expected types from the server. Any
     * message other than an expected type is handled as an error, and an
     * error is returned to the caller. This must be done since any unknown
     * message type cannot be correctly parsed for length and discarded from
     * the input stream.
     */

    /* we have to read 3 times to get a complete packet.
     * 1. read message type
     * 2. read message length
     * 3. actual message data
     *
     * So we use index = 0 to initialize the packet before starting reading.
     * we must check whether we need to complete the read.
     * when index == 1, set to receive message type. After message type is received,
     * we must check valid message type.
     * 
     * when index == 2, set to receive message length.
     * when index == 3, set to receive message data.
     * when index == 4, message data is completely received and reset index = 0.
     *
     */

    while (idk_ptr->receive_packet.index <= receive_packet_complete)
    {
        if (idk_ptr->receive_packet.index != receive_packet_init)
        {
            rc = receive_data_status(idk_ptr, &receive_status);
            if (rc != idk_success || receive_status == IDK_NETWORK_BUFFER_PENDING)
            {
                goto _ret;
            }
        }

        switch (idk_ptr->receive_packet.index)
        {
        case receive_packet_init:
            idk_ptr->receive_packet.packet_type = 0;
            idk_ptr->receive_packet.packet_length = 0;
            idk_ptr->receive_packet.length = 0;
            idk_ptr->receive_packet.total_length = 0;
            idk_ptr->receive_packet.index = 0;
            idk_ptr->receive_packet.data_packet = (idk_packet_t *)idk_ptr->receive_packet.buffer;
            idk_ptr->receive_packet.index++;
            break;

        case receive_packet_type:
            /* set to read the type of the message */
            idk_ptr->receive_packet.ptr = (uint8_t *)&idk_ptr->receive_packet.packet_type;
            idk_ptr->receive_packet.length = 0;
            idk_ptr->receive_packet.total_length = sizeof idk_ptr->receive_packet.packet_type;

    #if 0
            rc = receive_data(idk_ptr, idk_ptr->network_handle,
                             idk_ptr->receive_packet.ptr,
                             idk_ptr->receive_packet.total_length);
            if (rc < 0)
            {
                goto _ret;
            }
    #endif
            idk_ptr->receive_packet.index++;
            idk_ptr->receive_packet.length += rc;
            break;
        case receive_packet_length:
            /* Got message type let's get to message length
             * so make sure we support the message type.
             * Then, set to read message length. 
             */
            type_val = FROM_BE16(idk_ptr->receive_packet.packet_type);
            idk_ptr->receive_packet.packet_type = type_val;

            switch (type_val)
            {
                /* Expected MTv2 message types... */
                case E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP:
                    DEBUG_PRINTF("net_get_receive_packet: E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP 0x%x\n", (unsigned) type_val);
                    /*
                     * Obtain the MT message length (2 bytes).
                     * Note that legacy EDP version response messages do not have a length
                     * field. There is just a single byte of data remaining in the stream
                     * for this MT message type, so we provide a dummy length value of 1 in
                     * this case. All other MT message types do have a length field, which
                     * we will validate according to message type after we read the length
                     * field bytes.
                     */
                    /* Supply a length of 1 byte. */
                    idk_ptr->receive_packet.data_packet->length = 1;
                    idk_ptr->receive_packet.index++;
                    goto common_recv;
                    break;
                case E_MSG_MT2_TYPE_VERSION_OK:
                    DEBUG_PRINTF("net_get_receive_packet: E_MSG_MT2_TYPE_VERSION_OK 0x%x\n", (unsigned) type_val);
                    break;
                case E_MSG_MT2_TYPE_VERSION_BAD:
                    DEBUG_PRINTF("net_get_receive_packet: E_MSG_MT2_TYPE_VERSION_BAD 0x%x\n", (unsigned) type_val);
                    break;
                case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
                    DEBUG_PRINTF("net_get_receive_packet: E_MSG_MT2_TYPE_SERVER_OVERLOAD 0x%x\n", (unsigned) type_val);
                    break;
                case E_MSG_MT2_TYPE_KA_KEEPALIVE:
                    DEBUG_PRINTF("net_get_receive_packet: E_MSG_MT2_TYPE_KA_KEEPALIVE 0x%x\n", (unsigned) type_val);
                    break;
                case E_MSG_MT2_TYPE_PAYLOAD:
                    DEBUG_PRINTF("net_get_receive_packet: E_MSG_MT2_TYPE_PAYLOAD 0x%x\n", (unsigned) type_val);
                    break;
                /* Unexpected/unknown MTv2 message types... */
                case E_MSG_MT2_TYPE_VERSION:
            //  case E_MSG_MT2_TYPE_LEGACY_EDP_VERSION: // same as E_MSG_MT2_TYPE_VERSION
                case E_MSG_MT2_TYPE_KA_RX_INTERVAL:
                case E_MSG_MT2_TYPE_KA_TX_INTERVAL:
                case E_MSG_MT2_TYPE_KA_WAIT:
                default:
                    /* tell caller we have unexpected packet message .
                     * If callback tells us to continue, we continue reading packet data.
                     */
                    DEBUG_PRINTF("net_get_receive_packet: error type 0x%x\n", (unsigned) type_val);
                    request_id.base_request = idk_base_receive;
                    if (notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_invalid_packet) == idk_callback_abort)
                    {
                        rc =  idk_invalid_packet;
                        idk_ptr->receive_packet.index = 0;
                        goto _ret;
                    }
            }


            idk_ptr->receive_packet.ptr = (uint8_t *)&idk_ptr->receive_packet.packet_length;
            idk_ptr->receive_packet.length = 0;
            idk_ptr->receive_packet.total_length = sizeof idk_ptr->receive_packet.packet_length;

    #if 0
            rc = idk_receive(idk_ptr, idk_ptr->network_handle,
                             idk_ptr->receive_packet.ptr,
                             idk_ptr->receive_packet.total_length);
            if (rc < 0)
            {
                goto _ret;
            }
    #endif
            idk_ptr->receive_packet.length += rc;
            idk_ptr->receive_packet.index++;
            break;

        case receive_packet_data:
            /* got packet length so set to read message data */
            idk_ptr->receive_packet.data_packet->length = FROM_BE16(idk_ptr->receive_packet.packet_length);
            idk_ptr->receive_packet.packet_length = idk_ptr->receive_packet.data_packet->length;
            if (idk_ptr->receive_packet.packet_type != E_MSG_MT2_TYPE_PAYLOAD)
            {
                /*
                 * For all but payload messages, the length field value should be
                 * zero, as there is no data accompanying the message. The MT
                 * messages to which this applies here are:
                 *    E_MSG_MT2_TYPE_VERSION_OK
                 *    E_MSG_MT2_TYPE_VERSION_BAD
                 *    E_MSG_MT2_TYPE_SERVER_OVERLOAD
                 *    E_MSG_MT2_TYPE_KA_KEEPALIVE
                 */
                if (idk_ptr->receive_packet.packet_length != 0)
                {
                    request_id.base_request = idk_base_receive;
                    if (notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_invalid_payload_packet) == idk_callback_abort)
                    {
                        rc = idk_invalid_payload_packet;
                        idk_ptr->receive_packet.index = 0;
                        goto _ret;
                    }
                }
            }
            DEBUG_PRINTF("net_get_receive_packet: receive length field %d\n", idk_ptr->receive_packet.data_packet->length);



    common_recv:
            /* set to read message data */
            idk_ptr->receive_packet.total_length = idk_ptr->receive_packet.data_packet->length;

            if (idk_ptr->receive_packet.data_packet->length == 0)
            {
                /* set to complete data since no data to be read. */
                idk_ptr->receive_packet.index = 4;
            }

            /*
             * Read the actual message data bytes into the packet buffer.
             */
            else 
            {

                idk_ptr->receive_packet.ptr = IDK_PACKET_DATA_POINTER(idk_ptr->receive_packet.buffer, sizeof(idk_packet_t));
                idk_ptr->receive_packet.length = 0;
                idk_ptr->receive_packet.total_length = idk_ptr->receive_packet.data_packet->length;

    #if 0
                rc = receive_data(idk_ptr, idk_ptr->network_handle,
                                 idk_ptr->receive_packet.ptr,
                                 idk_ptr->receive_packet.total_length);
                if (rc < 0)
                {
                    goto _ret;
                }
    #endif
                idk_ptr->receive_packet.index++;
                idk_ptr->receive_packet.length += rc;

            }

            break;
        case receive_packet_complete:
            idk_ptr->receive_packet.data_packet->type = idk_ptr->receive_packet.packet_type;
            idk_ptr->receive_packet.index = 0;
            *p = idk_ptr->receive_packet.data_packet;
            goto _ret;
        }
    }

_ret:
    return rc;
}

static idk_callback_status_t net_connect_server(idk_data_t * idk_ptr, char * server_url, unsigned port)
{
    idk_callback_status_t status;
    idk_connect_request_t request;
    size_t length;
    idk_request_t request_id;

    request.host_name = server_url;
    request.port = port;

    request_id.base_request = idk_base_connect;
    status = idk_ptr->callback(idk_class_base, request_id, &request, sizeof request, &idk_ptr->network_handle, &length);
    if (status == idk_callback_continue)
    {
        if (idk_ptr->network_handle == NULL || length != sizeof(idk_network_handle_t))
        {
            request_id.base_request = idk_base_connect;
            status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_invalid_data_size);
        }
        else
        {
            idk_ptr->network_busy = false;
        }
    }
    return status;
}

static idk_callback_status_t net_close(idk_data_t * idk_ptr)
{
    idk_callback_status_t   status;
    idk_request_t request_id;

    if (idk_ptr->network_handle != NULL)
    {
        request_id.base_request = idk_base_close;
        status = idk_ptr->callback(idk_class_base, request_id, idk_ptr->network_handle, sizeof(idk_network_handle_t *), NULL, NULL);
        if (status == idk_callback_continue)
        {
            DEBUG_PRINTF("net_close: close 0x%x\n", (unsigned)idk_ptr->network_handle);
            idk_ptr->network_handle = NULL;
        }
    }
    return status;
}

