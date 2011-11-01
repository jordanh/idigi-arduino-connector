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

 /**
  * @file
  *  @brief Functions used by the IIK to interface to the OS.
  *
  */
#include <malloc.h>
#include <time.h>
#include <unistd.h>
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
 *     os_free(ptr);
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

/**
 * @brief   Sleep or relinquish for other task execution.
 *
 * Sleep or relinquish to run other task. This is called
 * to let other task to be executed when iik_run is called.
 * IIK calls this callback if IIK is busy and is not calling
 * receive callback
 *
 * @param [in] timeout  Maximum number in seconds to sleep
 *
 * @retval void
 *
 * Example Usage:
 * @code
 *     os_wait(1);
 * @endcode
 *
 * @see os_free
 */
void os_sleep(unsigned int const timeout)
{
    usleep(timeout * 1000 * 1000);
    return;
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

    case idigi_os_sleep:
        os_sleep(*((unsigned int *)request_data));
        status = idigi_callback_continue;
        break;

    default:
        APP_DEBUG("idigi_os_callback: unrecognized request [%d]\n", request);
        break;
    }

    return status;
}


