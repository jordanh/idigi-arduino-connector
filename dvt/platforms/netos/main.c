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

#include <pthread.h>
#include "idigi_api.h"
#include "platform.h"

extern idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

static void * idigi_run_thread(void * arg)
{
    idigi_status_t status;

    APP_DEBUG("idigi_run thread starts\n");

    status = idigi_run((idigi_handle_t)arg);

    APP_DEBUG("idigi_run thread exits %d\n", status);

    pthread_exit(arg);
}

static void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("idigi_run thread starts\n");

    status = application_run((idigi_handle_t)arg);

    APP_DEBUG("application_run thread exits %d\n", status);

    pthread_exit(arg);
}

int main (void)
{
    pthread_t idigi_thread;
    pthread_t application_thread;

    idigi_handle_t idigi_handle;

    APP_DEBUG("Start iDigi\n");
    idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
    if (idigi_handle != NULL)
    {
        int ccode;
        ccode = pthread_create(&idigi_thread, NULL, idigi_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("thread_create() error on idigi_process_thread %d\n", ccode);
            goto done;
        }

        ccode = pthread_create(&application_thread, NULL, application_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("thread_create() error on idigi_process_thread %d\n", ccode);
            goto done;
        }

        pthread_join(idigi_thread, NULL);
        pthread_join(application_thread, NULL);
    }
    else
    {
        printf("unable to initialize iDigi\n");
    }
done:
    return 0;
}
