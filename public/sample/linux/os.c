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
#include <malloc.h>
#include <time.h>
#include <unistd.h>
#include "os.h"


idk_callback_status_t os_malloc(size_t size, void ** ptr)
{
    idk_callback_status_t rc = idk_callback_continue;

	*ptr = malloc(size);
	if (*ptr == NULL)
	{
	    rc = idk_callback_busy;
	}
	return rc;
}

idk_callback_status_t os_free(void * ptr)
{
	free(ptr);
	return idk_callback_continue;
}

// clock_t gSysTime = 0;

idk_callback_status_t os_get_system_time(uint32_t * mstime)
{
#if 0
	clock_t		systime;
	systime = clock();

	*mstime = systime/CLOCKS_PER_SEC * 1000;
	if (gSysTime != (*mstime/1000))
	{
	    DEBUG_PRINTF("system_time = %d mstime = %d\n", (int) systime, *mstime/1000);
	    gSysTime = (*mstime/1000);
	}
#else
	extern time_t gSystemTime;

	time_t      curtime;

	time(&curtime);

	*mstime = (uint32_t)(curtime - gSystemTime) * 1000;

#endif
	return idk_callback_continue;
}

idk_callback_status_t os_wait(uint32_t mswait)
{
	usleep(mswait * 1000);
	return idk_callback_continue;
}


