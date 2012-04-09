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
#if (IDIGI_VERSION >= 0x1010000UL)
  #include "idigi_config.h"
#else
  #include "options.h"
#endif

#include "idigi_api.h"
#include "idigi_def.h"
#include "chk_config.h"
#include "bele.h"
#include "os_intf.h"
#include "network_intf.h"
#include "idigi_cc.h"
#if defined(IDIGI_FIRMWARE_SERVICE)
#include "idigi_fw.h"
#endif
#if defined(IDIGI_DATA_SERVICE) || defined(IDIGI_FILE_SYSTEM) || defined(IDIGI_REMOTE_CONFIGURATION)
#include "idigi_msg.h"
#endif
#if defined(IDIGI_DATA_SERVICE)
#if (IDIGI_VERSION < IDIGI_VERSION_1100)
#include "idigi_ds_legacy.h"
#else
#include "idigi_ds.h"
#endif
#endif
#if defined(IDIGI_FILE_SYSTEM)
#include "idigi_fs.h"
#endif
#include "layer.h"

idigi_handle_t idigi_init(idigi_callback_t const callback)
{

    idigi_data_t * idigi_handle = NULL;
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_status_t error_status = idigi_success;
    unsigned int i;


    static const struct {
        idigi_config_request_t request;
        size_t min_length;
        size_t max_length;
    } idigi_config_request_ids[] = {
#if !defined(IDIGI_VENDOR_ID)
            {idigi_config_vendor_id, VENDOR_ID_LENGTH, VENDOR_ID_LENGTH},
#endif
#if !defined(IDIGI_DEVICE_TYPE)
            {idigi_config_device_type, 1, DEVICE_TYPE_LENGTH},
#endif
            {idigi_config_device_id, DEVICE_ID_LENGTH, DEVICE_ID_LENGTH}
    };

    ASSERT_GOTO(callback != NULL, done);

    {
        void * handle;

        /* allocate idk data */
        status = malloc_cb(callback, sizeof *idigi_handle, &handle);
        if (status != idigi_callback_continue)
        {
            goto done;
        }

        ASSERT_GOTO(handle != NULL, done);
        idigi_handle = handle;
    }

    reset_initial_data(idigi_handle);
    idigi_handle->callback = callback;
    idigi_handle->facility_list = NULL;
    idigi_handle->facilities = 0;
    idigi_handle->network_busy = idigi_false;
    idigi_handle->edp_connected = idigi_false;
    idigi_handle->network_connected = idigi_false;
    idigi_handle->active_state = idigi_device_started;
    idigi_handle->error_code = idigi_success;

    /* get device id, vendor id, & device type */
    for (i=0; i < asizeof(idigi_config_request_ids); i++)
    {
        size_t length = 0;
        void * data;
        idigi_request_t request_id;
        void * * store_at = NULL;

        switch (idigi_config_request_ids[i].request)
        {
        case idigi_config_device_id:
            data = &idigi_handle->device_id;
            store_at = (void **)data;
            break;
#if !defined(IDIGI_VENDOR_ID)
        case idigi_config_vendor_id:
            data = &idigi_handle->vendor_id;
            store_at = (void **)data;
            break;
#endif
#if !defined(IDIGI_DEVICE_TYPE)
        case idigi_config_device_type:
            data = &idigi_handle->device_type;
            store_at = (void **)data;
            break;
#endif
        default:
            ASSERT(idigi_false);
            break;
        }
        request_id.config_request = idigi_config_request_ids[i].request;
        status = idigi_callback_no_request_data(idigi_handle->callback, idigi_class_config, request_id, &data, &length);

        *store_at = data;

        switch (status)
        {
        case idigi_callback_continue:
            if (data == NULL)
            {
                error_status = idigi_invalid_data;
                goto error;
            }
            if ((length < idigi_config_request_ids[i].min_length )||
                (length > idigi_config_request_ids[i].max_length))
            {
                error_status = idigi_invalid_data_size;
                goto error;
            }

#if !defined(IDIGI_DEVICE_TYPE)
            if (idigi_config_request_ids[i].request == idigi_config_device_type)
            {
                idigi_handle->device_type_length = length;
            }
#endif
            break;
        case idigi_callback_abort:
        case idigi_callback_unrecognized:
            goto error;

        case idigi_callback_busy:
            break;
        }
    }

    if (idigi_handle != NULL)
    {
        status = layer_get_supported_facilities(idigi_handle);
        if (status != idigi_callback_continue)
        {
            status = layer_remove_facilities(idigi_handle, facility_callback_delete);
            ASSERT(status != idigi_callback_continue);
            goto error;
        }
        else
        {
            goto done;
        }
    }

error:
    if (error_status != idigi_success)
    {
        idigi_request_t request_id;

        request_id.config_request = idigi_config_request_ids[i].request;
        /* if error occurs, notify caller then exit the function.
         */
        idigi_debug("idigi_init: base class_id request id = %d callback aborts\n", idigi_config_request_ids[i].request);
        notify_error_status(callback, idigi_class_config, request_id, error_status);
    }

    if (idigi_handle != NULL)
    {
        free_data(idigi_handle, idigi_handle);
        idigi_handle = NULL;
    }

done:
    return idigi_handle;
}

idigi_status_t idigi_step(idigi_handle_t const handle)
{
    idigi_status_t result = idigi_init_error;
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_data_t * const idigi_handle = handle;

    ASSERT_GOTO(handle != NULL, done);

    if (idigi_handle->active_state != idigi_device_started)
    {
        goto error;
    }
    result = idigi_success;


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
     * Make sure send is not pending before execute any layer functions
     * except facility layer which processes each facility.
     */
    if ((!IS_SEND_PENDING(idigi_handle)) ||
        (idigi_handle->edp_state == edp_facility_layer))
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
    if (status == idigi_callback_continue || status == idigi_callback_busy)
    {
        status = send_packet_process(idigi_handle);
    }

    switch (status)
    {
    case idigi_callback_abort:
    case idigi_callback_unrecognized:
        result = idigi_configuration_error;
        goto error;
    case idigi_callback_continue:
    case idigi_callback_busy:
        goto done;
    }

error:
    if (idigi_handle != NULL)
    {  /* error */
        status = close_server(idigi_handle);
        if (status != idigi_callback_busy)
        {
            if (idigi_handle->active_state == idigi_device_terminate)
            {   /*
                 * Terminated by idigi_dispatch call
                 * Free all memory.
                 */
                result = idigi_device_terminated;

                status = layer_remove_facilities(idigi_handle, facility_callback_delete);
                if (status == idigi_callback_abort)
                {
                    result = idigi_handle->error_code;
                }

                free_data(idigi_handle, idigi_handle);
            }
            else
            {
                if (idigi_handle->error_code != idigi_success)
                {
                    result = idigi_handle->error_code;
                    idigi_handle->error_code = idigi_success;

                }
            }
        }
        else
        {
            /* wait for close_server */
            idigi_handle->active_state = idigi_device_stop;
            result = idigi_success;
        }
    }

done:
    return result;
}

idigi_status_t idigi_run(idigi_handle_t const handle)
{
    idigi_status_t rc;

    do {
        rc = idigi_step(handle);
        sleep_timeout(handle);

    } while (rc == idigi_success);

    return rc;
}

idigi_status_t idigi_initiate_action(idigi_handle_t const handle, idigi_initiate_request_t const request, void const * const request_data, void  * const response_data)
{
    idigi_status_t result = idigi_init_error;
    idigi_data_t * idigi_ptr = (idigi_data_t *)handle;

    ASSERT_GOTO(handle != NULL, error);

    switch (idigi_ptr->active_state)
    {
    case idigi_device_started:
        break;

    case idigi_device_stop:
        result = idigi_server_disconnected;
        goto error;

    case idigi_device_terminate:
        result = idigi_device_terminated;
        /* fall thru */
    default:
        goto error;
    }

    switch (request)
    {
    case idigi_initiate_terminate:
        UNUSED_PARAMETER(request_data);
        UNUSED_PARAMETER(response_data);
        idigi_ptr->active_state = idigi_device_terminate;
        result = idigi_success;
        break;

#if defined(IDIGI_DATA_SERVICE)
    case idigi_initiate_data_service:
        if (idigi_ptr->edp_connected)
        {
            result = data_service_initiate(idigi_ptr, request_data, response_data);
        }
        break;
#endif

    default:
        ASSERT(idigi_false);
        result = idigi_invalid_data;
        break;
    }

error:
    return result;
}

