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
#include <errno.h>

#include "application.h"
/* #define THREAD_STACK_SIZE_DEBUG */

extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

idigi_handle_t idigi_handle;

pthread_t idigi_thread;
pthread_t application_thread;
pthread_t send_terminate_thread;

idigi_status_t idigi_run_thread_status = idigi_success;

extern char terminate_file_content[];



void * idigi_run_thread(void * arg)
{

    APP_DEBUG("idigi_run_thread starts pid = %d \n", getpid());
    idigi_run_thread_status = idigi_success;

    while (idigi_run_thread_status == idigi_success)
    {
        idigi_run_thread_status = idigi_run((idigi_handle_t)arg);
        if (idigi_run_thread_status == idigi_receive_error ||
            idigi_run_thread_status == idigi_send_error ||
            idigi_run_thread_status == idigi_connect_error)
        {
            APP_DEBUG("idigi_run_thread: idigi_run returns %d. Let's continue calling idigi_run\n",
                    idigi_run_thread_status);
            idigi_run_thread_status = idigi_success;
        }
    }
    APP_DEBUG("idigi_run thread exits %d\n", idigi_run_thread_status);

    idigi_run_thread_status = idigi_device_terminated;

    pthread_exit(arg);
}


void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("application_run_thread starts\n");

    status = application_run((idigi_handle_t)arg);

    APP_DEBUG("application_run thread exits %d\n", status);

    pthread_exit(arg);
}


void print_error(int error_no, char * const error_message)
{
    errno = error_no;
    perror("Error:");
    perror(error_message);
    ASSERT(false);
}

int start_idigi_thread(void)
{
    int ccode = -1;

    idigi_handle = idigi_init((idigi_callback_t) app_idigi_callback);
    if (idigi_handle != NULL)
    {
        ccode = pthread_create(&idigi_thread, NULL, idigi_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("thread_create() error on idigi_run_thread %d\n", ccode);
            idigi_run_thread_status = idigi_device_terminated;
        }
    }
    else
    {
        APP_DEBUG("unable to initialize iDigi\n");
    }

    return ccode;
}

int start_application_thread(void)
{
    int ccode = 0;

    if (idigi_handle != NULL)
    {
        ccode = pthread_create(&application_thread, NULL, application_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("thread_create() error on application_run_thread %d\n", ccode);
        }
    }

    return ccode;
}

int main (void)
{
    extern size_t total_malloc_size;

    int rc = 0;


    for (;;)
    {
        APP_DEBUG("Start iDigi\n");

        rc = start_idigi_thread();
        if (rc != 0)
        {
           goto done;
        }

        rc = start_application_thread();
        if (rc != 0)
        {
           goto done;
        }

        pthread_join(idigi_thread, NULL);
        pthread_join(application_thread, NULL);

        if (idigi_run_thread_status == idigi_device_terminated)
        {
            strcpy(terminate_file_content, "terminate_ok");
            APP_DEBUG("idigi_run has been terminated by idigi_initiate_terminate\n");
        }
        if (total_malloc_size != 0)
        {
            /* terminate iik so it will not reconnect to iDigi */
            APP_DEBUG("total malloc memory = %zu after all threads are canceled\n", total_malloc_size);
            if (idigi_run_thread_status == idigi_device_terminated)
            {
                strcpy(terminate_file_content, "terminate_memory_leak");
                APP_DEBUG("Error: idigi_run has been terminated by idigi_initiate_terminate but total malloc memory is not 0 after all threads are canceled\n");
            }
            if (put_file_active_count > 0)
            {
                APP_DEBUG("Error: %d active put requests\n", put_file_active_count);
            }
        }
        /* continue and reconnect iDigi so python test will not fail */
    }

done:
    return rc;
}
