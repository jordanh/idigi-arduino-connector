/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees+
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

#include <pthread.h>
#include "idigi_api.h"
#include "platform.h"
#include <unistd.h>

extern idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

pthread_t idigi_thread;
pthread_t application_thread;


__thread void* stack_start;
__thread long stack_max_size = 0L;

void check_stack_size(void)
{
    if (pthread_self() == idigi_thread)
    {
    /* address of 'nowhere' approximates end of stack */
    char nowhere;
    void* stack_end = (void*)&nowhere;
    /* may want to double check stack grows downward on your platform */
    long stack_size = (long)stack_start - (long)stack_end;
    /* update max_stack_size for this thread */

    if (stack_size > stack_max_size)
    {
      stack_max_size = stack_size;
      APP_DEBUG("check_stack_size= %ld\n", stack_max_size);
    }
    }
}

void * idigi_run_thread(void * arg)
{
    char nowhere;
    stack_start = (void*)&nowhere;
    idigi_status_t status;

    /* Call check_stack_size() at beginning of the thread.
     * Also call it in deeply nested functions */

    check_stack_size();

    APP_DEBUG("idigi_run_thread starts %d stack size = %ld\n", getpid(), stack_max_size);

    status = idigi_run((idigi_handle_t)arg);

    APP_DEBUG("idigi_run thread exits %d\n", status);

    check_stack_size();

    APP_DEBUG("idigi_run_thread: stack size = %ld\n", stack_max_size);

    pthread_exit(arg);
}


void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("idigi_run thread starts\n");

    status = application_run((idigi_handle_t)arg);

    APP_DEBUG("application_run thread exits %d\n", status);

    pthread_exit(arg);
}

int main (void)
{
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
