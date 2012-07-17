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
#include <unistd.h>
#include <errno.h>

#include "application.h"

idigi_handle_t idigi_handle;

pthread_t idigi_thread;
pthread_t application_thread;
//pthread_t send_terminate_thread;
idigi_status_t idigi_run_thread_status = idigi_success;

#define  STACK_INIT_VALUE   0xEF

__thread void * stack_top;
__thread void * stack_bottom;
__thread size_t stack_size_used = 0L;
__thread size_t stack_size = 0L;

static FILE * file_fd;
bool file_written = false;

static void * PrintThreadStackInit(size_t * StackSize, size_t * GuardSize)
{
    pthread_attr_t Attributes;
    void *StackTop;
    void *StackBottom;

    /* Get the pthread attributes */
    memset (&Attributes, 0, sizeof (Attributes));
    pthread_getattr_np (pthread_self(), &Attributes);
    /* From the attributes, get the stack info */
    pthread_attr_getstack (&Attributes, &StackTop, StackSize);
    pthread_attr_getguardsize(&Attributes, GuardSize);

    /* Done with the attributes */
    pthread_attr_destroy (&Attributes);

    StackBottom = (void *)((size_t)StackTop + *StackSize);

#ifdef THREAD_STACK_SIZE_DEBUG
    APP_DEBUG ("-------------------------------------\n");
    APP_DEBUG ("Thread Stack top:        %p\n", StackTop);
    APP_DEBUG ("Thread Stack size:       %zu bytes\n", *StackSize);
    APP_DEBUG ("Thread Stack guard size: %zu bytes\n", *GuardSize);
    APP_DEBUG ("Thread Min Stack size:   %d bytes\n", PTHREAD_STACK_MIN);
    APP_DEBUG ("Thread Stack bottom:     %p\n", StackBottom);
    APP_DEBUG ("-------------------------------------\n");
#endif
    return StackBottom;
}

void clear_stack_size(void)
{
    if (pthread_self() == idigi_thread)
    {
        unsigned char * ptr;
        size_t size;

        char nowhere;

        /* address of 'nowhere' approximates end of stack */
        void* stack_end = (void*)((long)&nowhere & 0xFFFFFFF4);

        /* may want to double check stack grows downward on your platform */
        size = (size_t)stack_bottom - (size_t)stack_end;

        if (size > stack_size_used)
        {
            stack_size_used = size;
        }

        for (ptr = stack_top; ptr < (unsigned char *)((size_t)stack_end-sizeof stack_end); ptr++)
        {
            *ptr = STACK_INIT_VALUE;
        }

    }
}

static size_t PrintSummaryStack(void)
{
    uint8_t * ptr = (uint8_t *)stack_top;

    while (*ptr == STACK_INIT_VALUE)
    {
        ptr++;
    }
    APP_DEBUG("======================================\n");
#ifdef THREAD_STACK_SIZE_DEBUG
    APP_DEBUG("idigi_run thread Stack End        = %p\n", stack_top);
    APP_DEBUG("idigi_run thread Stack Ptr        = %p\n", ptr);
    APP_DEBUG("idigi_run thread Stack Start      = %p\n", stack_bottom);
#endif
    APP_DEBUG("idigi_run thread Stack Size Used  = %d bytes\n", (unsigned)((uint8_t *)stack_bottom-ptr));
    return (size_t)((uint8_t *)stack_bottom-ptr);
}

static void open_stack_info_file(char * const filename)
{
//    FILE * file_fd;

    if (file_written) goto done;

    file_fd = fopen(filename, "a");
    if (file_fd == NULL)
    {
        APP_DEBUG("main: fopen failed on %s file\n", filename);
        goto done;
    }
    APP_DEBUG("open_stack_info_file--->\n");
    fprintf(file_fd, STACKSIZE_HTML_OPEN_BODY);
    fprintf(file_fd, STACKSIZE_HTML_TABLE);

    fprintf(file_fd, STACKSIZE_HTML_TABLE_ROW("%s"), "DVT Name", "stack_size");


done:
    return;
}

void write_stack_info_action(char const * const action)
{
    if (file_fd != NULL)
        fprintf(file_fd, STACKSIZE_HTML_TABLE_ROW("%s"), "Service", action);

}
static void write_stack_info_file(size_t const stack_size)
{
    if (file_fd == NULL || file_written)
    {
        goto done;
    }

    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST_OPEN);

#if defined IDIGI_DEBUG
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_DEBUG");
#endif

#if defined IDIGI_FIRMWARE_SERVICE
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_FIRMWARE_SERVICE");
#endif

#if defined IDIGI_COMPRESSION
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_COMPRESSION");
#endif

#if defined IDIGI_DATA_SERVICE
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_DATA_SERVICE");
#endif

#if defined IDIGI_FILE_SYSTEM
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_FILE_SYSTEM");
#endif

#if defined IDIGI_RCI_SERVICE
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_RCI_SERVICE");
#endif

#if defined IDIGI_DEVICE_TYPE
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_DEVICE_TYPE");
#endif

#if defined IDIGI_CLOUD_URL
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_RCI_SERVICE");
#endif

#if defined IDIGI_TX_KEEPALIVE_IN_SECONDS
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_TX_KEEPALIVE_IN_SECONDS");
#endif

#if defined IDIGI_RX_KEEPALIVE_IN_SECONDS
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_RX_KEEPALIVE_IN_SECONDS");
#endif

#if defined IDIGI_WAIT_COUNT
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_WAIT_COUNT");
#endif

#if defined IDIGI_VENDOR_ID
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_VENDOR_ID");
#endif

#if defined IDIGI_MSG_MAX_TRANSACTION
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_MSG_MAX_TRANSACTION");
#endif

#if defined IDIGI_CONNECTION_TYPE
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_CONNECTION_TYPE");
#endif

#if defined IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND
    fprintf(file_fd, "IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND");
#endif

#if defined IDIGI_WAN_PHONE_NUMBER_DIALED
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_WAN_PHONE_NUMBER_DIALED");
#endif

#if defined IDIGI_FIRMWARE_SUPPORT
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_FIRMWARE_SUPPORT");
#endif
#if defined IDIGI_DATA_SERVICE_SUPPORT
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_DATA_SERVICE_SUPPORT");
#endif
#if defined IDIGI_FILE_SYSTEM_SUPPORT
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_FILE_SYSTEM_SUPPORT");
#endif
#if defined IDIGI_REMOTE_CONFIGURATION_SUPPORT
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_REMOTE_CONFIGURATION_SUPPORT");
#endif

#if defined IDIGI_DEVICE_ID_METHOD
    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST, "IDIGI_DEVICE_ID_METHOD");
#endif
    fprintf(file_fd, STACKSIZE_HTML_TABLE_ROW("%zu"),"Maximum stack size in bytes", stack_size);

    fprintf(file_fd, STACK_SIZE_HTML_OPTION_LIST_CLOSE);

done:
    return;
}

static void close_stack_info_file(void)
{
//    FILE * file_fd;

    if (file_fd == NULL || file_written)
    {
        goto done;
    }
    fprintf(file_fd, STACKSIZE_HTML_CLOSE_BODY);

    fclose(file_fd);

//    file_written = true;
    file_fd = NULL;

    APP_DEBUG("close_stack_info_file<---\n");


done:
    return;
}


void * idigi_run_thread(void * arg)
{

    void * threadStackStart;
    size_t threadGuardSize;
    unsigned char * ptr;

    char nowhere;
    stack_bottom = (void*)&nowhere;

    threadStackStart = PrintThreadStackInit(&stack_size, &threadGuardSize);

    {
        size_t stackused = (size_t)((long)threadStackStart - (long)stack_bottom);
        stack_size -= stackused;
        stack_size -= threadGuardSize;
    }

    stack_top = (void *)((long)stack_bottom - stack_size);
    //memset(stack_top , STACK_INIT_VALUE, stack_size);
    for (ptr = stack_top; ptr < (unsigned char *)stack_bottom; ptr++)
    {
        *ptr = STACK_INIT_VALUE;
    }

    APP_DEBUG("idigi_run_thread starts %d Stack = %p to %p (size = %zu)\n", getpid(), stack_top, stack_bottom, stack_size);

    idigi_run_thread_status = idigi_success;

    while (idigi_run_thread_status == idigi_success)
    {
        idigi_run_thread_status = idigi_run(arg);
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

    PrintSummaryStack();
    write_stack_info_file(stack_size_used);
    APP_DEBUG("stack size between idigi_run and callback = %zu\n", stack_size_used);

    idigi_run_thread_status = idigi_device_terminated;

    pthread_exit(NULL);

    return NULL;
}


void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("application_run_thread starts\n");

    status = application_run(arg);

    APP_DEBUG("application_run thread exits %d\n", status);

    pthread_exit(NULL);

    return NULL;
}


void print_error(int error_no, char * const error_message)
{
    errno = error_no;
    perror("Error:");
    perror(error_message);
    ASSERT(0);
}

int start_idigi_thread(void)
{
    int ccode = -1;

    idigi_handle = idigi_init(app_idigi_callback);
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
    int rc = 0;


    for (;;)
    {
        APP_DEBUG("Start iDigi\n");
        open_stack_info_file(STACK_SIZE_FILE);

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

        if (total_malloc_size != 0)
        {
            /* terminate iik so it will not reconnect to iDigi */
            APP_DEBUG("total malloc memory = %zu after all threads are canceled\n", total_malloc_size);
            if (idigi_run_thread_status == idigi_device_terminated)
            {
                APP_DEBUG("Error: idigi_run has been terminated by idigi_initiate_terminate but total malloc memory is not 0 after all threads are canceled\n");
                if (file_fd != NULL)
                {
                    fprintf(file_fd, "<h1>Memory Leak</h1>\n");
                    fprintf(file_fd, "<p>Total active malloc memory after idigi_run is terminated = %zu</p>\n", total_malloc_size);
                }
            }
        }
        close_stack_info_file();
        usleep(1000000);
        /* continue and reconnect iDigi so python test will not fail */
    }

done:
    APP_DEBUG("Done %d.\n", rc);
    return rc;
}
