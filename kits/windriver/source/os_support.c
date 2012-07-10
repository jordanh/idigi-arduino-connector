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
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "os_support.h"
#include "idigi_api.h"
#include "platform.h"
#include "idigi_debug.h"

static sem_t sem_array[IC_MAX_NUM_EVENTS];

idigi_connector_error_t ic_create_event(int const event)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    int ret;

    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    APP_DEBUG("ic_create_event [%d]\n", event);

    ret = sem_init(&sem_array[event], 0, 0);
    ASSERT_GOTO(ret == 0, error);

    status = idigi_connector_success;
error:
    return status;
}
idigi_connector_error_t ic_get_event(int const event, unsigned long const event_bit, unsigned long timeout_ms)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    struct timespec wait_time;
    int ret;

    UNUSED_PARAMETER(event_bit);
    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    wait_time.tv_sec  = timeout_ms/1000;
    wait_time.tv_nsec = (timeout_ms%1000)*1000000;


    APP_DEBUG("ic_get_event: [%d] sec=[%d] nsec=[%d]\n", event, (int)wait_time.tv_sec, (int)wait_time.tv_nsec);

    /*ret = sem_timedwait(&sem_array[event], &wait_time);    */
    ret = sem_wait(&sem_array[event]);
    ASSERT_GOTO(ret == 0, error);

    status = idigi_connector_success;
error:
    return status;
}

idigi_connector_error_t ic_set_event(int const event, unsigned long const event_bit)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    int ret;

    UNUSED_PARAMETER(event_bit);
    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    APP_DEBUG("ic_set_event [%d]\n", event);

    ret = sem_post(&sem_array[event]);
    ASSERT_GOTO(ret == 0, error);

    status = idigi_connector_success;
error:
    return status;
}

/*
 * Check if semaphore is already set and if so clear it.
 */
idigi_connector_error_t ic_clear_event(int const event, unsigned long const event_bit)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    int ret, val;

    UNUSED_PARAMETER(event_bit);
    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    APP_DEBUG("ic_clear_event [%d]\n", event);

    while(sem_getvalue(&sem_array[event], &val) > 0)
    {
        ret = sem_trywait(&sem_array[event]);
        ASSERT_GOTO(ret == 0, error);
    }

    status = idigi_connector_success;
error:

    return status;

}

void ic_free(void *ptr)
{
    free(ptr);
}
void *ic_malloc(size_t size)
{
    return malloc(size);
}


void *idigi_run_thread(void * arg)
{
    idigi_connector_thread(0);
    pthread_exit(arg);
    return NULL;
}

idigi_connector_error_t ic_create_thread(void)
{
    pthread_t idigi_thread;
    int ret = 0;
    int ccode;

    ccode = pthread_create(&idigi_thread, NULL, idigi_run_thread, NULL);
    if (ccode != 0)
    {
        APP_DEBUG("thread_create() error on idigi_process_thread %d\n", ccode);
        goto error;
    }

error:

    return ret;
}

void ic_software_reset(void)
{
    return;
}

void ic_watchdog_reset(void)
{
    return;
}

#if (!defined IDIGI_DEBUG)
/* Stub routine: don't do anything (debug is shut off) */
void idigi_debug_printf(char const * const format, ...)
{
    UNUSED_PARAMETER(format);

}
#endif
