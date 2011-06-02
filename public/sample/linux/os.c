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
#include "idigi_data.h"


static iik_callback_status_t os_malloc(size_t size, void ** ptr)
{
    iik_callback_status_t rc = iik_callback_continue;
    *ptr = malloc(size);
    if (*ptr == NULL)
    {
        rc = iik_callback_busy;
    }
    return rc;
}

static iik_callback_status_t os_free(void * ptr)
{
    free(ptr);
    return iik_callback_continue;
}


static iik_callback_status_t os_get_system_time(uint32_t * mstime)
{
    time_t      curtime;

    time(&curtime);

    *mstime = (uint32_t)(curtime - deviceSystemUpStartTime) * 1000;

    return iik_callback_continue;
}

iik_callback_status_t idigi_os_callback(iik_os_request_t request,
                                        void const * request_data, size_t request_length,
                                        void * response_data, size_t * response_length)
{
    iik_callback_status_t status = iik_callback_continue;

    switch (request)
    {
    case iik_os_malloc:
        status = os_malloc(*((size_t *)request_data), (void **)response_data);
        break;

    case iik_os_free:
        status = os_free((void *)request_data);
        break;

    case iik_os_system_up_time:
        status = os_get_system_time((uint32_t *)response_data);
        break;
    }

    return status;
}


