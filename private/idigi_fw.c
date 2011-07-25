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
#define FW_TARGET_LIST_MSG_INTERVAL_PER_SECOND     30

#define FW_NULL_ASCII       0x00
#define FW_NEW_LINE_ASCII   0x0A

/**
 * Firmware Upgrade Facility Opcodes
 * @doc These are the valid opcodes for the Firmware Upgrade Facility
 * All other values are invalid and reserved.
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

enum {
    fw_invalid_target,
    fw_invalid_opcode,
    fw_invalid_msg
};

typedef enum {
    fw_user_abort,
    fw_device_error,
    fw_invalid_offset,
    fw_invalid_data,
    fw_hardware_error
} fw_abort_status_t;

typedef enum {
    fw_equal,
    fw_less_than,
    fw_greater_than
} fw_compare_type_t;

typedef struct {
    idigi_firmware_request_t request_id;
    uint16_t target;
    uint16_t target_count;
    uint32_t ka_time;
    bool keepalive;
    idigi_data_t * idigi_ptr;
    uint32_t version;
    uint32_t code_size;
    char * description;
    char * name_spec;
    size_t desc_length;
    size_t spec_length;
    char filename[FW_ID_STRING_LENGTH];
    uint16_t discovery_length;
} idigi_firmware_data_t;


static idigi_callback_status_t fw_discovery(idigi_data_t *idigi_ptr, void * facility_data, uint8_t * packet);

static idigi_callback_status_t get_fw_config(idigi_firmware_data_t * fw_ptr, idigi_firmware_request_t request_id,
                                           void * request, size_t request_size,
                                           void * response, size_t * response_size, fw_compare_type_t compare_type)
{
    idigi_data_t * idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t   status;
    unsigned timeout;
    uint32_t start_time_stamp, end_time_stamp;
    uint32_t rx_keepalive;
    uint32_t tx_keepalive;
    unsigned * req_timeout;
    size_t  length;
    idigi_request_t rid;

    /* Calculate the timeout value (when to send rx keepalive or 
     * receive tx keepalive) from last rx keepalive or tx keepalive.
     *
     * If callback exceeds the timeout value, error status callback
     * will be called. 
     * Also, make sure the response size from the callback matches 
     * the given response_size.
     */
    status = get_system_time(idigi_ptr, &start_time_stamp);
    if (status != idigi_callback_continue)
    {
        goto done;
    }

    rx_keepalive = get_timeout_limit_in_seconds(*idigi_ptr->rx_keepalive, (start_time_stamp - idigi_ptr->rx_ka_time));
    tx_keepalive = get_timeout_limit_in_seconds(*idigi_ptr->tx_keepalive, (start_time_stamp - idigi_ptr->tx_ka_time));

    timeout = MIN_VALUE(rx_keepalive, tx_keepalive);

    if (fw_ptr->ka_time > 0)
    {
        unsigned ka_timeout;
        /* when download starts, we need to send target list on every
         * FW_TARGET_LIST_MSG_INTERVAL_PER_SECOND second. This ka_time is
         * set when we receive fw_download_complete_opcode to trigger
         * the keepalive time for sending target list message.
         *
         * see this is min timeout value to the callback.
         */
        ka_timeout = get_timeout_limit_in_seconds(FW_TARGET_LIST_MSG_INTERVAL_PER_SECOND, (start_time_stamp - fw_ptr->ka_time));
        timeout = MIN_VALUE( timeout, ka_timeout);
    }

    /* put the timeout value in the request pointer
     * (1st field is timeout field for all fw callback)
     */
    if (request != NULL)
    {
        req_timeout = (unsigned *)request;
        *req_timeout = timeout;
    }

    rid.firmware_request = request_id;
    status = idigi_callback(idigi_ptr->callback, idigi_class_firmware, rid, request, request_size, response, &length);
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


    if (status == idigi_callback_continue && response_size != NULL)
    {
        if ((compare_type == fw_equal && length != *response_size) ||
            (compare_type == fw_less_than && length > *response_size) ||
            (compare_type == fw_greater_than && length < *response_size))
        {
            /* bad data so let abort */
            DEBUG_PRINTF("get_fw_config: invalid returned size of data %d max length %d\n", (int)length, (int)*response_size);
            idigi_ptr->error_code = idigi_invalid_data_size;
            notify_error_status(idigi_ptr->callback, idigi_class_firmware, rid, idigi_invalid_data_size);
            status = idigi_callback_abort;
        }
    }

    if (response_size != NULL)
    {
        *response_size = length;
    }

    if ((end_time_stamp- start_time_stamp) > timeout)
    {
        /* callback exceeds timeout value.
         * No need to abort just notify caller.
         */
        DEBUG_PRINTF("get_fw_config: callback exceeds timeout > %d seconds\n", (int)timeout);
        notify_error_status(idigi_ptr->callback, idigi_class_firmware, rid, idigi_exceed_timeout);
    }

    if (status == idigi_callback_busy && fw_ptr->ka_time > 0)
    {
        /*
         * Check whether we need to send target list message
         * to keep server alive.
         */
        fw_ptr->keepalive = ((end_time_stamp - fw_ptr->ka_time) >= FW_TARGET_LIST_MSG_INTERVAL_PER_SECOND);
    }
    else
    {
        fw_ptr->keepalive = false;
    }

done:
    return status;
}



static fw_abort_status_t get_abort_status_code(idigi_fw_status_t status)
{
    fw_abort_status_t code = fw_user_abort;

    /* convert status to abort status for abort message */
    switch (status)
    {
    case idigi_fw_user_abort:
        code = fw_user_abort;
        break;
    case idigi_fw_device_error:
        code = fw_device_error;
        break;
    case idigi_fw_invalid_offset:
        code = fw_invalid_offset;
        break;
    case idigi_fw_invalid_data:
        code = fw_invalid_data;
        break;
    case idigi_fw_hardware_error:
        code = fw_hardware_error;
        break;
    case idigi_fw_success:
    case idigi_fw_download_denied:
    case idigi_fw_download_invalid_size:
    case idigi_fw_download_invalid_version:
    case idigi_fw_download_unauthenticated:
    case idigi_fw_download_not_allowed:
    case idigi_fw_download_configured_to_reject:
    case  idigi_fw_encountered_error:
        /* not abort status */
        ASSERT(false);
        break;

    }
    return code;
}

static idigi_callback_status_t send_fw_abort(idigi_data_t * idigi_ptr, uint8_t target, uint8_t msg_opcode, uint8_t status)
{
#define FW_SEND_ABORT_LENGTH    3

    uint8_t * ptr;
    uint8_t status_code = status;
    uint8_t * edp_header;

    /* send abort or error
     *  --------------------------
     * |   0    |    1   |    2   |
     *  --------------------------
     * | opcode | target | status |
     *  --------------------------
     *
     *  Firmware ID string: [descr]0xa[file name spec]
     */
    edp_header =get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &ptr);
    if (edp_header == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }
    /* need to adjust abort status code in the fw_status_t */
    if (msg_opcode != fw_error_opcode && (status < idigi_fw_user_abort || status > idigi_fw_hardware_error))
    {
        status_code = fw_device_error;
    }
    else if (status >= idigi_fw_user_abort)
    {
        status_code = get_abort_status_code(status);
    }
    /* now add this target to the target list message */
    *ptr++ = msg_opcode;
    *ptr++ = target;
    *ptr++ = status_code;  /* adjust the Firmware download abort status code */

    status = enable_facility_packet(idigi_ptr, edp_header, FW_SEND_ABORT_LENGTH, E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);

done:
    return status;

}

static idigi_callback_status_t  process_fw_info_request(idigi_firmware_data_t * fw_ptr, uint8_t * message, uint16_t length)
{
#define FW_INFO_REQUEST_ID_COUNT    4

    idigi_data_t * idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t target;
    idigi_firmware_request_t request_list[FW_INFO_REQUEST_ID_COUNT] = { idigi_firmware_version, idigi_firmware_code_size,
                                              idigi_firmware_description, idigi_firmware_name_spec};

    DEBUG_PRINTF("Firmware Facility: process info request\n");
    /* parse firmware info request
     *  -----------------
     * |   0    |    1   |
     *  -----------------
     * | opcode | target |
     *  -----------------
     */
    fw_ptr->ka_time = 0;
    fw_ptr->keepalive = false;

    /* let's get the request target number from the request packet */
    message++; /* skip opcode */
    length--;

    target = *message;

    /* let's get and build target info response */
    while (fw_ptr->request_id < FW_INFO_REQUEST_ID_COUNT)
    {

        switch (request_list[fw_ptr->request_id])
        {
        case idigi_firmware_version:
        {
            /* add target number to the target list message before version*/
            idigi_fw_config_t request;

            request.target = target;

            status = get_fw_config(fw_ptr, idigi_firmware_version, &request, sizeof request,&fw_ptr->version, NULL, fw_equal);
            break;
        }
        case idigi_firmware_code_size:
        {
            idigi_fw_config_t request;
            request.target = target;
            status = get_fw_config(fw_ptr, idigi_firmware_code_size, &request, sizeof request, &fw_ptr->code_size, NULL, fw_equal);
            break;
        }
        case idigi_firmware_description:
        case idigi_firmware_name_spec:
            if (request_list[fw_ptr->request_id] == idigi_firmware_description)
            {
                fw_ptr->desc_length = FW_ID_STRING_LENGTH-fw_ptr->spec_length-1;
                idigi_fw_config_t request;

                request.target = target;
                status = get_fw_config(fw_ptr, idigi_firmware_description, &request, sizeof request, (void *)&fw_ptr->description, &fw_ptr->desc_length, fw_less_than);
            }
            else
            {
                fw_ptr->spec_length = FW_ID_STRING_LENGTH-fw_ptr->desc_length-1;
                idigi_fw_config_t request;

                request.target = target;
                status = get_fw_config(fw_ptr, idigi_firmware_name_spec, &request, sizeof request, (void *)&fw_ptr->name_spec, &fw_ptr->spec_length, fw_less_than);
            }

            if (status == idigi_callback_continue && (fw_ptr->desc_length + fw_ptr->spec_length) > (FW_ID_STRING_LENGTH -1))
            {
                idigi_request_t request_id;
                request_id.firmware_request = idigi_firmware_description;
                idigi_ptr->error_code = idigi_invalid_data_size;
                DEBUG_PRINTF("process_fw_info_request: description length = %d + name spec length = %d\n",
                                        (int)fw_ptr->desc_length, (int)fw_ptr->spec_length);
                notify_error_status(idigi_ptr->callback, idigi_class_firmware, request_id, idigi_ptr->error_code);
                fw_ptr->desc_length = 0;
                fw_ptr->spec_length = 0;
                status = idigi_callback_abort;
            }
            break;
        case idigi_firmware_target_count:
        case idigi_firmware_download_request:
        case idigi_firmware_binary_block:
        case idigi_firmware_download_complete:
        case idigi_firmware_download_abort:
        case idigi_firmware_target_reset:
            /* handle in different functions */
            ASSERT(0);
            break;

        }
        if (status != idigi_callback_continue)
        {
            goto done;
        }
        fw_ptr->request_id++;
    }

    /* once it's done getting all firmware info,
     * let's build a response.
     * build and send firmware info response
     *  ---------------------------------------------------
     * |   0    |    1   |  2 - 5  |  6 - 9    |  10 ...   |
     *  ---------------------------------------------------
     * | opcode | target | version | Available | Firmware  |
     * |        |        |         | code size | ID string |
     *  ---------------------------------------------------
     *
     *  Firmware ID string = [descr]0xa[file name spec]
     */
    if (fw_ptr->request_id == FW_INFO_REQUEST_ID_COUNT)
    {
        uint8_t * edp_header;
        uint8_t * ptr;
        uint8_t * start_ptr;

        edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &ptr);
        if (edp_header == NULL)
        {
            status = idigi_callback_busy;
            goto done;
        }
        start_ptr = ptr;

        *ptr++ = fw_info_response_opcode;
        *ptr++ = target;  /* target number */


        StoreBE32(ptr, fw_ptr->version);
        ptr += sizeof fw_ptr->version;

        StoreBE32(ptr, fw_ptr->code_size);
        ptr += sizeof fw_ptr->code_size;

        if (fw_ptr->description != NULL)
        {
            memcpy((void *)ptr, (void *)fw_ptr->description, fw_ptr->desc_length);
            ptr += fw_ptr->desc_length;
        }
        *ptr++ = FW_NEW_LINE_ASCII;

        if (fw_ptr->name_spec != NULL)
        {
            memcpy((void *)ptr, (void *)fw_ptr->name_spec, fw_ptr->spec_length);
            ptr += fw_ptr->spec_length;
        }

        /* reset back to initial values */
        fw_ptr->version = 0;
        fw_ptr->code_size = 0;
        fw_ptr->desc_length = 0;
        fw_ptr->spec_length = 0;
        fw_ptr->request_id = idigi_firmware_target_count;
        status = enable_facility_packet(idigi_ptr, edp_header, (ptr-start_ptr), E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);
    }

done:
    return status;
}

static uint8_t * get_newline_terminated_pointer(uint8_t * ptr, uint16_t * length)
{
    uint16_t i;
    uint8_t * pointer = NULL;

    /* set the beginning pointer */
    pointer = ptr;

    /* parse until 0x0a */
    for (i=0; i < *length; i++)
    {
        if (ptr[i] == FW_NEW_LINE_ASCII || ptr[i] == FW_NULL_ASCII)
        {
            ptr[i] = FW_NULL_ASCII;
            i++;
            break;
        }
    }

    *length = i;
    return pointer;
}
static idigi_callback_status_t process_fw_download_request(idigi_firmware_data_t * fw_ptr, uint8_t * message, uint16_t length)
{

    idigi_data_t * idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status;
    uint32_t value;
    idigi_fw_download_request_t request_data;
    idigi_fw_status_t response_status = idigi_fw_user_abort;
    uint8_t   * packet;
    uint8_t * send_ptr;


    packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &send_ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }
    request_data.file_name_spec = NULL;
    request_data.filename = NULL;
    request_data.desc_string = NULL;

    /* Parse firmware download request:
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

    message++; /* skip opcode */
    length--;

    request_data.target = *message;
    length--;
    message++;

    value = *((uint32_t *)message);
    request_data.version = FROM_BE32(value);
    length -= sizeof value;
    message += sizeof value;

    value = *((uint32_t *)message);
    request_data.code_size = FROM_BE32(value);
    length -= sizeof value;
    message += sizeof value;

    {
        uint16_t len;

        /* parse firmware ID String for label, filename spec and
         * file name separated by 0x0a.
         */
        len = length;
        request_data.desc_string = (char *)get_newline_terminated_pointer(message, &len);
        message += len;
        length -= len;

        /* get filename spec */
        len = length;
        request_data.file_name_spec = (char *)get_newline_terminated_pointer(message, &len);
        message += len;
        length -= len;
    }

    /* get filename */
    {
        uint8_t * temp_ptr;
        uint16_t len;

        len = length;
        temp_ptr = get_newline_terminated_pointer(message, &len);
        message += len;
        length -= len;
        if (temp_ptr != NULL)
        {
            memcpy(fw_ptr->filename, temp_ptr, len);
            fw_ptr->filename[len]= FW_NULL_ASCII;
            request_data.filename = (char *)fw_ptr->filename;
        }
    }

    /* call callback */
    status = get_fw_config(fw_ptr, idigi_firmware_download_request, &request_data, sizeof request_data, &response_status, NULL, fw_equal);
    if (status != idigi_callback_busy)
    {
        uint8_t * start_ptr;

        if (response_status > idigi_fw_user_abort)
        {
            send_fw_abort(idigi_ptr, request_data.target, fw_download_abort_opcode, response_status);
            goto done;
        }

        /* send firmware download response
         *  ---------------------------------
         * |  0     |   1    |     2         |
         *  ---------------------------------
         * | opcode | target | response type |
         *  ---------------------------------
         *
         *  Firmware ID string: [descr]0xa[file name spec]
         */

        start_ptr = send_ptr;

        /* now add this target to the target list message */
        *send_ptr++ = fw_download_response_opcode;
        *send_ptr++ = request_data.target;  /* target number */
        *send_ptr++ = response_status;

        status = enable_facility_packet(idigi_ptr, packet, (send_ptr - start_ptr), E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);
    }
done:
    return status;
}


static idigi_callback_status_t process_fw_binary_block(idigi_firmware_data_t * fw_ptr, uint8_t * message, uint16_t length)
{
    idigi_data_t * idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status;
    uint32_t value;
    uint8_t ack_required;
    idigi_fw_image_data_t request_data;
    uint8_t * packet;
    uint8_t * ptr;
    idigi_fw_status_t response_status = idigi_fw_user_abort;


    packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    /* Parse firmware binary block
     *  --------------------------------------------------------
     * |   0    |   1    |     2        |  3 - 6  |   7..       |
     *  --------------------------------------------------------
     * | opcode | target | Ack required |  offset | binary data |
     *  --------------------------------------------------------
     *
     */

    message++; /* skip opcode */
    length--;

    request_data.target = *message++;
    length--;

    ack_required = *message++;
    length--;

    value = *((uint32_t *)message);
    request_data.offset = FROM_BE32(value);
    length -= sizeof value;
    message += sizeof value;

    request_data.data = message;
    request_data.length = length;

    /* call callback */
    status = get_fw_config(fw_ptr, idigi_firmware_binary_block, &request_data, sizeof request_data, &response_status, NULL, fw_equal);


    if (status == idigi_callback_continue && response_status == idigi_fw_success)
    {

        if(ack_required)
        {
            /* send firmware binary block acknowledge
             *  -----------------------------------
             * |   0    |    1   | 2 - 5  |    6   |
             *  -----------------------------------
             * | opcode | target | offset | status |
             *  -----------------------------------
             *
             *  Firmware ID string: [descr]0xa[file name spec]
             */
            uint8_t * start_ptr;

            start_ptr = ptr;

            /* now add this target to the target list message */
            *ptr++ = fw_binary_block_ack_opcode;
            *ptr++ = request_data.target;  /* target number */

            StoreBE32(ptr, request_data.offset);
            ptr += sizeof request_data.offset;

            *ptr++ = idigi_fw_success;

            status = enable_facility_packet(idigi_ptr, packet, (ptr - start_ptr), E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);
        }
    }
    else if (status != idigi_callback_busy)
    {
        send_fw_abort(idigi_ptr, request_data.target,fw_download_abort_opcode, response_status);
    }
done:
    return status;
}

static idigi_callback_status_t process_fw_abort(idigi_firmware_data_t * fw_ptr, uint8_t * message, uint16_t length)
{
    idigi_callback_status_t status;
    idigi_fw_download_abort_t request_data;

    /* parse firmware download abort
     *  --------------------------
     * |   0    |   1    |   2    |
     *  --------------------------
     * | opcode | target | status |
     *  --------------------------
     *
     */
    message++;  /* skip opcode */
    length--;

    request_data.target = *message++;
    length--;

    request_data.status = *message++;
    length--;

    /* call callback */
    status = get_fw_config(fw_ptr, idigi_firmware_download_abort, &request_data, sizeof request_data, NULL, NULL, fw_equal);
    fw_ptr->ka_time = 0;
    return status;

}
static idigi_callback_status_t process_fw_complete(idigi_firmware_data_t * fw_ptr, uint8_t * message, uint16_t length)
{
    idigi_data_t * idigi_ptr = fw_ptr->idigi_ptr;
    idigi_callback_status_t status;
    uint32_t value;
    idigi_fw_download_complete_request_t request_data;
    idigi_fw_download_complete_response_t response_data;
    uint8_t * packet;
    uint8_t * ptr;

    packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &ptr);
    if (packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    /* Parse firmware downlaod complete
     *  ----------------------------------------
     * |   0    |   1    |   2 - 5   |  6 - 9   |
     *  ----------------------------------------
     * | opcode | target | code size | checksum |
     *  ----------------------------------------
     *
     */

    message++;  /* skip opcode */
    length--;

    request_data.target = *message++;
    length--;

    value = *((uint32_t *)message);
    request_data.code_size = FROM_BE32(value);
    length -= sizeof value;
    message += sizeof value;

    value = *((uint32_t *)message);
    request_data.checksum = FROM_BE32(value);
    length -= sizeof value;
    message += sizeof value;

    /* call callback */
    status = get_fw_config(fw_ptr, idigi_firmware_download_complete, &request_data, sizeof request_data, &response_data, NULL, fw_equal);
    if (status == idigi_callback_continue)
    {
        /* send firmware download complete response
         *  -------------------------------------------------
         * |   0    |   1    |  2 - 5  |  6 - 9     |  10    |
         *  --------------------------------------------------
         * | opcode | target | version | calculated | status |
         * |        |        |         |  checksum  |        |
         *  --------------------------------------------------
         *
         *  Firmware ID string: [descr]0xa[file name spec]
         */
        uint8_t * start_ptr;

        start_ptr = ptr;

        /* now add this target to the target list message */
        *ptr++ = fw_download_complete_response_opcode;
        *ptr++ = request_data.target;  /* target number */
        StoreBE32(ptr, response_data.version);
        ptr += sizeof response_data.version;


        StoreBE32(ptr, response_data.calculated_checksum);
        ptr += sizeof response_data.calculated_checksum;

        *ptr++ = response_data.status;

        fw_ptr->ka_time = 0;
        fw_ptr->keepalive = false;

        status = enable_facility_packet(idigi_ptr, packet, (ptr-start_ptr), E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);
    }
    else if (status == idigi_callback_abort)
    {
        send_fw_abort(idigi_ptr, request_data.target,fw_download_abort_opcode, response_data.status);
    }

done:
    return status;

}

static idigi_callback_status_t process_target_reset(idigi_firmware_data_t * fw_ptr, uint8_t * message, uint16_t length)
{
    idigi_callback_status_t status;
    idigi_fw_config_t request;

    UNUSED_PARAMETER(length);
    /* Parse firmware target reset
     *  -----------------
     * |   0    |   1    |
     *  -----------------
     * | opcode | target |
     *  -----------------
     *
     */
    message++; /* skip opcode */

    request.target = *message;

    status = get_fw_config(fw_ptr, idigi_firmware_target_reset, &request, sizeof request, NULL, NULL, fw_equal);

    return status;
}

static idigi_callback_status_t fw_process(idigi_data_t * idigi_ptr, void * facility_data, uint8_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t opcode, target;
    uint8_t * ptr;
    idigi_firmware_data_t * fw_ptr = facility_data;
    uint16_t length;
    uint8_t * edp_header = packet;


    if (fw_ptr->keepalive)
    {
        /* time to send target-list message during downloading to avoid 
         * disconnecting by server.
         * After sending, update the timing for next send.
         */
        status = fw_discovery(idigi_ptr, facility_data, packet);
        if (status != idigi_callback_abort)
        {
            status = get_system_time(idigi_ptr, &fw_ptr->ka_time);
            if (status == idigi_callback_abort)
            {
                goto done;
            }
            else 
            {
                fw_ptr->keepalive = false;
            }
        }
        else
        {
            goto done;
        }
    }

    if (packet == NULL)
    {
        DEBUG_PRINTF("fw_process: No Packet\n");
        goto done;
    }
    ptr = GET_PACKET_DATA_POINTER(packet, PACKET_EDP_FACILITY_SIZE);
    opcode = *ptr;
    target = *(ptr+1);
    if (target >= fw_ptr->target_count)
    {
        DEBUG_PRINTF("fw_process: invalid target\n");
        status = send_fw_abort(idigi_ptr, target, fw_error_opcode, fw_invalid_target);
        goto done;
    }

    length = message_load_be16(edp_header, length);

    if (opcode == fw_download_complete_opcode)
    {
        if (fw_ptr->ka_time == 0)
        {
            /* start firmware keepalive (which requires device to
             * send target list message).
             */
            fw_ptr->ka_time = idigi_ptr->tx_ka_time;
        }
    }


    switch(opcode)
    {
    case fw_info_request_opcode:
        status = process_fw_info_request(fw_ptr, ptr, length);
        break;
    case fw_download_request_opcode:
        status = process_fw_download_request(fw_ptr, ptr, length);
        break;
    case fw_binary_block_opcode:
        status = process_fw_binary_block(fw_ptr, ptr, length);
        break;
    case fw_download_abort_opcode:
        status = process_fw_abort(fw_ptr, ptr, length);
        fw_ptr->ka_time = 0;
        fw_ptr->keepalive = false;
        break;
    case fw_download_complete_opcode:
        status = process_fw_complete(fw_ptr, ptr, length);
        break;
    case fw_target_reset_opcode:
        status = process_target_reset(fw_ptr, ptr, length);
        fw_ptr->ka_time = 0;
        fw_ptr->keepalive = false;
        break;
    default:
        status = send_fw_abort(idigi_ptr, target, fw_error_opcode, fw_invalid_opcode);
        break;
    }

done:
    return status;
}

static idigi_callback_status_t fw_discovery(idigi_data_t *idigi_ptr, void * facility_data, uint8_t * packet)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t * send_packet;
    uint8_t * ptr;
    idigi_firmware_data_t * fw_ptr = facility_data;

    UNUSED_PARAMETER(packet);
    /* Firmware Access facility needs to send target list info during initialization phase at discovery layer */

    /* get packet pointer for constructing target list info */
    send_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &ptr);
    if (send_packet == NULL)
    {
        status = idigi_callback_busy;
        goto done;
    }

    /* Construct a target list message.
     * Get target count and then get version for each target to build target list message
     *
     * --------------------------------------------------------
     * |   0    |    1   |  2 - 5  |  6 ...                    |
     *  -------------------------------------------------------
     * | opcode | target | version | Additional target-version |
     * |        |        |         |       pairs               |
     *  -------------------------------------------------------
     *
     */
    if (fw_ptr->request_id == idigi_firmware_target_count)
    {
        unsigned timeout;

        status = get_fw_config(fw_ptr, idigi_firmware_target_count, &timeout, sizeof timeout, &fw_ptr->target_count, NULL, fw_equal);
        if (status == idigi_callback_continue)
        {
            ASSERT_GOTO(fw_ptr->target_count > 0, done);

            *ptr++ = fw_target_list_opcode;
            fw_ptr->discovery_length= 1;
            fw_ptr->target = 0;
            fw_ptr->request_id = idigi_firmware_version;
        }
    }

    if (fw_ptr->request_id == idigi_firmware_version)
    {
        ptr += fw_ptr->discovery_length;

        while (fw_ptr->target < fw_ptr->target_count)
        {
            uint32_t version;
            idigi_fw_config_t request;

            /* get the current firmware version for this target */
            request.target = fw_ptr->target;
            /* call callback */
            status = get_fw_config(fw_ptr, idigi_firmware_version, &request, sizeof request, &version, NULL, fw_equal);
            if (status == idigi_callback_continue)
            {

                *ptr = fw_ptr->target;  /* target number */
                ptr++;
                fw_ptr->discovery_length++;
                StoreBE32(ptr, version);
                ptr += sizeof version;
                fw_ptr->discovery_length += sizeof version;
                fw_ptr->target++;
            }
        }

        if (fw_ptr->target == fw_ptr->target_count)
        {
            status = enable_facility_packet(idigi_ptr, send_packet, fw_ptr->discovery_length,  E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);
            fw_ptr->request_id = idigi_firmware_target_count;
            fw_ptr->discovery_length = 0;
            fw_ptr->target = 0;
        }

    }


done:
    return status;
}

static idigi_callback_status_t fw_delete_facility(idigi_data_t * idigi_ptr)
{
    return del_facility_data(idigi_ptr, E_MSG_FAC_FW_NUM);
}

static idigi_callback_status_t fw_init_facility(idigi_data_t *idigi_ptr)
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
        status = add_facility_data(idigi_ptr, E_MSG_FAC_FW_NUM, &ptr,
                                   sizeof(idigi_firmware_data_t), fw_discovery, fw_process);

        if (status != idigi_callback_continue || ptr == NULL)
        {
            goto done;
        }
        fw_ptr = (idigi_firmware_data_t *)ptr;
   }
    fw_ptr->request_id = idigi_firmware_target_count;
    fw_ptr->target_count = 0;
    fw_ptr->target = 0;
    fw_ptr->ka_time = 0;
    fw_ptr->keepalive = false;
    fw_ptr->idigi_ptr = idigi_ptr;

done:
    return status;
}

