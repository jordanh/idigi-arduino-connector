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
#include "idk_api.h"
#include "idk_def.h"
#include "bele.h"
#include "os_intf.c"
#include "network_intf.c"
#include "idk_cc.c"
#include "idk_loopbk.c"
#include "idk_fw.c"
#include "idk_rci.c"
#include "layer.c"

idk_handle_t idk_init(idk_callback_t callback)
{
#define INIT_REQUEST_ID_COUNT   3

    idk_data_t * idk_handle = NULL;
    idk_callback_status_t status = idk_callback_abort;
    size_t length;
    unsigned i;
    void * data;
    size_t size;
    idk_request_t request_id;
    void * handle;

    struct {
        idk_base_request_t request;
        size_t length;
    } idk_base_request_ids[INIT_REQUEST_ID_COUNT] = {
            {idk_base_device_id, DEVICE_ID_LENGTH},
            {idk_base_vendor_id, VENDOR_ID_LENGTH},
            {idk_base_device_type, DEVICE_TYPE_LENGTH}
    };

    ASSERT(callback != NULL);

    if (callback != NULL)
    {
        /* allocate idk data */
        size = sizeof(idk_data_t);

        status = malloc_cb(callback, size, &handle);
        if (status != idk_callback_continue || handle == NULL)
        {
            goto done;
        }

        idk_handle = handle;

        init_setting(idk_handle);
        idk_handle->active_state = idk_device_started;
        idk_handle->callback = callback;
        idk_handle->facility_list = NULL;
        idk_handle->network_handle = NULL;
        idk_handle->facilities = 0;
        idk_handle->network_busy = false;

        /* get device id, vendor id, & device type */
        i = 0;
        while (i < INIT_REQUEST_ID_COUNT)
        {
            request_id.base_request = idk_base_request_ids[i].request;
            status = idk_callback(idk_handle->callback, idk_class_base, request_id, NULL, 0, &data, &length);

            if (status == idk_callback_continue)
            {
                /* if error occurs, notify caller and go back to the callback for
                 * caller fixing the problem.
                 */
                if (data == NULL)
                {
                    status = notify_error_status(callback, idk_class_base, request_id, idk_invalid_data);
                }
                else if ((idk_base_request_ids[i].request != idk_base_device_type) && (length != idk_base_request_ids[i].length))
                {
                    status = notify_error_status(callback, idk_class_base, request_id, idk_invalid_data_size);
                }
                else if ((idk_base_request_ids[i].request == idk_base_device_type) &&
                        ((length == 0) || (length > idk_base_request_ids[i].length)))
                {
                    status = notify_error_status(callback, idk_class_base, request_id, idk_invalid_data_size);
                }
                else
                {
                    switch (idk_base_request_ids[i].request)
                    {
                    case idk_base_device_id:
                       idk_handle->device_id = data;
                        break;
                    case idk_base_vendor_id:
                       idk_handle->vendor_id = data;
                        break;
                    case idk_base_device_type:
                        idk_handle->device_type = data;
                        break;
                    case idk_base_server_url:
                    case idk_base_password:
                    case idk_base_connection_type:
                    case idk_base_mac_addr:
                    case idk_base_link_speed:
                    case idk_base_phone_number:
                    case idk_base_tx_keepalive:
                    case idk_base_rx_keepalive:
                    case idk_base_wait_count:
                    case idk_base_ip_addr:
                    case idk_base_error_status:
                    case idk_base_disconnected:
                    case idk_base_connect:
                    case idk_base_send:
                    case idk_base_receive:
                    case idk_base_close:
                    case idk_base_malloc:
                    case idk_base_free:
                    case idk_base_system_time:
                    case idk_base_firmware_facility:
                        ASSERT(0);
                        /* Get these in different modules */
                        break;
                    }
                    i++;
                }
            }

            if (status != idk_callback_continue)
            {
                DEBUG_PRINTF("idk_init: base class request id = %d callback aborts\n", idk_base_request_ids[i].request);
                free_data(idk_handle, idk_handle);
                idk_handle = NULL;
                break;
            }
        }

    }

done:
    return (idk_handle_t) idk_handle;
}

idk_status_t idk_step(idk_handle_t const handle)
{
    idk_status_t rc = idk_init_error;
    idk_callback_status_t status = idk_callback_continue;
    idk_data_t * idk_handle = (idk_data_t *)handle;

    ASSERT_GOTO(handle != NULL, done);

    if (idk_handle->active_state != idk_device_started)
    {
        goto done;
    }
    rc = idk_success;

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
    if (!IS_SEND_PENDING(idk_handle))
    {
        switch (idk_handle->edp_state)
        {
        case edp_init_layer:
            status = configuration_layer(idk_handle);
            break;
        case edp_communication_layer:
            status = communication_layer(idk_handle);
            break;
        case edp_initialization_layer:
            status = initialization_layer(idk_handle);
            break;
        case edp_security_layer:
            status = security_layer(idk_handle);
            break;
        case edp_discovery_layer:
            status = discovery_layer(idk_handle);
            break;
        case edp_facility_layer:
            status = facility_layer(idk_handle);
            break;
        };
    }

    /* process any send data */
    if (status != idk_callback_abort && idk_handle->edp_connected)
    {
        status = send_packet_process(idk_handle);
    }
    if (status == idk_callback_abort)
    {
        rc = idk_handle->error_code;
    }

done:
    if (rc != idk_success && idk_handle != NULL)
    {  /* error */
        status = close_server(idk_handle);
        if (status != idk_callback_busy)
        {

            if (idk_handle->active_state == idk_device_terminate)
            {   /*
                 * Terminated by idk_dispatch call
                 * Free all memory.
                 */
                status = remove_facility_layer(idk_handle);
                if (status == idk_callback_abort)
                {
                    rc = idk_handle->error_code;
                }

                status = free_data(idk_handle, idk_handle);
                if (status != idk_callback_continue)
                {
                    DEBUG_PRINTF("idk_task: close_server returns abort");
                    rc = idk_configuration_error;
                }
               rc = idk_device_terminated;
            }
            else
            {
                if (idk_handle->error_code != idk_success)
                {
                    rc = idk_handle->error_code;
                }
                init_setting(idk_handle);

            }
        }
        else
        {
            /* wait for close_server */
            idk_handle->active_state = idk_device_stop;
            rc = idk_success;
        }
    }
    return rc;
}

idk_status_t idk_run(idk_handle_t const handle)
{
    idk_status_t rc = idk_success;

    while (rc == idk_success)
    {
        rc = idk_step(handle);
    }

    return rc;
}


idk_status_t idk_dispatch(idk_handle_t handle, idk_dispatch_request_t request, void * data)
{
    idk_status_t rc = idk_init_error;
    idk_data_t * idk_ptr = (idk_data_t *)handle;

    ASSERT_GOTO(handle != NULL, done);

    UNUSED_PARAMETER(data);

    rc = idk_success;
    switch (request)
    {
    case idk_dispatch_terminate:
        idk_ptr->active_state = idk_device_terminate;
        break;
    case idk_dispatch_put_service:
        /* TODO: implement */
        break;
    }
done:
    return rc;
}

