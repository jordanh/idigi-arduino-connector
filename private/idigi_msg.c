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

#if (defined IDIGI_COMPRESSION_BUILTIN)
#include "zlib.h"
#endif

#define MSG_RECV_WINDOW_SIZE      16384

#define MSG_CAPABILITIES_REQUEST  0x01
#define MSG_CAPABILITIES_RESPONSE 0x00

#define MSG_FACILITY_VERSION  0x01

#define MSG_MAX_RECV_TRANSACTIONS   1
#define MSG_UNLIMITED_TRANSACTIONS  0

#define MSG_INVALID_SESSION_ID      0xFFFF

#define MSG_NO_COMPRESSION          0
#define MSG_COMPRESSION_LIBZ        0xFF

#define MSG_START_HEADER_LEN    7
#define MSG_DATA_HEADER_LEN     4

typedef enum
{
    msg_service_id_none,
    msg_service_id_data,
    msg_service_id_file,
    msg_service_id_count
} msg_service_id_t;

typedef enum
{
    msg_opcode_capability,
    msg_opcode_start,
    msg_opcode_data,
    msg_opcode_ack,
    msg_opcode_error,
    msg_opcode_count
} msg_opcode_t;

typedef enum
{
    msg_state_init,
    msg_state_start,
    msg_state_start_and_last,
    msg_state_data,
    msg_state_last,
    msg_state_send_complete,
    msg_state_error
} msg_state_t;

#define MSG_FLAG_REQUEST      0x01
#define MSG_FLAG_LAST_DATA    0x02
#define MSG_FLAG_SENDER       0x04
#define MSG_FLAG_MORE_DATA    0x10000
#define MSG_FLAG_PAUSED       0x20000
#define MSG_FLAG_COMPRESSED   0x40000
#define MSG_FLAG_CALL_BACK    0x80000

typedef struct msg_data_block_t
{
    uint32_t available_window;
    msg_state_t state;
    uint8_t frame[MSG_MAX_PACKET_SIZE];
    uint8_t const * payload;
    size_t payload_length;

    uint8_t * user_data;
    size_t bytes_remaining;
    size_t bytes_in_frame;
    size_t total_message_bytes;

    size_t ack_count;
    uint32_t flags;
#if (defined IDIGI_COMPRESSION_BUILTIN)
    z_stream zlib;
#endif
}msg_data_block_t;

#define MsgIsRequest(block)     (((block)->flags & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST)
#define MsgIsLastData(block)    (((block)->flags & MSG_FLAG_LAST_DATA) == MSG_FLAG_LAST_DATA)
#define MsgIsMoreData(block)    (((block)->flags & MSG_FLAG_MORE_DATA) == MSG_FLAG_MORE_DATA)
#define MsgIsPaused(block)      (((block)->flags & MSG_FLAG_PAUSED) == MSG_FLAG_PAUSED)
#define MsgIsCompressed(block)  (((block)->flags & MSG_FLAG_COMPRESSED) == MSG_FLAG_COMPRESSED)
#define MsgIsCallback(block)    (((block)->flags & MSG_FLAG_CALL_BACK) == MSG_FLAG_CALL_BACK)

#define MsgSetRequest(block)     (block)->flags |= MSG_FLAG_REQUEST
#define MsgSetLastData(block)    (block)->flags |= MSG_FLAG_LAST_DATA
#define MsgSetMoreData(block)    (block)->flags |= MSG_FLAG_MORE_DATA
#define MsgSetPaused(block)      (block)->flags |= MSG_FLAG_PAUSED
#define MsgSetCompression(block) (block)->flags |= MSG_FLAG_COMPRESSED
#define MsgSetCallback(block)    (block)->flags |= MSG_FLAG_CALL_BACK

#define MsgClearMoreData(block)    (block)->flags &= ~MSG_FLAG_MORE_DATA
#define MsgClearPaused(block)      (block)->flags &= ~MSG_FLAG_PAUSED
#define MsgClearCompression(block) (block)->flags &= ~MSG_FLAG_COMPRESSED
#define MsgClearCallback(block)    (block)->flags &= ~MSG_FLAG_CALL_BACK

enum msg_capability_packet_t
{
    field_define(capabilty_packet, opcode, uint8_t),
    field_define(capabilty_packet, flags, uint8_t),
    field_define(capabilty_packet, version, uint8_t),
    field_define(capabilty_packet, max_transactions, uint8_t),
    field_define(capabilty_packet, window_size, uint32_t),
    field_define(capabilty_packet, compression_count, uint8_t),
    record_end(capabilty_packet)
};

enum msg_start_packet_t
{
    field_define(start_packet, opcode, uint8_t),
    field_define(start_packet, flags, uint8_t),
    field_define(start_packet, transaction_id, uint16_t),
    field_define(start_packet, service_id, uint16_t),
    field_define(start_packet, compression_id, uint8_t),
    record_end(start_packet)
};

enum msg_data_packet_t
{
    field_define(data_packet, opcode, uint8_t),
    field_define(data_packet, flags, uint8_t),
    field_define(data_packet, transaction_id, uint16_t),
    record_end(data_packet)
};

enum msg_ack_packet_t
{
    field_define(ack_packet, opcode, uint8_t),
    field_define(ack_packet, flags, uint8_t),
    field_define(ack_packet, transaction_id, uint16_t),
    field_define(ack_packet, ack_count, uint32_t),
    field_define(ack_packet, window_size, uint32_t),
    record_end(ack_packet)
};

enum msg_error_packet_t
{
    field_define(error_packet, opcode, uint8_t),
    field_define(error_packet, flags, uint8_t),
    field_define(error_packet, transaction_id, uint16_t),
    field_define(error_packet, error_code, uint8_t),
    record_end(error_packet)
};

#define MSG_SESSION_SIGNATURE   0x494C5347
/* Miscellaneous packet includes ack and error packets */
#define MSG_MISC_PKT_SIZE   (PACKET_EDP_FACILITY_SIZE + record_end(ack_packet))

enum {
    msg_server_request_id,
    msg_client_request_id,
    msg_reqeust_id_count
};

typedef struct msg_session_t
{
    uint16_t session_id;
    uint16_t service_id;
    uint32_t signature;
    msg_data_block_t send_block;
    msg_data_block_t recv_block;
    struct msg_session_t * next;
    struct msg_session_t * prev;

    void * service_data;
    bool send_ack;

} msg_session_t;

#define MsgIsSessionValid(session)  (session->signature == MSG_SESSION_SIGNATURE)


typedef idigi_callback_status_t idigi_msg_callback_t(idigi_data_t * const idigi_ptr, msg_state_t const msg_state, msg_session_t * const session, uint8_t const * buffer, size_t const length);

typedef struct {
    uint32_t window_size;
    unsigned int active_transactions;
    uint8_t max_transactions;
    bool compression_supported;
} msg_capabilities_t;

typedef struct
{
    msg_capabilities_t capabilities[msg_reqeust_id_count];

    idigi_msg_callback_t * service_cb[msg_service_id_count];
    uint16_t cur_id;
    msg_session_t * session_head;
    msg_session_t * session_tail;
    msg_session_t * pending;

    idigi_lock_handle_t lock_handle;
} idigi_msg_data_t;

typedef struct
{
    size_t header_length;
    uint8_t * header;
    size_t payload_length;
    void const * payload;
    unsigned int flag;
} msg_data_info_t;

static void msg_send_complete(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data);

static msg_session_t * msg_find_session(idigi_msg_data_t const * const msg_ptr, uint16_t const id, bool const client_owned)
{
    msg_session_t * session = msg_ptr->session_head;


    while (session != NULL)
    {
        if (session->session_id == id)
        {
            msg_data_block_t * block = client_owned ? &session->send_block : &session->recv_block;

            if (MsgIsRequest(block))
                break;
        }

        session = session->next;
    }

    return session;
}

static uint16_t msg_find_next_available_id(idigi_msg_data_t * const msg_ptr)
{
    uint16_t new_id = MSG_INVALID_SESSION_ID;
    uint16_t const last_assigned_id = msg_ptr->cur_id;

    do
    {
        static bool const client_owned = true;

        msg_ptr->cur_id++; /* wraps around at 64k to search entire range */

        if (msg_ptr->cur_id == MSG_INVALID_SESSION_ID)
            continue;

        if (msg_find_session(msg_ptr, msg_ptr->cur_id, client_owned) == NULL)
        {
            new_id = msg_ptr->cur_id;
            break;
        }

    } while (msg_ptr->cur_id != last_assigned_id);

    return new_id;
}

static void msg_initialize_data_block(msg_data_block_t * const data_block, uint32_t const window)
{
    data_block->available_window = window;
    data_block->flags = 0;
    data_block->state = msg_state_init;
    data_block->bytes_in_frame = 0;
    data_block->bytes_remaining = 0;
    data_block->total_message_bytes = 0;
    data_block->ack_count = 0;
}

static idigi_status_t msg_create_session(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr,
                                          uint16_t const service_id, bool const find_new_id, msg_session_t ** new_session)
{
    idigi_status_t result = idigi_invalid_data;
    uint16_t session_id = MSG_INVALID_SESSION_ID;
    msg_session_t * session;

    /* if find_new_id is set, it means client is going to send request msg to server and we need
     * to use service's capabilities.
     */
    uint8_t const msg_request_id = (find_new_id) ? msg_server_request_id: msg_client_request_id;

    ASSERT_GOTO(new_session != NULL, done);

    {
        idigi_callback_status_t const status = MutexLock(idigi_ptr, msg_ptr->lock_handle);
        switch (status)
        {
        case idigi_callback_busy:
            result = idigi_service_busy;
            goto done;
        case idigi_callback_continue:
            break;
        case idigi_callback_unrecognized:
        case idigi_callback_abort:
            result = idigi_configuration_error;
            goto done;
        }
    }

    {
        uint16_t const max_transactions = msg_ptr->capabilities[msg_request_id].max_transactions;
        uint16_t const active_transactions = msg_ptr->capabilities[msg_request_id].active_transactions;

        if ((max_transactions != MSG_UNLIMITED_TRANSACTIONS ) &&
            (active_transactions >= max_transactions))
        {
            DEBUG_PRINTF("msg_create_session: active transactions %d > max transactions %d\n", active_transactions, max_transactions);
            /* server should not send more than client's max transactions */
            ASSERT(msg_request_id != msg_client_request_id);
            result = idigi_no_resource;
            goto error;
        }
    }

    /* msg_request_id is id which session is created for.
     * If msg_request_id is msg_server_request_id, client is initiating this session to the server.
     * If msg_request_id is msg_client_request_id, server is initiating this session to the client.
     */
    if (msg_request_id == msg_server_request_id)
    {
        session_id = msg_find_next_available_id(msg_ptr);
        if (session_id == MSG_INVALID_SESSION_ID)
        {
            result = idigi_no_resource;
            goto error;
        }
    }

    {
        void *ptr;
        idigi_callback_status_t const status = malloc_data(idigi_ptr, sizeof(msg_session_t), &ptr);
        if (status != idigi_callback_continue)
        {
            result = (status == idigi_callback_busy) ? idigi_service_busy : idigi_configuration_error;

            goto error;
        }
        session = ptr;
    }

    session->session_id = session_id;
    session->service_id = service_id;
    session->service_data = NULL;
    session->send_ack = false;
    session->signature = MSG_SESSION_SIGNATURE;
    msg_initialize_data_block(&session->send_block, msg_ptr->capabilities[msg_server_request_id].window_size);
    msg_initialize_data_block(&session->recv_block, msg_ptr->capabilities[msg_client_request_id].window_size);

    add_node(&msg_ptr->session_head, session);
    if (msg_ptr->session_tail == NULL)
        msg_ptr->session_tail = session;
    msg_ptr->capabilities[msg_request_id].active_transactions++;

    *new_session = session;
    result = idigi_success;

error:
    MutexUnlock(idigi_ptr, msg_ptr->lock_handle);

done:
    return result;
}

static void msg_delete_session(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr,
                               msg_session_t * const session)
{
    MutexLock(idigi_ptr, msg_ptr->lock_handle);

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(session != NULL, error);

    remove_node(&msg_ptr->session_head, session);

#if (defined IDIGI_COMPRESSION_BUILTIN)
    if (MsgIsCompressed(&session->send_block))
        deflateEnd(&session->send_block.zlib);
    if (MsgIsCompressed(&session->recv_block))
        inflateEnd(&session->recv_block.zlib);
#endif

    if (msg_ptr->session_tail == session)
        msg_ptr->session_tail = session->prev;

    if (msg_ptr->pending == session)
    {
        msg_ptr->pending = msg_ptr->session_tail;
    }

    if (msg_ptr->pending == session)
    {
        msg_ptr->pending = (session->prev != NULL) ? session->prev : msg_ptr->session_tail;
    }

    if (session->service_data != NULL)
    {
        free_data(idigi_ptr, session->service_data);
    }
    DEBUG_PRINTF("msg_delete_session: session %p\n", session);

    {
        /* if request flag of send_block is set, it means client initiated the message
         * (transaction to server) and we need to use server's capablitites.
         */
        uint8_t const msg_request_id = MsgIsRequest(&session->send_block) ? msg_server_request_id : msg_client_request_id;

        ASSERT_GOTO(msg_ptr->capabilities[msg_request_id].active_transactions > 0, error);
        msg_ptr->capabilities[msg_request_id].active_transactions--;
    }
    free_data(idigi_ptr, session);

error:
    MutexUnlock(idigi_ptr, msg_ptr->lock_handle);
    return;
}

static uint16_t msg_get_service_count(idigi_msg_data_t const * const msg_ptr)
{
    uint16_t count = 0;
    uint16_t service_id;

    for (service_id = 0; service_id < msg_service_id_count; service_id++) 
    {
        if (msg_ptr->service_cb[service_id] !=  NULL) 
            count++;
    }

    return count;
}

static void msg_error_complete_callback(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data)
{
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    if (user_data != NULL)
        msg_delete_session(idigi_ptr, msg_ptr, user_data);

    release_packet_buffer(idigi_ptr, packet, status, NULL);

    return;
}

static idigi_callback_status_t msg_send_error(idigi_data_t * const idigi_ptr, msg_session_t * const session, uint16_t const session_id, idigi_msg_error_t const error_value, uint8_t const flag)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * error_packet;
    uint8_t * edp_header;

    edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &error_packet);
    if (edp_header == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    message_store_u8(error_packet, opcode, msg_opcode_error);
    message_store_u8(error_packet, flags, flag);
    message_store_be16(error_packet, transaction_id, session_id);

    {
        uint8_t const error = error_value;

        ASSERT_GOTO(error_value < idigi_msg_error_count, done);
        message_store_u8(error_packet, error_code, error);
    }

    status = initiate_send_facility_packet(idigi_ptr, edp_header, record_end(error_packet),
                                           E_MSG_FAC_MSG_NUM, msg_error_complete_callback, session);

done:
    return status;
}

static idigi_callback_status_t msg_send_capabilities(idigi_data_t * const idigi_ptr, idigi_msg_data_t const * const msg_data, uint8_t const flag)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * capabilty_packet = NULL;
    uint8_t * packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &capabilty_packet);
    size_t packet_len = record_bytes(capabilty_packet);
    uint8_t * variable_data_ptr = capabilty_packet + packet_len;

    if ((packet == NULL) || (capabilty_packet == NULL))
        goto error;

    /*
     * ------------------------------------------------------------------------------------------
     * |   0    |    1  |     2   |    3    |   4-7  |     8    | 9-(8+n) |     9+n   | s bytes |
     *  -----------------------------------------------------------------------------------------
     * | opcode | flags | version | Max Trx | Window | Count(n) | list    | Count (s) |  list   |
     * |        |       |         |         |  size  |  compression algo  |     Service Info    |
     *  -----------------------------------------------------------------------------------------
     */
    message_store_u8(capabilty_packet, opcode, msg_opcode_capability);
    message_store_u8(capabilty_packet, flags, flag);
    message_store_u8(capabilty_packet, version, MSG_FACILITY_VERSION);
    message_store_u8(capabilty_packet, max_transactions, msg_data->capabilities[msg_client_request_id].max_transactions);
    message_store_be32(capabilty_packet, window_size, msg_data->capabilities[msg_client_request_id].window_size);

    /* append compression algorithms supported */
    {
        uint8_t compression_length = (msg_data->capabilities[msg_client_request_id].compression_supported) ? 1 : 0;
        message_store_u8(capabilty_packet, compression_count, compression_length);
        if (compression_length > 0)
            *variable_data_ptr++ = MSG_COMPRESSION_LIBZ;
    }

    /* append service IDs of all listeners */
    {
        uint16_t const services = msg_get_service_count(msg_data);
        uint16_t service_id;

        StoreBE16(variable_data_ptr, services);
        variable_data_ptr += sizeof services;
        
        for (service_id = 0; service_id < msg_service_id_count; service_id++) 
        {
            if (msg_data->service_cb[service_id] !=  NULL)
            {
                StoreBE16(variable_data_ptr, service_id);
                variable_data_ptr += sizeof service_id;
            }
        }
    }

    packet_len = variable_data_ptr - capabilty_packet;
    status = initiate_send_facility_packet(idigi_ptr, packet, packet_len, E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);

error:
    return status;
}

static bool msg_fill_uncompressed_data(msg_data_block_t * const data_block, uint8_t * dest_ptr, size_t const bytes)
{
    if (bytes > 0)
    {
        memcpy(dest_ptr, data_block->user_data, bytes);
        data_block->user_data += bytes;
        data_block->bytes_remaining -= bytes;
        data_block->bytes_in_frame += bytes;
        data_block->total_message_bytes += bytes;
        if (data_block->bytes_remaining > 0)
            MsgSetMoreData(data_block);
        else
            MsgClearMoreData(data_block);
    }

    return true;
}

static bool msg_frame_uncompressed(msg_data_block_t * const data_block, msg_data_info_t const * const info)
{
    bool success = true;
    uint8_t * out_ptr = GET_PACKET_DATA_POINTER(data_block->frame, PACKET_EDP_FACILITY_SIZE);
    size_t bytes_in_frame = 0;

    switch (data_block->state) 
    {
    case msg_state_init:
        data_block->state = msg_state_start;
        MsgClearMoreData(data_block);
        ASSERT_GOTO(info != NULL, done);
        ASSERT_GOTO(info->header_length > 0, done);
        out_ptr += MSG_START_HEADER_LEN;
        memcpy(out_ptr, info->header, info->header_length);
        out_ptr += info->header_length;
        data_block->bytes_in_frame = MSG_START_HEADER_LEN + info->header_length;
        /* don't fill the data yet. let msg_process_pending() to build the message with data */
        goto done;


    case msg_state_start:
        out_ptr += data_block->bytes_in_frame;
        bytes_in_frame = data_block->bytes_in_frame;
#if 0
        success = msg_fill_uncompressed_data(data_block, out_ptr);
        if (MsgIsLastData(data_block) && !MsgIsMoreData(data_block))
            data_block->state = msg_state_start_and_last;
#endif
        break;
    
    case msg_state_data:
        out_ptr += MSG_DATA_HEADER_LEN;
        bytes_in_frame = MSG_DATA_HEADER_LEN;
#if 0
        success = msg_fill_uncompressed_data(data_block, out_ptr);
        if (MsgIsLastData(data_block) && !MsgIsMoreData(data_block))
            data_block->state = msg_state_last;
#endif
        break;

    default:
        ASSERT_GOTO(false, done);
        break;
    }

    {
        size_t const available_space = sizeof data_block->frame - (out_ptr - data_block->frame);
        size_t bytes = (available_space < data_block->bytes_remaining) ? available_space : data_block->bytes_remaining;

        uint32_t const window_size = data_block->available_window;
        size_t const unack_count = (data_block->total_message_bytes - data_block->ack_count);

        if (window_size < unack_count + bytes + bytes_in_frame)
        {
            bytes = ((window_size - unack_count) < bytes_in_frame) ? 0 : (window_size - unack_count - bytes_in_frame);
            MsgSetPaused(data_block);
        }

        if (bytes > 0)
        {
            data_block->bytes_in_frame = bytes_in_frame;
            success = msg_fill_uncompressed_data(data_block, out_ptr, bytes);
            if (MsgIsLastData(data_block) && !MsgIsMoreData(data_block))
                data_block->state = (data_block->state == msg_state_start) ? msg_state_start_and_last : msg_state_last;
        }
    }
done:
    return success;
}

#if (defined IDIGI_COMPRESSION_BUILTIN)
#define MSG_FILL_ZLIB(z, inp, in_len, outp, out_len)  \
    do { \
        z->next_in = inp;\
        z->avail_in = in_len;\
        z->next_out = outp;\
        z->avail_out = out_len;\
        } while (0)

static bool msg_frame_compressed(msg_data_block_t * const data_block, msg_data_info_t const * const info)
{
    bool success = false;
    z_streamp zlib_ptr = &data_block->zlib;
    uint8_t * out_ptr = GET_PACKET_DATA_POINTER(data_block->frame, PACKET_EDP_FACILITY_SIZE);
    size_t const frame_size = sizeof data_block->frame - (out_ptr - data_block->frame);
    size_t chunk_size = frame_size;
    int zret;
    int flush_flag = Z_NO_FLUSH;
    uint32_t const window_size = data_block->available_window;
    size_t data_bytes;

    switch (data_block->state)
    {
    case msg_state_init:
        ASSERT_GOTO(info != NULL, error);
        ASSERT_GOTO(info->header_length > 0, error);

        memset(zlib_ptr, 0, sizeof *zlib_ptr);
        zret = deflateInit(zlib_ptr, Z_DEFAULT_COMPRESSION);
        ASSERT_GOTO(zret == Z_OK, error);
        /* start header */
        out_ptr += MSG_START_HEADER_LEN;
        chunk_size -= MSG_START_HEADER_LEN;
        MSG_FILL_ZLIB(zlib_ptr, (uint8_t *)info->header, info->header_length, out_ptr, chunk_size);
        zret = deflate(zlib_ptr, Z_NO_FLUSH);
        ASSERT_GOTO(zret == Z_OK, error);
        ASSERT_GOTO(zlib_ptr->avail_in == 0, error);
        data_block->state = msg_state_start;
        MsgClearMoreData(data_block);

        data_block->total_message_bytes += chunk_size;
        break;

    case msg_state_start:
    case msg_state_data:
        if (zlib_ptr->avail_out == 0)
        {
            ASSERT_GOTO(data_block->state == msg_state_data, error);
            /* start messaging-data header */
            out_ptr += MSG_DATA_HEADER_LEN;
            chunk_size -= MSG_DATA_HEADER_LEN;
            zlib_ptr->avail_out = chunk_size;
            zlib_ptr->next_out = out_ptr;
/*            MSG_FILL_ZLIB(zlib_ptr, data_block->user_data, data_block->bytes_remaining, out_ptr, chunk_size); */
        }

        {
            /* The max data we can send to server is window size so
             * we don't want to compress data more than window size.
             */
            size_t const unack_count = data_block->total_message_bytes - data_block->ack_count;

            if (window_size < (unack_count + data_block->bytes_remaining))
            {
                /* compress the rest of data that is = window_size
                 * and flush the compression.
                 */
                MsgSetPaused(data_block);
                data_bytes = window_size - unack_count;
                zlib_ptr->next_in = data_block->user_data;
                zlib_ptr->avail_in = data_bytes;
                flush_flag = Z_FULL_FLUSH;
            }
            else
            {
                data_bytes = data_block->bytes_remaining;
                zlib_ptr->next_in = data_block->user_data;
                zlib_ptr->avail_in = data_block->bytes_remaining;
            }
        }
        zret = deflate(zlib_ptr, MsgIsLastData(data_block) ? Z_FINISH : flush_flag);
        ASSERT_GOTO(zret == Z_OK || zret == Z_STREAM_END, error);

        MsgClearMoreData(data_block);
        if (zret == Z_STREAM_END)
        {        
            ASSERT(MsgIsLastData(data_block));
            if (data_block->state == msg_state_start)
            {
                /* is compressed size more than uncompressed? */
                if (zlib_ptr->total_out > zlib_ptr->total_in)
                {
                    success = msg_frame_uncompressed(data_block, info);
                    MsgClearCompression(data_block);
                    goto error;
                }
                data_block->state = msg_state_start_and_last;
            }
            else
            {
                data_block->state = msg_state_last;
            }

            data_block->bytes_in_frame = frame_size - zlib_ptr->avail_out;
        }
        else if (zlib_ptr->avail_out == 0)
        {
            data_block->bytes_in_frame = frame_size;
        }
        else if (flush_flag == Z_FULL_FLUSH)
        {
            data_block->bytes_in_frame = frame_size - zlib_ptr->avail_out;
            zlib_ptr->avail_out = 0;
        }

        if (zlib_ptr->next_in != NULL)
        {
            size_t const bytes_processed = data_bytes - zlib_ptr->avail_in;

            data_block->user_data += bytes_processed;
            data_block->bytes_remaining -= bytes_processed;
            data_block->total_message_bytes += bytes_processed;
        }

        if (data_block->bytes_remaining > 0)
        {
            MsgSetMoreData(data_block);
        }
        break;

    default:
        ASSERT_GOTO(false, error);
        break;
    }

    success = true;

error:
    return success;
}
#endif

static idigi_callback_status_t msg_send_packet(msg_session_t * const session)
{
    msg_data_block_t * const data_block = &session->send_block;
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * ptr = GET_PACKET_DATA_POINTER(data_block->frame, PACKET_EDP_FACILITY_SIZE);

    switch (data_block->state) 
    {
    case msg_state_start:
    case msg_state_start_and_last:
        {
            uint8_t * start_packet = ptr;

            /*
             * -----------------------------------------------------------------
             * |   0    |    1  |   2-3   |    4-5     |    6    |     7 ... n |
             *  ----------------------------------------------------------------
             * | opcode | flags |   Xid   | Service ID | Comp ID |     Data    |
             *  ----------------------------------------------------------------
             */
            message_store_u8(start_packet, opcode, msg_opcode_start);
            {
                uint8_t flag = MsgIsRequest(data_block) ? MSG_FLAG_REQUEST : 0;

                if (data_block->state ==  msg_state_start_and_last)
                    flag |= MSG_FLAG_LAST_DATA;

                message_store_u8(start_packet, flags, flag);
            }
    
            message_store_be16(start_packet, transaction_id, session->session_id);    
            message_store_be16(start_packet, service_id, session->service_id);
            message_store_u8(start_packet, compression_id, (MsgIsCompressed(data_block) ? MSG_COMPRESSION_LIBZ : MSG_NO_COMPRESSION));
    
            data_block->state = msg_state_data;
        }
        break;

    case msg_state_data:
    case msg_state_last:
        {
            uint8_t * data_packet = ptr;

            /*
             * ---------------------------------------
             * |   0    |    1  |   2-3   |  4 ... n |
             *  --------------------------------------
             * | opcode | flags |   Xid   |  Data    |
             *  --------------------------------------
             */
            message_store_u8(data_packet, opcode, msg_opcode_data);
            {

                uint8_t flag = MsgIsRequest(data_block) ? MSG_FLAG_REQUEST : 0;

                if (data_block->state ==  msg_state_last)
                    flag |= MSG_FLAG_LAST_DATA;

                message_store_u8(data_packet, flags, flag);
            }

            message_store_be16(data_packet, transaction_id, session->session_id);
        }
        break;

    default:
        ASSERT(false);
        break;
    } 

    return status;
}

static idigi_callback_status_t msg_send_data(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr,
                                             msg_session_t * const session, msg_data_info_t const * const info)
{
    msg_data_block_t * const data_block = &session->send_block;
    idigi_callback_status_t status = idigi_callback_abort;

    UNUSED_PARAMETER(idigi_ptr);
    ASSERT_GOTO(session != NULL, error);
    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(MsgIsSessionValid(session), error);

    if (info != NULL) 
    {
        if (data_block->bytes_remaining > 0)
        {
            status = idigi_callback_busy;
            goto error;
        }
        data_block->bytes_remaining = info->payload_length;
        data_block->user_data = (uint8_t *)info->payload;
        data_block->payload = info->payload;
        data_block->payload_length = info->payload_length;
        data_block->bytes_in_frame = 0;
        MsgSetCallback(data_block);
        if ((info->flag & IDIGI_DATA_REQUEST_LAST) == IDIGI_DATA_REQUEST_LAST)
            MsgSetLastData(data_block);

        if (info->flag & IDIGI_DATA_REQUEST_COMPRESSED)
        {
#if (defined IDIGI_COMPRESSION_BUILTIN)
            /* in order to send compressed data, server must support compression */
            ASSERT_GOTO(msg_ptr->capabilities[msg_server_request_id].compression_supported, error);

            MsgSetCompression(data_block);
#else
            ASSERT_GOTO(false, error);
#endif
        }
    }

    if (data_block->state == msg_state_init)
    {
        bool success = false; 

#if (defined IDIGI_COMPRESSION_BUILTIN)
        if (MsgIsCompressed(data_block))
        {
            success = msg_frame_compressed(data_block, info);
        }
        else
#endif
        {
            success = msg_frame_uncompressed(data_block, info);
        }

        ASSERT_GOTO(success, error);
    }

    status = idigi_callback_continue;

error:
    return status;
}

static void * msg_send_start(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr,
                            msg_session_t * const session, uint16_t const service_id,
                            msg_data_info_t const * const info)
{
    msg_session_t * new_session = session;

    if (new_session == NULL)
    {
        bool const want_new_id = true;


        idigi_status_t const status = msg_create_session(idigi_ptr, msg_ptr, service_id, want_new_id, &new_session);
        if (status != idigi_success)
        {
            /* don't know how to pass error back to caller */
            new_session = NULL;
            goto error;
        }
        ASSERT_GOTO(new_session != NULL, error);
        MsgSetRequest(&new_session->send_block);
    }
    else
    {
        ASSERT_GOTO(MsgIsSessionValid(new_session), error);
    }

    {
        idigi_callback_status_t const status = msg_send_data(idigi_ptr, msg_ptr, new_session, info);

        ASSERT_GOTO(status == idigi_callback_continue, error);
    }

error:
    return new_session;
}

static void msg_send_complete(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data)
{
    msg_session_t * const session = user_data;
    msg_data_block_t * const data_block = &session->send_block;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    UNUSED_PARAMETER(packet);
    ASSERT_GOTO(session != NULL, done);

    if (status == idigi_success)
    {
        if (MsgIsMoreData(data_block))
        {
            if (!MsgIsPaused(data_block))
            {
                msg_send_data(idigi_ptr, msg_ptr, session, NULL);
            }
            goto done;
        }
    }

    if (MsgIsCallback(data_block) && data_block->bytes_remaining == 0)
    {
        idigi_msg_callback_t * cb_fn = msg_ptr->service_cb[session->service_id];
        size_t const bytes = data_block->payload_length;
        bool const last_chunk = (!MsgIsMoreData(data_block) && MsgIsLastData(data_block));

        ASSERT_GOTO(cb_fn != NULL, done);
        MsgClearCallback(data_block);

        cb_fn(idigi_ptr, msg_state_send_complete, session, data_block->payload, bytes);

        if ((status != idigi_success) || last_chunk)
        {
            /* don't delete the session if it's request since it requires a response */
            if (!MsgIsRequest(data_block))
            {
                msg_delete_session(idigi_ptr, msg_ptr, session);
            }
            else if (msg_ptr->pending == session)
            {
                msg_ptr->pending = (session->prev != NULL) ? session->prev : msg_ptr->session_tail;
            }
        }
    }

done:
    return;
}
static idigi_callback_status_t msg_send_ack(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * ack_packet;
    uint8_t * const edp_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &ack_packet);
    if (edp_packet == NULL)
    {
        goto done;
    }

    message_store_u8(ack_packet, opcode, msg_opcode_ack);
    {
        uint8_t const flag = MsgIsRequest(&session->recv_block) ? MSG_FLAG_REQUEST : 0;

        message_store_u8(ack_packet, flags, flag);
    }
    message_store_be16(ack_packet, transaction_id, session->session_id);
    message_store_be32(ack_packet, ack_count, session->recv_block.total_message_bytes);
    message_store_be32(ack_packet, window_size, msg_ptr->capabilities[msg_client_request_id].window_size);

    status = initiate_send_facility_packet(idigi_ptr, edp_packet, record_end(ack_packet),
                                           E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);
    if (status == idigi_callback_busy)
    {
        session->send_ack = true;
    }
    else
    {
        session->send_ack = false;
    }

done:
    return status;
}

static idigi_callback_status_t msg_update_ack(idigi_data_t * const idigi_ptr,  idigi_msg_data_t * const msg_ptr, msg_session_t * const session, size_t const length)
{
    msg_data_block_t * const data_block = &session->recv_block;
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(idigi_ptr);

    data_block->bytes_remaining += length;
    data_block->total_message_bytes += length;

    if ((data_block->total_message_bytes - data_block->ack_count) > (msg_ptr->capabilities[msg_client_request_id].window_size / 2))
    {
        status = msg_send_ack(idigi_ptr, msg_ptr, session);
        if (status == idigi_callback_continue)
        {
            data_block->bytes_remaining = 0;
        }
    }
    return status;
}

static idigi_callback_status_t msg_process_capabilities(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * capabilty_packet = ptr;
    bool const request_capabilities = (message_load_u8(capabilty_packet, flags) & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST;
    uint8_t const version = message_load_u8(capabilty_packet, version);

    ASSERT_GOTO(version == MSG_FACILITY_VERSION, error);
    msg_fac->capabilities[msg_server_request_id].max_transactions = message_load_u8(capabilty_packet, max_transactions);
    msg_fac->capabilities[msg_server_request_id].window_size = message_load_be32(capabilty_packet, window_size);

    {
        uint8_t const comp_count = message_load_u8(capabilty_packet, compression_count);
        uint8_t const * compression_list = capabilty_packet + record_bytes(capabilty_packet);
        int i;

        msg_fac->capabilities[msg_server_request_id].compression_supported = false;
        for (i = 0; i < comp_count; i++) 
        {
            uint8_t const compression_id = *compression_list++;

            if (compression_id == MSG_COMPRESSION_LIBZ)
            {
                msg_fac->capabilities[msg_server_request_id].compression_supported = true;
                break;
            }
        }
    }

    if (request_capabilities)
    {
        status = msg_send_capabilities(idigi_ptr, msg_fac, MSG_CAPABILITIES_RESPONSE);
    }

error:
    return status;
}

static idigi_callback_status_t msg_data_callback(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, msg_session_t * const session, uint8_t * data_ptr)
{
    idigi_callback_status_t status;
    msg_data_block_t * data_block = &session->recv_block;
    idigi_msg_callback_t * cb_fn = msg_fac->service_cb[session->service_id];

    if (session->send_ack)
    {
        /* here because we're still waiting to be able to
         * send an ACK. Just goto done since data is already
         * passed to the service callback.
         */
        status = msg_send_ack(idigi_ptr, msg_fac, session);
        goto done;
    }

    status = cb_fn(idigi_ptr, data_block->state, session, data_ptr, data_block->bytes_in_frame);

    if (status == idigi_callback_continue)
    {
        bool const last_data = (data_block->state == msg_state_last) || (data_block->state == msg_state_start_and_last);

        /* need to delete the session if error message is sent to cancel the session */

        if (!MsgIsRequest(data_block) && last_data) 
            msg_delete_session(idigi_ptr, msg_fac, session);
        else
        {
            status = msg_update_ack(idigi_ptr, msg_fac, session, data_block->bytes_in_frame);
            data_block->bytes_in_frame = 0;
            if (data_block->state == msg_state_start)
                data_block->state = msg_state_data;
        }
    }

done:
    return status;
}

#if (defined IDIGI_COMPRESSION_BUILTIN)
static idigi_callback_status_t msg_recv_compressed(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, msg_session_t * const session, uint8_t * ptr, uint16_t length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const data_block = &session->recv_block;
    z_streamp zlib_ptr = &data_block->zlib;
    size_t const frame_size = sizeof data_block->frame;

    /* was returned busy last time? */
    if (data_block->bytes_in_frame > 0) 
    {
        status = msg_data_callback(idigi_ptr, msg_fac, session, data_block->frame);
        if ((status != idigi_callback_continue) || (zlib_ptr->avail_in == 0))
            goto error;
    }
    else
    {
        ASSERT_GOTO(zlib_ptr->avail_in == 0, error);
        zlib_ptr->next_in = ptr;
        zlib_ptr->avail_in = length;
    }

    zlib_ptr->next_out = data_block->frame;
    zlib_ptr->avail_out = frame_size;

    do
    {
        int const ret = inflate(zlib_ptr, Z_NO_FLUSH);

        if (ret == Z_BUF_ERROR) /* no more input buffer to process */
        {
            break;
        }

        if ((ret != Z_STREAM_END) && (ret != Z_OK)) 
        {
            break;
        }

        if (zlib_ptr->avail_out != frame_size)
        {
            data_block->bytes_in_frame = frame_size - zlib_ptr->avail_out;

            if (MsgIsLastData(data_block) && (ret == Z_STREAM_END))
                data_block->state = (data_block->state == msg_state_start) ? msg_state_start_and_last : msg_state_last;
            
            status = msg_data_callback(idigi_ptr, msg_fac, session, data_block->frame); 
            if (status != idigi_callback_continue)
                break;

            zlib_ptr->next_out = data_block->frame;
            zlib_ptr->avail_out = frame_size;
        }
    } while (zlib_ptr->avail_in > 0);

error:
    return status;
}
#endif


static idigi_callback_status_t msg_process_data(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * ptr, uint16_t const length, bool const start)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session;
    uint8_t * start_packet = ptr;
    uint8_t const flag = message_load_u8(start_packet, flags);
    uint16_t const session_id = message_load_be16(start_packet, transaction_id);
    bool const request = (flag & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST;
    msg_data_block_t * data_block;
    idigi_msg_error_t error_code = idigi_msg_error_none;
    bool const client_owned = !request;

    if (start)
    {
        session = msg_find_session(msg_fac, session_id, client_owned);

        if (session == NULL)
        {
            uint16_t const service_id = message_load_be16(start_packet, service_id);
            static bool const want_new_id = false;

            if (!request)
            {
                /* Error here we should find the session for this response message from server
                 * unless this session has been canceled.
                 */
                error_code = idigi_msg_error_unknown_session;
                goto error;
            }
            {
                idigi_status_t const ccode = msg_create_session(idigi_ptr, msg_fac, service_id, want_new_id, &session);
                switch (ccode)
                {
                case idigi_success:
                    break;
                case idigi_no_resource:
                    error_code = idigi_msg_error_memory;
                    goto error;
                case idigi_service_busy:
                    /* return busy so the IIK calls back here */
                    status = idigi_callback_busy;
                    goto error;
                default:
                    status = idigi_callback_abort;
                    goto error;
                }
            }
            session->session_id = session_id;
        }

        data_block = &session->recv_block;
        if (request)
            MsgSetRequest(data_block);

        if (data_block->state == msg_state_init)
        {
            uint8_t const compression = message_load_u8(start_packet, compression_id);

            #if (defined IDIGI_COMPRESSION_BUILTIN)
            if (compression == MSG_COMPRESSION_LIBZ)
            {
                z_streamp zlib_ptr = &data_block->zlib;

                memset(zlib_ptr, 0, sizeof *zlib_ptr);
                {
                    int const ret = inflateInit(zlib_ptr);
    
                    if (ret != Z_OK)
                    {
                        error_code = idigi_msg_error_decompression_failure;
                        ASSERT(false);
                        goto error;
                    }
                }

                MsgSetCompression(data_block);
                if (request)
                    MsgSetCompression(&session->send_block);
            }
            #else
            if (compression != MSG_NO_COMPRESSION)
            {
                error_code = idigi_msg_error_decompression_failure;
                goto error;
            }
            #endif
    
            data_block->state = msg_state_start;
        }
    }
    else
    {
        session = msg_find_session(msg_fac, session_id, client_owned);
        if (session == NULL)
        {
            error_code = idigi_msg_error_unknown_session;
            goto error;
        }
        data_block = &session->recv_block;
        if (data_block->bytes_in_frame == 0)
            data_block->state = msg_state_data;
    }

    if ((flag & MSG_FLAG_LAST_DATA) == MSG_FLAG_LAST_DATA)
    {
        MsgSetLastData(data_block);
    }

    {
        size_t const pkt_length = start ? record_bytes(start_packet) : record_bytes(data_packet); 
        uint8_t *data_ptr = start_packet + pkt_length;
        size_t const data_length = length - pkt_length;

        #if (defined IDIGI_COMPRESSION_BUILTIN)
        if (MsgIsCompressed(data_block)) 
            status = msg_recv_compressed(idigi_ptr, msg_fac, session, data_ptr, data_length);
        else
        #endif
        {
            if (MsgIsLastData(data_block))
                data_block->state = start ? msg_state_start_and_last : msg_state_last;

            data_block->bytes_in_frame = data_length;
            status = msg_data_callback(idigi_ptr, msg_fac, session, data_ptr);
        }
    }

error:
    if (error_code != idigi_msg_error_none)
    {

        status = msg_send_error(idigi_ptr, session, session_id, error_code, flag);
    }
    return status;
}

static idigi_callback_status_t msg_process_ack(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t const * ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session;
    uint8_t const * ack_packet = ptr;

    {
        uint16_t const session_id = message_load_be16(ack_packet, transaction_id);
        uint8_t const flag = message_load_u8(ack_packet, flags);

        bool const client_owned = (flag & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST;

        session = msg_find_session(msg_fac, session_id, client_owned);
        if (session == NULL)
            goto error; /* already done sending all data */    
    }

    {
        msg_data_block_t * const data_block = &session->send_block;
        uint32_t const window_size = message_load_be32(ack_packet, window_size);

        msg_fac->capabilities[msg_server_request_id].window_size = window_size;

        data_block->ack_count = message_load_be32(ack_packet, ack_count);
        data_block->available_window = window_size;

        if (MsgIsPaused(data_block))
        {
            MsgClearPaused(data_block);
            if (MsgIsMoreData(data_block) && data_block->bytes_in_frame == 0)
            {
                status = msg_send_data(idigi_ptr, msg_fac, session, NULL);
            }
        }
    }

error:
    return status;
}

static idigi_callback_status_t msg_process_error(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * error_packet = ptr;
   
    {
#define ERROR_SERVER_RESPONSE   0x01
#define ERROR_CLIENT_REQUEST    0x10
#define ERROR_CLIENT_OWNED_REQUEST    (ERROR_SERVER_RESPONSE | ERROR_CLIENT_REQUEST)

        /* request flag  |  message sender flag
         *  0 | 0 error-message for client's response message (server request session)
         *  0 | 1 error-message for server's response message (client request session)
         *  1 | 0 error-message for client's request message (client request session)
         *  1 | 1 error-message for server's request message (server request session)
         */
        uint8_t const flag = (message_load_u8(error_packet, flags) & ERROR_CLIENT_OWNED_REQUEST);
        bool const client_owned = (flag == ERROR_CLIENT_REQUEST || flag == ERROR_SERVER_RESPONSE);
        uint16_t const session_id = message_load_be16(error_packet, transaction_id);
        msg_session_t * const session = msg_find_session(msg_fac, session_id, client_owned);

        if (session != NULL)
        {
            idigi_msg_callback_t * cb_fn = msg_fac->service_cb[session->service_id];
            uint8_t const error = message_load_u8(error_packet, error_code); 

            status = cb_fn(idigi_ptr, msg_state_error, session, &error, sizeof error);
            msg_delete_session(idigi_ptr, msg_fac, session);
        }
        else
        {
            DEBUG_PRINTF("msg_process_error: unable to find session id = %d\n", session_id);
        }
    }

    return status;
}

static idigi_callback_status_t msg_discovery(idigi_data_t * const idigi_ptr, void * const facility_data, uint8_t * const packet)
{
    UNUSED_PARAMETER(packet);

    return msg_send_capabilities(idigi_ptr, facility_data, MSG_FLAG_REQUEST);
}

static idigi_callback_status_t msg_process(idigi_data_t * const idigi_ptr, void * const facility_data, uint8_t * const edp_header)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * const msg_ptr = facility_data;
    uint8_t * ptr = GET_PACKET_DATA_POINTER(edp_header, PACKET_EDP_FACILITY_SIZE);
    uint16_t const length = message_load_be16(edp_header, length);    
    uint8_t const opcode = *ptr;

    ASSERT_GOTO(edp_header != NULL, error);

    switch (opcode)
    {
        case msg_opcode_capability:
            status = msg_process_capabilities(idigi_ptr, msg_ptr, ptr);
            break;

        case msg_opcode_start:
            status = msg_process_data(idigi_ptr, msg_ptr, ptr, length, true);
            break;

        case msg_opcode_data:
            status = msg_process_data(idigi_ptr, msg_ptr, ptr, length, false);
            break;

        case msg_opcode_ack:
            status = msg_process_ack(idigi_ptr, msg_ptr, ptr);
            break;

        case msg_opcode_error:
            status = msg_process_error(idigi_ptr, msg_ptr, ptr);
            break;

        default:            
            DEBUG_PRINTF("msg_process: Invalid opcode\n");
            break;
    }

error:
    return status;
}

static idigi_callback_status_t msg_process_pending(idigi_data_t * const idigi_ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(msg_ptr != NULL, done);

    if (msg_ptr->pending == NULL)
    {
        msg_ptr->pending = msg_ptr->session_tail;
    }

    if (msg_ptr->pending != NULL)
    {
        msg_session_t * const session = msg_ptr->pending;
        msg_data_block_t * const data_block = &session->send_block;

        if (data_block->bytes_remaining > 0 && !MsgIsPaused(data_block))
        {
            bool success = false;

    #if (defined IDIGI_COMPRESSION_BUILTIN)
            if (MsgIsCompressed(data_block))
            {
                success = msg_frame_compressed(data_block, NULL);
            }
            else
    #endif
            {
                success = msg_frame_uncompressed(data_block, NULL);
            }
            ASSERT_GOTO(success, done);
        }

        if (data_block->bytes_in_frame > 0)
        {

            msg_send_packet(session);

            status = initiate_send_facility_packet(idigi_ptr, data_block->frame, data_block->bytes_in_frame, E_MSG_FAC_MSG_NUM, msg_send_complete, session);
            if (status == idigi_callback_continue)
            {
                data_block->bytes_in_frame = 0;

                /* set pending to next session and return busy to continue sending next session */
                if (!MsgIsMoreData(data_block))
                {
                    msg_ptr->pending = (session->prev != NULL) ? session->prev : msg_ptr->session_tail;
                }
            }
            else
            {
                DEBUG_PRINTF("msg_process_pending: status = %d session %p id =%d bytes_in_frame %d payload length = %d.\n", status,
                        session, session->session_id, data_block->bytes_in_frame, data_block->payload_length);
            }
        }
        else
        {
            msg_send_complete(idigi_ptr, NULL, idigi_success, session);
        }

    }

done:
    return status;
}

static void msg_delete_all_sessions(idigi_data_t * const idigi_ptr,  idigi_msg_data_t * const msg_ptr, uint16_t const service_id)
{
    msg_session_t * session = msg_ptr->session_head;

    while(session != NULL)
    {
        msg_session_t * next_session = session->next;

        if (session->service_id == service_id )
            msg_delete_session(idigi_ptr, msg_ptr, session);
        session = next_session;
    }
}

static idigi_callback_status_t msg_cleanup_all_sessions(idigi_data_t * const idigi_ptr, uint16_t const service_id)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session = msg_ptr->session_head;

    while(session != NULL)
    {
        msg_session_t * next_session = session->next;

        if (session->service_id == service_id)
        {
            idigi_msg_callback_t * cb_fn = msg_ptr->service_cb[session->service_id];
            uint8_t const error = idigi_msg_error_cancel;

            cb_fn(idigi_ptr, msg_state_error, session, &error, sizeof error);
            msg_delete_session(idigi_ptr, msg_ptr, session);
        }
        session = next_session;
    }

    return status;
}

static idigi_callback_status_t msg_delete_facility(idigi_data_t * const idigi_ptr, uint16_t const service_id)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(service_id < msg_service_id_count, error);
    ASSERT_GOTO(msg_ptr->service_cb[service_id] != NULL, error);

    msg_ptr->service_cb[service_id] = NULL;

    /* is list empty? */
    {
        bool is_empty = true;
        int i;

        for(i = 0; i < msg_service_id_count; i++) 
        {
            if (msg_ptr->service_cb[i] != NULL)
            {
                is_empty = false;
                break;
            }
        }

        msg_delete_all_sessions(idigi_ptr, msg_ptr, service_id);
        if (is_empty)
        {
            MutexLockDelete(idigi_ptr, msg_ptr->lock_handle);
            status = del_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
        }
        else
        {
            status = idigi_callback_continue;
        }
    }

error:
    return status;
}

static uint16_t msg_init_facility(idigi_data_t * const idigi_ptr, uint16_t service_id, idigi_msg_callback_t callback)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    if (msg_ptr == NULL)
    {
        void * fac_ptr = NULL;

        status = add_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM, &fac_ptr, sizeof *msg_ptr, msg_discovery, msg_process);

        ASSERT_GOTO(status == idigi_callback_continue, done);
        ASSERT_GOTO(fac_ptr != NULL, done);

        msg_ptr = fac_ptr;
        memset(msg_ptr, 0, sizeof *msg_ptr);

        {
            idigi_request_t const request_id = {idigi_os_lock_init};

            status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_operating_system, request_id,
                                                    &msg_ptr->lock_handle, NULL);
            switch (status)
            {
            case idigi_callback_unrecognized:
                msg_ptr->lock_handle = NULL;
                break;
            case idigi_callback_continue:
                break;
            default:
                /* set error code if it has not been set */
                idigi_ptr->error_code = idigi_configuration_error;
                goto done;
            }
        }

#if (defined IDIGI_COMPRESSION_BUILTIN)
        msg_ptr->capabilities[msg_client_request_id].compression_supported = true;
#endif
    }

    {
        idigi_request_t const request_id = {idigi_data_service_max_transactions};

        status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_data_service, request_id,
                                                &msg_ptr->capabilities[msg_client_request_id].max_transactions, NULL);
        if (status != idigi_callback_continue && status != idigi_callback_unrecognized)
        {
            /* set error code if it has not been set */
            if (idigi_ptr->error_code == idigi_success)
            {
                idigi_ptr->error_code = idigi_configuration_error;
            }
            goto done;
        }
    }

    msg_ptr->capabilities[msg_client_request_id].window_size = MSG_RECV_WINDOW_SIZE;

    msg_ptr->service_cb[service_id] = callback;
    status = idigi_callback_continue;

done:
    return status;
}


