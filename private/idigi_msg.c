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

#define MSG_FLAG_RESPONSE     0x00
#define MSG_FLAG_REQUEST      0x01
#define MSG_FLAG_LAST_DATA    0x02
#define MSG_RECV_WINDOW_SIZE  16384

#define MSG_CAPABILITIES_REQUEST  0x01
#define MSG_CAPABILITIES_RESPONSE 0x00

#define MSG_FACILITY_VERSION  0x01

#define MSG_MAX_RECV_TRANSACTIONS   1
#define MSG_UNLIMITED_TRANSACTIONS  0

#define MSG_INVALID_SESSION_ID      0xFFFF

#define MSG_DATA_RESPONSE   0x00
#define MSG_DATA_REQUEST    0x01
#define MSG_DATA_LAST       0x02

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
    msg_status_send_complete,
    msg_status_error,
    msg_status_start,
    msg_status_data,
    msg_status_end
} msg_status_t;

typedef enum
{
    msg_type_tx,
    msg_type_rx,
    msg_type_count
} msg_type_t;

typedef enum
{
    msg_state_init,
    msg_state_start,
    msg_state_start_and_last,
    msg_state_data,
    msg_state_data_and_last
} msg_state_t;

typedef struct msg_session_t
{
    uint16_t session_id;
    uint16_t service_id;
    uint32_t available_window;
    msg_state_t state;
    uint8_t frame[MSG_MAX_PACKET_SIZE];
    uint8_t * user_data;
    size_t bytes_to_send;
    size_t total_bytes;
    size_t bytes_in_frame;
    uint8_t compression;
    bool last_chunk;
    bool flow_controlled;
    bool more_data;
#if (defined IDIGI_COMPRESSION_BUILTIN)
    z_stream zlib;
#endif
    struct msg_session_t * next;
    struct msg_session_t * prev;
} msg_session_t;

typedef idigi_callback_status_t idigi_msg_callback_t(idigi_data_t * const idigi_ptr, msg_status_t const msg_status, msg_session_t * const session, uint8_t const * buffer, size_t const length);

typedef struct 
{
    uint32_t peer_window;
    idigi_msg_callback_t * service_cb[msg_service_id_count];
    uint16_t cur_id;
    uint8_t max_transactions;
    uint8_t active_transactions;
    uint8_t peer_compression;
    msg_session_t * session_head[msg_type_count];
    msg_session_t * pending;
} idigi_msg_data_t;

typedef struct
{
    #define SERVICE_PATH_MAX_LENGTH    256
    #define SERVICE_PARAM_MAX_LENGTH   32
    #define SERVICE_HEADER_MAX_LENGTH  (SERVICE_PATH_MAX_LENGTH + SERVICE_PARAM_MAX_LENGTH)

    size_t header_length;
    uint8_t header[SERVICE_HEADER_MAX_LENGTH];
    size_t payload_length;
    uint8_t * payload;
    uint16_t flag;
} msg_data_info_t;

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

#define PeerSupportsUnlimitedTransactions(msg_ptr)  (msg_ptr->max_transactions == MSG_UNLIMITED_TRANSACTIONS)
static void msg_send_complete(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data);

static msg_session_t * msg_find_session(idigi_msg_data_t const * const msg_ptr, uint16_t const id, msg_type_t const type)
{
    msg_session_t * session = msg_ptr->session_head[type];

    while (session != NULL) 
    {
        if (session->session_id == id)
            break;

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
        msg_ptr->cur_id++; /* wraps around at 64k to search entire range */

        if (msg_ptr->cur_id == MSG_INVALID_SESSION_ID)
            continue;

        if (msg_find_session(msg_ptr, msg_ptr->cur_id, msg_type_tx) == NULL)
        {
            if (msg_find_session(msg_ptr, msg_ptr->cur_id, msg_type_rx) == NULL)
            {
                new_id = msg_ptr->cur_id;
                break;
            }
        }
    } while (msg_ptr->cur_id != last_assigned_id);

    return new_id;
}

static msg_session_t * msg_create_session(idigi_data_t * const idigi_ptr, uint16_t const service_id, msg_type_t const type)
{
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    uint16_t session_id = MSG_INVALID_SESSION_ID;
    msg_session_t * session = NULL;

    ASSERT_GOTO(idigi_ptr != NULL, error);
    ASSERT_GOTO(msg_ptr != NULL, error);

    if (!PeerSupportsUnlimitedTransactions(msg_ptr) && 
        (msg_ptr->active_transactions >= msg_ptr->max_transactions))
    {
        goto error;
    }

    if (type == msg_type_tx)
    {
        session_id = msg_find_next_available_id(msg_ptr);
        ASSERT_GOTO(session_id != MSG_INVALID_SESSION_ID, error);
    }

    {
        void *ptr;
        idigi_callback_status_t const status = malloc_data(idigi_ptr, sizeof(msg_session_t), &ptr);

        ASSERT_GOTO(status == idigi_callback_continue, error);
        session = ptr;
    }

    session->session_id = session_id;
    session->service_id = service_id;
    session->available_window = msg_ptr->peer_window;
    session->compression = MSG_NO_COMPRESSION;
    session->state = msg_state_init;
    session->bytes_in_frame = 0;
    session->bytes_to_send = 0;

    add_node(&msg_ptr->session_head[type], session);    
    msg_ptr->active_transactions++;

error:
    return session;
}

static void msg_delete_session(idigi_data_t * const idigi_ptr,  msg_session_t * const session, msg_type_t const type)
{
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(session != NULL, error);

    if (msg_ptr->pending == session)
        msg_ptr->pending = session->prev;

    del_node(&msg_ptr->session_head[type], session);

#if (defined IDIGI_COMPRESSION_BUILTIN)
    if (session->compression == MSG_COMPRESSION_LIBZ) 
    {
        if (type == msg_type_tx)
            deflateEnd(&session->zlib);
        else
            inflateEnd(&session->zlib);
    }
#endif

    free_data(idigi_ptr, session);
    ASSERT_GOTO(msg_ptr->active_transactions > 0, error);
    msg_ptr->active_transactions--;

error:
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
    message_store_u8(capabilty_packet, max_transactions, MSG_MAX_RECV_TRANSACTIONS);
    message_store_be32(capabilty_packet, window_size, MSG_RECV_WINDOW_SIZE);

    /* append compression algorithms supported */
    {
        #if (defined IDIGI_COMPRESSION_BUILTIN)
        uint8_t const compression_length =  1;
        #else
        uint8_t const compression_length =  0;
        #endif

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

static bool msg_fill_data(msg_session_t * const session, uint8_t * dest_ptr)
{
    size_t const available_space = sizeof session->frame - (dest_ptr - session->frame);
    size_t const bytes = (available_space < session->bytes_to_send) ? available_space : session->bytes_to_send;

    if (bytes > 0)
    {
        memcpy(dest_ptr, session->user_data, bytes);
        session->user_data += bytes;
        session->bytes_to_send -= bytes;
        session->bytes_in_frame += bytes;
        session->more_data = (session->bytes_to_send > 0);
    }

    return true;
}

static bool msg_frame_uncompressed(msg_session_t * const session, msg_data_info_t const * const info)
{
    bool success = false;
    uint8_t * out_ptr = GET_PACKET_DATA_POINTER(session->frame, PACKET_EDP_FACILITY_SIZE);

    switch (session->state) 
    {
    case msg_state_init:
        session->state = msg_state_start;
        session->more_data = false;
        /* intentional fall through */

    case msg_state_start:
        ASSERT_GOTO(info != NULL, error);
        ASSERT_GOTO(info->header_length > 0, error);
        out_ptr += MSG_START_HEADER_LEN;
        memcpy(out_ptr, info->header, info->header_length);
        out_ptr += info->header_length;
        session->bytes_in_frame = MSG_START_HEADER_LEN + info->header_length;
        success = msg_fill_data(session, out_ptr);
        if (session->last_chunk && !session->more_data)
            session->state = msg_state_start_and_last;
        break;
    
    case msg_state_data:
        out_ptr += MSG_DATA_HEADER_LEN;
        session->bytes_in_frame = MSG_DATA_HEADER_LEN;
        success = msg_fill_data(session, out_ptr);
        if (session->last_chunk && !session->more_data)
            session->state = msg_state_data_and_last;
        break;

    default:
        ASSERT_GOTO(false, error);
        break;
    }

error:
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

static bool msg_frame_compressed(msg_session_t * const session, msg_data_info_t const * const info)
{
    bool success = false;
    z_streamp zlib_ptr = &session->zlib;
    uint8_t * out_ptr = GET_PACKET_DATA_POINTER(session->frame, PACKET_EDP_FACILITY_SIZE);
    size_t const frame_size = sizeof session->frame - (out_ptr - session->frame);
    size_t chunk_size = frame_size;
    int zret;

    switch (session->state)
    {
    case msg_state_init:
        ASSERT_GOTO(info != NULL, error);
        ASSERT_GOTO(info->header_length > 0, error);

        memset(zlib_ptr, 0, sizeof *zlib_ptr); /* sorry, many fields need initialization */
        zret = deflateInit(zlib_ptr, Z_DEFAULT_COMPRESSION);
        ASSERT_GOTO(zret == Z_OK, error);

        out_ptr += MSG_START_HEADER_LEN;
        chunk_size -= MSG_START_HEADER_LEN;

        MSG_FILL_ZLIB(zlib_ptr, (uint8_t *)info->header, info->header_length, out_ptr, chunk_size);
        zret = deflate(zlib_ptr, Z_NO_FLUSH);
        ASSERT_GOTO(zret == Z_OK, error);
        ASSERT_GOTO(zlib_ptr->avail_in == 0, error);
        session->state = msg_state_start;
        session->more_data = false;
        /* intentional fall through */

    case msg_state_start:
    case msg_state_data:
        if (zlib_ptr->avail_out == 0)
        {
            ASSERT_GOTO(session->state == msg_state_data, error);
            out_ptr += MSG_DATA_HEADER_LEN;
            chunk_size -= MSG_DATA_HEADER_LEN;
            MSG_FILL_ZLIB(zlib_ptr, session->user_data, session->bytes_to_send, out_ptr, chunk_size);
        }
        else
        {
            zlib_ptr->next_in = session->user_data;
            zlib_ptr->avail_in = session->bytes_to_send;
        }

        zret = deflate(zlib_ptr, session->last_chunk ? Z_FINISH : Z_NO_FLUSH);        
        ASSERT_GOTO(zret == Z_OK || zret == Z_STREAM_END, error);

        session->more_data = false;
        if (zret == Z_STREAM_END)
        {        
            if (session->state == msg_state_start)
            {
                /* is compressed size more than uncompressed? */
                if (zlib_ptr->total_out > zlib_ptr->total_in)
                {
                    success = msg_frame_uncompressed(session, info);
                    session->compression = MSG_NO_COMPRESSION;
                    goto error;
                }
                session->state = msg_state_start_and_last;
            }
            else
                session->state = msg_state_data_and_last;

            session->bytes_in_frame = frame_size - zlib_ptr->avail_out;
        }
        else if (zlib_ptr->avail_out == 0)
        {
            session->bytes_in_frame = frame_size;
            session->more_data = true;
        }

        {
            size_t const bytes_processed = session->bytes_to_send - zlib_ptr->avail_in;

            session->user_data += bytes_processed;
            session->bytes_to_send -= bytes_processed;
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
    idigi_callback_status_t status = idigi_callback_abort;
    uint8_t * ptr = GET_PACKET_DATA_POINTER(session->frame, PACKET_EDP_FACILITY_SIZE);

    switch (session->state) 
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
                uint8_t const flag = MSG_DATA_REQUEST | ((session->state ==  msg_state_start_and_last) ? MSG_DATA_LAST : 0);

                message_store_u8(start_packet, flags, flag);
            }
    
            message_store_be16(start_packet, transaction_id, session->session_id);    
            message_store_be16(start_packet, service_id, session->service_id);
            message_store_u8(start_packet, compression_id, session->compression);
    
            session->state = msg_state_data;
        }
        break;

    case msg_state_data:
    case msg_state_data_and_last:
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
                uint8_t const flag = MSG_DATA_REQUEST | ((session->state ==  msg_state_data_and_last) ? MSG_DATA_LAST : 0);

                message_store_u8(data_packet, flags, flag);
            }

            message_store_be16(data_packet, transaction_id, session->session_id);
        }
        break;

    default:
        ASSERT_GOTO(false, error);
        break;
    } 

    session->available_window -= session->bytes_in_frame;
    session->flow_controlled = (session->available_window < sizeof session->frame);
    status = idigi_callback_continue;

error:
    return status;
}

static idigi_callback_status_t msg_send_data(idigi_data_t * const idigi_ptr, msg_session_t * const session, msg_data_info_t const * const info)
{
    idigi_callback_status_t status = idigi_callback_busy;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(session != NULL, error);
    ASSERT_GOTO(msg_ptr != NULL, error);

    if (info != NULL) 
    {
        if (session->bytes_to_send > 0)
            goto error;

        session->bytes_to_send = info->payload_length;
        session->user_data = info->payload;
        session->total_bytes = info->payload_length;
        session->bytes_in_frame = 0;
        session->last_chunk = (info->flag & IDIGI_DATA_REQUEST_LAST) != 0;
        #if (defined IDIGI_COMPRESSION_BUILTIN)
        session->compression = (info->flag & IDIGI_DATA_REQUEST_COMPRESSED) ? msg_ptr->peer_compression : MSG_NO_COMPRESSION;
        #endif
    }

    {
        bool success = false; 

        #if (defined IDIGI_COMPRESSION_BUILTIN)
        if (session->compression == MSG_COMPRESSION_LIBZ)
        {
            success = msg_frame_compressed(session, info);
        }
        else
        #endif
        {
            success = msg_frame_uncompressed(session, info);
        }

        ASSERT_GOTO(success, error);
    }

    status = (session ->bytes_in_frame > 0) ? msg_send_packet(session) : idigi_callback_continue;
    if (msg_ptr->pending == NULL)
        msg_ptr->pending = session;

error:
    return status;
}

static void * msg_send_start(idigi_data_t * const idigi_ptr, uint16_t const service_id, msg_data_info_t const * const info)
{
    msg_session_t * const session = msg_create_session(idigi_ptr, service_id, msg_type_tx);

    ASSERT_GOTO(session != NULL, error);

    {
        idigi_callback_status_t const status = msg_send_data(idigi_ptr, session, info);

        ASSERT_GOTO(status == idigi_callback_continue, error);
    }

error:
    return session;
}

static void msg_send_complete(idigi_data_t * const idigi_ptr, uint8_t const * const packet, idigi_status_t const status, void * const user_data)
{
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * const session = user_data;

    UNUSED_PARAMETER(packet);
    ASSERT_GOTO(session != NULL, done);

    if (status == idigi_success)
    {
        if (session->more_data)
        {
            if (!session->flow_controlled) 
                msg_send_data(idigi_ptr, session, NULL);

            goto done;
        }
    }

    {
        idigi_msg_callback_t * cb_fn = msg_ptr->service_cb[session->service_id];
        size_t const bytes = session->total_bytes - session->bytes_to_send;

        ASSERT_GOTO(cb_fn != NULL, done);
        cb_fn(idigi_ptr, msg_status_send_complete, session, (session->user_data - bytes), bytes);
        if ((status != idigi_success) || (!session->more_data && session->last_chunk))
            msg_delete_session(idigi_ptr, session, msg_type_tx);
    }

done:
    return;
}

static idigi_callback_status_t msg_process_capabilities(idigi_data_t * const idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * capabilty_packet = ptr;
    bool const request_capabilities = (message_load_u8(capabilty_packet, flags) & MSG_DATA_REQUEST) == MSG_DATA_REQUEST;
    uint8_t const version = message_load_u8(capabilty_packet, version);

    ASSERT_GOTO(version == MSG_FACILITY_VERSION, error);
    msg_fac->max_transactions = message_load_u8(capabilty_packet, max_transactions);
    msg_fac->peer_window = message_load_be32(capabilty_packet, window_size);

    {
        uint8_t const comp_count = message_load_u8(capabilty_packet, compression_count);
        uint8_t const * compression_list = capabilty_packet + record_bytes(capabilty_packet);
        int i;

        msg_fac->peer_compression = MSG_NO_COMPRESSION;
        for (i = 0; i < comp_count; i++) 
        {
            uint8_t const compression_id = *compression_list++;

            if (compression_id == MSG_COMPRESSION_LIBZ)
            {
                msg_fac->peer_compression = compression_id;
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


#if (defined IDIGI_COMPRESSION_BUILTIN)
static idigi_callback_status_t msg_recv_compressed(idigi_data_t * const idigi_ptr, idigi_msg_data_t const * const msg_fac, msg_session_t * const session, uint8_t * ptr, uint16_t length)
{
    idigi_callback_status_t status = idigi_callback_abort;
    z_streamp zlib_ptr = &session->zlib;

    ASSERT_GOTO(zlib_ptr->avail_in == 0, error);
    zlib_ptr->next_in = ptr;
    zlib_ptr->avail_in = length;

    do
    {
        int const ret = inflate(zlib_ptr, Z_NO_FLUSH);

        if (ret == Z_BUF_ERROR) /* no more input buffer to process */
            break;

        if ((ret == Z_STREAM_END) || (zlib_ptr->avail_out == 0))
        {
            msg_status_t state;
            size_t uncompressed_length = sizeof session->frame - zlib_ptr->avail_out;
            idigi_msg_callback_t * cb_fn = msg_fac->service_cb[session->service_id];

            if (session->last_chunk && (zlib_ptr->avail_out == 0))
                state = msg_status_end;
            else
                state = (zlib_ptr->total_in == length) ? msg_status_start : msg_status_data;
            
            status = cb_fn(idigi_ptr, state, session, session->frame, uncompressed_length);

            if (session->last_chunk && (zlib_ptr->avail_out == 0))
                msg_delete_session(idigi_ptr, session, msg_type_rx);
        }

    } while (zlib_ptr->avail_in > 0);

    status = idigi_callback_continue;

error:
    return status;
}
#endif

static idigi_callback_status_t msg_process_data(idigi_data_t * const idigi_ptr, idigi_msg_data_t const * const msg_fac, uint8_t * ptr, uint16_t const length, bool const start)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_session_t * session;
    uint8_t * start_packet = ptr;
    uint8_t const flag = message_load_u8(start_packet, flags);
    uint16_t const session_id = message_load_be16(start_packet, transaction_id);
    size_t pkt_length = 0;

    if (start)
    {
        uint16_t const service_id = message_load_be16(start_packet, service_id);

        session = msg_create_session(idigi_ptr, service_id, msg_type_rx);
        ASSERT_GOTO(session != NULL, error);
        session->session_id = session_id;
        session->compression = message_load_u8(start_packet, compression_id);;

        #if (defined IDIGI_COMPRESSION_BUILTIN)
        if (session->compression == MSG_COMPRESSION_LIBZ)
        {
            z_streamp zlib_ptr = &session->zlib;

            memset(zlib_ptr, 0, sizeof session->zlib);
            {
                int const ret = inflateInit(zlib_ptr);

                ASSERT_GOTO(ret == Z_OK, error);
                zlib_ptr->next_out = session->frame;
                zlib_ptr->avail_out = sizeof session->frame;
            }
        }
        #else
        ASSERT_GOTO(session->compression == MSG_NO_COMPRESSION, error);
        #endif
        pkt_length = record_bytes(start_packet);
    }
    else
    {
        session = msg_find_session(msg_fac, session_id, msg_type_rx);
        ASSERT_GOTO(session != NULL, error);
        pkt_length = record_bytes(data_packet);
    }

    session->last_chunk = (flag & MSG_FLAG_LAST_DATA) == MSG_FLAG_LAST_DATA;
    {
        uint8_t *data_ptr = start_packet + pkt_length;
        size_t const data_length = length - pkt_length;

        #if (defined IDIGI_COMPRESSION_BUILTIN)
        if (session->compression == MSG_COMPRESSION_LIBZ) 
            status = msg_recv_compressed(idigi_ptr, msg_fac, session, data_ptr, data_length);
        else
        #endif
        {
            idigi_msg_callback_t * cb_fn = msg_fac->service_cb[session->service_id];
            msg_status_t state = start ? msg_status_start : msg_status_data;
    
            if (session->last_chunk)
                state = msg_status_end;
            status = cb_fn(idigi_ptr, state, session, data_ptr, data_length);
            if (session->last_chunk) 
                msg_delete_session(idigi_ptr, session, msg_type_rx);
        }
    }

    if ((flag & MSG_FLAG_REQUEST) == MSG_FLAG_REQUEST)
    {
        /* TODO: send response, needed for file handling */
    }

error:
    return status;
}

static idigi_callback_status_t msg_process_ack(idigi_data_t * const idigi_ptr, idigi_msg_data_t const * const msg_fac, uint8_t const * ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session;
    uint8_t const * ack_packet = ptr;

    {
        uint16_t const session_id = message_load_be16(ack_packet, transaction_id);

        session = msg_find_session(msg_fac, session_id, msg_type_tx);
        if (session == NULL)
            goto error; /* already done sending all data */    
    }

    session->available_window = message_load_be32(ack_packet, window_size);
    if (session->available_window > 0)
    {
        if (session->flow_controlled) 
        {
            session->flow_controlled = false;
            if (session->more_data) 
                status = msg_send_data(idigi_ptr, session, NULL);
        }
    }

error:
    return status;
}

static idigi_callback_status_t msg_process_error(idigi_data_t * const idigi_ptr, idigi_msg_data_t const * const msg_fac, uint8_t * const ptr)
{
    idigi_callback_status_t status = idigi_callback_abort;
    uint8_t * error_packet = ptr;
   
    {
        uint16_t const session_id = message_load_be16(error_packet, transaction_id);

        msg_session_t * const session = msg_find_session(msg_fac, session_id, msg_type_tx);
        ASSERT_GOTO(session != NULL, error);

        {
            idigi_msg_callback_t * cb_fn = msg_fac->service_cb[session->service_id];
            uint8_t const error = message_load_u8(error_packet, error_code); 

            status = cb_fn(idigi_ptr, msg_status_error, session, &error, sizeof error);
            msg_delete_session(idigi_ptr, session, msg_type_tx);
        }
    }

error:
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

    if ((msg_ptr != NULL) && (msg_ptr->pending != NULL))
    {
        msg_session_t * session = msg_ptr->pending;

        if (session->bytes_in_frame > 0) 
        {
            status = initiate_send_facility_packet(idigi_ptr, session->frame, session->bytes_in_frame, E_MSG_FAC_MSG_NUM, msg_send_complete, session);
            if (status == idigi_callback_continue)
            {
                session->bytes_in_frame = 0;
                if (!session->more_data)
                {
                    session = session->prev;

                    if ((session != NULL) && (session->bytes_in_frame > 0)) 
                        msg_ptr->pending = session;
                }
            }
        }
        else
            msg_send_complete(idigi_ptr, NULL, idigi_success, session);
    }

    return status;
}

static void msg_delete_all_sessions(idigi_data_t * const idigi_ptr,  idigi_msg_data_t * const msg_ptr, uint16_t const service_id)
{
    msg_type_t msg_type;

    for (msg_type = msg_type_tx; msg_type < msg_type_count; msg_type++) 
    {
        msg_session_t * session = msg_ptr->session_head[msg_type];

        while(session != NULL)
        {
            msg_session_t * next_session = session->next;

            if (session->service_id ==service_id )
                msg_delete_session(idigi_ptr, session, msg_type);
            session = next_session;
        }
    }
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
        status = is_empty ? del_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM) : idigi_callback_continue;
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

        ASSERT_GOTO(status == idigi_callback_continue, error);
        ASSERT_GOTO(fac_ptr != NULL, error);

        msg_ptr = fac_ptr;
        memset(msg_ptr, 0, sizeof *msg_ptr);
    }

    msg_ptr->service_cb[service_id] = callback;
    status = idigi_callback_continue;

error:
    return status;
}

