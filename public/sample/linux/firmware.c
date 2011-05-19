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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "firmware.h"
#include "idigi_data.h"
#include "os.h"

typedef struct {
    uint32_t    version;
    uint32_t    code_size;
    char        * name_spec;
    char        * description;
} firmware_list_t;

firmware_list_t gFirmwareList[] = {
        /* version     code_size     name_spec       description */
/*    {0x00000100, (uint32_t)-1, ".*\\.[bB][iI][nN]", "Binary Image" } */
    {0x10000000, (uint32_t)-1, "[pP][aA][rR][tT][iI][tT][iI][oO][nN]1" "Partition 1 Image"},
    {0x10000001, (uint32_t)-1, "[pP][aA][rR][tT][iI][tT][iI][oO][nN]2" "Partition 2 Image"},
    {0x10000002, (uint32_t)-1, "[pP][aA][rR][tT][iI][tT][iI][oO][nN]3" "Partition 3 Image"},
};
uint16_t gFirmwareListCount = sizeof gFirmwareList/sizeof gFirmwareList[0];

idk_callback_status_t firmware_download_request(idk_fw_download_request_t * download_data, idk_fw_status_t * download_status)
{
    idk_callback_status_t status = idk_callback_continue;

#error "Add code to process firmware downlaod request";

    DEBUG_PRINTF("firmware_download_request...\n");
    if (download_data == NULL || download_status == NULL)
    {
        DEBUG_PRINTF("--- ERROR: IDK passes incorrect parameters\n");
        *download_status = idk_fw_download_denied;
        goto _ret;
    }

    DEBUG_PRINTF("--- target = %d\n", download_data->target);
    DEBUG_PRINTF("--- version = 0x%04X\n", download_data->version);
    DEBUG_PRINTF("--- code size = %d\n", download_data->code_size);
    DEBUG_PRINTF("--- desc_string = %s\n", download_data->desc_string);
    DEBUG_PRINTF("--- file name spec = %s\n", download_data->file_name_spec);
    DEBUG_PRINTF("--- filename = %s\n", download_data->filename);

    *download_status = idk_fw_download_success;

_ret:
    return status;
}

idk_callback_status_t firmware_image_data(idk_fw_image_data_t * image_data, idk_fw_status_t * data_status)
{
    idk_callback_status_t   status = idk_callback_continue;

#error "Add code to process fimware image data";
    DEBUG_PRINTF("firmware_image_data...\n");
    if (image_data == NULL)
    {
        DEBUG_PRINTF("ERROR: IDK passes incorrect parameters\n");
        goto _ret;
    }

    DEBUG_PRINTF("--- target = %d\n", image_data->target);
    DEBUG_PRINTF("--- offset = 0x%04X\n", image_data->offset);
    DEBUG_PRINTF("--- data = 0x%x\n", (unsigned)image_data->data);
    DEBUG_PRINTF("--- length = %d\n", image_data->length);


    *data_status = idk_fw_success;
_ret:
    return status;
}


idk_callback_status_t firmware_download_complete(idk_fw_download_complete_request_t * request_data, idk_fw_download_complete_response_t * response_data)
{
    idk_callback_status_t   status = idk_callback_continue;

#error "Add code to process firmware download complete";

    DEBUG_PRINTF("firmware_download_complete...\n");
    if (request_data == NULL || response_data == NULL)
    {
        DEBUG_PRINTF("--- Error: IDK passes incorrect parameters\n");
        goto _ret;
    }

    DEBUG_PRINTF("--- target = %d\n", request_data->target);
    DEBUG_PRINTF("--- code size = %d\n", request_data->code_size);
    DEBUG_PRINTF("--- checksum = 0x%x\n", (unsigned)request_data->checksum);

    response_data->status = idk_fw_download_success;

    /* Server currently use calculated checksum field for write status.
     * 0 means no error.
     */
    response_data->calculated_checksum = 0;


_ret:
    return status;
}

idk_callback_status_t firmware_download_abort(idk_fw_download_abort_t * abort_data)
{
    idk_callback_status_t   status = idk_callback_continue;;

#error "Add code to handle firmware download abort";

    DEBUG_PRINTF("firmware_download_abort...\n");
    if (abort_data == NULL)
    {
        DEBUG_PRINTF("--- Error: IDK passes incorrect parameters\n");
        goto _ret;
    }

_ret:
    return status;
}

idk_callback_status_t firmware_reset(idk_fw_config_t * reset_data)
{
    idk_callback_status_t   status = idk_callback_continue;;

#error "Add code to do firmware reset";
    DEBUG_PRINTF("firmware_reset...\n");

    return status;
}


idk_callback_status_t idigi_firmware_callback(idk_firmware_request_t request,
                                                  void const * request_data, size_t request_length,
                                                  void * response_data, size_t * response_length)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_fw_config_t * config = (idk_fw_config_t *)request_data;;

    switch (request)
    {
    case idk_firmware_target_count:
    {
        uint16_t * value = (uint16_t *)response_data;
#error "specify number of targets"
        *value = gFirmwareListCount;
        break;
    }
    case idk_firmware_version:
#error "Return firmware version";
        if (config->target < gFirmwareListCount)
        {
            uint32_t * version = (uint32_t *)response_data;
            *version = gFirmwareList[config->target].version;
        }
        else
        {
            DEBUG_PRINTF("idigi_firmware_callback: invalid target %d\n", config->target);
        }
        break;

    case idk_firmware_code_size:
#error "Return firmware code size";
        if (config->target < gFirmwareListCount)
        {
            uint32_t * code_size = (uint32_t *)response_data;
            * code_size = gFirmwareList[config->target].code_size;
        }
        else
        {
            DEBUG_PRINTF("idigi_firmware_callback: invalid target %d\n", config->target);
        }
        break;

    case idk_firmware_description:
#error "return firmware description";
        if (config->target < gFirmwareListCount)
        {
            char ** desc = (char **)response_data;
            *desc = gFirmwareList[config->target].description;
            *response_length = strlen(gFirmwareList[config->target].description);
        }
        else
        {
            DEBUG_PRINTF("idigi_firmware_callback: invalid target %d\n", config->target);
        }
        break;
    case idk_firmware_name_spec:
#error "return firmware file name spec";
        if (config->target < gFirmwareListCount)
        {
            char ** spec = (char **)response_data;
            * spec =  gFirmwareList[config->target].name_spec;
            *response_length = strlen(gFirmwareList[config->target].name_spec);
        }
        else
        {
            DEBUG_PRINTF("idigi_firmware_callback: invalid target %d\n", config->target);
        }
        break;
    case idk_firmware_download_request:
        status = firmware_download_request((idk_fw_download_request_t *)request_data, (idk_fw_status_t *)response_data);
        break;

    case idk_firmware_binary_block:
        status =  firmware_image_data((idk_fw_image_data_t *) request_data, (idk_fw_status_t *)response_data);
        break;

    case idk_firmware_download_complete:
        status =  firmware_download_complete((idk_fw_download_complete_request_t *) request_data,
                                                                   (idk_fw_download_complete_response_t *) response_data);

        break;
    case idk_firmware_download_abort:
        status =  firmware_download_abort((idk_fw_download_abort_t *) request_data);
        break;

    case idk_firmware_target_reset:
        status =  firmware_reset((idk_fw_config_t *) request_data);
        break;
    }

    return status;
}
