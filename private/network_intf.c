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


static bool valid_interval_limit(iik_data_t * iik_ptr, uint32_t start, uint32_t limit)
{
    uint32_t elapsed;
    bool rc = false;

    if (get_system_time(iik_ptr, &elapsed) == iik_callback_continue)
    {
        elapsed -= start;
        rc = (elapsed < limit);
    }

    return rc;
}

static void set_iik_state(iik_data_t * iik_ptr, int state)
{
    iik_ptr->edp_state = state;
    iik_ptr->layer_state = 0;
    iik_ptr->request_id = 0;
}


static iik_callback_status_t enable_send_packet(iik_data_t * iik_ptr, iik_packet_t * packet)
{
    iik_callback_status_t status = iik_callback_continue;
    uint16_t length;
    uint16_t msg_type;

    /* Setup send data to be sent. send_packet_process() will actually
     * send out the data.
     */

    if (iik_ptr->send_packet.total_length > 0)
    {
        DEBUG_PRINTF("enable_send_packet: unable to enable another send since previous data is still pending\n");
        status = iik_callback_busy;
        goto done;
    }

    iik_ptr->send_packet.total_length = packet->length;
    iik_ptr->send_packet.ptr = (uint8_t *)&packet->type;
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
    iik_ptr->send_packet.total_length += sizeof(packet->type) + sizeof(packet->length);
    /* clear the actual number of bytes to be sent */
    iik_ptr->send_packet.length = 0;

done:
    return status;
}

static iik_callback_status_t enable_facility_packet(iik_data_t * iik_ptr, uint16_t facility, uint8_t security_code)
{
    iik_facility_packet_t   * packet;

    packet = (iik_facility_packet_t *)iik_ptr->send_packet.buffer;
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
    return enable_send_packet(iik_ptr, (iik_packet_t *)packet);
}

static int send_buffer(iik_data_t * iik_ptr, uint8_t * buffer, size_t length)
{
    int bytes_sent = 0;

    iik_callback_status_t status;
    iik_write_request_t write_data;
    unsigned tx_ka_timeout;
    unsigned rx_ka_timeout;
    uint32_t time_stamp;
    iik_request_t request_id;

    size_t length_written, size;


    if (iik_ptr->network_busy || iik_ptr->network_handle == NULL)
    {
        /* don't do any network activity */
        goto done;
    }

    if (get_system_time(iik_ptr, &time_stamp) != iik_callback_continue)
    {
        bytes_sent = -iik_configuration_error;
        goto done;
    }

    /* Get rx keepalive timeout that we need to send a keepalive packet:
     *   = configured keepalive time  - (current time - last time we sent a packet);
     *      
     * Get tx keepalive timeout that we expect a keepalive packet from server:
     *   = configured keepalive time  - (current time - last time we received a packet);
     *
     * timeout value is in seconds.
     */
    rx_ka_timeout = get_timeout_limit_in_seconds(*iik_ptr->rx_keepalive, (time_stamp - iik_ptr->rx_ka_time));
    tx_ka_timeout = get_timeout_limit_in_seconds(*iik_ptr->tx_keepalive, (time_stamp - iik_ptr->tx_ka_time));

    write_data.timeout = MIN_VALUE(rx_ka_timeout, tx_ka_timeout);

    write_data.buffer = buffer;
    write_data.length = length;
    write_data.network_handle = iik_ptr->network_handle;

    size = sizeof length_written;
    request_id.network_request = iik_network_send;
    status = iik_callback(iik_ptr->callback, iik_class_network, request_id, &write_data, sizeof write_data, &length_written, &size);
    if (status == iik_callback_continue && length_written > 0) 
    {
        bytes_sent = length_written;

        /* Retain the "last (RX) message send" time. */
        status = get_system_time(iik_ptr, &iik_ptr->rx_ka_time);
        if (status == iik_callback_abort)
        {
            bytes_sent = -iik_configuration_error;
        }
    }
    else if (status == iik_callback_abort)
    {
        bytes_sent = -iik_send_error;
    }

done:
    return bytes_sent;
}

static iik_packet_t * get_packet_buffer(iik_data_t * iik_ptr, size_t packet_size, uint8_t ** buf)
{
    iik_packet_t * packet = NULL;
    uint8_t * ptr = NULL;

    /* Return an available packet pointer for caller to setup data to be sent to server.
     *
     * make sure send is not pending
     */
    if (iik_ptr->send_packet.total_length == 0)
    {
        packet = (iik_packet_t *)iik_ptr->send_packet.buffer;
        packet->avail_length = sizeof iik_ptr->send_packet.buffer - sizeof packet->avail_length;
        ptr = GET_PACKET_DATA_POINTER(packet, packet_size);
    }
    *buf = ptr;
    return packet;
}

static iik_callback_status_t rx_keepalive_process(iik_data_t * iik_ptr)
{
#define IIK_MTV2_VERSION            2

    iik_callback_status_t status = iik_callback_continue;
    iik_packet_t * packet;
    uint16_t rx_keepalive;

    if (iik_ptr->network_handle == NULL)
    {
        goto done;
    }

    /* Sends rx keepalive if keepalive timing is expired. */
    rx_keepalive = *iik_ptr->rx_keepalive;

    if (valid_interval_limit(iik_ptr, iik_ptr->rx_ka_time, (rx_keepalive * MILLISECONDS_PER_SECOND)))
    {
        /* not expired yet. no need to send rx keepalive */
        goto done;
    }

    if (iik_ptr->send_packet.total_length > 0)
    {
        /* time to send rx keepalive but send is still pending */
        goto done;
    }

    DEBUG_PRINTF("rx_keepalive_process: time to send Rx keepalive\n");

    packet = (iik_packet_t *)&iik_ptr->rx_keepalive_packet;
    packet->length = 0;
    packet->type = E_MSG_MT2_TYPE_KA_KEEPALIVE;
    status = enable_send_packet(iik_ptr, packet);

done:
    return status;
}

static iik_callback_status_t send_packet_process(iik_data_t * iik_ptr)
{
    uint8_t * buf;
    size_t  length;
    int bytes_sent;
    iik_callback_status_t status = iik_callback_continue;

    /* if nothing needs to be sent, check whether we need to send rx keepalive */
    if (iik_ptr->send_packet.total_length == 0 && iik_ptr->edp_state >= edp_discovery_layer)
    {
        rx_keepalive_process(iik_ptr);
    }

    if (iik_ptr->send_packet.total_length > 0)
    {
        buf = iik_ptr->send_packet.ptr + iik_ptr->send_packet.length;
        length = iik_ptr->send_packet.total_length;

        bytes_sent = send_buffer(iik_ptr, buf, length);
        if (bytes_sent > 0)
        {
            iik_ptr->send_packet.total_length -= bytes_sent;
            iik_ptr->send_packet.length += bytes_sent;
        }
        else
        {
            iik_ptr->error_code = -bytes_sent;
            status = iik_callback_abort;
        }
    }

    if (status == iik_callback_continue && iik_ptr->send_packet.total_length > 0)
    {
        status = iik_callback_busy;
    }
    return status;

}


static int receive_data(iik_data_t * iik_ptr, uint8_t * buffer, size_t length)
{
    int bytes_received = 0;

    iik_callback_status_t status;
    iik_read_request_t read_data;
    unsigned tx_keepalive;
    unsigned tx_ka_timeout;
    unsigned rx_ka_timeout;
    uint8_t wait_count;
    uint32_t time_stamp;
    size_t  length_read, size;
    iik_request_t request_id;

    if (!iik_ptr->network_busy)
    {

        if (get_system_time(iik_ptr, &time_stamp) != iik_callback_continue)
        {
            bytes_received = -iik_configuration_error;
            goto done;
        }

        /* rx keepalive timeout that we need to send a keepalive packet:
         *   = configured keepalive time  - (current time - last time we sent a packet);
         *
         * tx keepalive timeout that we expect a keepalive packet from server:
         *   = configured keepalive time  - (current time - last time we received a packet);
         *
         * timeout value is in seconds.
         */
        rx_ka_timeout = get_timeout_limit_in_seconds(*iik_ptr->rx_keepalive, (time_stamp - iik_ptr->rx_ka_time));
        tx_ka_timeout = get_timeout_limit_in_seconds(*iik_ptr->tx_keepalive, (time_stamp - iik_ptr->tx_ka_time));

        read_data.timeout = MIN_VALUE(tx_ka_timeout, rx_ka_timeout);

        read_data.network_handle = iik_ptr->network_handle;
        read_data.buffer = buffer;
        read_data.length = length;

        size = sizeof length_read;

        request_id.network_request = iik_network_receive;
        status = iik_callback(iik_ptr->callback, iik_class_network, request_id, &read_data, sizeof read_data, &length_read, &size);
        if (status == iik_callback_abort)
        {
            DEBUG_PRINTF("receive_data: callback returns abort\n");
            bytes_received = -iik_receive_error;
            goto done;
        }
        else if (status == iik_callback_busy)
        {
            goto done;
        }

        if (length_read > 0)
        {
            bytes_received = (int)length_read;
            /* Retain the "last (TX) message send" time. */
            if (get_system_time(iik_ptr, &iik_ptr->tx_ka_time) != iik_callback_continue)
            {
                bytes_received = -iik_configuration_error;
            }
            goto done;
        }
    }

    tx_keepalive = *iik_ptr->tx_keepalive * MILLISECONDS_PER_SECOND;
    if (tx_keepalive > 0)
    {
        wait_count = *iik_ptr->wait_count;
        if (!valid_interval_limit(iik_ptr, iik_ptr->tx_ka_time, (tx_keepalive  * wait_count)))
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
            bytes_received = -iik_keepalive_error;
            request_id.network_request = iik_network_receive;
            notify_error_status(iik_ptr->callback, iik_class_config, request_id, iik_keepalive_error);
            DEBUG_PRINTF("iik_receive: keepalive fail\n");
        }
    }

done:
    return bytes_received;
}


static iik_callback_status_t receive_data_status(iik_data_t * iik_ptr)
{
    uint8_t * buf;
    size_t  length;
    iik_callback_status_t status = iik_callback_continue;
    int     read_length;

    if (iik_ptr->receive_packet.length < iik_ptr->receive_packet.total_length)
    {
        buf = iik_ptr->receive_packet.ptr + iik_ptr->receive_packet.length;
        length = iik_ptr->receive_packet.total_length - iik_ptr->receive_packet.length;

        read_length = receive_data(iik_ptr, buf, length);
        if (read_length > 0)
        {
            iik_ptr->receive_packet.length += read_length;
        }
        else if (read_length < 0)
        {
            iik_ptr->error_code = -read_length;
            status = iik_callback_abort;
            goto done;
        }
    }

    if (iik_ptr->receive_packet.total_length > 0 &&
        iik_ptr->receive_packet.length < iik_ptr->receive_packet.total_length)
    {
        status = iik_callback_busy;
    }
done:
    return status;
}


static iik_callback_status_t receive_packet(iik_data_t * iik_ptr, iik_packet_t ** packet)
{
    iik_callback_status_t status = iik_callback_continue;
    uint16_t type_val;
    iik_request_t request_id;

    *packet = NULL;

    if (iik_ptr->network_handle == NULL)
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

    while (iik_ptr->receive_packet.index <= receive_packet_complete)
    {
        if (iik_ptr->receive_packet.index != receive_packet_init)
        {
            status = receive_data_status(iik_ptr);
            if (status != iik_callback_continue)
            {  /* receive pending */
                goto done;
            }
        }

        switch (iik_ptr->receive_packet.index)
        {
        case receive_packet_init:
            iik_ptr->receive_packet.packet_type = 0;
            iik_ptr->receive_packet.packet_length = 0;
            iik_ptr->receive_packet.length = 0;
            iik_ptr->receive_packet.total_length = 0;
            iik_ptr->receive_packet.index = 0;
            iik_ptr->receive_packet.data_packet = (iik_packet_t *)iik_ptr->receive_packet.buffer;
            iik_ptr->receive_packet.index++;
            break;

        case receive_packet_type:
            /* set to read the type of the message */
            iik_ptr->receive_packet.ptr = (uint8_t *)&iik_ptr->receive_packet.packet_type;
            iik_ptr->receive_packet.length = 0;
            iik_ptr->receive_packet.total_length = sizeof iik_ptr->receive_packet.packet_type;
            iik_ptr->receive_packet.index++;
            break;

        case receive_packet_length:
            /* Got message type let's get to message length
             * so make sure we support the message type.
             * Then, set to read message length. 
             */
            type_val = FROM_BE16(iik_ptr->receive_packet.packet_type);
            iik_ptr->receive_packet.packet_type = type_val;

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
                    iik_ptr->receive_packet.data_packet->length = 1;
                    iik_ptr->receive_packet.index++;

                    iik_ptr->receive_packet.total_length = iik_ptr->receive_packet.data_packet->length;

                    /*
                     * Read the actual message data bytes into the packet buffer.
                     */
                    iik_ptr->receive_packet.ptr = GET_PACKET_DATA_POINTER(iik_ptr->receive_packet.buffer, sizeof(iik_packet_t));
                    iik_ptr->receive_packet.length = 0;
                    iik_ptr->receive_packet.total_length = iik_ptr->receive_packet.data_packet->length;
                    iik_ptr->receive_packet.index++;

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
                    request_id.network_request = iik_network_receive;
                    iik_ptr->error_code = iik_invalid_packet;
                    status = notify_error_status(iik_ptr->callback, iik_class_config, request_id, iik_invalid_packet);
                    if (status == iik_callback_abort)
                    {
                        iik_ptr->receive_packet.index = 0;
                        goto done;
                    }
            }

            if (type_val != E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP)
            {   /* set up to read message length */
                iik_ptr->receive_packet.ptr = (uint8_t *)&iik_ptr->receive_packet.packet_length;
                iik_ptr->receive_packet.length = 0;
                iik_ptr->receive_packet.total_length = sizeof iik_ptr->receive_packet.packet_length;
                iik_ptr->receive_packet.index++;
            }
            break;

        case receive_packet_data:
            /* got packet length so set to read message data */
            iik_ptr->receive_packet.data_packet->length = FROM_BE16(iik_ptr->receive_packet.packet_length);
            iik_ptr->receive_packet.packet_length = iik_ptr->receive_packet.data_packet->length;
            if (iik_ptr->receive_packet.packet_type != E_MSG_MT2_TYPE_PAYLOAD)
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
                if (iik_ptr->receive_packet.packet_length != 0)
                {
                    DEBUG_PRINTF("iik_get_receive_packet: Invalid payload\n");
                    request_id.network_request = iik_network_receive;
                    iik_ptr->error_code = iik_invalid_payload_packet;
                    status = notify_error_status(iik_ptr->callback, iik_class_config, request_id, iik_invalid_payload_packet);
                    if (status == iik_callback_abort)
                    {
                        iik_ptr->receive_packet.index = 0;
                        goto done;
                    }
                }
            }
            /* set to read message data */
            iik_ptr->receive_packet.total_length = iik_ptr->receive_packet.data_packet->length;

            if (iik_ptr->receive_packet.data_packet->length == 0)
            {
                /* set to complete data since no data to be read. */
                iik_ptr->receive_packet.index = 4;
            }
            else 
            {
                /*
                 * Read the actual message data bytes into the packet buffer.
                 */
                iik_ptr->receive_packet.ptr = GET_PACKET_DATA_POINTER(iik_ptr->receive_packet.buffer, sizeof(iik_packet_t));
                iik_ptr->receive_packet.length = 0;
                iik_ptr->receive_packet.total_length = iik_ptr->receive_packet.data_packet->length;
                iik_ptr->receive_packet.index++;

            }

            break;
        case receive_packet_complete:
            iik_ptr->receive_packet.data_packet->type = iik_ptr->receive_packet.packet_type;
            iik_ptr->receive_packet.index = receive_packet_init;
            *packet = iik_ptr->receive_packet.data_packet;
            goto done;
        }
    }

done:
    return status;
}

static iik_callback_status_t connect_server(iik_data_t * iik_ptr, char * server_url, unsigned port)
{
    iik_callback_status_t status;
    iik_connect_request_t request;
    size_t length;
    iik_request_t request_id;

    request.host_name = server_url;
    request.port = port;

    request_id.network_request = iik_network_connect;
    status = iik_callback(iik_ptr->callback, iik_class_network, request_id, &request, sizeof request, &iik_ptr->network_handle, &length);
    if (status == iik_callback_continue)
    {
        if (iik_ptr->network_handle != NULL && length == sizeof(iik_network_handle_t))
        {
            iik_ptr->edp_connected = true;
            iik_ptr->network_busy = false;
        }
        else
        {
            iik_ptr->error_code = (iik_ptr->network_handle == NULL) ? iik_invalid_data : iik_invalid_data_size;
            request_id.network_request = iik_network_connect;
            status = notify_error_status(iik_ptr->callback, iik_class_config, request_id, iik_ptr->error_code);
        }
    }
    else if (status == iik_callback_abort)
    {
        iik_ptr->error_code = iik_connect_error;
    }
    return status;
}

static iik_callback_status_t close_server(iik_data_t * iik_ptr)
{
    iik_callback_status_t status = iik_callback_continue;
    iik_request_t request_id;

    if (iik_ptr->network_handle != NULL)
    {
        request_id.network_request = iik_network_close;
        status = iik_callback(iik_ptr->callback, iik_class_network, request_id, iik_ptr->network_handle, sizeof(iik_network_handle_t *), NULL, NULL);
        if (status == iik_callback_continue)
        {
            iik_ptr->network_handle = NULL;
            iik_ptr->edp_connected = false;
            iik_ptr->send_packet.total_length = 0;
            iik_ptr->receive_packet.index = 0;
;
        }
        else if (status == iik_callback_abort)
        {
            DEBUG_PRINTF("close_server: close callback aborts\n");
            iik_ptr->network_handle = NULL;
            iik_ptr->error_code = iik_close_error;
            iik_ptr->send_packet.total_length = 0;
            iik_ptr->receive_packet.index = 0;
        }
    }
    return status;
}

