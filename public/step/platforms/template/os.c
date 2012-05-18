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

#include "idigi_api.h"
#include "platform.h"

idigi_callback_status_t app_os_malloc(size_t const size, void ** ptr)
{
    UNUSED_ARGUMENT(size);
    UNUSED_ARGUMENT(ptr);
    return idigi_callback_continue;
}

void app_os_free(void * const ptr)
{
    UNUSED_ARGUMENT(ptr);
    return;
}

idigi_callback_status_t app_os_get_system_time(unsigned long * const uptime)
{
    UNUSED_ARGUMENT(uptime);
    return idigi_callback_continue;
}

idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds)
{
    UNUSED_ARGUMENT(timeout_in_seconds);
    return idigi_callback_continue;
}

idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                                        void const * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        status    = app_os_malloc(*((size_t *)request_data), response_data);
        break;

    case idigi_os_free:
        app_os_free((void * const)request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        status    = app_os_get_system_time(response_data);
        break;

    case idigi_os_sleep:
        status = app_os_sleep(*((unsigned int *)request_data));
        break;

    default:
        status = idigi_callback_unrecognized;
        break;
    }

    return status;
}
/**
 * @endcond
 */


