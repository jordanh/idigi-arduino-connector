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
    data_service_opcode_put_response,
    data_service_opcode_device_request,
    data_service_opcode_device_response
} data_service_opcode_t;


typedef struct
{
    idigi_data_service_request_t  request_type;
    void * user_context;
} data_service_context_t;

static void set_service_data_error(msg_service_request_t * const service_data, idigi_msg_error_t const error_status)
{
    service_data->service_type = msg_service_type_error;
    *((idigi_msg_error_t *)service_data->data_ptr) = error_status;
    service_data->length_in_bytes = sizeof error_status;
}

static idigi_callback_status_t process_device_request(idigi_data_t * const idigi_ptr,
                                                      msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    data_service_context_t * service_context = service_data->session->service_context;
    uint8_t const * ds_device_request = service_data->data_ptr;
    char * target_string = NULL;
    idigi_msg_error_t error_status = idigi_msg_error_cancel;
    unsigned int flag = 0;


    if (MsgIsStart(service_data->flags))
    {
        /* 1st message so let's parse message-start packet:
         *
         * Data Service Device request format:
         *  -------------------------------------------------------------------------------------------------------
         * |   0    |   1    |  2+N   |    +1     |     +1      |    +1       |    +M       |  ...       | +P      |
         *  -------------------------------------------------------------------------------------------------------
         * | Opcode | Target | Target | Parameter | Parameter 1 | Parameter 1 | Parameter 1 | Additioanl | Payload |
         * |        | length | string |   count   |     ID      | data length |    data     | parameters |         |
         *  -------------------------------------------------------------------------------------------------------
         *
         */
        enum {
            field_define(ds_device_request, opcode, uint8_t),
            field_define(ds_device_request, target_length, uint8_t),
            record_end(ds_device_request_header)
        };

        enum {
            field_define(ds_device_request, parameter_count, uint8_t)
        };

        enum {
            field_define(ds_device_request, parameter_id, uint8_t),
            field_define(ds_device_request, parameter_length, uint8_t),
            record_end(ds_device_request_parameter)
        };

        size_t min_data_length;
        uint8_t const target_length =  message_load_u8(ds_device_request, target_length);

        ASSERT_GOTO((message_load_u8(ds_device_request, opcode) == data_service_opcode_device_request), done);

        min_data_length = target_length +
                          record_bytes(ds_device_request_header) +
                          field_named_data(ds_device_request, parameter_count, size);
        ds_device_request += record_bytes(ds_device_request_header);

        ASSERT_GOTO(service_data->length_in_bytes >= min_data_length, done);

        {
            /* 1st time here so let's allocate service context memory for device request */
            void * ptr;

            status = malloc_data(idigi_ptr, sizeof *service_context, &ptr);
            if (status != idigi_callback_continue)
            {
                goto done;
            }

            service_context = ptr;
            service_data->session->service_context = service_context;
        }

        target_string = (char *)ds_device_request;
        service_context->request_type = idigi_data_service_device_request;
        service_context->user_context = NULL;

        flag = IDIGI_MSG_FIRST_DATA;
        ds_device_request += target_length;

        {
            /* TODO: Parse and process each parameter in the future.
             *      Ignore all parameters now.
             */

            uint8_t const parameter_count = message_load_u8(ds_device_request, parameter_count);
            uint8_t i;

            ds_device_request += field_named_data(ds_device_request, parameter_count, size);

            for (i=0; i < parameter_count; i++)
            {
                 uint8_t const parameter_length = message_load_u8(ds_device_request, parameter_length);
                 min_data_length += record_bytes(ds_device_request_parameter) + parameter_length;
                 ASSERT_GOTO(service_data->length_in_bytes >= min_data_length, done);

                 ds_device_request += record_bytes(ds_device_request_parameter); /* skip id and length */
                 ds_device_request += parameter_length;
            }
        }
        /* Add NUL to the target string. Must NULL it after parsing all parameters.
         * The NUL char is on parameter_count field in the request.
         */
        target_string[target_length] = '\0';
    }

    {
        /* pass data to the callback */
        idigi_request_t const request_id = {idigi_data_service_device_request};
        idigi_data_service_msg_request_t request_data;
        idigi_data_service_msg_response_t response_data;
        idigi_data_service_block_t server_data;
        idigi_data_service_device_request_t device_request;

        size_t response_data_length = sizeof response_data;

        ASSERT(service_context != NULL);
        /* setup structure to the callback */
        request_data.message_type = idigi_data_service_type_have_data;
        request_data.service_context = &device_request;
        request_data.server_data = &server_data;

        server_data.data = (void *)ds_device_request;
        server_data.length_in_bytes = service_data->length_in_bytes -
                                      (ds_device_request - (uint8_t *)service_data->data_ptr);
        server_data.flags = (MsgIsLastData(service_data->flags)) ? (IDIGI_MSG_LAST_DATA | flag) : flag;


        device_request.target = target_string;
        device_request.device_handle = service_data->session;


        response_data.user_context = service_context->user_context;
        response_data.message_status = idigi_msg_error_none;
        response_data.client_data = NULL;

        status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                                &request_data, sizeof request_data, &response_data, &response_data_length);

        switch (status)
        {
        case idigi_callback_unrecognized:
        case idigi_callback_abort:
            /* cancel the message */
            goto done;

        case idigi_callback_continue:
            if (response_data_length != sizeof response_data)
            {
                /* wrong size returned and let's cancel the request */
                notify_error_status(idigi_ptr->callback, idigi_class_data_service, request_id, idigi_invalid_data_size);
                goto done;
            }

            if (response_data.message_status != idigi_msg_error_none)
            {
                /* error returned so cancel this message */
                error_status = response_data.message_status;
                goto done;
            }
            /* fall thru to clear error status */
        case idigi_callback_busy:
            /* no error */
            error_status = idigi_msg_error_none;
            break;
        }
        service_context->user_context = response_data.user_context;
    }

done:
    if (error_status != idigi_msg_error_none)
    {
        set_service_data_error(service_data, error_status);
    }
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_data)
{
/* Data Service Device response format:
 *  ---------------------------------
 * |   0    |   1    |     2...      |
 *  ---------------------------------
 * | Opcode | status | Response Data |
 *  ---------------------------------
 */
enum {
    field_define(ds_device_response, opcode, uint8_t),
    field_define(ds_device_response, status, uint8_t),
    record_end(ds_device_response_header)
};

    idigi_callback_status_t status;
    data_service_context_t * const service_context = service_data->session->service_context;
    /* save some room for response header on 1st response data */
    size_t header_length = (MsgIsStart(service_data->flags)) ? record_bytes(ds_device_response_header) : 0;
    uint8_t * const data_ptr = service_data->data_ptr;

    idigi_request_t const request_id = {idigi_data_service_device_request};
    idigi_data_service_msg_request_t request_data;
    idigi_data_service_msg_response_t response_data;
    idigi_data_service_device_request_t device_request;
    idigi_data_service_block_t client_data;

    size_t response_data_length = sizeof response_data;

    /* setup request data passed to callback */
    request_data.service_context = &device_request;
    request_data.message_type = idigi_data_service_type_need_data;
    request_data.server_data = NULL;

    device_request.target = NULL;
    device_request.device_handle = service_data->session;

    /* setup response data so that callback updates it */
    response_data.client_data = &client_data;
    response_data.user_context = service_context->user_context;

    client_data.data = data_ptr + header_length;
    client_data.length_in_bytes = service_data->length_in_bytes - header_length;
    client_data.flags = 0;

    status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                            &request_data, sizeof request_data, &response_data, &response_data_length);

    if (status == idigi_callback_continue)
    {
        if (response_data_length != sizeof response_data)
        {
            /* wrong size returned and let's cancel the request */
            notify_error_status(idigi_ptr->callback, idigi_class_data_service, request_id, idigi_invalid_data_size);
            response_data.message_status = idigi_msg_error_cancel;
        }
        if (response_data.message_status != idigi_msg_error_none)
        {
            /* cancel this message */
            set_service_data_error(service_data, response_data.message_status);
            goto done;
        }

        if (MsgIsStart(service_data->flags))
        {
            enum {
                idigi_data_service_device_success,
                idigi_data_service_device_not_handled
            };

            /* Add header for 1st response message */
            uint8_t * const ds_device_response = service_data->data_ptr;
            uint8_t const status = ((client_data.flags & IDIGI_MSG_DATA_NOT_PROCESSED) == IDIGI_MSG_DATA_NOT_PROCESSED) ?
                                    idigi_data_service_device_not_handled : idigi_data_service_device_success;

            message_store_u8(ds_device_response, opcode, data_service_opcode_device_response);
            message_store_u8(ds_device_response, status, status);
        }

        if ((client_data.flags & IDIGI_MSG_LAST_DATA))
        {
            MsgSetLastData(service_data->flags);
        }
        service_data->length_in_bytes = client_data.length_in_bytes + header_length;
        service_context->user_context = response_data.user_context;
    }

done:
    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_data)
{
    idigi_callback_status_t status;
    data_service_context_t * const service_context = service_data->session->service_context;
    idigi_request_t const request_id = {idigi_data_service_device_request};

    idigi_data_service_msg_request_t request_data;
    idigi_data_service_msg_response_t response_data;
    idigi_data_service_device_request_t device_request;
    idigi_data_service_block_t server_data;

    size_t response_data_length = sizeof response_data;

    device_request.target = NULL;
    device_request.device_handle = service_data->session;

    request_data.service_context = &device_request;
    request_data.message_type = idigi_data_service_type_error;
    request_data.server_data = &server_data;

    /* get error code from the data pointer */
    server_data.data = service_data->data_ptr;
    server_data.length_in_bytes = service_data->length_in_bytes;
    server_data.flags = 0;

    response_data.user_context = service_context->user_context;
    response_data.message_status = idigi_msg_error_none;
    response_data.client_data = NULL;

    status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                                        &request_data, sizeof request_data,
                                        &response_data, &response_data_length);
    return status;
}

static idigi_callback_status_t data_service_device_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    
    switch (service_data->service_type)
    {
    case msg_service_type_need_data:
        status = process_device_response(idigi_ptr, service_data);
        break;

    case msg_service_type_have_data:
        status = process_device_request(idigi_ptr, service_data);
        break;

    case msg_service_type_error:
        status = process_device_error(idigi_ptr, service_data);
        break;

    case msg_service_type_free:
        free_data(idigi_ptr, service_data->session->service_context);
        break;

    default:
        status = idigi_callback_unrecognized;
        ASSERT(idigi_false);
        break;
    }

    return status;
}

static void process_put_request_error(msg_service_request_t * const service_data, idigi_msg_error_t const error_code)
{
    idigi_msg_error_t * error = service_data->data_ptr;

    *error = error_code;
    service_data->length_in_bytes = sizeof error_code;
    service_data->service_type = msg_service_type_error;
}

static size_t fill_put_request_header(idigi_data_service_put_request_t const * const request, uint8_t * const data)
{
    uint8_t * ptr = data;

    *ptr++ = data_service_opcode_put_request;

    /* fill path */
    if (request->path != NULL)
    {
        uint8_t const bytes = strlen(request->path);

        *ptr++ = bytes;
        memcpy(ptr, request->path, bytes);
        ptr += bytes;
    }

    /* fill parameters */
    {
        idigi_bool_t const type = (request->content_type != NULL);
        idigi_bool_t const archive = (request->flags & IDIGI_DATA_PUT_ARCHIVE) == IDIGI_DATA_PUT_ARCHIVE;
        idigi_bool_t const append = (request->flags & IDIGI_DATA_PUT_APPEND) == IDIGI_DATA_PUT_APPEND;
        static uint8_t const parameter_requested = 1;
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
            uint8_t const bytes = strlen(request->content_type);

            *ptr++ = parameter_id_content_type;
            *ptr++ = bytes;
            memcpy(ptr, request->content_type, bytes);
            ptr += bytes;
        }

        if (archive)
        {
            *ptr++ = parameter_id_archive;
            *ptr++ = parameter_requested;
        }

        if (append)
        {
            *ptr++ = parameter_id_append;
            *ptr++ = parameter_requested;
        }
    }

    ASSERT(ptr > data);
    return (size_t)(ptr - data);
}

static idigi_callback_status_t call_put_request_user(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data, idigi_data_service_msg_request_t * const request, idigi_data_service_msg_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    {
        idigi_request_t request_id;
        size_t response_bytes = sizeof *response;

        request_id.data_service_request = idigi_data_service_put_request;
        status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id, request, sizeof *request, response, &response_bytes);
    }

    if (status == idigi_callback_continue)
    {
        if (response->message_status != idigi_msg_error_none)
        {
            process_put_request_error(service_data, response->message_status);
            goto error;
        }

        if (service_data->service_type == msg_service_type_need_data)
        {
            idigi_data_service_block_t * user_data = response->client_data;

            service_data->flags = 0;
            service_data->length_in_bytes += user_data->length_in_bytes;
            if ((user_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
                MsgSetLastData(service_data->flags);
        }
    }

error:
    return status;
}

static idigi_callback_status_t process_put_request(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data, idigi_data_service_msg_request_t * const request, idigi_data_service_msg_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;    
    idigi_data_service_block_t * user_data = response->client_data;

    if (MsgIsStart(service_data->flags))
    {
        uint8_t * dptr = service_data->data_ptr;
        size_t const bytes = fill_put_request_header(request->service_context, dptr);

        ASSERT_GOTO(bytes < service_data->length_in_bytes, error);
        user_data->length_in_bytes = service_data->length_in_bytes - bytes;
        user_data->data = dptr + bytes;
        service_data->length_in_bytes = bytes;
    }
    else
    {
        user_data->data = service_data->data_ptr;
        user_data->length_in_bytes = service_data->length_in_bytes;
        service_data->length_in_bytes = 0;
    }

    status = call_put_request_user(idigi_ptr, service_data, request, response);
    goto done;

error:
    process_put_request_error(service_data, idigi_msg_error_format);

done:
    return status;
}

static idigi_callback_status_t process_put_response(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data, idigi_data_service_msg_request_t * request, idigi_data_service_msg_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_service_block_t * user_data = request->server_data;

    /* Data Service put response format:
     *  ---------------------------------
     * |   0    |   1    |     2...      |
     *  ---------------------------------
     * | Opcode | status | Response Data |
     *  ---------------------------------
     */
    enum 
    {
        field_define(put_response, opcode, uint8_t),
        field_define(put_response, status, uint8_t),
        record_end(put_response)
    };

    enum
    {
        ds_data_success,
        ds_data_bad_request,
        ds_data_service_unavailable,
        ds_data_server_error
    };

    uint8_t * const put_response = service_data->data_ptr;
    uint8_t const opcode = message_load_u8(put_response, opcode);
    uint8_t const result = message_load_u8(put_response, status);
    
    ASSERT_GOTO(MsgIsStart(service_data->flags), error);
    ASSERT_GOTO(opcode == data_service_opcode_put_response, error);
    
    user_data->flags = IDIGI_MSG_FIRST_DATA | IDIGI_MSG_LAST_DATA;
    user_data->length_in_bytes = service_data->length_in_bytes - record_end(put_response);
    user_data->data = put_response + record_end(put_response);

    switch (result) 
    {
    case ds_data_success:
        user_data->flags |= IDIGI_MSG_RESP_SUCCESS;
        break;

    case ds_data_bad_request:
        user_data->flags |= IDIGI_MSG_BAD_REQUEST;
        break;

    case ds_data_service_unavailable:
        user_data->flags |= IDIGI_MSG_UNAVAILABLE;
        break;

    case ds_data_server_error:
        user_data->flags |= IDIGI_MSG_SERVER_ERROR;
        break;

    default:
        ASSERT(idigi_false);
        break;
    }

    status = call_put_request_user(idigi_ptr, service_data, request, response);
    goto done;

error:
    process_put_request_error(service_data, idigi_msg_error_format);

done:
    return status;
}

static idigi_callback_status_t data_service_put_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_abort;
    data_service_context_t * const context = service_data->session->service_context;
    idigi_data_service_msg_request_t request_data;
    idigi_data_service_msg_response_t response_data;
    idigi_data_service_block_t user_data;

    ASSERT_GOTO(context != NULL, error);
    request_data.server_data = (service_data->service_type == msg_service_type_need_data) ? NULL : &user_data;
    response_data.client_data = (service_data->service_type == msg_service_type_need_data) ? &user_data : NULL;
    request_data.service_context = context->user_context;
    response_data.message_status = idigi_msg_error_none;
    user_data.flags = 0;

    switch (service_data->service_type)
    {
    case msg_service_type_need_data:
        request_data.message_type = idigi_data_service_type_need_data;
        status = process_put_request(idigi_ptr, service_data, &request_data, &response_data);
        break;

    case msg_service_type_have_data:
        request_data.message_type = idigi_data_service_type_have_data;
        status = process_put_response(idigi_ptr, service_data, &request_data, &response_data);
        break;

    case msg_service_type_error:
        user_data.data = service_data->data_ptr;
        user_data.length_in_bytes = service_data->length_in_bytes;
        request_data.message_type = idigi_data_service_type_error;
        status = call_put_request_user(idigi_ptr, service_data, &request_data, &response_data);
        break;

    case msg_service_type_free:
        free_data(idigi_ptr, context);
        status = idigi_callback_continue;
        goto done;

    default:
        status = idigi_callback_unrecognized;
        ASSERT_GOTO(idigi_false, error);
        break;
    }

error:
done:
    return status;
}

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_abort;
    data_service_context_t const * const context = service_data->session->service_context;

    ASSERT_GOTO(idigi_ptr != NULL, done);
    ASSERT_GOTO(service_data != NULL, done);
    ASSERT_GOTO(service_data->session != NULL, done);

    if (context == NULL) 
    {
        status = data_service_device_request_callback(idigi_ptr, service_data);
        goto done;
    }

    switch (context->request_type)
    {
        case idigi_data_service_put_request:            
            status = data_service_put_request_callback(idigi_ptr, service_data);
            break;

        case idigi_data_service_device_request:
            status = data_service_device_request_callback(idigi_ptr, service_data);
            break;

        default:
            ASSERT(idigi_false);
            break;
    }

done:
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
    data_service_context_t * context = NULL;
    idigi_msg_error_t result;

    UNUSED_PARAMETER(response);
    ASSERT_GOTO(request != NULL, done);

    {
        void * data_ptr;
        idigi_callback_status_t const ret = malloc_data(idigi_ptr, sizeof *context, &data_ptr);

        if (ret != idigi_callback_continue) 
        {
            status = idigi_no_resource;
            goto done;
        }
        
        context = data_ptr;
        context->request_type = idigi_data_service_put_request;
    }

    {
        msg_session_t * const session = msg_start_session(idigi_ptr, msg_service_id_data, &result);
        if (session == NULL) 
        {
            free_data(idigi_ptr, context);
            status = (result == idigi_msg_error_memory) ? idigi_no_resource : idigi_service_busy;
            goto done;
        }
    
        context->user_context = (void *)request; /* unconst */
        session->service_context = context;
    }

    status = idigi_success;

done:
    return status;
}


