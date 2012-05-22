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

#define FW_ID_STRING_LENGTH     128  /* bytes */

/* time to send target list to keep download alive */
#define FW_TARGET_LIST_MSG_INTERVAL_IN_SECONDS     30

/**
 * Firmware Upgrade Facility Opcodes
 * @doc These are the valid opcodes for the Firmware Upgrade Facility
 */
typedef enum {
    fw_target_list_opcode,
    fw_info_request_opcode,
    fw_info_response_opcode,
    fw_download_request_opcode,
    fw_download_response_opcode,
    fw_binary_block_opcode,
    fw_binary_block_ack_opcode,
    fw_download_abort_opcode,
    fw_download_complete_opcode,
    fw_download_complete_response_opcode,
    fw_target_reset_opcode,
    fw_download_status_opcode,
    fw_error_opcode
} fw_opcodes_t;

typedef union {
    enum {
        fw_invalid_target,
        fw_invalid_opcode,
        fw_invalid_msg
    } error_status;

    enum {
        fw_user_abort,
        fw_device_error,
        fw_invalid_offset,
        fw_invalid_data,
        fw_hardware_error
    } abort_status;

    idigi_fw_status_t   user_status;

} fw_abort_status_t;

typedef enum {
    fw_equal,
    fw_less_than,
    fw_greater_than
} fw_compare_type_t;

/* Firmware message header format:
 *  ------------------------
 * |   0    |   1    | 3... |
 *  ------------------------
 * | opcode | target | data |
 *  ------------------------
 *
 */
enum fw_message {
    field_define(fw_message, opcode, uint8_t),
    field_define(fw_message, target, uint8_t),
    record_end(fw_message)
};
#define FW_MESSAGE_HEADER_SIZE record_bytes(fw_message)
#define FW_MESSAGE_RESPONSE_MAX_SIZE    16

typedef struct {
    idigi_data_t * idigi_ptr;
    unsigned long last_fw_keepalive_sent_time;
    uint32_t version;
    uint32_t code_size;
    char * description;
    char * name_spec;
    size_t desc_length;
    size_t spec_length;
    uint16_t target_count;
    idigi_bool_t fw_keepalive_start;
    uint8_t target;
    uint8_t response_buffer[FW_MESSAGE_RESPONSE_MAX_SIZE + PACKET_EDP_FACILITY_SIZE];
    size_t  response_size;
    idigi_bool_t send_busy;
    idigi_bool_t update_started;

} idigi_firmware_data_t;

static idigi_callback_status_t get_fw_config(idigi_firmware_data_t * const fw_ptr, idigi_firmware_request_t const fw_request_id,
                                           void * const request, size_t const request_size,
                                           void * response, size_t * const response_size,
                                           fw_compare_type_t const compare_type)
{

    idigi_data_t * const idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status;
    unsigned long timeout;
    unsigned long start_time_stamp = 0;
    unsigned long end_time_stamp = 0;
    unsigned long time_to_send_rx_keepalive = 0;
    unsigned long time_to_receive_tx_keepalive;
    size_t  length = 0;
    idigi_request_t request_id;

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

    if (fw_ptr->last_fw_keepalive_sent_time > 0)
    {
        /* when download starts, we need to send target list on every
         * FW_TARGET_LIST_MSG_INTERVAL_IN_SECONDS second. This
         * last_fw_keealive_sent_time is set when we receive
         * fw_download_complete_opcode to trigger the firmware
         * keepalive timing for sending target list message.
         *
         */
        unsigned long const keepalive_timeout = get_elapsed_value(FW_TARGET_LIST_MSG_INTERVAL_IN_SECONDS,
                                       fw_ptr->last_fw_keepalive_sent_time,
                                       start_time_stamp);

        timeout = MIN_VALUE( timeout, keepalive_timeout);
    }

    /* put the timeout value in the request pointer
     * (1st field is timeout field for all fw callback)
     */
    if (request != NULL)
    {
        unsigned int * const req_timeout = request;
        /* we want callback to return immediately */
        *req_timeout = 0;

    }

    request_id.firmware_request = fw_request_id;
    status = idigi_callback(idigi_ptr->callback, idigi_class_firmware, request_id, request, request_size, response, &length);

    if (get_system_time(idigi_ptr, &end_time_stamp) != idigi_callback_continue)
    {
        status = idigi_callback_abort;
        goto done;
    }

    switch (status)
    {
    case idigi_callback_continue:
        if (response_size != NULL)
        {
            if ((compare_type == fw_equal && length != *response_size) ||
                (compare_type == fw_less_than && length > *response_size) ||
                (compare_type == fw_greater_than && length < *response_size))
            {
                /* bad data so let abort */
                idigi_debug("get_fw_config: returned invalid size %lu (length %lu)\n", (unsigned long int)length, (unsigned long int)*response_size);
                idigi_ptr->error_code = idigi_invalid_data_size;
                notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_invalid_data_size);
                 status = idigi_callback_abort;
                goto done;
            }

            *response_size = length;
        }
        break;
    case idigi_callback_busy:
        break;
    case idigi_callback_abort:
    case idigi_callback_unrecognized:
        idigi_ptr->error_code = idigi_configuration_error;
        status = idigi_callback_abort;
        goto done;
    }

    if ((end_time_stamp- start_time_stamp) > timeout+1)
    {
        /* Just notify callback that we exceed keepalive timeout.
         * No need to abort just notify caller.
         *
         * Server will disconnect us if we miss sending
         * Rx keepalive.
         */
        idigi_debug("get_fw_config: callback exceeds timeout %u  > %u seconds\n", (end_time_stamp- start_time_stamp), timeout);
        notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_exceed_timeout);
    }

    if (status == idigi_callback_busy && fw_ptr->last_fw_keepalive_sent_time > 0)
    {
        /*
         * Check whether we need to send target list message
         * to keep server alive.
         */
        fw_ptr->fw_keepalive_start = ((end_time_stamp - fw_ptr->last_fw_keepalive_sent_time) >= FW_TARGET_LIST_MSG_INTERVAL_IN_SECONDS) ? idigi_true : idigi_false;
    }
    else
    {
        fw_ptr->fw_keepalive_start = idigi_false;
    }

done:
    return status;
}

static fw_abort_status_t get_abort_status_code(idigi_fw_status_t const status)
{
    fw_abort_status_t code;

    code.abort_status = fw_user_abort;

    /* convert status to abort status code for abort message */
    switch (status)
    {
    case idigi_fw_user_abort:
        code.abort_status = fw_user_abort;
        break;
    case idigi_fw_invalid_offset:
        code.abort_status = fw_invalid_offset;
        break;
    case idigi_fw_invalid_data:
        code.abort_status = fw_invalid_data;
        break;
    case idigi_fw_hardware_error:
        code.abort_status = fw_hardware_error;
        break;
    case idigi_fw_device_error:
    case idigi_fw_download_denied:
    case idigi_fw_download_invalid_size:
    case idigi_fw_download_invalid_version:
    case idigi_fw_download_unauthenticated:
    case idigi_fw_download_not_allowed:
    case idigi_fw_download_configured_to_reject:
    case  idigi_fw_encountered_error:
        /* not abort status so defult to device error */
        code.abort_status = fw_device_error;
        break;
    case idigi_fw_success:
        ASSERT(idigi_false);
        break;

    }
    return code;
}

/* abort and error message format:
 *  --------------------------
 * |   0    |    1   |    2   |
 *  --------------------------
 * | opcode | target | status |
 *  --------------------------
 *
 */
enum fw_abort {
    field_define(fw_abort, opcode, uint8_t),
    field_define(fw_abort, target, uint8_t),
    field_define(fw_abort, status, uint8_t),
    record_end(fw_abort)
};

#define FW_ABORT_HEADER_SIZE    record_bytes(fw_abort)

static idigi_callback_status_t send_fw_message(idigi_firmware_data_t * const fw_ptr)
{

    idigi_callback_status_t status;

    status = initiate_send_facility_packet(fw_ptr->idigi_ptr, fw_ptr->response_buffer, fw_ptr->response_size, E_MSG_FAC_FW_NUM, NULL, NULL);
    fw_ptr->send_busy = (status == idigi_callback_busy) ? idigi_true : idigi_false;
    return status;

}

static idigi_callback_status_t send_fw_abort(idigi_firmware_data_t * const fw_ptr, uint8_t const target, uint8_t const msg_opcode, fw_abort_status_t const abort_status)
{

    idigi_callback_status_t status = idigi_callback_continue;

    uint8_t * fw_abort = GET_PACKET_DATA_POINTER(fw_ptr->response_buffer, PACKET_EDP_FACILITY_SIZE);
    uint8_t abort_code = (uint8_t)abort_status.error_status;

    ASSERT(abort_status.error_status <= UCHAR_MAX);

    /* need to adjust abort status code in the fw_status_t */
    if (msg_opcode != fw_error_opcode)
    {
        fw_abort_status_t status;
        status = get_abort_status_code(abort_status.user_status);

        ASSERT(status.abort_status <= UCHAR_MAX);
        abort_code = (uint8_t)status.abort_status;

    }

    ASSERT((sizeof fw_ptr->response_buffer - PACKET_EDP_FACILITY_SIZE) > FW_ABORT_HEADER_SIZE);

    /* build abort message */
    message_store_u8(fw_abort, opcode, msg_opcode);
    message_store_u8(fw_abort, target, target);
    message_store_u8(fw_abort, status, abort_code);

    fw_ptr->response_size = FW_ABORT_HEADER_SIZE;
    status = send_fw_message(fw_ptr);

    if (fw_ptr->target == target)
    {
        fw_ptr->update_started = idigi_false;
    }
    return status;

}

static idigi_callback_status_t  process_fw_info_request(idigi_firmware_data_t * const fw_ptr, uint8_t * const fw_message, uint16_t const length)
{
/* firmware info response message format:
 *  ---------------------------------------------------
 * |   0    |    1   |  2 - 5  |  6 - 9    |  10 ...   |
 *  ---------------------------------------------------
 * | opcode | target | version | Available | Firmware  |
 * |        |        |         | code size | ID string |
 *  ---------------------------------------------------
 *
 *  Firmware ID string = [descr]0xa[file name spec]
*/
enum fw_info {
    field_define(fw_info, opcode, uint8_t),
    field_define(fw_info, target, uint8_t),
    field_define(fw_info, version, uint32_t),
    field_define(fw_info, code_size, uint32_t),
    record_end(fw_info)
};

#define MAX_FW_INFO_REQUEST_LENGTH  2

    idigi_data_t * const idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_firmware_request_t const request_list[] = {
                                idigi_firmware_version, idigi_firmware_code_size,
                                idigi_firmware_description, idigi_firmware_name_spec};
    unsigned int const request_list_count = asizeof(request_list);
    uint8_t const target = message_load_u8(fw_message, target);
    unsigned int i;

    idigi_debug("Firmware Facility: process info request\n");
    /* parse firmware info request
     *  -----------------
     * |   0    |    1   |
     *  -----------------
     * | opcode | target |
     *  -----------------
     */
    fw_ptr->last_fw_keepalive_sent_time = 0;
    fw_ptr->fw_keepalive_start = idigi_false;

    if (length != MAX_FW_INFO_REQUEST_LENGTH)
    {
        fw_abort_status_t fw_status;
        idigi_debug("process_fw_info_request: invalid message length\n");

        fw_status.error_status = fw_invalid_msg;
        status = send_fw_abort(fw_ptr, target, fw_error_opcode, fw_status);
        goto done;

    }

    CONFIRM(offsetof(idigi_fw_config_t, timeout) == 0);

    /* let's get and build target info response */
    for (i=0; i < request_list_count; i++)
    {
        idigi_fw_config_t request;

        request.target = target;

        switch (request_list[i])
        {
        case idigi_firmware_version:
            /* add target number to the target list message before version*/
            status = get_fw_config(fw_ptr, idigi_firmware_version, &request, sizeof request,&fw_ptr->version, NULL, fw_equal);
            break;

        case idigi_firmware_code_size:
            status = get_fw_config(fw_ptr, idigi_firmware_code_size, &request, sizeof request, &fw_ptr->code_size, NULL, fw_equal);
            break;

        case idigi_firmware_description:
        case idigi_firmware_name_spec:
            if (request_list[i] == idigi_firmware_description)
            {
                fw_ptr->desc_length = FW_ID_STRING_LENGTH-fw_ptr->spec_length-1;
                status = get_fw_config(fw_ptr, idigi_firmware_description, &request, sizeof request, (void *)&fw_ptr->description, &fw_ptr->desc_length, fw_less_than);
            }
            else
            {
                fw_ptr->spec_length = FW_ID_STRING_LENGTH-fw_ptr->desc_length-1;
                status = get_fw_config(fw_ptr, idigi_firmware_name_spec, &request, sizeof request, (void *)&fw_ptr->name_spec, &fw_ptr->spec_length, fw_less_than);
            }

            if (status == idigi_callback_continue && (fw_ptr->desc_length + fw_ptr->spec_length) > (FW_ID_STRING_LENGTH -1))
            {
                idigi_request_t request_id;

                request_id.firmware_request = idigi_firmware_description;
                idigi_ptr->error_code = idigi_invalid_data_size;
                idigi_debug("process_fw_info_request: description length = %lu + name spec length = %lu\n",
                                        (unsigned long int)fw_ptr->desc_length, (unsigned long int)fw_ptr->spec_length);
                notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_ptr->error_code);
                fw_ptr->desc_length = 0;
                fw_ptr->spec_length = 0;
                status = idigi_callback_abort;
            }
            break;
        default:
            ASSERT(idigi_false);
            break;

        }
        if (status != idigi_callback_continue)
        {
            ASSERT(status == idigi_callback_abort);
            status = idigi_callback_abort;
            goto done;
        }
    }

    /* once it's done getting all firmware info,
     * let's build a response.
     * build and send firmware info response
    */
    if (i == request_list_count)
    {
        uint8_t * edp_header;
        uint8_t * fw_info;
        uint8_t * start_ptr;
        size_t avail_length;

        edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &fw_info, &avail_length);
        if (edp_header == NULL)
        {
            status = idigi_callback_busy;
            goto done;
        }
        start_ptr = fw_info;

        ASSERT(avail_length > (record_bytes(fw_info) + fw_ptr->desc_length + fw_ptr->spec_length + 1));

        message_store_u8(fw_info, opcode, fw_info_response_opcode);
        message_store_u8(fw_info, target, target);
        message_store_be32(fw_info, version, fw_ptr->version);
        message_store_be32(fw_info, code_size, fw_ptr->code_size);
        fw_info += record_bytes(fw_info);

        if (fw_ptr->description != NULL)
        {
            memcpy(fw_info, fw_ptr->description, fw_ptr->desc_length);
            fw_info += fw_ptr->desc_length;
        }
        *fw_info++ = '\n';

        if (fw_ptr->name_spec != NULL)
        {
            memcpy(fw_info, fw_ptr->name_spec, fw_ptr->spec_length);
            fw_info += fw_ptr->spec_length;
        }

        /* reset back to initial values */
        fw_ptr->version = 0;
        fw_ptr->code_size = 0;
        fw_ptr->desc_length = 0;
        fw_ptr->spec_length = 0;

        status = initiate_send_facility_packet(idigi_ptr, edp_header, fw_info-start_ptr, E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);

        if (status != idigi_callback_continue)
        {
            release_packet_buffer(idigi_ptr, edp_header, idigi_success, NULL);
        }
    }

done:
    return status;
}

static idigi_callback_status_t process_fw_download_request(idigi_firmware_data_t * const fw_ptr, uint8_t * fw_download_request, uint16_t const length)
{

/* Firmware download request message format:
 *  -----------------------------------------------------------
 * |   0    |   1    |  2 - 5  |  6 - 9    |  10...             |
 *  -----------------------------------------------------------
 * | opcode | target | version | code size | firmware ID string |
 *  ------------------------------------------------------------
 *
 *  Firmware ID string: [label]0x0a[file name spec]0xa[file name]
 *
 *  Call the callback with these values and send download request response.
 */
enum fw_download_request {
    field_define(fw_download_request, opcode, uint8_t),
    field_define(fw_download_request, target, uint8_t),
    field_define(fw_download_request, version, uint32_t),
    field_define(fw_download_request, code_size, uint32_t),
    record_end(fw_download_request)
};


/* Firmware download response message format:
 *  ---------------------------------
 * |  0     |   1    |     2         |
 *  ---------------------------------
 * | opcode | target | response type |
 *  ---------------------------------
 *
 */
enum fw_download_response {
    field_define(fw_download_response, opcode, uint8_t),
    field_define(fw_download_response, target, uint8_t),
    field_define(fw_download_response, response_type, uint8_t),
    record_end(fw_download_response)
};

    idigi_callback_status_t status = idigi_callback_continue;
    idigi_fw_download_request_t request_data;
    idigi_fw_status_t response_status = idigi_fw_device_error;
    uint16_t string_id_length;

    uint8_t abort_opcode = fw_download_abort_opcode;

    CONFIRM(offsetof(idigi_fw_download_request_t, timeout) == 0);

    request_data.target = message_load_u8(fw_download_request, target);

    if (length < record_bytes(fw_download_request))
    {
        idigi_debug("process_fw_download_request: invalid message length\n");
        abort_opcode = fw_error_opcode;
        response_status = (idigi_fw_status_t)fw_invalid_msg;
        goto error;
    }

    if (fw_ptr->update_started == idigi_true)
    {
        idigi_debug("process_fw_download_reqeust: cannot start another firmware update target %d\n", request_data.target);
        goto error;
    }

    /* Parse firmware download request. Then, call the callback
     * with these values and send download request response.
     */
    request_data.version = message_load_be32(fw_download_request, version);
    request_data.code_size = message_load_be32(fw_download_request, code_size);

    request_data.file_name_spec = NULL;
    request_data.filename = NULL;
    request_data.desc_string = NULL;

    string_id_length = length - record_bytes(fw_download_request);

    {
        char * string_id_ptr = (char *)fw_download_request;
        unsigned int i;
        char ** string_id_items[2];
        string_id_items[0] = &request_data.desc_string;
        string_id_items[1] = &request_data.file_name_spec;

        string_id_ptr += record_bytes(fw_download_request);

        /* parse firmware ID String for label and filename spec
         * separated by 0x0a.
         */
        for (i=0; i < asizeof(string_id_items); i++)
        {
            char * end_ptr;
            *string_id_items[i]= string_id_ptr;

            end_ptr = strchr(string_id_ptr, '\n');

            ASSERT(end_ptr != NULL);
            string_id_length -= (1 + end_ptr - string_id_ptr);
            *end_ptr = '\0';

            string_id_ptr++;
        }
        /* get filename */
        request_data.filename = string_id_ptr;
        *(request_data.filename + string_id_length) = '\0';
    }

    /* call callback */
    status = get_fw_config(fw_ptr, idigi_firmware_download_request, &request_data, sizeof request_data, &response_status, NULL, fw_equal);
error:
    if (status != idigi_callback_busy)
    {
        uint8_t * fw_download_response = GET_PACKET_DATA_POINTER(fw_ptr->response_buffer, PACKET_EDP_FACILITY_SIZE);

        if (response_status >= idigi_fw_user_abort)
        {
            fw_abort_status_t fw_status;

            fw_status.user_status = response_status;
            status = send_fw_abort(fw_ptr, request_data.target, abort_opcode, fw_status);
            goto done;
        }

        /* get a buffer for sending a response */
        ASSERT((sizeof fw_ptr->response_buffer - PACKET_EDP_FACILITY_SIZE) > record_bytes(fw_download_response));

        /* send firmware download response */
        message_store_u8(fw_download_response, opcode, fw_download_response_opcode);
        message_store_u8(fw_download_response, target, request_data.target);
        message_store_u8(fw_download_response, response_type, response_status);

        fw_ptr->response_size = record_bytes(fw_download_response);

        status = send_fw_message(fw_ptr);
        if (response_status == idigi_fw_success)
        {
            fw_ptr->update_started = idigi_true;
            fw_ptr->target = request_data.target;
        }

    }

done:
    return status;
}

static idigi_callback_status_t process_fw_binary_block(idigi_firmware_data_t * const fw_ptr, uint8_t * const fw_binary_block, uint16_t const length)
{
/* Firmware binary block message format:
 *  --------------------------------------------------------
 * |   0    |   1    |     2        |  3 - 6  |   7..       |
 *  --------------------------------------------------------
 * | opcode | target | Ack required |  offset | binary data |
 *  --------------------------------------------------------
 *
 */
enum fw_binary_block {
    field_define(fw_binary_block, opcode, uint8_t),
    field_define(fw_binary_block, target, uint8_t),
    field_define(fw_binary_block, ack_required, uint8_t),
    field_define(fw_binary_block, offset, uint32_t),
    record_end(fw_binary_block)
};

/* Firmware binary block acknowledge message format:
 *  -----------------------------------
 * |   0    |    1   | 2 - 5  |    6   |
 *  -----------------------------------
 * | opcode | target | offset | status |
 *  -----------------------------------
 *
 */
enum fw_binary_ack {
    field_define(fw_binary_ack, opcode, uint8_t),
    field_define(fw_binary_ack, target, uint8_t),
    field_define(fw_binary_ack, offset, uint32_t),
    field_define(fw_binary_ack, status, uint8_t),
    record_end(fw_binary_ack)
};

    idigi_data_t * const idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status = idigi_callback_abort;
    uint8_t ack_required;
    idigi_fw_image_data_t request_data;

    idigi_fw_status_t response_status = idigi_fw_user_abort;

    CONFIRM(offsetof(idigi_fw_image_data_t, timeout) == 0);

    /* Parse firmware binary block */
    request_data.target = message_load_u8(fw_binary_block, target);

    if (fw_ptr->update_started == idigi_false)
    {
        /* Ignore this packet since we have not started downloading.
         * We may already abort download request.
         */
        status = idigi_callback_continue;
        goto done;
    }

    if ((fw_ptr->target != request_data.target) || (length < record_bytes(fw_binary_block)))
    {
        idigi_request_t request_id;
        fw_abort_status_t fw_status;

        request_id.firmware_request = idigi_firmware_download_request;
        idigi_debug("process_fw_binary_block: invalid target or message length\n");
        fw_status.error_status = fw_invalid_msg;
        status = send_fw_abort(fw_ptr, request_data.target, fw_error_opcode, fw_status);
        if (status == idigi_callback_continue)
        {
            /* need to notify application during firmware update */
            notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_invalid_packet);
        }
        goto done;
    }

    /* Parse firmware binary block */
    ack_required = message_load_u8(fw_binary_block, ack_required);
    request_data.offset = message_load_be32(fw_binary_block, offset);
    request_data.length = length - record_bytes(fw_binary_block);

    request_data.data = (fw_binary_block + record_bytes(fw_binary_block));

    status = get_fw_config(fw_ptr, idigi_firmware_binary_block, &request_data, sizeof request_data, &response_status, NULL, fw_equal);

    if (status == idigi_callback_continue && response_status == idigi_fw_success)
    {

        if(ack_required)
        {
            uint8_t * fw_binary_ack = GET_PACKET_DATA_POINTER(fw_ptr->response_buffer, PACKET_EDP_FACILITY_SIZE);

            ASSERT((sizeof fw_ptr->response_buffer - PACKET_EDP_FACILITY_SIZE) > record_bytes(fw_binary_ack));
            /* send firmware binary block acknowledge */
            message_store_u8(fw_binary_ack, opcode, fw_binary_block_ack_opcode);
            message_store_u8(fw_binary_ack, target,request_data.target);
            message_store_be32(fw_binary_ack, offset, request_data.offset);
            message_store_u8(fw_binary_ack, status, idigi_fw_success);

            fw_ptr->response_size = record_bytes(fw_binary_ack);
            status = send_fw_message(fw_ptr);
        }
    }
    else if (status != idigi_callback_busy)
    {
        fw_abort_status_t fw_status;
        fw_status.user_status = response_status;
        status = send_fw_abort(fw_ptr, request_data.target, fw_download_abort_opcode, fw_status);
    }
done:
    return status;
}

static idigi_callback_status_t process_fw_abort(idigi_firmware_data_t * const fw_ptr, uint8_t * const fw_abort, uint16_t const length)
{

    idigi_callback_status_t status = idigi_callback_continue;

    /* parse firmware download abort */
    if (length != FW_ABORT_HEADER_SIZE)
    {
        idigi_debug("process_fw_abort: invalid message length\n");
    }
    else if (fw_ptr->update_started)
    {
        idigi_fw_download_abort_t request_data;
        uint8_t abort_status = message_load_u8(fw_abort, status);

        CONFIRM(offsetof(idigi_fw_download_abort_t, timeout) == 0);

        request_data.target = message_load_u8(fw_abort, target);
        switch (abort_status)
        {
        case fw_user_abort:
            request_data.status = idigi_fw_user_abort;
            break;
        case fw_device_error:
            request_data.status = idigi_fw_device_error;
            break;
        case fw_invalid_offset:
            request_data.status = idigi_fw_invalid_offset;
            break;
        case fw_invalid_data:
            request_data.status = idigi_fw_invalid_data;
            break;
        case fw_hardware_error:
            request_data.status = idigi_fw_hardware_error;
            break;
        default:
            ASSERT(idigi_false);
            break;
        }

        /* call callback */
        if (fw_ptr->target == request_data.target)
        {
            status = get_fw_config(fw_ptr, idigi_firmware_download_abort, &request_data, sizeof request_data, NULL, NULL, fw_equal);
            if (status != idigi_callback_busy)
            {
                fw_ptr->update_started = idigi_false;
            }
            fw_ptr->last_fw_keepalive_sent_time = 0;
        }
    }
    return status;

}

static idigi_callback_status_t process_fw_complete(idigi_firmware_data_t * const fw_ptr, uint8_t * const fw_complete_request, uint16_t const length)
{

/* Firmware downlaod complete message format:
 *  ----------------------------------------
 * |   0    |   1    |   2 - 5   |  6 - 9   |
 *  ----------------------------------------
 * | opcode | target | code size | checksum |
 *  ----------------------------------------
 *
 */
enum fw_complete_request {
    field_define(fw_complete_request, opcode, uint8_t),
    field_define(fw_complete_request, target, uint8_t),
    field_define(fw_complete_request, code_size, uint32_t),
    field_define(fw_complete_request, checksum, uint32_t),
    record_end(fw_complete_request)
};

/* Firmware download complete response message format:
 *  -------------------------------------------------
 * |   0    |   1    |  2 - 5  |  6 - 9     |  10    |
 *  --------------------------------------------------
 * | opcode | target | version | calculated | status |
 * |        |        |         |  checksum  |        |
 *  --------------------------------------------------
 */
enum fw_complete_response {
    field_define(fw_complete_response, opcode, uint8_t),
    field_define(fw_complete_response, target, uint8_t),
    field_define(fw_complete_response, version, uint32_t),
    field_define(fw_complete_response, checksum, uint32_t),
    field_define(fw_complete_response, status, uint8_t),
    record_end(fw_complete_response)
};

    idigi_data_t * const idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_fw_download_complete_request_t request_data;
    idigi_fw_download_complete_response_t response_data;

    CONFIRM(offsetof(idigi_fw_download_complete_request_t, timeout) == 0);

    request_data.target = message_load_u8(fw_complete_request, target);

    if ((length != record_bytes(fw_complete_request)) ||
        (fw_ptr->update_started == idigi_false) ||
        (fw_ptr->target != request_data.target))
    {
        idigi_request_t request_id;
        fw_abort_status_t  fw_status;

        request_id.firmware_request = idigi_firmware_download_complete;
        idigi_debug("process_fw_complete: invalid message length\n");
        fw_status.error_status = fw_invalid_msg;
        status = send_fw_abort(fw_ptr, request_data.target, fw_error_opcode, fw_status);
        if (status == idigi_callback_continue)
        {
            /* need to notify application to end download complete */
            notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_invalid_packet);
        }
        goto done;
    }

    /* Parse firmware downlaod complete */
    request_data.code_size = message_load_be32(fw_complete_request, code_size);
    request_data.checksum = message_load_be32(fw_complete_request, checksum);

    /* call callback */
    response_data.calculated_checksum = 0; /* set to 0 since it's reserved */
    status = get_fw_config(fw_ptr, idigi_firmware_download_complete, &request_data, sizeof request_data, &response_data, NULL, fw_equal);
    if (status == idigi_callback_continue)
    {
        uint8_t * fw_complete_response = GET_PACKET_DATA_POINTER(fw_ptr->response_buffer, PACKET_EDP_FACILITY_SIZE);

        ASSERT((sizeof fw_ptr->response_buffer - PACKET_EDP_FACILITY_SIZE) > record_bytes(fw_complete_response));

        /* send firmware download complete response */
        message_store_u8(fw_complete_response, opcode, fw_download_complete_response_opcode);
        message_store_u8(fw_complete_response, target, request_data.target);
        message_store_be32(fw_complete_response, version, response_data.version);
        message_store_be32(fw_complete_response, checksum, response_data.calculated_checksum);
        message_store_u8(fw_complete_response, status, response_data.status);

        fw_ptr->last_fw_keepalive_sent_time = 0;
        fw_ptr->fw_keepalive_start = idigi_false;

        fw_ptr->response_size = record_bytes(fw_complete_response);
        status = send_fw_message(fw_ptr);
        fw_ptr->update_started = idigi_false;
    }
    else if (status != idigi_callback_busy)
    {
        fw_abort_status_t fw_status;

        fw_status.user_status = idigi_fw_user_abort;
        status = send_fw_abort(fw_ptr, request_data.target, fw_download_abort_opcode, fw_status);
    }


done:
    return status;

}

static idigi_callback_status_t process_target_reset(idigi_firmware_data_t * const fw_ptr, uint8_t * const fw_message, uint16_t const length)
{
    idigi_callback_status_t status;
    idigi_fw_config_t request;

    UNUSED_PARAMETER(length);
    idigi_debug("Firmware Facility: process target reset\n");

    request.target = message_load_u8(fw_message, target);

    status = get_fw_config(fw_ptr, idigi_firmware_target_reset, &request, sizeof request, NULL, NULL, fw_equal);

    return status;
}

static void send_discovery_packet_callback(idigi_data_t * const idigi_ptr, uint8_t const * const packet,
                                           idigi_status_t const status, void * const user_data)
{
    idigi_firmware_data_t * const fw_ptr = user_data;
    /* update fw download keepalive timing */
    (void)get_system_time(idigi_ptr, &fw_ptr->last_fw_keepalive_sent_time);
    release_packet_buffer(idigi_ptr, packet, status, user_data);

}

static idigi_callback_status_t fw_discovery(idigi_data_t * const idigi_ptr, void * const facility_data,
                                            uint8_t * const packet, unsigned int * receive_timeout)
{
/* Firmware target list message format:
 *
 * --------------------------------------------------------
 * |   0    |    1   |  2 - 5  |  6 ...                    |
 *  -------------------------------------------------------
 * | opcode | target | version | Additional target-version |
 * |        |        |         |       pairs               |
 *  -------------------------------------------------------
 *
 */
enum fw_target_list_hdr {
    field_define(fw_target_list, opcode, uint8_t)
};

/* target + version pairs format: */
enum fw_target_list{
    field_define(fw_target_list, target, uint8_t),
    field_define(fw_target_list, version, uint32_t),
    record_end(fw_target_list)
};

    size_t const target_list_header_size = field_named_data(fw_target_list, opcode, size);
    size_t const target_list_size = record_bytes(fw_target_list);

    idigi_callback_status_t status = idigi_callback_continue;
    idigi_firmware_data_t * const fw_ptr = facility_data;

    UNUSED_PARAMETER(packet);
    UNUSED_PARAMETER(receive_timeout);

    /* Construct a target list message.
     * Get target count and then get version for each target to build target list message
     *
     */
    {
        unsigned int timeout;

        status = get_fw_config(fw_ptr, idigi_firmware_target_count, &timeout, sizeof timeout, &fw_ptr->target_count, NULL, fw_equal);
        if (status == idigi_callback_continue)
        {
            if (fw_ptr->target_count > 0)
            {
                uint16_t const buffer_size = sizeof idigi_ptr->send_packet.packet_buffer.buffer;
                uint16_t const overhead = (PACKET_EDP_FACILITY_SIZE + target_list_header_size);
                uint16_t const max_targets = (buffer_size - overhead) / target_list_size;

                /* get max count of targets that fit into the response buffer */
                if (fw_ptr->target_count > max_targets)
                {
                    idigi_request_t request_id;

                    request_id.firmware_request = idigi_firmware_target_count;
                    status = idigi_callback_abort;
                    notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_invalid_data_range);
                    goto done;
                }
            }
            else
            {
                idigi_debug("fw_discovery: No target supported\n");
                goto done;
            }
        }
        else
        {
            ASSERT(status == idigi_callback_abort);
            status = idigi_callback_abort;
        }
    }

    {

        uint8_t * edp_header;
        uint8_t * fw_target_list;
        size_t avail_length;
        size_t discovery_length = 0;
        uint16_t target_number;

        /* get packet pointer for constructing target list info */
        edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &fw_target_list, &avail_length);
        if (edp_header == NULL)
        {
            status = idigi_callback_busy;
            goto done;
        }

        message_store_u8(fw_target_list, opcode, fw_target_list_opcode);

        discovery_length = target_list_header_size;
        fw_target_list += discovery_length;

        for (target_number=0; target_number < fw_ptr->target_count; target_number++)
        {
            uint32_t version = 0;
            idigi_fw_config_t request;

            /* get the current firmware version for this target */
            request.target = (uint8_t)target_number;
            /* call callback */
            status = get_fw_config(fw_ptr, idigi_firmware_version, &request, sizeof request, &version, NULL, fw_equal);
            if (status == idigi_callback_continue)
            {

                message_store_u8(fw_target_list, target, request.target);
                message_store_be32(fw_target_list, version, version);

                /* set up for next target pair info*/
                fw_target_list += target_list_size;
                discovery_length += target_list_size;
            }
            else
            {
                ASSERT(status == idigi_callback_abort);
                status = idigi_callback_abort;
                goto error;
            }
        }

        status = initiate_send_facility_packet(idigi_ptr, edp_header, discovery_length,
                                               E_MSG_FAC_FW_NUM, send_discovery_packet_callback, fw_ptr);
error:
        if (status != idigi_callback_continue)
        {
            release_packet_buffer(idigi_ptr, edp_header, idigi_success, NULL);
        }
    }

done:
    return status;
}

static idigi_callback_status_t fw_process(idigi_data_t * const idigi_ptr, void * const facility_data,
                                          uint8_t * const edp_header, unsigned int * const receive_timeout)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_firmware_data_t * const fw_ptr = facility_data;
    uint8_t opcode;
    uint8_t target;
    uint8_t * fw_message;
    uint16_t length;

    if (edp_header == NULL)
    {
        goto done;
    }

    if (fw_ptr->fw_keepalive_start)
    {
        status = fw_discovery(idigi_ptr, facility_data, edp_header, receive_timeout);
        if (status == idigi_callback_continue)
        {
            (void)get_system_time(idigi_ptr, &fw_ptr->last_fw_keepalive_sent_time);
            fw_ptr->fw_keepalive_start = idigi_false;
            status = idigi_callback_busy;
        }
        goto done;
    }

    if (fw_ptr->send_busy == idigi_true)
    {
        /* callback is already called for this message.
         * We're here because we were unable to send a response
         * message which already message is constructed in
         * fw_ptr->response_buffer.
         */
        status = send_fw_message(fw_ptr);
        goto done;
    }

    length = message_load_be16(edp_header, length);
    if (length < FW_MESSAGE_HEADER_SIZE)
    {
        idigi_debug("fw_process: invalid packet size %d\n", length);
        goto done;
    }

    fw_message = GET_PACKET_DATA_POINTER(edp_header, PACKET_EDP_FACILITY_SIZE);
    opcode = message_load_u8(fw_message, opcode);
    target = message_load_u8(fw_message, target);

    if (target >= fw_ptr->target_count)
    {
        fw_abort_status_t  fw_status;

        idigi_debug("fw_process: invalid target\n");

        fw_status.error_status = fw_invalid_target;
        status = send_fw_abort(fw_ptr, target, fw_error_opcode, fw_status);
        goto done;
    }

    switch(opcode)
    {
    case fw_info_request_opcode:
        status = process_fw_info_request(fw_ptr, fw_message, length);
        break;
    case fw_download_request_opcode:
        status = process_fw_download_request(fw_ptr, fw_message, length);
        break;
    case fw_binary_block_opcode:
        status = process_fw_binary_block(fw_ptr, fw_message, length);
        break;
    case fw_download_abort_opcode:
        status = process_fw_abort(fw_ptr, fw_message, length);
        fw_ptr->last_fw_keepalive_sent_time = 0;
        fw_ptr->fw_keepalive_start = idigi_false;
        break;
    case fw_download_complete_opcode:
        if (fw_ptr->last_fw_keepalive_sent_time == 0)
        {
            /* start firmware keepalive which allow firmware download complete
             * callback to start flash (which requires device to
             * send target list message).
             *
             * Note. We only start firmware keepalive when we receive this complete
             * code. Can we start when we receive block opcode?
             */
            fw_ptr->last_fw_keepalive_sent_time = idigi_ptr->last_tx_keepalive_received_time;
        }
        status = process_fw_complete(fw_ptr, fw_message, length);
        break;
    case fw_target_reset_opcode:
        status = process_target_reset(fw_ptr, fw_message, length);
        fw_ptr->last_fw_keepalive_sent_time = 0;
        fw_ptr->fw_keepalive_start = idigi_false;
        break;
    default:
    {
        fw_abort_status_t  fw_status;
        fw_status.error_status = fw_invalid_opcode;
        status = send_fw_abort(fw_ptr, target, fw_error_opcode, fw_status);
        break;
    }
    }

done:
    return status;
}

static idigi_callback_status_t idigi_facility_firmware_delete(idigi_data_t * const idigi_ptr)
{
    return del_facility_data(idigi_ptr, E_MSG_FAC_FW_NUM);
}

static idigi_callback_status_t idigi_facility_firmware_init(idigi_data_t * const idigi_ptr, unsigned int const facility_index)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_firmware_data_t * fw_ptr;

    /* Add firmware access facility to iDigi
     *
     * Make sure firmware access facility is not already created. If firmware
     * access facility is already created, we probably reconnect to server
     * so just need to reset to initial state.
     *
     */
    fw_ptr = get_facility_data(idigi_ptr, E_MSG_FAC_FW_NUM);
    if (fw_ptr == NULL)
    {
        void * ptr;
        status = add_facility_data(idigi_ptr, facility_index, E_MSG_FAC_FW_NUM, &ptr, sizeof *fw_ptr);

        if (status != idigi_callback_continue || ptr == NULL)
        {
            goto done;
        }
        fw_ptr = ptr;
   }
    fw_ptr->target_count = 0;
    fw_ptr->target = 0;
    fw_ptr->desc_length = 0;
    fw_ptr->spec_length = 0;
    fw_ptr->last_fw_keepalive_sent_time = 0;
    fw_ptr->fw_keepalive_start = idigi_false;
    fw_ptr->send_busy = idigi_false;
    fw_ptr->update_started = idigi_false;
    fw_ptr->idigi_ptr = idigi_ptr;

done:
    return status;
}

