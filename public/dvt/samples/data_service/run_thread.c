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
//#include <sys/resource.h>
//#include <sys/types.h>


__thread void* stack_start;
__thread long stack_max_size = 0L;

void check_stack_size(void)
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

