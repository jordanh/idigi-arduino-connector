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

/* If set, IDK will start as a separated thread calling iik_run */
#define IIK_THREAD  0

time_t  deviceSystemUpStartTime;


iik_callback_status_t idigi_callback(iik_class_t class, iik_request_t request,
                                    void const * request_data, size_t request_length,
                                    void * response_data, size_t * response_length)
{
    iik_callback_status_t   status = iik_callback_continue;

    switch (class)
    {
    case iik_class_config:
        status = idigi_config_callback(request.config_request, request_data, request_length, response_data, response_length);
        break;
    case iik_class_operating_system:
        status = idigi_os_callback(request.os_request, request_data, request_length, response_data, response_length);
        break;
    case iik_class_network:
        status = idigi_network_callback(request.network_request, request_data, request_length, response_data, response_length);
        break;
    case iik_class_firmware:
        status = idigi_firmware_callback(request.firmware_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}


#if IIK_THREAD

#include <pthread.h>

void * iik_process_thread(void * arg)
{
    iik_status_t status;

    printf("IDK Process thread starts...\n");

    status = iik_run((iik_handle_t)arg);

    printf("IDK process thread exits... %d\n", status);

    pthread_exit(arg);

}

int main (void)
{
    pthread_t   iik_thread;
    void * idkdone;

    time(&deviceSystemUpStartTime);

    iDigiSetting.iik_handle = iik_init((iik_callback_t) idigi_callback);
    if (iDigiSetting.iik_handle != 0)
    {

        if (pthread_create(&iik_thread, NULL, iik_process_thread, iDigiSetting.iik_handle) != 0)
        {
            perror("thread_create() error on iik_process_thread");
            goto done;
        }

        if (pthread_join(iik_thread, &idkdone) != 0)
        {
            perror("thread_join() error on my_process_thread");
            goto done;
        }
    }
    else
    {
        printf("unable to initialize IDK\n");
    }
done:
    return 0;
}
#else

int main (void)
{
    iik_status_t status = iik_success;

    time(&deviceSystemUpStartTime);


    iDigiSetting.iik_handle = iik_init((iik_callback_t) idigi_callback);
    if (iDigiSetting.iik_handle != 0)
    {

        while (status == iik_success)
        {
            status = iik_step(iDigiSetting.iik_handle);
            iDigiSetting.select_data = 0;

            if (status != iik_success)
            {
                printf("main: iik_task returns error %d\n", status);
            }
            else
            {
                iDigiSetting.select_data |= NETWORK_TIMEOUT_SET | NETWORK_READ_SET;
                network_select(iDigiSetting.socket_fd, iDigiSetting.select_data, 1);
            }
        }
   }
    else
    {
        printf("unable to initialize IDK\n");
    }
    return 0;
}

#endif
