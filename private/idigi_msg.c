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
    msg_status_response
} msg_status_t;

typedef idigi_callback_status_t idigi_msg_callback_t(idigi_data_t * idigi_ptr, msg_status_t const msg_status, uint16_t session_id, uint8_t * buffer, size_t const length);

typedef struct msg_session_t
{
    uint16_t session_id;
    uint16_t service_id;
    uint32_t available_window;
    idigi_msg_callback_t * cb_func;
    union
    {
        idigi_packet_t packet;
        uint8_t frame[MSG_MAX_PACKET_SIZE];
    };
    uint8_t * user_data;
    size_t bytes_to_send;
    size_t bytes_sent;
    uint8_t compression;
    bool last_chunk;
    bool flow_controlled;
#if (defined _COMPRESSION)
    z_stream zlib;
#endif
    struct msg_session_t * next;
    struct msg_session_t * prev;
} msg_session_t;

typedef struct 
{
    uint32_t window_size; /* peer window size */
    uint16_t service_ids[msg_service_id_count];
    uint16_t cur_id;
    uint8_t max_transactions;
    uint8_t active_transactions;
    msg_session_t * session_head;
} idigi_msg_data_t;

typedef struct
{
    size_t header_length;
    uint8_t * header;
    size_t payload_length;
    uint8_t * payload;
    uint16_t flag;
} msg_data_info_t;

static void msg_send_complete(idigi_data_t * idigi_ptr, idigi_packet_t * packet, idigi_status_t status, void * user_data);

static msg_session_t * msg_find_session(idigi_msg_data_t const * const msg_ptr, uint16_t const id)
{
    msg_session_t * session = msg_ptr->session_head;
    
    while (session != NULL) 
    {
        if (session->session_id == id)
            break;

        session = session->next;
    }

    return session;
}

static uint16_t find_next_available_id(idigi_msg_data_t * const msg_ptr)
{
    uint16_t new_id = MSG_INVALID_SESSION_ID;
    uint8_t last_assigned_id = msg_ptr->cur_id;

    do
    {
        msg_ptr->cur_id++; /* wraps after 64k */

        if (msg_ptr->cur_id == MSG_INVALID_SESSION_ID)
            continue;

        if (msg_find_session(msg_ptr, msg_ptr->cur_id) == NULL)
        {
            new_id = msg_ptr->cur_id;
            break;
        }

    } while (msg_ptr->cur_id != last_assigned_id);

    return new_id;
}

static msg_session_t * msg_create_session(idigi_data_t * idigi_ptr, uint16_t const service_id, idigi_msg_callback_t service_cb)
{
    idigi_msg_data_t * msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    uint16_t session_id = MSG_INVALID_SESSION_ID;
    msg_session_t * session = NULL;

    ASSERT_GOTO(idigi_ptr != NULL, error);
    ASSERT_GOTO(msg_ptr != NULL, error);

    if ((msg_ptr->max_transactions != MSG_UNLIMITED_TRANSACTIONS) && 
        (msg_ptr->active_transactions >= msg_ptr->max_transactions))
    {
        goto error;
    }

    session_id = find_next_available_id(msg_ptr);
    ASSERT_GOTO(session_id != MSG_INVALID_SESSION_ID, error);

    {
        void *ptr;
        idigi_callback_status_t const status = malloc_data(idigi_ptr, sizeof(msg_session_t), &ptr);

        ASSERT_GOTO(status == idigi_callback_continue, error);
        session = ptr;
    }

    session->session_id = session_id;
    session->service_id = service_id;
    session->cb_func = service_cb;
    session->available_window = msg_ptr->window_size;
    session->compression = MSG_NO_COMPRESSION;

    session->prev = NULL;
    session->next = msg_ptr->session_head;
    if (msg_ptr->session_head != NULL) 
        msg_ptr->session_head->prev = session;
    msg_ptr->session_head = session;
    
    msg_ptr->active_transactions++;

error:
    return session;
}

static void msg_delete_session(idigi_data_t * idigi_ptr, uint16_t const session_id)
{
    idigi_msg_data_t * msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session = msg_find_session(msg_ptr, session_id);

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(session != NULL, error);

    {
        msg_session_t * next = session->next;
        msg_session_t * prev = session->prev;

        if (next != NULL) 
            next->prev = prev;
        if (prev != NULL) 
            prev->next = next;

        if (session == msg_ptr->session_head)
            msg_ptr->session_head = next; 
    }

    free_data(idigi_ptr, session);
    ASSERT_GOTO((msg_ptr->active_transactions > 0) || (msg_ptr->max_transactions == MSG_UNLIMITED_TRANSACTIONS), error);
    msg_ptr->active_transactions--;

error:
    return;
}

static uint16_t frame_service_list(idigi_msg_data_t const * const msg_ptr, uint8_t * buffer)
{
    uint16_t count = 0;
    uint16_t i;

    for (i = 0; i < msg_service_id_count; i++) 
    {
        if (msg_ptr->service_ids[i] !=  msg_service_id_none) 
        {        
            StoreBE16(buffer, msg_ptr->service_ids[i]);
            buffer += sizeof msg_ptr->service_ids[0];
            count++;
        }
    }

    return count;
}

static idigi_callback_status_t send_msg_capabilities(idigi_data_t * idigi_ptr, idigi_msg_data_t const * const msg_data, uint8_t const flag)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * cur_ptr;
    uint8_t * start_ptr;
    idigi_packet_t * packet;

    /* get packet pointer for constructing capability request */
    packet =(idigi_packet_t *) get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, sizeof(idigi_packet_t), &start_ptr);
    if ((packet == NULL) || (start_ptr == NULL))
        goto error;

    cur_ptr = start_ptr;

    /*
     * ------------------------------------------------------------------------------------------
     * |   0    |    1  |     2   |    3    |   4-7  |     8    | 9-(8+n) |     9+n   | s bytes |
     *  -----------------------------------------------------------------------------------------
     * | opcode | flags | version | Max Trx | Window | Count(n) | list    | Count (s) |  list   |
     * |        |       |         |         |  size  |  compression algo  |     Service Info    |
     *  -----------------------------------------------------------------------------------------
     */
    *cur_ptr++ = msg_opcode_capability;
    *cur_ptr++ = flag;
    *cur_ptr++ = MSG_FACILITY_VERSION;
    *cur_ptr++ = MSG_MAX_RECV_TRANSACTIONS;

    /* append window size */
    {
        uint32_t window = MSG_RECV_WINDOW_SIZE;

        StoreBE32(cur_ptr, window);
        cur_ptr += sizeof window;
    }

    /* append compression algorithms supported */
#if (defined _COMPRESSION)
    *cur_ptr++ = 1;
    *cur_ptr++ = MSG_COMPRESSION_LIBZ;
#else
    *cur_ptr++ = 0;
#endif

    /* append service IDs of all listeners */
    {
        uint16_t service_count = frame_service_list(msg_data, cur_ptr + 2);

        StoreBE16(cur_ptr, service_count);
        cur_ptr += sizeof service_count;
        cur_ptr += (sizeof(uint16_t) * service_count);
    }

    packet->header.length = cur_ptr - start_ptr;
    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);

error:
    return status;
}

static idigi_callback_status_t msg_send_packet(idigi_data_t *idigi_ptr, msg_session_t * session, msg_data_info_t * info, uint8_t * data)
{
    idigi_callback_status_t status = idigi_callback_busy;
    uint8_t * start = GET_PACKET_DATA_POINTER(session->frame, sizeof(idigi_packet_t));

    if (session->compression != MSG_NO_COMPRESSION)
    {
        ASSERT_GOTO(session->compression == MSG_COMPRESSION_LIBZ, error);
    }

    if (info != NULL) 
    {
        if (info->header_length > 0) 
        {
            memcpy(data, info->header, info->header_length);
            data += info->header_length;
        }
    }

    {
        size_t available_space = sizeof session->frame - (data - session->frame);
        
        if (available_space > session->available_window)
            available_space = session->available_window;
    
        if (available_space > session->bytes_to_send)
            available_space = session->bytes_to_send;
    
        memcpy(data, session->user_data, available_space);
        data += available_space;
        session->user_data += available_space;
        session->bytes_to_send -= available_space;
        session->bytes_sent += available_space;
        session->available_window -= available_space;

        session->flow_controlled = (session->available_window == 0);

        if (session->bytes_to_send > 0) 
        {
            /* clear the last data chunk flag if set */
            #define FLAG_BYTE_POSITION  1

            start[FLAG_BYTE_POSITION] &= ~MSG_DATA_LAST;
        }
    }

    /* packet length is already updated in the above layer */
    {
        idigi_packet_t * packet = &session->packet;

        packet->header.length = data - start;
        status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_MSG_NUM, msg_send_complete, session);
    }

error:
    return status;    
}

static idigi_callback_status_t msg_send_data(idigi_data_t *idigi_ptr, msg_session_t * session, msg_data_info_t * info)
{
    uint8_t * ptr = GET_PACKET_DATA_POINTER(session->frame, sizeof(idigi_packet_t));

    /*
     * ---------------------------------------
     * |   0    |    1  |   2-3   |  4 ... n |
     *  --------------------------------------
     * | opcode | flags |   Xid   |  Data    |
     *  --------------------------------------
     */

    if (info != NULL) 
    {
        session->bytes_to_send = info->payload_length;
        session->user_data = info->payload;
        session->last_chunk = (info->flag & IDIGI_DATA_REQUEST_LAST) != 0; 
    }

    *ptr++ = msg_opcode_data;
    *ptr++ = MSG_DATA_REQUEST | (session->last_chunk ? MSG_DATA_LAST : 0);
    StoreBE16(ptr, session->session_id);
    ptr += sizeof session->session_id;

    return msg_send_packet(idigi_ptr, session, info, ptr);
}

static void * msg_send_start(idigi_data_t *idigi_ptr, uint16_t const service_id, idigi_msg_callback_t callback, msg_data_info_t * info)
{
    msg_session_t * session = msg_create_session(idigi_ptr, service_id, callback);
    uint8_t * ptr;

    ASSERT_GOTO(session != NULL, error);
    session->bytes_to_send = info->payload_length;
    session->user_data = info->payload;

    ptr = GET_PACKET_DATA_POINTER(session->frame, sizeof(idigi_packet_t));

    /*
     * -----------------------------------------------------------------
     * |   0    |    1  |   2-3   |    4-5     |    6    |     7 ... n |
     *  ----------------------------------------------------------------
     * | opcode | flags |   Xid   | Service ID | Comp ID |     Data    |
     * |        |       |         |   (Only for start)   |             |
     *  ----------------------------------------------------------------
     */
    *ptr++ = msg_opcode_start;

    {
        uint8_t msg_flag = MSG_DATA_REQUEST;

        session->last_chunk = (info->flag & IDIGI_DATA_REQUEST_LAST) != 0;
        if (session->last_chunk)
            msg_flag |= MSG_DATA_LAST;

        *ptr++ = msg_flag;
    }

    StoreBE16(ptr, session->session_id);
    ptr += sizeof session->session_id;

    StoreBE16(ptr, session->service_id);
    ptr += sizeof session->service_id;

#if (defined _COMPRESSION)
    session->compression = (info->flag & IDIGI_DATA_REQUEST_COMPRESSED) ? MSG_COMPRESSION_LIBZ : MSG_NO_COMPRESSION;
#endif

    *ptr++ = session->compression;
    msg_send_packet(idigi_ptr, session, info, ptr);

error:
    return session;
}

static void msg_send_complete(idigi_data_t * idigi_ptr, idigi_packet_t * packet, idigi_status_t status, void * user_data)
{
    msg_session_t * session = user_data;

    UNUSED_PARAMETER(packet);
    ASSERT_GOTO(session != NULL, done);

    if (status == idigi_success)
    {
        if (session->bytes_to_send > 0)
        {
            if (!session->flow_controlled) 
                msg_send_data(idigi_ptr, session, NULL);

            goto done;
        }
    }

    if (session->cb_func) 
    {
        size_t bytes = session->bytes_sent;

        session->bytes_sent = 0;
        session->cb_func(idigi_ptr, msg_status_send_complete, session->session_id, NULL, bytes);
    }

done:
    return;
}

static idigi_callback_status_t process_capabilities(idigi_data_t * idigi_ptr, idigi_msg_data_t * const msg_fac, uint8_t * ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t request = *ptr++;
    uint8_t version = *ptr++;

    /* is version valid? */
    if (version != MSG_FACILITY_VERSION)
    {
        DEBUG_PRINTF("process_capabilities invalid version [%d]\n", version);
    }

    msg_fac->max_transactions = *ptr++;

    /* update window size */
    {
        uint32_t window = LoadBE32(ptr);

        msg_fac->window_size = window;
        ptr += sizeof window;
    }

    /* TODO: extract compression and match with supported */

    if (request) 
        status = send_msg_capabilities(idigi_ptr, msg_fac, MSG_CAPABILITIES_RESPONSE);

    return status;
}

static idigi_callback_status_t process_msg_data(idigi_data_t * idigi_ptr, idigi_msg_data_t const * const msg_fac, uint8_t * ptr, uint16_t length, bool const start)
{
    idigi_callback_status_t status = idigi_callback_abort;
    uint16_t service_id;
    uint8_t const * start_ptr = ptr;

    ptr++; /* flag? */

    {
        uint16_t session_id = LoadBE16(ptr);        
        msg_session_t * session = msg_find_session(msg_fac, session_id);

        ptr += sizeof session_id;

        if (start)
        {
            service_id = LoadBE16(ptr);
    
            if (session != NULL) 
            {
                ASSERT_GOTO(session->service_id == service_id, error);
            }
            else
            {
                /* TODO: server originated call, create a new session */
            }

            ptr += sizeof service_id;
        }
        else
        {
            ASSERT_GOTO(session != NULL, error);
            /* TODO: server originated call? process the data */
        }
        
        length -= (ptr - start_ptr);
        status = session->cb_func(idigi_ptr, start ? msg_opcode_start : msg_opcode_data, session_id, ptr, length);
    }

error:
    return status;
}

static idigi_callback_status_t process_msg_ack(idigi_data_t * idigi_ptr, idigi_msg_data_t const * const msg_fac, uint8_t const * ptr)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_session_t * session;
    uint16_t session_id;

    ptr++; /* flags, not used at this point */

    session_id = LoadBE16(ptr);        

    session = msg_find_session(msg_fac, session_id);
    ASSERT_GOTO(session != NULL, error);
    ptr += sizeof session_id;

    {
        uint32_t ack_count = LoadBE32(ptr);

        ptr += sizeof ack_count;
    }

    status = idigi_callback_continue;
    session->available_window = LoadBE32(ptr); /* window size */
    if (session->available_window > 0)
    {
        if (session->flow_controlled) 
        {
            session->flow_controlled = false;
            if (session->bytes_to_send > 0) 
                status = msg_send_data(idigi_ptr, session, NULL);
        }
    }

error:
    return status;
}

static idigi_callback_status_t process_msg_error(idigi_data_t * idigi_ptr, idigi_msg_data_t const * const msg_fac, uint8_t * ptr)
{
    idigi_callback_status_t status = idigi_callback_abort;

    ptr++;

    {
        uint16_t session_id = LoadBE16(ptr);        
        msg_session_t * session = msg_find_session(msg_fac, session_id);
        
        ASSERT_GOTO(session != NULL, error);
        ptr += sizeof session_id;
        status = session->cb_func(idigi_ptr, msg_status_error, session_id, ptr, sizeof(uint8_t));
    }

error:
    return status;
}

static idigi_callback_status_t msg_discovery(idigi_data_t *idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    UNUSED_PARAMETER(packet);

    return send_msg_capabilities(idigi_ptr, facility_data, MSG_FLAG_REQUEST);
}

static idigi_callback_status_t msg_process(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * const msg_ptr = facility_data;
    uint8_t opcode;
    uint8_t * ptr;

    DEBUG_PRINTF("idigi_msg_process...\n");

    ASSERT_GOTO(packet != NULL, error);

    ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
    opcode = *ptr++;
    packet->header.length--;

    switch (opcode) 
    {
        case msg_opcode_capability:
            status = process_capabilities(idigi_ptr, msg_ptr, ptr);
            break;

        case msg_opcode_start:
            status = process_msg_data(idigi_ptr, msg_ptr, ptr, packet->header.length, true);
            break;

        case msg_opcode_data:
            status = process_msg_data(idigi_ptr, msg_ptr, ptr, packet->header.length, false);
            break;

        case msg_opcode_ack:
            status = process_msg_ack(idigi_ptr, msg_ptr, ptr);
            break;

        case msg_opcode_error:
            status = process_msg_error(idigi_ptr, msg_ptr, ptr);
            break;

        default:            
            DEBUG_PRINTF("msg_process: Invalid opcode\n");
            break;
    }

error:
    return status;
}

static idigi_callback_status_t msg_delete_facility(idigi_data_t * idigi_ptr, uint16_t const service_id)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(service_id < msg_service_id_count, error);
    ASSERT_GOTO(msg_ptr->service_ids[service_id] != msg_service_id_none, error);

    msg_ptr->service_ids[service_id] = msg_service_id_none;

    /* is list empty? */
    {
        bool is_empty = true;
        int i;

        for(i = 0; i < msg_service_id_count; i++) 
        {
            if (msg_ptr->service_ids[i] != msg_service_id_none)
            {
                is_empty = false;
                break;
            }
        }

        if (is_empty) 
            status = del_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
        else
            status = idigi_callback_continue;
    }

error:
    return status;
}

static uint16_t msg_init_facility(idigi_data_t *idigi_ptr, uint16_t service_id)
{
    idigi_callback_status_t status = idigi_callback_abort;
    void * fac_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    idigi_msg_data_t * msg_ptr = fac_ptr;

    if (fac_ptr == NULL)
    {
        status = add_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM, &fac_ptr,
                                   sizeof *msg_ptr, msg_discovery, msg_process);

        ASSERT_GOTO(status == idigi_callback_continue, error);
        ASSERT_GOTO(fac_ptr != NULL, error);

        msg_ptr = fac_ptr;
        msg_ptr->window_size = 0;
        msg_ptr->cur_id = MSG_INVALID_SESSION_ID;

        {
            uint16_t i;

            for (i = 0; i < msg_service_id_count; i++) 
                msg_ptr->service_ids[i] = msg_service_id_none;
        }
    }

    msg_ptr->service_ids[service_id] = service_id;
    status = idigi_callback_continue;

error:
    return status;
}

