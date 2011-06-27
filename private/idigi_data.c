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

#define DATA_SERVICE_REQUEST_OPCODE     0x00
#define DATA_SERVICE_RESPONSE_OPCODE    0x01

/* opcode (1) + flags (1) + transaction id (2) + session id (2) + compression ID (1) */
#define DATA_SERVICE_MSG_START_LENGTH   0x07
/* opcode (1) + flags (1) + transaction id (2) */
#define DATA_SERVICE_MSG_DATA_LENGTH    0x04

enum
{
    parameter_id_content_type,
    parameter_id_archive,
    parameter_count
};

#define DATA_ARCHIVE_REQUEST            0x01
#define DATA_NO_ARCHIVE                 0x00

typedef struct
{
    bool in_use;
    uint32_t bytes_sent;
    idigi_data_request_t const * req_ptr;
    union
    {
        idigi_packet_t pkt_header;
        uint8_t buffer[MSG_MAX_PACKET_SIZE];
    };
} data_service_record_t;

/* one active transaction at any time */
static data_service_record_t data_service_record;
static void data_service_send_complete(idigi_data_t * idigi_ptr, idigi_packet_t * packet, idigi_status_t status, void * user_data);

static idigi_callback_status_t data_service_callback(idigi_data_t * idigi_ptr, msg_opcode_t msg_type, uint8_t * data, size_t length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    /* need to call user callback here */
    switch (msg_type) 
    {
        case msg_opcode_error:
        {
            idigi_data_error_t error_info;

            error_info.handle = LoadBE16(data);
            data += sizeof(uint16_t);
            error_info.error = *data;

            {                
                idigi_request_t request_id;

                request_id.data_service_request = idigi_data_service_error;
                status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id, &error_info, sizeof error_info, NULL, 0);
                msg_delete_session(idigi_ptr, error_info.handle);        
            }

            break;
        }

        case msg_opcode_start:
        case msg_opcode_data:
        {
            idigi_data_response_t response;

            ASSERT_GOTO(*data++ == DATA_SERVICE_RESPONSE_OPCODE, error);

            response.handle = data_service_record.req_ptr->handle;
            response.status = *data++;
            response.msg_length = length - 2;
            response.message = data;

            {                
                idigi_request_t request_id;

                request_id.data_service_request = idigi_data_service_response;
                status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id, &response, sizeof response, NULL, 0);
                msg_delete_session(idigi_ptr, response.handle);        
            }
        }

        default:
            break;
    }

error:
    return status;
}

static size_t fill_data_service_header(idigi_data_request_t const * request, uint8_t *data)
{
    uint8_t * ptr = data;

    *ptr++ = DATA_SERVICE_REQUEST_OPCODE;
    *ptr++ = request->path_length;
    if (request->path_length > 0) 
    {
        memcpy(ptr, request->path, request->path_length);
        ptr += request->path_length;
    }

    *ptr++ = parameter_count;
    *ptr++ = parameter_id_content_type;
    *ptr++ = request->content_type_length;
    if (request->content_type_length > 0) 
    {
        memcpy(ptr, request->content_type, request->content_type_length);
        ptr += request->content_type_length;
    }

    *ptr++ = parameter_id_archive;
    *ptr++ = ((request->flag & IDIGI_DATA_REQUEST_ARCHIVE) != 0) ? DATA_ARCHIVE_REQUEST : DATA_NO_ARCHIVE;

    return (ptr - data);
}

static idigi_status_t data_service_send_chunk(idigi_data_t * data_ptr, bool first_chunk, uint16_t * handle)
{
    idigi_status_t status = idigi_send_error;
    uint16_t session_id = MSG_INVALID_SESSION_ID;
    data_service_record_t * service = &data_service_record;
    idigi_data_request_t const * request = service->req_ptr;
    uint16_t flag = request->flag;
    size_t pkt_length;
    uint8_t * data;
    
    {
        bool const start_requested = (flag & IDIGI_DATA_REQUEST_START) != 0;
        bool const start_bit = (start_requested && first_chunk);
        size_t const header_len = sizeof(idigi_packet_t) + (start_bit ? DATA_SERVICE_MSG_START_LENGTH : DATA_SERVICE_MSG_DATA_LENGTH);

        data = service->buffer + header_len;
        if (start_bit) 
        {
            session_id = msg_create_session(data_ptr, msg_service_id_data, data_service_callback);
            ASSERT_GOTO(session_id != MSG_INVALID_SESSION_ID, error);
            ASSERT_GOTO(handle != NULL, error);
            *handle = session_id;

            {
                size_t const filled_len = fill_data_service_header(request, data);

                data += filled_len; 
            }
        }
        else
        {
            session_id = request->handle;
            flag &= ~IDIGI_DATA_REQUEST_START;
        }
    }

    pkt_length = (data - (service->buffer + sizeof(idigi_packet_t))); /* includes facility header */
    ASSERT_GOTO(pkt_length < MSG_MAX_PACKET_SIZE, error);

    {
        size_t const available = MSG_MAX_PACKET_SIZE - pkt_length;
        size_t const remaining = request->payload_length - service->bytes_sent;
        bool const more = (remaining > available);
        size_t const bytes = more ? available : remaining;

        if (more) 
            flag &= ~IDIGI_DATA_REQUEST_LAST;

        memcpy(data, &request->payload[service->bytes_sent], bytes);
        pkt_length += bytes;
        service->bytes_sent += bytes;
    }

    {
        idigi_packet_t * packet = &service->pkt_header;

        packet->header.length = (uint16_t)pkt_length;
        status = msg_send_data(data_ptr, session_id, packet, flag, data_service_send_complete);
    }

error:
    return status;
}

static void data_service_send_complete(idigi_data_t * idigi_ptr, idigi_packet_t * packet, idigi_status_t status, void * user_data)
{
    UNUSED_PARAMETER(packet);
    UNUSED_PARAMETER(user_data);

    if (status == idigi_success) 
    {
        if (data_service_record.bytes_sent < data_service_record.req_ptr->payload_length)
        {
            status = data_service_send_chunk(idigi_ptr, false, NULL);
            if (status == idigi_success) 
                goto done;
        }
    }

    {
        idigi_request_t request_id;
        data_service_record_t * service = &data_service_record;
        idigi_data_request_t const * request = service->req_ptr;
        idigi_data_send_t send_info = {.handle = request->handle, .status = status, .bytes_sent = service->bytes_sent};

       // if ((status != idigi_success) || ((request->flag & IDIGI_DATA_REQUEST_LAST) != 0))
       //     msg_delete_session(idigi_ptr, request->handle);

        request_id.data_service_request = idigi_data_service_send_complete;

        service->in_use = false; /* data_service_record is available for next transaction */
        idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id, &send_info, sizeof send_info, NULL, 0);        
    }

done:
    return;
}

static idigi_callback_status_t data_service_delete(idigi_data_t * data_ptr)
{
    return msg_delete_facility(data_ptr, msg_service_id_data);
}

static idigi_callback_status_t data_service_init(idigi_data_t * data_ptr)
{
    data_service_record.in_use = false;
    return msg_init_facility(data_ptr, msg_service_id_data);
}

static idigi_status_t data_service_initiate(idigi_data_t * data_ptr,  void const * request, void  * response)
{
    idigi_status_t status = idigi_invalid_data;
    data_service_record_t * service = &data_service_record;

    ASSERT_GOTO(request != NULL, error);
    ASSERT_GOTO(response != NULL, error);

    if (service->in_use) 
    {
        status = idigi_service_busy;
        goto error;
    }

    service->in_use = true;
    service->req_ptr = request;
    service->bytes_sent = 0;
    
    status = data_service_send_chunk(data_ptr, true, response);

error:
    return status;
}

