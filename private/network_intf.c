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


typedef enum {
    receive_packet_init,
    receive_packet_type,
    receive_packet_length,
    receive_packet_data,
    receive_packet_complete
} receive_packet_index_t;


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
    idk_ptr->layer_state = 0;
    idk_ptr->request_id = 0;
}


static idk_callback_status_t enable_send_packet(idk_data_t * idk_ptr, idk_packet_t * packet)
{
    idk_callback_status_t status = idk_callback_continue;
    uint16_t length;
    uint16_t msg_type;

    /* Setup send data to be sent. send_packet_process() will actually
     * send out the data.
     */

    if (idk_ptr->send_packet.total_length > 0)
    {
        DEBUG_PRINTF("enable_send_packet: unable to enable another send since previous data is still pending\n");
        status = idk_callback_busy;
        goto done;
    }

    idk_ptr->send_packet.total_length = packet->length;
    idk_ptr->send_packet.ptr = (uint8_t *)&packet->type;
    length = TO_BE16(packet->length);

    /*
     * MTv2 (and later)...
     * Packet format for MT version:
     *   | type | length | data |
     * It has a 2-octet type field and the 2-octet length field and followed by data.
     *
     */

    msg_type = TO_BE16(packet->type);

    packet->type = msg_type;
    packet->length = length;
    /* The total length must include the type and length fields */
    idk_ptr->send_packet.total_length += sizeof(packet->type) + sizeof(packet->length);
    /* clear the actual number of bytes to be sent */
    idk_ptr->send_packet.length = 0;

done:
    return status;
}

static idk_callback_status_t enable_facility_packet(idk_data_t * idk_ptr, uint16_t facility, uint8_t security_code)
{
    idk_facility_packet_t   * packet;

    packet = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
    /* this function is to set up a facility packet to be sent.
     *
     * facility packet:
     *    -----------------------------------------------------------------------------------------
     *   |      1 - 4         |          5           |         6         |  7 - 8   |   9...       |
     *    -----------------------------------------------------------------------------------------
     *   | MTv2 packet format | data security coding | discovery payload | facility | facility data|
     *    -----------------------------------------------------------------------------------------
     *   | type   |   length  |
     *    --------------------
     */

    packet->type = E_MSG_MT2_TYPE_PAYLOAD;

    packet->sec_coding = security_code;
    packet->disc_payload = DISC_OP_PAYLOAD;
    packet->facility = TO_BE16(facility);
    packet->length += sizeof(packet->sec_coding) + sizeof(packet->disc_payload) + sizeof(packet->facility);
    return enable_send_packet(idk_ptr, (idk_packet_t *)packet);
}

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
        goto done;
    }
    tx_keepalive = *idk_ptr->tx_keepalive;
    rx_keepalive = *idk_ptr->rx_keepalive;
    if (get_system_time(idk_ptr, &time_stamp) != idk_callback_continue)
    {
        bytes_sent = -idk_configuration_error;
        goto done;
    }

    /* rx keepalive timeout that we need to send a keepalive packet:
     *   = configured keepalive time  - (current time - last time we send a packet);
     *      
     * tx keepalive timeout that we expect a keepalive packet from server:
     *   = configured keepalive time  - (current time - last time we receive a packet);
     *
     * timeout value is in seconds.
     */
    write_data.timeout = (MIN_VALUE(((rx_keepalive * MILLISECONDS_PER_SECOND) - (time_stamp - idk_ptr->rx_ka_time)),
                                 ((tx_keepalive * MILLISECONDS_PER_SECOND) - (time_stamp - idk_ptr->tx_ka_time)))/
                                 MILLISECONDS_PER_SECOND);

    write_data.buffer = buffer;
    write_data.length = length;
    write_data.network_handle = idk_ptr->network_handle;

    size = sizeof length_written;
    request_id.base_request = idk_base_send;
    status = idk_callback(idk_ptr->callback, idk_class_base, request_id, &write_data, sizeof write_data, &length_written, &size);
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
        bytes_sent = -idk_send_error;
    }

done:
    return bytes_sent;
}

static idk_packet_t * get_packet_buffer(idk_data_t * idk_ptr, size_t packet_size, uint8_t ** buf)
{
    idk_packet_t * packet = NULL;
    uint8_t * ptr = NULL;

    /* Return an available packet pointer for caller to setup data to be sent to server.
     *
     * make sure send is not pending
     */
    if (idk_ptr->send_packet.total_length == 0)
    {
        packet = (idk_packet_t *)idk_ptr->send_packet.buffer;
        packet->avail_length = sizeof idk_ptr->send_packet.buffer - sizeof packet->avail_length;
        ptr = GET_PACKET_DATA_POINTER(packet, packet_size);
    }
    *buf = ptr;
    return packet;
}

static idk_callback_status_t rx_keepalive_process(idk_data_t * idk_ptr)
{
#define IDK_MTV2_VERSION            2

    idk_callback_status_t status = idk_callback_continue;
    idk_packet_t * packet;
    uint16_t rx_keepalive;

    if (idk_ptr->network_handle == NULL)
    {
        goto done;
    }

    /* Sends rx keepalive if keepalive timing is expired. */
    rx_keepalive = *idk_ptr->rx_keepalive;

    if (valid_interval_limit(idk_ptr, idk_ptr->rx_ka_time, (rx_keepalive * MILLISECONDS_PER_SECOND)))
    {
        /* not expired yet. no need to send rx keepalive */
        goto done;
    }

    if (idk_ptr->send_packet.total_length > 0)
    {
        /* time to send rx keepalive but send is still pending */
        goto done;
    }

    DEBUG_PRINTF("rx_keepalive_process: time to send Rx keepalive\n");

    packet = (idk_packet_t *)&idk_ptr->rx_keepalive_packet;
    packet->length = 0;
    packet->type = E_MSG_MT2_TYPE_KA_KEEPALIVE;
    status = enable_send_packet(idk_ptr, packet);

done:
    return status;
}

static idk_callback_status_t send_packet_process(idk_data_t * idk_ptr)
{
    uint8_t * buf;
    size_t  length;
    int bytes_sent;
    idk_callback_status_t status = idk_callback_continue;

    /* if nothing needs to be sent, check whether we need to send rx keepalive */
    if (idk_ptr->send_packet.total_length == 0 && idk_ptr->edp_state >= edp_discovery_layer)
    {
        rx_keepalive_process(idk_ptr);
    }

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
            idk_ptr->error_code = -bytes_sent;
            status = idk_callback_abort;
        }
    }

    if (status == idk_callback_continue && idk_ptr->send_packet.total_length > 0)
    {
        status = idk_callback_busy;
    }
    return status;

}


static int receive_data(idk_data_t * idk_ptr, uint8_t * buffer, size_t length)
{
    int bytes_received = 0;

    idk_callback_status_t status;
    idk_read_request_t read_data;
    unsigned tx_keepalive, tx_ka_time;
    unsigned rx_keepalive, rx_ka_time;
    uint8_t wait_count;
    uint32_t time_stamp;
    size_t  length_read, size;
    idk_request_t request_id;

    tx_keepalive = *idk_ptr->tx_keepalive * MILLISECONDS_PER_SECOND;
    rx_keepalive = *idk_ptr->rx_keepalive * MILLISECONDS_PER_SECOND;
    wait_count = *idk_ptr->wait_count;

    if (!idk_ptr->network_busy)
    {

        if (get_system_time(idk_ptr, &time_stamp) != idk_callback_continue)
        {
            bytes_received = -idk_configuration_error;
            goto done;
        }

        /* rx keepalive timeout that we need to send a keepalive packet:
         *   = configured keepalive time  - (current time - last time we send a packet);
         *
         * tx keepalive timeout that we expect a keepalive packet from server:
         *   = configured keepalive time  - (current time - last time we receive a packet);
         *
         * timeout value is in seconds.
         */
        tx_ka_time = time_stamp - idk_ptr->tx_ka_time;
        while (tx_ka_time >= tx_keepalive)
        {
            tx_ka_time -= tx_keepalive;
        }

        rx_ka_time = time_stamp - idk_ptr->rx_ka_time;
        while (rx_ka_time >= rx_keepalive)
        {
            rx_ka_time -= rx_keepalive;
        }
        read_data.timeout = (MIN_VALUE((rx_keepalive - tx_ka_time ),
                                     (tx_keepalive- rx_ka_time))/MILLISECONDS_PER_SECOND);

        read_data.network_handle = idk_ptr->network_handle;
        read_data.buffer = buffer;
        read_data.length = length;

        size = sizeof length_read;

        request_id.base_request = idk_base_receive;
        status = idk_callback(idk_ptr->callback, idk_class_base, request_id, &read_data, sizeof read_data, &length_read, &size);
        if (status == idk_callback_abort)
        {
            DEBUG_PRINTF("receive_data: callback returns abort\n");
            bytes_received = -idk_receive_error;
            goto done;
        }
        else if (status == idk_callback_busy)
        {
            goto done;
        }

        if (length_read > 0)
        {
            bytes_received = (int)length_read;
            /* Retain the "last (TX) message send" time. */
            if (get_system_time(idk_ptr, &idk_ptr->tx_ka_time) != idk_callback_continue)
            {
                bytes_received = -idk_configuration_error;
            }
            goto done;
        }
    }

    if (tx_keepalive > 0)
    {
        if (!valid_interval_limit(idk_ptr, idk_ptr->tx_ka_time, (tx_keepalive  * wait_count)))
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

done:
    return bytes_received;
}


static idk_callback_status_t receive_data_status(idk_data_t * idk_ptr)
{
    uint8_t * buf;
    size_t  length;
    idk_callback_status_t status = idk_callback_continue;
    int     read_length;

    if (idk_ptr->receive_packet.length < idk_ptr->receive_packet.total_length)
    {
        buf = idk_ptr->receive_packet.ptr + idk_ptr->receive_packet.length;
        length = idk_ptr->receive_packet.total_length - idk_ptr->receive_packet.length;

        read_length = receive_data(idk_ptr, buf, length);
        if (read_length > 0)
        {
            idk_ptr->receive_packet.length += read_length;
        }
        else if (read_length < 0)
        {
            idk_ptr->error_code = -read_length;
            status = idk_callback_abort;
            goto done;
        }
    }

    if (idk_ptr->receive_packet.total_length > 0 &&
        idk_ptr->receive_packet.length < idk_ptr->receive_packet.total_length)
    {
        status = idk_callback_busy;
    }
done:
    return status;
}


static idk_callback_status_t receive_packet(idk_data_t * idk_ptr, idk_packet_t ** packet)
{
    idk_callback_status_t status = idk_callback_continue;
    uint16_t type_val;
    idk_request_t request_id;

    *packet = NULL;

    if (idk_ptr->network_handle == NULL)
    {
        goto done;
    }

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
     * So we use index == receive_packet_init to initialize the packet before starting reading.
     * When index == receive_packet_type, set to receive message type. After message type is received,
     * we must check valid message type.
     * 
     * When index == receive_packet_length, set to receive message length.
     * When index == receive_packet_data, set to receive message data.
     * When index == receive_packet_complete, message data is completely received and
     * reset index = receive_packet_init and exit.
     *
     */

    while (idk_ptr->receive_packet.index <= receive_packet_complete)
    {
        if (idk_ptr->receive_packet.index != receive_packet_init)
        {
            status = receive_data_status(idk_ptr);
            if (status != idk_callback_continue)
            {  /* receive pending */
                goto done;
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
            idk_ptr->receive_packet.index++;
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
                    DEBUG_PRINTF("receive_packet: E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP 0x%x\n", (unsigned) type_val);
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

                    idk_ptr->receive_packet.total_length = idk_ptr->receive_packet.data_packet->length;

                    /*
                     * Read the actual message data bytes into the packet buffer.
                     */
                    idk_ptr->receive_packet.ptr = GET_PACKET_DATA_POINTER(idk_ptr->receive_packet.buffer, sizeof(idk_packet_t));
                    idk_ptr->receive_packet.length = 0;
                    idk_ptr->receive_packet.total_length = idk_ptr->receive_packet.data_packet->length;
                    idk_ptr->receive_packet.index++;

                    break;
                case E_MSG_MT2_TYPE_VERSION_OK:
                    break;
                case E_MSG_MT2_TYPE_VERSION_BAD:
                    DEBUG_PRINTF("receive_packet: E_MSG_MT2_TYPE_VERSION_BAD 0x%x\n", (unsigned) type_val);
                    break;
                case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
                    DEBUG_PRINTF("receive_packet: E_MSG_MT2_TYPE_SERVER_OVERLOAD 0x%x\n", (unsigned) type_val);
                    break;
                case E_MSG_MT2_TYPE_KA_KEEPALIVE:
                    break;
                case E_MSG_MT2_TYPE_PAYLOAD:
                    break;
                /* Unexpected/unknown MTv2 message types... */
                case E_MSG_MT2_TYPE_VERSION:
                case E_MSG_MT2_TYPE_KA_RX_INTERVAL:
                case E_MSG_MT2_TYPE_KA_TX_INTERVAL:
                case E_MSG_MT2_TYPE_KA_WAIT:
                default:
                    /* tell caller we have unexpected packet message .
                     * If callback tells us to continue, we continue reading packet data.
                     */
                    DEBUG_PRINTF("receive_packet: error type 0x%x\n", (unsigned) type_val);
                    request_id.base_request = idk_base_receive;
                    idk_ptr->error_code = idk_invalid_packet;
                    status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_invalid_packet);
                    if (status == idk_callback_abort)
                    {
                        idk_ptr->receive_packet.index = 0;
                        goto done;
                    }
            }

            if (type_val != E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP)
            {   /* set up to read message length */
                idk_ptr->receive_packet.ptr = (uint8_t *)&idk_ptr->receive_packet.packet_length;
                idk_ptr->receive_packet.length = 0;
                idk_ptr->receive_packet.total_length = sizeof idk_ptr->receive_packet.packet_length;
                idk_ptr->receive_packet.index++;
            }
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
                    DEBUG_PRINTF("idk_get_receive_packet: Invalid payload\n");
                    request_id.base_request = idk_base_receive;
                    idk_ptr->error_code = idk_invalid_payload_packet;
                    status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_invalid_payload_packet);
                    if (status == idk_callback_abort)
                    {
                        idk_ptr->receive_packet.index = 0;
                        goto done;
                    }
                }
            }
            /* set to read message data */
            idk_ptr->receive_packet.total_length = idk_ptr->receive_packet.data_packet->length;

            if (idk_ptr->receive_packet.data_packet->length == 0)
            {
                /* set to complete data since no data to be read. */
                idk_ptr->receive_packet.index = 4;
            }
            else 
            {
                /*
                 * Read the actual message data bytes into the packet buffer.
                 */
                idk_ptr->receive_packet.ptr = GET_PACKET_DATA_POINTER(idk_ptr->receive_packet.buffer, sizeof(idk_packet_t));
                idk_ptr->receive_packet.length = 0;
                idk_ptr->receive_packet.total_length = idk_ptr->receive_packet.data_packet->length;
                idk_ptr->receive_packet.index++;

            }

            break;
        case receive_packet_complete:
            idk_ptr->receive_packet.data_packet->type = idk_ptr->receive_packet.packet_type;
            idk_ptr->receive_packet.index = receive_packet_init;
            *packet = idk_ptr->receive_packet.data_packet;
            goto done;
        }
    }

done:
    return status;
}

static idk_callback_status_t connect_server(idk_data_t * idk_ptr, char * server_url, unsigned port)
{
    idk_callback_status_t status;
    idk_connect_request_t request;
    size_t length;
    idk_request_t request_id;

    request.host_name = server_url;
    request.port = port;

    request_id.base_request = idk_base_connect;
    status = idk_callback(idk_ptr->callback, idk_class_base, request_id, &request, sizeof request, &idk_ptr->network_handle, &length);
    if (status == idk_callback_continue)
    {
        if (idk_ptr->network_handle != NULL && length == sizeof(idk_network_handle_t))
        {
            idk_ptr->edp_connected = true;
            idk_ptr->network_busy = false;
        }
        else
        {
            idk_ptr->error_code = (idk_ptr->network_handle == NULL) ? idk_invalid_data : idk_invalid_data_size;
            request_id.base_request = idk_base_connect;
            status = notify_error_status(idk_ptr->callback, idk_class_base, request_id, idk_ptr->error_code);
        }
    }
    else if (status == idk_callback_abort)
    {
        idk_ptr->error_code = idk_connect_error;
    }
    return status;
}

static idk_callback_status_t close_server(idk_data_t * idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_request_t request_id;

    if (idk_ptr->network_handle != NULL)
    {
        request_id.base_request = idk_base_close;
        status = idk_callback(idk_ptr->callback, idk_class_base, request_id, idk_ptr->network_handle, sizeof(idk_network_handle_t *), NULL, NULL);
        if (status == idk_callback_continue)
        {
            idk_ptr->network_handle = NULL;
            idk_ptr->edp_connected = false;
            idk_ptr->send_packet.total_length = 0;
            idk_ptr->receive_packet.index = 0;
;
        }
        else if (status == idk_callback_abort)
        {
            DEBUG_PRINTF("close_server: close callback aborts\n");
            idk_ptr->network_handle = NULL;
            idk_ptr->error_code = idk_close_error;
            idk_ptr->send_packet.total_length = 0;
            idk_ptr->receive_packet.index = 0;
        }
    }
    return status;
}

