/*
 * Copyright (c) 2012 Digi International Inc.,
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
#include "idigi_connector.h"
#include "os_support.h"
#include "data_service.h"
#include "idigi_config.h"
#include "idigi_def.h"

typedef struct
{
    idigi_data_service_put_request_t header;
    idigi_app_send_data_t data_ptr;
} idigi_connector_send_t;

static idigi_connector_callbacks_t idigi_callback_list =
{
    NULL, /* status callback */
    NULL, /* device request */
    NULL /* device response */
};

static idigi_handle_t idigi_handle = NULL;

static idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                                  void *  request_data, size_t const request_length,
                                                  void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    switch (class_id)
    {
    case idigi_class_config:
        status = app_config_handler(request_id.config_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_operating_system:
        status = app_os_handler(request_id.os_request, request_data, request_length, response_data, response_length);
        break;

    case idigi_class_network:
        status = app_network_handler(request_id.network_request, request_data, request_length, response_data, response_length);
        break;

#if (defined IDIGI_DATA_SERVICE)
    case idigi_class_data_service:
        status = app_data_service_handler(request_id.firmware_request, request_data, request_length, response_data, response_length);
        break;
#endif

#if (defined IDIGI_FIRMWARE_SERVICE)
    case idigi_class_firmware:
        status = app_firmware_handler(request_id.firmware_request, request_data, request_length, response_data, response_length);
        break;
#endif

#if (defined IDIGI_FILE_SYSTEM)
    case idigi_class_file_system:
        status = app_file_system_handler(request_id.file_system_request, request_data, request_length, response_data, response_length);
        break;
#endif

    default:
        /* not supported */
        break;
    }

    return status;
}

void idigi_connector_thread(unsigned long initial_data)
{
    idigi_status_t status = idigi_success;
    idigi_data_t * idigi_ptr = (idigi_data_t *)idigi_handle;

    UNUSED_PARAMETER(initial_data);

    APP_DEBUG("idigi_connector_thread start\n");

    do
    {
        status = idigi_run(idigi_handle);

        if (status == idigi_receive_error ||
            status == idigi_send_error ||
            status == idigi_keepalive_error ||
            status == idigi_connect_error)
        {
            APP_DEBUG("idigi_connector_run returned %d..restarting\n", status);
            status = idigi_success; /* Network error: restart and reconnect to iDigi. */
            
            /* Update Keepalive time */
            app_os_get_system_time(&idigi_ptr->last_tx_keepalive_received_time);
        }
    } while (status == idigi_success);

    APP_DEBUG("idigi_connector_thread exit %d\n", status);
}

idigi_connector_error_t idigi_connector_start(idigi_status_callback_t status_callback)
{
    idigi_connector_error_t status = idigi_connector_init_error;

    UNUSED_PARAMETER(status_callback);

    idigi_handle = idigi_init((idigi_callback_t)app_idigi_callback);
    ASSERT_GOTO(idigi_handle != NULL, error);

    status = ic_create_thread();
    ASSERT_GOTO(status == idigi_connector_success, error);

    status = ic_create_event(IC_SEND_DATA_EVENT);
    ASSERT_GOTO(status == idigi_connector_success, error);

error:
    return status;
}

idigi_connector_error_t idigi_register_device_request_callbacks(idigi_device_request_callback_t request_callback, idigi_device_response_callback_t response_callback)
{
    idigi_connector_error_t status = idigi_connector_success;

    if ((request_callback == NULL) || (response_callback == NULL))
    {
        APP_DEBUG("idigi_register_device_request_callbacks: NULL parameter\n");
        status = idigi_connector_invalid_parameter;
        goto done;
    }

    if (idigi_callback_list.device_request != NULL)
    {
        APP_DEBUG("idigi_register_device_request_callbacks: already registered once\n");
        status = idigi_connector_already_registered;
        goto done;
    }

    idigi_callback_list.device_request = request_callback;
    idigi_callback_list.device_response = response_callback;

done:
    return status;
}

idigi_connector_error_t idigi_send_data(char const * const path, idigi_connector_data_t * const device_data, char const * const content_type)
{
    static unsigned long send_event_block = 0;
    idigi_connector_error_t result = idigi_connector_network_error;
    idigi_connector_send_t * const send_info = ic_malloc(sizeof(idigi_connector_send_t));

    if ((path == NULL) || (device_data == NULL))
    {
        APP_DEBUG("idigi_send_data: invalid parameter\n");
        result = idigi_connector_invalid_parameter;
        goto error;
    }

    if (send_info == NULL)
    {
        APP_DEBUG("idigi_send_data: malloc failed\n");
        result = idigi_connector_resource_error;
        goto error;
    }

    {
        unsigned long available_bit = 0x80000000;

        while (available_bit != 0)
        {
            if (!(send_event_block & available_bit))
                break;
            available_bit >>= 1;
        }

        send_event_block |= available_bit;
        send_info->data_ptr.event_bit = available_bit;
        if (available_bit == 0)
        {
            APP_DEBUG("idigi_send_data: Exceeded maximum of 32 sessions active at a time\n");
            result = idigi_connector_resource_error;
            goto error;
        }

        /* make sure none of the stale event is pending */
        ic_clear_event(IC_SEND_DATA_EVENT, available_bit);
    }

    /* we are storing some stack variables here, need to block until we get a response */
    send_info->data_ptr.next_data = device_data->data_ptr;
    send_info->data_ptr.bytes_remaining = device_data->length_in_bytes;
    send_info->header.path = path;
    send_info->header.content_type = content_type;
    send_info->header.context = &send_info->data_ptr;

    send_info->header.flags = 0;
    if ((device_data->flags & IDIGI_FLAG_APPEND_DATA) == IDIGI_FLAG_APPEND_DATA)
        send_info->header.flags |= IDIGI_DATA_PUT_APPEND;
    if ((device_data->flags & IDIGI_FLAG_ARCHIVE_DATA) == IDIGI_FLAG_ARCHIVE_DATA)
        send_info->header.flags |= IDIGI_DATA_PUT_ARCHIVE;

    {
        idigi_status_t const status = idigi_initiate_action(idigi_handle, idigi_initiate_data_service, &send_info->header, NULL);

        if (status == idigi_success)
        {
            #define IC_SEND_TIMEOUT_IN_MSEC 90000
            result = ic_get_event(IC_SEND_DATA_EVENT, send_info->data_ptr.event_bit, IC_SEND_TIMEOUT_IN_MSEC);
            ASSERT_GOTO(result == idigi_connector_success, error);
            result = send_info->data_ptr.error;
        }
        else
        {
            result = (status == idigi_init_error) ? idigi_connector_init_error : idigi_connector_resource_error;
        }
    }

error:
    if (send_info != NULL)
    {
        send_event_block &= ~send_info->data_ptr.event_bit;
        ic_free(send_info);
    }

    return result;
}

idigi_connector_callbacks_t * idigi_get_app_callbacks(void)
{
    return &idigi_callback_list;
}

