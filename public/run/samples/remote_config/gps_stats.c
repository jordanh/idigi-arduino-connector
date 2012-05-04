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


typedef struct {
    float latitude;
    float longitude;
} gps_location_t;

static gps_location_t gps_data = { 45.01049, -93.254674 };

idigi_callback_status_t app_gps_stats_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    switch (request->element.id)
    {
    case idigi_state_gps_stats_latitude:
        response->element_data.element_value->float_value = gps_data.latitude;
        break;
    case idigi_state_gps_stats_longitude:
        response->element_data.element_value->float_value = gps_data.longitude;
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;
}

