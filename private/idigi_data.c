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

enum
{
    parameter_id_content_type,
    parameter_id_archive,
    parameter_id_append,
    parameter_count
};

#define DATA_ARCHIVE_REQUEST    0x01
#define DATA_APPEND_REQUEST     0x01

#define DATA_SERVICE_HEADER_MAX_LENGTH  256

static idigi_callback_status_t data_service_callback(idigi_data_t * idigi_ptr, msg_status_t const msg_status, msg_session_t * const session, uint8_t * data, size_t const length)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_request_t request;

    ASSERT_GOTO(session != NULL, error);

    switch (msg_status) 
    {
        case msg_status_error:
        {
            idigi_data_error_t error_info = {.session_id = session->session_id, .error = *data};

            request.data_service_request = idigi_data_service_error;
            status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request, &error_info, sizeof error_info, NULL, 0);
            msg_delete_session(idigi_ptr, session, msg_type_tx);
            break;
        }

        case msg_status_send_complete:
        {
            idigi_data_send_t send_info = {.session_id = session->session_id, .status = idigi_success, .bytes_sent = length};

            request.data_service_request = idigi_data_service_send_complete;
            status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request, &send_info, sizeof send_info, NULL, 0);
            break;
        }

        case msg_status_start:
        case msg_status_end:
        {
            idigi_data_response_t response;

            ASSERT_GOTO(*data++ == DATA_SERVICE_RESPONSE_OPCODE, error);
            response.status = *data++;
            
            response.session_id = session->session_id;
            response.message.size = length - 2; /* opcode(1 byte) + status(1 byte) */
            response.message.value = data;

            request.data_service_request = idigi_data_service_response;
            status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request, &response, sizeof response, NULL, 0);
            msg_delete_session(idigi_ptr, session, msg_type_tx);
            break;
        }

        default:
            ASSERT_GOTO(false, error);
            break;
    }

error:
    return status;
}

static size_t fill_data_block(idigi_data_block_t const * const block, uint8_t * data)
{
    *data++ = block->size;
    if (block->size > 0) 
        memcpy(data, block->value, block->size);

    return block->size + 1;
}

static size_t fill_data_service_header(idigi_data_request_t const * const request, uint8_t * data)
{
    uint8_t * ptr = data;
    size_t blk_length;
    bool const archive = (request->flag & IDIGI_DATA_REQUEST_ARCHIVE) != 0;
    bool const append = (request->flag & IDIGI_DATA_REQUEST_APPEND) != 0;
    uint8_t params = 1;

    *ptr++ = DATA_SERVICE_REQUEST_OPCODE;

    blk_length = fill_data_block(&request->path, ptr);
    ptr += blk_length;

    if (archive) params++;
    if (append) params++;

    *ptr++ = params;
    *ptr++ = parameter_id_content_type;
    blk_length = fill_data_block(&request->content_type, ptr);
    ptr += blk_length;

    if (archive)
    {
        *ptr++ = parameter_id_archive;
        *ptr++ = DATA_ARCHIVE_REQUEST;
    }

    if (append)
    {
        *ptr++ = parameter_id_append;
        *ptr++ = DATA_APPEND_REQUEST;
    }

    return (ptr - data);
}

static idigi_callback_status_t data_service_delete(idigi_data_t * data_ptr)
{
    return msg_delete_facility(data_ptr, msg_service_id_data);
}

static idigi_callback_status_t data_service_init(idigi_data_t * data_ptr)
{
    return msg_init_facility(data_ptr, msg_service_id_data, data_service_callback);
}

static idigi_status_t data_service_initiate(idigi_data_t * data_ptr,  void const * request, void  * response)
{
    idigi_status_t status = idigi_invalid_data;
    idigi_data_request_t const * service = request;

    ASSERT_GOTO(request != NULL, error);

    if ((service->flag & IDIGI_DATA_REQUEST_START) != 0)
    {
        uint8_t header[DATA_SERVICE_HEADER_MAX_LENGTH];
        size_t header_length = fill_data_service_header(service, header);

        ASSERT_GOTO(response != NULL, error);

        {
            void ** const session_ptr = response;
            msg_data_info_t const info = {.header_length = header_length, .header = header, .payload_length = service->payload.size, .payload = service->payload.data, .flag = service->flag};
            *session_ptr = msg_send_start(data_ptr, msg_service_id_data, &info);

            status = (*session_ptr != NULL) ? idigi_success : idigi_configuration_error;
        }
    }
    else
    {
        msg_data_info_t const info = {.header_length = 0, .header = NULL, .payload_length = service->payload.size, .payload = service->payload.data,  .flag = service->flag};
        idigi_callback_status_t ret_status = msg_send_data(data_ptr, service->session, &info);

        status = (ret_status == idigi_callback_continue) ? idigi_success : idigi_configuration_error;
    }

error:
    return status;
}

