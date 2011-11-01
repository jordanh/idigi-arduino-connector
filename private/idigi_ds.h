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

typedef struct ds_device_request_service {
    bool response_started;
    char * target_string;
    void * user_context;
    idigi_data_service_device_data_t message_data;
} ds_device_request_service_t;

typedef struct data_service_context_t
{
    idigi_data_service_request_t  request_type;
    void * user_context;
} data_service_context_t;

static size_t fill_data_service_header(idigi_data_put_header_t const * const request, uint8_t * const data)
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
        bool const type = (request->content_type != NULL);
        bool const archive = (request->flags & IDIGI_DATA_PUT_ARCHIVE) == IDIGI_DATA_PUT_ARCHIVE;
        bool const append = (request->flags & IDIGI_DATA_PUT_APPEND) == IDIGI_DATA_PUT_APPEND;
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

static idigi_callback_status_t process_device_request(idigi_data_t * const idigi_ptr,
                                                      msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_unrecognized;
    data_service_context_t * context = service_data->session->service_context;
    ds_device_request_service_t * device_request;
    uint8_t const * ds_device_request = service_data->data_ptr;

    if (service_data->flags & IDIGI_MSG_FIRST_DATA)
    {
        /* Data Service Device request format:
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

        /* This is the start message which contains Data Service Device Request header.
         * Let's get and save target field.
         */
        size_t min_data_length;
        uint8_t const target_length =  message_load_u8(ds_device_request, target_length);
        size_t length = (sizeof *context + sizeof *device_request + target_length +1);

        ASSERT_GOTO(message_load_u8(ds_device_request, opcode) == data_service_opcode_device_request, done);

        min_data_length = target_length +
                          record_bytes(ds_device_request_header) +
                          field_named_data(ds_device_request, parameter_count, size);
        ds_device_request += record_bytes(ds_device_request_header);

        ASSERT_GOTO(service_data->length_in_bytes > min_data_length, done);

        {
            /* 1st time here so let's allocate memory for handling device request message */
            void * ptr;

            status = malloc_data(idigi_ptr, (sizeof *context + sizeof *device_request + target_length +1), &ptr);
            if (status != idigi_callback_continue)
            {
                goto done;
            }

            context = ptr;
        }

        device_request = (ds_device_request_service_t *)(context + 1);
        device_request->target_string = (char *)device_request + sizeof *device_request;
        memcpy(device_request->target_string, ds_device_request, target_length);
        device_request->target_string[target_length] = '\0';
        device_request->user_context = NULL;
        device_request->response_started = false;
        context->request_type = idigi_data_service_device_request;
        context->user_context = device_request;
        service_data->session->service_context = context;

        printf("device_request: context 0x%x size %u 0x%x\n", (unsigned)context, length, (unsigned)((uint8_t *)context + length));
        printf("device_request: context->user_context %p %u\n", context->user_context, sizeof *device_request);
        printf("device_request: target_string %p size %u\n", device_request->target_string, target_length);
        printf("device_request: 0x%x\n",(unsigned)(device_request->target_string + target_length));

        ds_device_request += target_length;

        {
            /* TODO: parse and process each parameter in the future.
             *      ignore all parameters now.
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
    }

    {
        idigi_request_t const request_id = {idigi_data_service_device_request};
        idigi_data_service_device_request_t request_data;
        idigi_data_service_device_response_t response_data;

        device_request = context->user_context;
        ASSERT_GOTO(device_request != NULL, done);

        request_data.user_context = device_request->user_context;
        request_data.target = device_request->target_string;
        request_data.session = service_data->session;
        request_data.message_type = idigi_data_service_device_message_request;

        request_data.message_data = &device_request->message_data;
        request_data.message_data->data = (void *)ds_device_request;
        request_data.message_data->data_length = service_data->length_in_bytes -
                                                (ds_device_request - (uint8_t *)service_data->data_ptr);
        request_data.message_data->flag = (service_data->flags & (IDIGI_MSG_FIRST_DATA | IDIGI_MSG_LAST_DATA));


        response_data.message_data = NULL;
        status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                                &request_data, sizeof request_data, &response_data, NULL);
        if (status == idigi_callback_continue && response_data.status != idigi_data_service_device_success)
        {
            /* set this to cancel this message */
            status = idigi_callback_unrecognized;
        }
        device_request->user_context = response_data.user_context;
        printf("device_request: user context %p\n", device_request->user_context);
    }
done:
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
    idigi_callback_status_t status = idigi_callback_unrecognized;
    data_service_context_t * const context = service_data->session->service_context;
    ds_device_request_service_t * device_request = context->user_context;

    idigi_request_t const request_id = {idigi_data_service_device_request};
    idigi_data_service_device_request_t request_data;
    idigi_data_service_device_response_t response_data;
    size_t header_length = (device_request->response_started) ? 0 : record_bytes(ds_device_response_header);
    uint8_t * data_ptr = service_data->data_ptr;

    request_data.user_context = device_request->user_context;
    request_data.target = device_request->target_string;
    request_data.session = service_data->session;
    request_data.message_type = idigi_data_service_device_message_response;
    request_data.message_data = NULL;

    response_data.message_data = &device_request->message_data;
    response_data.message_data->data = data_ptr + header_length;
    response_data.message_data->data_length = service_data->length_in_bytes - header_length;
    response_data.message_data->flag = 0;

    status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id,
                            &request_data, sizeof request_data, &response_data, NULL);
    if (status == idigi_callback_continue)
    {
        if (!device_request->response_started)
        {
            /* 1st response message */
            uint8_t * const ds_device_response = service_data->data_ptr;

            message_store_u8(ds_device_response, opcode, data_service_opcode_device_response);
            message_store_u8(ds_device_response, status, response_data.status);
            service_data->flags = response_data.message_data->flag | IDIGI_MSG_FIRST_DATA;
        }
        else
        {
            service_data->flags = response_data.message_data->flag;
        }
        service_data->length_in_bytes = response_data.message_data->data_length + header_length;
        device_request->user_context = response_data.user_context;
        printf("device_response: user_context %p\n", device_request->user_context);
    }
    return status;
}

static idigi_callback_status_t process_device_error(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_unrecognized;
    data_service_context_t * const context = service_data->session->service_context;
    ds_device_request_service_t * device_request = context->user_context;
    idigi_request_t const request_id = {idigi_data_service_device_request};

    idigi_data_service_device_request_t request_data;

    request_data.user_context = device_request->user_context;
    request_data.target = device_request->target_string;
    request_data.session = service_data->session;
    request_data.message_type = idigi_data_service_device_message_error;
    request_data.message_data = &device_request->message_data;

    request_data.message_data->data = service_data->data_ptr;
    request_data.message_data->data_length = service_data->length_in_bytes;
    request_data.message_data->flag = 0;

    status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request_id,
                            &request_data, sizeof request_data);
    return status;
}

static idigi_callback_status_t data_service_device_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    data_service_context_t * const context = service_data->session->service_context;
    
    UNUSED_PARAMETER(idigi_ptr);
    UNUSED_PARAMETER(service_data);

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
        free_data(idigi_ptr, context);
        break;

    default:
        status = idigi_callback_unrecognized;
        ASSERT(false);
        break;
    }

    return status;
}

static idigi_callback_status_t data_service_put_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_abort;
    data_service_context_t * const context = service_data->session->service_context;
    idigi_data_put_request_t request_data;
    idigi_data_put_response_t response_data;

    ASSERT_GOTO(context != NULL, error);
    request_data.session_handle = service_data->session;
    request_data.header_context = context->user_context;
    response_data.flags = 0;

    switch (service_data->service_type)
    {
    case msg_service_type_need_data:
        request_data.request_type = idigi_data_put_need_data;
        if (MsgIsStart(service_data->flags))
        {
            uint8_t * dptr = service_data->data_ptr;
            size_t const bytes = fill_data_service_header(context->user_context, dptr);

            ASSERT_GOTO(bytes < service_data->length_in_bytes, error);
            response_data.length_in_bytes = service_data->length_in_bytes - bytes;
            response_data.data = dptr + bytes;
            service_data->length_in_bytes = bytes;
        }
        else
        {
            response_data.data = service_data->data_ptr;
            response_data.length_in_bytes = service_data->length_in_bytes;
            response_data.length_in_bytes = 0;
        }
        break;

    case msg_service_type_have_data:
        {
            uint8_t * dptr = service_data->data_ptr;
            uint8_t const opcode = *dptr++;

            ASSERT_GOTO(MsgIsStart(service_data->flags), error);
            ASSERT_GOTO(opcode == data_service_opcode_put_response, error);

            request_data.request_type = idigi_data_put_have_data;
            response_data.flags = IDIGI_MSG_FIRST_DATA | IDIGI_MSG_LAST_DATA;
            response_data.length_in_bytes = service_data->length_in_bytes - sizeof opcode;
            response_data.data = dptr;
        }
        break;

    case msg_service_type_error:
        request_data.request_type = idigi_data_put_error;
        response_data.length_in_bytes = service_data->length_in_bytes;
        response_data.data = service_data->data_ptr;
        break;
            
    case msg_service_type_free:
        free_data(idigi_ptr, context);
        status = idigi_callback_continue;
        break;

    default:
        ASSERT_GOTO(false, error);
        break;
    }

    if (service_data->service_type != msg_service_type_free)
    {
        idigi_request_t request;
        size_t response_bytes = sizeof request_data;

        request.data_service_request = idigi_data_service_put_request;
        status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request, &request_data, sizeof request_data, &response_data, &response_bytes);
        if (status == idigi_callback_continue)
        {
            if (response_data.response_type == idigi_data_put_error) 
            {
                status = idigi_callback_unrecognized;
                goto error;
            }

            if (service_data->service_type == msg_service_type_need_data)
            {
                service_data->length_in_bytes += response_data.length_in_bytes;
    
                service_data->flags = 0;
                if ((response_data.flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
                    MsgSetLastData(service_data->flags);
            }
        }
    }

error:
    return status;
}

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_data)
{
    idigi_callback_status_t status = idigi_callback_abort;
    data_service_context_t * const context = service_data->session->service_context;

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
            ASSERT(false);
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

static data_service_context_t * data_service_create_context(idigi_data_t * const idigi_ptr, idigi_data_service_request_t const type)
{
    data_service_context_t * context = NULL;
    void * ptr;
    idigi_callback_status_t const status = malloc_data(idigi_ptr, sizeof *context, &ptr);

    if (status == idigi_callback_continue)
    {
        context = ptr;
        context->request_type = type;
    }

    return context;
}

static idigi_status_t data_service_initiate(idigi_data_t * const idigi_ptr,  void const * request, void  * response)
{
    idigi_status_t status = idigi_invalid_data;
    data_service_context_t * context;
    idigi_msg_error_t result;

    ASSERT_GOTO(request != NULL, done);

    context = data_service_create_context(idigi_ptr, idigi_data_service_put_request);
    if (context == NULL)
    {
        status = idigi_no_resource;
        goto done;
    }

    {
        msg_session_t * const session = msg_start_session(idigi_ptr, msg_service_id_data, &result);
        if (session == NULL) 
        {
            free_data(idigi_ptr, context);
            status = (result == idigi_msg_error_memory) ? idigi_no_resource : idigi_service_busy;
            goto done;
        }
    
        context->user_context = (void *)request;
        session->service_context = context;

        {
            msg_session_t ** const session_handle = response;

            *session_handle = session;
        }
    }

    status = idigi_success;

done:
    return status;
}


