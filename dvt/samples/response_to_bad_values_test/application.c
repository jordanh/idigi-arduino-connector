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

#include <stdlib.h>
#include <stdbool.h>
#include "idigi_api.h"
#include "platform.h"
#include "application.h"

extern void idigi_debug_hexvalue(char * label, uint8_t * buff, int length);

extern int unlink (const char *name);

extern void clear_stack_size(void);

extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle, char * const filename, char * const content);

/*
 * This table lists all the errors we wish to cause in response to the callbacks.
 */

forcedErrorInfo_t forcedErrorTable[] =
{
    {                                       
        idigi_config_device_id,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_vendor_id,
        false,
        0,
        0,
        0
    },
    {                                       
        idigi_config_device_type,
        true,
        app_get_forced_error_device_type,
        0,
        0
    },
    {
        idigi_config_server_url,
        false,
        0,
        0,
        0
    },
    {                                       
        idigi_config_connection_type,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_mac_addr,
        false,
        0,
        0,
        0
    },
    {                                       
        idigi_config_link_speed,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_phone_number,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_tx_keepalive,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_rx_keepalive,
        true,
        app_get_forced_error_rx_keepalive_interval,
        0,
        0
    },
    {
        idigi_config_wait_count,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_ip_addr,
        true,
        app_get_forced_error_ip_address, 
        0,
        2
    },
    {
        idigi_config_error_status,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_firmware_facility,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_data_service,
        false,
        0,
        0,
        0
    },
    {
        idigi_config_file_system,
        false,
        0,
        0,
        0
    },
#if (IDIGI_VERSION >= IDIGI_VERSION_1100)
    {
        idigi_config_max_transaction,
        false,
        0,
        0,
        0
    },
#endif
}; 

idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;


    switch (class_id)
    {
    case idigi_class_config:
        if (forcedErrorTable[request_id.config_request].initiate_test == true && forcedErrorTable[request_id.config_request].fptr != 0)
        {
            status = forcedErrorTable[request_id.config_request].fptr(request_data, request_length, response_data, response_length);
            forcedErrorTable[request_id.config_request].initiate_test = false;
        }
        else
        {
            status = app_config_handler(request_id.config_request, request_data, request_length, response_data, response_length);
        }
        break;

    case idigi_class_operating_system:
        status = app_os_handler(request_id.os_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_network:
        status = app_network_handler(request_id.network_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_data_service:
        status = app_data_service_handler(request_id.data_service_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_firmware:
        status = app_firmware_handler(request_id.firmware_request, request_data, request_length, response_data, response_length);

    default:
        /* not supported */
        break;
    }
    clear_stack_size();

    return status;
}
           
#define SLEEP_ONE_SECOND  1

#define params_file_name     "./dvt/cases/test_files/params.bin"
#define response_file_name   "./dvt/cases/test_files/params_response.txt"
#define success_response_str "Success"
#define failure_response_str "Failure"

extern int app_os_malloc(size_t const size, void ** ptr);

int application_run(idigi_handle_t handle)
{
    UNUSED_ARGUMENT(handle); 

    for (;;)
    {
        app_os_sleep(SLEEP_ONE_SECOND);

    }
    return 0;
}

