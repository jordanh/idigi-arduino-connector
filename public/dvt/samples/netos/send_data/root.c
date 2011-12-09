/*
 *  Copyright (c) 1996-2007 Digi International Inc., All Rights Reserved
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
 * Description.
 * =======================================================================
 *  applicationStart() is the entry point for user applications.  This
 *  This function will be called after the kernel has started and the
 *  TCP/IP stack has loaded.  applicationTcpDown() is called periodically
 *  after the kernel has started while the system is waiting for the
 *  TCP/IP stack to start.  
 *
 *
 *
 *
 * Edit Date/Ver   Edit Description
 * ==============  =======================================================
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <tx_api.h>
#include <fservapi.h>

#include <pthread.h>
#include <idigi_api.h>
#include "platform.h"

extern idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);


/*
 *
 *  Function: void applicationTcpDown (void)
 *
 *  Description:
 *
 *      This routine will be called by the NET+OS root thread once every 
 *      clock tick while it is waiting for the TCP/IP stack to come up.  
 *      This function can increment a counter everytime it's called to 
 *      keep track of how long we've been waiting for the stack to start.
 *      If we've been waiting too long, then this function can do something
 *      to handle the error.  
 *
 *      This function will not be called once the stack has started.
 *
 *  Parameters:
 *
 *      none
 *
 *  Return Values:
 *
 *      none
 *
 */

void applicationTcpDown (void)

{
    static int ticksPassed = 0;

    ticksPassed++;
/*
 * Code to handle error condition if the stack doesn't come up goes here.
 */
}


extern idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

static void * idigi_run_thread(void * arg)
{
    idigi_status_t status;

    APP_DEBUG("idigi_run thread starts\n");

    status = idigi_run((idigi_handle_t)arg);

    APP_DEBUG("idigi_run thread exits %d\n", status);

    return arg;
}

static void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("idigi_run thread starts\n");

    status = application_run((idigi_handle_t)arg);

    APP_DEBUG("application_run thread exits %d\n", status);

    return arg;
}

void applicationStart (void)
{
    pthread_t idigi_thread;
    pthread_t application_thread;

    idigi_handle_t idigi_handle;

    /* start FTP server */
    naFtpDlInit(NA_FTP_DEF_USERS);

    APP_DEBUG("Start iDigi\n");
    idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
    if (idigi_handle != NULL)
    {
        int ccode;
        ccode = pthread_create(&idigi_thread, NULL, idigi_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("pthread_create() error on idigi_process_thread %d\n", ccode);
            goto done;
        }

        ccode = pthread_create(&application_thread, NULL, application_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("pthread_create() error on idigi_process_thread %d\n", ccode);
            goto done;
        }
    }
    else
    {
        printf("unable to initialize iDigi\n");
    }
done:
    return;
}




