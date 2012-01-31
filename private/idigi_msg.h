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

#if (defined IDIGI_COMPRESSION)
#include "zlib.h"
#endif

#define MSG_FACILITY_VERSION  0x01
#define MSG_COMPRESSION_NONE  0x00
#define MSG_COMPRESSION_LIBZ  0xFF

#define MSG_INVALID_CLIENT_SESSION  0xFFFF

#define MSG_FLAG_REQUEST      0x01
#define MSG_FLAG_LAST_DATA    0x02
#define MSG_FLAG_SENDER       0x04

#define MSG_FLAG_RECEIVING    0x100
#define MSG_FLAG_COMPRESSED   0x200
#define MSG_FLAG_START        0x4000
#define MSG_FLAG_ACK_PENDING  0x8000
#define MSG_FLAG_CLIENT_OWNED 0x10000

#define MsgIsBitSet(flag, bit)  (((flag) & (bit)) == (bit))
#define MsgBitSet(flag, bit)    ((flag) |= (bit))
#define MsgBitClear(flag, bit)  ((flag) &= ~(bit))

#define MsgIsRequest(flag)      MsgIsBitSet((flag), MSG_FLAG_REQUEST)
#define MsgIsLastData(flag)     MsgIsBitSet((flag), MSG_FLAG_LAST_DATA)
#define MsgIsSender(flag)       MsgIsBitSet((flag), MSG_FLAG_SENDER)
#define MsgIsReceiving(flag)    MsgIsBitSet((flag), MSG_FLAG_RECEIVING)
#define MsgIsCompressed(flag)   MsgIsBitSet((flag), MSG_FLAG_COMPRESSED)
#define MsgIsStart(flag)        MsgIsBitSet((flag), MSG_FLAG_START)
#define MsgIsAckPending(flag)   MsgIsBitSet((flag), MSG_FLAG_ACK_PENDING)
#define MsgIsClientOwned(flag)  MsgIsBitSet((flag), MSG_FLAG_CLIENT_OWNED)

#define MsgSetRequest(flag)     MsgBitSet((flag), MSG_FLAG_REQUEST)
#define MsgSetLastData(flag)    MsgBitSet((flag), MSG_FLAG_LAST_DATA)
#define MsgSetSender(flag)      MsgBitSet((flag), MSG_FLAG_SENDER)
#define MsgSetReceiving(flag)   MsgBitSet((flag), MSG_FLAG_RECEIVING)
#define MsgSetCompression(flag) MsgBitSet((flag), MSG_FLAG_COMPRESSED)
#define MsgSetStart(flag)       MsgBitSet((flag), MSG_FLAG_START)
#define MsgSetAckPending(flag)  MsgBitSet((flag), MSG_FLAG_ACK_PENDING)
#define MsgSetClientOwned(flag) MsgBitSet((flag), MSG_FLAG_CLIENT_OWNED)

#define MsgClearRequest(flag)     MsgBitClear((flag), MSG_FLAG_REQUEST)
#define MsgClearLastData(flag)    MsgBitClear((flag), MSG_FLAG_LAST_DATA)
#define MsgClearReceiving(flag)   MsgBitClear((flag), MSG_FLAG_RECEIVING)
#define MsgClearStart(flag)       MsgBitClear((flag), MSG_FLAG_START)
#define MsgClearAckPending(flag)  MsgBitClear((flag), MSG_FLAG_ACK_PENDING)
#define MsgClearCompression(flag) MsgBitClear((flag), MSG_FLAG_COMPRESSED)

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
    msg_opcode_error
} msg_opcode_t;

typedef enum
{
    msg_block_state_send_request,
    msg_block_state_recv_request,
    msg_block_state_send_response,
    msg_block_state_recv_response
} msg_block_state_t;

typedef enum
{
    msg_state_init,
    msg_state_get_data,
    msg_state_compress,
    msg_state_send_compressed,
    msg_state_wait_send_complete,
    msg_state_wait_ack,
    msg_state_receive,
    msg_state_decompress,
    msg_state_process_decompressed,
    msg_state_send_ack,
    msg_state_send_error,
    msg_state_delete
} msg_state_t;

typedef enum
{
    msg_service_type_need_data,
    msg_service_type_have_data,
    msg_service_type_error,
    msg_service_type_free
} msg_service_type_t;

typedef enum 
{
    msg_capability_server,
    msg_capability_client,
    msg_capability_count
} msg_capability_type_t;

enum msg_capability_packet_t
{
    field_define(capability_packet, opcode, uint8_t),
    field_define(capability_packet, flags, uint8_t),
    field_define(capability_packet, version, uint8_t),
    field_define(capability_packet, max_transactions, uint8_t),
    field_define(capability_packet, window_size, uint32_t),
    field_define(capability_packet, compression_count, uint8_t),
    record_end(capability_packet)
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

typedef struct msg_data_block_t
{
    size_t total_bytes;
    size_t available_window;
    size_t ack_count;
#if (defined IDIGI_COMPRESSION)
    uint8_t  buffer_in[MSG_MAX_RECV_PACKET_SIZE];
    uint8_t  buffer_out[MSG_MAX_RECV_PACKET_SIZE];
    size_t   bytes_out;
    int      z_flag;
    z_stream zlib;
#endif
} msg_data_block_t;

typedef struct msg_session_t
{
    unsigned int session_id;
    unsigned int service_id;
    void * service_context;
    unsigned int status_flag;
    msg_data_block_t data_block;
    msg_state_t state;
    idigi_msg_error_t error;
    unsigned int error_flag;
    struct msg_session_t * next;
    struct msg_session_t * prev;
} msg_session_t;

typedef struct
{
    msg_session_t * session;
    msg_service_type_t service_type;
    void * data_ptr;
    size_t length_in_bytes;
    unsigned int flags;
    idigi_msg_error_t error_value;
} msg_service_request_t;

typedef idigi_callback_status_t idigi_msg_callback_t(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data);

typedef struct 
{
    uint32_t window_size;
    unsigned int active_transactions;
    unsigned int max_transactions;
    idigi_bool_t compression_supported;
} msg_capabilities_t;

typedef struct
{
    msg_capabilities_t capabilities[msg_capability_count];
    idigi_msg_callback_t * service_cb[msg_service_id_count];
    idigi_bool_t session_locked;
    struct
    {
        msg_session_t * head;
        msg_session_t * tail;
        msg_session_t * current;
    } session;
    unsigned int last_assigned_id;
} idigi_msg_data_t;

static msg_session_t * msg_find_session(idigi_msg_data_t const * const msg_ptr, unsigned int const id, idigi_bool_t const client_owned)
{
    msg_session_t * session = msg_ptr->session.head;

    while (session != NULL)
    {
        if (session->session_id == id)
        {
            if (MsgIsClientOwned(session->status_flag) == client_owned)
                break;
        }

        session = session->next;
    }    

    return session;
}

static uint16_t msg_find_next_available_id(idigi_msg_data_t * const msg_ptr)
{
    unsigned int new_id = MSG_INVALID_CLIENT_SESSION;
    unsigned int const last_id = msg_ptr->last_assigned_id;

    do
    {
        idigi_bool_t const client_owned = idigi_true;

        msg_ptr->last_assigned_id++;
        if (msg_ptr->last_assigned_id >= MSG_INVALID_CLIENT_SESSION)
        {
            msg_ptr->last_assigned_id = 0;
            continue;
        }

        if (msg_find_session(msg_ptr, msg_ptr->last_assigned_id, client_owned) == NULL)
        {
            new_id = msg_ptr->last_assigned_id;
            break;
        }

    } while (msg_ptr->last_assigned_id != last_id);

    return new_id;
}

static void msg_set_error(msg_session_t * const session, idigi_msg_error_t const error_code)
{
    idigi_bool_t const client_request_error = MsgIsClientOwned(session->status_flag) && !MsgIsReceiving(session->status_flag);
    idigi_bool_t const client_response_error = !MsgIsClientOwned(session->status_flag) && !MsgIsReceiving(session->status_flag);
    idigi_bool_t const server_request_error = !MsgIsClientOwned(session->status_flag) && MsgIsReceiving(session->status_flag);

    if (client_request_error && MsgIsStart(session->status_flag))
    {
        /* no need to send an error. just delete since nothing has been sent to server */
        session->state = msg_state_delete;
        goto done;
    }

    session->error = error_code;
    session->state = msg_state_send_error;

    if (client_response_error && MsgIsStart(session->status_flag))
    {
        /* canceling server request since no response data has been sent yet */
        MsgSetRequest(session->error_flag);
        goto done;
    }

    if (client_request_error || server_request_error)
        MsgSetRequest(session->error_flag);

    if (client_request_error || client_response_error)
        MsgSetSender(session->error_flag);

done:
    return;
}

static idigi_callback_status_t msg_call_service_layer(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_ptr, msg_session_t * const session, msg_service_type_t const type, void * const data, size_t const bytes, unsigned int status_flag)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    idigi_msg_callback_t * cb_fn = msg_ptr->service_cb[session->service_id];

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(cb_fn != NULL, error);

    service_ptr->session = session;
    service_ptr->service_type = type;
    service_ptr->data_ptr = data;
    service_ptr->length_in_bytes = bytes;
    service_ptr->flags = status_flag;

    status = cb_fn(idigi_ptr, service_ptr);
    if ((status == idigi_callback_continue) && (service_ptr->service_type != type))
    {

        if (MsgIsStart(session->status_flag) && MsgIsClientOwned(session->status_flag))
        {
            session->state = msg_state_delete;
        }
        else
        {
            msg_set_error(session, service_ptr->error_value);
        }
        status = idigi_callback_unrecognized;
    }

error:
    return status;
}

static idigi_callback_status_t msg_inform_error(idigi_data_t * const idigi_ptr, msg_session_t * const session, idigi_msg_error_t error_code)
{
    msg_service_request_t service_data;

    msg_set_error(session, error_code);
    return msg_call_service_layer(idigi_ptr, &service_data, session, msg_service_type_error, &error_code, sizeof error_code, 0);
}

static msg_session_t * msg_create_session(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, unsigned int const service_id, 
                                          idigi_bool_t const client_owned, idigi_msg_error_t * const result)
{
    unsigned int session_id = MSG_INVALID_CLIENT_SESSION;
    msg_capability_type_t const capability_id = client_owned ? msg_capability_server : msg_capability_client;
    msg_session_t * session = NULL;

    ASSERT_GOTO(result != NULL, done);
    *result = idigi_msg_error_memory;
    ASSERT_GOTO(msg_ptr != NULL, done);

    {
        unsigned int const max_transactions = msg_ptr->capabilities[capability_id].max_transactions;
        unsigned int const active_transactions = msg_ptr->capabilities[capability_id].active_transactions;
        unsigned int const unlimited_transactions = 0;

        if ((max_transactions != unlimited_transactions) && (active_transactions >= max_transactions))
        {
            idigi_debug("msg_create_session: active transactions %d > max transactions %d\n", active_transactions, max_transactions);
            ASSERT(client_owned); /* server should not send more than client's max transactions */
            goto done;
        }
    }

    if (client_owned)
    {
        session_id = msg_find_next_available_id(msg_ptr);
        if (session_id == MSG_INVALID_CLIENT_SESSION) goto done;
    }

    {
        void * ptr;
        idigi_callback_status_t const status = malloc_data(idigi_ptr, sizeof *session, &ptr);

        if (status != idigi_callback_continue) goto done;
        session = ptr;
    }

    session->session_id = session_id;
    session->service_id = service_id;
    session->error = idigi_msg_error_none;
    session->error_flag = 0;
    session->service_context = NULL;
    session->state = msg_state_init;

    session->status_flag = 0;
    MsgSetRequest(session->status_flag);
    if (client_owned) MsgSetClientOwned(session->status_flag);

    if (msg_ptr->session_locked) goto error;
    msg_ptr->session_locked = idigi_true;
    add_list_node(&msg_ptr->session.head, &msg_ptr->session.tail, session);
    msg_ptr->session_locked = idigi_false;

    msg_ptr->capabilities[capability_id].active_transactions++;
    *result = idigi_msg_error_none;
    goto done;

error:
    *result = idigi_msg_error_busy;
    free_data(idigi_ptr, session);
    session = NULL;

done:
    return session;
}

static void msg_delete_session(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, msg_session_t * const session)
{
    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(session != NULL, error);
    
    if (msg_ptr->session_locked) goto error;
    msg_ptr->session_locked = idigi_true;
    remove_list_node(&msg_ptr->session.head, &msg_ptr->session.tail, session);
    if (msg_ptr->session.current == session)
        msg_ptr->session.current = (session->prev != NULL) ? session->prev : msg_ptr->session.tail;
    msg_ptr->session_locked = idigi_false;

    #if (defined IDIGI_COMPRESSION)
    {
        int(*func)(z_streamp strm) = MsgIsReceiving(session->status_flag) ? inflateEnd : deflateEnd;

        func(&session->data_block.zlib);
    }
    #endif

    {
        msg_capability_type_t const capability_id = MsgIsClientOwned(session->status_flag) ? msg_capability_server : msg_capability_client;

        ASSERT_GOTO(msg_ptr->capabilities[capability_id].active_transactions > 0, error);
        msg_ptr->capabilities[capability_id].active_transactions--;
    }

    {
        msg_service_request_t service_data;

        msg_call_service_layer(idigi_ptr, &service_data, session, msg_service_type_free, NULL, 0, 0);
    }

    free_data(idigi_ptr, session);

error:
    return;
}

#if (defined IDIGI_COMPRESSION)
static idigi_msg_error_t msg_initialize_zlib(z_streamp zlib_ptr, msg_block_state_t state)
{
    int zret;

    switch(state)
    {
    case msg_block_state_send_response:
        inflateEnd(zlib_ptr);
        /* no break */

    case msg_block_state_send_request:
        memset(zlib_ptr, 0, sizeof *zlib_ptr);
        zret = deflateInit(zlib_ptr, Z_DEFAULT_COMPRESSION);
        break;

    case msg_block_state_recv_response:
        deflateEnd(zlib_ptr);
        /* no break */

    case msg_block_state_recv_request:
        memset(zlib_ptr, 0, sizeof *zlib_ptr);
        zret = inflateInit(zlib_ptr);
        break;

    default:
        zret = Z_ERRNO;
        ASSERT(idigi_false);
        break;
    }

    return (zret == Z_OK) ? idigi_msg_error_none : idigi_msg_error_compression_failure;
}
#endif

static idigi_msg_error_t msg_initialize_data_block(msg_session_t * const session, uint32_t const window_size, msg_block_state_t state)
{
    msg_data_block_t * const dblock = &session->data_block;
    idigi_msg_error_t result = idigi_msg_error_none;

    ASSERT_GOTO(session != NULL, error);
    MsgSetStart(session->status_flag);
    MsgClearLastData(session->status_flag);
    dblock->ack_count = 0;
    dblock->total_bytes = 0;

    switch(state) 
    {
    case msg_block_state_send_response:
    case msg_block_state_send_request:
        dblock->available_window = window_size;
        MsgClearReceiving(session->status_flag);
        session->state = msg_state_get_data;
        break;

    case msg_block_state_recv_response:
    case msg_block_state_recv_request:
        dblock->available_window = window_size;
        MsgSetReceiving(session->status_flag);
        session->state = msg_state_receive;
        break;

    default:
        ASSERT_GOTO(idigi_false, error);
        break;
    }

#if (defined IDIGI_COMPRESSION)
    dblock->bytes_out = 0;
    dblock->z_flag = Z_NO_FLUSH;
    result = msg_initialize_zlib(&dblock->zlib, state);
    if (result == idigi_msg_error_none) 
        MsgSetCompression(session->status_flag);
#endif

error:
    return result;
}

static idigi_callback_status_t msg_send_error(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, msg_session_t * const session, uint16_t const session_id, idigi_msg_error_t const error_value, uint8_t const flag)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * error_packet;
    uint8_t * const edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &error_packet, NULL);

    if (edp_header == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    message_store_u8(error_packet, opcode, msg_opcode_error);
    message_store_u8(error_packet, flags, flag);
    message_store_be16(error_packet, transaction_id, session_id);
    message_store_u8(error_packet, error_code, error_value);

    status = initiate_send_facility_packet(idigi_ptr, edp_header, record_end(error_packet), E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);
    if ((status != idigi_callback_busy) && (session != NULL))
        msg_delete_session(idigi_ptr, msg_ptr, session);

done:
    return status;
}

static idigi_callback_status_t msg_send_capabilities(idigi_data_t * const idigi_ptr, idigi_msg_data_t const * const msg_ptr, uint8_t const flag)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * capability_packet = NULL;
    uint8_t * const edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &capability_packet, NULL);
    size_t packet_len = record_bytes(capability_packet);
    uint8_t * variable_data_ptr = capability_packet + packet_len;

    if ((edp_header == NULL) || (capability_packet == NULL))
        goto error;

    /*
     * ------------------------------------------------------------------------------------------
     * |   0    |    1  |     2   |    3    |   4-7  |     8    | 9-(8+n) |     9+n   | s bytes |
     *  -----------------------------------------------------------------------------------------
     * | opcode | flags | version | Max Trx | Window | Count(n) | list    | Count (s) |  list   |
     * |        |       |         |         |  size  |  compression algo  |     Service Info    |
     *  -----------------------------------------------------------------------------------------
     */
    message_store_u8(capability_packet, opcode, msg_opcode_capability);
    message_store_u8(capability_packet, flags, flag);
    message_store_u8(capability_packet, version, MSG_FACILITY_VERSION);
    message_store_u8(capability_packet, max_transactions, msg_ptr->capabilities[msg_capability_client].max_transactions);
    message_store_be32(capability_packet, window_size, msg_ptr->capabilities[msg_capability_client].window_size);

    /* append compression algorithms supported */
    {
        uint8_t const compression_length = (msg_ptr->capabilities[msg_capability_client].compression_supported) ? 1 : 0;

        message_store_u8(capability_packet, compression_count, compression_length);
        if (compression_length > 0)
            *variable_data_ptr++ = MSG_COMPRESSION_LIBZ;
    }

    /* append service IDs of all listeners */
    {
        uint16_t services = 0;
        uint16_t service_id;

        for (service_id = 0; service_id < msg_service_id_count; service_id++) 
            if (msg_ptr->service_cb[service_id] !=  NULL) services++;

        StoreBE16(variable_data_ptr, services);
        variable_data_ptr += sizeof services;

        for (service_id = 0; service_id < msg_service_id_count; service_id++) 
        {
            if (msg_ptr->service_cb[service_id] !=  NULL)
            {
                StoreBE16(variable_data_ptr, service_id);
                variable_data_ptr += sizeof service_id;
            }
        }
    }

    packet_len = variable_data_ptr - capability_packet;
    status = initiate_send_facility_packet(idigi_ptr, edp_header, packet_len, E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);

error:
    return status;
}

static void msg_fill_msg_header(msg_session_t * const session, void * ptr)
{
    uint8_t flag = 0;

    if MsgIsRequest(session->status_flag)
        MsgSetRequest(flag);

    if (MsgIsLastData(session->status_flag))
        MsgSetLastData(flag);

    if (MsgIsStart(session->status_flag))
    {
        uint8_t * const start_packet = ptr;

        MsgClearStart(session->status_flag);

        /*
        * -----------------------------------------------------------------
        * |   0    |    1  |   2-3   |    4-5     |    6    |     7 ... n |
        *  ----------------------------------------------------------------
        * | opcode | flags |   Xid   | Service ID | Comp ID |     Data    |
        *  ----------------------------------------------------------------
        */
        message_store_u8(start_packet, opcode, msg_opcode_start);
        message_store_u8(start_packet, flags, flag);
        message_store_be16(start_packet, transaction_id, session->session_id);
        message_store_be16(start_packet, service_id, session->service_id);
        message_store_u8(start_packet, compression_id, (MsgIsCompressed(session->status_flag) ? MSG_COMPRESSION_LIBZ : MSG_COMPRESSION_NONE));
    }
    else
    {
        uint8_t * const data_packet = ptr;

        /*
         * ---------------------------------------
         * |   0    |    1  |   2-3   |  4 ... n |
         *  --------------------------------------
         * | opcode | flags |   Xid   |  Data    |
         *  --------------------------------------
         */
        message_store_u8(data_packet, opcode, msg_opcode_data);
        message_store_u8(data_packet, flags, flag);
        message_store_be16(data_packet, transaction_id, session->session_id);
    }
}

#if (defined IDIGI_COMPRESSION)
static void msg_send_complete(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data)
{
    msg_session_t * const session = user_data;

    UNUSED_PARAMETER(packet);
    ASSERT_GOTO(session != NULL, error);

    if (status != idigi_success)
    {
        msg_inform_error(idigi_ptr, session, idigi_send_error);
        goto error;
    }

    /* update session state */
    if (MsgIsLastData(session->status_flag))
    {
        session->state = MsgIsRequest(session->status_flag) ? msg_state_receive : msg_state_delete;
        goto done;
    }

    {
        msg_data_block_t * const dblock = &session->data_block;
        z_streamp zlib_ptr = &dblock->zlib;

        if (zlib_ptr->avail_out == 0)
        {
            session->state = msg_state_compress;
            goto done;
        }

        dblock->z_flag = Z_NO_FLUSH;
        session->state = MsgIsAckPending(session->status_flag) ? msg_state_wait_ack : msg_state_get_data;
        zlib_ptr->avail_out = 0;
    }

error:
done:
    return;
}

static idigi_callback_status_t msg_send_compressed_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = &session->data_block;

    ASSERT_GOTO(dblock->bytes_out > 0, error);

    status = initiate_send_facility_packet(idigi_ptr, dblock->buffer_out, dblock->bytes_out, E_MSG_FAC_MSG_NUM, msg_send_complete, session);
    switch (status)
    {
    case idigi_callback_busy:
        session->state = msg_state_send_compressed;
        break;

    case idigi_callback_continue:
        dblock->bytes_out = 0;
        session->state = msg_state_wait_send_complete;
        break;

    default:
        msg_inform_error(idigi_ptr, session, idigi_send_error);
        break;
    }

error:
    return status;
}

static idigi_callback_status_t msg_compress_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const dblock = &session->data_block;
    uint8_t * const msg_buffer = GET_PACKET_DATA_POINTER(dblock->buffer_out, PACKET_EDP_FACILITY_SIZE);
    size_t const frame_bytes = sizeof dblock->buffer_out - PACKET_EDP_FACILITY_SIZE;
    z_streamp zlib_ptr = &dblock->zlib;
    int zret;

    if (zlib_ptr->avail_out == 0)
    {
        size_t const header_length = MsgIsStart(session->status_flag) ? record_end(start_packet) : record_end(data_packet);

        zlib_ptr->next_out = msg_buffer + header_length;
        zlib_ptr->avail_out = frame_bytes - header_length;
    }

    zret = deflate(zlib_ptr, dblock->z_flag);
    switch (zret) 
    {
    case Z_OK:
        if ((dblock->z_flag != Z_SYNC_FLUSH) && (zlib_ptr->avail_out > 0))
            goto done;
        break;

    case Z_STREAM_END:
        MsgSetLastData(session->status_flag);
        break;

    default:
        status = msg_inform_error(idigi_ptr, session, idigi_msg_error_compression_failure);
        goto done;
    }

    msg_fill_msg_header(session, msg_buffer);
    dblock->bytes_out = frame_bytes - zlib_ptr->avail_out;
    status = msg_send_compressed_data(idigi_ptr, session);

done:
    return status;
}

static idigi_callback_status_t msg_get_service_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = &session->data_block;
    z_streamp zlib_ptr = &dblock->zlib;

    ASSERT_GOTO(!MsgIsLastData(session->status_flag), done);

    if (zlib_ptr->avail_in == 0)
    {
        msg_service_request_t service_data;
        unsigned int const flag = (dblock->total_bytes == 0) ? MSG_FLAG_START : 0;

        status = msg_call_service_layer(idigi_ptr, &service_data, session, msg_service_type_need_data, dblock->buffer_in, sizeof dblock->buffer_in, flag);
        if (status != idigi_callback_continue)
            goto done;
    
        zlib_ptr->next_in = dblock->buffer_in;
        zlib_ptr->avail_in = service_data.length_in_bytes;
        dblock->total_bytes += service_data.length_in_bytes;
        if (MsgIsLastData(service_data.flags))
            dblock->z_flag = Z_FINISH;
    }

    if (dblock->z_flag == Z_NO_FLUSH)
    {
        size_t const unacked_bytes = dblock->total_bytes - dblock->ack_count;
        size_t const safe_window_size = dblock->available_window - sizeof dblock->buffer_in;

        if (unacked_bytes > safe_window_size)
        {
            dblock->z_flag = Z_SYNC_FLUSH;
            MsgSetAckPending(session->status_flag);
        }
    }

    status = msg_compress_data(idigi_ptr, session);

done:
    return status;
}

#else

static idigi_callback_status_t msg_get_service_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    uint8_t * msg_buffer;
    size_t bytes_in_buffer;
    uint8_t * const edp_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &msg_buffer, &bytes_in_buffer);
    size_t const header_bytes = MsgIsStart(session->status_flag) ? record_end(start_packet) : record_end(data_packet);
    size_t const available_bytes = bytes_in_buffer - header_bytes;
    idigi_callback_status_t status = idigi_callback_abort;
    msg_service_request_t service_data;

    if (edp_packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    ASSERT_GOTO(!MsgIsLastData(session->status_flag), error);
    {
        unsigned int const flag = MsgIsStart(session->status_flag) ? MSG_FLAG_START : 0;

        status = msg_call_service_layer(idigi_ptr, &service_data, session, msg_service_type_need_data, (msg_buffer + header_bytes), available_bytes, flag);
        if (status != idigi_callback_continue)
            goto error;
    }

    if (MsgIsLastData(service_data.flags)) 
        MsgSetLastData(session->status_flag);

    {
        size_t const packet_size = service_data.length_in_bytes + header_bytes;

        msg_fill_msg_header(session, msg_buffer);
        status = initiate_send_facility_packet(idigi_ptr, edp_packet, packet_size, E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);
        if (status != idigi_callback_continue)
        {
            msg_inform_error(idigi_ptr, session, idigi_send_error);
            goto error;
        }
    }

    {     
        msg_data_block_t * const dblock = &session->data_block;

        dblock->total_bytes += service_data.length_in_bytes;
        if (MsgIsLastData(session->status_flag))
        {
            session->state = MsgIsRequest(session->status_flag) ? msg_state_receive : msg_state_delete;
            goto done;
        }

        if ((dblock->total_bytes - dblock->ack_count) > (dblock->available_window - available_bytes))
        {
            MsgSetAckPending(session->status_flag);
            session->state = msg_state_wait_ack;
        }

        goto done;
    }

error:
    release_packet_buffer(idigi_ptr, edp_packet, idigi_success, NULL);

done:
    return status;
}
#endif

static msg_session_t * msg_start_session(idigi_data_t * const idigi_ptr, uint16_t const service_id, idigi_msg_error_t * const result)
{
    static idigi_bool_t const client_owned = idigi_true;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session = msg_create_session(idigi_ptr, msg_ptr, service_id, client_owned, result);

    if (session != NULL)
    {
        *result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_server].window_size, msg_block_state_send_request);
        if (*result != idigi_msg_error_none)
        {
            msg_delete_session(idigi_ptr, msg_ptr, session);
            session = NULL;
        }
    }

    return session;
}

static idigi_callback_status_t msg_send_ack(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * ack_packet;
    uint8_t * const edp_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &ack_packet, NULL);

    if (edp_packet == NULL)
        goto done;

    message_store_u8(ack_packet, opcode, msg_opcode_ack);
    {
        uint8_t const flag = MsgIsClientOwned(session->status_flag) ? 0 : MSG_FLAG_REQUEST;

        message_store_u8(ack_packet, flags, flag);
    }
    message_store_be16(ack_packet, transaction_id, session->session_id);
    message_store_be32(ack_packet, ack_count, session->data_block.total_bytes);
    message_store_be32(ack_packet, window_size, msg_ptr->capabilities[msg_capability_client].window_size);

    status = initiate_send_facility_packet(idigi_ptr, edp_packet, record_end(ack_packet),
                                           E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);
    if (status != idigi_callback_busy)
    {
        session->data_block.ack_count = session->data_block.total_bytes;
        MsgClearAckPending(session->status_flag);
        session->state = msg_state_receive;
    }

done:
    return status;
}

static idigi_callback_status_t msg_process_capabilities(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * capability_packet = ptr;
    idigi_bool_t const request_capabilities = (message_load_u8(capability_packet, flags) & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST;
    uint8_t const version = message_load_u8(capability_packet, version);

    ASSERT_GOTO(version == MSG_FACILITY_VERSION, error);
    msg_fac->capabilities[msg_capability_server].max_transactions = message_load_u8(capability_packet, max_transactions);
    msg_fac->capabilities[msg_capability_server].window_size = message_load_be32(capability_packet, window_size);

    {
        uint8_t const comp_count = message_load_u8(capability_packet, compression_count);
        uint8_t const * compression_list = capability_packet + record_bytes(capability_packet);
        int i;

        msg_fac->capabilities[msg_capability_server].compression_supported = idigi_false;
        for (i = 0; i < comp_count; i++) 
        {
            uint8_t const compression_id = *compression_list++;

            if (compression_id == MSG_COMPRESSION_LIBZ)
            {
                msg_fac->capabilities[msg_capability_server].compression_supported = idigi_true;
                break;
            }
        }
    }

    if (request_capabilities)
    {
        uint8_t const capability_flag = 0;

        status = msg_send_capabilities(idigi_ptr, msg_fac, capability_flag);
    }

error:
    return status;
}

static idigi_callback_status_t msg_pass_service_data(idigi_data_t * const idigi_ptr, msg_session_t * const session, void * data, size_t const bytes)
{
    idigi_callback_status_t status = idigi_callback_continue;

    ASSERT_GOTO(session != NULL, error);

    {
        unsigned int service_flag = 0;
        msg_service_request_t service_data;

        if (MsgIsStart(session->status_flag))
            MsgSetStart(service_flag);

        if (MsgIsLastData(session->status_flag))
            MsgSetLastData(service_flag);

        status = msg_call_service_layer(idigi_ptr, &service_data, session, msg_service_type_have_data, data, bytes, service_flag);
    }

    if (status == idigi_callback_continue)
    {
        msg_data_block_t * const dblock = &session->data_block;

        MsgClearStart(session->status_flag);

        if (MsgIsLastData(session->status_flag))
        {
            if (MsgIsRequest(session->status_flag))
            {
                idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
                idigi_msg_error_t const result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_server].window_size, msg_block_state_send_response);

                MsgClearRequest(session->status_flag);

                if (result != idigi_msg_error_none)
                    status = msg_inform_error(idigi_ptr, session, result);
            }
            else
                session->state = msg_state_delete;
        }
        else
        {
            dblock->total_bytes += bytes;

            if ((dblock->total_bytes - dblock->ack_count) > (dblock->available_window/2))
            {
                MsgSetAckPending(session->status_flag);
                session->state = msg_state_send_ack;
            }
            else
                session->state = msg_state_receive;
        }
    }

error:
    return status;
}

#if (defined IDIGI_COMPRESSION)
static idigi_callback_status_t msg_process_decompressed_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const dblock = &session->data_block;
    z_streamp zlib_ptr = &dblock->zlib;

    ASSERT_GOTO(dblock->bytes_out > 0, error);

    status = msg_pass_service_data(idigi_ptr, session, dblock->buffer_out, dblock->bytes_out);
    switch (status)
    {
    case idigi_callback_continue:
        dblock->bytes_out = 0;
        if (session->state != msg_state_get_data)
        {
            if (zlib_ptr->avail_out == 0)
                session->state = msg_state_decompress;
            else
            {
                if (MsgIsAckPending(session->status_flag))
                    session->state = msg_state_send_ack;
            }
        }
        break;

    case idigi_callback_busy:
        session->state = msg_state_process_decompressed;
        status = idigi_callback_continue;
        break;

    default:
        break;
    }

error:
    return status;
}

static idigi_callback_status_t msg_decompress_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const dblock = &session->data_block;
    z_streamp zlib_ptr = &dblock->zlib;

    if (zlib_ptr->avail_out == 0)
    {
        zlib_ptr->next_out = dblock->buffer_out;
        zlib_ptr->avail_out = sizeof dblock->buffer_out;
    }

    {
        int const zret = inflate(zlib_ptr, Z_NO_FLUSH);

        session->state = MsgIsAckPending(session->status_flag) ? msg_state_send_ack : msg_state_receive;
        switch(zret) 
        {
        case Z_BUF_ERROR:
            if (zlib_ptr->avail_in > 0)
                goto error;
            goto done;
            
        case Z_OK:
            if (zlib_ptr->avail_out > 0)
                goto done;
            break;

        case Z_STREAM_END:
            MsgSetLastData(session->status_flag);
            break;

        default:
            goto error;            
        }

        dblock->bytes_out = sizeof dblock->buffer_out - zlib_ptr->avail_out;
        status = msg_process_decompressed_data(idigi_ptr, session);
    }

    goto done;

error:    
    status = msg_inform_error(idigi_ptr, session, idigi_msg_error_decompression_failure);

done:
    return status;
}

static idigi_callback_status_t msg_process_compressed_data(idigi_data_t * const idigi_ptr, msg_session_t * const session, uint8_t * data, size_t const bytes)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const dblock = &session->data_block;
    z_streamp zlib_ptr = &dblock->zlib;

    if (zlib_ptr->avail_in > 0)
    {
        status = idigi_callback_busy;
        goto done;
    }

    ASSERT_GOTO(bytes < sizeof dblock->buffer_in, done);
    memcpy(dblock->buffer_in, data, bytes);

    zlib_ptr->next_in = dblock->buffer_in;
    zlib_ptr->avail_in = bytes;

    if (MsgIsStart(session->status_flag))
    {
        zlib_ptr->next_out = dblock->buffer_out;
        zlib_ptr->avail_out = sizeof dblock->buffer_out;
    }

    status = msg_decompress_data(idigi_ptr, session);

done:
    return status;
}
#endif

static idigi_callback_status_t msg_process_service_data(idigi_data_t * const idigi_ptr, msg_session_t * const session, uint8_t * msg_data, size_t const frame_bytes, size_t const header_bytes, unsigned const flag)
{
    idigi_callback_status_t status;
    uint8_t * buffer = msg_data + header_bytes;
    size_t const bytes = frame_bytes - header_bytes;

    if (session->state != msg_state_receive)
    {
        idigi_bool_t const isError = session->state == msg_state_send_error;
        idigi_bool_t const isDeleted = session->state == msg_state_delete;

        status = (isError || isDeleted) ? idigi_callback_continue : idigi_callback_busy;
        goto done;
    }

    #if (defined IDIGI_COMPRESSION)
    if (MsgIsCompressed(session->status_flag))
    {
        status = msg_process_compressed_data(idigi_ptr, session, buffer, bytes);
        goto done;
    }
    #endif

    if (MsgIsLastData(flag))
        MsgSetLastData(session->status_flag);

    status = msg_pass_service_data(idigi_ptr, session, buffer, bytes);

done:
    return status;
}

static idigi_callback_status_t msg_process_start(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, uint8_t * ptr, uint16_t const length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_error_t result = idigi_msg_error_none;
    msg_session_t * session;
    uint8_t * start_packet = ptr;
    unsigned flag = message_load_u8(start_packet, flags);
    uint16_t const session_id = message_load_be16(start_packet, transaction_id);
    idigi_bool_t const request = MsgIsRequest(flag);
    idigi_bool_t const client_owned = !request;

    session = msg_find_session(msg_ptr, session_id, client_owned);
    if (session == NULL)
    {
        uint16_t const service_id = message_load_be16(start_packet, service_id);

        if (client_owned)
        {
             result = idigi_msg_error_unknown_session;
             goto error;
        }

        session = msg_create_session(idigi_ptr, msg_ptr, service_id, client_owned, &result);
        if (session == NULL)
        {
            if (result == idigi_msg_error_busy)
            {
                status = idigi_callback_busy;
                goto done;
            }

            goto error;
        }
        session->session_id = session_id;
    }
    else
    {
        /* We didn't check for duplicate session intentionally, because the busy
           from application will result in duplicate session incorrectly */
        if (!request)
            MsgClearRequest(session->status_flag);

        if (session->state == msg_state_send_error) 
            goto done;
    }

    result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_client].window_size, request ? msg_block_state_recv_request : msg_block_state_recv_response);
    if (result != idigi_msg_error_none)
        goto error;

    MsgClearLastData(session->status_flag);

    {
        uint8_t const compression = message_load_u8(start_packet, compression_id);

        #if (defined IDIGI_COMPRESSION)
        if (compression == MSG_COMPRESSION_NONE)
            MsgClearCompression(session->status_flag);
        #else
        if (compression != MSG_COMPRESSION_NONE)
        {
            result = idigi_msg_error_decompression_failure;
            goto error;
        }
        #endif
    }

    MsgSetStart(flag);
    status = msg_process_service_data(idigi_ptr, session, ptr, length, record_end(start_packet), flag);
    goto done;

error:
    status = (session != NULL) ? msg_inform_error(idigi_ptr, session, result) : msg_send_error(idigi_ptr, msg_ptr, session, session_id, result, flag);

done:
    return status;
}

static idigi_callback_status_t msg_process_data(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, uint8_t * ptr, uint16_t const length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session;
    uint8_t * const data_packet = ptr;
    unsigned const flag = message_load_u8(data_packet, flags);
    uint16_t const session_id = message_load_be16(data_packet, transaction_id);
    idigi_bool_t const client_owned = !MsgIsRequest(flag);

    session = msg_find_session(msg_ptr, session_id, client_owned);
    if (session == NULL)
    {
        status = msg_send_error(idigi_ptr, msg_ptr, session, session_id, idigi_msg_error_unknown_session, flag);
        goto done;
    }

    status = msg_process_service_data(idigi_ptr, session, ptr, length, record_end(data_packet), flag);

done:
    return status;
}

static idigi_callback_status_t msg_process_ack(idigi_msg_data_t * const msg_fac, uint8_t const * ptr)
{
    msg_session_t * session;
    uint8_t const * const ack_packet = ptr;

    {
        uint16_t const session_id = message_load_be16(ack_packet, transaction_id);
        uint8_t const flag = message_load_u8(ack_packet, flags);
        idigi_bool_t const client_owned = MsgIsRequest(flag);

        session = msg_find_session(msg_fac, session_id, client_owned);
        /* already closed? done sending all data */
        if (session == NULL)
            goto done;
    }

    {
        msg_data_block_t * const dblock = &session->data_block;

        dblock->available_window = message_load_be32(ack_packet, window_size);
        dblock->ack_count = message_load_be32(ack_packet, ack_count);

        if (dblock->available_window > 0)
        {
            MsgClearAckPending(session->status_flag);
            if (session->state == msg_state_wait_ack)
                session->state = msg_state_get_data;
        }
    }

done:
    return idigi_callback_continue;
}

static idigi_callback_status_t msg_process_error(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * const error_packet = ptr;
    uint8_t const flag = message_load_u8(error_packet, flags);
    idigi_bool_t const client_request_error = MsgIsRequest(flag) && !MsgIsSender(flag);
    idigi_bool_t const server_response_error = !MsgIsRequest(flag) && MsgIsSender(flag);
    idigi_bool_t const client_owned = client_request_error || server_response_error;
    uint16_t const session_id = message_load_be16(error_packet, transaction_id);
    msg_session_t * const session = msg_find_session(msg_fac, session_id, client_owned);

    if (session != NULL && session->state != msg_state_delete && session->state != msg_state_send_error)
    {
        idigi_msg_error_t error = message_load_u8(error_packet, error_code);

        status = msg_inform_error(idigi_ptr, session, error);
        msg_delete_session(idigi_ptr, msg_fac, session);
    }
    else
    {
        idigi_debug("msg_process_error: unable to find session id = %d\n", session_id);
    }

    return status;
}

static idigi_callback_status_t msg_discovery(idigi_data_t * const idigi_ptr, void * const facility_data, uint8_t * const packet, unsigned int * const receive_timeout)
{
    uint8_t const capability_flag = MSG_FLAG_REQUEST;

    UNUSED_PARAMETER(packet);
    UNUSED_PARAMETER(receive_timeout);

    return msg_send_capabilities(idigi_ptr, facility_data, capability_flag);
}

static void msg_switch_session(idigi_msg_data_t * const msg_ptr, msg_session_t * const session)
{
    if (!msg_ptr->session_locked)
    {
        msg_ptr->session_locked = idigi_true;
        msg_ptr->session.current = (session->prev != NULL) ? session->prev : msg_ptr->session.tail;
        msg_ptr->session_locked = idigi_false;
    }
}

static idigi_callback_status_t msg_process_pending(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, unsigned int * const receive_timeout)
{
    idigi_callback_status_t status = idigi_callback_continue;

    if (msg_ptr->session_locked) goto done;

    msg_ptr->session_locked = idigi_true;
    if (msg_ptr->session.current == NULL)
        msg_ptr->session.current = msg_ptr->session.tail;
    msg_ptr->session_locked = idigi_false;

    if (msg_ptr->session.current != NULL)
    {
        msg_session_t * const session = msg_ptr->session.current;

        *receive_timeout = MIN_RECEIVE_TIMEOUT_IN_SECONDS;
        switch (session->state) 
        {
        case msg_state_init:
        case msg_state_wait_ack:
        case msg_state_receive:
        case msg_state_wait_send_complete:
            msg_switch_session(msg_ptr, session);
            *receive_timeout = MAX_RECEIVE_TIMEOUT_IN_SECONDS;
            break;

        case msg_state_get_data:
            status = msg_get_service_data(idigi_ptr, session);
            if (status == idigi_callback_busy) 
                msg_switch_session(msg_ptr, session);
            break;

        #if (defined IDIGI_COMPRESSION)
        case msg_state_compress:
            status = msg_compress_data(idigi_ptr, session);
            break;

        case msg_state_send_compressed:
            status = msg_send_compressed_data(idigi_ptr, session);
            break;

        case msg_state_decompress:
            status = msg_decompress_data(idigi_ptr, session);
            break;

        case msg_state_process_decompressed:
            status = msg_process_decompressed_data(idigi_ptr, session);
            break;
        #endif

        case msg_state_send_ack:
            status = msg_send_ack(idigi_ptr, msg_ptr, session);
            break;

        case msg_state_send_error:
            status = msg_send_error(idigi_ptr, msg_ptr, session, session->session_id, session->error, session->error_flag);
            break;

        case msg_state_delete:
            msg_delete_session(idigi_ptr, msg_ptr, session);
            break;

        default:
            status = idigi_callback_abort;
            ASSERT_GOTO(idigi_false, done);
            break;
        }
    }

done:
    return status;
}

static idigi_callback_status_t msg_process(idigi_data_t * const idigi_ptr, void * const facility_data, uint8_t * const edp_header, unsigned int * const receive_timeout)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * const msg_ptr = facility_data;

    ASSERT_GOTO(idigi_ptr != NULL, error);
    ASSERT_GOTO(msg_ptr != NULL, error);

    if (edp_header != NULL)
    {
        uint8_t * const data_ptr = GET_PACKET_DATA_POINTER(edp_header, PACKET_EDP_FACILITY_SIZE);
        uint16_t const length = message_load_be16(edp_header, length);    
        uint8_t const opcode = *data_ptr;

        switch (opcode)
        {
            case msg_opcode_capability:
                status = msg_process_capabilities(idigi_ptr, msg_ptr, data_ptr);
                break;
    
            case msg_opcode_start:
                status = msg_process_start(idigi_ptr, msg_ptr, data_ptr, length);
                break;
    
            case msg_opcode_data:
                status = msg_process_data(idigi_ptr, msg_ptr, data_ptr, length);
                break;
    
            case msg_opcode_ack:
                status = msg_process_ack(msg_ptr, data_ptr);
                break;
    
            case msg_opcode_error:
                status = msg_process_error(idigi_ptr, msg_ptr, data_ptr);
                break;
    
            default:            
                idigi_debug("msg_process: Invalid opcode\n");
                break;
        }
    }

    if (msg_process_pending(idigi_ptr, msg_ptr, receive_timeout) == idigi_callback_abort) 
        status = idigi_callback_abort;

error:
    return status;
}

static idigi_callback_status_t msg_cleanup_all_sessions(idigi_data_t * const idigi_ptr, uint16_t const service_id)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session = msg_ptr->session.head;

    while(session != NULL)
    {
        msg_session_t * next_session = session->next;

        if (session->service_id == service_id)
        {
            if (session->state != msg_state_delete && session->state != msg_state_send_error)
            {
                idigi_msg_error_t error_code = idigi_msg_error_cancel;
                msg_service_request_t service_data;

                msg_call_service_layer(idigi_ptr, &service_data, session, msg_service_type_error, &error_code, sizeof error_code, 0);
            }
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

    {
        idigi_bool_t is_empty = idigi_true;
        int i;

        for(i = 0; i < msg_service_id_count; i++) 
        {
            if (msg_ptr->service_cb[i] != NULL)
            {
                is_empty = idigi_false;
                break;
            }
        }

        status = is_empty ? del_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM) : idigi_callback_continue;
    }

error:
    return status;
}

static idigi_callback_status_t msg_init_facility(idigi_data_t * const idigi_ptr, unsigned int const facility_index, uint16_t service_id, idigi_msg_callback_t callback)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    if (msg_ptr == NULL)
    {
        void * fac_ptr = NULL;

        status = add_facility_data(idigi_ptr, facility_index, E_MSG_FAC_MSG_NUM, &fac_ptr, sizeof *msg_ptr);
        ASSERT_GOTO(status == idigi_callback_continue, done);
        ASSERT_GOTO(fac_ptr != NULL, done);

        msg_ptr = fac_ptr;
        memset(msg_ptr, 0, sizeof *msg_ptr);

        #if (defined IDIGI_COMPRESSION)
        msg_ptr->capabilities[msg_capability_client].compression_supported = idigi_true;
        #endif

        #if (IDIGI_VERSION < IDIGI_VERSION_1100)
        #define IDIGI_MSG_MAX_TRANSACTION   1
        #endif

        #if (defined IDIGI_MSG_MAX_TRANSACTION)
        msg_ptr->capabilities[msg_capability_client].max_transactions = IDIGI_MSG_MAX_TRANSACTION;
        #else
        {
            idigi_request_t const request_id = {idigi_config_max_transaction};

            status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_config, request_id,
                                                    &msg_ptr->capabilities[msg_capability_client].max_transactions, NULL);
            if (status != idigi_callback_continue && status != idigi_callback_unrecognized)
            {
                idigi_ptr->error_code = idigi_configuration_error;
                goto done;
            }
            if (msg_ptr->capabilities[msg_capability_client].max_transactions > IDIGI_MAX_TRANSACTIONS_LIMIT)
            {
                notify_error_status(idigi_ptr->callback, idigi_class_config, request_id, idigi_invalid_data_range);
            }
        }
        #endif

        {
            uint32_t const recv_window = 16384; 

            ASSERT(recv_window > MSG_MAX_RECV_PACKET_SIZE);
            msg_ptr->capabilities[msg_capability_client].window_size = recv_window;
        }
    }

    msg_ptr->service_cb[service_id] = callback;
    status = idigi_callback_continue;

done:
    return status;
}


