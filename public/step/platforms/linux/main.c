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
 *
 * Platform using step in a Linux environment
 * =======================================================================
 *
 */
#include <stdlib.h>
#include <unistd.h>
#include "idigi_api.h"
#include "platform.h"

int main (void)
{
    int status=EXIT_FAILURE;
    idigi_handle_t idigi_handle;

    APP_DEBUG("main: Starting iDigi\n");
    idigi_handle = idigi_init(app_idigi_callback);
    if (idigi_handle == NULL)
    {
        APP_DEBUG("main: idigi_init() failed\n");
        goto done;
    }

    /* Sample program control loop */
    for(;;)
    {
        idigi_status_t status;
        /*
         * Example of using idigi_step(), we run a portion of the IIK then run
         * a portion of the users application.
         */
        status = idigi_step(idigi_handle);
        if (status != idigi_success && status != idigi_receive_error &&
            status != idigi_send_error && status != idigi_connect_error)
        {
            /* We exclude idigi_receive_error, idigi_send_error, &
             * idigi_connect_error. We want to reconnect iDigi
             * even if idigi_step returns error in receive, send, or connect.
             */
            APP_DEBUG("main: idigi_step() failed\n");
            break;
        }
        if (application_step(idigi_handle) != 0)
        {
            APP_DEBUG("main: application_step() failed\n");
            goto done;
        }
        usleep(1000);
    }

done:
    return status;
}
