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
#include "idigi_api.h"
#include "idigi_def.h"
#include "bele.h"
#include "os_intf.c"
#include "network_intf.c"
#include "idigi_cc.c"
#include "idigi_fw.c"
#include "idigi_msg.c"
#include "idigi_data.c"
#include "idigi_rci.c"
#include "layer.c"

idigi_handle_t idigi_init(idigi_callback_t const callback)
{
#define INIT_REQUEST_ID_COUNT   3

    idigi_data_t * idigi_handle = NULL;
    idigi_callback_status_t status = idigi_callback_abort;
    void * handle;
    unsigned i;


    struct {
        idigi_config_request_t request;
        size_t length;
    } idigi_config_request_ids[INIT_REQUEST_ID_COUNT] = {
            {idigi_config_device_id, DEVICE_ID_LENGTH},
            {idigi_config_vendor_id, VENDOR_ID_LENGTH},
            {idigi_config_device_type, DEVICE_TYPE_LENGTH}
    };

    ASSERT_GOTO(callback != NULL, done);

    /* allocate idk data */
    status = malloc_cb(callback, sizeof(idigi_data_t), &handle);
    if (status != idigi_callback_continue)
    {
        goto done;
    }

    ASSERT_GOTO(handle != NULL, done);

    idigi_handle = handle;

    init_setting(idigi_handle);
    idigi_handle->active_state = idigi_device_started;
    idigi_handle->callback = callback;
    idigi_handle->facility_list = NULL;
    idigi_handle->network_handle = NULL;
    idigi_handle->facilities = 0;
    idigi_handle->network_busy = false;
    idigi_handle->edp_connected = false;
    idigi_handle->receive_packet.packet_buffer.in_used = false;
    idigi_handle->receive_packet.packet_buffer.next = NULL;



    /* get device id, vendor id, & device type */
    i = 0;
    while (i < INIT_REQUEST_ID_COUNT)
    {
        size_t length;
        void * data;
        idigi_request_t request_id;
        idigi_status_t error_status = idigi_success;

        request_id.config_request = idigi_config_request_ids[i].request;
        status = idigi_callback(idigi_handle->callback, idigi_class_config, request_id, NULL, 0, &data, &length);

        if (status == idigi_callback_continue)
        {
            if (data == NULL)
            {
                error_status = idigi_invalid_data;
            }
            else if ((idigi_config_request_ids[i].request != idigi_config_device_type) &&
                     (length != idigi_config_request_ids[i].length))
            {
                error_status = idigi_invalid_data_size;
            }
            else if ((idigi_config_request_ids[i].request == idigi_config_device_type) &&
                    ((length == 0) || (length > idigi_config_request_ids[i].length)))
            {
                error_status = idigi_invalid_data_size;
            }
            else
            {
                switch (idigi_config_request_ids[i].request)
                {
                case idigi_config_device_id:
                   idigi_handle->device_id = data;
                    break;
                case idigi_config_vendor_id:
                   idigi_handle->vendor_id = data;
                    break;
                case idigi_config_device_type:
                    idigi_handle->device_type = data;
                    break;
                case idigi_config_server_url:
                case idigi_config_connection_type:
                case idigi_config_mac_addr:
                case idigi_config_link_speed:
                case idigi_config_phone_number:
                case idigi_config_tx_keepalive:
                case idigi_config_rx_keepalive:
                case idigi_config_wait_count:
                case idigi_config_ip_addr:
                case idigi_config_error_status:
                case idigi_config_firmware_facility:
                case idigi_config_data_service:
                    ASSERT(0);
                    /* Get these in different modules */
                    break;
                }
                i++;
            }
        }

        if (error_status != idigi_success)
        {
            /* if error occurs, notify caller then exit the function.
             */
            notify_error_status(callback, idigi_class_config, request_id, error_status);
            status = idigi_callback_abort;
        }

        if (status != idigi_callback_continue)
        {
            DEBUG_PRINTF("idigi_init: base class_id request id = %d callback aborts\n", idigi_config_request_ids[i].request);
            free_data(idigi_handle, idigi_handle);
            idigi_handle = NULL;
            break;
        }
    }

    if (idigi_handle != NULL)
    {
        status = get_supported_facilities(idigi_handle);

    }

done:
    return (idigi_handle_t) idigi_handle;
}

idigi_status_t idigi_step(idigi_handle_t const handle)
{
    idigi_status_t rc = idigi_init_error;
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_t * idigi_handle = (idigi_data_t *)handle;

    ASSERT_GOTO(handle != NULL, done);

    if (idigi_handle->active_state != idigi_device_started)
    {
        goto done;
    }
    rc = idigi_success;

    /* process edp layers.
     *
     * States 1 to 5 are layers to establish communication between iDigi
     * and iDigi server. State 6 is waiting and processing any messages
     * from server.
     *
     * 1. edp_init_layer
     * 2. edp_communication_layer
     * 3. edp_initialization_layer
     * 4. edp_security_layer
     * 5. edp_discovery_layer
     * 6. edp_facility_layer
     *
     * Each layer should not send data directly. Send process function is
     * called to send any data to server.
     *
     * Make sure send is not pending before execute any layer functions.
     *
     */
    if (!IS_SEND_PENDING(idigi_handle))
    {
        switch (idigi_handle->edp_state)
        {
        case edp_init_layer:
            status = configuration_layer(idigi_handle);
            break;
        case edp_communication_layer:
            status = communication_layer(idigi_handle);
            break;
        case edp_initialization_layer:
            status = initialization_layer(idigi_handle);
            break;
        case edp_security_layer:
            status = security_layer(idigi_handle);
            break;
        case edp_discovery_layer:
            status = discovery_layer(idigi_handle);
            break;
        case edp_facility_layer:
            status = facility_layer(idigi_handle);
            break;
        };
    }

    /* process any send data */
    if (status != idigi_callback_abort && idigi_handle->edp_connected)
    {
        status = send_packet_process(idigi_handle);
    }
    if (status == idigi_callback_abort)
    {
        rc = idigi_handle->error_code;
    }

done:
    if (rc != idigi_success && idigi_handle != NULL)
    {  /* error */
        status = close_server(idigi_handle);
        if (status != idigi_callback_busy)
        {

            if (idigi_handle->active_state == idigi_device_terminate)
            {   /*
                 * Terminated by idigi_dispatch call
                 * Free all memory.
                 */
                rc = idigi_device_terminated;

                status = remove_facility_layer(idigi_handle);
                if (status == idigi_callback_abort)
                {
                    rc = idigi_handle->error_code;
                }

                status = free_data(idigi_handle, idigi_handle);
                if (status != idigi_callback_continue)
                {
                    DEBUG_PRINTF("idigi_task: close_server returns abort");
                    rc = idigi_configuration_error;
                }
            }
            else
            {
                if (idigi_handle->error_code != idigi_success)
                {
                    rc = idigi_handle->error_code;
                }
                init_setting(idigi_handle);

            }
        }
        else
        {
            /* wait for close_server */
            idigi_handle->active_state = idigi_device_stop;
            rc = idigi_success;
        }
    }
    return rc;
}

idigi_status_t idigi_run(idigi_handle_t const handle)
{
    idigi_status_t rc = idigi_success;

    while (rc == idigi_success)
    {
        rc = idigi_step(handle);
    }

    return rc;
}


idigi_status_t idigi_initiate_action(idigi_handle_t handle, idigi_dispatch_request_t request, void const * request_data, void  * response_data)
{
    idigi_status_t rc = idigi_init_error;
    idigi_data_t * idigi_ptr = (idigi_data_t *)handle;

    ASSERT_GOTO(handle != NULL, done);

    UNUSED_PARAMETER(request_data);
    UNUSED_PARAMETER(response_data);

    rc = idigi_success;
    switch (request)
    {
    case idigi_dispatch_terminate:
        idigi_ptr->active_state = idigi_device_terminate;
        break;
    case idigi_dispatch_put_service:
        /* TODO: implement */
        break;
    }
done:
    return rc;
}

