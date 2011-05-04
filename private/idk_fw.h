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
//#include <malloc.h>
//#include <string.h>

//#include "bele.c"
//#include "idk_def.h"
//#include "ei_security.h"
//#include "ei_msg.h"

#define IDK_FA_ID_STRING_LENGTH     128  /* bytes */

#define IDK_FW_TARGET_LIST_MSG_INTERVAL             30 * IDK_MILLISECONDS /* time to send target list to keep download alive */

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
/**
 * Firmware Upgrade Facility Opcodes
 * @doc These are the valid opcodes for the Firmware Upgrade Facility
 * All other values are invalid and reserved.
 */
/// Target List Message
#define E_FAC_FU_OP_LIST_TARGETS 0x00
/// Firmware Info Request
#define E_FAC_FU_OP_INFO_REQ 0x01
/// Firmware Info Response
#define E_FAC_FU_OP_INFO_RESP 0x02
/// Firmware Download Request
#define E_FAC_FU_OP_DNLD_REQ 0x03
/// Firmware Download Response
#define E_FAC_FU_OP_DNLD_RESP 0x04
/// Firmware Code Block
#define E_FAC_FU_OP_CODE_BLK 0x05
/// Firmware Binary Block Acknowledge
#define E_FAC_FU_OP_CODE_BLK_ACK 0x06
/// Firmware Download Abort
#define E_FAC_FU_OP_DNLD_ABORT 0x07
/// Firmware Download Complete
#define E_FAC_FU_OP_DNLD_CMPLT 0x08
/// Firmware Download Complete Response
#define E_FAC_FU_OP_DNLD_CMPLT_RESP 0x09
/// Request Target Reset
#define E_FAC_FU_OP_TARGET_RESET 0x0a
/// Firmware Download Status
#define E_FAC_FU_OP_DNLD_STATUS 0x0b
/// Error, Target Invalid
#define E_FAC_FU_OP_TARGET_INVALID 0x0c

typedef enum {
    fw_equal,
    fw_less_than,
    fw_greater_than
} fw_compare_type_t;

// struct idk_firmware_data_t   * gIrlFirmwareFacilityData = NULL;

uint32_t gFwTimeout = 0;


typedef struct idk_firmware_data {

    idk_facility_t  facility;

    idk_firmware_request_t request_id;
    uint16_t target;
    uint16_t target_count;
    uint32_t ka_time;
    bool keepalive;


} idk_firmware_data_t;


static idk_status_t idk_fw_process(idk_data_t * idk_ptr, idk_facility_t * fac_ptr);


static idk_callback_status_t get_fw_config(idk_data_t * idk_ptr, idk_firmware_request_t request_id,
                                           void * request, size_t request_size,
                                           void * response, size_t * response_size, fw_compare_type_t compare_type)
{
    idk_callback_status_t   status;
    unsigned timeout;
    uint32_t time_stamp, time_stamp1;
    int ecode;
    uint32_t rx_keepalive;
    uint32_t tx_keepalive;
    idk_firmware_data_t * fw_ptr;
    unsigned * req_timeout;
    size_t  length;
    idk_request_t rid;

    fw_ptr = (idk_firmware_data_t *)get_facility_data(idk_ptr, idk_base_firmware_facility);
    if (fw_ptr == NULL)
    {
        status = idk_facility_init_error;
        goto _ret;
    }

    /* Calculate the timeout value (when to send rx keepalive or 
     * receive tx keepalive) from last rx keepalive or tx keepalive.
     * If callback exceeds the timeout value, error status callback
     * will be called. 
     * Also, make sure the response size from the callback matches 
     * the given response_size.
     */
    status = get_system_time(idk_ptr, &time_stamp);
    if (status != idk_callback_continue)
    {
        goto _ret;
    }

    rx_keepalive = (*idk_ptr->rx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->rx_ka_time);
    tx_keepalive = (*idk_ptr->tx_keepalive * IDK_MILLISECONDS) - (time_stamp - idk_ptr->tx_ka_time);

    ecode = idk_configuration_error;


    timeout = IDK_MIN( rx_keepalive, tx_keepalive);

    if (fw_ptr->ka_time > 0)
    {
        /* when starts downloading, we need to send target list on every
         * IDK_FW_TARGET_LIST_MSG_INTERVAL second. This ka_time is
         * set when we receive IDK_FW_DOWNLOAD_COMPLETE_OPCODE to trigger
         * the keepalive time for sending target list message.
         *
         * see this is min timeout value to the callback.
         */
        timeout = IDK_MIN( timeout, IDK_FW_TARGET_LIST_MSG_INTERVAL - (time_stamp - fw_ptr->ka_time));

    }

    if (gFwTimeout != timeout)
    {
        printf("fw timeout: %d\n", timeout);
        gFwTimeout = timeout;
    }

    /* put the timeout value in the request (1st field is timeout field) */
    req_timeout = (unsigned *)request;
    *req_timeout = timeout/IDK_MILLISECONDS;

    rid.firmware_request = request_id;
    status = idk_ptr->callback(idk_class_firmware, rid, request, request_size, response, &length);

    if (get_system_time(idk_ptr, &time_stamp1) != idk_callback_continue)
    {
        status = idk_callback_abort;
        goto _ret;
    }
    
    if (status == idk_callback_abort)
    {
        fw_ptr->keepalive = false;
        goto _ret;
    }

    if (status == idk_callback_continue && response_size != NULL)
    {
        if (compare_type == fw_equal && length == *response_size)
        {
            goto _cont;
        }
        else if (compare_type == fw_less_than && length < *response_size)
        {
            goto _cont;
        }
        else if (compare_type == fw_less_than && length > *response_size)
        {
            goto _cont;
        }

        DEBUG_PRINTF("get_fw_version: invalid returned size of data %d\n", length);

        status = notify_error_status(idk_ptr->callback, idk_class_firmware, rid, idk_invalid_data_size);
        if (status == idk_callback_abort)
        {
            goto _ret;
        }
        status = idk_callback_busy;
    }

_cont:
    *response_size = length;
            
    if ((time_stamp1- time_stamp) > timeout)
    {
        /* callback exceeds timeout value */
        DEBUG_PRINTF("get_fw_config: callback exceeds timeout (%d - %d) > %d\n", (int)time_stamp1, (int)time_stamp, (int)timeout);
        if (notify_error_status(idk_ptr->callback, idk_class_firmware, rid, idk_exceed_timeout) == idk_callback_abort)
        {
            goto _ret;
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

_ret:
    return status;
}

//#define VALID_SIZE_TYPE(size, type)    (size == sizeof(type))

static idk_callback_status_t get_fw_target_count(idk_data_t * idk_ptr,  uint16_t * count)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_request_t request_id;
//    size_t size;


    DEBUG_PRINTF("--- get FW target count\n");

    request_id.firmware_request = idk_firmware_target_count;
    status = idk_ptr->callback(idk_class_firmware, request_id, NULL, 0, count, NULL);

    if (status == idk_callback_continue)
    {
#if 0
        if (!VALID_SIZE_TYPE(size, uint16_t))
        {
            DEBUG_PRINTF("get_fw_target_count: invalid the returned size of data %d\n", size);

            request_id.firmware_request = idk_firmware_target_count;
            status = notify_error_status(idk_ptr->callback, idk_class_firmware, request_id, idk_invalid_data_size);
            if (status == idk_callback_continue)
            {
                status = idk_callback_busy; 
            }
        }
#endif
        DEBUG_PRINTF("get_fw_target_count: target count = %d\n", *count);
    }

    return status;
}

static idk_callback_status_t get_fw_version(idk_data_t * idk_ptr, uint8_t target, uint32_t * version)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_fw_config_t request;

    DEBUG_PRINTF("--- get firmware target version\n");

    request.target = target;
    
   status = get_fw_config(idk_ptr, idk_firmware_version, &request, sizeof request, version, NULL, fw_equal);
    if (status == idk_callback_continue)
    {
        DEBUG_PRINTF("idk_get_fw_version: target version = 0x%x\n", (unsigned)*version);
    }

    return status;
}

static idk_callback_status_t get_fw_code_size(idk_data_t * idk_ptr, uint8_t target, uint32_t * code_size)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_fw_config_t request;

    DEBUG_PRINTF("--- get firmware target version\n");

    request.target = target;

    status = get_fw_config(idk_ptr, idk_firmware_code_size, &request, sizeof request, code_size, NULL, fw_equal);

    if (status == idk_callback_continue)
    {
        DEBUG_PRINTF("get_fw_code_size: code size = 0x%x\n", (unsigned)*code_size);
    }

    return status;
}

static idk_callback_status_t get_fw_description(idk_data_t * idk_ptr, uint8_t target, char ** desc, size_t * length)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_fw_config_t request;

    DEBUG_PRINTF("--- get firmware description\n");

    request.target = target;
    status = get_fw_config(idk_ptr, idk_firmware_description, &request, sizeof request, *desc, length, fw_less_than);

    return status;
}

static idk_callback_status_t get_fw_name_spec(idk_data_t * idk_ptr,  uint8_t target, char ** spec, size_t * length)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_fw_config_t request;

    DEBUG_PRINTF("--- get firmware name spec\n");

    request.target = target;
    status = get_fw_config(idk_ptr, idk_firmware_name_spec, &request, sizeof request, *spec, length, fw_less_than);

    return status;
}


static idk_callback_status_t send_fw_abort(idk_data_t * idk_ptr, uint8_t target, uint8_t status)
{
    uint8_t * ptr;
    uint8_t status_code = status;
    idk_facility_packet_t   * p;

    /* send firmware info request
     *  ---------------------------------------------------
     * |   0    |    1   |  2 - 5  |  6 - 9   |  10 ...    |
     *  ---------------------------------------------------
     * | opcode | target | version | Available | Firmware  |
     * |        |        |         | code size | ID string |
     *  --------------------------------------------------
     *
     *  Firmware ID string: [descr]0xa[file name spec]
     */
    p = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
    ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    if (status < idk_fw_user_abort || status > idk_fw_hardware_error)
    {
        status_code = idk_fw_device_error;
    }
    /* now add this target to the target list message */
    *ptr++ = IDK_FW_DOWNLOAD_ABORT_OPCODE;
    *ptr++ = target;
    *ptr++ = status_code - idk_fw_user_abort;  /* adjust the Firmware download abort status code */
    p->length += 3;

    return net_enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);

}

static idk_callback_status_t firmware_discovery_layer(idk_data_t *idk_ptr)
{
    idk_callback_status_t status;
    uint8_t * ptr;
    idk_facility_packet_t * p;
    idk_firmware_data_t * fw_ptr;

    fw_ptr = (idk_firmware_data_t *)get_facility_data(idk_ptr, idk_base_firmware_facility);
    if (fw_ptr == NULL)
    {
        status = add_facility_data(idk_ptr, idk_base_firmware_facility, (idk_facility_t **) &fw_ptr, sizeof(idk_firmware_data_t), idk_fw_process);

        if (status == idk_callback_abort || fw_ptr == NULL)
        {
            goto _ret;
        }
        fw_ptr->request_id = 0;
        fw_ptr->target_count = 0;
        fw_ptr->target = 0;
        fw_ptr->ka_time = 0;
        fw_ptr->keepalive = false;
    }


    DEBUG_PRINTF("fw_discovery_layer_init: sends target list info\n");
    p = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
    ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));

    /* Construct a target list message.
     * Get target count and then get version for each target to build target list message
     *
     */
    if (fw_ptr->request_id == idk_firmware_target_count)
    {
        if (fw_ptr->target_count == 0)
        {
            goto _ret;
        }
        else
        {
            status = get_fw_target_count(idk_ptr, &fw_ptr->target_count);
            if (status == idk_callback_continue)
            {
                p->length = 0;
                *ptr = E_FAC_FU_OP_LIST_TARGETS;
                p->length++;
                fw_ptr->target = 0;
                fw_ptr->request_id = idk_firmware_version;
            }

        }

    }

    if (fw_ptr->request_id == idk_firmware_version)
    {
        ptr += p->length;

        while (fw_ptr->target < fw_ptr->target_count)
        {
            uint32_t ver, version;

            /* get the current firmware version for this target */
            status = get_fw_version(idk_ptr, fw_ptr->target, &ver);
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
                p->length++;
                version = TO_BE32(ver);
                memcpy((void *)(ptr+1), (void *)&version, sizeof version);
                p->length += sizeof version;
                fw_ptr->target++;
            }
        }

        if (fw_ptr->target == fw_ptr->target_count)
        {
            status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
            fw_ptr->request_id = idk_firmware_target_count;;
            fw_ptr->target = 0;
        }
    }


_ret:
    return status;
}


static idk_callback_status_t  process_fw_info_request(idk_data_t * idk_ptr, idk_firmware_data_t * fw_ptr, idk_facility_packet_t * p)
{
    idk_callback_status_t status = idk_callback_continue;
    char * data;
    size_t length;
    uint8_t target;
    uint32_t value, be_value;
    uint8_t * ptr;
    idk_facility_packet_t * pkt;
    idk_firmware_request_t request_list[] = { idk_firmware_version, idk_firmware_code_size,
                                                                idk_firmware_description, idk_firmware_name_spec};

    /* build and send firmware info response
     *  ---------------------------------------------------
     * |   0    |    1   |  2 - 5  |  6 - 9   |  10 ...    |
     *  ---------------------------------------------------
     * | opcode | target | version | Available | Firmware  |
     * |        |        |         | code size | ID string |
     *  --------------------------------------------------
     *
     *  Firmware ID string = [descr]0xa[file name spec]
     */
    DEBUG_PRINTF("process_fw_info_request...\n");

    /* let's get the request target number from the request packet */
    ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    ptr++; /* skip the opcode */
    target = *ptr;

    pkt = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
    ptr = IDK_PACKET_DATA_POINTER(pkt, sizeof(idk_facility_packet_t));

    /* let's get and build target info response */
    while (fw_ptr->request_id < sizeof request_list/sizeof request_list[0])
    {
        switch (request_list[fw_ptr->request_id])
        {
        case idk_firmware_version:
            /* add target number to the target list message before version*/
            *ptr++ = IDK_FW_INFO_RESPONSE_OPCODE;
            *ptr++ = target;  /* target number */
            pkt->length = 1;

            status = get_fw_version(idk_ptr, target, &value);
            goto _build_value_cont;

        case idk_firmware_code_size:
            status = get_fw_code_size(idk_ptr, target, &value);

_build_value_cont:
            if (status == idk_callback_continue)
            {
                ptr += pkt->length;
                be_value = TO_BE32(value);
                memcpy((void *)ptr, (void *)&be_value, sizeof be_value);
                pkt->length += sizeof be_value; /* sizeof uint32_t */
            }
            break;

        case idk_firmware_description:
            length = IDK_FA_ID_STRING_LENGTH-1;
            status = get_fw_description(idk_ptr, target, (char **)&data, &length);
            goto _build_name_cont;

        case idk_firmware_name_spec:
            length = IDK_FA_ID_STRING_LENGTH-1;
            status = get_fw_name_spec(idk_ptr, target, (char **)&data, &length);

_build_name_cont:
            if (status == idk_callback_continue)
            {
                if ((pkt->length + length) > (IDK_FA_ID_STRING_LENGTH + sizeof value * 2))
                {
                    idk_request_t request_id;
                    request_id.firmware_request = idk_firmware_description;
                    status = notify_error_status(idk_ptr->callback, idk_class_firmware, request_id, idk_invalid_data_size);
                    if (status != idk_callback_continue)
                    {
                        goto _ret;
                    }
                }
                if (data != NULL)
                {
                    ptr += pkt->length;
                    memcpy((void *)ptr, (void *)data, length);
                    p->length += length;
                    if (request_list[fw_ptr->request_id] == idk_firmware_description)
                    {
                        ptr += length;
                        *ptr = 0x0a;
                        p->length++;
                    }
                }
            }
            break;
        default:
            break;

        }
        if (status != idk_callback_continue)
        {
            goto _ret;
        }
        fw_ptr->request_id++;
    }


    if (fw_ptr->request_id == sizeof request_list/sizeof request_list[0])
    {
        fw_ptr->request_id = idk_firmware_target_count; /* reset back to original state */
        status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
    }


_ret:
    return status;
}

static idk_callback_status_t process_fw_download_request(idk_data_t * idk_ptr, idk_firmware_data_t * fw_ptr, idk_facility_packet_t * p)
{
    idk_callback_status_t status;
    uint32_t value;
    idk_fw_download_request_t request_data;
    idk_fw_status_t response_status = idk_fw_user_abort;
    idk_facility_packet_t   * pkt;
    int i;
    uint8_t * buf;
    uint16_t length;
    uint8_t * ptr, * data_ptr;


    DEBUG_PRINTF("Process Firmware Download request...\n");

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
    buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    buf++; /* skip opcode */
//  buf = p->data + 1;
    length = p->length-1;

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

    if (*buf != 0x0a && *buf != 0x00)
    {
        request_data.desc_string = (char *)buf;
    }

    for (i=0; i < p->length; i++)
    {
        if (buf[i] == 0x0a || buf[i] == 0x00)
        {
            buf[i] = 0x00;
            break;
        }
    }
    length -= (i+1);
    buf += (i+1);


    if (*buf != 0x0a && * buf != 0x00)
    {
        request_data.file_name_spec = (char *)buf;
    }
    for (i=0; i < p->length; i++)
    {
        if (buf[i] == 0x0a || buf[i] == 0x00)
        {
            buf[i] = 0x00;
            break;
        }
    }
    length -= (i+1);
    buf += (i+1);

    if (*buf != 0x0a && *buf != 0x00)
    {
        request_data.filename = (char *)buf;
    }
    buf[length] = 0x0;

    status = get_fw_config(idk_ptr, idk_firmware_download_request, &request_data, sizeof request_data, &response_status, NULL, fw_equal);

    if (status == idk_callback_abort)
    {
        if (response_status > idk_fw_user_abort)
        {
            send_fw_abort(idk_ptr, request_data.target, response_status);
        }
        else
        {
            goto _rsp;
        }
    }
    else if (status == idk_callback_continue)
    {

        response_status = idk_fw_success;

_rsp:
        /* send firmware download response
         *  ---------------------------------------------
         * |     0       |     1    |     2                   |
         *  ---------------------------------------------
         * | opcode | target | response type |
         *  --------------------------------------------
         *
         *  Firmware ID string: [descr]0xa[file name spec]
         */

        pkt = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
        data_ptr = ptr = IDK_PACKET_DATA_POINTER(pkt, sizeof(idk_facility_packet_t));

        /* now add this target to the target list message */
        *ptr++ = IDK_FW_DOWNLOAD_RESPONSE_OPCODE;
        *ptr++ = request_data.target;  /* target number */
        *ptr++ = response_status;

        pkt->length = ptr - data_ptr;
        status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
    }

    return status;
}

static idk_callback_status_t process_fw_binary_block(idk_data_t * idk_ptr, idk_firmware_data_t * fw_ptr, idk_facility_packet_t * p)
{
    idk_callback_status_t status;
    uint32_t value;
    uint8_t ack_required;
    idk_fw_image_data_t request_data;
    idk_facility_packet_t * pkt;
    uint8_t * buf;
    uint16_t length;

    DEBUG_PRINTF("Process Firmware Binary Block...\n");

    /* Parse firmware binary block
     *  --------------------------------------------------------
     * |   0    |   1    |     2        |  3 - 6  |   7..       |
     *  --------------------------------------------------------
     * | opcode | target | Ack required |  offset | binary data |
     *  --------------------------------------------------------
     *
     */

    buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    buf++; /* skip opcode */

//  buf = p->data+1;
    length = p->length-1;

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

    status = get_fw_config(idk_ptr, idk_firmware_binary_block, &request_data, sizeof request_data, NULL, NULL, fw_equal);


    if (status == idk_callback_continue)
    {

        if(ack_required)
        {
            /* send firmware binary block acknowledge
             *  ---------------------------------------------------
             * |   0      |     1     | 2 - 5  |    6       |
             *  -----------------------------------------------------
             * | opcode | target | offset | status |
             *  ----------------------------------------------------
             *
             *  Firmware ID string: [descr]0xa[file name spec]
             */
            uint8_t * ptr, * data_ptr;

            pkt = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
            data_ptr = ptr = IDK_PACKET_DATA_POINTER(pkt, sizeof(idk_facility_packet_t));

            /* now add this target to the target list message */
            *ptr++ = IDK_FW_BINARY_BLOCK_ACK_OPCODE;
            *ptr++ = request_data.target;  /* target number */
            value = TO_BE32(request_data.offset);
            memcpy(ptr, &value, sizeof value);
            ptr += sizeof value;

            *ptr++ = idk_fw_success;

            pkt->length = ptr - data_ptr;

            status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
        }
    }
    else if (status == idk_callback_abort)
    {
        send_fw_abort(idk_ptr, request_data.target, idk_fw_device_error);
    }

    return status;
}

static idk_callback_status_t process_fw_abort(idk_data_t * idk_ptr, idk_firmware_data_t * fw_ptr, idk_facility_packet_t * p)
{
    idk_callback_status_t status;
    idk_fw_download_abort_t request_data;
    uint8_t * buf;
    uint16_t length;

    DEBUG_PRINTF("Process Firmware Abort...\n");

    /* parse firmware download abort
     *  --------------------------
     * |   0    |   1    |   2    |
     *  --------------------------
     * | opcode | target | status |
     *  --------------------------
     *
     */
    buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    buf++;
//  buf = p->data + 1;
    length = p->length-1;

    request_data.target = *buf;
    length--;
    buf++;

    request_data.status = *buf;
    length--;
    buf++;

    status = get_fw_config(idk_ptr, idk_firmware_download_abort, &request_data, sizeof request_data, NULL, NULL, fw_equal);

    return status;

}
static idk_callback_status_t process_fw_complete(idk_data_t * idk_ptr, idk_firmware_data_t * fw_ptr, idk_facility_packet_t * p)
{
    idk_callback_status_t status;
    uint32_t value;
    idk_fw_download_complete_request_t request_data;
    idk_fw_download_complete_response_t response_data;
    idk_facility_packet_t * pkt;
    uint8_t * buf;
    uint16_t length;

    DEBUG_PRINTF("Process Firmware Download Complete...\n");

    /* Parse firmware downlaod complete
     *  ----------------------------------------
     * |   0    |   1    |   2 - 5   |  6 - 9   |
     *  ----------------------------------------
     * | opcode | target | code size | checksum |
     *  ----------------------------------------
     *
     */

    buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    buf++;
//  buf = p->data + 1;
    length = p->length-1;

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
    status = get_fw_config(idk_ptr, idk_firmware_download_complete, &request_data, sizeof request_data, &response_data, NULL, fw_equal);
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
        uint8_t * ptr, * data_ptr;

        pkt = (idk_facility_packet_t *)idk_ptr->send_packet.buffer;
        data_ptr = ptr = IDK_PACKET_DATA_POINTER(pkt, sizeof(idk_facility_packet_t));

        /* now add this target to the target list message */
        *ptr++ = IDK_FW_DOWNLOAD_COMPLETE_RESPONSE_OPCODE;
        *ptr++ = request_data.target;  /* target number */
        value = TO_BE32(response_data.version);
        memcpy((void *)ptr, (void *)&value, sizeof value);
        ptr += sizeof value;

        value = TO_BE32(response_data.calculated_checksum);
        memcpy((void *)ptr, (void *)&value, sizeof value);
        ptr += sizeof value;

        *ptr++ = response_data.status;

        pkt->length = ptr - data_ptr;

        status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_FW_NUM, SECURITY_PROTO_NONE);
    }
    else if (status == idk_callback_abort)
    {
        send_fw_abort(idk_ptr, request_data.target, response_data.status);
    }

    return status;

}

static idk_callback_status_t process_target_reset(idk_data_t * idk_ptr, idk_firmware_data_t * fw_ptr, idk_facility_packet_t * p)
{
    idk_callback_status_t status;
    idk_fw_config_t request;
    uint8_t             * buf;
    uint16_t            length;

    DEBUG_PRINTF("Process Firmware Reset\n");

    /* Parse firmware target reset
     *  -----------------
     * |   0    |   1    |
     *  -----------------
     * | opcode | target |
     *  -----------------
     *
     */
    buf = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
    buf++;
//  buf = p->data + 1;
    length = p->length;

    request.target = *buf;
    length--;
    buf++;

    status = get_fw_config(idk_ptr, idk_firmware_target_reset, &request, sizeof request, NULL, NULL, fw_equal);

    return status;
}

static idk_status_t idk_fw_process(idk_data_t * idk_ptr, idk_facility_t * fac_ptr)
{
    idk_status_t rc = idk_success;
    idk_callback_status_t status;
    uint8_t opcode;
    idk_facility_packet_t * p;
    uint8_t * ptr;
    idk_firmware_data_t * fw_ptr = (idk_firmware_data_t *)fac_ptr;


    DEBUG_PRINTF("idk_fw_process...\n");

    if (fw_ptr->keepalive)
    {
        /* time to send target-list message during downloading to avoid 
         * disconnecting by server.
         * After sending, update the timing for next send.
         */
        status = firmware_discovery_layer(idk_ptr);
        if (status != idk_callback_abort)
        {
            if (get_system_time(idk_ptr, &fw_ptr->ka_time) != idk_callback_continue)
            {
                rc = idk_configuration_error;
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
        p = (idk_facility_packet_t *)fac_ptr->packet;
        ptr = IDK_PACKET_DATA_POINTER(p, sizeof(idk_facility_packet_t));
        opcode = *ptr;

//      if (opcode == IDK_FW_BINARY_BLOCK_OPCODE || opcode == IDK_FW_DOWNLOAD_COMPLETE_OPCODE)
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
            status = process_fw_info_request(idk_ptr, fw_ptr, p);
            break;
        case IDK_FW_DOWNLOAD_REQUEST_OPCODE:
            status = process_fw_download_request(idk_ptr, fw_ptr, p);
            break;
        case IDK_FW_BINARY_BLOCK_OPCODE:
            status = process_fw_binary_block(idk_ptr, fw_ptr, p);
            break;
        case IDK_FW_DOWNLOAD_ABORT_OPCODE:
            status = process_fw_abort(idk_ptr, fw_ptr, p);
            break;
        case IDK_FW_DOWNLOAD_COMPLETE_OPCODE:
            status = process_fw_complete(idk_ptr, fw_ptr, p);
            break;
        case IDK_FW_TARGET_RESET_OPCODE:
            status = process_target_reset(idk_ptr, fw_ptr, p);
            break;
        }

        if (status == idk_callback_abort)
        {
            fw_ptr->facility.packet = NULL;
            rc = idk_firwmare_download_error;
        }
        else if (status != idk_callback_busy)
        {
            fw_ptr->facility.packet = NULL;
        }

    }


    return rc;
}

