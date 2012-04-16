/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
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
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"
#include "remote_config_cb.h"

#define DEVICE_INFO_NO_MEMORY_HINT            "Memory"

#define DEVICE_INFO_PRODUCT_LENGTH  64
#define DEVICE_INFO_MODEL_LENGTH  32
#define DEVICE_INFO_COMPANY_LENGTH  64
#define DEVICE_INFO_DESC_LENGTH  128
#define DEVICE_INFO_SYSPWD_LENGTH  32

typedef struct {
    char product[DEVICE_INFO_PRODUCT_LENGTH];
    char model[DEVICE_INFO_MODEL_LENGTH];
    char company[DEVICE_INFO_COMPANY_LENGTH];
    char desc[DEVICE_INFO_DESC_LENGTH];
    size_t desc_length;
    char syspwd[DEVICE_INFO_SYSPWD_LENGTH];
} device_info_config_data_t;

device_info_config_data_t device_info_config_data = {"ICC Product", "\0", "Digi International Inc.", "ICC Demo on Linux\n"
        "with firmware ugrade, put service, device request and remote configuration supports", 102, "\0"};

idigi_callback_status_t app_device_info_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{

    void * ptr;
    remote_group_session_t * const session_ptr = response->user_context;
    device_info_config_data_t * device_info_ptr = NULL;

    UNUSED_ARGUMENT(request);

    ASSERT(session_ptr != NULL);

    if (app_os_malloc(sizeof *device_info_ptr, &ptr) != 0)
    {
        response->error_id = idigi_global_error_load_fail;
        response->element_data.error_hint = DEVICE_INFO_NO_MEMORY_HINT;
        goto done;
    }

    device_info_ptr = ptr;
    *device_info_ptr = device_info_config_data;

    session_ptr->group_context = device_info_ptr;
done:
    return idigi_callback_continue;
}

idigi_callback_status_t app_device_info_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;


    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    switch (request->element.id)
    {
    case idigi_setting_device_info_syspwd:
        ASSERT(request->element.type == idigi_element_type_password);

    case idigi_setting_device_info_product:
    case idigi_setting_device_info_model:
    case idigi_setting_device_info_company:
    case idigi_setting_device_info_desc:
    {
        device_info_config_data_t * const device_info_ptr = session_ptr->group_context;

        char * config_data[] = {device_info_ptr->product, device_info_ptr->model,
                                device_info_ptr->company, device_info_ptr->desc,
                                device_info_ptr->syspwd};

        response->element_data.element_value->string_value.buffer = config_data[request->element.id];
        if (request->element.id == idigi_setting_device_info_desc)
        {
            ASSERT(request->element.type == idigi_element_type_multiline_string);
            response->element_data.element_value->string_value.length_in_bytes = device_info_ptr->desc_length;
        }
        else
        {
            if (request->element.id != idigi_setting_device_info_syspwd)
            {
                ASSERT(request->element.type == idigi_element_type_string);
            }
            response->element_data.element_value->string_value.length_in_bytes = strlen(config_data[request->element.id]);
        }
        break;
    }
    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_device_info_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;


    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    switch (request->element.id)
    {
    case idigi_setting_device_info_product:
    case idigi_setting_device_info_model:
    case idigi_setting_device_info_company:
    case idigi_setting_device_info_desc:
    case idigi_setting_device_info_syspwd:
    {
        device_info_config_data_t * const device_info_ptr = session_ptr->group_context;

        struct {
            char * store_data;
            size_t min_length;
            size_t max_length;
        } config_data[] = {
                {device_info_ptr->product, 1, sizeof device_info_ptr->product},
                {device_info_ptr->model, 0, sizeof device_info_ptr->model},
                {device_info_ptr->company, 0, sizeof device_info_ptr->company},
                {device_info_ptr->desc, 0, sizeof device_info_ptr->desc},
                {device_info_ptr->syspwd, 0, sizeof device_info_ptr->syspwd},
        };


        if (request->element.value->string_value.length_in_bytes < config_data[request->element.id].min_length ||
            request->element.value->string_value.length_in_bytes > config_data[request->element.id].max_length)
        {
            response->error_id = idigi_setting_device_info_error_invalid_length;
            response->element_data.error_hint = NULL;
            goto done;
        }
        memcpy(config_data[request->element.id].store_data, request->element.value->string_value.buffer, request->element.value->string_value.length_in_bytes);
        config_data[request->element.id].store_data[request->element.value->string_value.length_in_bytes] = '\0';
        if (request->element.id == idigi_setting_device_info_desc)
        {
            device_info_ptr->desc_length = request->element.value->string_value.length_in_bytes;
        }
        break;
    }
    default:
        ASSERT(0);
        break;
    }
done:
    return status;
}

idigi_callback_status_t app_device_info_group_end(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{

    device_info_config_data_t * device_info_ptr;
    remote_group_session_t * const session_ptr = response->user_context;

    /* save the data */

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_info_ptr = session_ptr->group_context;

    if (request->action == idigi_remote_action_set)
    {
        device_info_config_data = *device_info_ptr;
    }

    if (device_info_ptr != NULL)
    {
        app_os_free(device_info_ptr);
    }
    return idigi_callback_continue;
}

void app_device_info_group_cancel(void * context)
{
    remote_group_session_t * const session_ptr = context;

    if (session_ptr != NULL)
    {
        app_os_free(session_ptr->group_context);
    }

}

