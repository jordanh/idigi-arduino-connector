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

typedef enum
{
    data_service_opcode_put_request,
    data_service_opcode_put_response
} data_service_opcode_t;

typedef struct data_service_context_t
{
    idigi_data_request_t const * user_request;
    size_t bytes_sent;
    idigi_bool_t have_data;
} data_service_context_t;

static idigi_callback_status_t data_service_call_user(idigi_data_t * const idigi_ptr, idigi_data_service_request_t const request_id, msg_session_t * const session, void * data, size_t const bytes)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_request_t request;

    ASSERT_GOTO(idigi_ptr != NULL, done);
    ASSERT_GOTO(session != NULL, done);

    request.data_service_request = request_id;
    switch (request_id) 
    {
        case idigi_data_service_error:
        {
            idigi_data_error_t error_info;
            idigi_msg_error_t const * const error_value = data;

            error_info.session = session;
            error_info.error = *error_value;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &error_info, sizeof error_info);
            break;
        }

        case idigi_data_service_send_complete:
        {
            idigi_data_send_t send_info;

            send_info.session = session;
            send_info.status = idigi_success;
            send_info.bytes_sent = bytes;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &send_info, sizeof send_info);
            break;
        }

        case idigi_data_service_response:
        {
            idigi_data_response_t response;
            uint8_t * user_data = data;

            {
                uint8_t const opcode = *user_data++;

                ASSERT_GOTO(opcode == data_service_opcode_put_response, done);
            }

            response.status = *user_data++;            
            response.session = session;
            response.message.size = bytes - 2; /* opcode(1 byte) + status(1 byte) */
            response.message.value = user_data;

            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &response, sizeof response);
            break;
        }

        default:
            ASSERT_GOTO(idigi_false, done);
            break;
    }

done:
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
        idigi_bool_t const type = (request->content_type.size == 0);
        idigi_bool_t const archive = (request->flag & IDIGI_DATA_REQUEST_ARCHIVE) == IDIGI_DATA_REQUEST_ARCHIVE;
        idigi_bool_t const append = (request->flag & IDIGI_DATA_REQUEST_APPEND) == IDIGI_DATA_REQUEST_APPEND;
        uint8_t params = 0;

        enum
        {
            parameter_id_content_type,
            parameter_id_archive,
            parameter_id_append,
            parameter_count
        };

        if (type) params++;
        if (archive) params++;
        if (append) params++;

        *ptr++ = params;    
        if (type) 
        {
            *ptr++ = parameter_id_content_type;
            ptr = fill_data_block(&request->content_type, ptr);
        }
    
        if (archive)
            ptr = fill_parameter_request(parameter_id_archive, ptr);
    
        if (append)
            ptr = fill_parameter_request(parameter_id_append, ptr);
    }

    ASSERT(ptr > data);
    return (size_t)(ptr - data);
}

static idigi_callback_status_t data_service_get_data(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_session_t * const session = service_request->session;
    msg_service_data_t * const service_data = service_request->need_data;
    data_service_context_t * const context = session->service_context;
    idigi_data_request_t const * const request = context->user_request;
    uint8_t * dptr = service_data->data_ptr;
    size_t data_bytes = 0;

    ASSERT_GOTO(context != NULL, done);

    if (!context->have_data) 
    {
        status = idigi_callback_busy;
        goto done;
    }

    if (MsgIsStart(service_data->flags))
    {
        data_bytes = fill_data_service_header(request, dptr);

        ASSERT_GOTO(data_bytes < service_data->length_in_bytes, done);
        dptr += data_bytes;
    }

    {
        idigi_data_payload_t const * const pblock = &request->payload;
        size_t const available_bytes = service_data->length_in_bytes - data_bytes;
        size_t const bytes_to_send = pblock->size - context->bytes_sent;
        idigi_bool_t const data_complete = bytes_to_send <= available_bytes;
        size_t const bytes_to_copy =  data_complete ? bytes_to_send : available_bytes;

        memcpy(dptr, &pblock->data[context->bytes_sent], bytes_to_copy);
        service_data->length_in_bytes = data_bytes + bytes_to_copy;
        context->bytes_sent += bytes_to_copy;

        if (data_complete) 
        {
            idigi_bool_t const last_chunk = (request->flag & IDIGI_DATA_REQUEST_LAST) == IDIGI_DATA_REQUEST_LAST;

            context->have_data = idigi_false;
            if (last_chunk)
                MsgSetLastData(service_data->flags);
            status = data_service_call_user(idigi_ptr, idigi_data_service_send_complete, session, pblock->data, context->bytes_sent);
        }
        else
            status = idigi_callback_continue;
    }

done:
    return status;
}

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_abort;

    switch (service_request->service_type)
    {
    case msg_service_type_need_data:
        status = data_service_get_data(idigi_ptr, service_request);
        break;

    case msg_service_type_have_data:
        {
            msg_service_data_t * const service_data = service_request->have_data;
            uint8_t const opcode = *(uint8_t *)service_data->data_ptr;

            if (opcode == data_service_opcode_put_response)
            {
                status = data_service_call_user(idigi_ptr, idigi_data_service_response, service_request->session, service_data->data_ptr, service_data->length_in_bytes);
            }
            else
            {
                idigi_msg_error_t const error_code = idigi_msg_error_no_service;

                service_request->error_value = error_code;
                service_request->service_type = msg_service_type_error;
                status = idigi_callback_continue;
                idigi_debug("Data service request/response with opcode %d is not supported\n", opcode);
            }
        }
        break;

    case msg_service_type_error:
        status = data_service_call_user(idigi_ptr, idigi_data_service_error, service_request->session, service_request->error_value, sizeof service_request->error_value);
        break;
            
    case msg_service_type_free:
        {
            msg_session_t * const session = service_request->session;

            if (session->service_context != NULL) 
            {
                free_data(idigi_ptr, session->service_context);
            }
        }
        status = idigi_callback_continue;
        break;

    default:
        ASSERT_GOTO(idigi_false, error);
        break;
    }

error:
    return status;
}

static idigi_callback_status_t idigi_facility_data_service_cleanup(idigi_data_t * const idigi_ptr)
{
    return msg_cleanup_all_sessions(idigi_ptr,  msg_service_id_data);
}

static idigi_callback_status_t idigi_facility_data_service_delete(idigi_data_t * const data_ptr)
{
    return msg_delete_facility(data_ptr, msg_service_id_data);
}

static idigi_callback_status_t idigi_facility_data_service_init(idigi_data_t * const data_ptr, unsigned int const facility_index)
{
    return msg_init_facility(data_ptr, facility_index, msg_service_id_data, data_service_callback);
}

static idigi_status_t data_service_initiate(idigi_data_t * const idigi_ptr,  void const * request, void  * response)
{
    idigi_status_t status = idigi_invalid_data;
    idigi_data_request_t const * const service = request;
    data_service_context_t * context = NULL;
    idigi_msg_error_t result;

    ASSERT_GOTO(request != NULL, error);

    if ((service->flag & IDIGI_DATA_REQUEST_START) != IDIGI_DATA_REQUEST_START)
    {
        msg_session_t const * const session = service->session;

        ASSERT_GOTO(session != NULL, error);
        context = session->service_context;
        ASSERT_GOTO(!context->have_data, error);
        goto done;
    }

    {
        void * ptr;

        status = malloc_data(idigi_ptr, sizeof *context, &ptr);
        if (status != idigi_callback_continue) 
        {
            status = idigi_no_resource;
            goto error;
        }

        context = ptr;
        context->have_data = idigi_false;
    }

    {
        msg_session_t * const session = msg_start_session(idigi_ptr, msg_service_id_data, &result);
        if (session == NULL) 
        {
            free_data(idigi_ptr, context);
            status = (result == idigi_msg_error_memory) ? idigi_no_resource : idigi_service_busy;
            goto error;
        }
    
        session->service_context = context;

        {
            msg_session_t ** const session_handle = response;

            *session_handle = session;
        }
    }

done:
    context->user_request = service;
    context->have_data = idigi_true;
    context->bytes_sent = 0;
    status = idigi_success;

error:
    return status;
}


