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

#include "idigi_api.h"
#include "idigi_remote.h"
#include "remote_config.h"
#include "idigi_remote_sci.h"

typedef idigi_callback_status_t (* idigi_remote_command_cb_t) (idigi_remote_data_t * const remote_ptr);

typedef struct {
    unsigned int string_index;
    idigi_remote_action_t action;
    idigi_remote_group_type_t group_type;
    idigi_remote_command_cb_t command_cb;
}idigi_remote_command_t;

idigi_remote_command_t const supported_commands[] = {
        {IDIGI_QUERY_SETTING_STRING_INDEX,
         idigi_remote_action_query,
         idigi_remote_group_config,
         idigi_remote_query_handler},

        {IDIGI_QUERY_STATE_STRING_INDEX,
         idigi_remote_action_query,
         idigi_remote_group_sysinfo,
         idigi_remote_query_handler},

        {IDIGI_SET_SETTING_STRING_INDEX,
         idigi_remote_action_set,
         idigi_remote_group_config,
        idigi_remote_set_handler},

        {IDIGI_SET_STATE_STRING_INDEX,
         idigi_remote_action_set,
         idigi_remote_group_sysinfo,
        idigi_remote_set_handler}
};

idigi_remote_data_t idigi_remote_data;

void print_xml_value(char const * const name)
{
    unsigned char length = *name;
    char * ptr = (char *)name+1;

    printf("%.*s", length, ptr);
}

void print_xml_open_index(char const * const tag, unsigned int index)
{
    unsigned char length = *tag;
    char * ptr = (char *)tag+1;

    printf("<%.*s index=\"%d\">", length, ptr, index);
}

void print_xml_open_attr(char const * const tag, char const * const attr_tag, char const * const attr_value_tag)
{
    unsigned char len = *tag;
    char * ptr = (char *)tag+1;

    unsigned char attr_len = *attr_tag;
    char * attr_ptr = (char *)attr_tag+1;

    unsigned char value_len = *attr_value_tag;
    char * value_ptr = (char *)attr_value_tag+1;

    printf("<%.*s %.*s=\"%.*s\">", len, ptr, attr_len, attr_ptr, value_len, value_ptr);
}

void print_xml_open(char const * const tag)
{
    unsigned char length = *tag;
    char * ptr = (char *)tag+1;

    printf("<%.*s>", length, ptr);
}

void print_xml_close(char const * const tag)
{
    unsigned char length = *tag;
    char * ptr = (char *)tag+1;

    printf("</%.*s>", length, ptr);
}

void print_xml_open_close(char const * const tag)
{
    unsigned char length = *tag;
    char * ptr = (char *)tag+1;

    printf("<%.*s/>", length, ptr);
}

void print_xml_error(char const * const * errors, unsigned int count, unsigned int error_id, char const * const hint)
{

    ASSERT(hint != NULL);

    printf("<error id=\"%d\">", error_id);
    if (error_id <= idigi_group_error_count)
    {
        static unsigned int const idigi_errors_index[] = {
                ERROR_FIELD_NOT_EXIT_STRING_INDEX,
                ERROR_LOAD_FAILED_STRING_INDEX,
                ERROR_SAVE_FAILED_STRING_INDEX,
                ERROR_UNKNOWN_VALUE_STRING_INDEX
        };

        unsigned char length = idigi_all_strings[idigi_errors_index[error_id -1]];
        char * ptr = (char *)&idigi_all_strings[idigi_errors_index[error_id -1]]+1;

        printf("<desc>%.*s</desc>", length, ptr);

    }
    else if ((error_id- idigi_group_error_count) <= count)
    {
        unsigned int index = error_id - idigi_group_error_count;
        unsigned char length = *errors[index];
        char * ptr = (char *)errors[index]+1;

        printf("<desc>%.*s</desc>", length, ptr);
    }
    if (hint != NULL)
    {
        printf("<hint>%s</hint>", hint);

    }
    printf("</error>\n");
}

idigi_callback_status_t remote_sci_request(char * rci_command, size_t length)
{
    idigi_callback_status_t status;
    idigi_remote_group_response_t  * response_data = &idigi_remote_data.response_data;
    size_t response_length = sizeof *response_data;
    idigi_request_t request_id;
    size_t i;

    response_data->user_context = NULL;
    idigi_remote_data.request_id = idigi_remote_config_session_start;
    request_id.remote_config_request = idigi_remote_data.request_id;
    status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                                NULL, 0, response_data, &response_length);
    if (status != idigi_callback_continue)
    {
        goto done;
    }

    printf("\n");
    print_xml_open_attr(&idigi_all_strings[IDIGI_RCI_REPLY_STRING_INDEX],
                        &idigi_all_strings[IDIGI_VERSION_STRING_INDEX],
                        &idigi_all_strings[IDIGI_RCI_VERSION_STRING_INDEX]);

    if (response_data->error_id != idigi_success)
    {
        goto error;
    }

    for (i=0; i < asizeof(supported_commands); i++)
    {
        unsigned char const cmd_length = idigi_all_strings[supported_commands[i].string_index];
        char * cmd = (char *)&idigi_all_strings[supported_commands[i].string_index] + 1;

        if (length == cmd_length &&
            memcmp(rci_command, cmd, length) == 0)
        {
            idigi_remote_group_request_t * const request_data = &idigi_remote_data.request_data;

            request_data->action = supported_commands[i].action;
            request_data->group_type = supported_commands[i].group_type;

            response_data->element_data.error_hint = NULL;

            printf("\n");
            print_xml_open(&idigi_all_strings[supported_commands[i].string_index]);

            idigi_remote_data.request_id = idigi_remote_config_action_start;
            request_id.remote_config_request = idigi_remote_data.request_id;
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

            status = supported_commands[i].command_cb(&idigi_remote_data);

            if (status != idigi_callback_continue)
            {
                goto done;
            }

            response_data->element_data.error_hint = NULL;

            idigi_remote_data.request_id = idigi_remote_config_action_end;
            request_id.remote_config_request = idigi_remote_data.request_id;
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
            break;
        }
    }

error:
    if (response_data->error_id != idigi_success)
    {
        print_xml_error(NULL, 0, response_data->error_id, response_data->element_data.error_hint);
    }

    switch (idigi_remote_data.request_id)
    {
    case idigi_remote_config_action_start:
    case idigi_remote_config_action_end:
        printf("\n");
        print_xml_close(&idigi_all_strings[supported_commands[i].string_index]);
        break;
    default:
        break;
    }

    idigi_remote_data.request_id = idigi_remote_config_session_end;
    request_id.remote_config_request = idigi_remote_data.request_id;
    status = app_idigi_callback(idigi_class_remote_config_service, request_id,
                                NULL, 0, response_data, &length);
    if (status != idigi_callback_continue)
    {
        goto done;
    }

    printf("\n");
    print_xml_close(&idigi_all_strings[IDIGI_RCI_REPLY_STRING_INDEX]);
    printf("\n");

done:
    return status;

}
