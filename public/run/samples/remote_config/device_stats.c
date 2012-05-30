/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
#include <time.h>
#include <stdlib.h>
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"

#define DEVICE_STATS_TIME_STRING_LENGTH 25

typedef struct {
    time_t curtime;
    int32_t signed_integer;
    char timestring[DEVICE_STATS_TIME_STRING_LENGTH];
} device_stats_config_data_t;

device_stats_config_data_t device_stats_config_data = {0, -10, "\0"};

idigi_callback_status_t app_device_stats_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{

    remote_group_session_t * const session_ptr = response->user_context;

    UNUSED_ARGUMENT(request);

    ASSERT(session_ptr != NULL);

    if (device_stats_config_data.curtime == 0)
    {
        struct tm * gmt;
        /* get initial time */
        app_os_get_system_time((unsigned long *)&device_stats_config_data.curtime);
        gmt = gmtime(&device_stats_config_data.curtime);
        sprintf(device_stats_config_data.timestring,
                                     "%04d-%02d-%02dT%02d:%02d:%02dZ",
                                     gmt->tm_year + 1900,
                                     gmt->tm_mon + 1,
                                     gmt->tm_mday,
                                     gmt->tm_hour,
                                     gmt->tm_min,
                                     gmt->tm_sec);
    }

    session_ptr->group_context = &device_stats_config_data;

    return idigi_callback_continue;
}

idigi_callback_status_t app_device_stats_group_get(idigi_remote_group_request_t const * const  request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;

    device_stats_config_data_t * device_stats_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_stats_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_setting_device_stats_curtime:
    {
        ASSERT(request->element.type == idigi_element_type_datetime);
        response->element_data.element_value->string_value = device_stats_ptr->timestring;
        break;
    }
    case idigi_setting_device_stats_signed_integer:
        ASSERT(request->element.type == idigi_element_type_int32);
        response->element_data.element_value->integer_signed_value = device_stats_ptr->signed_integer;
        break;

    default:
        ASSERT(0);
        break;
    }

    return status;
}

idigi_callback_status_t app_device_stats_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    remote_group_session_t * const session_ptr = response->user_context;
    device_stats_config_data_t * device_stats_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_stats_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_setting_device_stats_curtime:
    {


#if 0
        struct tm * lt;
        int t;
        char * ptr;
        size_t len;

        ASSERT(request->element.type == idigi_element_type_datetime);
        ASSERT(request->element.value != NULL);
        ASSERT(request->element.value->string_value != NULL);

        lt = localtime(&device_stats_ptr->curtime);

        ptr = request->element.value->string_value;
        len = 4;
        *(ptr+len) = '\0';
        t = atoi(ptr);
        if (t < 1900)
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "must be > 1900";
            goto done;
        }
        lt->tm_year = t - 1900;

        ptr += (len +1);
        len = 2;
        *(ptr+len) = '\0';
        t = atoi(ptr);
        if (t < 0 || t > 6)
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "month between 1 and 12";
            goto done;
        }
        lt->tm_mon = t -1;

        ptr += (len +1);
        len = 2;
        if (*(ptr+len) != 'T')
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "Time format";
            goto done;
        }
        *(ptr+len) = '\0';
        t = atoi(ptr);
        if (t < 1 || t > 31)
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "day of the month between 1 and 31";
            goto done;
        }
        lt->tm_mday = t;

        ptr += (len +1);
        len = 2;
        *(ptr+len) = '\0';
        t = atoi(ptr);
        if (t < 0 || t > 23)
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "hour between 0 and 23";
            goto done;
        }
        lt->tm_hour = t;

        ptr += (len +1);
        len = 2;
        *(ptr+len) = '\0';
        t = atoi(ptr);
        if (t < 0 || t > 59)
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "Minute between 0 and 59";
            goto done;
        }
        lt->tm_min = t;

        ptr += (len +1);
        len = 2;
        *(ptr+len) = '\0';
        t = atoi(ptr);
        if (t < 0 || t > 59)
        {
            response->error_id = idigi_group_error_unknown_value;
            response->element_data.error_hint = "Second between 0 and 59";
            goto done;
        }
        lt->tm_sec = t;

        ptr += (len +1);
        len = 2;

        if (mktime(lt) == -1)
        {
            response->error_id = idigi_group_error_save_failed;
            response->element_data.error_hint = "Cannot set time";
            goto done;
        }
#else
        ASSERT(request->element.type == idigi_element_type_datetime);
        ASSERT(request->element.value != NULL);
        ASSERT(request->element.value->string_value != NULL);
/*        ASSERT(strlen(request->element.value->string_value) == DEVICE_STATS_TIME_STRING_LENGTH); */

        ASSERT(strlen(request->element.value->string_value) < sizeof device_stats_ptr->timestring);
        memcpy(device_stats_ptr->timestring, request->element.value->string_value, strlen(request->element.value->string_value));
#endif

        break;
    }
    case idigi_setting_device_stats_signed_integer:
        ASSERT(request->element.type == idigi_element_type_int32);
        device_stats_ptr->signed_integer= request->element.value->integer_signed_value;
        break;

    default:
        ASSERT(0);
        break;
    }

    return status;
}

