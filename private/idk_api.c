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
#include "os_intf.h"
#include "network_intf.h"
#include "idk_cc.h"
#include "idk_fw.h"
#include "idk_rci.h"
#include "layer.h"

enum {
    idk_device_started,
    idk_device_stop,
    idk_device_terminate
};
#define IDK_MAX_IDK_HANDLE  1

static idk_data_t * idkData = NULL;


idk_status_t idk_step(idk_handle_t const handle)
{
    idk_status_t rc = idk_success;
    idk_data_t * idk_handle = (idk_data_t *)handle;
    int sent_status = IDK_NETWORK_BUFFER_COMPLETE;

    if (idk_handle != NULL || idk_handle->active_state != idk_device_started)
    {
        rc = idk_init_error;
        goto _ret;
    }


    if (idk_handle->edp_connected)
    {
        rc = net_send_packet(idk_handle, &sent_status);
        if (rc != idk_success)
        {
            DEBUG_PRINTF("idk_task: send status returns %d\n", rc);
            goto _ret;
        }
    }

    if (sent_status == IDK_NETWORK_BUFFER_COMPLETE)
    {
        switch (idk_handle->edp_state)
        {
        case edp_init_layer:
            rc = configuration_layer(idk_handle);
            break;
        case edp_communication_layer:
            rc = communication_layer(idk_handle);
            break;
        case edp_initialization_layer:
            rc = initialization_layer(idk_handle);
            break;
        case edp_security_layer:
            rc = security_layer(idk_handle);
            break;
        case edp_discovery_layer:
            rc = discovery_layer(idk_handle);
            break;
        case edp_facility_layer:
            rc = facility_layer(idk_handle);
            break;
        };
    }

_ret:
    if (rc != idk_success)
    {
        idk_callback_status_t status;
        idk_request_t request_id;

        do {
            status = net_close(idk_handle);

            if (status == idk_callback_abort)
            {
                DEBUG_PRINTF("idk_task: net_close returns abort");
                rc = idk_close_error;
            }
            else if (status == idk_callback_busy)
            {
                /* TODO: need to delay */
            }
        } while (status == idk_callback_busy);

        init_setting(idk_handle);

        if (idk_handle->active_state == idk_device_terminate)
        {
            rc = remove_facility_layer(idk_handle);
            if (rc == idk_success)
            {
                rc = idk_device_terminated;
            }

            request_id.base_request = idk_base_free;

            do {
                status = idk_handle->callback(idk_class_base, request_id, idk_handle, sizeof idk_handle, NULL, NULL);
                if (status == idk_callback_abort)
                {
                    DEBUG_PRINTF("idk_task: net_close returns abort");
                    rc = idk_configuration_error;
                }
                else if (status == idk_callback_busy)
                {
                    /* TODO: need to delay */
                }
            } while (status == idk_callback_busy);

            idkData = NULL;

        }
    }
    return rc;
}

idk_handle_t idk_init(idk_callback_t callback)
{
    idk_data_t * idk_handle = NULL;
    idk_callback_status_t status = idk_callback_abort;
    size_t length;
    unsigned i;
    void * data;
    size_t size;
    idk_request_t request_id;

    idk_base_request_t idk_base_request_ids[] = {
            idk_base_device_id, idk_base_vendor_id, idk_base_device_type
    };


    if (callback != NULL)
    {
        /* allocate idk data */
        size = sizeof(idk_data_t);

        request_id.base_request = idk_base_malloc;
        status = callback(idk_class_base, request_id, &size, sizeof size, &idk_handle, NULL);
        if (status == idk_callback_abort)
        {
            goto _ret;
        }

        idkData = idk_handle;
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
                if ((idk_base_request_ids[i] == idk_base_device_id && length != IDK_DEVICE_ID_LENGTH) ||
                    (idk_base_request_ids[i] == idk_base_vendor_id && length != IDK_VENDOR_ID_LENGTH) ||
                    (idk_base_request_ids[i] == idk_base_device_type && length > IDK_DEVICE_TYPE_LENGTH))
                {
                    status = notify_error_status(callback, idk_class_base, request_id, idk_invalid_data_size);
                }
                else
                {
                    i++;
                }
            }

            if (status == idk_callback_abort)
            {
                DEBUG_PRINTF("idk_init: base class request id = %d callback aborts\n", idk_base_request_ids[i]);
                request_id.base_request = idk_base_free;
                callback(idk_class_base, request_id, idk_handle, sizeof idk_handle, NULL, NULL);
                idk_handle = idkData = NULL;
                break;
            }
        }

    }

_ret:
    return (idk_handle_t) idk_handle;
}


/* Stops or terminates IRL.
 *
 *
 *
 * @param handler       handler returned from idk_init.
 * @param stop_flag     IDK_STOP flag to stop IRL which IRL will disconnect iDigi server
 *                      and may be reconnect to iDigi server again.
 *                      IDK_TERMINATE flag to terminate IRL which IRL will disconnect iDigi
 *                      server and free all memory used. IRL cannot be restart again unless
 *                      idk_init is called again.
 *
 * @return idk_success      IRL was successfully stopped or terminated.
 * @return IDK_INIT_ERR     Invalid handle
 *
 *
 */
idk_status_t idk_dispatch(idk_handle_t handle, idk_dispatch_request_t request, void * data)
{
    idk_status_t rc = idk_success;
    idk_data_t * idk_ptr = (idk_data_t *)handle;

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
