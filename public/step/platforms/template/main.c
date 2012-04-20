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
#include "idigi_api.h"
#include "platform.h"

extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

int main (void)
{
    idigi_handle_t idigi_handle;

    APP_DEBUG("main: Starting iDigi\n");
    /* TODO: Initialized iik by calling idigi_init
     * with a callback. Replace idigi_callback with
     * your callback function or add idigi_callback.
     *
     */
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
        if (status != idigi_success)
        {
            APP_DEBUG("main: idigi_step() failed\n");
            break;
        }

        /* TODO: execute other processes
         *
         */
    }

done:
    return 0;
}
