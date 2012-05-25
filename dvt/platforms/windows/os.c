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

 /**
  * @file
  *  @brief Functions used by the iDigi Connector to interface to the OS.
  *
  */
#include <malloc.h>
#include <time.h>
//#include <unistd.h>
#include "idigi_api.h"
#include "platform.h"

/**
 * @brief   Dynamically allocate memory
 *
 * Dynamically allocate memory, if you are not using malloc()
 * from the C library replace the malloc() call to an equivalent
 * call on your system.
 * 
 * @param [in] size  Number of bytes to allocate
 * 
 * @param [in] ptr  pointer to be filled in with the address of
 *                  the allocated memory
 *  
 * @retval true  Memory was allocated
 * 
 * @retval false  Memory was not allocated
 *
 * Example Usage:
 * @code
 *     status = os_malloc(len, &ptr);
 * @endcode 
 *  
 * @see os_free
 */
bool os_malloc(size_t const size, void ** ptr)
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

/**
 * @brief   Free Dynamically allocate memory.
 *
 * Free dynamically allocate memory, if you are not using 
 * free() from the C library replace the free() call to an 
 * equivalent call on your system. 
 * 
 * @param [in] ptr  pointer to memory to be freed
 *  
 * @retval void
 * 
 * Example Usage:
 * @code
 *     status = os_free(ptr);
 * @endcode 
 *  
 * @see os_free
 */
void os_free(void * const ptr)
{
    ASSERT(ptr != NULL);

    if (ptr != NULL)
    {
        free(ptr);
    }
    return;
}


extern time_t system_start_time;
/**
 * @brief   Get the system time.
 *
 * Get the current time (number of seconds since the start of
 * the Unix epoch January 1, 1970) from the system clock.
 * 
 * @param [in] uptime   Current system time in seconds.
 *  
 * @retval true Able to get system time
 * 
 * @retval false System time unavailable
 * 
 * Example Usage:
 * @code
 *     status = os_get_system_time(&uptime);
 * @endcode 
 *  
 */
bool os_get_system_time(uint32_t * const uptime)
{
    time((time_t *)uptime);

    return true;
}

idigi_callback_status_t idigi_os_callback(idigi_os_request_t const request,
                                        void * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    bool ret=false;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

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
        APP_DEBUG("idigi_os_callback: unrecognized request [%d]\n", request);
        break;
    }

    return status;
}


