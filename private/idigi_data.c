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
#define DS_MAX_TARGET_STRING_LENGTH 256
    bool response_started;
    msg_data_info_t info;
    char target_string[DS_MAX_TARGET_STRING_LENGTH];
    void * user_context;
} ds_device_request_service_t;

typedef struct {
    uint8_t service_opcode;
    union {
        ds_device_request_service_t device_request;
    } service_data;
} ds_service_data_t;

static idigi_callback_status_t ds_config(idigi_data_t * const idigi_ptr, idigi_data_service_request_t const ds_request_id,
                                           void * const request, size_t const request_size,
                                           void * response, size_t * response_size)
{
    idigi_callback_status_t status;
    unsigned timeout;
    uint32_t start_time_stamp;
    uint32_t end_time_stamp;
    uint32_t time_to_send_rx_keepalive;
    uint32_t time_to_receive_tx_keepalive;
    idigi_request_t const request_id = {ds_request_id};

    /* Calculate the timeout value (when to send rx keepalive or
     * receive tx keepalive) from last rx keepalive or tx keepalive.
     *
     * If callback exceeds the timeout value, error status callback
     * will be called.
     */
    status =  get_keepalive_timeout(idigi_ptr, &time_to_send_rx_keepalive, &time_to_receive_tx_keepalive, &start_time_stamp);
    if (status != idigi_callback_continue)
    {
        if (time_to_send_rx_keepalive == 0)
        {
            /*  needs to return immediately for rx_keepalive. */
            status = idigi_callback_busy;
        }
        goto done;
    }

    timeout = MIN_VALUE(time_to_send_rx_keepalive, time_to_receive_tx_keepalive);


    /* put the timeout value in the request pointer
     * (1st field is timeout field for all ds callback)
     */
    if (request != NULL)
    {
        unsigned * const req_timeout = (unsigned * const)request;

        *req_timeout = timeout;
    }

    status = idigi_callback(idigi_ptr->callback, idigi_class_data_service, request_id, request, request_size, response, response_size);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
        goto done;
    }

    if (get_system_time(idigi_ptr, &end_time_stamp) != idigi_callback_continue)
    {
        idigi_ptr->error_code = idigi_configuration_error;
        status = idigi_callback_abort;
        goto done;
    }

    if ((end_time_stamp- start_time_stamp) > timeout)
    {
        /* callback exceeds timeout value.
         * No need to abort just notify caller.
         *
         * Server will disconnect us if we miss sending
         * Rx keepalive.
         */
        DEBUG_PRINTF("get_fw_config: callback exceeds timeout > %u seconds\n", timeout);
        notify_error_status(idigi_ptr->callback, idigi_class_data_service, request_id, idigi_exceed_timeout);
    }

done:
    return status;
}


static idigi_callback_status_t ds_process_device_request(idigi_data_t * const idigi_ptr,
                                                         msg_session_t * const session,
                                                         msg_status_t const msg_status,
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
    ds_service_data_t * const service_data = session->service_data;
    ds_device_request_service_t * const device_request = &service_data->service_data.device_request;
    uint16_t flag = 0;

    uint8_t const * ds_device_request = data;


    switch (msg_status)
    {
    case msg_status_start:
    case msg_status_start_and_last:
    {
        /* This is the start message which contains Data Service Device Request header. */

        /* get and save target */
        uint8_t const target_length =  message_load_u8(ds_device_request, target_length);
        ds_device_request += record_bytes(ds_device_request_header);
        ASSERT(target_length < (sizeof device_request->target_string-1));

        memcpy(device_request->target_string, ds_device_request, target_length);
        device_request->target_string[target_length] = '\0';
        device_request->info.flag = 0;
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

             ds_device_request += record_bytes(ds_device_request_parameter); /* skip id and length */
             ds_device_request += parameter_length;
             ASSERT(length >= (size_t)(ds_device_request - data));
        }
    }
    /* fall thru for calling the callback */
    case msg_status_last:
    case msg_status_data:
    {
        idigi_ds_device_request_t request_data;
        request_data.user_context = device_request->user_context;
        request_data.target = device_request->target_string;
        request_data.data = ds_device_request;
        request_data.data_length = length - (ds_device_request - data);
        request_data.session_id = session->session_id;
        request_data.flag = ((msg_status == msg_status_start) || (msg_status == msg_status_data))? flag : (flag |IDIGI_DATA_REQUEST_LAST);
        status = ds_config(idigi_ptr, idigi_data_service_device_request, &request_data, sizeof request_data, &device_request->user_context, NULL);
        break;
    }
    default:
        ASSERT(false);
        break;
    }

    return status;
}

static idigi_callback_status_t data_service_callback(idigi_data_t * const idigi_ptr, msg_status_t const msg_status, msg_session_t * const session, uint8_t const * data, size_t const length)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_request_t request;

#if defined(DEBUG)
    static char * msg_status_text[] =
    {
        "msg_status_send_complete",
        "msg_status_error",
        "msg_status_start",
        "msg_status_start_and_last",
        "msg_status_data",
        "msg_status_last"
    };
#endif

    ASSERT_GOTO(idigi_ptr != NULL, error);
    ASSERT_GOTO(session != NULL, error);

    {
        bool const non_empty_data = ((data != NULL) && (length > 0));
        bool const empty_data = ((data == NULL) && (length == 0));

        ASSERT_GOTO(non_empty_data || empty_data, error);
    }

    DEBUG_PRINTF("data_service_callback: msg_status = %s (%d)\n", msg_status_text[msg_status], msg_status);
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

        case msg_status_data:
        case msg_status_last:
            /* currently, we only handle these msg_status_data & msg_status_end for
             * Data Service Device Request message
             */
            ASSERT_GOTO(session->service_data != NULL, error);
            {
                ds_service_data_t * const ds_service = session->service_data;
                ASSERT_GOTO(ds_service->service_opcode == data_service_device_request_opcode, error);
            }
            status = ds_process_device_request(idigi_ptr,session, msg_status, data, length);
            break;

        case msg_status_start:
        case msg_status_start_and_last:
        {
            uint8_t const opcode = *data;
            if (opcode == data_service_device_request_opcode)
            {
                ds_service_data_t * ds_service;
                void * ptr;
                status = malloc_data(idigi_ptr, sizeof *ds_service, &ptr);
                if (status != idigi_callback_continue)
                {
                    goto error;
                }
                ds_service = ptr;
                ds_service->service_opcode = opcode;

                session->service_data = ds_service;
                status = ds_process_device_request(idigi_ptr,session, msg_status, data, length);
            }
            else
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
            }
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

            ASSERT_GOTO(service->session == NULL, error);
            *session_ptr = msg_send_start(data_ptr, service->session, msg_service_id_data, &info);
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

static idigi_status_t data_service_response_initiate(idigi_data_t * const idigi_ptr,  idigi_ds_device_response_t const * const request)
{
    idigi_status_t result = idigi_invalid_data;
    idigi_msg_data_t const * const msg_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_MSG_NUM);
    msg_session_t * session;
    ds_service_data_t * service_data;

    ASSERT_GOTO(request != NULL, done);

    session = msg_find_session(msg_ptr, request->session_id);
    ASSERT_GOTO(session != NULL, done);

    service_data = session->service_data;

    switch (service_data->service_opcode)
    {
    case data_service_device_request_opcode:
    {
        ds_device_request_service_t * const device_request = &service_data->service_data.device_request;
        msg_data_info_t * const info = &device_request->info;

        if (!device_request->response_started)
        { /* initiate send */
            enum {
                field_define(ds_device_response, opcode, uint8_t),
                field_define(ds_device_response, status, uint8_t),
                record_end(ds_device_response_header)
            };
            uint8_t * ds_device_response = info->header;

            message_store_u8(ds_device_response, opcode, data_service_device_response_opcode);
            message_store_u8(ds_device_response, status, request->status);
            info->header_length = record_bytes(ds_device_response_header);

            info->payload = request->data;
            info->payload_length = request->data_length;
            info->flag = (request->flag & IDIGI_DATA_REQUEST_LAST) | IDIGI_DATA_REQUEST_START;
            {
                void * ptr;
                ptr = msg_send_start(idigi_ptr, session, msg_service_id_data, info);
                ASSERT(ptr == session);
                if (ptr != NULL)
                {
                    result = idigi_success;
                    device_request->response_started = true;
                }
                else
                {
                    result = idigi_configuration_error;
                }
            }
        }
        else
        {
            info->header_length = 0;
            info->payload_length = request->data_length;
            info->payload = request->data;
            info->flag = (request->flag & IDIGI_DATA_REQUEST_LAST);

            idigi_callback_status_t const status = msg_send_data(idigi_ptr, session, info);
            result = (status == idigi_callback_continue) ? idigi_success : idigi_configuration_error;
        }
        break;
    }

    default:
        break;
    }

done:
    return result;
}

