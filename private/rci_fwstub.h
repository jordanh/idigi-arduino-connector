/*
 * Copyright (c) 2011, 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#define FW_TARGET_COUNT 1
#define FW_NO_CODE_SIZE -1
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
    idigi_bool_t send_busy;
    size_t  response_size;
    uint8_t response_buffer[FW_MESSAGE_RESPONSE_MAX_SIZE + PACKET_EDP_FACILITY_SIZE];
} idigi_firmware_data_t;


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
    idigi_callback_status_t status = idigi_callback_continue;
    uint8_t const target = message_load_u8(fw_message, target);

    idigi_debug_printf("Firmware Facility: process info request\n");
    /* parse firmware info request
     *  -----------------
     * |   0    |    1   |
     *  -----------------
     * | opcode | target |
     *  -----------------
     */
    if (length != MAX_FW_INFO_REQUEST_LENGTH)
    {
        fw_abort_status_t fw_status;
        idigi_debug_printf("process_fw_info_request: invalid message length\n");

        fw_status.error_status = fw_invalid_msg;
        status = send_fw_abort(fw_ptr, target, fw_error_opcode, fw_status);
        goto done;

    }

    /* let's build a response.
     * build and send firmware info response
    */
    {
        #define FW_NOT_UPGRADABLE_DESCRIPTION "Non updatable firmware"

        static const char fw_target_description[] = FW_NOT_UPGRADABLE_DESCRIPTION;
        static const size_t fw_target_description_length = sizeof fw_target_description -1;

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

        ASSERT(avail_length > (record_bytes(fw_info) + fw_target_description_length));

        message_store_u8(fw_info, opcode, fw_info_response_opcode);
        message_store_u8(fw_info, target, 0);
        message_store_be32(fw_info, version, FIRMWARE_TARGET_ZERO_VERSION);
        message_store_be32(fw_info, code_size, FW_NO_CODE_SIZE);
        fw_info += record_bytes(fw_info);



        memcpy(fw_info, fw_target_description, fw_target_description_length);
        fw_info += fw_target_description_length;
        *fw_info++ = '\n';

        /* reset back to initial values */

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
    fw_abort_status_t response_status;

    uint8_t const target_number = message_load_u8(fw_download_request, target);

    if (length < record_bytes(fw_download_request))
    {
        idigi_debug_printf("process_fw_download_request: invalid message length\n");
        response_status.error_status = fw_invalid_msg;
        status = send_fw_abort(fw_ptr, target_number, fw_error_opcode, response_status);
        goto done;
    }

    {
        /* get a buffer for sending a response */
        uint8_t * fw_download_response = GET_PACKET_DATA_POINTER(fw_ptr->response_buffer, PACKET_EDP_FACILITY_SIZE);

        ASSERT((sizeof fw_ptr->response_buffer - PACKET_EDP_FACILITY_SIZE) > record_bytes(fw_download_response));

        /* send error firmware download response */
        response_status.user_status = idigi_fw_download_configured_to_reject;
        message_store_u8(fw_download_response, opcode, fw_download_response_opcode);
        message_store_u8(fw_download_response, target, target_number);
        message_store_u8(fw_download_response, response_type, response_status.user_status);

        fw_ptr->response_size = record_bytes(fw_download_response);

        status = send_fw_message(fw_ptr);
    }

done:
    return status;
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
enum fw_target_list {
    field_define(fw_target_list, opcode, uint8_t),
    field_define(fw_target_list, target, uint8_t),
    field_define(fw_target_list, version, uint32_t),
    record_end(fw_target_list)
};

    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(packet);
    UNUSED_PARAMETER(receive_timeout);
    UNUSED_PARAMETER(facility_data);

    /* Construct a target list message.
     */
    {

        uint8_t * edp_header;
        uint8_t * fw_target_list;
        size_t avail_length;

        /* get packet pointer for constructing target list info */
        edp_header = get_packet_buffer(idigi_ptr, E_MSG_FAC_FW_NUM, &fw_target_list, &avail_length);
        if (edp_header == NULL)
        {
            status = idigi_callback_busy;
            goto done;
        }

        ASSERT(avail_length >= record_bytes(fw_target_list));

        message_store_u8(fw_target_list, opcode, fw_target_list_opcode);

        {
            uint8_t const target_number = 0; /* one target only */

            message_store_u8(fw_target_list, target, target_number);
            message_store_be32(fw_target_list, version, FIRMWARE_TARGET_ZERO_VERSION);
        }

        status = initiate_send_facility_packet(idigi_ptr, edp_header, record_bytes(fw_target_list),
                                               E_MSG_FAC_FW_NUM, release_packet_buffer, NULL);
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

    UNUSED_PARAMETER(receive_timeout);
    fw_ptr->idigi_ptr = idigi_ptr;

    if (edp_header == NULL)
    {
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
        idigi_debug_printf("fw_process: invalid packet size %d\n", length);
        goto done;
    }

    fw_message = GET_PACKET_DATA_POINTER(edp_header, PACKET_EDP_FACILITY_SIZE);
    opcode = message_load_u8(fw_message, opcode);
    target = message_load_u8(fw_message, target);

    ASSERT(FW_TARGET_COUNT == 1);

    if (target >= FW_TARGET_COUNT)
    {
        fw_abort_status_t  fw_status;

        idigi_debug_printf("fw_process: invalid target\n");

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
    case fw_download_complete_opcode:
    case fw_binary_block_opcode:
        /* We already reject the download request.
         * so should not receive this packet.
         */
        ASSERT(idigi_false);
        break;
    case fw_download_abort_opcode:
        break;
    case fw_target_reset_opcode:
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
    fw_ptr->send_busy = idigi_false;
    fw_ptr->idigi_ptr = idigi_ptr;

done:
    return status;
}

