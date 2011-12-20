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
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "idigi_api.h"
#include "platform.h"
#include "idigi_dvt.h"

#define DVT_FW_UNKNOWN_FILE_SIZE   -1

static dvt_data_t dvt_data_list[dvt_case_last] =
{
    {dvt_case_fw_bin_image, 0x00000100, dvt_state_init, "Binary Image", ".*\\.[bB][iI][nN]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_lib_image, 0x01000000, dvt_state_init, "Library Image", ".*\\.a", "libidigi.a", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL}, 
    {dvt_case_fw_exe_image, 0x0D010104, dvt_state_init, "Executable", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_exi_image, 0x0D01010C, dvt_state_init, "Executable", ".*\\.[Ee][Xx][Ii]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_exn_image, 0x0D010117, dvt_state_init, "Executable", ".*\\.[Ee][Xx][Nn]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_download_denied, 0x15000000, dvt_state_init, "Download Denied", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_invalid_size, 0x16000000, dvt_state_init, "Invalid Size", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_invalid_version, 0x17000000, dvt_state_init, "Invalid Version", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_unauthenticated, 0x18000000, dvt_state_init, "Unauthenticated", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_not_allowed, 0x19000000, dvt_state_init, "Not Allowed", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_configured_to_reject, 0x1A000000, dvt_state_init, "Configured to Reject", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_encountered_error, 0x1B000000, dvt_state_init, "Encountered Error", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_user_abort, 0x1E000000, dvt_state_init, "User Abort", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_device_error, 0x1F000000, dvt_state_init, "Device Error", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_invalid_offset, 0x20000000, dvt_state_init, "Invalid Offset", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_invalid_data, 0x21000000, dvt_state_init, "Invalid Data", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_fw_hardware_error, 0x22000000, dvt_state_init, "Hardware Error", ".*\\.[Ee][Xx][Ee]", "firmware.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_no_flag, 0x00000011, dvt_state_init, "Data Service PUT, Plain Text No Flags", ".*\\.ds.p.p.n", "fromfirmware.txt", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_defaults, 0x00000012, dvt_state_init, "Data Service PUT, Defaults", ".*\\.ds.p.d", "", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_append, 0x00000013, dvt_state_init, "Data Service PUT, Append", ".*.\\.ds.p.ap", "main/appender.txt", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_archive, 0x00000014, dvt_state_init, "Data Service PUT, Archive", ".*\\.ds.p.ar", "totally/archived.txt", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_both, 0x00000015, dvt_state_init, "Data Service PUT, Both", ".*\\.ds.p.b", "not/uploaded.txt", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_busy, 0x0D010104, dvt_state_init, "Data Service PUT, Busy", ".*\\.[Ee][Xx][Ee]", "busy.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_cancel_at_start, 0x0D010104, dvt_state_init, "Data Service PUT, Cancel at start", ".*\\.[Ee][Xx][Ee]", "cancel1.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_cancel_at_middle, 0x0D010104, dvt_state_init, "Data Service PUT, Cancel at middle", ".*\\.[Ee][Xx][Ee]", "cancel2.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL},
    {dvt_case_put_request_timeout, 0x0D010104, dvt_state_init, "Data Service PUT, Timeout", ".*\\.[Ee][Xx][Ee]", "timeout.bin", DVT_FW_UNKNOWN_FILE_SIZE, NULL, NULL}
};

dvt_data_t * dvt_current_ptr = NULL;

static void firmware_download_request(idigi_fw_download_request_t const * const download_info, idigi_fw_status_t * download_status)
{
    static size_t const max_code_size =  64 * 1024 * 1024;

    if ((download_info == NULL) || (download_status == NULL))
    {
        APP_DEBUG("firmware_download_request ERROR: iDigi passes incorrect parameters\n");
        *download_status = idigi_fw_download_denied;
        goto error;
    }

    if (dvt_current_ptr != NULL)
    {
        APP_DEBUG("firmware_download_request ERROR: In progress target : %d\n", dvt_current_ptr->target);
        *download_status = idigi_fw_device_error;
        goto error;
    }

    dvt_current_ptr->state = dvt_state_init;
    APP_DEBUG("target = %d\n", download_info->target);
    if (download_info->target >= dvt_case_last) 
    {
        *download_status = idigi_fw_user_abort;
        goto error;
    }
    dvt_current_ptr = &dvt_data_list[download_info->target];
    dvt_current_ptr->target = download_info->target;

    APP_DEBUG("version = 0x%04X\n", download_info->version);
    dvt_current_ptr->version = download_info->version;

    APP_DEBUG("filename = %s\n", download_info->filename);
    if (strlen(download_info->filename) > DVT_FILE_NAME_SIZE) 
    {
        *download_status = idigi_fw_device_error;
        goto error;
    }
    strcpy(dvt_current_ptr->file_name, download_info->filename);

    APP_DEBUG("code size = %d\n", download_info->code_size);
    if (download_info->code_size > max_code_size)
    {
        *download_status = idigi_fw_download_invalid_size;
        goto error;
    }
    dvt_current_ptr->file_size = download_info->code_size;

    APP_DEBUG("desc_string = %s\n", download_info->desc_string);
    if (strlen(download_info->desc_string) > DVT_DESCRIPTION_SIZE) 
    {
        *download_status = idigi_fw_device_error;
        goto error;
    }
    strcpy(dvt_current_ptr->description, download_info->desc_string);

    APP_DEBUG("file name spec = %s\n", download_info->file_name_spec);
    if (strlen(download_info->file_name_spec) > DVT_NAME_SPEC_SIZE) 
    {
        *download_status = idigi_fw_device_error;
        goto error;
    }
    strcpy(dvt_current_ptr->name_spec, download_info->file_name_spec);

    switch (download_info->target) 
    {
    case dvt_case_fw_download_denied:
    	*download_status = idigi_fw_download_denied;
    	break;

    case dvt_case_fw_invalid_size:
    	*download_status = idigi_fw_download_invalid_size;
    	break;

    case dvt_case_fw_invalid_version:
    	*download_status = idigi_fw_download_invalid_version;
    	break;

    case dvt_case_fw_unauthenticated:
    	*download_status = idigi_fw_download_unauthenticated;
    	break;

    case dvt_case_fw_not_allowed:
    	*download_status = idigi_fw_download_not_allowed;
    	break;

    case dvt_case_fw_configured_to_reject:
    	*download_status = idigi_fw_download_configured_to_reject;
    	break;

    case dvt_case_fw_encountered_error:
    	*download_status = idigi_fw_encountered_error;
    	break;

    default:
        dvt_current_ptr->file_size = download_info->code_size;
        if (dvt_current_ptr->file_size == 0) 
        {
            *download_status = idigi_fw_success;
            break;
        }

        dvt_current_ptr->file_content = malloc(download_info->code_size);
        if (dvt_current_ptr->file_content == NULL)
        {
            *download_status = idigi_fw_device_error;
            goto error;
        }

        dvt_current_ptr->state = dvt_state_fw_download_progress;
        *download_status = idigi_fw_success;
        break;
    }

error:
    if ((dvt_current_ptr != NULL) && (*download_status != idigi_fw_success))
        dvt_current_ptr = NULL;
    return;
}

static void firmware_image_data(idigi_fw_image_data_t const * const image_data, idigi_fw_status_t * data_status)
{
    if ((image_data == NULL) || (data_status == NULL))
    {
        APP_DEBUG("firmware_image_data: invalid parameter\n");
        *data_status = idigi_fw_device_error;
        goto error;
    }

    if (dvt_current_ptr->state != dvt_state_fw_download_progress) 
    {
        APP_DEBUG("firmware_image_data: invalid DVT state [%d]\n", dvt_current_ptr->state);
        *data_status = idigi_fw_device_error;
        goto error;
    }

    APP_DEBUG("target = %d, offset = %zu, length = %zu\n", image_data->target, image_data->offset, image_data->length);

    if ((image_data->offset + image_data->length) > dvt_current_ptr->file_size)
    {
        APP_DEBUG("firmware_image_data: invalid offset/length\n");
        *data_status = idigi_fw_invalid_offset;
        goto error;
    }

    switch (image_data->target) 
    {
    case dvt_case_fw_user_abort:
        *data_status = idigi_fw_user_abort;
        break;

    case dvt_case_fw_device_error:
        *data_status = idigi_fw_device_error;
        break;

    case dvt_case_fw_invalid_offset:
        *data_status = idigi_fw_invalid_offset;
        break;

    case dvt_case_fw_invalid_data:
        *data_status = idigi_fw_invalid_data;
        break;

    case dvt_case_fw_hardware_error:
        *data_status = idigi_fw_hardware_error;
        break;

    default:        
        *data_status = idigi_fw_success;
        memcpy(dvt_current_ptr->file_content, image_data->data, image_data->length);
        goto done;
    }
    
error:
    cleanup_dvt_data();

done:
    return;
}

static void firmware_download_complete(idigi_fw_download_complete_request_t const * const complete_request, idigi_fw_download_complete_response_t * complete_response)
{

    if ((complete_request == NULL) || (complete_response == NULL))
    {
        APP_DEBUG("firmware_download_complete Error: iDigi passes incorrect parameters\n");
        complete_response->status = idigi_fw_device_error;
        goto error;
    }

    if (dvt_current_ptr->state != dvt_state_fw_download_progress) 
    {
        APP_DEBUG("firmware_download_complete: invalid DVT state [%d]\n", dvt_current_ptr->state);
        complete_response->status = idigi_fw_device_error;
        goto error;
    }

    APP_DEBUG("target    = %d\n",    complete_request->target);
    APP_DEBUG("code size = %u\n",    complete_request->code_size);
    APP_DEBUG("checksum  = 0x%x\n", (unsigned)complete_request->checksum);

    complete_response->status = idigi_fw_download_success;

    if (complete_request->code_size != dvt_current_ptr->file_size)
    {
        APP_DEBUG("firmware_download_complete: actual image size (%u) != the code size received (%zu)\n",
                      complete_request->code_size, dvt_current_ptr->file_size);
    }

    dvt_current_ptr->state = dvt_state_fw_download_complete;
    goto done;

error:
    cleanup_dvt_data();

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
    }

    cleanup_dvt_data();
    return status;
}

static idigi_callback_status_t firmware_reset(idigi_fw_config_t const * const reset_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    UNUSED_PARAMETER(reset_data);
    /* Server requests firmware reboot */
    APP_DEBUG("firmware_reset\n");

    cleanup_dvt_data();
    return status;
}

idigi_callback_status_t idigi_firmware_callback(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;

    UNUSED_PARAMETER(request_length);

    switch (request)
    {
    case idigi_firmware_target_count:
    {
        uint16_t * count = response_data;

        /* return total number of firmware update targets */
        *count = dvt_case_last;
        break;
    }

    case idigi_firmware_version:
    {
        uint32_t * version = response_data;

        /* return the target version number */
        *version = dvt_data_list[config->target].version;
        break;
    }

    case idigi_firmware_code_size:
    {
        /* Return the target code size */
        uint32_t * code_size = response_data;

        *code_size = dvt_data_list[config->target].file_size;
        break;
    }

    case idigi_firmware_description:
    {
        /* return pointer to firmware target description */
        char ** description = (char **)response_data;

        *description = dvt_data_list[config->target].description;
        *response_length = strlen(dvt_data_list[config->target].description);
       break;
    }

    case idigi_firmware_name_spec:
    {
        /* return pointer to firmware target description */
        char ** name_spec = (char **)response_data;

        *name_spec = dvt_data_list[config->target].name_spec;
        *response_length = strlen(dvt_data_list[config->target].name_spec);
        break;
    }

    case idigi_firmware_download_request:
        firmware_download_request(request_data, response_data);
        break;

    case idigi_firmware_binary_block:
        firmware_image_data(request_data, response_data);
        break;

    case idigi_firmware_download_complete:
        firmware_download_complete(request_data, response_data);
        break;

    case idigi_firmware_download_abort:
        status =  firmware_download_abort(request_data);
        break;

    case idigi_firmware_target_reset:
        status =  firmware_reset(request_data);
        break;
    }

    return status;
}

void cleanup_dvt_data(void)
{
    if (dvt_current_ptr->file_content != NULL) 
    {
        free(dvt_current_ptr->file_content);
        dvt_current_ptr->file_content = NULL;
    }

    dvt_current_ptr->state = dvt_state_init;
}

