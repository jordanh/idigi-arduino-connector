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
#if 0
#define  STACK_SIZE  (1024 * 1024 * 8)
#define  STACK_INIT_VALUE   0xEF

__thread void * stack_top;
__thread void * stack_start;
__thread long iik_stack_max_size = 0L;
__thread long thread_stack_size = STACK_SIZE;

size_t PrintThreadStackInfo(long thread_stack_start)
{
    pthread_attr_t Attributes;
    void *StackTop;
    void *StackBottom;
    size_t StackSize;
    size_t StackUsed;

    /* Get the pthread attributes */
    memset (&Attributes, 0, sizeof (Attributes));
    pthread_getattr_np (pthread_self(), &Attributes);
    /* From the attributes, get the stack info */
    pthread_attr_getstack (&Attributes, &StackTop, &StackSize);
    /* Done with the attributes */
    pthread_attr_destroy (&Attributes);

    StackBottom = (void *)((long)StackTop + StackSize);
    StackUsed = (size_t)((long)StackBottom - (long)thread_stack_start);

    APP_DEBUG ("-------------------\n");
    APP_DEBUG ("Stack top:     %p\n", StackTop);
    APP_DEBUG ("Stack size:    0x%x (%u) bytes\n", StackSize, StackSize);
    APP_DEBUG ("Stack bottom:  %p\n", StackBottom);
    APP_DEBUG ("Initial thread stack used: %d\n", StackUsed);

    if (StackSize < (size_t)thread_stack_size)
    {
        thread_stack_size = StackSize - 0x1000;
    }

    return StackUsed;
}

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

        if (stack_size > iik_stack_max_size)
        {
            iik_stack_max_size = stack_size;
            APP_DEBUG("check_stack_size= %ld\n", iik_stack_max_size);
        }
        APP_DEBUG(">>>> %p\n", stack_end);
    }
}

void clear_stack_size(void)
{
    if (pthread_self() == idigi_thread)
    {
        /* address of 'nowhere' approximates end of stack */
        char nowhere;
        void* stack_end = (void*)&nowhere;
        /* may want to double check stack grows downward on your platform */
        long stack_size = (long)stack_start - (long)stack_end;
        long empty_size = (long)stack_end - (long)stack_top;
        /* update max_stack_size for this thread */

        if (stack_size > iik_stack_max_size)
        {
            iik_stack_max_size = stack_size;
            APP_DEBUG("clear_stack_size= %ld\n", iik_stack_max_size);
        }

        APP_DEBUG("<<<< %p (%ld)\n", stack_end, empty_size);
        //memset((void *)stack_top , STACK_INIT_VALUE, empty_size);
    }
}

size_t PrintSummaryStack(void)
{
    uint8_t * ptr = (uint8_t *)stack_top;

    while (*ptr == STACK_INIT_VALUE)
    {
        ptr++;
    }
    APP_DEBUG("======================================\n");
    APP_DEBUG("Stack End     = 0x%x\n", (unsigned)stack_top);
    APP_DEBUG("Stack Ptr     = 0x%x\n", (unsigned)ptr);
    APP_DEBUG("Stack Start   = 0x%x\n", (unsigned)stack_start);
    APP_DEBUG("Size in bytes = %d\n", (unsigned)((uint8_t *)stack_start-ptr));
    return (size_t)((uint8_t *)stack_start-ptr);
}
#endif

void * idigi_run_thread(void * arg)
{
#if 0
    char nowhere;
    stack_start = (void*)&nowhere;
#endif
    idigi_status_t status;

#if 0
    stack_top = (void *)((long)stack_start - thread_stack_size);
    memset(stack_top , STACK_INIT_VALUE, thread_stack_size);
    /* Call check_stack_size() at beginning of the thread.
     * Also call it in deeply nested functions */

    PrintThreadStackInfo((long)stack_start);

    check_stack_size();
#endif
    APP_DEBUG("idigi_run_thread starts %d\n", getpid());

    status = idigi_run((idigi_handle_t)arg);

    APP_DEBUG("idigi_run thread exits %d\n", status);
#if 0
    PrintSummaryStack();
    PrintThreadStackInfo((long)stack_start);

    APP_DEBUG("idigi_run_thread: stack size = %ld\n", iik_stack_max_size);
#endif
    pthread_exit(arg);
}


void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("application_run thread starts\n");

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
