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
#include <time.h>
#include <stdlib.h>
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"

#define SYSTEM_STRING_LENGTH 64


typedef struct {
    char contact[SYSTEM_STRING_LENGTH];
    char location[SYSTEM_STRING_LENGTH];
    char description[SYSTEM_STRING_LENGTH];
} system_data_t;

system_data_t system_config_data = {"\0", "\0", "\0"};


idigi_callback_status_t app_system_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    system_data_t * const system_ptr = &system_config_data;

    ASSERT(request->element.type == idigi_element_type_password);

    switch (request->element.id)
    {
    case idigi_setting_system_contact:
        response->element_data.element_value->string_value = system_ptr->contact;
        break;
    case idigi_setting_system_location:
        response->element_data.element_value->string_value = system_ptr->location;
        break;
    case idigi_setting_system_description:
        response->element_data.element_value->string_value = system_ptr->description;
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_system_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    system_data_t * const system_ptr = &system_config_data;
    char * src_ptr = NULL;

    UNUSED_ARGUMENT(response);

    ASSERT(request->element.type == idigi_element_type_password);
    ASSERT(strlen(request->element.value->string_value) < sizeof system_ptr->contact);

    switch (request->element.id)
    {
    case idigi_setting_system_contact:
        src_ptr = system_ptr->contact;
        break;
    case idigi_setting_system_location:
        src_ptr = system_ptr->location;
        break;
    case idigi_setting_system_description:
        src_ptr = system_ptr->description;
        break;
    default:
        ASSERT(0);
        break;
    }

    if (src_ptr != NULL)
    {
        memcpy(src_ptr, request->element.value->string_value, strlen(request->element.value->string_value));
    }
    return status;
}

