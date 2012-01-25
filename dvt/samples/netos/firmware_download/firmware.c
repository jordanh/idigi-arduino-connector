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
#include <stdio.h>
#include <stdlib.h>

#include "idigi_api.h"
#include "platform.h"
#include "fservapi.h"


#ifndef asizeof
#define asizeof(array) (sizeof(array)/sizeof(array[0]))
#endif

typedef struct {
    uint32_t    version;
    uint32_t    code_size;
    char        * name_spec;
    char        * description;
} firmware_list_t;

/* list of all supported firmware target info */
static firmware_list_t firmware_list[] = {
    /* version     code_size     name_spec          description */
    {0x01000000, (uint32_t)-1, ".*\\.[tT][xX][tT]", "Text File"}, /* any *.a files */
    {0x00000100, (uint32_t)-1, ".*\\.[bB][iI][nN]", "Binary Image" }  /* any *.bin files */
};
static uint16_t firmware_list_count = asizeof(firmware_list);

static int firmware_download_started = 0;
static size_t total_image_size = 0;

static char filename[256];
static int handle = 0;
static int is_fw_image = 0;

static void firmware_download_request(idigi_fw_download_request_t const * const download_info, idigi_fw_status_t * download_status)
{

    if ((download_info == NULL) || (download_status == NULL))
    {
        APP_DEBUG("firmware_download_request ERROR: iDigi passes incorrect parameters\n");
        *download_status = idigi_fw_download_denied;
        goto done;
    }
    if (firmware_download_started)
    {
        *download_status = idigi_fw_device_error;
        goto done;
    }

    APP_DEBUG("target = %d\n",         download_info->target);
    APP_DEBUG("version = 0x%04X\n",    download_info->version);
    APP_DEBUG("code size = %d\n",      download_info->code_size);
    APP_DEBUG("desc_string = %s\n",    download_info->desc_string);
    APP_DEBUG("file name spec = %s\n", download_info->file_name_spec);
    APP_DEBUG("filename = %s\n",       download_info->filename);

    strcpy(filename, download_info->filename);
    is_fw_image = naFWIsFirmwareFile(filename);
    ++handle;
    
    total_image_size = 0;
    firmware_download_started = 1;

    *download_status = idigi_fw_success;

done:
    return;
}

static void firmware_image_data(idigi_fw_image_data_t const * const image_data, idigi_fw_status_t * data_status)
{
    if (image_data == NULL)
    {
        APP_DEBUG("firmware_image_data: invalid parameter\n");
        goto done;
    }

    APP_DEBUG("target = %d", image_data->target);
    APP_DEBUG(" offset = 0x%04X", image_data->offset);
    APP_DEBUG(" data = %p", image_data->data);
    total_image_size += image_data->length;
    APP_DEBUG(" length = %zu (total = %zu)\n", image_data->length, total_image_size);

    if (is_fw_image)
    {
        int result = naFWStoreImage((char *) image_data->data, image_data->length, filename, handle);
        if (result != 0)
        {
            *data_status = idigi_fw_encountered_error;
            APP_DEBUG("firmware_image_data: naFWStoreImage returned %d\n", result);
            goto done;
        }
    }
    *data_status = idigi_fw_success;
done:
    return;
}

static void firmware_download_complete(idigi_fw_download_complete_request_t const * const complete_request, idigi_fw_download_complete_response_t * complete_response)
{

    if ((complete_request == NULL) || (complete_response == NULL))
    {
        APP_DEBUG("firmware_download_complete Error: iDigi passes incorrect parameters\n");
        goto done;
    }

    APP_DEBUG("target    = %d\n",    complete_request->target);
    APP_DEBUG("code size = %u\n",    complete_request->code_size);
    APP_DEBUG("checksum  = 0x%x\n", (unsigned)complete_request->checksum);

    /* 
     * The server currently use calculated checksum field for write status.
     * 0 means no error.
     */
    complete_response->calculated_checksum = 0;

    if (complete_request->code_size == total_image_size)
    {
        complete_response->status = idigi_fw_download_success;
    }
    else
    {
        APP_DEBUG("firmware_download_complete: actual image size (%u) != the code size received (%zu)\n",
                      complete_request->code_size, total_image_size);

        complete_response->status = idigi_fw_download_checksum_mismatch;
    }
   
    firmware_download_started = 0;

done:
    return;
}

static idigi_callback_status_t firmware_download_abort(idigi_fw_download_abort_t const * const abort_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    /* Server is aborting firmware update */
    APP_DEBUG("firmware_download_abort\n");
    if (abort_data == NULL)
    {
        APP_DEBUG("firmware_download_abort Error: iDigi passes incorrect parameters\n");
        goto done;
    }

done:
    return status;
}

static idigi_callback_status_t firmware_reset(idigi_fw_config_t const * const reset_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    UNUSED_ARGUMENT(reset_data);
    /* Server requests firmware reboot */
    APP_DEBUG("firmware_reset\n");

    if (is_fw_image)
    {
        naFWControlClose(handle);
    }

    return status;
}

idigi_callback_status_t idigi_firmware_callback(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
    case idigi_firmware_target_count:
    {
        uint16_t * count = (uint16_t *)response_data;
        /* return total number of firmware update targets */
        *count = firmware_list_count;
        APP_DEBUG("idigi_firmware_target_count %d\n", *count);

        break;
    }
    case idigi_firmware_version:
    {
        uint32_t * version = (uint32_t *)response_data;
        /* return the target version number */
        *version = firmware_list[config->target].version;
        APP_DEBUG("idigi_firmware_version %x\n", *version);

        break;
    }
    case idigi_firmware_code_size:
    {
        /* Return the target code size */
        uint32_t * code_size = (uint32_t *)response_data;
        *code_size = firmware_list[config->target].code_size;

        APP_DEBUG("idigi_firmware_code_size %d\n", *code_size);
        break;
    }
    case idigi_firmware_description:
    {
        /* return pointer to firmware target description */
        char ** description = (char **)response_data;
        *description = firmware_list[config->target].description;
        *response_length = strlen(firmware_list[config->target].description);

       APP_DEBUG("idigi_firmware_description %s\n", *description);
       break;
    }
    case idigi_firmware_name_spec:
    {
        /* return pointer to firmware target description */
        char ** name_spec = (char **)response_data;
        *name_spec = firmware_list[config->target].name_spec;
        *response_length = strlen(firmware_list[config->target].name_spec);

        APP_DEBUG("idigi_firmware_name_spec %s\n", *name_spec);
        break;
    }
    case idigi_firmware_download_request:
        firmware_download_request((idigi_fw_download_request_t *)request_data, (idigi_fw_status_t *)response_data);
        break;

    case idigi_firmware_binary_block:
        firmware_image_data((idigi_fw_image_data_t *) request_data, (idigi_fw_status_t *)response_data);
        break;

    case idigi_firmware_download_complete:
        firmware_download_complete((idigi_fw_download_complete_request_t *) request_data,
                                  (idigi_fw_download_complete_response_t *) response_data);
        break;

    case idigi_firmware_download_abort:
        status =  firmware_download_abort((idigi_fw_download_abort_t *) request_data);
        break;

    case idigi_firmware_target_reset:
        status =  firmware_reset((idigi_fw_config_t *) request_data);
        break;

    }

    return status;
}
