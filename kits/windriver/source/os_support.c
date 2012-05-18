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
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "os_support.h"
#include "idigi_api.h"
#include "platform.h"

static sem_t sem_array[IC_MAX_NUM_EVENTS];

idigi_connector_error_t ic_create_event(int const event)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    int ret;

    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    APP_DEBUG("ic_create_event event [%d]\n", event);

    ret = sem_init(&sem_array[event], 0, 0);
    ASSERT_GOTO(ret == 0, error);

    status = idigi_connector_success;
error:
    return status;
}
idigi_connector_error_t ic_get_event(int const event, unsigned long timeout_ms)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    struct timespec wait_time;
    int ret;

    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    wait_time.tv_sec  = timeout_ms/1000;
    wait_time.tv_nsec = (timeout_ms%1000)*1000000;


    APP_DEBUG("ic_get_event event [%d] sec=[%d] nsec=[%d]\n", event, (int)wait_time.tv_sec, (int)wait_time.tv_nsec);

    /*ret = sem_timedwait(&sem_array[event], &wait_time);    */
    ret = sem_wait(&sem_array[event]);
    ASSERT_GOTO(ret == 0, error);

    status = idigi_connector_success;
error:
    return status;
}

idigi_connector_error_t ic_set_event(int const event)
{
    idigi_connector_error_t status=idigi_connector_event_error;
    int ret;

    ASSERT_GOTO(event < IC_MAX_NUM_EVENTS, error);

    APP_DEBUG("ic_set_event [%d]\n", event);

    ret = sem_post(&sem_array[event]);
    ASSERT_GOTO(ret == 0, error);

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
    idigi_status_t status = idigi_success;
    int ccode;

    ccode = pthread_create(&idigi_thread, NULL, idigi_run_thread, NULL);
    if (ccode != 0)
    {
        APP_DEBUG("thread_create() error on idigi_process_thread %d\n", ccode);
        goto error;
    }

error:

    return status;
}
