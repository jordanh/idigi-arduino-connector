/*
 * Copyright (c) 2011, 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
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

#define MSG_FLAG_CLIENT_OWNED 0x20
#define MSG_FLAG_RECEIVING    0x40
#define MSG_FLAG_START        0x80
#define MSG_FLAG_ACK_PENDING  0x100
#define MSG_FLAG_COMPRESSED   0x200
#define MSG_FLAG_INFLATED     0x400
#define MSG_FLAG_DEFLATED     0x800
#define MSG_FLAG_SEND_NOW     0x1000
#define MSG_FLAG_DOUBLE_BUF   0x2000

#define MsgIsBitSet(flag, bit)   (idigi_bool(((flag) & (bit)) == (bit)))
#define MsgIsBitClear(flag, bit) (idigi_bool(((flag) & (bit)) == 0))
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
#define MsgIsInflated(flag)     MsgIsBitSet((flag), MSG_FLAG_INFLATED)
#define MsgIsDeflated(flag)     MsgIsBitSet((flag), MSG_FLAG_DEFLATED)
#define MsgIsSendNow(flag)      MsgIsBitSet((flag), MSG_FLAG_SEND_NOW)
#define MsgIsDoubleBuf(flag)    MsgIsBitSet((flag), MSG_FLAG_DOUBLE_BUF)

#define MsgIsNotRequest(flag)      MsgIsBitClear((flag), MSG_FLAG_REQUEST)
#define MsgIsNotLastData(flag)     MsgIsBitClear((flag), MSG_FLAG_LAST_DATA)
#define MsgIsNotSender(flag)       MsgIsBitClear((flag), MSG_FLAG_SENDER)
#define MsgIsNotReceiving(flag)    MsgIsBitClear((flag), MSG_FLAG_RECEIVING)
#define MsgIsNotClientOwned(flag)  MsgIsBitClear((flag), MSG_FLAG_CLIENT_OWNED)
#define MsgIsNotInflated(flag)     MsgIsBitClear((flag), MSG_FLAG_INFLATED)
#define MsgIsNotDeflated(flag)     MsgIsBitClear((flag), MSG_FLAG_DEFLATED)

#define MsgSetRequest(flag)     MsgBitSet((flag), MSG_FLAG_REQUEST)
#define MsgSetLastData(flag)    MsgBitSet((flag), MSG_FLAG_LAST_DATA)
#define MsgSetSender(flag)      MsgBitSet((flag), MSG_FLAG_SENDER)
#define MsgSetReceiving(flag)   MsgBitSet((flag), MSG_FLAG_RECEIVING)
#define MsgSetCompression(flag) MsgBitSet((flag), MSG_FLAG_COMPRESSED)
#define MsgSetStart(flag)       MsgBitSet((flag), MSG_FLAG_START)
#define MsgSetAckPending(flag)  MsgBitSet((flag), MSG_FLAG_ACK_PENDING)
#define MsgSetClientOwned(flag) MsgBitSet((flag), MSG_FLAG_CLIENT_OWNED)
#define MsgSetInflated(flag)    MsgBitSet((flag), MSG_FLAG_INFLATED)
#define MsgSetDeflated(flag)    MsgBitSet((flag), MSG_FLAG_DEFLATED)
#define MsgSetSendNow(flag)     MsgBitSet((flag), MSG_FLAG_SEND_NOW)
#define MsgSetDoubleBuf(flag)   MsgBitSet((flag), MSG_FLAG_DOUBLE_BUF)

#define MsgClearRequest(flag)     MsgBitClear((flag), MSG_FLAG_REQUEST)
#define MsgClearLastData(flag)    MsgBitClear((flag), MSG_FLAG_LAST_DATA)
#define MsgClearReceiving(flag)   MsgBitClear((flag), MSG_FLAG_RECEIVING)
#define MsgClearStart(flag)       MsgBitClear((flag), MSG_FLAG_START)
#define MsgClearAckPending(flag)  MsgBitClear((flag), MSG_FLAG_ACK_PENDING)
#define MsgClearCompression(flag) MsgBitClear((flag), MSG_FLAG_COMPRESSED)
#define MsgClearInflated(flag)    MsgBitClear((flag), MSG_FLAG_INFLATED)
#define MsgClearDeflated(flag)    MsgBitClear((flag), MSG_FLAG_DEFLATED)
#define MsgClearSendNow(flag)     MsgBitClear((flag), MSG_FLAG_SEND_NOW)

typedef enum
{
    msg_service_id_none,
    msg_service_id_data,
    msg_service_id_file,
    msg_service_id_rci,
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
    msg_state_send_data,
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
    msg_service_type_free,
    msg_service_type_pending_request
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
    unsigned int status_flag;
#if (defined IDIGI_COMPRESSION)
    uint8_t  buffer_in[MSG_MAX_RECV_PACKET_SIZE];
    uint8_t  buffer_out[MSG_MAX_RECV_PACKET_SIZE];
    size_t   bytes_out;
    int      z_flag;
    z_stream zlib;
#endif
} msg_data_block_t;

typedef struct
{
    void * data_ptr;
    size_t length_in_bytes;
    unsigned int flags;
} msg_service_data_t;

typedef struct
{
    void * session;
    msg_service_type_t service_type;
    msg_service_data_t * need_data;
    msg_service_data_t * have_data;
    idigi_msg_error_t error_value;
} msg_service_request_t;

typedef struct msg_session_t
{
    unsigned int session_id;
    unsigned int service_id;
    void * service_context;
    msg_state_t current_state;
    msg_state_t saved_state;
    uint8_t * send_data_ptr;
    size_t send_data_bytes;
    msg_data_block_t * in_dblock;
    msg_data_block_t * out_dblock;
    idigi_msg_error_t error;
    unsigned int error_flag;
    msg_service_request_t service_layer_data;
    struct msg_session_t * next;
    struct msg_session_t * prev;
} msg_session_t;

typedef idigi_callback_status_t idigi_msg_callback_t(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request);

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
    void const * pending_service_request;
} idigi_msg_data_t;

static msg_session_t * msg_find_session(idigi_msg_data_t const * const msg_ptr, unsigned int const id, idigi_bool_t const client_owned)
{
    msg_session_t * session = msg_ptr->session.head;

    while (session != NULL)
    {
        if (session->session_id == id)
        {
            unsigned int const status = (session->in_dblock != NULL) ? session->in_dblock->status_flag : session->out_dblock->status_flag;

            if (MsgIsClientOwned(status) == client_owned)
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
    msg_data_block_t * const dblock = (session->in_dblock != NULL) ? session->in_dblock : session->out_dblock;
    idigi_bool_t const client_request_error = idigi_bool(MsgIsClientOwned(dblock->status_flag) && MsgIsNotReceiving(dblock->status_flag));
    idigi_bool_t const client_response_error = idigi_bool(MsgIsNotClientOwned(dblock->status_flag) && MsgIsNotReceiving(dblock->status_flag));
    idigi_bool_t const server_request_error = idigi_bool(MsgIsNotClientOwned(dblock->status_flag) && MsgIsReceiving(dblock->status_flag));

    if (client_request_error && MsgIsStart(dblock->status_flag))
    {
        /* no need to send an error. just delete since nothing has been sent to server */
        session->current_state = msg_state_delete;
        goto done;
    }

    session->error = error_code;
    session->current_state = msg_state_send_error;

    if (client_response_error && MsgIsStart(dblock->status_flag))
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

static idigi_callback_status_t msg_call_service_layer(idigi_data_t * const idigi_ptr, msg_session_t * const session, msg_service_type_t const type)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_service_request_t * const service_ptr = &session->service_layer_data;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    idigi_msg_callback_t * cb_fn;

    ASSERT_GOTO(msg_ptr != NULL, error);
    cb_fn = msg_ptr->service_cb[session->service_id];
    ASSERT_GOTO(cb_fn != NULL, error);

    service_ptr->service_type = type;
    service_ptr->session = session;

    status = cb_fn(idigi_ptr, service_ptr);
    if ((status == idigi_callback_continue) && (service_ptr->service_type == msg_service_type_error))
    {
        msg_set_error(session, service_ptr->error_value);
        status = idigi_callback_unrecognized;
    }

error:
    return status;
}

static idigi_callback_status_t msg_inform_error(idigi_data_t * const idigi_ptr, msg_session_t * const session, idigi_msg_error_t error_code)
{
    session->service_layer_data.error_value = error_code;

    return msg_call_service_layer(idigi_ptr, session, msg_service_type_error);
}

static msg_session_t * msg_create_session(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, unsigned int const service_id,
                                          idigi_bool_t const client_owned, idigi_msg_error_t * const result)
{
    unsigned int session_id = MSG_INVALID_CLIENT_SESSION;
    msg_capability_type_t const capability_id = client_owned ? msg_capability_server : msg_capability_client;
    msg_session_t * session = NULL;
    unsigned int flags = 0;

    ASSERT_GOTO(result != NULL, done);
    *result = idigi_msg_error_memory;
    ASSERT_GOTO(msg_ptr != NULL, done);

    {
        unsigned int const max_transactions = msg_ptr->capabilities[capability_id].max_transactions;
        unsigned int const active_transactions = msg_ptr->capabilities[capability_id].active_transactions;
        unsigned int const unlimited_transactions = 0;

        if ((max_transactions != unlimited_transactions) && (active_transactions >= max_transactions))
        {
            idigi_debug_printf("msg_create_session: active transactions %d > max transactions %d, service id %d\n", active_transactions, max_transactions, service_id);
            ASSERT(client_owned); /* server should not send more than client's max transactions */
            goto done;
        }
    }

    if (client_owned)
    {
        session_id = msg_find_next_available_id(msg_ptr);
        if (session_id == MSG_INVALID_CLIENT_SESSION) goto done;
        MsgSetClientOwned(flags);
    }

    MsgSetRequest(flags);
    #if (defined IDIGI_COMPRESSION)
    MsgSetCompression(flags);
    #endif

    switch (service_id)
    {
    case msg_service_id_rci:
        MsgSetDoubleBuf(flags);
        break;

    default:
        break;
    }

    {
        void * ptr;
        size_t const bytes_in_block = sizeof(msg_data_block_t);
        size_t const bytes_in_service_data = sizeof(msg_service_data_t);
        size_t const bytes_in_session = sizeof *session;
        size_t const single_buffer_bytes = bytes_in_block + bytes_in_service_data;
        size_t const double_buffer_bytes = MsgIsCompressed(flags) ? 2 * single_buffer_bytes : (2 * single_buffer_bytes) + MSG_MAX_SEND_PACKET_SIZE;
        size_t const total_bytes = bytes_in_session + (MsgIsDoubleBuf(flags) ? double_buffer_bytes : single_buffer_bytes);
        idigi_callback_status_t const status = malloc_data(idigi_ptr, total_bytes, &ptr);
        uint8_t * data_ptr = ptr;

        if (status != idigi_callback_continue) goto done;
        session = ptr;
        data_ptr += bytes_in_session;

        if (MsgIsDoubleBuf(flags))
        {
            session->out_dblock = (msg_data_block_t *)data_ptr;
            session->in_dblock = session->out_dblock + 1;
            data_ptr += (2 * bytes_in_block);
            session->service_layer_data.have_data = (msg_service_data_t *)data_ptr;
            session->service_layer_data.need_data = session->service_layer_data.have_data + 1;
            data_ptr += (2 * bytes_in_service_data);
            session->send_data_ptr = MsgIsCompressed(flags) ? NULL : data_ptr;
        }
        else
        {
            msg_data_block_t ** const current_dblock_ptr = client_owned ? &session->out_dblock : &session->in_dblock;
            msg_data_block_t ** const other_dblock_ptr = client_owned ? &session->in_dblock : &session->out_dblock;

            *current_dblock_ptr = (msg_data_block_t *)data_ptr;
            *other_dblock_ptr = NULL;
            data_ptr += bytes_in_block;
            session->service_layer_data.need_data = (msg_service_data_t *)(client_owned ? data_ptr : NULL);
            session->service_layer_data.have_data = (msg_service_data_t *)(client_owned ? NULL : data_ptr);
            session->send_data_ptr = NULL;
        }
    }

    session->session_id = session_id;
    session->service_id = service_id;
    session->error = idigi_msg_error_none;
    session->error_flag = 0;
    session->send_data_bytes = 0;
    session->service_context = NULL;
    session->current_state = msg_state_init;
    session->saved_state = msg_state_init;

    if (session->out_dblock != NULL)
        session->out_dblock->status_flag = flags;

    if (session->in_dblock != NULL)
        session->in_dblock->status_flag = flags;

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
        if ((session->in_dblock != NULL) && MsgIsInflated(session->in_dblock->status_flag))
            inflateEnd(&session->in_dblock->zlib);

        if ((session->out_dblock != NULL) && MsgIsDeflated(session->out_dblock->status_flag))
            deflateEnd(&session->out_dblock->zlib);
    }
    #endif

    {
        unsigned int const status = (session->in_dblock != NULL) ? session->in_dblock->status_flag : session->out_dblock->status_flag;
        msg_capability_type_t const capability_id = MsgIsClientOwned(status) ? msg_capability_server : msg_capability_client;

        ASSERT_GOTO(msg_ptr->capabilities[capability_id].active_transactions > 0, error);
        msg_ptr->capabilities[capability_id].active_transactions--;
    }

    msg_call_service_layer(idigi_ptr, session, msg_service_type_free);
    free_data(idigi_ptr, session);

error:
    return;
}

static void msg_default_data_block(msg_data_block_t * dblock, uint32_t const window_size)
{
    #if (defined IDIGI_COMPRESSION)
    dblock->bytes_out = 0;
    dblock->z_flag = Z_NO_FLUSH;
    #endif

    dblock->available_window = window_size;
    dblock->ack_count = 0;
    dblock->total_bytes = 0;
    MsgSetStart(dblock->status_flag);
    MsgClearLastData(dblock->status_flag);
}

static idigi_msg_error_t msg_initialize_data_block(msg_session_t * const session, uint32_t const window_size, msg_block_state_t state)
{
    idigi_msg_error_t result = idigi_msg_error_none;

    ASSERT_GOTO(session != NULL, error);

    switch(state)
    {
    case msg_block_state_send_response:
        if (session->out_dblock == NULL)
        {
            #if (defined IDIGI_COMPRESSION)
            ASSERT_GOTO(session->in_dblock != NULL, compression_error);
            inflateEnd(&session->in_dblock->zlib);
            MsgClearInflated(session->in_dblock->status_flag);
            #endif
            session->out_dblock = session->in_dblock;
            ASSERT_GOTO(session->service_layer_data.have_data != NULL, error);
            session->service_layer_data.need_data = session->service_layer_data.have_data;
            session->in_dblock = NULL;
            session->service_layer_data.have_data = NULL;
        }
        MsgClearRequest(session->out_dblock->status_flag);
        /* intentional fall through */

    case msg_block_state_send_request:
        msg_default_data_block(session->out_dblock, window_size);
        MsgClearReceiving(session->out_dblock->status_flag);
        session->current_state = msg_state_get_data;
        session->service_layer_data.need_data->data_ptr = NULL;
        #if (defined IDIGI_COMPRESSION)
        if (idigi_bool(MsgIsNotDeflated(session->out_dblock->status_flag)))
        {
            memset(&session->out_dblock->zlib, 0, sizeof session->out_dblock->zlib);
            ASSERT_GOTO(deflateInit(&session->out_dblock->zlib, Z_DEFAULT_COMPRESSION) == Z_OK, compression_error);
            MsgSetDeflated(session->out_dblock->status_flag);
        }
        #endif
        break;

    case msg_block_state_recv_response:
        if (session->in_dblock == NULL)
        {
            #if (defined IDIGI_COMPRESSION)
            ASSERT_GOTO(session->out_dblock != NULL, compression_error);
            deflateEnd(&session->out_dblock->zlib);
            MsgClearDeflated(session->out_dblock->status_flag);
            #endif
            session->in_dblock = session->out_dblock;
            ASSERT_GOTO(session->service_layer_data.need_data != NULL, error);
            session->service_layer_data.have_data = session->service_layer_data.need_data;
            MsgClearRequest(session->in_dblock->status_flag);
            session->out_dblock = NULL;
            session->service_layer_data.need_data = NULL;
        }
        /* intentional fall through */

    case msg_block_state_recv_request:
        msg_default_data_block(session->in_dblock, window_size);
        MsgSetReceiving(session->in_dblock->status_flag);
        session->current_state = msg_state_receive;
        #if (defined IDIGI_COMPRESSION)
        if (idigi_bool(MsgIsNotInflated(session->in_dblock->status_flag)))
        {
            memset(&session->in_dblock->zlib, 0, sizeof session->in_dblock->zlib);
            ASSERT_GOTO(inflateInit(&session->in_dblock->zlib) == Z_OK, compression_error);
            MsgSetInflated(session->in_dblock->status_flag);
        }
        #endif
        break;

    default:
        ASSERT_GOTO(idigi_false, error);
        break;
    }

    goto done;

#if (defined IDIGI_COMPRESSION)
compression_error:
    result = idigi_msg_error_compression_failure;
#endif

error:
done:
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

    if MsgIsRequest(session->out_dblock->status_flag)
        MsgSetRequest(flag);

    if (MsgIsLastData(session->out_dblock->status_flag))
        MsgSetLastData(flag);

    if (MsgIsStart(session->out_dblock->status_flag))
    {
        uint8_t * const start_packet = ptr;

        MsgClearStart(session->out_dblock->status_flag);

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
        message_store_u8(start_packet, compression_id, (MsgIsCompressed(session->out_dblock->status_flag) ? MSG_COMPRESSION_LIBZ : MSG_COMPRESSION_NONE));
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

static idigi_callback_status_t msg_send_complete(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data)
{
    idigi_callback_status_t return_status = idigi_callback_continue;

    msg_session_t * const session = user_data;
    msg_data_block_t * const dblock = session->out_dblock;

    ASSERT_GOTO(session != NULL, error);
    ASSERT_GOTO(dblock != NULL, error);

    if ((MsgIsDoubleBuf(dblock->status_flag) == idigi_false) && (MsgIsCompressed(dblock->status_flag) == idigi_false))
        return_status = release_packet_buffer(idigi_ptr, packet, idigi_success, NULL);

    switch (status)
    {
        case idigi_service_busy:
            session->current_state = msg_state_send_data;
            break;

        case idigi_success:
            /* update session state */
            if (MsgIsLastData(dblock->status_flag))
            {
                if (MsgIsRequest(dblock->status_flag))
                    session->current_state = msg_state_receive;
                else
                {
                    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

                    msg_delete_session(idigi_ptr, msg_ptr, session);
                }
                goto done;
            }

            #if (defined IDIGI_COMPRESSION)
            dblock->bytes_out = 0;
            if (dblock->zlib.avail_out == 0)
            {
                session->current_state = msg_state_compress;
                goto done;
            }

            dblock->z_flag = Z_NO_FLUSH;
            dblock->zlib.avail_out = 0;
            #else
            session->send_data_bytes = 0;
            #endif

            session->service_layer_data.need_data->data_ptr = NULL;
            session->current_state = MsgIsAckPending(dblock->status_flag) ? msg_state_wait_ack : session->saved_state;

            break;

        default:
            msg_inform_error(idigi_ptr, session, idigi_msg_error_send);
            break;
    }

error:
done:
    return return_status;
}

static idigi_callback_status_t msg_send_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    uint8_t * buffer;
    size_t bytes;

    #if (defined IDIGI_COMPRESSION)
    {
        msg_data_block_t * const dblock = session->out_dblock;

        bytes = dblock->bytes_out;
        buffer = dblock->buffer_out;
    }
    #else
    bytes = session->send_data_bytes;
    buffer = session->send_data_ptr;
    #endif

    ASSERT_GOTO(bytes > 0, error);
    status = initiate_send_facility_packet(idigi_ptr, buffer, bytes, E_MSG_FAC_MSG_NUM, msg_send_complete, session);
    if (status != idigi_callback_continue)
    {
        idigi_status_t const result = (status == idigi_callback_busy) ? idigi_service_busy : idigi_send_error;

        msg_send_complete(idigi_ptr, buffer, result, session);
    }
    else
        session->current_state = msg_state_wait_send_complete;

error:
    return status;
}

#if (defined IDIGI_COMPRESSION)
static idigi_callback_status_t msg_compress_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const dblock = session->out_dblock;
    uint8_t * const msg_buffer = GET_PACKET_DATA_POINTER(dblock->buffer_out, PACKET_EDP_FACILITY_SIZE);
    size_t const frame_bytes = sizeof dblock->buffer_out - PACKET_EDP_FACILITY_SIZE;
    z_streamp zlib_ptr = &dblock->zlib;
    int zret;

    if (zlib_ptr->avail_out == 0)
    {
        size_t const header_length = MsgIsStart(dblock->status_flag) ? record_end(start_packet) : record_end(data_packet);

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
        MsgSetLastData(dblock->status_flag);
        break;

    default:
        status = msg_inform_error(idigi_ptr, session, idigi_msg_error_compression_failure);
        goto done;
    }

    msg_fill_msg_header(session, msg_buffer);
    dblock->bytes_out = frame_bytes - zlib_ptr->avail_out;
    status = msg_send_data(idigi_ptr, session);

done:
    return status;
}

static idigi_callback_status_t msg_prepare_send_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = session->out_dblock;

    UNUSED_PARAMETER(idigi_ptr);
    ASSERT_GOTO(dblock != NULL, error);
    ASSERT_GOTO(dblock->zlib.avail_in == 0, error);

    {
        unsigned int const flag = (dblock->total_bytes == 0) ? MSG_FLAG_START : 0;
        msg_service_data_t * const service_data = session->service_layer_data.need_data;

        ASSERT_GOTO(service_data != NULL, error);
        service_data->data_ptr = dblock->buffer_in;
        service_data->length_in_bytes = sizeof dblock->buffer_in;
        service_data->flags = flag;
        status = idigi_callback_continue;
    }

error:
    return status;
}

static idigi_callback_status_t msg_process_send_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = session->out_dblock;

    ASSERT_GOTO(dblock != NULL, error);
    ASSERT_GOTO(dblock->zlib.avail_in == 0, error);

    {
        z_streamp const zlib_ptr = &dblock->zlib;
        msg_service_data_t * const service_data = session->service_layer_data.need_data;

        zlib_ptr->next_in = dblock->buffer_in;
        zlib_ptr->avail_in = service_data->length_in_bytes;
        dblock->total_bytes += service_data->length_in_bytes;
        if (MsgIsLastData(service_data->flags))
            dblock->z_flag = Z_FINISH;
    }

    if (dblock->z_flag == Z_NO_FLUSH)
    {
        size_t const unacked_bytes = dblock->total_bytes - dblock->ack_count;
        size_t const safe_window_size = dblock->available_window - sizeof dblock->buffer_in;

        if (unacked_bytes > safe_window_size)
        {
            dblock->z_flag = Z_SYNC_FLUSH;
            MsgSetAckPending(dblock->status_flag);
        }
    }

    status = msg_compress_data(idigi_ptr, session);

error:
    return status;
}

#else

static idigi_callback_status_t msg_prepare_send_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    uint8_t * msg_buffer;
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = session->out_dblock;

    ASSERT_GOTO(dblock != NULL, error);
    if (MsgIsDoubleBuf(dblock->status_flag))
    {
        msg_buffer = GET_PACKET_DATA_POINTER(session->send_data_ptr, PACKET_EDP_FACILITY_SIZE);
        session->send_data_bytes = MSG_MAX_SEND_PACKET_SIZE - PACKET_EDP_FACILITY_SIZE;
    }
    else
    {
        session->send_data_ptr = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &msg_buffer, &session->send_data_bytes);
        if (session->send_data_ptr == NULL)
        {
            status = idigi_callback_busy;
            goto done;
        }
    }

    {
        size_t const header_bytes = MsgIsStart(dblock->status_flag) ? record_end(start_packet) : record_end(data_packet);
        msg_service_data_t * const service_data = session->service_layer_data.need_data;
        unsigned int const flag = MsgIsStart(dblock->status_flag) ? MSG_FLAG_START : 0;

        ASSERT_GOTO(MsgIsNotLastData(dblock->status_flag), error);
        ASSERT_GOTO(session->send_data_bytes > header_bytes, error);
        ASSERT_GOTO(service_data != NULL, error);

        service_data->data_ptr = msg_buffer + header_bytes;
        service_data->length_in_bytes = session->send_data_bytes - header_bytes;
        service_data->flags = flag;
        status = idigi_callback_continue;
    }

error:
done:
    return status;
}

static idigi_callback_status_t msg_process_send_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = session->out_dblock;
    msg_service_data_t * const service_data = session->service_layer_data.need_data;

    ASSERT_GOTO(dblock != NULL, error);
    if (MsgIsLastData(service_data->flags))
        MsgSetLastData(dblock->status_flag);

    {
        size_t const header_bytes = MsgIsStart(dblock->status_flag) ? record_end(start_packet) : record_end(data_packet);
        uint8_t * const msg_buffer = GET_PACKET_DATA_POINTER(session->send_data_ptr, PACKET_EDP_FACILITY_SIZE);

        session->send_data_bytes = service_data->length_in_bytes + header_bytes;
        msg_fill_msg_header(session, msg_buffer);
        status = msg_send_data(idigi_ptr, session);
        if (status != idigi_callback_continue)
            goto error;
    }

    dblock->total_bytes += service_data->length_in_bytes;
    if ((dblock->total_bytes - dblock->ack_count) > (dblock->available_window - MSG_MAX_SEND_PACKET_SIZE))
        MsgSetAckPending(dblock->status_flag);

error:
    return status;
}

#endif

static idigi_callback_status_t msg_get_service_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_data_block_t * const dblock = session->out_dblock;

    ASSERT_GOTO(dblock != NULL, error);
    ASSERT_GOTO(MsgIsNotLastData(dblock->status_flag), done);

    status = msg_prepare_send_data(idigi_ptr, session);
    if (status != idigi_callback_continue)
        goto error;

    status = msg_call_service_layer(idigi_ptr, session, msg_service_type_need_data);
    if (status != idigi_callback_continue)
    {
        /* release the buffer */
        if ((MsgIsDoubleBuf(dblock->status_flag) == idigi_false) && (MsgIsCompressed(dblock->status_flag) == idigi_false))
            release_packet_buffer(idigi_ptr, session->send_data_ptr, idigi_success, NULL);

        goto done;
    }

    session->saved_state = session->current_state;
    status = msg_process_send_data(idigi_ptr, session);

error:
done:
    return status;
}

#if (defined IDIGI_DATA_SERVICE)
static idigi_bool_t msg_initiate_request(idigi_data_t * const idigi_ptr, void const * const service_context)
{
    idigi_bool_t success = idigi_false;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(msg_ptr != NULL, error);
    if (msg_ptr->pending_service_request == NULL)
    {
        msg_ptr->pending_service_request = service_context;
        success = idigi_true;
    }

error:
    return success;
}

static void msg_start_session(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr)
{
    static idigi_bool_t const client_owned = idigi_true;
    idigi_msg_error_t result = idigi_msg_error_none;
    msg_session_t * const session = msg_create_session(idigi_ptr, msg_ptr, msg_service_id_data, client_owned, &result);

    if (session == NULL)
        goto error;

    result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_server].window_size, msg_block_state_send_request);

    {
        idigi_msg_callback_t * const cb_fn = msg_ptr->service_cb[msg_service_id_data];

        if (cb_fn != NULL)
        {
            msg_service_request_t service_data;

            service_data.session = session;
            service_data.service_type = msg_service_type_pending_request;
            service_data.have_data = (void *)msg_ptr->pending_service_request;
            service_data.error_value = result;

            cb_fn(idigi_ptr, &service_data);
            if (service_data.error_value != idigi_msg_error_none)
                msg_delete_session(idigi_ptr, msg_ptr, session);
        }
    }

    msg_ptr->pending_service_request = NULL;

error:
    return;
}
#endif

static idigi_callback_status_t msg_send_ack(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * ack_packet;
    uint8_t * const edp_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, &ack_packet, NULL);
    msg_data_block_t * const dblock = session->in_dblock;

    if (edp_packet == NULL)
        goto done;

    ASSERT_GOTO(dblock != NULL, error);
    message_store_u8(ack_packet, opcode, msg_opcode_ack);
    {
        uint8_t const flag = MsgIsClientOwned(dblock->status_flag) ? 0 : MSG_FLAG_REQUEST;

        message_store_u8(ack_packet, flags, flag);
    }
    message_store_be16(ack_packet, transaction_id, session->session_id);
    message_store_be32(ack_packet, ack_count, dblock->total_bytes);
    message_store_be32(ack_packet, window_size, msg_ptr->capabilities[msg_capability_client].window_size);

    status = initiate_send_facility_packet(idigi_ptr, edp_packet, record_end(ack_packet),
                                           E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);
    if (status != idigi_callback_busy)
    {
        dblock->ack_count = dblock->total_bytes;
        MsgClearAckPending(dblock->status_flag);
        session->current_state = msg_state_receive;
    }

error:
done:
    return status;
}

static idigi_callback_status_t msg_process_capabilities(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * capability_packet = ptr;
    idigi_bool_t const request_capabilities = idigi_bool((message_load_u8(capability_packet, flags) & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST);
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
    msg_data_block_t * const dblock = session->in_dblock;

    ASSERT_GOTO(session != NULL, error);
    ASSERT_GOTO(dblock != NULL, error);

    {
        unsigned int service_flag = 0;
        msg_service_data_t * const service_data = session->service_layer_data.have_data;

        ASSERT_GOTO(service_data != NULL, error);
        if (MsgIsStart(dblock->status_flag))
            MsgSetStart(service_flag);

        if (MsgIsLastData(dblock->status_flag))
            MsgSetLastData(service_flag);

        service_data->data_ptr = data;
        service_data->length_in_bytes = bytes;
        service_data->flags = service_flag;
        if ((MsgIsDoubleBuf(dblock->status_flag)) && (session->service_layer_data.need_data->data_ptr == NULL))
        {
            status = msg_prepare_send_data(idigi_ptr, session);
            ASSERT_GOTO(status == idigi_callback_continue, error);
        }
        status = msg_call_service_layer(idigi_ptr, session, msg_service_type_have_data);
    }

    if (status == idigi_callback_abort)
        goto error;

    if (status == idigi_callback_continue)
    {
        MsgClearStart(dblock->status_flag);

        if (MsgIsLastData(dblock->status_flag))
        {
            if (MsgIsRequest(dblock->status_flag))
            {
                idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
                idigi_msg_error_t result;

                ASSERT_GOTO(msg_ptr != NULL, error);
                if (MsgIsDoubleBuf(dblock->status_flag) == idigi_false)
                {
                    result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_server].window_size, msg_block_state_send_response);
                    if (result != idigi_msg_error_none)
                        status = msg_inform_error(idigi_ptr, session, result);
                }
                else
                {
                    MsgClearRequest(dblock->status_flag);
                    MsgClearRequest(session->out_dblock->status_flag);
                }
            }
            else
                session->current_state = msg_state_delete;
        }
        else
        {
            dblock->total_bytes += bytes;

            if ((dblock->total_bytes - dblock->ack_count) > (dblock->available_window/2))
            {
                MsgSetAckPending(dblock->status_flag);
                session->current_state = msg_state_send_ack;
            }
            else
                session->current_state = msg_state_receive;
        }
    }

    if (MsgIsDoubleBuf(dblock->status_flag))
    {
        ASSERT_GOTO(session->service_layer_data.need_data != NULL, error);
        session->saved_state = session->current_state;

        if (MsgIsSendNow(session->service_layer_data.need_data->flags))
        {
            idigi_callback_status_t const send_status = msg_process_send_data(idigi_ptr, session);

            ASSERT_GOTO(send_status != idigi_callback_abort, error);
            session->service_layer_data.need_data->data_ptr = NULL;
        }
    }

error:
    return status;
}

#if (defined IDIGI_COMPRESSION)
static idigi_callback_status_t msg_process_decompressed_data(idigi_data_t * const idigi_ptr, msg_session_t * const session)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_data_block_t * const dblock = session->in_dblock;

    ASSERT_GOTO(dblock != NULL, error);
    ASSERT_GOTO(dblock->bytes_out > 0, error);

    status = msg_pass_service_data(idigi_ptr, session, dblock->buffer_out, dblock->bytes_out);
    switch (status)
    {
    case idigi_callback_continue:
        dblock->bytes_out = 0;
        if (session->current_state != msg_state_get_data)
        {
            if (dblock->zlib.avail_out == 0)
                session->current_state = msg_state_decompress;
            else
            {
                if (MsgIsAckPending(dblock->status_flag))
                    session->current_state = msg_state_send_ack;
            }
        }
        break;

    case idigi_callback_busy:
        session->current_state = msg_state_process_decompressed;
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
    msg_data_block_t * const dblock = session->in_dblock;
    z_streamp zlib_ptr;

    ASSERT_GOTO(dblock != NULL, error);
    zlib_ptr = &dblock->zlib;

    if (zlib_ptr->avail_out == 0)
    {
        zlib_ptr->next_out = dblock->buffer_out;
        zlib_ptr->avail_out = sizeof dblock->buffer_out;
    }

    {
        int const zret = inflate(zlib_ptr, Z_NO_FLUSH);

        session->current_state = MsgIsAckPending(dblock->status_flag) ? msg_state_send_ack : msg_state_receive;
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
            MsgSetLastData(dblock->status_flag);
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
    msg_data_block_t * const dblock = session->in_dblock;
    z_streamp zlib_ptr;

    ASSERT_GOTO(dblock != NULL, error);
    zlib_ptr = &dblock->zlib;

    if (zlib_ptr->avail_in > 0)
    {
        status = idigi_callback_busy;
        goto done;
    }

    ASSERT_GOTO(bytes < sizeof dblock->buffer_in, error);
    memcpy(dblock->buffer_in, data, bytes);

    zlib_ptr->next_in = dblock->buffer_in;
    zlib_ptr->avail_in = bytes;

    status = msg_decompress_data(idigi_ptr, session);

error:
done:
    return status;
}
#endif

static idigi_callback_status_t msg_process_service_data(idigi_data_t * const idigi_ptr, msg_session_t * const session, uint8_t * msg_data, size_t const frame_bytes, size_t const header_bytes, unsigned const flag)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * buffer = msg_data + header_bytes;
    size_t const bytes = frame_bytes - header_bytes;
    msg_data_block_t * const dblock = session->in_dblock;

    ASSERT_GOTO(dblock != NULL, error);
    if (session->current_state != msg_state_receive)
    {
        status = ((session->current_state == msg_state_send_error) || (session->current_state == msg_state_delete)) ? idigi_callback_continue : idigi_callback_busy;
        goto done;
    }

    #if (defined IDIGI_COMPRESSION)
    if (MsgIsCompressed(dblock->status_flag))
    {
        status = msg_process_compressed_data(idigi_ptr, session, buffer, bytes);
        goto done;
    }
    #endif

    if (MsgIsLastData(flag))
        MsgSetLastData(dblock->status_flag);

    status = msg_pass_service_data(idigi_ptr, session, buffer, bytes);

error:
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
    idigi_bool_t const client_owned = MsgIsNotRequest(flag);

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
        if (session->out_dblock != NULL)
        {
            result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_client].window_size, msg_block_state_send_response);
            if (result != idigi_msg_error_none)
                goto error;
        }

        result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_client].window_size, msg_block_state_recv_request);
        if (result != idigi_msg_error_none)
            goto error;
    }
    else
    {
        if (session->current_state == msg_state_send_error)
            goto done;

        if (client_owned)
        {
            result = msg_initialize_data_block(session, msg_ptr->capabilities[msg_capability_client].window_size, msg_block_state_recv_response);
            if (result != idigi_msg_error_none)
                goto error;
        }
    }

    {
        uint8_t const compression = message_load_u8(start_packet, compression_id);

        #if (defined IDIGI_COMPRESSION)
        if (compression == MSG_COMPRESSION_NONE)
            MsgClearCompression(session->in_dblock->status_flag);
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
    idigi_bool_t const client_owned = MsgIsNotRequest(flag);

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
        msg_data_block_t * const dblock = session->out_dblock;

        ASSERT_GOTO(dblock != NULL, error);
        dblock->available_window = message_load_be32(ack_packet, window_size);
        dblock->ack_count = message_load_be32(ack_packet, ack_count);

        if (dblock->available_window > 0)
        {
            MsgClearAckPending(dblock->status_flag);
            if (session->current_state == msg_state_wait_ack)
                session->current_state = session->saved_state;
        }
    }

error:
done:
    return idigi_callback_continue;
}

static idigi_callback_status_t msg_process_error(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * const error_packet = ptr;
    uint8_t const flag = message_load_u8(error_packet, flags);
    idigi_bool_t const client_request_error = idigi_bool(MsgIsRequest(flag) && MsgIsNotSender(flag));
    idigi_bool_t const server_response_error = idigi_bool(MsgIsNotRequest(flag) && MsgIsSender(flag));
    idigi_bool_t const client_owned = idigi_bool(client_request_error || server_response_error);
    uint16_t const session_id = message_load_be16(error_packet, transaction_id);
    msg_session_t * const session = msg_find_session(msg_fac, session_id, client_owned);

    if (session != NULL)
    {
        if ((session->current_state != msg_state_delete) && (session->current_state != msg_state_send_error))
        {
            uint8_t const error_val = message_load_u8(error_packet, error_code);

            ASSERT_GOTO(error_val < idigi_msg_error_count, error);
            {
                idigi_msg_error_t const msg_error = (idigi_msg_error_t)error_val;

                status = msg_inform_error(idigi_ptr, session, msg_error);
                if (status != idigi_callback_busy)
                    msg_delete_session(idigi_ptr, msg_fac, session);
            }
        }
    }
    else
    {
        idigi_debug_printf("msg_process_error: unable to find session id = %d\n", session_id);
    }

error:
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

#if (defined IDIGI_DATA_SERVICE)
    if (msg_ptr->pending_service_request != NULL)
        msg_start_session(idigi_ptr, msg_ptr);
#endif

    msg_ptr->session_locked = idigi_true;
    if (msg_ptr->session.current == NULL)
        msg_ptr->session.current = msg_ptr->session.tail;
    msg_ptr->session_locked = idigi_false;

    *receive_timeout = MAX_RECEIVE_TIMEOUT_IN_SECONDS;
    if (msg_ptr->session.current != NULL)
    {
        msg_session_t * const session = msg_ptr->session.current;

        *receive_timeout = MIN_RECEIVE_TIMEOUT_IN_SECONDS;
        switch (session->current_state)
        {
        case msg_state_init:
        case msg_state_wait_ack:
        case msg_state_receive:
        case msg_state_wait_send_complete:
            msg_switch_session(msg_ptr, session);
            break;

        case msg_state_get_data:
            session->saved_state = msg_state_get_data;
            status = msg_get_service_data(idigi_ptr, session);
            if (status == idigi_callback_busy)
                msg_switch_session(msg_ptr, session);
            break;

        case msg_state_send_data:
            status = msg_send_data(idigi_ptr, session);
            break;

        #if (defined IDIGI_COMPRESSION)
        case msg_state_compress:
            status = msg_compress_data(idigi_ptr, session);
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
            idigi_debug_printf("Failed %X, state%d\n", session, session->current_state);
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
                idigi_debug_printf("msg_process: Invalid opcode\n");
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
    msg_session_t * session;

    ASSERT_GOTO(msg_ptr != NULL, error);
    session= msg_ptr->session.head;

    while(session != NULL)
    {
        msg_session_t * next_session = session->next;

        if (session->service_id == service_id)
        {
            if (session->current_state != msg_state_delete && session->current_state != msg_state_send_error)
                msg_inform_error(idigi_ptr, session, idigi_msg_error_cancel);

            msg_delete_session(idigi_ptr, msg_ptr, session);
        }

        session = next_session;
    }

    msg_ptr->pending_service_request = NULL;

error:
    return status;
}

static idigi_callback_status_t msg_delete_facility(idigi_data_t * const idigi_ptr, uint16_t const service_id)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    if (msg_ptr == NULL)
    {
        status = idigi_callback_continue;
        goto error;
    }

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
            idigi_request_t request_id;

            request_id.config_request = idigi_config_max_transaction;
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

#if (defined IDIGI_RCI_SERVICE)

#include "rci_service.h"

#if (!defined IDIGI_FIRMWARE_SERVICE)
#include "rci_fwstub.h"
#endif
#endif

