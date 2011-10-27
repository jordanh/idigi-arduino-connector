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

/* OS interface routines for the IIK */

static bool os_malloc(size_t const size, void ** ptr)
{
    bool status=true;

    *ptr = malloc(size);
    ASSERT(*ptr != NULL);
    if (*ptr != NULL)
    {
        status = true;
    }

    return status;
}

static void os_free(void * const ptr)
{
    ASSERT(ptr != NULL);

    if (ptr != NULL)
    {
        free(ptr);
    }
    return;
}

static bool os_get_system_time(uint32_t * const uptime)
{
    time_t curtime;
    bool ret = time(&curtime);

    if (ret)
    {
        /* Up time in seconds */
        *uptime = (uint32_t)(curtime - device_data.start_system_up_time);
    }
    else
    {
        *uptime = 0; /* Time is not available */
    }

    return ret;
}

idigi_callback_status_t idigi_os_callback(idigi_os_request_t const request,
                                        void * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    bool ret=false;

    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        ret    = os_malloc(*((size_t *)request_data), (void **)response_data);
        status = (ret == true) ? idigi_callback_continue : idigi_callback_busy;
        break;

    case idigi_os_free:
        os_free(request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        ret    = os_get_system_time((uint32_t *)response_data);
        status = (ret == true) ? idigi_callback_continue : idigi_callback_abort;
        break;
    default:
        DEBUG_PRINTF("idigi_os_callback: unrecognized request [%d]\n", request);
        break;
    }

    return status;
}


