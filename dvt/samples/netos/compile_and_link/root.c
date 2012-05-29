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


#include <stdio.h>
#include <stdlib.h>
#include <tx_api.h>
#include <fservapi.h>

#include <idigi_api.h>

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


/*
 *
 *  Function: void applicationStart (void)
 *
 *  Description:
 *
 *      This routine is responsible for starting the user application.  It should 
 *      create any threads or other resources the application needs.
 *
 *      ThreadX, the NET+OS device drivers, and the TCP/IP stack will be running
 *      when this function is called.
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

void applicationStart (void)

{
    idigi_handle_t idigi_handle;

    /* start FTP server */
    naFtpDlInit(NA_FTP_DEF_USERS);

    idigi_handle = idigi_init(NULL);

    if (idigi_handle == NULL)
        printf("idigi_init error\n"); 
}



