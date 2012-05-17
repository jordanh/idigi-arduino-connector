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

enum {
    parity_none,
    parity_odd,
    parity_even
};

enum {
    xbreak_off,
    xbreak_on
};
#define SERIAL_COUNT    2

#define SERIAL_INVALID_STORED_VALUE_HINT "Invalid value"
#define SERIAL_NO_MEMORY_HINT            "Memory"

typedef struct {
    unsigned int baud;
    unsigned int parity;
    unsigned int databits;
    unsigned char xbreak_enable;
} serial_config_data_t;

static uint32_t serial_txbytes[SERIAL_COUNT];

static serial_config_data_t serial_config_data[SERIAL_COUNT] = {
    {9600, parity_none, 8, xbreak_off},
    {115200, parity_none, 8, xbreak_off},
};

idigi_callback_status_t app_serial_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    void * ptr;
    remote_group_session_t * const session_ptr = response->user_context;
    serial_config_data_t * serial_ptr = NULL;
    int group_index = request->group.index -1;

    ASSERT(session_ptr != NULL);

    ptr = malloc(sizeof *serial_ptr);
    if (ptr == NULL)
    {
        response->error_id = idigi_global_error_memory_fail;
        goto done;
    }

    serial_ptr = ptr;

    *serial_ptr = serial_config_data[group_index];
done:
    session_ptr->group_context = serial_ptr;
    return idigi_callback_continue;
}

idigi_callback_status_t app_serial_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    remote_group_session_t * const session_ptr = response->user_context;
    serial_config_data_t * serial_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    serial_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_setting_serial_baud:
        switch (serial_ptr->baud)
        {
        case 2400:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_2400;
            break;
        case 4800:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_4800;
            break;
        case 9600:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_9600;
            break;
        case 19200:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_19200;
            break;
        case 38400:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_38400;
            break;
        case 57600:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_57600;
            break;
        case 115200:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_115200;
            break;
        case 230400:
            response->element_data.element_value->enum_value = idigi_setting_serial_baud_230400;
            break;
        default:
            response->error_id = idigi_setting_serial_error_invalid_baud;
            response->element_data.error_hint = SERIAL_INVALID_STORED_VALUE_HINT;
            break;
        }
        break;
    case idigi_setting_serial_parity:
        switch (serial_ptr->parity)
        {
        case parity_none:
            response->element_data.element_value->enum_value = idigi_setting_serial_parity_none;
            break;
        case parity_odd:
            response->element_data.element_value->enum_value = idigi_setting_serial_parity_odd;
            break;
        case parity_even:
            response->element_data.element_value->enum_value = idigi_setting_serial_parity_even;
            break;
        default:
            response->error_id = idigi_setting_serial_error_invalid_parity;
            response->element_data.error_hint = SERIAL_INVALID_STORED_VALUE_HINT;
            break;
        }
        break;
    case idigi_setting_serial_databits:
        response->element_data.element_value->integer_signed_value = serial_ptr->databits;
        break;
    case idigi_setting_serial_xbreak:
        response->element_data.element_value->on_off_value = (serial_ptr->xbreak_enable) ? idigi_on: idigi_off;
        break;
    case idigi_setting_serial_txbytes:
        response->element_data.element_value->integer_unsigned_value = serial_txbytes[request->group.index-1];
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_serial_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    remote_group_session_t * const session_ptr = response->user_context;
    serial_config_data_t * serial_ptr;


    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    serial_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_setting_serial_baud:
    {
        unsigned int const serial_supported_baud_rates[] = { 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400};

        switch (request->element.value->enum_value)
        {
        case idigi_setting_serial_baud_2400:
        case idigi_setting_serial_baud_4800:
        case idigi_setting_serial_baud_9600:
        case idigi_setting_serial_baud_19200:
        case idigi_setting_serial_baud_38400:
        case idigi_setting_serial_baud_57600:
        case idigi_setting_serial_baud_115200:
        case idigi_setting_serial_baud_230400:
            ASSERT(request->element.type == idigi_element_type_enum);
            serial_ptr->baud = serial_supported_baud_rates[request->element.value->enum_value];
            break;
        default:
            response->error_id = idigi_setting_serial_error_invalid_baud;
            break;
        }
        break;
    }
    case idigi_setting_serial_parity:
    {
        unsigned int const serial_supported_parity[] = {parity_none, parity_odd, parity_even};

        switch (request->element.value->enum_value)
        {
        case idigi_setting_serial_parity_none:
        case idigi_setting_serial_parity_odd:
        case idigi_setting_serial_parity_even:
            ASSERT(request->element.type == idigi_element_type_enum);
            serial_ptr->parity = serial_supported_parity[request->element.value->enum_value];
            break;
        default:
            response->error_id = idigi_setting_serial_error_invalid_parity;
            break;
        }
        break;
    }
    case idigi_setting_serial_databits:
        ASSERT(request->element.type == idigi_element_type_uint32);
        serial_ptr->databits = request->element.value->integer_unsigned_value;
        break;
    case idigi_setting_serial_xbreak:
        ASSERT(request->element.type == idigi_element_type_on_off);
        serial_ptr->xbreak_enable = (request->element.value->on_off_value == idigi_on) ? 1: 0;
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;

}

idigi_callback_status_t app_serial_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{

    remote_group_session_t * const session_ptr = response->user_context;
    serial_config_data_t * serial_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    serial_ptr = session_ptr->group_context;

    if (request->action == idigi_remote_action_set)
    {
        serial_config_data[request->group.index-1] = *serial_ptr;
    }

    if (serial_ptr != NULL)
    {
        free(serial_ptr);
    }
    return idigi_callback_continue;
}

void app_serial_group_cancel(void * const context)
{
    remote_group_session_t * const session_ptr = context;

    if (session_ptr != NULL)
    {
        free(session_ptr->group_context);
    }

}

