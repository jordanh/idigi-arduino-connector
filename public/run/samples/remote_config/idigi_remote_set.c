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
#include "platform.h"
#include "remote_config.h"
#include "idigi_remote_sci.h"

extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

static void set_group_element_value(unsigned int group_id, unsigned int group_index,
                            unsigned int element_id,
                            idigi_group_element_t const * const element_ptr,
                            idigi_element_value_t * const element_value)
{
    UNUSED_ARGUMENT(group_index);

    switch (group_id)
    {
    case idigi_group_serial:
        switch (element_id)
        {
        case idigi_group_serial_baud:
            ASSERT(element_ptr->type != idigi_element_type_enum);
            element_value->enum_value = idigi_group_serial_baud_19200;
            break;
        case idigi_group_serial_parity:
            ASSERT(element_ptr->type != idigi_element_type_enum);
            element_value->enum_value = idigi_group_serial_parity_even;
            break;
        case idigi_group_serial_databits:
            ASSERT(element_ptr->type != idigi_element_type_uint32);
            element_value->integer_unsigned_value = element_ptr->value_limit->integer_unsigned_value.max_value -1;
            break;
        case idigi_group_serial_xbreak:
            ASSERT(element_ptr->type != idigi_element_type_on_off);
            element_value->on_off_value = idigi_on;
            break;
        default:
            ASSERT(0);
        }
        break;
    case idigi_group_ethernet:
        switch (element_id)
        {
        case idigi_group_ethernet_ip:
            ASSERT(element_ptr->type != idigi_element_type_ipv4);
            static char set_ip_address[] = "10.52.18.75";
            element_value->string_value.buffer = (char *)set_ip_address;
            element_value->string_value.length_in_bytes = sizeof set_ip_address -1;
            break;

        case idigi_group_ethernet_subnet:
            ASSERT(element_ptr->type != idigi_element_type_ipv4);
            static char set_subnet_address[] = "255.255.255.0";
            element_value->string_value.buffer = (char *)set_subnet_address;
            element_value->string_value.length_in_bytes = sizeof set_subnet_address -1;
            break;

        case idigi_group_ethernet_gateway:
            ASSERT(element_ptr->type != idigi_element_type_ipv4);
            static char set_gateway_address[] = "10.52.18.1";
            element_value->string_value.buffer = (char *)set_gateway_address;
            element_value->string_value.length_in_bytes = sizeof set_gateway_address -1;
            break;
        case idigi_group_ethernet_dhcp:
            ASSERT(element_ptr->type != idigi_element_type_boolean);
            element_value->boolean_value = idigi_boolean_false;
            break;

        case idigi_group_ethernet_dns:
            ASSERT(element_ptr->type != idigi_element_type_fqdnv4);
            static char set_dns_address[] = "digi.com";
            element_value->string_value.buffer = (char *)set_dns_address;
            element_value->string_value.length_in_bytes = sizeof set_dns_address -1;
            break;
        case idigi_group_ethernet_duplex:
            ASSERT(element_ptr->type != idigi_element_type_enum);
            element_value->enum_value = idigi_group_ethernet_duplex_full;
            break;
        default:
            ASSERT(0);
            break;

        }
        break;
    case idigi_group_device_stats:
        switch (element_id)
        {
        case idigi_group_device_stats_curtime:
            ASSERT(element_ptr->type != idigi_element_type_datetime);
            static char set_time_string[] = "2012-03-08T04:58:05-5000";
            element_value->string_value.buffer = (char *)set_time_string;
            element_value->string_value.length_in_bytes = sizeof set_time_string -1;
            break;
        case idigi_group_device_stats_ctemp:
            ASSERT(element_ptr->type != idigi_element_type_float);
            element_value->float_value = 35.7;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    case idigi_group_device_info:
        switch (element_id)
        {
        case idigi_group_device_info_product:
            ASSERT(element_ptr->type != idigi_element_type_string);
            static char set_product[] = "ICC Testing Product";
            element_value->string_value.buffer = (char *)set_product;
            element_value->string_value.length_in_bytes = sizeof set_product -1;
            break;
        case idigi_group_device_info_model:
            ASSERT(element_ptr->type != idigi_element_type_string);
            static char set_model[] = "ICC Testing Model";
            element_value->string_value.buffer = (char *)set_model;
            element_value->string_value.length_in_bytes = sizeof set_model -1;
            break;
        case idigi_group_device_info_company:
            ASSERT(element_ptr->type != idigi_element_type_string);
            static char const set_company[] = "My Company";
            element_value->string_value.buffer = (char *)set_company;
            element_value->string_value.length_in_bytes = sizeof set_company -1;
            break;
        case idigi_group_device_info_desc:
            ASSERT(element_ptr->type != idigi_element_type_string);
            static char set_desc[] = "ICC Testing\nTesting Remote Configurations";
            element_value->string_value.buffer = (char *)set_desc;
            element_value->string_value.length_in_bytes = sizeof set_desc -1;
            break;
        case idigi_group_device_info_syspwd:
            ASSERT(element_ptr->type != idigi_element_type_string);
            static char set_syspwd[] = "password";
            element_value->string_value.buffer = (char *)set_syspwd;
            element_value->string_value.length_in_bytes = sizeof set_syspwd -1;
            break;
        }
        break;
    default:
        ASSERT(0);
        printf("set_group_element_value: invalid group %d\n", group_id);
        break;
    }
}

extern idigi_callback_status_t idigi_remote_set_handler(idigi_remote_data_t * remote_ptr)
{
    idigi_callback_status_t status;
    unsigned int group_id;

    idigi_remote_group_response_t * const response_data = &remote_ptr->response_data;
    idigi_remote_group_request_t * const request_data = &remote_ptr->request_data;

    idigi_group_t const * group_ptr = NULL;
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

    request_data->action = idigi_remote_action_set;

    for (group_id = 0; group_id < group_count; group_id++)
    {
        idigi_request_t request_id;
        unsigned int index;

        group_ptr = &group_table[group_id];

        for (index = group_ptr->start_index; index <= group_ptr->end_index; index++)
        {
            unsigned int element_index;
            size_t length = sizeof *response_data;

            request_data->group_id = group_id;
            request_data->group_index = index;
            request_data->element_value = NULL;

            response_data->error_id = idigi_success;
            response_data->element_data.error_hint = NULL;

            request_id.remote_config_request = idigi_remote_config_group_start;
            status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                                        request_data, sizeof request_data, response_data, &length);
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
                size_t length = sizeof *response_data;
                idigi_element_value_t element_value;

                idigi_group_element_t const * const element_ptr = &group_ptr->elements.data[element_index];

                if (element_ptr->access != idigi_element_access_read_only)
                {
                    request_data->element_id = element_index;
                    request_data->element_type = element_ptr->type;
                    request_data->element_value = &element_value;

                    set_group_element_value(group_id, index, element_index, element_ptr, &element_value);

                    response_data->error_id = idigi_success;
                    response_data->element_data.error_hint = NULL;

                    request_id.remote_config_request = idigi_remote_config_group_process;
                    status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                            request_data, (sizeof *request_data + sizeof element_value), response_data, &length);
                    if (status != idigi_callback_continue)
                    {
                        goto done;
                    }

                    printf("\n\t");


                    if (response_data->error_id != idigi_success)
                    {
                        element_name = (char *)element_ptr->name;
                        print_xml_open(element_ptr->name);
                        goto error;
                    }
                    else
                    {
                        print_xml_open_close(element_ptr->name);
                    }
                }
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
