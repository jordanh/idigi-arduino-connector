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
  *  @brief Functions used by the iDigi connector to interface to the OS.
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
 * @retval idigi_callback_continue  Memory was allocated.
 * 
 * @retval idigi_callback_abort     Memory was not allocated and abort iDigi connector.
 *
 * @see os_free
 * @see @ref malloc API Operating System Callback
 */
idigi_callback_status_t app_os_malloc(size_t const size, void ** ptr)
{
    UNUSED_ARGUMENT(size);
    UNUSED_ARGUMENT(ptr);
    return idigi_callback_continue;
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
 * by the iDigi connector.
 *
 * 
 * @param [in] uptime   Current system time in seconds.
 *  
 * @retval idigi_callback_continue Able to get system time
 * 
 * @retval idigi_callback_abort    System time unavailable and abort iDigi connector.
 * 
 * @see @ref uptime API Operating System Callback
 */
idigi_callback_status_t app_os_get_system_time(unsigned long * const uptime)
{
    UNUSED_ARGUMENT(uptime);
    return idigi_callback_continue;
}

/**
 * @brief   Sleep or relinquish for other task execution.
 *
 * Sleep or relinquish to run other task. This is called
 * to let other task to be executed when idigi_run is called.
 * iDigi connector calls this callback if iDigi connector is busy and is not calling
 * receive callback
 *
 * @param [in] timeout_in_seconds  Maximum number in seconds to sleep
 *
 * @retval idigi_callback_continue  It successfully sleeps or relinquishes for other task execution.
 * @retval idigi_callback_abort     Unable to sleep or relinquish for other task execution and abort iDigi connector.
 *
 * @see @ref sleep API Operating System Callback
 */
idigi_callback_status_t app_os_sleep(unsigned int const timeout_in_seconds)
{
    UNUSED_ARGUMENT(timeout_in_seconds);
    return idigi_callback_continue;
}

/**
 * @cond DEV
 */
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


