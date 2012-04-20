/*
 * Copyright (c) 1996-2012 Digi International Inc.,
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
  *  @brief Functions used by the IIK to interface to the OS.
  *
  */
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
 * @retval 0  Memory was allocated
 * 
 * @retval -1  Memory was not allocated
 *
 * @see os_free
 * @see @ref malloc API Operating System Callback
 */
int app_os_malloc(size_t const size, void ** ptr)
{
    UNUSED_ARGUMENT(size);
    UNUSED_ARGUMENT(ptr);
    return 0;
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
 * @retval none
 * 
 * @see os_malloc
 * @see @ref free API Operating System Callback
 */
void app_os_free(void * const ptr)
{
    UNUSED_ARGUMENT(ptr);
    return;
}

/**
 * @brief   Get the system time.
 *
 * Get the current system time in seconds, this is only used as a reference 
 * by the IIK.
 *
 * 
 * @param [in] uptime   Current system time in seconds.
 *  
 * @retval 0 Able to get system time
 * 
 * @retval -1 System time unavailable
 * 
 * @see @ref uptime API Operating System Callback
 */
int app_os_get_system_time(unsigned long * const uptime)
{
    UNUSED_ARGUMENT(uptime);
    return 0;
}

/**
 * @brief   Sleep or relinquish for other task execution.
 *
 * Sleep or relinquish to run other task. This is called
 * to let other task to be executed when idigi_run is called.
 * IIK calls this callback if IIK is busy and is not calling
 * receive callback
 *
 * @param [in] timeout_in_seconds  Maximum number in seconds to sleep
 *
 * @retval None
 *
 * @see @ref sleep API Operating System Callback
 */
void app_os_sleep(unsigned int const timeout_in_seconds)
{
    UNUSED_ARGUMENT(timeout_in_seconds);
    return;
}

/**
 * @cond DEV
 */
idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                                        void * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int ret;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
    case idigi_os_malloc:
        ret    = app_os_malloc(*((size_t *)request_data), (void **)response_data);
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_busy;
        break;

    case idigi_os_free:
        app_os_free(request_data);
        status = idigi_callback_continue;
        break;

    case idigi_os_system_up_time:
        ret    = app_os_get_system_time((unsigned long *)response_data);
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
        break;

    case idigi_os_sleep:
        app_os_sleep(*((unsigned int *)request_data));
        status = idigi_callback_continue;
        break;

    default:
        break;
    }

    return status;
}
/**
 * @endcond
 */


