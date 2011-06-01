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

#define IDK_FA_ID_STRING_LENGTH     128  /* bytes */

/* time to send target list to keep download alive */
#define IDK_FW_TARGET_LIST_MSG_INTERVAL     (30 * IDK_MILLISECONDS)

#define IDK_FW_INFO_REQUEST_OPCODE          0x01
#define IDK_FW_INFO_RESPONSE_OPCODE         0x02
#define IDK_FW_DOWNLOAD_REQUEST_OPCODE      0x03
#define IDK_FW_DOWNLOAD_RESPONSE_OPCODE     0x04
#define IDK_FW_BINARY_BLOCK_OPCODE          0x05
#define IDK_FW_BINARY_BLOCK_ACK_OPCODE      0x06
#define IDK_FW_DOWNLOAD_ABORT_OPCODE        0x07
#define IDK_FW_DOWNLOAD_COMPLETE_OPCODE     0x08
#define IDK_FW_DOWNLOAD_COMPLETE_RESPONSE_OPCODE    0x09
#define IDK_FW_TARGET_RESET_OPCODE                  0x0a
#define IDK_FW_ERROR_OPCODE                 0x0c


enum {
    idk_fw_invalid_target,
    idk_fw_invalid_opcode,
    idk_fw_invalid_msg
};
/**
 * Firmware Upgrade Facility Opcodes
 * @doc These are the valid opcodes for the Firmware Upgrade Facility
 * All other values are invalid and reserved.
 */

/* Target List Message */
#define E_FAC_FU_OP_LIST_TARGETS 0x00
/* Firmware Info Request */
#define E_FAC_FU_OP_INFO_REQ 0x01
/* Firmware Info Response */
#define E_FAC_FU_OP_INFO_RESP 0x02
/* Firmware Download Request */
#define E_FAC_FU_OP_DNLD_REQ 0x03
/* Firmware Download Response */
#define E_FAC_FU_OP_DNLD_RESP 0x04
/* Firmware Code Block */
#define E_FAC_FU_OP_CODE_BLK 0x05
/* Firmware Binary Block Acknowledge */
#define E_FAC_FU_OP_CODE_BLK_ACK 0x06
/* Firmware Download Abort */
#define E_FAC_FU_OP_DNLD_ABORT 0x07
/* Firmware Download Complete */
#define E_FAC_FU_OP_DNLD_CMPLT 0x08
/* Firmware Download Complete Response */
#define E_FAC_FU_OP_DNLD_CMPLT_RESP 0x09
/* Request Target Reset */
#define E_FAC_FU_OP_TARGET_RESET 0x0a
/* Firmware Download Status */
#define E_FAC_FU_OP_DNLD_STATUS 0x0b
/* Error, Target Invalid */
#define E_FAC_FU_OP_TARGET_INVALID 0x0c

typedef enum {
    fw_equal,
    fw_less_than,
    fw_greater_than
} fw_compare_type_t;


typedef struct {

    idk_facility_t  facility;

    idk_firmware_request_t request_id;
    uint16_t target;
    uint16_t target_count;
    uint32_t ka_time;
    bool keepalive;
    idk_data_t * idk_ptr;
    uint32_t version;
    uint32_t code_size;
    char * description;
    char * name_spec;
    size_t desc_length;
    size_t spec_length;
    char filename[IDK_FA_ID_STRING_LENGTH];


} idk_firmware_data_t;


static idk_callback_status_t fw_discovery(idk_data_t *idk_ptr, idk_facility_t * fac_ptr);


static idk_callback_status_t get_fw_config(idk_firmware_data_t * fw_ptr, idk_firmware_request_t request_id,
                                           void * request, size_t request_size,
                                           void * response, size_t * response_size, fw_compare_type_t compare_type)
{
    idk_data_t * idk_ptr = fw_ptr->idk_ptr;
    idk_callback_status_t   status;
    unsigned timeout;
    uint32_t time_stamp, time_stamp1;
    uint32_t rx_keepalive;
    uint32_t tx_keepalive;
    unsigned * req_timeout;
    size_t  length;
    idk_request_t rid;

    /* Calculate the timeout value (when to send rx keepalive or 
     * receive tx keepalive) from last rx keepalive or tx keepalive.
     *
     * If callback exceeds the timeout value, error status callback
     * will be called. 
     * Also, make sure the response size from the callback matches 
     * the given response_size.
     */
    status = get_system_time(idk_ptr, &time_stamp);
    if (status != idk_callback_continue)
    {
        goto done;
    }

    rx_keepalive = (*idk_ptr->rx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->rx_ka_time);
    tx_keepalive = (*idk_ptr->tx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->tx_ka_time);

    timeout = IDK_MIN( rx_keepalive, tx_keepalive);

    if (fw_ptr->ka_time > 0)
    {
        /* when download starts, we need to send target list on every
         * IDK_FW_TARGET_LIST_MSG_INTERVAL second. This ka_time is
         * set when we receive IDK_FW_DOWNLOAD_COMPLETE_OPCODE to trigger
         * the keepalive time for sending target list message.
         *
         * see this is min timeout value to the callback.
         */
        timeout = IDK_MIN( timeout, IDK_FW_TARGET_LIST_MSG_INTERVAL - (time_stamp - fw_ptr->ka_time));

    }

    /* put the timeout value in the request pointer
     * (1st field is timeout field for all fw callback)
     */
    if (request != NULL)
    {
        req_timeout = (unsigned *)request;
        *req_timeout = timeout/IDK_MILLISECONDS;
    }

    rid.firmware_request = request_id;
    status = idk_callback(idk_ptr->callback, idk_class_firmware, rid, request, request_size, response, &length);
    if (status == idk_callback_abort)
    {
        idk_ptr->error_code = idk_configuration_error;
        goto done;
    }

    if (get_system_time(idk_ptr, &time_stamp1) != idk_callback_continue)
    {
        idk_ptr->error_code = idk_configuration_error;
        status = idk_callback_abort;
        goto done;
    }


    if (status == idk_callback_continue && response_size != NULL)
    {
        if ((compare_type == fw_equal && length != *response_size) ||
            (compare_type == fw_less_than && length > *response_size) ||
            (compare_type == fw_greater_than && length < *response_size))
        {

            DEBUG_PRINTF("get_fw_config: invalid returned size of data %d max length %d\n", (int)length, (int)*response_size);
            idk_ptr->error_code = idk_invalid_data_size;
            status = notify_error_status(idk_ptr->callback, idk_class_firmware, rid, idk_invalid_data_size);
            if (status == idk_callback_abort)
            {
                goto done;
            }
            /* come back here to correct idk_invalid_data_size */
            status = idk_callback_busy;
        }
    }

    if (response_size != NULL)
    {
        *response_size = length;
    }

    if ((time_stamp1- time_stamp) > timeout)
    {
        /* callback exceeds timeout value */
        DEBUG_PRINTF("get_fw_config: callback exceeds timeout (%d - %d) > %d\n", (int)time_stamp1, (int)time_stamp, (int)timeout);
        status = notify_error_status(idk_ptr->callback, idk_class_firmware, rid, idk_exceed_timeout);
        if (status == idk_callback_abort)
        {
            goto done;
        }
        status = idk_callback_busy;
    }

    if (status == idk_callback_busy && fw_ptr->ka_time > 0)
    {
        /*
         * Check whether we need to send target list message
         * to keep server alive.
         */
        fw_ptr->keepalive = ((time_stamp1 - fw_ptr->ka_time) >= IDK_FW_TARGET_LIST_MSG_INTERVAL);
    }
    else
    {
        fw_ptr->keepalive = false;
    }

done:
    return status;
}


static idk_callback_status_t get_fw_target_count(idk_firmware_data_t * fw_ptr,  uint16_t * count)
{
    idk_callback_status_t status;
    unsigned timeout;


    status = get_fw_config(fw_ptr, idk_firmware_target_count, &timeout, sizeof timeout, count, NULL, fw_equal);


    if (status == idk_callback_continue)
    {
        DEBUG_PRINTF("get_fw_target_count: target count = %d\n", *count);
    }

    return status;
}

static idk_callback_status_t get_fw_version(idk_firmware_data_t * fw_ptr, uint8_t target, uint32_t * version)
{
    idk_callback_status_t status;
    idk_fw_config_t request;

    request.target = target;
    
   status = get_fw_config(fw_ptr, idk_firmware_version, &request, sizeof request, version, NULL, fw_equal);
    return status;
}

static idk_callback_status_t get_fw_code_size(idk_firmware_data_t * fw_ptr, uint8_t target, uint32_t * code_size)
{
    idk_callback_status_t status;
    idk_fw_config_t request;

    request.target = target;

    status = get_fw_config(fw_ptr, idk_firmware_code_size, &request, sizeof request, code_size, NULL, fw_equal);

    return status;
}

static idk_callback_status_t get_fw_description(idk_firmware_data_t * fw_ptr, uint8_t target, char ** desc, size_t * length)
{
    idk_callback_status_t status;
    idk_fw_config_t request;

    request.target = target;
    status = get_fw_config(fw_ptr, idk_firmware_description, &request, sizeof request, (void *)desc, length, fw_less_than);

    return status;
}

static idk_callback_status_t get_fw_name_spec(idk_firmware_data_t * fw_ptr,  uint8_t target, char ** spec, size_t * length)
{
    idk_callback_status_t status;
    idk_fw_config_t request;

    request.target = target;
    status = get_fw_config(fw_ptr, idk_firmware_name_spec, &request, sizeof request, (void *)spec, length, fw_less_than);

    return status;
}

#define IDK_FW_ABORT_STATUS_CODE(x) (x-idk_fw_user_abort)

static idk_callback_status_t send_fw_abort(idk_data_t * idk_ptr, uint8_t target, uint8_t msg_opcode, uint8_t status)
{
#define FW_SEND_ABORT_LENGTH    3

    uint8_t * ptr;
    uint8_t status_code = status;
    idk_facility_packet_t   * p;

    /* send abort or error
     *  --------------------------
     * |   0    |    1   |    2   |
     *  --------------------------
     * | opcode | target | status |
     *  --------------------------
     *
     *  Firmware ID string: [descr]0xa[file name spec]
     */
    p =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
    if (p == NULL)
    {
        status = idk_callback_busy;
        goto done;
    }
    /* need to adjust abort status code in the fw_status_t */
    if (msg_opcode != IDK_FW_ERROR_OPCODE && (status < idk_fw_user_abort || status > idk_fw_hardware_error))
    {
        status_code = IDK_FW_ABORT_STATUS_CODE(idk_fw_device_error);
    }
    else if (status >= idk_fw_user_abort)
    {
        status_code = IDK_FW_ABORT_STATUS_CODE(status);
    }
    /* now add this target to the target list message */
    *ptr++ = msg_opcode;
    *ptr++ = target;
    *ptr++ = status_code;  /* adjust the Firmware download abort status code */
    p->length = FW_SEND_ABORT_LENGTH;


    status = enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);

done:
    return status;

}



static idk_callback_status_t  process_fw_info_request(idk_firmware_data_t * fw_ptr, idk_facility_packet_t * packet)
{
#define FW_INFO_REQUEST_ID_COUNT    4

    idk_data_t * idk_ptr = fw_ptr->idk_ptr;
    idk_callback_status_t status = idk_callback_continue;
    uint8_t target;
    uint8_t * ptr, * data_ptr;
    idk_facility_packet_t * send_packet;
    idk_firmware_request_t request_list[FW_INFO_REQUEST_ID_COUNT] = { idk_firmware_version, idk_firmware_code_size,
                                              idk_firmware_description, idk_firmware_name_spec};

    /* parse firmware info request
     *  -----------------
     * |   0    |    1   |
     *  -----------------
     * | opcode | target |
     *  -----------------
     *
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
    fw_ptr->ka_time = 0;
    fw_ptr->keepalive = false;

    /* let's get the request target number from the request packet */
    ptr = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    ptr++; /* skip the opcode */
    target = *ptr;

    /* let's get and build target info response */
    while (fw_ptr->request_id < FW_INFO_REQUEST_ID_COUNT)
    {

        switch (request_list[fw_ptr->request_id])
        {
        case idk_firmware_version:
            /* add target number to the target list message before version*/
            status = get_fw_version(fw_ptr, target, &fw_ptr->version);
            break;

        case idk_firmware_code_size:
            status = get_fw_code_size(fw_ptr, target, &fw_ptr->code_size);
            break;

        case idk_firmware_description:
        case idk_firmware_name_spec:
            if (request_list[fw_ptr->request_id] == idk_firmware_description)
            {
                fw_ptr->desc_length = IDK_FA_ID_STRING_LENGTH-fw_ptr->spec_length-1;
                status = get_fw_description(fw_ptr, target, (char **)&fw_ptr->description, &fw_ptr->desc_length);
            }
            else
            {
                fw_ptr->spec_length = IDK_FA_ID_STRING_LENGTH-fw_ptr->desc_length-1;
                status = get_fw_name_spec(fw_ptr, target, (char **)&fw_ptr->name_spec, &fw_ptr->spec_length);
            }

            if (status == idk_callback_continue && (fw_ptr->desc_length + fw_ptr->spec_length) > (IDK_FA_ID_STRING_LENGTH -1))
            {
                idk_request_t request_id;
                request_id.firmware_request = idk_firmware_description;
                idk_ptr->error_code = idk_invalid_data_size;
                DEBUG_PRINTF("process_fw_info_request: description length = %d + name spec length = %d\n",
                                        (int)fw_ptr->desc_length, (int)fw_ptr->spec_length);
                status = notify_error_status(idk_ptr->callback, idk_class_firmware, request_id, idk_invalid_data_size);
                if (status != idk_callback_abort)
                {
                    fw_ptr->desc_length = 0;
                    fw_ptr->spec_length = 0;
                    status = idk_callback_busy;
                    goto done;
                }
            }
            break;
        case idk_firmware_target_count:
        case idk_firmware_download_request:
        case idk_firmware_binary_block:
        case idk_firmware_download_complete:
        case idk_firmware_download_abort:
        case idk_firmware_target_reset:
            /* handle in different functions */
            break;

        }
        if (status != idk_callback_continue)
        {
            goto done;
        }
        fw_ptr->request_id++;
    }


    if (fw_ptr->request_id == FW_INFO_REQUEST_ID_COUNT)
    {
        send_packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
        if (send_packet == NULL)
        {
            status = idk_callback_busy;
            goto done;
        }
        data_ptr = ptr;

        *ptr++ = IDK_FW_INFO_RESPONSE_OPCODE;
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
        *ptr++ = 0x0a;

        if (fw_ptr->name_spec != NULL)
        {
            memcpy((void *)ptr, (void *)fw_ptr->name_spec, fw_ptr->spec_length);
            ptr += fw_ptr->spec_length;
        }

        send_packet->length = (uint16_t)(ptr - data_ptr);
        /* reset back to initial values */
        fw_ptr->version = 0;
        fw_ptr->code_size = 0;
        fw_ptr->desc_length = 0;
        fw_ptr->spec_length = 0;
        fw_ptr->request_id = idk_firmware_target_count;
        status = enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
    }

done:
    return status;
}

static idk_callback_status_t process_fw_download_request(idk_firmware_data_t * fw_ptr, idk_facility_packet_t * packet)
{
#define FW_NULL_ASCII       0x00
#define FW_NEW_LINE_ASCII   0x0A

    idk_data_t * idk_ptr = fw_ptr->idk_ptr;
    idk_callback_status_t status;
    uint32_t value;
    idk_fw_download_request_t request_data;
    idk_fw_status_t response_status = idk_fw_user_abort;
    idk_facility_packet_t   * send_packet;
    int i;
    uint8_t * buf;
    uint16_t length;
    uint8_t * send_ptr, * data_ptr;


    send_packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &send_ptr);
    if (send_packet == NULL)
    {
        status = idk_callback_busy;
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

    /* get the requested target number from the request packet */
    buf = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    buf++; /* skip opcode */
    length = packet->length-1;

    request_data.target = *buf;
    length--;
    buf++;

    value = *((uint32_t *)buf);
    request_data.version = FROM_BE32(value);
    length -= sizeof value;
    buf += sizeof value;

    value = *((uint32_t *)buf);
    request_data.code_size = FROM_BE32(value);
    length -= sizeof value;
    buf += sizeof value;

    if (*buf != FW_NEW_LINE_ASCII && *buf != FW_NULL_ASCII)
    {
        request_data.desc_string = (char *)buf;
    }

    for (i=0; i < packet->length; i++)
    {
        if (buf[i] == FW_NEW_LINE_ASCII || buf[i] == FW_NULL_ASCII)
        {
            buf[i] = FW_NULL_ASCII;
            break;
        }
    }
    length -= (i+1);
    buf += (i+1);


    if (*buf != FW_NEW_LINE_ASCII && * buf != FW_NULL_ASCII)
    {
        request_data.file_name_spec = (char *)buf;
    }
    for (i=0; i < packet->length; i++)
    {
        if (buf[i] == FW_NEW_LINE_ASCII || buf[i] == FW_NULL_ASCII)
        {
            buf[i] = FW_NULL_ASCII;
            break;
        }
    }
    length -= (i+1);
    buf += (i+1);

    fw_ptr->filename[0] = FW_NULL_ASCII;
    if (*buf != FW_NEW_LINE_ASCII && *buf != FW_NULL_ASCII)
    {
        memcpy(fw_ptr->filename, buf, length);
        fw_ptr->filename[length]= FW_NULL_ASCII;
        request_data.filename = (char *)fw_ptr->filename;
    }

    status = get_fw_config(fw_ptr, idk_firmware_download_request, &request_data, sizeof request_data, &response_status, NULL, fw_equal);


    if (status != idk_callback_busy)
    {

        if (response_status > idk_fw_user_abort)
        {
            send_fw_abort(idk_ptr, request_data.target, IDK_FW_DOWNLOAD_ABORT_OPCODE, response_status);
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

        data_ptr = send_ptr;

        /* now add this target to the target list message */
        *send_ptr++ = IDK_FW_DOWNLOAD_RESPONSE_OPCODE;
        *send_ptr++ = request_data.target;  /* target number */
        *send_ptr++ = response_status;

        send_packet->length = send_ptr - data_ptr;
        status = enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
    }
done:
    return status;
}


static idk_callback_status_t process_fw_binary_block(idk_firmware_data_t * fw_ptr, idk_facility_packet_t * packet)
{
    idk_data_t * idk_ptr = fw_ptr->idk_ptr;
    idk_callback_status_t status;
    uint32_t value;
    uint8_t ack_required;
    idk_fw_image_data_t request_data;
    idk_facility_packet_t * send_packet;
    uint8_t * buf;
    uint16_t length;
    uint8_t * ptr, * data_ptr;
    idk_fw_status_t response_status = idk_fw_user_abort;


    send_packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
    if (send_packet == NULL)
    {
        status = idk_callback_busy;
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

    buf = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    buf++; /* skip opcode */

    length = packet->length-1;

    request_data.target = *buf;
    length--;
    buf++;

    ack_required = *buf;
    length--;
    buf++;

    value = *((uint32_t *)buf);
    request_data.offset = FROM_BE32(value);
    length -= sizeof value;
    buf += sizeof value;

    request_data.data = buf;
    request_data.length = length;

    status = get_fw_config(fw_ptr, idk_firmware_binary_block, &request_data, sizeof request_data, &response_status, NULL, fw_equal);


    if (status == idk_callback_continue && response_status == idk_fw_success)
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
             data_ptr = ptr;

            /* now add this target to the target list message */
            *ptr++ = IDK_FW_BINARY_BLOCK_ACK_OPCODE;
            *ptr++ = request_data.target;  /* target number */

            StoreBE32(ptr, request_data.offset);
            ptr += sizeof request_data.offset;

            *ptr++ = idk_fw_success;

            send_packet->length = ptr - data_ptr;

            status = enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
        }
    }
    else if (status != idk_callback_busy)
    {
        send_fw_abort(idk_ptr, request_data.target,IDK_FW_DOWNLOAD_ABORT_OPCODE, response_status);
    }
done:
    return status;
}

static idk_callback_status_t process_fw_abort(idk_firmware_data_t * fw_ptr, idk_facility_packet_t * packet)
{
    idk_callback_status_t status;
    idk_fw_download_abort_t request_data;
    uint8_t * buf;
    uint16_t length;

    /* parse firmware download abort
     *  --------------------------
     * |   0    |   1    |   2    |
     *  --------------------------
     * | opcode | target | status |
     *  --------------------------
     *
     */
    buf = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    buf++;
    length = packet->length-1;

    request_data.target = *buf;
    length--;
    buf++;

    request_data.status = *buf;
    length--;
    buf++;

    status = get_fw_config(fw_ptr, idk_firmware_download_abort, &request_data, sizeof request_data, NULL, NULL, fw_equal);
    fw_ptr->ka_time = 0;
    return status;

}
static idk_callback_status_t process_fw_complete(idk_firmware_data_t * fw_ptr, idk_facility_packet_t * packet)
{
    idk_data_t * idk_ptr = fw_ptr->idk_ptr;
    idk_callback_status_t status;
    uint32_t value;
    idk_fw_download_complete_request_t request_data;
    idk_fw_download_complete_response_t response_data;
    idk_facility_packet_t * send_packet;
    uint8_t * buf;
    uint16_t length;
    uint8_t * ptr, * data_ptr;

    send_packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
    if (send_packet == NULL)
    {
        status = idk_callback_busy;
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

    buf = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    buf++;
    length = packet->length-1;

    request_data.target = *buf;
    length--;
    buf++;

    value = *((uint32_t *)buf);
    request_data.code_size = FROM_BE32(value);
    length -= sizeof value;
    buf += sizeof value;

    value = *((uint32_t *)buf);
    request_data.checksum = FROM_BE32(value);
    length -= sizeof value;
    buf += sizeof value;

    length = sizeof response_data;
    status = get_fw_config(fw_ptr, idk_firmware_download_complete, &request_data, sizeof request_data, &response_data, NULL, fw_equal);
    if (status == idk_callback_continue)
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
        data_ptr = ptr;

        /* now add this target to the target list message */
        *ptr++ = IDK_FW_DOWNLOAD_COMPLETE_RESPONSE_OPCODE;
        *ptr++ = request_data.target;  /* target number */
        StoreBE32(ptr, response_data.version);
        ptr += sizeof response_data.version;


        StoreBE32(ptr, response_data.calculated_checksum);
        ptr += sizeof response_data.calculated_checksum;

        *ptr++ = response_data.status;

        send_packet->length = ptr - data_ptr;

        fw_ptr->ka_time = 0;
        fw_ptr->keepalive = false;

        status = enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
    }
    else if (status == idk_callback_abort)
    {
        send_fw_abort(idk_ptr, request_data.target,IDK_FW_DOWNLOAD_ABORT_OPCODE, response_data.status);
    }

done:
    return status;

}

static idk_callback_status_t process_target_reset(idk_firmware_data_t * fw_ptr, idk_facility_packet_t * packet)
{
    idk_callback_status_t status;
    idk_fw_config_t request;
    uint8_t             * buf;
    uint16_t            length;

    /* Parse firmware target reset
     *  -----------------
     * |   0    |   1    |
     *  -----------------
     * | opcode | target |
     *  -----------------
     *
     */
    buf = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
    buf++;
    length = packet->length;

    request.target = *buf;
    length--;
    buf++;

    status = get_fw_config(fw_ptr, idk_firmware_target_reset, &request, sizeof request, NULL, NULL, fw_equal);

    return status;
}

static idk_callback_status_t fw_process(idk_data_t * idk_ptr, idk_facility_t * fac_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    uint8_t opcode, target;
    idk_facility_packet_t * packet;
    uint8_t * ptr;
    idk_firmware_data_t * fw_ptr = (idk_firmware_data_t *)fac_ptr;


    if (fw_ptr->keepalive)
    {
        /* time to send target-list message during downloading to avoid 
         * disconnecting by server.
         * After sending, update the timing for next send.
         */
        status = fw_discovery(idk_ptr, fac_ptr);
        if (status != idk_callback_abort)
        {
            status = get_system_time(idk_ptr, &fw_ptr->ka_time);
            if (status == idk_callback_abort)
            {
                goto done;
            }
            else 
            {
                fw_ptr->keepalive = false;
                goto _cont;
            }
        }
    }
    else
    {

_cont:
        packet =(idk_facility_packet_t *) fac_ptr->packet;
        if (packet == NULL)
        {
            DEBUG_PRINTF("fw_process: No Packet\n");
            goto done;
        }
        ptr = IDK_PACKET_DATA_POINTER(packet, sizeof(idk_facility_packet_t));
        opcode = *ptr;
        target = *(ptr+1);
        if (target >= fw_ptr->target_count)
        {
            DEBUG_PRINTF("fw_process: invalid target\n");
            status = send_fw_abort(idk_ptr, target, IDK_FW_ERROR_OPCODE, idk_fw_invalid_target);
            goto done;
        }


        if (opcode == IDK_FW_DOWNLOAD_COMPLETE_OPCODE)
        {
            if (fw_ptr->ka_time == 0)
            {
                /* start firmware keepalive (which requires device to
                 * send target list message).
                 */
                fw_ptr->ka_time = idk_ptr->tx_ka_time;
            }
        }


        switch(opcode)
        {
        case IDK_FW_INFO_REQUEST_OPCODE:
            status = process_fw_info_request(fw_ptr, packet);
            break;
        case IDK_FW_DOWNLOAD_REQUEST_OPCODE:
            status = process_fw_download_request(fw_ptr, packet);
            break;
        case IDK_FW_BINARY_BLOCK_OPCODE:
            status = process_fw_binary_block(fw_ptr, packet);
            break;
        case IDK_FW_DOWNLOAD_ABORT_OPCODE:
            status = process_fw_abort(fw_ptr, packet);
            fw_ptr->ka_time = 0;
            fw_ptr->keepalive = false;
            break;
        case IDK_FW_DOWNLOAD_COMPLETE_OPCODE:
            status = process_fw_complete(fw_ptr, packet);
            break;
        case IDK_FW_TARGET_RESET_OPCODE:
            status = process_target_reset(fw_ptr, packet);
            fw_ptr->ka_time = 0;
            fw_ptr->keepalive = false;
            break;
        default:
            status = send_fw_abort(idk_ptr, target, IDK_FW_ERROR_OPCODE, idk_fw_invalid_opcode);
            break;
        }
    }

done:
    if (status != idk_callback_busy)
    {   /* we are done with the packet */
        fw_ptr->facility.packet = NULL;
    }
    return status;
}

static idk_callback_status_t fw_discovery(idk_data_t *idk_ptr, idk_facility_t * fac_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    uint8_t * ptr;
    idk_facility_packet_t * packet;
    idk_firmware_data_t * fw_ptr = (idk_firmware_data_t *)fac_ptr;

    /* Firmware Access facility needs to send target list info during initialization phase at discovery layer */

    /* get packet pointer for constructing target list info */
    packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &ptr);
    if (packet == NULL)
    {
        status = idk_callback_busy;
        goto done;
    }

    /* Construct a target list message.
     * Get target count and then get version for each target to build target list message
     *
     */
    if (fw_ptr->request_id == idk_firmware_target_count)
    {
       status = get_fw_target_count(fw_ptr, &fw_ptr->target_count);
        if (status == idk_callback_continue)
        {
            packet->length = 0;
            *ptr = E_FAC_FU_OP_LIST_TARGETS;
            packet->length++;
            fw_ptr->target = 0;
            fw_ptr->request_id = idk_firmware_version;
        }
    }

    if (fw_ptr->request_id == idk_firmware_version)
    {
        ptr += packet->length;

        while (fw_ptr->target < fw_ptr->target_count)
        {
            uint32_t version;

            /* get the current firmware version for this target */
            status = get_fw_version(fw_ptr, fw_ptr->target, &version);
            if (status == idk_callback_continue)
            {
                /*
                 * --------------------------------------------------------
                 * |   0    |    1   |  2 - 5  |  6 ...                    |
                 *  -------------------------------------------------------
                 * | opcode | target | version | Additional target-version |
                 * |        |        |         |       pairs               |
                 *  -------------------------------------------------------
                 */

                *ptr = fw_ptr->target;  /* target number */
                ptr++;
                packet->length++;
                StoreBE32(ptr, version);
                ptr += sizeof version;
                packet->length += sizeof version;
                fw_ptr->target++;
            }
        }

        if (fw_ptr->target == fw_ptr->target_count)
        {
            status = enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
            fw_ptr->request_id = idk_firmware_target_count;;
            fw_ptr->target = 0;
        }
    }


done:
    return status;
}

static idk_callback_status_t fw_delete_facility(idk_data_t * idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_cc_data_t * cc_ptr;

    cc_ptr = (idk_cc_data_t *)get_facility_data(idk_ptr, E_MSG_FAC_FW_NUM);
    if (cc_ptr != NULL)
    {
        status = del_facility_data(idk_ptr, E_MSG_FAC_FW_NUM);
    }
    return status;
}

static idk_callback_status_t fw_init_facility(idk_data_t *idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_firmware_data_t * fw_ptr;

    /* Add firmware access facility to IDK */
    fw_ptr = (idk_firmware_data_t *)get_facility_data(idk_ptr, E_MSG_FAC_FW_NUM);
    if (fw_ptr == NULL)
    {
        status = add_facility_data(idk_ptr, E_MSG_FAC_FW_NUM, (idk_facility_t **) &fw_ptr,
                                   sizeof(idk_firmware_data_t), fw_discovery, fw_process);

        if (status != idk_callback_continue || fw_ptr == NULL)
        {
            goto done;
        }
   }
    fw_ptr->request_id = 0;
    fw_ptr->target_count = idk_firmware_target_count;
    fw_ptr->target = 0;
    fw_ptr->ka_time = 0;
    fw_ptr->keepalive = false;
    fw_ptr->idk_ptr = idk_ptr;

done:
    return status;
}

