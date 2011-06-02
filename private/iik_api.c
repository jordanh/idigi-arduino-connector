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
#include "iik_api.h"
#include "iik_def.h"
#include "bele.h"
#include "os_intf.c"
#include "network_intf.c"
#include "iik_cc.c"
#include "iik_loopbk.c"
#include "iik_fw.c"
#include "iik_rci.c"
#include "layer.c"

iik_handle_t iik_init(iik_callback_t const callback)
{
#define INIT_REQUEST_ID_COUNT   3

    iik_data_t * iik_handle = NULL;
    iik_callback_status_t status = iik_callback_abort;
    size_t length;
    unsigned i;
    void * data;
    size_t size;
    iik_request_t request_id;
    void * handle;

    struct {
        iik_config_request_t request;
        size_t length;
    } iik_config_request_ids[INIT_REQUEST_ID_COUNT] = {
            {iik_config_device_id, DEVICE_ID_LENGTH},
            {iik_config_vendor_id, VENDOR_ID_LENGTH},
            {iik_config_device_type, DEVICE_TYPE_LENGTH}
    };

    ASSERT_GOTO(callback != NULL, done);

    /* allocate idk data */
    size = sizeof(iik_data_t);

    status = malloc_cb(callback, size, &handle);
    if (status != iik_callback_continue || handle == NULL)
    {
        goto done;
    }

    iik_handle = handle;

    init_setting(iik_handle);
    iik_handle->active_state = iik_device_started;
    iik_handle->callback = callback;
    iik_handle->facility_list = NULL;
    iik_handle->network_handle = NULL;
    iik_handle->facilities = 0;
    iik_handle->network_busy = false;
    iik_handle->edp_connected = false;


    /* get device id, vendor id, & device type */
    i = 0;
    while (i < INIT_REQUEST_ID_COUNT)
    {
        request_id.config_request = iik_config_request_ids[i].request;
        status = iik_callback(iik_handle->callback, iik_class_config, request_id, NULL, 0, &data, &length);

        if (status == iik_callback_continue)
        {
            /* if error occurs, notify caller and go back to the callback for
             * caller fixing the problem.
             */
            if (data == NULL)
            {
                status = notify_error_status(callback, iik_class_config, request_id, iik_invalid_data);
            }
            else if ((iik_config_request_ids[i].request != iik_config_device_type) && (length != iik_config_request_ids[i].length))
            {
                status = notify_error_status(callback, iik_class_config, request_id, iik_invalid_data_size);
            }
            else if ((iik_config_request_ids[i].request == iik_config_device_type) &&
                    ((length == 0) || (length > iik_config_request_ids[i].length)))
            {
                status = notify_error_status(callback, iik_class_config, request_id, iik_invalid_data_size);
            }
            else
            {
                switch (iik_config_request_ids[i].request)
                {
                case iik_config_device_id:
                   iik_handle->device_id = data;
                    break;
                case iik_config_vendor_id:
                   iik_handle->vendor_id = data;
                    break;
                case iik_config_device_type:
                    iik_handle->device_type = data;
                    break;
                case iik_config_server_url:
                case iik_config_password:
                case iik_config_connection_type:
                case iik_config_mac_addr:
                case iik_config_link_speed:
                case iik_config_phone_number:
                case iik_config_tx_keepalive:
                case iik_config_rx_keepalive:
                case iik_config_wait_count:
                case iik_config_ip_addr:
                case iik_config_error_status:
                case iik_config_disconnected:
                case iik_config_firmware_facility:
                    ASSERT(0);
                    /* Get these in different modules */
                    break;
                }
                i++;
            }
        }

        if (status != iik_callback_continue)
        {
            DEBUG_PRINTF("iik_init: base class request id = %d callback aborts\n", iik_config_request_ids[i].request);
            free_data(iik_handle, iik_handle);
            iik_handle = NULL;
            break;
        }
    }

done:
    return (iik_handle_t) iik_handle;
}

iik_status_t iik_step(iik_handle_t const handle)
{
    iik_status_t rc = iik_init_error;
    iik_callback_status_t status = iik_callback_continue;
    iik_data_t * iik_handle = (iik_data_t *)handle;

    ASSERT_GOTO(handle != NULL, done);

    if (iik_handle->active_state != iik_device_started)
    {
        goto done;
    }
    rc = iik_success;

    /* process edp layers.
     *
     * States 1 to 5 are layers to establish communication between IDK
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
     * Each layer should not send data directly. This function will process any data
     * that needs to be sent.
     *
     * Make sure send is not pending before execute the layer.
     *
     */
    if (!IS_SEND_PENDING(iik_handle))
    {
        switch (iik_handle->edp_state)
        {
        case edp_init_layer:
            status = configuration_layer(iik_handle);
            break;
        case edp_communication_layer:
            status = communication_layer(iik_handle);
            break;
        case edp_initialization_layer:
            status = initialization_layer(iik_handle);
            break;
        case edp_security_layer:
            status = security_layer(iik_handle);
            break;
        case edp_discovery_layer:
            status = discovery_layer(iik_handle);
            break;
        case edp_facility_layer:
            status = facility_layer(iik_handle);
            break;
        };
    }

    /* process any send data */
    if (status != iik_callback_abort && iik_handle->edp_connected)
    {
        status = send_packet_process(iik_handle);
    }
    if (status == iik_callback_abort)
    {
        rc = iik_handle->error_code;
    }

done:
    if (rc != iik_success && iik_handle != NULL)
    {  /* error */
        status = close_server(iik_handle);
        if (status != iik_callback_busy)
        {

            if (iik_handle->active_state == iik_device_terminate)
            {   /*
                 * Terminated by iik_dispatch call
                 * Free all memory.
                 */
                status = remove_facility_layer(iik_handle);
                if (status == iik_callback_abort)
                {
                    rc = iik_handle->error_code;
                }

                status = free_data(iik_handle, iik_handle);
                if (status != iik_callback_continue)
                {
                    DEBUG_PRINTF("iik_task: close_server returns abort");
                    rc = iik_configuration_error;
                }
               rc = iik_device_terminated;
            }
            else
            {
                if (iik_handle->error_code != iik_success)
                {
                    rc = iik_handle->error_code;
                }
                init_setting(iik_handle);

            }
        }
        else
        {
            /* wait for close_server */
            iik_handle->active_state = iik_device_stop;
            rc = iik_success;
        }
    }
    return rc;
}

iik_status_t iik_run(iik_handle_t const handle)
{
    iik_status_t rc = iik_success;

    while (rc == iik_success)
    {
        rc = iik_step(handle);
    }

    return rc;
}


iik_status_t iik_initiate_action(iik_handle_t handle, iik_dispatch_request_t request, void const * request_data, void  * response_data)
{
    iik_status_t rc = iik_init_error;
    iik_data_t * iik_ptr = (iik_data_t *)handle;

    ASSERT_GOTO(handle != NULL, done);

    UNUSED_PARAMETER(request_data);
    UNUSED_PARAMETER(response_data);

    rc = iik_success;
    switch (request)
    {
    case iik_dispatch_terminate:
        iik_ptr->active_state = iik_device_terminate;
        break;
    case iik_dispatch_put_service:
        /* TODO: implement */
        break;
    }
done:
    return rc;
}

