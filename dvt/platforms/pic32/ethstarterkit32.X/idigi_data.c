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

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_status_t const msg_status, msg_session_t * const session, uint8_t const * data, size_t const length)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_request_t request;

    ASSERT_GOTO(idigi_ptr != NULL, error);
    ASSERT_GOTO(session != NULL, error);

    {
        bool const non_empty_data = ((data != NULL) && (length > 0));
        bool const empty_data = ((data == NULL) && (length == 0));

        ASSERT_GOTO(non_empty_data || empty_data, error);
    }

    switch (msg_status) 
    {
        case msg_status_error:
        {
            idigi_data_error_t const error_info = {session->session_id, *data};

            request.data_service_request = idigi_data_service_error;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &error_info, sizeof error_info);
            break;
        }

        case msg_status_send_complete:
        {
            idigi_data_send_t const send_info = {session->session_id, idigi_success, length};

            request.data_service_request = idigi_data_service_send_complete;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &send_info, sizeof send_info);
            break;
        }

        case msg_status_start:
        case msg_status_end:
        {
            idigi_data_response_t response;

            {
                uint8_t const data_service_response = 1;
                uint8_t const opcode = *data++;

                ASSERT_GOTO(opcode == data_service_response, error);
            }
            response.status = *data++;
            
            response.session_id = session->session_id;
            response.message.size = length - 2; /* opcode(1 byte) + status(1 byte) */
            response.message.value = data;

            request.data_service_request = idigi_data_service_response;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &response, sizeof response);
            break;
        }

        default:
            ASSERT_GOTO(false, error);
            break;
    }

error:
    return status;
}

static uint8_t * fill_data_block(idigi_data_block_t const * const block, uint8_t * data)
{
    *data++ = block->size;

    ASSERT(block->size > 0);
    if (block->size > 0)
    {
        memcpy(data, block->value, block->size);
        data += block->size;
    }

    return data;
}

static uint8_t * fill_parameter_request(uint8_t const param_id, uint8_t * data)
{    
    uint8_t const param_request = 1;

    *data++ = param_id;
    *data++ = param_request;

    return data;
}

static size_t fill_data_service_header(idigi_data_request_t const * const request, uint8_t * const data)
{
    uint8_t * ptr = data;

    {
        uint8_t const data_service_request = 0;

        *ptr++ = data_service_request;
    }

    ptr = fill_data_block(&request->path, ptr);

    {
        bool const archive = (request->flag & IDIGI_DATA_REQUEST_ARCHIVE) == IDIGI_DATA_REQUEST_ARCHIVE;
        bool const append = (request->flag & IDIGI_DATA_REQUEST_APPEND) == IDIGI_DATA_REQUEST_APPEND;
        uint8_t params = 1;

        enum
        {
            parameter_id_content_type,
            parameter_id_archive,
            parameter_id_append,
            parameter_count
        };

        if (archive) params++;
        if (append) params++;
    
        *ptr++ = params;
        *ptr++ = parameter_id_content_type;
        ptr = fill_data_block(&request->content_type, ptr);
    
        if (archive)
            ptr = fill_parameter_request(parameter_id_archive, ptr);
    
        if (append)
            ptr = fill_parameter_request(parameter_id_append, ptr);
    }

    ASSERT(ptr > data);
    return (size_t)(ptr - data);
}

static idigi_callback_status_t idigi_facility_data_service_delete(idigi_data_t * const data_ptr)
{
    return msg_delete_facility(data_ptr, msg_service_id_data);
}

static idigi_callback_status_t idigi_facility_data_service_init(idigi_data_t * const data_ptr)
{
    return msg_init_facility(data_ptr, msg_service_id_data, data_service_callback);
}

static idigi_status_t data_service_initiate(idigi_data_t * const data_ptr,  void const * request, void  * response)
{
    idigi_status_t status = idigi_invalid_data;
    idigi_data_request_t const * service = request;

    ASSERT_GOTO(request != NULL, error);

    if ((service->flag & IDIGI_DATA_REQUEST_START) != 0)
    {
        msg_data_info_t info;

        ASSERT_GOTO(response != NULL, error);

        info.header_length = fill_data_service_header(service, info.header);
        info.payload = service->payload.data;
        info.payload_length = service->payload.size;
        info.flag = service->flag;

        {
            void ** const session_ptr = response;

            *session_ptr = msg_send_start(data_ptr, msg_service_id_data, &info);
            status = (*session_ptr != NULL) ? idigi_success : idigi_configuration_error;
        }
    }
    else
    {
        msg_data_info_t const info = {0, {0}, service->payload.size, service->payload.data, service->flag};
        idigi_callback_status_t const ret_status = msg_send_data(data_ptr, service->session, &info);

        status = (ret_status == idigi_callback_continue) ? idigi_success : idigi_configuration_error;
    }

error:
    return status;
}

