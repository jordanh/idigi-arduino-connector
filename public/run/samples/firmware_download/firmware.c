/*
 * Copyright (c) 1996-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
*/
#include "idigi_api.h"
#include "platform.h"

#define asizeof(array) (sizeof(array)/sizeof(array[0]))

typedef struct {
    uint32_t    version;
    uint32_t    code_size;
    char        * name_spec;
    char        * description;
} firmware_list_t;

/* list of all supported firmware target info */
static firmware_list_t firmware_list[] = {
    /* version     code_size     name_spec          description */
    {0x01000000, (uint32_t)-1, ".*\\.a",            "Library Image"}, /* any *.a files */
    {0x00000100, (uint32_t)-1, ".*\\.[bB][iI][nN]", "Binary Image" }  /* any *.bin files */
};

static int firmware_download_started = 0;
static size_t total_image_size = 0;

static void app_firmware_download_request(idigi_fw_download_request_t const * const download_info, idigi_fw_status_t * download_status)
{

    if ((download_info == NULL) || (download_status == NULL))
    {
        APP_DEBUG("app_firmware_download_request ERROR: iDigi passes incorrect parameters\n");
        goto done;
    }
    if (firmware_download_started)
    {
        *download_status = idigi_fw_device_error;
        goto done;
    }

    APP_DEBUG("target = %d\n",         download_info->target);
    APP_DEBUG("filename = %s\n",       download_info->filename);
    APP_DEBUG("code size = %d\n",      download_info->code_size);

    total_image_size = 0;
    firmware_download_started = 1;

    *download_status = idigi_fw_success;

done:
    return;
}

static void app_firmware_image_data(idigi_fw_image_data_t const * const image_data, idigi_fw_status_t * data_status)
{
    if (image_data == NULL || data_status == NULL)
    {
        APP_DEBUG("app_firmware_image_data ERROR: iDigi passes incorrect parameters\n");
        goto done;
    }

    if (!firmware_download_started)
    {
        APP_DEBUG("app_firmware_image_data:no firmware download request started\n");
        *data_status = idigi_fw_download_denied;
        goto done;
    }

    APP_DEBUG("target = %d\n", image_data->target);
    APP_DEBUG("offset = 0x%04X\n", image_data->offset);
    APP_DEBUG("data = %p\n", image_data->data);
    total_image_size += image_data->length;
    APP_DEBUG("length = %zu (total = %zu)\n", image_data->length, total_image_size);


    *data_status = idigi_fw_success;
done:
    return;
}

static void app_firmware_download_complete(idigi_fw_download_complete_request_t const * const complete_request, idigi_fw_download_complete_response_t * complete_response)
{

    if ((complete_request == NULL) || (complete_response == NULL))
    {
        APP_DEBUG("app_firmware_download_complete Error: iDigi passes incorrect parameters\n");
        goto done;
    }

    if (!firmware_download_started)
    {
        APP_DEBUG("app_firmware_download_complete:no firmware download request started\n");
        complete_response->status = idigi_fw_download_not_complete;
        goto done;
    }

    APP_DEBUG("target    = %d\n",    complete_request->target);
    APP_DEBUG("code size = %u\n",    complete_request->code_size);
    APP_DEBUG("checksum  = 0x%x\n", (unsigned)complete_request->checksum);

    complete_response->status = idigi_fw_download_success;

    if (complete_request->code_size != total_image_size)
    {
        APP_DEBUG("app_firmware_download_complete: actual image size (%u) != the code size received (%zu)\n",
                      complete_request->code_size, total_image_size);
        complete_response->status = idigi_fw_download_not_complete;
    }

    firmware_download_started = 0;

done:
    return;
}

static idigi_callback_status_t app_firmware_download_abort(idigi_fw_download_abort_t const * const abort_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    /* Server is aborting firmware update */
    APP_DEBUG("app_firmware_download_abort\n");
    firmware_download_started = 0;

    if (abort_data == NULL)
    {
        APP_DEBUG("app_firmware_download_abort Error: iDigi passes incorrect parameters\n");
        goto done;
    }

done:
    return status;
}

static idigi_callback_status_t app_firmware_reset(idigi_fw_config_t const * const reset_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    UNUSED_ARGUMENT(reset_data);
    /* Server requests firmware reboot */
    APP_DEBUG("app_firmware_reset\n");

    firmware_download_started = 0;

    return status;
}

idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
    case idigi_firmware_target_count:
        if (response_data != NULL)
        {
            static uint16_t firmware_list_count = asizeof(firmware_list);

            uint16_t * count = response_data;

            *count = firmware_list_count;
            break;
        }
    case idigi_firmware_version:
        if (response_data != NULL)
        {
            uint32_t * version = response_data;

             *version = firmware_list[config->target].version;
            break;
        }
    case idigi_firmware_code_size:
        if (response_data != NULL)
        {
            uint32_t * code_size = response_data;

            *code_size = firmware_list[config->target].code_size;
            break;
        }
    case idigi_firmware_description:
        if (response_data != NULL)
        {
            char ** description = (char **)response_data;

            *description = firmware_list[config->target].description;
            *response_length = strlen(firmware_list[config->target].description);
           break;
        }
    case idigi_firmware_name_spec:
        if (response_data != NULL)
        {
            char ** name_spec = (char **)response_data;

            *name_spec = firmware_list[config->target].name_spec;
            *response_length = strlen(firmware_list[config->target].name_spec);
            break;
        }
    case idigi_firmware_download_request:
        app_firmware_download_request(request_data, response_data);
        break;

    case idigi_firmware_binary_block:
        app_firmware_image_data(request_data, response_data);
        break;

    case idigi_firmware_download_complete:
        app_firmware_download_complete(request_data, response_data);
        break;

    case idigi_firmware_download_abort:
        status =  app_firmware_download_abort(request_data);
        break;

    case idigi_firmware_target_reset:
        status =  app_firmware_reset(request_data);
        break;

    }

    return status;
}
