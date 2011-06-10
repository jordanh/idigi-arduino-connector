/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
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

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "idigi_data.h"

/* If set, iDigi will start as a separated thread calling idigi_run */
#define IDIGI_THREAD  0

#define ONE_SECOND  1
time_t  deviceSystemUpStartTime;


idigi_callback_status_t idigi_callback(idigi_class_t class, idigi_request_t request,
                                    void const * request_data, size_t request_length,
                                    void * response_data, size_t * response_length)
{
    idigi_callback_status_t   status = idigi_callback_continue;

    switch (class)
    {
    case idigi_class_config:
        status = idigi_config_callback(request.config_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_operating_system:
        status = idigi_os_callback(request.os_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_network:
        status = idigi_network_callback(request.network_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_firmware:
        status = idigi_firmware_callback(request.firmware_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}


#if IDIGI_THREAD

#include <pthread.h>

void * idigi_process_thread(void * arg)
{
    idigi_status_t status;

    DEBUG_PRINTF("iDigi Process thread starts...\n");

    status = idigi_run((idigi_handle_t)arg);

    DEBUG_PRINTF("iDigi process thread exits... %d\n", status);

    pthread_exit(arg);

}

int main (void)
{
    pthread_t   idigi_thread;
    void * idkdone;

    time(&deviceSystemUpStartTime);

    iDigiSetting.idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
    if (iDigiSetting.idigi_handle != 0)
    {

        if (pthread_create(&idigi_thread, NULL, idigi_process_thread, iDigiSetting.idigi_handle) != 0)
        {
            perror("thread_create() error on idigi_process_thread");
            goto done;
        }

        if (pthread_join(idigi_thread, &idkdone) != 0)
        {
            perror("thread_join() error on my_process_thread");
            goto done;
        }
    }
    else
    {
        DEBUG_PRINTF("unable to initialize iDigi\n");
    }
done:
    return 0;
}
#else

int main (void)
{
    idigi_status_t status = idigi_success;

    time(&deviceSystemUpStartTime);

    DEBUG_PRINTF("Start iDigi\n");
    iDigiSetting.idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
    if (iDigiSetting.idigi_handle != 0)
    {

        while (status == idigi_success)
        {
            status = idigi_step(iDigiSetting.idigi_handle);
            iDigiSetting.select_data = 0;

            if (status == idigi_success)
            {
                iDigiSetting.select_data |= NETWORK_TIMEOUT_SET | NETWORK_READ_SET;
                network_select(iDigiSetting.socket_fd, iDigiSetting.select_data, ONE_SECOND);
            }
        }
        DEBUG_PRINTF("idigi status = %d\n", status);
   }
   DEBUG_PRINTF("iDigi stops running!\n");
    return 0;
}

#endif
