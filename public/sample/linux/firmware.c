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
    {0x00000100, (uint32_t)-1, ".*\\.[bB][iI][nN]", "Binary Image" }
};
uint16_t gFirmwareListCount = sizeof gFirmwareList/sizeof gFirmwareList[0];


typedef struct {
    uint8_t target;
    int     fd;
    size_t  total_length;
    bool        start_download;
    idk_fw_status_t status;
} firmware_data_t;

firmware_data_t gFirmwareData = { 0, -1, 0, false, idk_fw_success};

int fw_state = 0;

idk_callback_status_t firmware_download_request(idk_fw_download_request_t * download_data, idk_fw_status_t * download_status)
{
    idk_callback_status_t status = idk_callback_continue;

    DEBUG_PRINTF("firmware_download_request...\n");
    if (download_data == NULL || download_status == NULL || download_data->target >= gFirmwareListCount)
    {
        DEBUG_PRINTF("--- ERROR: IDK passes incorrect parameters\n");
        *download_status = idk_fw_download_denied;
        goto _ret;
    }

    if (fw_state > 0)
    {
        status = idk_callback_busy;
        fw_state--;
    }
    else
        fw_state = 10;

    gFirmwareData.target = download_data->target;



    DEBUG_PRINTF("--- target = %d\n", download_data->target);
    DEBUG_PRINTF("--- version = 0x%04X\n", download_data->version);
    DEBUG_PRINTF("--- code size = %d\n", download_data->code_size);
    DEBUG_PRINTF("--- desc_string = %s\n", download_data->desc_string);
    DEBUG_PRINTF("--- file name spec = %s\n", download_data->file_name_spec);
    DEBUG_PRINTF("--- filename = %s\n", download_data->filename);

    gFirmwareData.fd = open(download_data->filename, (O_WRONLY | O_CREAT | O_TRUNC), (S_IRWXU | S_IRWXG | S_IRWXO));

    if (gFirmwareData.fd == -1)
    {
        perror("firmware_download_request: open fails");
        *download_status = idk_fw_download_denied;
        goto _ret;
    }
    else
    {
        DEBUG_PRINTF("--- ready for image data\n");
        *download_status = idk_fw_success;
        gFirmwareData.status = idk_fw_success;;
        gFirmwareData.total_length = 0;
        gFirmwareData.start_download = true;
    }

_ret:
    return status;
}

idk_callback_status_t firmware_image_data(idk_fw_image_data_t * image_data)
{
    idk_callback_status_t   status = idk_callback_continue;
    int bytes;

    DEBUG_PRINTF("firmware_image_data...\n");
    if (image_data == NULL || !gFirmwareData.start_download || gFirmwareData.target != image_data->target)
    {
        DEBUG_PRINTF("ERROR: IDK passes incorrect parameters\n");
        goto _ret;
    }

    if (image_data->length > 0)
    {
        DEBUG_PRINTF("--- target = %d\n", image_data->target);
        DEBUG_PRINTF("--- offset = 0x%04X\n", image_data->offset);
        DEBUG_PRINTF("--- data = 0x%x\n", (unsigned)image_data->data);
        DEBUG_PRINTF("--- length = %d\n", image_data->length);
    }

    if (fw_state > 0)
    {
        status = idk_callback_busy;
        fw_state--;
        goto _ret;
    }

    bytes = write(gFirmwareData.fd, image_data->data, image_data->length);
    if (bytes < 0)
    {
        if (errno != EAGAIN)
        {
            perror("firmware_image_data: write fails");
            gFirmwareData.status = idk_fw_encounterted_error;
            goto _ret;
        }
        bytes = 0;
    }

    gFirmwareData.total_length += bytes;

    if (image_data->length != bytes)
    {
        status = idk_callback_busy;
    }

_ret:
    return status;
}

int gFwComplete = 0;

idk_callback_status_t firmware_download_complete(idk_fw_download_complete_request_t * request_data, idk_fw_download_complete_response_t * response_data)
{
    idk_callback_status_t   status = idk_callback_continue;

    DEBUG_PRINTF("firmware_download_complete...\n");
    if (request_data == NULL || response_data == NULL || !gFirmwareData.start_download ||
         gFirmwareData.target != request_data->target)
    {
        DEBUG_PRINTF("--- Error: IDK passes incorrect parameters\n");
        goto _ret;
    }

    if (gFwComplete == 0)
    {
        printf("firmware: wait %d sec\n", request_data->timeout);
        hal_wait(request_data->timeout * 1000);
        gFwComplete = 1;
        status = idk_callback_busy;
        printf("firmware: return %d\n", status);
        goto _ret;
    }


    DEBUG_PRINTF("--- target = %d\n", request_data->target);
    DEBUG_PRINTF("--- code size = %d\n", request_data->code_size);
    DEBUG_PRINTF("--- checksum = 0x%x\n", (unsigned)request_data->checksum);

    if (gFirmwareData.total_length != request_data->code_size)
    {
        DEBUG_PRINTF("firmware_download_complete: code size incorrect (received %d != %d)\n",
                                    gFirmwareData.total_length, request_data->code_size);
    }

    if (gFirmwareData.status != idk_fw_success)
    {
        response_data->status = idk_fw_download_not_complete;
        goto _ret;
    }
    else
    {
        response_data->status = idk_fw_download_success;
        gFirmwareList[gFirmwareData.target].version++;
    }

    /* We know server use calculated checksum field for write status */
    response_data->calculated_checksum = gFirmwareData.status;

    response_data->version = gFirmwareList[gFirmwareData.target].version;

    if (gFirmwareData.fd != -1)
    {
        if (close(gFirmwareData.fd) < 0)
            perror("firmware_download_complete: close fails");
    }

    gFirmwareData.fd = -1;
    gFirmwareData.total_length = 0;
    gFirmwareData.start_download = false;
    gFirmwareData.status = idk_fw_success;

_ret:
    return status;
}

idk_callback_status_t firmware_download_abort(idk_fw_download_abort_t * abort_data)
{
    idk_callback_status_t   status = idk_callback_continue;;

    DEBUG_PRINTF("firmware_download_abort...\n");
    if (abort_data == NULL || !gFirmwareData.start_download || gFirmwareData.target != abort_data->target)
    {
        DEBUG_PRINTF("--- Error: IDK passes incorrect parameters\n");
        goto _ret;
    }

    if (gFirmwareData.fd != -1)
    {
        if (close(gFirmwareData.fd) < 0)
            perror("firmware_download_abort: close fails");
    }

    gFirmwareData.fd = -1;
    gFirmwareData.total_length = 0;
    gFirmwareData.start_download = false;
    gFirmwareData.status = idk_fw_success;

_ret:
    return status;
}

idk_callback_status_t firmware_reset(idk_fw_config_t * reset_data)
{
    idk_callback_status_t   status = idk_callback_continue;;

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
        *value = gFirmwareListCount;
        break;
    }
    case idk_firmware_version:
        if (config->target < gFirmwareListCount)
        {
            uint32_t * version = (uint32_t *)response_data;
            *version = gFirmwareList[config->target].version;
        }
        else
        {
            DEBUG_PRINTF("idigi_firmware_callback: invalid target %d\n", config->target);
        }
        if (fw_state > 0)
        {
            status = idk_callback_busy;
            fw_state--;
        }
        else
            fw_state = 5;
        break;

    case idk_firmware_code_size:
        if (config->target < gFirmwareListCount)
        {
            uint32_t * code_size = (uint32_t *)response_data;
            * code_size = gFirmwareList[config->target].code_size;
        }
        else
        {
            DEBUG_PRINTF("idigi_firmware_callback: invalid target %d\n", config->target);
        }

        if (fw_state > 0)
        {
            status = idk_callback_busy;
            fw_state--;
        }
        else
        {
            fw_state = 30;
        }
        break;

    case idk_firmware_description:
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
        if (fw_state > 0)
        {
            status = idk_callback_busy;
            fw_state--;
        }
        else
            fw_state = 2;
        break;
    case idk_firmware_name_spec:
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
        status =  firmware_image_data((idk_fw_image_data_t *) request_data);
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
