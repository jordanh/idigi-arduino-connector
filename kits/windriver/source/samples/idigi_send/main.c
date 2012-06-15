/*
 * Copyright (c) 1996-2012 Digi International Inc.,
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
#include "idigi_connector.h"


static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    printf("Got an event from iDigi Connector %d[%s]\n", status, status_message);
}

int main (void)
{
    idigi_connector_data_t ic_data;
    idigi_connector_error_t ret;
    int i;

    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        printf("idigi_send_data failed [%d]", ret);
    }

    sleep(5); /* Give the iDigi connector time to start */

    /* Data to send to the server */
    ic_data.data_ptr         = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
    ic_data.length_in_bytes  = strlen(ic_data.data_ptr);
    ic_data.flags            = 0;

    for (;;)
    {
    	printf("Sending [%d] bytes to idigi\n", ic_data.length_in_bytes);
    	/* Sent to the file test.txt in the root directory for the device */
        ret = idigi_send_data("test.txt", &ic_data, "text/plain");

        if (ret != idigi_connector_success)
        {
            printf("idigi_send_data failed [%d]", ret);
        }
        sleep(60);
    }

    return 0;
}
