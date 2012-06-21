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
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config_cb.h"
#include "float.h"

#define VALUE_TO_STRING(value)   # value
#define MACRO_TO_STRING(macro)   VALUE_TO_STRING(macro)

#define GPS_STATS_LOCATION_STRING_LENGTH   sizeof (MACRO_TO_STRING(FLT_MAX))

typedef struct {
    float latitude;
    float longitude;
} gps_location_t;

static gps_location_t gps_data = { 45.01049, -93.254674 };

idigi_callback_status_t app_gps_stats_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    char * ptr;
    float value;

    switch (request->element.id)
    {
    case idigi_state_gps_stats_latitude:
    {
        static char gps_latitude[GPS_STATS_LOCATION_STRING_LENGTH];
        ptr = gps_latitude;
        value = gps_data.latitude;
        break;
    }
    case idigi_state_gps_stats_longitude:
    {
        static char gps_longitude[GPS_STATS_LOCATION_STRING_LENGTH];
        ptr = gps_longitude;
        value = gps_data.longitude;
        break;
    }
    default:
        ASSERT(0);
        goto done;
    }

    snprintf(ptr, GPS_STATS_LOCATION_STRING_LENGTH, "%f", value);
    response->element_data.element_value->string_value = ptr;

done:
    return status;
}

