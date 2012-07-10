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
#include <sys/timeb.h>
#include <stdlib.h>


#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"

#define TIME_FORMAT "%04d-%02d-%02dT%02d:%02d:%02d%c%02d%02d"

typedef struct {
    struct timeb current_time;
    char timestring[sizeof "2010-12-12T12:12:12-0000"];
} device_time_config_data_t;

device_time_config_data_t device_time_config_data = {{0}, "\0"};

idigi_callback_status_t app_device_time_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{

    remote_group_session_t * const session_ptr = response->user_context;

    UNUSED_ARGUMENT(request);

    ASSERT(session_ptr != NULL);

    ftime(&device_time_config_data.current_time);

    session_ptr->group_context = &device_time_config_data;

    return idigi_callback_continue;
}

idigi_callback_status_t app_device_time_group_get(idigi_remote_group_request_t const * const  request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    remote_group_session_t * const session_ptr = response->user_context;

    device_time_config_data_t * device_time_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_time_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_setting_device_time_curtime:
    {
        struct tm * the_time;

        ASSERT(request->element.type == idigi_element_type_datetime);

//        the_time = localtime(&device_time_ptr->current_time.time);
        the_time = gmtime(&device_time_ptr->current_time.time);

        if (the_time == NULL)
        {
            response->error_id = idigi_global_error_load_fail;
            response->element_data.error_hint = "Time is not available.";
            goto done;
        }

        {
            int tz_hour = device_time_ptr->current_time.timezone / 60;
            int const tz_min = device_time_ptr->current_time.timezone % 60;
            int const timestring_size = sizeof device_time_ptr->timestring;

            snprintf(device_time_ptr->timestring, timestring_size,
                                     TIME_FORMAT,
                                     the_time->tm_year + 1900,
                                     the_time->tm_mon + 1,
                                     the_time->tm_mday,
                                     the_time->tm_hour,
                                     the_time->tm_min,
                                     the_time->tm_sec,
                                     (device_time_ptr->current_time.timezone > 0) ? '-' : '+',
                                     tz_hour, tz_min);

            response->element_data.element_value->string_value = device_time_ptr->timestring;
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

idigi_callback_status_t app_device_time_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    remote_group_session_t * const session_ptr = response->user_context;
    device_time_config_data_t * device_time_ptr;

    ASSERT(session_ptr != NULL);
    ASSERT(session_ptr->group_context != NULL);

    device_time_ptr = session_ptr->group_context;

    switch (request->element.id)
    {
    case idigi_setting_device_time_curtime:
    {

        #define TIME_FORMAT_ERROR_HINT  "Time format"

        typedef enum {
            get_year, get_month_dash, get_month, get_day_dash, get_day, get_time_sparator,
            get_hour, get_minute_colon, get_minute, get_second_colon, get_second,
            get_tz, get_tzhour, get_tzmin, get_done
        } datetime_state;

        datetime_state state = get_year;
        struct tm * lt;
        size_t string_length;
        size_t current_length = 0;
        int timezone;

        ASSERT(request->element.type == idigi_element_type_datetime);
        ASSERT(request->element.value != NULL);
        ASSERT(request->element.value->string_value != NULL);

        lt = localtime(&device_time_ptr->current_time.time);
        if (lt == NULL)
        {
            response->error_id = idigi_global_error_save_fail;
            response->element_data.error_hint = "Time is not available.";
            goto done;
        }

        string_length = strlen(request->element.value->string_value);

        while (current_length < string_length && state != get_done)
        {
            char const * const ptr = &request->element.value->string_value[current_length];
            size_t len =0;

            switch (state)
            {
            case get_year:
                len = 4;
                break;
            case get_month:
            case get_day:
            case get_hour:
            case get_minute:
            case get_second:
            case get_tzhour:
            case get_tzmin:
                len = 2;
                break;
            case get_tz:
            case get_month_dash:
            case get_day_dash:
            case get_time_sparator:
            case get_minute_colon:
            case get_second_colon:
                len = 1;
                break;
            case get_done:
                len = 0;
                goto error;
            }

            if ((current_length + len) > string_length)
            {
                goto error;
            }
            current_length += len;

            {
                int t;
                char timebuf[len+1];

                memcpy(timebuf, ptr, len);
                timebuf[len] = '\0';

                switch (state)
                {
                case get_year:
                    t = atoi(timebuf);
                    if (t < 1900)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "must be > 1900";
                        goto done;
                    }
                    lt->tm_year = t - 1900;
                    state = get_month_dash;
                    break;

                case get_month_dash:
                case get_day_dash:
                     if (*timebuf != '-')
                     {
                         goto error;
                     }
                     state = (state == get_month_dash) ? get_month : get_day;
                     break;

                case get_month:
                    t = atoi(timebuf);
                    if (t < 0 || t > 12)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "month between 1 and 12";
                        goto done;
                    }
                    lt->tm_mon = t -1;
                    state = get_day_dash;
                    break;

                case get_day:
                    t = atoi(timebuf);
                    if (t < 1 || t > 31) /* day */
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "day of the month between 1 and 31";
                        goto done;
                    }
                    lt->tm_mday = t;
                    state = get_time_sparator;
                    break;

                case get_time_sparator:
                    if (*timebuf != 'T')
                    {
                        goto error;
                    }
                    state = get_hour;
                    break;

                case get_hour:
                    t = atoi(timebuf);
                    if (t < 0 || t > 23)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "hour between 0 and 23";
                        goto done;
                    }
                    lt->tm_hour = t;
                    state = get_minute_colon;
                    break;

                case get_minute_colon:
                case get_second_colon:
                    if (*timebuf != ':')
                    {
                        goto error;
                    }
                    state = (state == get_minute_colon) ? get_minute : get_second;
                    break;

                case get_minute:
                    t = atoi(timebuf);
                    if (t < 0 || t > 59)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "Minute between 0 and 59";
                        goto done;
                    }
                    lt->tm_min = t;
                    state = get_second_colon;
                    break;

                case get_second:
                    t = atoi(timebuf);
                    if (t < 0 || t > 59)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "Second between 0 and 59";
                        goto done;
                    }
                    lt->tm_sec = t;
                    state = get_tz;
                    break;

                case get_tz:
                    timezone = 0;

                    if (*timebuf == '-')
                    {
                        timezone = -1;
                        state = get_tzhour;
                    }
                    else if (*timebuf == '+')
                    {
                        state = get_tzhour;
                    }
                    else if (*timebuf == 'Z')
                    {
                        state = get_done;
                    }
                    else
                    {
                        goto error;
                    }
                    break;

                case get_tzhour:
                    t = atoi(timebuf);
                    if (t < 0 || t > 24)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "Invalid timezone";
                        goto done;
                    }
                    timezone *= (t * 60);
                    state = get_tzmin;
                    break;

                case get_tzmin:
                    t = atoi(timebuf);
                    if (t < 0 || t > 59)
                    {
                        response->error_id = idigi_setting_device_time_error_invalid_time;
                        response->element_data.error_hint = "Invalid timezone";
                        goto done;
                    }
                    timezone += t;
                    state = get_done;
                    break;

                case get_done:
                    goto error;
                }
            }
        }

        if ((current_length < string_length) || ((state != get_done) && (state != get_tz)))
        {
            goto error;

        }

        if (mktime(lt) == -1)
        {
            response->error_id = idigi_global_error_save_fail;
            response->element_data.error_hint = "Cannot set time";
            goto done;
        }

        break;
    }

    default:
        ASSERT(0);
        break;
    }

    goto done;

error:
    response->error_id = idigi_setting_device_time_error_invalid_time;
    response->element_data.error_hint = TIME_FORMAT_ERROR_HINT;

done:
    return status;
}

