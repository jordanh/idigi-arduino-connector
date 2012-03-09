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
#include "idigi_api.h"
#include "platform.h"
#include "idigi_remote.h"
#include "remote_config.h"

#define DEVICE_STATS_TIME_STRING_LENGTH 22

extern int app_os_get_system_time(unsigned long * const uptime);

typedef struct {
    time_t curtime;
    float   temperature;

    char timestring[DEVICE_STATS_TIME_STRING_LENGTH];
} device_stats_config_data_t;

device_stats_config_data_t device_stats_config_data = {0, 0, "\0"};

idigi_callback_status_t app_device_stats_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{

    UNUSED_ARGUMENT(request);

    if (device_stats_config_data.curtime == 0)
    {
        /* get initial time */
        app_os_get_system_time((unsigned long *)&device_stats_config_data.curtime);
    }

    response->user_context = &device_stats_config_data;

    return idigi_callback_continue;
}

idigi_callback_status_t app_device_stats_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    device_stats_config_data_t * device_stats_ptr;

    ASSERT(reqeust->user_context);

    device_stats_ptr = response->user_context;

    switch (request->element_id)
    {
    case idigi_group_device_stats_curtime:
    {
        struct tm * gmt;

        ASSERT(request->element_type == idigi_element_type_datetime);
        gmt = gmtime(&device_stats_ptr->curtime);
        response->element_data.element_value->string_value.length_in_bytes = sprintf(device_stats_ptr->timestring,
                                    "%04d-%02d-%02dT%02d:%02d:%02dtz",
                                    gmt->tm_year + 1900,
                                    gmt->tm_mon + 1,
                                    gmt->tm_mday,
                                    gmt->tm_hour,
                                    gmt->tm_min,
                                    gmt->tm_sec);
        response->element_data.element_value->string_value.buffer = device_stats_ptr->timestring;
        break;
    }
    case idigi_group_device_stats_ctemp:
        ASSERT(request->element_type == idigi_element_type_float);
        response->element_data.element_value->float_value = device_stats_ptr->temperature;
        break;
    case idigi_group_device_stats_ftemp:
        ASSERT(request->element_type == idigi_element_type_float);
        response->element_data.element_value->float_value = device_stats_ptr->temperature * 9 / 5 + 32;
        break;

    default:
        ASSERT(0);
        response->error_id = idigi_group_error_unknown_value;
        response->element_data.error_hint = NULL;
        break;
    }

    return status;
}

idigi_callback_status_t app_device_stats_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    device_stats_config_data_t * device_stats_ptr;

    ASSERT(reqeust->user_context);

    device_stats_ptr = response->user_context;

    switch (request->element_id)
    {
    case idigi_group_device_stats_curtime:
    {
        struct tm * lt;
        int t;
        char * ptr;
        size_t len;

        ASSERT(request->element_type == idigi_element_type_datetime);
        ASSERT(request->element_value->string_value.buffer != NULL)
        ASSERT(request->element_value->string_value.length_in_bytes != DEVICE_STATS_TIME_STRING_LENGTH);

        lt = localtime(&device_stats_ptr->curtime);

        ptr = request->element_value->string_value.buffer;
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

#if 0
        if (mktime(lt) == -1)
        {
            response->error_id = idigi_group_error_save_failed;
            response->element_data.error_hint = "Cannot set time";
            goto done;
        }
#endif

        break;
    }
    case idigi_group_device_stats_ctemp:
        ASSERT(request->element_type == idigi_element_type_float);
        device_stats_ptr->temperature= request->element_value->float_value;
        break;

    case idigi_group_device_stats_ftemp:
    default:
        ASSERT(0);
        response->error_id = idigi_group_error_unknown_value;
        response->element_data.error_hint = NULL;
        break;
    }
done:
    return status;
}

