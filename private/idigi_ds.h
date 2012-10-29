/*
 * Copyright (c) 2011 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
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
    void * callback_context;
} data_service_context_t;

static void set_data_service_error(msg_service_request_t * const service_request, idigi_msg_error_t const error_code)
{
    service_request->error_value = error_code;
    service_request->service_type = msg_service_type_error;
}

static idigi_callback_status_t process_device_request(idigi_data_t * const idigi_ptr,
                                                      msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    data_service_context_t * device_request_service = session->service_context;
    msg_service_data_t * const service_data = service_request->have_data;
    uint8_t const * ds_device_request = service_data->data_ptr;
    idigi_msg_error_t error_status = idigi_msg_error_cancel;
    idigi_bool_t const isFirstRequest = idigi_bool(MsgIsStart(service_data->flags));
    char * target_string = NULL;

    if (isFirstRequest)
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

        uint8_t const target_length =  message_load_u8(ds_device_request, target_length);

        size_t const min_data_length = target_length +
                          record_bytes(ds_device_request_header) +
                          field_named_data(ds_device_request, parameter_count, size);

        ASSERT_GOTO((message_load_u8(ds_device_request, opcode) == data_service_opcode_device_request), done);
        ASSERT_GOTO(service_data->length_in_bytes >= min_data_length, done);

        ds_device_request += record_bytes(ds_device_request_header);

        if (device_request_service == NULL)
        {
            /* 1st time here so let's allocate service context memory for device request service */
            void * ptr;

            status = malloc_data(idigi_ptr, sizeof *device_request_service, &ptr);
            if (status != idigi_callback_continue)
            {
                if (status == idigi_callback_busy)
                {
                    error_status = idigi_msg_error_none;
                }
                goto done;
            }

            device_request_service = ptr;
            session->service_context = device_request_service;
            device_request_service->callback_context = NULL;
        }

        target_string = (char *)ds_device_request;
        device_request_service->request_type = idigi_data_service_device_request;

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
                 unsigned int const parameter_length = message_load_u8(ds_device_request, parameter_length);
                 size_t const data_length = min_data_length + record_bytes(ds_device_request_parameter) + parameter_length;
                 ASSERT_GOTO(service_data->length_in_bytes >= data_length, done);

                 ds_device_request += record_bytes(ds_device_request_parameter); /* skip id and length */
                 ds_device_request += parameter_length;
            }
        }
        /* Add NUL to the target string. Must NUL-terminate it after parsing all parameters.
         * The NUL char is on parameter_count field in the request.
         */
        target_string[target_length] = '\0';
    }

    {
        /* pass data to the callback */
        idigi_request_t request_id;
        idigi_data_service_msg_request_t request_data;
        idigi_data_service_msg_response_t response_data;
        idigi_data_service_block_t server_data;
        idigi_data_service_device_request_t device_request;

        size_t response_data_length = sizeof response_data;

        ASSERT(device_request_service != NULL);
        /* setup structure to the callback */
        request_data.message_type = idigi_data_service_type_have_data;
        request_data.service_context = &device_request;
        request_data.server_data = &server_data;
        /* setup device request data */
        server_data.data = (void *)ds_device_request;
        server_data.length_in_bytes = service_data->length_in_bytes -
                                      (ds_device_request - (uint8_t *)service_data->data_ptr);
        server_data.flags = ((isFirstRequest ? IDIGI_MSG_FIRST_DATA : 0) |
                            (MsgIsLastData(service_data->flags) ? IDIGI_MSG_LAST_DATA: 0));

        /* setup request target */
        device_request.target = target_string;
        device_request.device_handle = session;


        response_data.user_context = device_request_service->callback_context;
        response_data.message_status = idigi_msg_error_none;
        response_data.client_data = NULL;

        request_id.data_service_request = idigi_data_service_device_request;
        status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                                &request_data, sizeof request_data, &response_data, &response_data_length);

        switch (status)
        {
        case idigi_callback_unrecognized:
            /* let message continue to process error status */
            status = idigi_callback_continue;
            /* fall thru to set error */
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
            error_status = idigi_msg_error_none;
            break;
        }
        device_request_service->callback_context = response_data.user_context;
    }

done:
    if (error_status != idigi_msg_error_none)
    {
        set_data_service_error(service_request, error_status);
    }
    return status;
}

static idigi_callback_status_t process_device_response(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_request)
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
    msg_service_data_t * const service_data = service_request->need_data;
    msg_session_t * const session = service_request->session;
    data_service_context_t * const device_request_service = session->service_context;
    idigi_bool_t const isFirstResponse = idigi_bool(MsgIsStart(service_data->flags));

    /* save some room for response header on 1st response data */
    size_t const header_length = isFirstResponse ? record_bytes(ds_device_response_header) : 0;
    uint8_t * const data_ptr = service_data->data_ptr;

    idigi_request_t request_id;
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
    device_request.device_handle = service_request->session;

    /* setup response data so that callback updates it */
    response_data.client_data = &client_data;
    response_data.user_context = device_request_service->callback_context;
    response_data.message_status = idigi_msg_error_none;

    client_data.data = data_ptr + header_length;
    client_data.length_in_bytes = service_data->length_in_bytes - header_length;
    client_data.flags = 0;

    request_id.data_service_request = idigi_data_service_device_request;
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
            set_data_service_error(service_request, response_data.message_status);
            goto done;
        }

        if (isFirstResponse)
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

        if ((client_data.flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
        {
            MsgSetLastData(service_data->flags);
        }
        service_data->length_in_bytes = client_data.length_in_bytes + header_length;
        device_request_service->callback_context = response_data.user_context;
    }

done:
    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_request)
{
    idigi_callback_status_t status;
    msg_session_t * const session = service_request->session;
    data_service_context_t * const device_request_service = session->service_context;
    idigi_request_t request_id;

    idigi_data_service_msg_request_t request_data;
    idigi_data_service_msg_response_t response_data;
    idigi_data_service_device_request_t device_request;
    idigi_data_service_block_t server_data;

    size_t response_data_length = sizeof response_data;

    device_request.target = NULL;
    device_request.device_handle = session;

    request_data.service_context = &device_request;
    request_data.message_type = idigi_data_service_type_error;
    request_data.server_data = &server_data;

    /* get error code from the data pointer */
    server_data.data = &service_request->error_value;
    server_data.length_in_bytes = sizeof service_request->error_value;
    server_data.flags = IDIGI_MSG_FIRST_DATA | IDIGI_MSG_LAST_DATA;

    response_data.user_context = device_request_service->callback_context;
    response_data.message_status = idigi_msg_error_none;
    response_data.client_data = NULL;

    request_id.data_service_request = idigi_data_service_device_request;
    status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                                        &request_data, sizeof request_data,
                                        &response_data, &response_data_length);
    return status;
}

static idigi_callback_status_t data_service_device_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    
    switch (service_request->service_type)
    {
    case msg_service_type_need_data:
        status = process_device_response(idigi_ptr, service_request);
        break;

    case msg_service_type_have_data:
        status = process_device_request(idigi_ptr, service_request);
        break;

    case msg_service_type_error:
        status = process_device_error(idigi_ptr, service_request);
        break;

    case msg_service_type_free:
        {
            msg_session_t * const session = service_request->session;

            free_data(idigi_ptr, session->service_context);
        }
        break;

    default:
        status = idigi_callback_unrecognized;
        ASSERT(idigi_false);
        break;
    }

    return status;
}

static size_t fill_put_request_header(idigi_data_service_put_request_t const * const request, uint8_t * const data)
{
    uint8_t * ptr = data;

    *ptr++ = data_service_opcode_put_request;

    /* fill path */
    if (request->path != NULL)
    {
        uint8_t const bytes = strlen(request->path);

        ASSERT(strlen(request->path) <= UCHAR_MAX);
        *ptr++ = bytes;
        memcpy(ptr, request->path, bytes);
        ptr += bytes;
    }

    /* fill parameters */
    {
        idigi_bool_t const have_type = idigi_bool(request->content_type != NULL);
        idigi_bool_t const have_archive = idigi_bool((request->flags & IDIGI_DATA_PUT_ARCHIVE) == IDIGI_DATA_PUT_ARCHIVE);
        idigi_bool_t const have_append = idigi_bool((request->flags & IDIGI_DATA_PUT_APPEND) == IDIGI_DATA_PUT_APPEND);
        static uint8_t const parameter_requested = 1;
        uint8_t params = 0;

        enum
        {
            parameter_id_content_type,
            parameter_id_archive,
            parameter_id_append,
            parameter_count
        };

        if (have_type) params++;
        if (have_archive) params++;
        if (have_append) params++;

        *ptr++ = params;

        if (have_type)
        {
            uint8_t const bytes = strlen(request->content_type);

            ASSERT(strlen(request->content_type) <= UCHAR_MAX);

            *ptr++ = parameter_id_content_type;
            *ptr++ = bytes;
            memcpy(ptr, request->content_type, bytes);
            ptr += bytes;
        }

        if (have_archive)
        {
            *ptr++ = parameter_id_archive;
            *ptr++ = parameter_requested;
        }

        if (have_append)
        {
            *ptr++ = parameter_id_append;
            *ptr++ = parameter_requested;
        }
    }

    return (size_t)(ptr - data);
}

static idigi_callback_status_t call_put_request_user(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request, idigi_data_service_msg_request_t * const request, idigi_data_service_msg_response_t * const response)
{
    idigi_callback_status_t status;

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
            set_data_service_error(service_request, response->message_status);
            goto error;
        }

        if (service_request->service_type == msg_service_type_need_data)
        {
            msg_service_data_t * const service_data = service_request->need_data;
            idigi_data_service_block_t * const user_data = response->client_data;

            service_data->flags = 0;
            service_data->length_in_bytes += user_data->length_in_bytes;
            if ((user_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
                MsgSetLastData(service_data->flags);
        }
    }

error:
    return status;
}

static idigi_callback_status_t process_put_request(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request, idigi_data_service_msg_request_t * const request, idigi_data_service_msg_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;    
    idigi_data_service_block_t * const user_data = response->client_data;
    msg_service_data_t * const service_data = service_request->need_data;

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

    status = call_put_request_user(idigi_ptr, service_request, request, response);
    goto done;

error:
    set_data_service_error(service_request, idigi_msg_error_format);

done:
    return status;
}

static idigi_callback_status_t process_put_response(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request, idigi_data_service_msg_request_t * request, idigi_data_service_msg_response_t * response)
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

    msg_service_data_t * const service_data = service_request->have_data;
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

    status = call_put_request_user(idigi_ptr, service_request, request, response);
    goto done;

error:
    set_data_service_error(service_request, idigi_msg_error_format);

done:
    return status;
}

static idigi_callback_status_t data_service_put_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_session_t * const session = service_request->session;
    data_service_context_t * const context = session->service_context;
    idigi_data_service_msg_request_t request_data;
    idigi_data_service_msg_response_t response_data;
    idigi_data_service_block_t user_data;

    request_data.service_context = (context != NULL) ? context->callback_context : service_request->have_data;
    response_data.message_status = idigi_msg_error_none;
    user_data.flags = 0;

    switch (service_request->service_type)
    {
    case msg_service_type_need_data:
        request_data.server_data = NULL;
        response_data.client_data = &user_data;
        request_data.message_type = idigi_data_service_type_need_data;
        status = process_put_request(idigi_ptr, service_request, &request_data, &response_data);
        break;

    case msg_service_type_have_data:
        request_data.server_data =  &user_data;
        response_data.client_data = NULL;
        request_data.message_type = idigi_data_service_type_have_data;
        status = process_put_response(idigi_ptr, service_request, &request_data, &response_data);
        break;

    case msg_service_type_error:
        request_data.server_data =  &user_data;
        response_data.client_data = NULL;
        user_data.data = &service_request->error_value;
        user_data.length_in_bytes = sizeof service_request->error_value;
        request_data.message_type = idigi_data_service_type_error;
        status = call_put_request_user(idigi_ptr, service_request, &request_data, &response_data);
        break;

    case msg_service_type_free:
        if (context != NULL)
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

static idigi_callback_status_t data_service_put_request_init(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    data_service_context_t * context = NULL;
    void * data_ptr;
    idigi_callback_status_t const ret = malloc_data(idigi_ptr, sizeof *context, &data_ptr);

    if (ret != idigi_callback_continue)
    {
        set_data_service_error(service_request, idigi_msg_error_memory);
        data_service_put_request_callback(idigi_ptr, service_request);
        goto error;
    }

    context = data_ptr;
    context->callback_context = service_request->have_data;
    context->request_type = idigi_data_service_put_request;
    session->service_context = context;

error:
    return idigi_callback_continue;
}

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_abort;
    msg_session_t * const session = service_request->session;
    data_service_context_t const * const context = session->service_context;

    ASSERT_GOTO(idigi_ptr != NULL, done);
    ASSERT_GOTO(service_request != NULL, done);
    ASSERT_GOTO(session != NULL, done);

    if (context == NULL) 
    {
        idigi_msg_callback_t * const init_fn = (service_request->service_type == msg_service_type_pending_request) ? data_service_put_request_init : data_service_device_request_callback;

        status = init_fn(idigi_ptr, service_request);
        goto done;
    }

    switch (context->request_type)
    {
        case idigi_data_service_put_request:
            status = data_service_put_request_callback(idigi_ptr, service_request);
            break;

        case idigi_data_service_device_request:
            status = data_service_device_request_callback(idigi_ptr, service_request);
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

    UNUSED_PARAMETER(response);
    ASSERT_GOTO(request != NULL, error);

    status = msg_initiate_request(idigi_ptr, request) ? idigi_success : idigi_service_busy;

error:
    return status;
}


