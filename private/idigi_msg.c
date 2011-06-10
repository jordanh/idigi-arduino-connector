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

#define IDIGI_MSG_CAPABILITIES_OPCODE         0x00
#define IDIGI_MSG_MESSAGE_START_OPCODE        0x01
#define IDIGI_MSG_MESSAGE_DATA_OPCODE         0x02
#define IDIGI_MSG_MESSAGE_ACK_OPCODE          0x03
#define IDIGI_MSG_ERROR_OPCODE                0x04

#define IDIGI_MSG_FLAG_RESPONSE               0x00
#define IDIGI_MSG_FLAG_REQUEST                0x01
#define IDIGI_MSG_FLAG_LAST_DATA              0x02
#define IDIGI_MSG_RECV_WINDOW_SIZE            16384

#define IDIGI_CAPABILITIES_REQUEST    0x01
#define IDIGI_CAPABILITIES_RESPONSE   0x00

#define IDIGI_MSG_FACILITY_VERSION    0x01
#define IDIGI_MSG_MAX_TRANSACTIONS    1

#define IDIGI_MSG_MAX_SERVICE       4
#define IDIGI_MSG_MAX_COMPRESSIONS  4

#define DATA_SERVICE_ID     0x01
#define FILE_SERVICE_ID     0x02

#define EMPTY_SERVICE_HANDLE       0xFFFF

typedef idigi_callback_status_t idigi_msg_callback_t(uint16_t handle, uint8_t * buffer, uint16_t length);

typedef struct
{
    uint16_t service_id;
    uint16_t handle;
    idigi_msg_callback_t * cb_func;
} idigi_msg_service_t;

typedef struct 
{
    uint32_t window_size; /* peer window size */
    uint16_t cur_handle;
    uint16_t service_count;
    idigi_msg_service_t service_list[IDIGI_MSG_MAX_SERVICE];
    uint8_t  compression_count;
    uint8_t  compression_list[IDIGI_MSG_MAX_COMPRESSIONS];
} idigi_msg_data_t;

static idigi_msg_service_t * get_service(idigi_msg_data_t * msg_ptr, uint16_t handle)
{
    uint16_t i;
    idigi_msg_service_t * service = NULL;

    for (i = 0; i < IDIGI_MSG_MAX_SERVICE; i++) 
    {
        if (msg_ptr->service_list[i].handle == handle)
        {
            service = &msg_ptr->service_list[i];
            break;
        }
    }

    return service;
}

static uint16_t get_next_handle(idigi_msg_data_t * msg_ptr)
{
    uint16_t handle = EMPTY_SERVICE_HANDLE;

    do
    {
        int i;

        msg_ptr->cur_handle++;

        /* in use? */
        for (i = 0; i < IDIGI_MSG_MAX_SERVICE; i++) 
        {
            if (msg_ptr->service_list[i].handle == msg_ptr->cur_handle)
                break;
        }

        if (i == IDIGI_MSG_MAX_SERVICE) 
            handle = msg_ptr->cur_handle;
                
    } while (handle == EMPTY_SERVICE_HANDLE);

    return handle;
}

static uint8_t update_compression_algo_list(idigi_msg_data_t * msg_ptr, uint8_t * buffer)
{
    uint8_t count = 0;

    for (count = 0; count < msg_ptr->compression_count; count++) 
        *buffer++ = msg_ptr->compression_list[count];

    return count;
}

static uint16_t update_service_list(idigi_msg_data_t * msg_ptr, uint8_t * buffer)
{
    uint16_t count = 0;
    uint16_t i;

    for (i = 0; i < IDIGI_MSG_MAX_SERVICE; i++) 
    {
        idigi_msg_service_t const * const service = &msg_ptr->service_list[i];

        if (service->handle !=  EMPTY_SERVICE_HANDLE) 
        {        
            StoreBE16(buffer, service->service_id);
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
    *cur_ptr++ = IDIGI_MSG_CAPABILITIES_OPCODE;
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
        uint8_t algo_count = update_compression_algo_list(msg_data, cur_ptr+1);

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
    status = enable_facility_packet(idigi_ptr, packet, E_MSG_FAC_MSG_NUM, release_packet_buffer);

error:
    return status;
}

static idigi_callback_status_t process_capabilities(idigi_data_t * idigi_ptr, idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t request = *ptr++;
    uint8_t version = *ptr++;

    /* is version valid? */
    if (version != IDIGI_MSG_MAX_TRANSACTIONS)
    {
        DEBUG_PRINTF("process_capabilities invalid version [%d]\n", version);
    }

    /* update window size */
    {
        uint32_t window = LoadNative32(ptr);

        if (window != 0) 
        {
            msg_fac->window_size = window;
        }

        ptr += sizeof window;
    }

    /* TODO: extract compression and match with supported */

    if (request) 
    {
        status = send_msg_capabilities(idigi_ptr, msg_fac, IDIGI_CAPABILITIES_RESPONSE);
    }

    return status;
}

static idigi_callback_status_t process_msg_start(idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    (void)msg_fac;
    (void)ptr;

    return idigi_callback_continue;
}

static idigi_callback_status_t process_msg_data(idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    (void)msg_fac;
    (void)ptr;

    return idigi_callback_continue;
}

static idigi_callback_status_t process_msg_ack(idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    (void)msg_fac;
    (void)ptr;

    return idigi_callback_continue;
}

static idigi_callback_status_t process_msg_error(idigi_msg_data_t * msg_fac, uint8_t *ptr)
{
    (void)msg_fac;
    (void)ptr;

    return idigi_callback_continue;
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
        case IDIGI_MSG_CAPABILITIES_OPCODE:
            status = process_capabilities(idigi_ptr, msg_ptr, ptr);
            break;

        case IDIGI_MSG_MESSAGE_START_OPCODE:
            status = process_msg_start(msg_ptr, ptr);
            break;

        case IDIGI_MSG_MESSAGE_DATA_OPCODE:
            status = process_msg_data(msg_ptr, ptr);
            break;

        case IDIGI_MSG_MESSAGE_ACK_OPCODE:
            status = process_msg_ack(msg_ptr, ptr);
            break;

        case IDIGI_MSG_ERROR_OPCODE:
            status = process_msg_error(msg_ptr, ptr);
            break;

        default:            
            DEBUG_PRINTF("msg_process: Invalid opcode\n");
            break;
    }

error:
    return status;
}

static idigi_callback_status_t msg_delete_facility(idigi_data_t * idigi_ptr, uint16_t handle)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_msg_data_t * msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);

    if (msg_ptr != NULL) 
    {
        idigi_msg_service_t * service = get_service(msg_ptr, handle);

        ASSERT_GOTO(service != NULL, error);
        service->handle = EMPTY_SERVICE_HANDLE;
        ASSERT_GOTO(msg_ptr->service_count > 0, error);
        msg_ptr->service_count--;
        
        /* is list empty? */
        if (msg_ptr->service_count == 0) 
            status = del_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    }

error:
    return status;
}

static uint16_t msg_init_facility(idigi_data_t *idigi_ptr, uint16_t service_id, idigi_msg_callback_t * callback)
{
    idigi_msg_data_t * msg_ptr;
    uint16_t handle = EMPTY_SERVICE_HANDLE;

    msg_ptr = (idigi_msg_data_t *)get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    if (msg_ptr == NULL)
    {
        idigi_callback_status_t const status = add_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM, (void **)&msg_ptr,
                                                        sizeof(idigi_msg_data_t), msg_discovery, msg_process);

        ASSERT_GOTO(status == idigi_callback_continue, error);
        ASSERT_GOTO(msg_ptr != NULL, error);

        msg_ptr->window_size = 0;
        msg_ptr->cur_handle = 0x4964; /* start from random number to avoid easy hacking */
        msg_ptr->service_count = 0;
        msg_ptr->compression_count = 0; /* TODO: Add supported compression algorithms */

        {
            uint16_t i;

            for (i = 0; i < IDIGI_MSG_MAX_SERVICE; i++) 
                msg_ptr->service_list[i].handle = EMPTY_SERVICE_HANDLE;
        }
    }

    ASSERT_GOTO(msg_ptr->service_count < IDIGI_MSG_MAX_SERVICE, error);

    {
        idigi_msg_service_t * service = get_service(msg_ptr, EMPTY_SERVICE_HANDLE);

        ASSERT_GOTO(service != NULL, error); 
        service->service_id = service_id;
        service->handle = msg_ptr->cur_handle;
        service->cb_func = callback;
    }

    handle =  msg_ptr->cur_handle;
    msg_ptr->cur_handle = get_next_handle(msg_ptr);
    msg_ptr->service_count++;

error:
    return handle;
}

