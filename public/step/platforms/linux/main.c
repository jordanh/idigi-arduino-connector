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

#include <stdlib.h>
#include <unistd.h>
#include "idigi_api.h"
#include "platform.h"

extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

int main (void)
{
    int status=EXIT_FAILURE;
    idigi_handle_t idigi_handle;

    APP_DEBUG("main: Starting iDigi\n");
    idigi_handle = idigi_init((idigi_callback_t) app_idigi_callback);
    if (idigi_handle == NULL)
    {
        APP_DEBUG("main: idigi_init() failed\n");
        goto done;
    }

    /* Sample program control loop */
    for(;;)
    {
        idigi_status_t status;
        /*
         * Example of using idigi_step(), we run a portion of the IIK then run
         * a portion of the users application.
         */
        status = idigi_step(idigi_handle);
        if (status != idigi_success && status != idigi_receive_error &&
            status != idigi_send_error && status != idigi_connect_error)
        {
            /* We exclude idigi_receive_error, idigi_send_error, &
             * idigi_connect_error. We want to reconnect iDigi
             * even if idigi_step returns error in receive, send, or connect.
             */
            APP_DEBUG("main: idigi_step() failed\n");
            break;
        }
        if (application_step(idigi_handle) != 0)
        {
            APP_DEBUG("main: application_step() failed\n");
            goto done;
        }
        usleep(1000);
    }

done:
    return status;
}
