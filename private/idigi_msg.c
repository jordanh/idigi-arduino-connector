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

#define IDIGI_MSG_FLAG_RESPONSE     0x00
#define IDIGI_MSG_FLAG_REQUEST      0x01
#define IDIGI_MSG_FLAG_LAST_DATA    0x02
#define IDIGI_MSG_RECV_WINDOW_SIZE  16384

#define IDIGI_CAPABILITIES_REQUEST  0x01
#define IDIGI_CAPABILITIES_RESPONSE 0x00

#define IDIGI_MSG_FACILITY_VERSION  0x01
#define IDIGI_MSG_MAX_TRANSACTIONS  1

#define MSG_NO_COMPRESSION          0

#define MSG_INVALID_SESSION_ID      0xFFFF

#define MSG_DATA_RESPONSE   0x00
#define MSG_DATA_REQUEST    0x01
#define MSG_DATA_LAST       0x02

typedef enum
{
    msg_service_id_none,
    msg_service_id_data,
    msg_service_id_file,
    msg_service_id_count
} msg_service_id_t;

typedef enum
{
    msg_compression_none,
    msg_compression_count
} msg_compression_t;

typedef enum
{
    msg_opcode_capability,
    msg_opcode_start,
    msg_opcode_data,
    msg_opcode_ack,
    msg_opcode_error,
    msg_opcode_count
} msg_opcode_t;

typedef idigi_callback_status_t idigi_msg_callback_t(idigi_data_t * idigi_ptr, msg_opcode_t msg_type, uint8_t * buffer, size_t length);

typedef struct msg_session_t
{
    uint16_t session_id;
    uint16_t service_id;
    idigi_msg_callback_t * cb_func;
    uint8_t compression_id;
    struct msg_session_t * next;
    struct msg_session_t * prev;
} msg_session_t;

typedef struct 
{
    uint32_t window_size; /* peer window size */
    uint16_t service_ids[msg_service_id_count];
    uint8_t  compression_list[msg_compression_count];
    uint16_t cur_id;
    msg_session_t * session_head;
} idigi_msg_data_t;

static msg_session_t * msg_find_session(idigi_msg_data_t * msg_ptr, uint16_t id)
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

static uint16_t find_next_available_id(idigi_msg_data_t * msg_ptr)
{
    /* Note: this loop will block if we have 64k (realistic?) sessions at any time */
    while (1) 
    {
        msg_ptr->cur_id++; /* wraps around after 64k */

        if (msg_ptr->cur_id == MSG_INVALID_SESSION_ID)
            continue;

        if (msg_find_session(msg_ptr, msg_ptr->cur_id) == NULL)
            break;
    }

    return msg_ptr->cur_id;
}

static uint16_t msg_create_session(idigi_data_t * idigi_ptr, uint16_t const service_id, idigi_msg_callback_t service_cb)
{
    idigi_msg_data_t * msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    uint16_t session_id = MSG_INVALID_SESSION_ID;
    void *ptr;

    ASSERT_GOTO(idigi_ptr != NULL, error);
    ASSERT_GOTO(msg_ptr != NULL, error);

    {
        idigi_callback_status_t const status = malloc_data(idigi_ptr, sizeof(msg_session_t), &ptr);

        ASSERT_GOTO(status == idigi_callback_continue, error);
    }

    {
        msg_session_t * session = ptr;

        session->service_id = service_id;
        session->cb_func = service_cb;
        session->session_id = find_next_available_id(msg_ptr);
        session->compression_id = MSG_NO_COMPRESSION;
        session->prev = NULL;
        session->next = msg_ptr->session_head;
        if (msg_ptr->session_head != NULL) 
            msg_ptr->session_head->prev = session;
        msg_ptr->session_head = session;
    
        session_id = session->session_id;
    }

error:
    return session_id;
}

static void msg_delete_session(idigi_data_t * idigi_ptr, uint16_t const service_id)
{
    idigi_msg_data_t * msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session = msg_find_session(msg_ptr,service_id);

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

error:
    return;
}

static uint8_t update_compression_list(idigi_msg_data_t * msg_ptr, uint8_t * buffer)
{
    uint8_t count = 0;
    uint8_t i;

    for (i = 0; i < msg_compression_count; i++)
    {
        if (msg_ptr->compression_list[i] != msg_compression_none) 
        {
            *buffer++ = msg_ptr->compression_list[i];
            count++;
        }
    }

    return count;
}

static uint16_t update_service_list(idigi_msg_data_t * msg_ptr, uint8_t * buffer)
{
    uint16_t count = 0;
    uint16_t i;

    for (i = 0; i < msg_service_id_count; i++) 
    {
        if (msg_ptr->service_ids[i] !=  msg_service_id_none) 
        {        
            StoreBE16(buffer, msg_ptr->service_ids[i]);
            buffer += sizeof(uint16_t);
            count++;
        }
    }

    return count;
}

static idigi_callback_status_t send_msg_capabilities(idigi_data_t * idigi_ptr, idigi_msg_data_t * msg_data, uint8_t flag)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * cur_ptr;
    uint8_t * start_ptr;
    idigi_packet_t * packet;

    /* get packet pointer for constructing capability request */
    packet =(idigi_packet_t *) get_packet_buffer(idigi_ptr, E_MSG_FAC_MSG_NUM, sizeof(idigi_packet_t), &start_ptr);
    if ((packet == NULL) || (start_ptr == NULL))
    {
        status = idigi_callback_busy;
        goto error;
    }

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
    *cur_ptr++ = IDIGI_MSG_FACILITY_VERSION;
    *cur_ptr++ = IDIGI_MSG_MAX_TRANSACTIONS;

    /* append window size */
    {
        uint32_t window = IDIGI_MSG_RECV_WINDOW_SIZE;

        StoreBE32(cur_ptr, window);
        cur_ptr += sizeof window;
    }

    /* append compression algorithms supported */
    {
        uint8_t algo_count = update_compression_list(msg_data, cur_ptr+1);

        *cur_ptr++ = algo_count;
        cur_ptr += algo_count;
    }

    /* append service IDs of all listeners */
    {
        uint16_t service_count = update_service_list(msg_data, cur_ptr + 2);

        StoreBE16(cur_ptr, service_count);
        cur_ptr += sizeof service_count;
        cur_ptr += (sizeof(uint16_t) * service_count);
    }

    packet->header.length = cur_ptr - start_ptr;
    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_MSG_NUM, release_packet_buffer, NULL);

error:
    return status;
}

static idigi_callback_status_t process_capabilities(idigi_data_t * idigi_ptr, idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t request = *ptr++;
    uint8_t version = *ptr++;

    /* is version valid? */
    if (version != IDIGI_MSG_FACILITY_VERSION)
    {
        DEBUG_PRINTF("process_capabilities invalid version [%d]\n", version);
    }

    ptr++;  //TODO: store peer max transaction option

    /* update window size */
    {
        uint32_t window = LoadBE32(ptr);

        if (window != 0) 
            msg_fac->window_size = window;

        ptr += sizeof window;
    }

    /* TODO: extract compression and match with supported */

    if (request) 
        status = send_msg_capabilities(idigi_ptr, msg_fac, IDIGI_CAPABILITIES_RESPONSE);

    return status;
}

static idigi_callback_status_t process_message(idigi_data_t * idigi_ptr, idigi_msg_data_t * msg_fac, uint8_t *ptr, uint16_t length, bool start)
{
    idigi_callback_status_t status = idigi_callback_abort;
    uint16_t service_id;
    uint8_t * start_ptr = ptr;

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
            ptr++; /* compression? */
        }
        else
        {
            ASSERT_GOTO(session != NULL, error);
        }
        
        length = (ptr - start_ptr);
        status = session->cb_func(idigi_ptr, start ? msg_opcode_start : msg_opcode_data, ptr, length);
    }

error:
    return status;
}

static idigi_callback_status_t process_msg_ack(idigi_data_t * idigi_ptr, idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    UNUSED_PARAMETER(idigi_ptr);

    ptr++; /* flags, not used at this point */
    ptr += sizeof(uint16_t); /* session id */

    {
        uint32_t ack_count = LoadBE32(ptr);

        ptr += sizeof ack_count;
    }

    msg_fac->window_size = LoadBE32(ptr);

    return idigi_callback_continue;
}

static idigi_callback_status_t process_msg_error(idigi_data_t * idigi_ptr, idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    idigi_callback_status_t status = idigi_callback_abort;

    ptr++;

    {
        uint16_t session_id = LoadBE16(ptr);        
        msg_session_t * session = msg_find_session(msg_fac, session_id);
        
        ASSERT_GOTO(session != NULL, error);
        status = session->cb_func(idigi_ptr, msg_opcode_error, ptr, (sizeof session_id + sizeof(uint8_t)));
    }

error:
    return status;
}

static idigi_status_t msg_send_data(idigi_data_t *idigi_ptr, uint16_t session_id, idigi_packet_t * packet, uint8_t const flags, send_complete_cb_t complete_cb)
{
    idigi_status_t ret_status = idigi_service_busy;
    uint8_t * ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
    bool const start = ((flags & IDIGI_DATA_REQUEST_START) != 0);

    /*
     * -----------------------------------------------------------------
     * |   0    |    1  |   2-3   |    4-5     |    6    |     7 ... n |
     *  ----------------------------------------------------------------
     * | opcode | flags |   Xid   | Service ID | Comp ID |     Data    |
     * |        |       |         |   (Only for start)   |             |
     *  ----------------------------------------------------------------
     */
    {
        uint8_t const opcode = start ? msg_opcode_start : msg_opcode_data;

        *ptr++ = opcode;
    }

    {
        uint8_t msg_flag = MSG_DATA_REQUEST;

        msg_flag |= (flags & IDIGI_DATA_REQUEST_LAST) ? MSG_DATA_LAST : 0;
        *ptr++ = msg_flag;
    }

    StoreBE16(ptr, session_id);
    ptr += sizeof session_id;

    if (start) 
    {
        idigi_msg_data_t * msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
        msg_session_t * session = msg_find_session(msg_ptr, session_id);

        StoreBE16(ptr, session->service_id);
        ptr += sizeof session->service_id;

        *ptr++ = session->compression_id;
    }

    /* packet length is already updated in the above layer */
    {
        idigi_callback_status_t status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_MSG_NUM, complete_cb, NULL);

        ASSERT_GOTO(status == idigi_callback_continue, error);
    }

    ret_status = idigi_success;

error:
    return ret_status;
}

static idigi_callback_status_t msg_discovery(idigi_data_t *idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    UNUSED_PARAMETER(packet);

    return send_msg_capabilities(idigi_ptr, facility_data, IDIGI_MSG_FLAG_REQUEST);
}

static idigi_callback_status_t msg_process(idigi_data_t * idigi_ptr, void * facility_data, idigi_packet_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * msg_ptr = facility_data;
    uint8_t opcode;
    uint8_t * ptr;

    DEBUG_PRINTF("idigi_msg_process...\n");

    ASSERT_GOTO(packet != NULL, error);

    ptr = GET_PACKET_DATA_POINTER(packet, sizeof(idigi_packet_t));
    opcode = *ptr++;

    switch (opcode) 
    {
        case msg_opcode_capability:
            status = process_capabilities(idigi_ptr, msg_ptr, ptr);
            break;

        case msg_opcode_start:
            status = process_message(idigi_ptr, msg_ptr, ptr, packet->header.length, true);
            break;

        case msg_opcode_data:
            status = process_message(idigi_ptr, msg_ptr, ptr, packet->header.length, false);
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

static idigi_callback_status_t msg_delete_facility(idigi_data_t * idigi_ptr, uint16_t service_id)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_msg_data_t * msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(msg_ptr != NULL, error);
    ASSERT_GOTO(service_id < msg_service_id_count, error);

    msg_ptr->service_ids[service_id] = msg_service_id_none;

    /* is list empty? */
    {
        int i;

        for(i = 0; i < msg_service_id_count; i++) 
        {
            if (msg_ptr->service_ids[i] != msg_service_id_none)
                break;
        }

        if (i == msg_service_id_count) 
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
    idigi_msg_data_t * msg_ptr;

    msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    if (msg_ptr == NULL)
    {
        status = add_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM, (void **)&msg_ptr,
                                   sizeof(idigi_msg_data_t), msg_discovery, msg_process);

        ASSERT_GOTO(status == idigi_callback_continue, error);
        ASSERT_GOTO(msg_ptr != NULL, error);

        msg_ptr->window_size = 0;
        msg_ptr->cur_id = 0x4964; /* start from random number to avoid easy hacking */

        {
            uint16_t i;

            for (i = 0; i < msg_service_id_count; i++) 
                msg_ptr->service_ids[i] = msg_service_id_none;

            for (i = 0; i < msg_compression_count; i++)
                msg_ptr->compression_list[i] = msg_compression_none;
        }
    }

    msg_ptr->service_ids[service_id] = service_id;
    status = idigi_callback_continue;

error:
    return status;
}

