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


#define RCI_COMMAND_REQUEST_START_OPCODE    0x01
#define RCI_COMMAND_REQUEST_DATA_OPCODE     0x02
#define RCI_COMMAND_REQUEST_END_OPCODE      0x03
#define RCI_COMMAND_REPLY_START_OPCODE      0x04
#define RCI_COMMAND_REPLY_DATA_OPCODE       0x05
#define RCI_COMMAND_REPLY_END_OPCODE        0x06
#define RCI_ERROR_DETECTED_OPCODE           0xE0
#define RCI_ANNOUNCE_ALGORITHM_OPCODE       0xB0

#define RCI_NO_COMPRESSION                 0x00
#define RCI_ZLIB_COMPRESSION                0xFF


#define RCI_COMPRESSION_COUNT               1

#define RCI_ZLIB_COMPRESSION_LENGTH 16

enum {
    rci_no_reply_requested,
    rci_reply_requested
};

enum {
    rci_idle_state,
    rci_configured_state,
    rci_request_start_state,
    rci_request_data_state,
    rci_request_end_start,
    rci_process_data_state,
    rci_send_response_state
};

typedef enum {
    rci_no_error,
    rci_fatal_error,
    rci_time_out_error,
    rci_compression_error,
    rci_decompression_error,
    rci_protocol_error,

    rci_message_format_error = 0xc1,
    rci_memory_error,
    rci_send_error,
    rci_processing_error
} rci_status_t;

typedef struct {
    int state;
    rci_status_t error_code;
    unsigned    supported_compression;
    uint8_t compression;
    struct {
        uint32_t total_length;
        uint32_t uncompressed_length;
        uint8_t * buffer;
        uint8_t * pointer;
        uint32_t length;
    } request;

    struct {
        uint8_t * buffer;
        uint8_t * pointer;
        uint32_t length;
        uint8_t * data;
        uint32_t data_length;
    } response;

} rci_data_t;


static idigi_callback_status_t rci_discovery(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet);

#if 0
static int send_message(idigi_data_t * idigi_ptr, uint8_t opcode, int error,
             uint32_t uncomp_len, uint32_t comp_len,
             void *data, uint32_t length, uint8_t comp_alg)
{

    uint8_t * ptr;
    uint8_t * start_ptr;
    idigi_packet_t   * packet;

    /* send abort or error
     *  --------------------------
     * |   0    |    1   |    2   |
     *  --------------------------
     * | opcode | target | status |
     *  --------------------------
     *
     *  Firmware ID string: [descr]0xa[file name spec]
     */
    packet =get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    start_ptr = ptr;

    *ptr++ = opcode;


    if (opcode == RCI_OP_REPLY_START)
    {
        // Add compression and length fields to packet
        pkt.buf[1] = comp_alg;
        StoreBE32 (&pkt.buf[2], uncomp_len);
        pkt.length += 5;
        if (comp_alg != RCI_COMP_ALG_NONE)
        {
            StoreBE32 (&pkt.buf[6], comp_len);
            pkt.length += 4;
        }
    }
    else if (opcode == RCI_OP_ERROR)
    {
        // Add error code field to packet
        pkt.buf[1] = error;
        pkt.length++;
    }

    if (data != NULL)
    {
        // Add data to packet
        memcpy (&pkt.buf[pkt.length], data, length);
        pkt.length += length;
    }

    if (e_mux_send (e_fac_rci_mc, E_MUX_FAC_RCI_NUM, &pkt) < 0)
    {
        return RCI_ERR_SEND;
    }

    return RCI_ERR_NONE;
}
#endif

static idigi_callback_status_t send_error_message(idigi_data_t * idigi_ptr, uint8_t error_code)
{

    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * ptr;
    uint8_t * start_ptr;
    idigi_packet_t * packet;

    /* send error message:
     *  ---------------------
     * |   0    |      1     |
     *  ---------------------
     * | opcode | error code |
     *  ---------------------
     *
     */
    packet =get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    start_ptr = ptr;
    *ptr++ = RCI_ERROR_DETECTED_OPCODE;
    *ptr++ = error_code;
    packet->header.length = ptr - start_ptr;

    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_RCI_NUM, release_packet_buffer, NULL);
done:
    return status;
}

static void rci_done_request(idigi_data_t * idigi_ptr, rci_data_t * rci_ptr)
{
    /* these buffers are pointer from malloc callback, we need to release them */
    if (rci_ptr->request.buffer != NULL)
    {
        idigi_request_t request_id;
        request_id.rci_request = idigi_rci_decompress_data_done;
        idigi_callback(idigi_ptr->callback, idigi_class_rci, request_id, NULL, 0, NULL, NULL);
    }
    if (rci_ptr->response.buffer != NULL)
    {
        idigi_request_t request_id;
        request_id.rci_request = idigi_rci_compress_data_done;
        idigi_callback(idigi_ptr->callback, idigi_class_rci, request_id, NULL, 0, NULL, NULL);
    }
    rci_ptr->request.buffer = NULL;
    rci_ptr->response.buffer = NULL;
    rci_ptr->state = rci_configured_state;
}

static char const * no_query_state_response = "<rci_replay version=\"1.1\"> <query_state/> </rci_reply>";
static void rci_send_packet_callback(idigi_data_t * idigi_ptr, idigi_packet_t * packet, idigi_status_t status, void * user_data)
{
    rci_data_t * rci_ptr = user_data;
    uint8_t * ptr;

//    release_packet_buffer(idigi_ptr, packet, status, NULL);

    if (status == idigi_success && rci_ptr->response.length > 0)
    {
        uint16_t send_length;
        uint8_t * start_ptr;
        uint8_t opcode;

        if (rci_ptr->response.length >= packet->header.avail_length)
        {
            send_length = packet->header.avail_length;
            opcode = RCI_COMMAND_REPLY_DATA_OPCODE;
        }
        else
        {
            send_length = rci_ptr->response.length;
            opcode = RCI_COMMAND_REPLY_END_OPCODE;
        }
        packet =get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, sizeof(idigi_packet_t), &ptr);
        ASSERT_GOTO(packet != NULL, done);
        start_ptr = ptr;
        *ptr++ = opcode;
        *ptr++ = rci_ptr->compression;

        StoreBE32(ptr, send_length);
        ptr += sizeof send_length;

        /* now add this target to the target list message */
        memcpy(ptr, rci_ptr->response.pointer, send_length);
        ptr += send_length;
        packet->header.length = ptr - start_ptr;
        rci_ptr->response.pointer += send_length;
        rci_ptr->response.length -= send_length;
        DEBUG_PRINTF("rci_send_packet_callback: send chunk length %d\n", send_length);
        status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_RCI_NUM, rci_send_packet_callback, rci_ptr);
        ASSERT(status == idigi_callback_continue);
    }
    else
    {
        release_packet_buffer(idigi_ptr, packet, status, NULL);
        rci_done_request(idigi_ptr, rci_ptr);
        DEBUG_PRINTF("rci_send_packet_callback: done sending\n");
    }
done:
    return;
}

static idigi_callback_status_t rci_send_response(idigi_data_t * idigi_ptr, rci_data_t * rci_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_packet_t * packet;
    uint8_t * ptr;
    uint8_t * start_ptr;
    uint32_t send_length;


    packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_RCI_NUM, sizeof(idigi_packet_t), &ptr);
    if (packet == NULL)
    {
        goto done;
    }
    start_ptr = ptr;

    rci_ptr->response.buffer = NULL;
    rci_ptr->response.pointer = rci_ptr->response.data;
    rci_ptr->response.length = rci_ptr->response.data_length;

    if (rci_ptr->compression == RCI_ZLIB_COMPRESSION)
    {
        idigi_request_t request_id;
        request_id.rci_request = idigi_rci_compress_data;
        status = idigi_callback(idigi_ptr->callback, idigi_class_rci, request_id, NULL, 0, NULL, NULL);
        if (status != idigi_callback_continue)
        {
            goto done;
        }
    }
    *ptr++ = RCI_COMMAND_REPLY_START_OPCODE;
    *ptr++ = rci_ptr->compression;

    send_length = (rci_ptr->response.length >= packet->header.avail_length) ?
                   packet->header.avail_length: rci_ptr->response.length;

    StoreBE32(ptr, rci_ptr->response.data_length);
    ptr += sizeof rci_ptr->response.data_length;

    if (rci_ptr->compression == RCI_ZLIB_COMPRESSION)
    {
        StoreBE32(ptr, send_length);
        ptr += sizeof send_length;
    }

    /* now add this target to the target list message */
    memcpy(ptr, rci_ptr->response.pointer, send_length);
    ptr += send_length;
    packet->header.length = ptr - start_ptr;

    rci_ptr->response.pointer += send_length;
    rci_ptr->response.length -= send_length;

    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_RCI_NUM, rci_send_packet_callback, rci_ptr);

done:
    return status;
}

static idigi_callback_status_t rci_process_request_data(idigi_data_t * idigi_ptr, rci_data_t * rci_ptr, uint8_t * data, uint32_t length)
{

    idigi_callback_status_t status = idigi_callback_continue;

    ASSERT_GOTO(data != NULL, done);
    ASSERT_GOTO(length > 0, done);

    if (rci_ptr->compression != rci_ptr->supported_compression)
    {
        rci_ptr->compression = RCI_NO_COMPRESSION;
    }

    if (rci_ptr->compression != RCI_NO_COMPRESSION)
    {
        idigi_request_t request_id;
        request_id.rci_request = idigi_rci_decompress_data;
        status = idigi_callback(idigi_ptr->callback, idigi_class_rci, request_id, NULL, 0, NULL, NULL);
        if (status != idigi_callback_continue)
        {
            goto done;
        }
    }
    /* SAX parser request data
     * TODO: add SAX parser process
     */

    rci_ptr->response.data = (uint8_t *)no_query_state_response;
    rci_ptr->response.data_length = strlen(no_query_state_response);
    rci_ptr->state = rci_send_response_state;


done:
    return status;
}

static idigi_callback_status_t rci_command_request_start(idigi_data_t * idigi_ptr, rci_data_t * rci_ptr, uint8_t * message, uint32_t length)
{
#define RCI_COMMAND_REQUEST_START_MIN_LENGTH    6
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * ptr = message;
    uint32_t len = length;

    /* Parse Command request start message. opcode is not
     * included in the message argument.
     *
     * Uncompressed request format:
     *  ----------------------------------------------
     * |   0    |      1      |     2 - 5    |   10...|
     *  ----------------------------------------------
     * | opcode | compression | uncompressed |  data  |
     * |        |  algorithm  |     size     |        |
     *  ----------------------------------------------
     *
     * Compressed request format:
     *  -----------------------------------------------------------
     * |   0    |      1      |     2 - 5    |   6 - 9    |  10...|
     *  -----------------------------------------------------------
     * | opcode | compression | uncompressed | compressed | data  |
     * |        |  algorithm  |     size     |    size    |       |
     *  ----------------------------------------------------------
     *
     */

    ASSERT(length > RCI_COMMAND_REQUEST_START_MIN_LENGTH);
    if (length <= RCI_COMMAND_REQUEST_START_MIN_LENGTH)
    {
        rci_ptr->error_code = rci_message_format_error;
        goto done;
    }

    rci_ptr->compression = *ptr++;
    len--;
    rci_ptr->request.uncompressed_length = LoadBE32(ptr);
    ptr += sizeof(uint32_t);
    len -= sizeof(uint32_t);

    if (rci_ptr->compression != RCI_NO_COMPRESSION)
    {
        /* get compressed size */
        rci_ptr->request.length = LoadBE32(ptr);
        ptr += sizeof(uint32_t);
        len -= sizeof(uint32_t);
    }
    else
    {   /* no compressed size so the uncompressed length is the size of the data */
        rci_ptr->request.length = rci_ptr->request.uncompressed_length;
    }


    ASSERT(len <= rci_ptr->request.length);
    if (len > rci_ptr->request.length)
    {
        rci_ptr->error_code = rci_message_format_error;
        goto done;
    }

    /* we only have one block of data so let's process it */
    if (len == rci_ptr->request.length)
    {
        status = rci_process_request_data(idigi_ptr, rci_ptr, ptr, rci_ptr->request.length);
    }
    else
    {  /* more blocks of data is expected so let's allocate memory and
        * save the 1st block
        */
        void * buf;
        status = malloc_data(idigi_ptr, rci_ptr->request.length, &buf);
        if (status != idigi_callback_continue)
        {
            goto done;
        }
        ASSERT(buf != NULL);
        if (buf == NULL)
        {
            rci_ptr->error_code = rci_memory_error;
            goto done;
        }

        rci_ptr->request.buffer = buf;
        rci_ptr->request.pointer = buf;
        rci_ptr->state = rci_request_start_state;

        memcpy(rci_ptr->request.pointer, ptr, len);
        rci_ptr->request.pointer += len;
    }

done:
    return status;
}
static idigi_callback_status_t rci_process(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet)
{

    idigi_callback_status_t status = idigi_callback_continue;
    rci_data_t * rci_ptr = facility_data;
    uint8_t opcode;
    uint8_t * ptr;
    uint16_t length;

    /* process incoming message from server for RCI facility */

    UNUSED_PARAMETER(idigi_ptr);
    if (packet == NULL)
    {
        goto done;
    }

    /* get the DATA portion of the packet */
    ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
    opcode = *ptr++;
    length = packet->header.length-1; /* skip opcode */


    if (rci_ptr->state != rci_send_response_state)
    {
        switch(opcode)
        {
        case RCI_COMMAND_REQUEST_START_OPCODE:
            ASSERT(rci_ptr->state < rci_request_start_state);
            if (rci_ptr->state >= rci_request_start_state)
            {
                /* we already have received command request start message */
                rci_ptr->error_code = rci_protocol_error;
                goto done;
            }
            status = rci_command_request_start(idigi_ptr, rci_ptr, ptr, length);
            break;
        case RCI_COMMAND_REQUEST_DATA_OPCODE:
        case RCI_COMMAND_REQUEST_END_OPCODE:
            ASSERT(rci_ptr->state == rci_request_start_state);
            if (rci_ptr->state != rci_request_start_state)
            {
                /* we already have received command request start message */
                rci_ptr->error_code = rci_protocol_error;
                goto done;
            }

            /* just copy the data into the buffer that allocated on
             * RCI_COMMAND_REQUEST_START_OPCODE process.
             */
            ASSERT(((rci_ptr->request.pointer - rci_ptr->request.buffer) + (uint32_t)length) <= rci_ptr->request.length);
            if (((rci_ptr->request.pointer - rci_ptr->request.buffer) + (uint32_t)length) > rci_ptr->request.length)
            {
                rci_ptr->error_code = rci_message_format_error;
                goto done;
            }
            memcpy(rci_ptr->request.pointer, ptr, length);
            rci_ptr->request.pointer += length;

            if (opcode == RCI_COMMAND_REQUEST_END_OPCODE)
            {
                status = rci_process_request_data(idigi_ptr, rci_ptr, rci_ptr->request.pointer, rci_ptr->request.length);
            }
            break;

        case RCI_ERROR_DETECTED_OPCODE:
            DEBUG_PRINTF("RCI: got Error Detected message\n");
            rci_done_request(idigi_ptr, rci_ptr);
            break;

        case RCI_ANNOUNCE_ALGORITHM_OPCODE:
            DEBUG_PRINTF("RCI: server requests announcement message\n");
            status = rci_discovery(idigi_ptr, rci_ptr, packet);
            break;
        }
    }


    if (status == idigi_callback_continue && rci_ptr->state == rci_send_response_state)
    {
        status = rci_send_response(idigi_ptr, rci_ptr);
    }


done:
    if (rci_ptr->error_code != rci_no_error)
    {
        if (status != idigi_callback_abort)
        {
            status = send_error_message(idigi_ptr, rci_ptr->error_code);
        }
        rci_done_request(idigi_ptr, rci_ptr);
    }

    return status;
}
#define RCI_SET_SUPPORTED_COMPRESSION(i)                (0x01 << (i))
#define RCI_IS_SUPPORTED_COMPRESSION(compression, i)    ((compression) & (0x01 << (i)))

static idigi_callback_status_t rci_discovery(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    rci_data_t * rci_ptr = facility_data;
    idigi_packet_t * response_packet;
    uint8_t * ptr;
    uint8_t * start_ptr;

    UNUSED_PARAMETER(packet);
    /* RCI needs to send announce compression algorithm */
    DEBUG_PRINTF("RCI facility: send announce compression algorithm\n");

    /* Need to call callback to get compression support after
     * RCI is just initialized (rci_init just called).
     *
     * We don't need to call the callback again if we already call the calback.
     */
    if (rci_ptr->state == rci_idle_state)
    {
        idigi_request_t request_id;
        bool compression_supported;

        status = idigi_callback(idigi_ptr->callback, idigi_class_rci, request_id, &compression_supported, sizeof compression_supported, NULL, NULL);
        if (status != idigi_callback_continue)
        {
            goto done;
        }
        if (compression_supported)
        {
            rci_ptr->supported_compression = RCI_ZLIB_COMPRESSION;
        }
        rci_ptr->state = rci_configured_state;
    }

    if (packet != NULL)
    {
        uint8_t opcode;
        uint8_t flag;

        /* get the DATA portion of the packet */
        ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
        opcode = *ptr++;
        flag = *ptr++;

        if (flag == rci_no_reply_requested)
        {
            /* no reply is requested. */
            goto done;
        }
    }
    /* Build announce compression algorithm
     *  -------------------------------------------------------
     * |   0    |   1  |     2     |     3       |  4...       |
     *  -------------------------------------------------------
     * | opcode | flag | algorithm | compression | additioanl  |
     * |        |      |   count   |  algorithm  | compression.|
     *  -------------------------------------------------------
     *
     */
    response_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_RCI_NUM, sizeof(idigi_packet_t), &ptr);
    if (response_packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }
    start_ptr = ptr;

    *ptr++ = RCI_ANNOUNCE_ALGORITHM_OPCODE;
    *ptr++ = ((packet == NULL) ? rci_reply_requested: rci_no_reply_requested);

    /* We currently support only zlib compression.
     * TODO: If additional compression is added, update announcement message.
     *
     */
    *ptr++ = RCI_COMPRESSION_COUNT;
    *ptr++ = rci_ptr->supported_compression;
    response_packet->header.length = ptr - start_ptr;

    status = enable_facility_packet(idigi_ptr, response_packet, E_MSG_FAC_RCI_NUM, release_packet_buffer, NULL);

done:
    return status;
}

static idigi_callback_status_t rci_delete_facility(idigi_data_t * idigi_ptr)
{
    return del_facility_data(idigi_ptr, E_MSG_FAC_RCI_NUM);
}

static idigi_callback_status_t rci_init_facility(idigi_data_t * idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    rci_data_t * rci_ptr;

    /* Add Connection control facility to iDigi
     *
     * Make sure connection control is not already created. If Connection
     * control facility is already created, we probably reconnect to server
     * so just need to reset to initial state.
     *
     */
    rci_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_RCI_NUM);
    if (rci_ptr == NULL)
    {
        void * ptr;

        status = add_facility_data(idigi_ptr, E_MSG_FAC_RCI_NUM, &ptr, sizeof(rci_data_t), rci_discovery, rci_process);

        if (status == idigi_callback_abort || ptr == NULL)
        {
            goto done;
        }
        rci_ptr = (rci_data_t *)ptr;
        rci_ptr->state = rci_idle_state;
        rci_ptr->supported_compression = 0;
        rci_ptr->error_code = rci_no_error;
        rci_ptr->request.buffer = NULL;
        rci_ptr->response.buffer = NULL;

    }

done:
    return status;
}


