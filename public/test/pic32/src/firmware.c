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
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include "idigi.h"
#include "firmware.h"

#define asizeof(array)      (sizeof(array)/sizeof(array[0]))

static bool firmware_download_started = false;
static size_t total_image_size = 0;

firmware_list_t* firmware_list;
uint8_t firmware_list_count;

//idigi_data_request_t * data_service_request;

void initialize_firmware(){

    // [firmware]
    // num_targets = 6
    firmware_list_count = 6;
    
    firmware_list = malloc(firmware_list_count * sizeof (firmware_list_t));

    // [firmware.target.0]
    // name_spec = .*\.[bB][iI][nN]
    // description = Binary Image
    // version = 0.0.1.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[0].description = "Binary Image";
    firmware_list[0].name_spec = ".*\\.[bB][iI][nN]";
    firmware_list[0].version = 0x00000100;
    firmware_list[0].code_size = 8;
    firmware_list[0].data_service_enabled = false;

    // [firmware.target.1]
    // name_spec = .*\.a
    // description = Library Image
    // version = 1.0.0.0
    // file = ../../private/libidigi.so
    firmware_list[1].description = "Library Image";
    firmware_list[1].name_spec = ".*\\.a";
    firmware_list[1].version = 0x01000000;
    firmware_list[1].code_size = 8;
    firmware_list[1].data_service_enabled = false;


    // [firmware.target.2]
    // name_spec = .*\.[Ee][Xx][Ee]
    // description = Executable
    // version = 13.1.1.4
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[2].description = "Executable";
    firmware_list[2].name_spec = ".*\\.[Ee][Xx][Ee]";
    firmware_list[2].version = 0x0D010104;
    firmware_list[2].code_size = 8;
    firmware_list[2].data_service_enabled = false;


    // [firmware.target.3]
    // name_spec = .*\.[Ee][Xx][Ii]
    // description = Executable
    // version = 13.1.1.12
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[3].description = "Executable";
    firmware_list[3].name_spec = ".*\\.[Ee][Xx][Ei]";
    firmware_list[3].version = 0x0D01010E;
    firmware_list[3].code_size = 8;
    firmware_list[3].data_service_enabled = false;

    // [firmware.target.4]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Executable
    // version = 13.1.1.23
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[4].description = "Executable";
    firmware_list[4].name_spec = ".*\\.[Ee][Xx][NN]";
    firmware_list[4].version = 0x0D010117;
    firmware_list[4].code_size = 8;
    firmware_list[4].data_service_enabled = false;

    // [firmware.target.5]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Download Denied
    // version = 21.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[5].description = "Download Denied";
    firmware_list[5].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[5].version = 0x15000000;
    firmware_list[5].code_size = 8;
    firmware_list[5].data_service_enabled = false;
    /*
    // [firmware.target.6]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Download Invalid Size
    // version = 22.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[6].description = "Download Invalid Size";
    firmware_list[6].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[6].version = 0x16000000;
    firmware_list[6].code_size = (uint32_t)-1;
    firmware_list[6].data_service_enabled = false;

    // [firmware.target.7]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Download Invalid Version
    // version = 23.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[7].description = "Download Invalid Version";
    firmware_list[7].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[7].version = 0x17000000;
    firmware_list[7].code_size = (uint32_t)-1;
    firmware_list[7].data_service_enabled = false;

    // [firmware.target.8]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Download Unauthenticated
    // version = 24.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[8].description = "Download Unauthenticated";
    firmware_list[8].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[8].version = 0x18000000;
    firmware_list[8].code_size = (uint32_t)-1;
    firmware_list[8].data_service_enabled = false;

    // [firmware.target.9]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Download Not Allowed
    // version = 25.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[9].description = "Download Not Allowed";
    firmware_list[9].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[9].version = 0x19000000;
    firmware_list[9].code_size = (uint32_t)-1;
    firmware_list[9].data_service_enabled = false;
    
    // [firmware.target.10]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Download Configured to Reject
    // version = 26.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[10].description = "Download Configured to Reject";
    firmware_list[10].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[10].version = 0x1A000000;
    firmware_list[10].code_size = (uint32_t)-1;
    firmware_list[10].data_service_enabled = false;

    // [firmware.target.11]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Encountered Error
    // version = 27.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[11].description = "Encountered Error";
    firmware_list[11].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[11].version = 0x1B000000;
    firmware_list[11].code_size = (uint32_t)-1;
    firmware_list[11].data_service_enabled = false;

    // [firmware.target.12]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = User Abort
    // version = 30.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[12].description = "User Abort";
    firmware_list[12].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[12].version = 0x1E000000;
    firmware_list[12].code_size = (uint32_t)-1;
    firmware_list[12].data_service_enabled = false;

    // [firmware.target.13]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Device Error
    // version = 31.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[13].description = "Device Error";
    firmware_list[13].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[13].version = 0x1F000000;
    firmware_list[13].code_size = (uint32_t)-1;
    firmware_list[13].data_service_enabled = false;

    // [firmware.target.14]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Invalid Offset
    // version = 32.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[14].description = "Invalid Offset";
    firmware_list[14].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[14].version = 0x20000000;
    firmware_list[14].code_size = (uint32_t)-1;
    firmware_list[14].data_service_enabled = false;

    // [firmware.target.15]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Invalid Data
    // version = 33.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[15].description = "Invalid Data";
    firmware_list[15].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[15].version = 0x21000000;
    firmware_list[15].code_size = (uint32_t)-1;
    firmware_list[15].data_service_enabled = false;


    // [firmware.target.16]
    // name_spec = .*\.[Ee][Xx][Nn]
    // description = Hardware Error
    // version = 34.0.0.0
    // file = /var/lib/hudson/tools/iik/firmware.bin
    firmware_list[16].description = "Hardware Error";
    firmware_list[16].name_spec = ".*\\.[Ee][Xx][Nn]";
    firmware_list[16].version = 0x22000000;
    firmware_list[16].code_size = (uint32_t)-1;
    firmware_list[16].data_service_enabled = false;*/
}

static idigi_callback_status_t firmware_download_request(idigi_fw_download_request_t * download_data, idigi_fw_status_t * download_status)
{
    idigi_callback_status_t status = idigi_callback_continue;
    *download_status = idigi_fw_success;
    return status;
}

static idigi_callback_status_t firmware_image_data(idigi_fw_image_data_t * image_data, idigi_fw_status_t * data_status)
{
    idigi_callback_status_t   status = idigi_callback_continue;
    *data_status = idigi_fw_success;
    return status;
}


static idigi_callback_status_t firmware_download_complete(idigi_fw_download_complete_request_t * request_data, idigi_fw_download_complete_response_t * response_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;
    response_data->status = idigi_fw_download_success;
    response_data->calculated_checksum = 0;
    return status;
}

static idigi_callback_status_t firmware_download_abort(idigi_fw_download_abort_t * abort_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;
    return status;
}

static idigi_callback_status_t firmware_reset(idigi_fw_config_t * reset_data)
{
    idigi_callback_status_t   status = idigi_callback_continue;
    return status;
}

idigi_callback_status_t idigi_firmware_callback(idigi_firmware_request_t request,
                                                  void * const request_data, size_t request_length,
                                                  void * response_data, size_t * response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;


    switch (request)
    {
    case idigi_firmware_target_count:
    {
        uint16_t * count = (uint16_t *)response_data;
        /* return total number of firmware update targets */
        *count = firmware_list_count;
        break;
    }
    case idigi_firmware_version:
    {
        uint32_t * version = (uint32_t *)response_data;
        /* return the target version number */
        *version = firmware_list[config->target].version;
        break;
    }
    case idigi_firmware_code_size:
    {
        /* Return the target code size */
        uint32_t * code_size = (uint32_t *)response_data;
        *code_size = firmware_list[config->target].code_size;
        break;
    }
    case idigi_firmware_description:
    {
        /* return pointer to firmware target description */
        char ** description = (char **)response_data;
        *description = firmware_list[config->target].description;
        *response_length = strlen(firmware_list[config->target].description);
       break;
    }
    case idigi_firmware_name_spec:
    {
        /* return pointer to firmware target description */
        char ** name_spec = (char **)response_data;
        *name_spec = firmware_list[config->target].name_spec;
        *response_length = strlen(firmware_list[config->target].name_spec);
        break;
    }
    case idigi_firmware_download_request:
        status = firmware_download_request((idigi_fw_download_request_t *)request_data, (idigi_fw_status_t *)response_data);
        break;

    case idigi_firmware_binary_block:
        status =  firmware_image_data((idigi_fw_image_data_t *) request_data, (idigi_fw_status_t *)response_data);
        break;

    case idigi_firmware_download_complete:
        status =  firmware_download_complete((idigi_fw_download_complete_request_t *) request_data,
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