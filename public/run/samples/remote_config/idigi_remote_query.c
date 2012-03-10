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
#include <stdio.h>
#include "idigi_api.h"
#include "remote_config.h"
#include "idigi_remote_sci.h"


unsigned int print_group_element_value(idigi_group_element_t const * const element_ptr, idigi_element_value_t const * const value_ptr)
{
    unsigned int error_id = idigi_success;

    switch (element_ptr->type)
    {
    case idigi_element_type_datetime:
    case idigi_element_type_ipv4:
    case idigi_element_type_fqdnv4:
    case idigi_element_type_fqdnv6:
     case idigi_element_type_string:
    case idigi_element_type_multiline_string:
    case idigi_element_type_password:
        if (element_ptr->value_limit != NULL)
        {
            if (value_ptr->string_value.length_in_bytes < element_ptr->value_limit->string_value.min_length_in_bytes ||
                value_ptr->string_value.length_in_bytes > element_ptr->value_limit->string_value.max_length_in_bytes)
            {
                goto error;
            }
        }
        if (value_ptr->string_value.length_in_bytes > 0)
        {
            printf("%.*s",value_ptr->string_value.length_in_bytes, value_ptr->string_value.buffer);
        }
        break;
    case idigi_element_type_int32:
        if (element_ptr->value_limit != NULL)
        {
            if (value_ptr->integer_signed_value < element_ptr->value_limit->integer_signed_value.min_value ||
                value_ptr->integer_signed_value > element_ptr->value_limit->integer_signed_value.max_value)
            {
                goto error;
            }
        }
        printf("%d", value_ptr->integer_signed_value);
        break;
    case idigi_element_type_uint32:
    case idigi_element_type_hex32:
    case idigi_element_type_0xhex:
    {
        enum {
            FORMAT_UINT_INDEX,
            FORMAT_HEX_INDEX,
            FORMAT_0XHEX_INDEX
        };
        char const * const format_integer[] = {"%d", "%X", "0x%X"};
        int i = FORMAT_UINT_INDEX;

        if (element_ptr->value_limit != NULL)
        {
            if (value_ptr->integer_unsigned_value < element_ptr->value_limit->integer_unsigned_value.min_value ||
                value_ptr->integer_unsigned_value > element_ptr->value_limit->integer_unsigned_value.max_value)
            {
                goto error;
            }
        }
        if (element_ptr->type == idigi_element_type_hex32) i = FORMAT_HEX_INDEX;
        else if (element_ptr->type == idigi_element_type_0xhex) i = FORMAT_0XHEX_INDEX;

        printf(format_integer[i], value_ptr->integer_unsigned_value);
        break;
    }
    case idigi_element_type_float:
        if (element_ptr->value_limit != NULL)
        {
            if (value_ptr->float_value < element_ptr->value_limit->float_value.min_value ||
                value_ptr->float_value > element_ptr->value_limit->float_value.max_value)
            {
                goto error;
            }
        }
        printf("%f", value_ptr->float_value);
        break;
    case idigi_element_type_enum:
        ASSERT(element_ptr->value_limit != NULL)
        if (value_ptr->enum_value > element_ptr->value_limit->enum_value.count)
        {
            goto error;
        }
        print_xml_value(element_ptr->value_limit->enum_value.value[value_ptr->enum_value]);
        break;
    case idigi_element_type_on_off:
    {
        int const i = (value_ptr->on_off_value == idigi_on) ? ON_STRING_INDEX : OFF_STRING_INDEX;

        print_xml_value(&idigi_all_strings[i]);
        break;
    }
    case idigi_element_type_boolean:
    {
        int const i = (value_ptr->boolean_value == idigi_boolean_true) ? TRUE_STRING_INDEX : FALSE_STRING_INDEX;

        print_xml_value(&idigi_all_strings[i]);
        break;
    }
    }
    goto done;

error:
    /* should call the error status callback */
    error_id = idigi_group_error_unknown_value;

done:
    return error_id;
}

idigi_callback_status_t idigi_remote_query_handler(idigi_remote_data_t * remote_ptr)
{
    idigi_callback_status_t status;

    idigi_remote_group_response_t * const response_data = &remote_ptr->response_data;
    idigi_remote_group_request_t * const request_data = &remote_ptr->request_data;

    idigi_group_t const * group_ptr = NULL;
    unsigned int group_id;
    char * element_name = NULL;

    size_t group_count;
    idigi_group_t const * group_table;

    switch(request_data->group_type)
    {
    case idigi_remote_group_config:
        group_count = idigi_config_group_count;
        group_table = idigi_config_groups;
        break;
    case idigi_remote_group_sysinfo:
        group_count = idigi_sysinfo_group_count;
        group_table = idigi_sysinfo_groups;
        break;
    }

    request_data->action = idigi_remote_action_query;

    for (group_id = 0; group_id < group_count; group_id++)
    {
        unsigned int index;

        group_ptr = &group_table[group_id];

        for (index = group_ptr->start_index; index <= group_ptr->end_index; index++)
        {
            idigi_request_t request_id;
            unsigned int element_index;
            size_t length = sizeof *response_data;

            request_data->group_id = group_id;
            request_data->group_index = index;
            request_data->element_value = NULL;

            response_data->error_id = idigi_success;
            response_data->element_data.error_hint = NULL;

            request_id.remote_config_request = idigi_remote_config_group_start;
            status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                                        request_data, sizeof *request_data, response_data, &length);
            if (status != idigi_callback_continue)
            {
                goto done;
            }

            printf("\n");
            if (group_ptr->start_index != group_ptr->end_index)
            {
                print_xml_open_index(group_ptr->name, index);
            }
            else
            {
                print_xml_open(group_ptr->name);
            }

            if (response_data->error_id != idigi_success)
            {
                    goto error;
            }

            for (element_index=0; element_index < group_ptr->elements.count; element_index++)
            {
                idigi_element_value_t element_value;

                idigi_group_element_t const * const element_ptr = &group_ptr->elements.data[element_index];
                request_data->element_id = element_index;
                request_data->element_type = element_ptr->type;

                response_data->error_id = idigi_success;
                response_data->element_data.element_value = &element_value;
                length += sizeof element_value;
                request_id.remote_config_request = idigi_remote_config_group_process;
                status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                        request_data, sizeof *request_data, response_data, &length);
                if (status != idigi_callback_continue)
                {
                    goto done;
                }

                printf("\n\t");
                print_xml_open(element_ptr->name);

                if (response_data->error_id != idigi_success)
                {
                    element_name = (char *) element_ptr->name;
                    goto error;
                }

                response_data->error_id = print_group_element_value(element_ptr, &element_value);
                if (response_data->error_id != idigi_success)
                {
                    response_data->element_data.error_hint = NULL;
                    goto error;
                }
                printf("\n\t");
                print_xml_close(element_ptr->name);

            }

            response_data->error_id = idigi_success;
            response_data->element_data.error_hint = NULL;

            request_id.remote_config_request = idigi_remote_config_group_end;
            status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                                        request_data, sizeof *request_data, response_data, &length);
            if (status != idigi_callback_continue)
            {
                goto done;
            }

            if (response_data->error_id != idigi_success)
            {
                goto error;
            }

            printf("\n");
            print_xml_close(group_ptr->name);
        }
        printf("\n");
    }
    goto done;

error:
    print_xml_error(group_ptr->errors.description, group_ptr->errors.count, response_data);
    if (element_name != NULL)
    {
        print_xml_close(element_name);

    }
    print_xml_close(group_ptr->name);

done:
    return status;

}


