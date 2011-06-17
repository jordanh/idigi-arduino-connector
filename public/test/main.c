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
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "idigi_struct.h"
#include "idigi_data.h"
#include "firmware.h"

/* If set, iDigi will start as a separated thread calling idigi_run */

#define ONE_SECOND  1
time_t  deviceSystemUpStartTime;
idigi_callback_status_t idigi_callback(idigi_class_t class, idigi_request_t request,
                                    void const * request_data, size_t request_length,
                                    void * response_data, size_t * response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    switch (class)
    {
    case idigi_class_config:
        status = idigi_config_callback(request.config_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_operating_system:
        status = idigi_os_callback(request.os_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_network:
        status = idigi_network_callback(request.network_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_firmware:
        status = idigi_firmware_callback(request.firmware_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}

static int handle_config(void *user, const char* section, const char* name, const char* value){
	idigi_data_t* config = (idigi_data_t*)user;

	if(strcmp(section, "device") == 0){
		if(strcmp(name, "tx_keepalive") == 0){
			config->tx_keepalive = atoi(value);
		}
		else if(strcmp(name, "rx_keepalive") == 0){
			config->rx_keepalive = atoi(value);
		}
		else if(strcmp(name, "wait_count") == 0){
			config->wait_count = atoi(value);
		}
		else if(strcmp(name, "device_type") == 0){
			config->device_type = strdup(value);
		}
		else if(strcmp(name, "server_url") == 0){
			config->server_url = strdup(value);
		}
		else if(strcmp(name, "vendor_id") == 0){
			long vendor_id = atol(value);
      
			config->vendor_id[3] = (uint8_t)(vendor_id);
			config->vendor_id[2] = (uint8_t)(vendor_id >> 8);
			config->vendor_id[1] = (uint8_t)(vendor_id >> 16);
			config->vendor_id[0] = (uint8_t)(vendor_id >> 24);
		}
		else if(strcmp(name, "mac_addr") == 0){
			sscanf(value, "%2hhx%2hhx%2hhx:%2hhx%2hhx%2hhx",
					&config->mac_addr[0],
					&config->mac_addr[1],
					&config->mac_addr[2],
					&config->mac_addr[3],
					&config->mac_addr[4],
					&config->mac_addr[5]);
		}
	}
	else if(strcmp(section, "firmware") == 0){
		if(strcmp(name, "num_targets") == 0){
			firmware_list_count = atoi(value);
			firmware_list = malloc(firmware_list_count * sizeof (firmware_list_t));
		}
	}
	else{
		int i;
		char f_section[50];
		for(i = 0; i < firmware_list_count; i++){
			sprintf(f_section, "firmware.target.%d", i);
			if(strcmp(section, f_section) == 0){
				if(strcmp(name, "name_spec") == 0){
					(&firmware_list[i])->name_spec = strdup(value);
				}
				else if(strcmp(name, "description") == 0){
					(&firmware_list[i])->description = strdup(value);
				}
				else if(strcmp(name, "version") == 0){
					uint8_t version[4];
					sscanf(value, "%hhu.%hhu.%hhu.%hhu",
							&version[0], &version[1], &version[2], &version[3]);

					(&firmware_list[i])->version += version[0] << 24;
					(&firmware_list[i])->version += version[1] << 16;
					(&firmware_list[i])->version += version[2] << 8;
					(&firmware_list[i])->version += version[3] << 0;
				}
				else if(strcmp(name, "file") == 0){
					(&firmware_list[i])->code_size = (uint32_t)-1;
				}
			}
		}
	}
}

int main (int argc, char* argv[])
{
	if(argc < 2){
		perror("Usage: iik_test config_file\n");
		exit(-1);
	}
	char* config_file = argv[1];

	if(ini_parse(config_file, handle_config, &iDigiSetting) < 0){
		fprintf(stderr, "Can't load configuration from '%s'.\n", config_file);
		exit(-2);
	}

	iDigiSetting.socket_fd = -1;

    idigi_status_t status = idigi_success;

    time(&deviceSystemUpStartTime);
    time(&iDigiSetting.start_system_up_time);
    DEBUG_PRINTF("Start iDigi\n");
    iDigiSetting.idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
    if (iDigiSetting.idigi_handle != 0)
    {

        while (status == idigi_success)
        {
            status = idigi_step(iDigiSetting.idigi_handle);
            iDigiSetting.select_data = 0;

            if (status == idigi_success)
            {
                iDigiSetting.select_data |= NETWORK_TIMEOUT_SET | NETWORK_READ_SET;
                network_select(iDigiSetting.socket_fd, iDigiSetting.select_data, ONE_SECOND);
            }
        }
        DEBUG_PRINTF("idigi status = %d\n", status);
   }
   DEBUG_PRINTF("iDigi stops running!\n");
   return 0;
}
