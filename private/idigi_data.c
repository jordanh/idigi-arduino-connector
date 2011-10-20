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

enum {
    data_service_put_request_opcode,
    data_service_put_response_opcode,
    data_service_device_request_opcode,
    data_service_device_response_opcode
};

typedef struct {
    bool response_started;
    char *target_string;
    void * user_context;
} ds_device_request_service_t;

static idigi_callback_status_t ds_process_device_request(idigi_data_t * const idigi_ptr,
                                                         msg_session_t * const session,
                                                         msg_state_t const msg_state,
                                                         uint8_t const * data,
                                                         size_t const length)
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

    idigi_callback_status_t status = idigi_callback_continue;
    ds_device_request_service_t * device_request = session->service_data;
    uint8_t const * ds_device_request = data;
    idigi_msg_error_t error_code = idigi_msg_error_cancel;
    uint16_t flag = 0;

    switch (msg_state)
    {
    case msg_state_start:
    case msg_state_start_and_last:
    {
        size_t min_data_length;

        {
            /* This is the start message which contains Data Service Device Request header.
             * Let's get and save target field.
             */
            uint8_t const target_length =  message_load_u8(ds_device_request, target_length);

            min_data_length = target_length +
                              record_bytes(ds_device_request_header) +
                              field_named_data(ds_device_request, parameter_count, size);
            ds_device_request += record_bytes(ds_device_request_header);

            ASSERT_GOTO(length > min_data_length, error);

            if (session->service_data == NULL)
            {
                /* 1st time here so let's allocate memory for handling device request message */
                void * ptr;

                status = malloc_data(idigi_ptr, sizeof *device_request + target_length +1, &ptr);
                if (status != idigi_callback_continue)
                {
                    error_code = idigi_msg_error_memory;
                    goto error;
                }
                device_request = ptr;
                device_request->target_string = (char *)device_request + sizeof *device_request;
                session->service_data = device_request;
                memcpy(device_request->target_string, ds_device_request, target_length);
                device_request->target_string[target_length] = '\0';
                device_request->user_context = NULL;
            }

            device_request->response_started = false;
            ds_device_request += target_length;
            flag = IDIGI_DATA_REQUEST_START;
        }

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
                 ASSERT_GOTO(length >= min_data_length, error);

                 ds_device_request += record_bytes(ds_device_request_parameter); /* skip id and length */
                 ds_device_request += parameter_length;
            }
        }
    }
    /* fall thru for calling the callback */
    case msg_state_last:
    case msg_state_data:
    {
        idigi_request_t const request_id = {idigi_data_service_request};
        idigi_data_service_device_request_t request_data;
        ASSERT_GOTO(device_request != NULL, error);

        request_data.user_context = device_request->user_context;
        request_data.target = device_request->target_string;
        request_data.data = ds_device_request;
        request_data.data_length = length - (ds_device_request - data);
        request_data.session = session;
        request_data.flag = ((msg_state == msg_state_start) || (msg_state == msg_state_data))? flag : (flag | IDIGI_DATA_REQUEST_LAST);
        status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id, &request_data, sizeof request_data, &device_request->user_context, NULL);
        if (status == idigi_callback_abort)
        {
            idigi_ptr->error_code = idigi_configuration_error;
        }
        else if (status == idigi_callback_unrecognized)
        {
            error_code = idigi_msg_error_no_service;
            goto error;
        }
        break;
    }
    default:
        ASSERT(false);
        goto error;
    }
    goto done;

error:
    /* send error to cancel server's request message */
    status = msg_send_error(idigi_ptr, session, session->session_id, error_code , MSG_FLAG_REQUEST);

done:
    return status;
}

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_state_t const msg_state, msg_session_t * const session, uint8_t const * data, size_t const length)
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

    switch (msg_state) 
    {
        case msg_state_error:
        {
            idigi_data_error_t error_info;

            error_info.session = session;
            error_info.error = *data;
            request.data_service_request = idigi_data_service_error;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &error_info, sizeof error_info);
            break;
        }

        case msg_state_send_complete:
        {
            idigi_data_send_t send_info;

            send_info.session = session;
            send_info.payload = data;
            send_info.bytes_sent = length;
            send_info.status = idigi_success;
            request.data_service_request = idigi_data_service_send_complete;
            status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &send_info, sizeof send_info);
            break;
        }

        case msg_state_start:
        case msg_state_start_and_last:
        {
            uint8_t const opcode = *data;

            if (opcode == data_service_device_request_opcode)
            {
                status = ds_process_device_request(idigi_ptr, session, msg_state, data, length);
            }
            else
            {
                idigi_data_response_t response;

                {
                    uint8_t const data_service_response = 1;
                    ASSERT_GOTO(opcode == data_service_response, error);
                }
                data++; /* skip opcode */
                response.status = *data++;

                response.session = session;
                response.message.size = length - 2; /* opcode(1 byte) + status(1 byte) */
                response.message.value = data;

                request.data_service_request = idigi_data_service_response;
                status = idigi_callback_no_response(idigi_ptr->callback, idigi_class_data_service, request, &response, sizeof response);
            }
            break;
        }

        case msg_state_data:
        case msg_state_last:
            /* currently, we only handle these msg_status_data & msg_status_end for
             * Data Service Device Request message
             */
            ASSERT_GOTO(session->service_data != NULL, error);
            status = ds_process_device_request(idigi_ptr, session, msg_state, data, length);
            break;

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

static idigi_callback_status_t idigi_facility_data_service_cleanup(idigi_data_t * const idigi_ptr)
{
    return msg_cleanup_all_sessions(idigi_ptr,  msg_service_id_data);
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
#define SERVICE_PATH_MAX_LENGTH    256
#define SERVICE_PARAM_MAX_LENGTH   32
#define SERVICE_HEADER_MAX_LENGTH  (SERVICE_PATH_MAX_LENGTH + SERVICE_PARAM_MAX_LENGTH)

    idigi_status_t status = idigi_invalid_data;
    idigi_data_request_t const * service = request;
    idigi_msg_data_t * const msg_ptr = get_facility_data(data_ptr, E_MSG_FAC_MSG_NUM);

    ASSERT_GOTO(request != NULL, error);

    if (msg_ptr->capabilities[msg_server_request_id].window_size == 0)
    {
        status = idigi_init_error;
        goto error;
    }

    if ((service->flag & IDIGI_DATA_REQUEST_START) != 0)
    {
        msg_data_info_t info;
        uint8_t header_buffer[SERVICE_HEADER_MAX_LENGTH];

        ASSERT_GOTO(response != NULL, error);

        info.header = header_buffer;
        info.header_length = fill_data_service_header(service, info.header);
        info.payload = service->payload.data;
        info.payload_length = service->payload.size;
        info.flag = service->flag;

        {
            void ** const session_ptr = response;

            ASSERT_GOTO(service->session == NULL, error);
            *session_ptr = msg_send_start(data_ptr, msg_ptr, service->session, msg_service_id_data, &info);
            status = (*session_ptr != NULL) ? idigi_success : idigi_no_resource;
        }
    }
    else
    {
        msg_data_info_t info;
        idigi_callback_status_t ret_status;

        info.header_length = 0;
        info.header = NULL;
        info.payload_length = service->payload.size;
        info.payload = service->payload.data;
        info.flag = service->flag;
        ret_status = msg_send_data(data_ptr, msg_ptr, service->session, &info);
        status = (ret_status == idigi_callback_continue) ? idigi_success : idigi_configuration_error;
    }

error:
    return status;
}

static idigi_status_t data_service_response_initiate(idigi_data_t * const idigi_ptr,  idigi_data_service_device_response_t const * const request)
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

    idigi_status_t result = idigi_invalid_data;
    idigi_msg_data_t * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session;
    msg_data_info_t info;

    uint8_t ds_device_response[record_bytes(ds_device_response_header)];

    ASSERT_GOTO(request != NULL, done);
    session = request->session;

    ASSERT_GOTO(msg_find_session(msg_ptr, session->session_id, msg_server_request_id) == session, done);
    ASSERT_GOTO(session->service_data != NULL, done);

    {
        ds_device_request_service_t * const device_request = session->service_data;

        if (!device_request->response_started)
        { /* let's construct the response */
            message_store_u8(ds_device_response, opcode, data_service_device_response_opcode);
            message_store_u8(ds_device_response, status, request->status);
            info.header = ds_device_response;
            info.header_length = record_bytes(ds_device_response_header);

            info.payload = request->data;
            info.payload_length = request->data_length;
            info.flag = (request->flag & IDIGI_DATA_REQUEST_LAST) | IDIGI_DATA_REQUEST_START;
            device_request->response_started = true;
        }
        else
        {
            info.header_length = 0;
            info.header = NULL;
            info.payload_length = request->data_length;
            info.payload = request->data;
            info.flag = (request->flag & IDIGI_DATA_REQUEST_LAST);

        }
    }

    {
        idigi_callback_status_t const status = msg_send_data(idigi_ptr, msg_ptr, session, &info);

        switch (status)
        {
        case idigi_callback_busy:
            result = idigi_service_busy;
            break;

        case idigi_callback_abort:
        case idigi_callback_unrecognized:
            /* set error here to cancel client's request message.
             * msg_process_pending() will actually send the error-message.
             */
            result = idigi_invalid_data;
            break;

        case idigi_callback_continue:
            result = idigi_success;
            break;
        }
    }

done:
    return result;
}

