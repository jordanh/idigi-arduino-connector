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

#include <malloc.h>
#include <time.h>
#include <unistd.h>
#include "idigi_api.h"
#include "platform.h"

typedef struct malloc_info{
    void const * ptr;
    size_t length;
    struct malloc_info * prev;
    struct malloc_info * next;
} malloc_info_t;

static malloc_info_t  * malloc_info_list = NULL;
size_t          total_malloc_size = 0;

static void add_malloc_ptr(void const * const ptr, size_t const length)
{
    malloc_info_t   * pMalloc;

    pMalloc = malloc(sizeof *pMalloc);
    ASSERT(pMalloc != NULL);

    if (pMalloc != NULL)
    {
        pMalloc->ptr = ptr;
        pMalloc->length = length;

        if (malloc_info_list != NULL)
        {
            malloc_info_list->prev = pMalloc;
        }
        pMalloc->next = malloc_info_list;
        pMalloc->prev = NULL;
        malloc_info_list = pMalloc;

        total_malloc_size += length;

    }
}

/* free and remove allocated memory */
static void free_malloc_ptr(void const * const ptr)
{
    malloc_info_t   * pMalloc;

    for (pMalloc = malloc_info_list; pMalloc != NULL; pMalloc = pMalloc->next)
    {
        if (pMalloc->ptr == ptr)
        {
            if (pMalloc->next != NULL) \
            {\
                pMalloc->next->prev = pMalloc->prev;\
            }\
            if (pMalloc->prev != NULL) \
            {\
                pMalloc->prev->next = pMalloc->next;\
            }\
            if (pMalloc == malloc_info_list)\
            {\
                malloc_info_list = pMalloc->next;\
            }\

            total_malloc_size -= pMalloc->length;
            free(pMalloc);
            break;
        }
    }

    ASSERT(pMalloc != NULL);
}

idigi_callback_status_t app_os_malloc(size_t const size, void ** ptr)
{
    idigi_callback_status_t status= idigi_callback_abort;

    *ptr = malloc(size);
    ASSERT(*ptr != NULL);
    if (*ptr != NULL)
    {
#if 0
        size_t i;
        uint8_t * thisPtr = *ptr;
        for (i=0; i<size; i++)
        {
            *(thisPtr + i) = 0xCD;
        }
#else
        memset(*ptr, 0xCD, size);
#endif
        status = idigi_callback_continue;
        add_malloc_ptr(*ptr, size);
    }

    return status;
}

void app_os_free(void * const ptr)
{
    ASSERT(ptr != NULL);

    if (ptr != NULL)
    {
        free_malloc_ptr(ptr);
        free(ptr);
    }
    return;
}

idigi_callback_status_t app_os_get_system_time(unsigned long * const uptime)
{
    time((time_t *)uptime);

    return idigi_callback_continue;
}

idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds)
{
    unsigned int const timeout_in_microseconds = timeout_in_seconds * 1000000;

    usleep(timeout_in_microseconds);
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
        APP_DEBUG("app_os_handler: unrecognized request [%d]\n", request);
        status = idigi_callback_unrecognized;
        break;
    }

    return status;
}


