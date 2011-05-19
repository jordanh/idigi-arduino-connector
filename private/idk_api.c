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

enum {
    idk_device_started,
    idk_device_stop,
    idk_device_terminate
};

idk_handle_t idk_init(idk_callback_t callback)
{
    idk_data_t * idk_handle = NULL;
    idk_callback_status_t status = idk_callback_abort;
    size_t length;
    unsigned i;
    void * data;
    size_t size;
    idk_request_t request_id;
    idk_status_t err_status;

    idk_base_request_t idk_base_request_ids[] = {
            idk_base_device_id, idk_base_vendor_id, idk_base_device_type
    };


    if (callback != NULL)
    {
        /* allocate idk data */
        size = sizeof(idk_data_t);

        status = malloc_cb(callback, size, (void **)&idk_handle);
        if (status != idk_callback_continue || idk_handle == NULL)
        {
            goto _ret;
        }

        init_setting(idk_handle);
        idk_handle->active_state = idk_device_started;
        idk_handle->callback = (idk_callback_t)callback;

        /* get device id, vendor id, & device type */
        i = 0;
        while (i < sizeof idk_base_request_ids/sizeof idk_base_request_ids[0])
        {
            request_id.base_request = idk_base_request_ids[i];
            status = idk_handle->callback(idk_class_base, request_id, NULL, 0, &data, &length);
            if (status == idk_callback_continue)
            {
                if (idk_base_request_ids[i] != idk_base_device_type && data == NULL)
                {
                    err_status = idk_invalid_data;
                    goto _err;
                }
                else if ((idk_base_request_ids[i] == idk_base_device_id && length != IDK_DEVICE_ID_LENGTH) ||
                    (idk_base_request_ids[i] == idk_base_vendor_id && length != IDK_VENDOR_ID_LENGTH) ||
                    (idk_base_request_ids[i] == idk_base_device_type && length > IDK_DEVICE_TYPE_LENGTH))
                {
                    err_status = idk_invalid_data_size;
_err:
                    status = notify_error_status(callback, idk_class_base, request_id, idk_invalid_data_size);
                }
                else
                {
                    switch (idk_base_request_ids[i])
                    {
                    case idk_base_device_id:
                        idk_handle->device_id = (uint8_t *)data;
                        break;
                    case idk_base_vendor_id:
                        idk_handle->vendor_id = (uint8_t *)data;
                        break;
                    case idk_base_device_type:
                        idk_handle->device_type = (char *)data;
                        break;
                    default:
                        break;

                    }
                    i++;
                }
            }

            if (status != idk_callback_continue)
            {
                DEBUG_PRINTF("idk_init: base class request id = %d callback aborts\n", idk_base_request_ids[i]);
                free_data(idk_handle, idk_handle);
                idk_handle = NULL;
                break;
            }
        }

    }

_ret:
    return (idk_handle_t) idk_handle;
}

idk_status_t idk_step(idk_handle_t const handle)
{
    idk_status_t rc = idk_success;
    idk_callback_status_t status = idk_callback_continue;
    idk_data_t * idk_handle = (idk_data_t *)handle;

    if (idk_handle == NULL || idk_handle->active_state != idk_device_started)
    {
        rc = idk_init_error;
        goto _ret;
    }

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
    if (!IDK_SEND_PENDING(idk_handle))
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
        status = net_send_packet(idk_handle);
    }
    if (status == idk_callback_abort)
    {
        rc = idk_handle->error_code;
    }

_ret:
    if (rc != idk_success && idk_handle != NULL)
    {  /* error */
        status = net_close(idk_handle);

        if (status != idk_callback_continue)
        {
            DEBUG_PRINTF("idk_task: net_close returns abort");
        }
        init_setting(idk_handle);

        if (idk_handle->active_state == idk_device_terminate)
        {   /* terminated by idk_dispatch call */
            status = remove_facility_layer(idk_handle);
            if (status == idk_callback_abort)
            {
                rc = idk_handle->error_code;
            }

            status = free_data(idk_handle, idk_handle);
           if (status != idk_callback_continue)
            {
                DEBUG_PRINTF("idk_task: net_close returns abort");
                rc = idk_configuration_error;
            }
           rc = idk_device_terminated;

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
    idk_status_t rc = idk_success;
    idk_data_t * idk_ptr = (idk_data_t *)handle;

    (void)data;

    switch (request)
    {
    case idk_dispatch_terminate:
        idk_ptr->active_state = idk_device_terminate;
        break;
    default:
        break;
    }

    return rc;
}

