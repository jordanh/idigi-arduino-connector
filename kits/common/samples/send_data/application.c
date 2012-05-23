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

#include <stdio.h>
#include "platform.h"
#include "idigi_connector.h"

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    APP_DEBUG("idigi_status: status update %d [%s]\n", status, status_message);
}

int application_start(void)
{
    idigi_connector_error_t ret;
    int status=-1;

    APP_DEBUG("application_start: calling idigi_connector_start\n");
    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_connector_start failed [%d]\n", ret);
        goto error;
    }

    APP_DEBUG("Sending data to cloud using idigi_send_data...\n");
    do
    {
        static idigi_connector_data_t ic_data = {0};
        static char buffer[] = "iDigi Device application data!\n";

        #define WAIT_FOR_A_SECOND  1
        app_os_sleep(WAIT_FOR_A_SECOND);

        ic_data.data_ptr = buffer;
        ic_data.length_in_bytes = (sizeof buffer) - 1;
        ret = idigi_send_data("sample.txt", &ic_data, NULL);

    } while (ret == idigi_connector_init_error);

    if (ret != idigi_connector_success)
    {
        APP_DEBUG("\nSend failed [%d]\n", ret);
        goto error;
    }

    APP_DEBUG("\nSend completed\n");
    status = 0;

error:
    return status;
}
