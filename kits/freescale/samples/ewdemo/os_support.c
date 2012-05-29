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
#include <stdlib.h>
#include <main.h>
#include "os_support.h"
#include "idigi_api.h"
#include "platform.h"

static char const * event_list[] =
{
    "idigi_send"
};

static size_t const event_list_size = asizeof(event_list);

idigi_connector_error_t ic_create_event(int const event)
{
    idigi_connector_error_t status = idigi_connector_event_error;

    ASSERT_GOTO(event < event_list_size, error);
    if (_event_create(event_list[event]) != MQX_OK)
    {
        APP_DEBUG("Failed to create %s event\n", event_list[event]);
        goto error;
    }

    status = idigi_connector_success;

error:
    return status;
}

idigi_connector_error_t ic_get_event(int const event, unsigned long const event_bit, unsigned long timeout_ms)
{
    pointer event_ptr;
    idigi_connector_error_t status = idigi_connector_event_error;

    ASSERT_GOTO(event < event_list_size, error);
    if (_event_open(event_list[event], &event_ptr) != MQX_OK)
    {
        APP_DEBUG("Failed to open event(get) %s\n", event_list[event]);
        goto error;
    }

    if (_event_wait_all(event_ptr, event_bit, timeout_ms) != MQX_OK)
    {
        APP_DEBUG("Failed to wait on event %s\n", event_list[event]);
        goto error;
    }

    if (_event_clear(event_ptr, event_bit) != MQX_OK)
    {
        APP_DEBUG("Failed to clear event %s\n", event_list[event]);
        goto error;
    }
    
    _event_close(event_ptr);

    status = idigi_connector_success;

error:
    return status;
}

idigi_connector_error_t ic_set_event(int const event, unsigned long const event_bit)
{
    pointer event_ptr;
    idigi_connector_error_t status = idigi_connector_event_error;

    ASSERT_GOTO(event < event_list_size, error);
    if (_event_open(event_list[event], &event_ptr) != MQX_OK)
    {
        APP_DEBUG("Failed to open event(get) %s\n", event_list[event]);
        goto error;
    }

    if (_event_set(event_ptr, event_bit) != MQX_OK)
    {
        APP_DEBUG("Failed to clear event %s\n", event_list[event]);
        goto error;
    }

    status = idigi_connector_success;
    
    _event_close(event_ptr);

error:
    return status;
}

void * ic_malloc(size_t size)
{
	void *ptr;
	
	ptr = _mem_alloc(size);
	
#ifdef DEBUG_MALLOC
    APP_DEBUG ("ic_malloc: ptr = [%x] size = [%d]\n", ptr, size);
#endif
	
    return ptr;
	
    // return _mem_alloc(size);
}

void ic_free(void * ptr)
{
	unsigned int result;
	
    if (ptr != NULL)
    {

#ifdef DEBUG_MALLOC
        APP_DEBUG ("ic_free: ptr = [%x]\n", ptr);
#endif
        result = _mem_free(ptr);
    	if (result)
    	{
    		APP_DEBUG("ic_free: _mem_free failed [%x]\n", result);
    	}
    }
    else
    {
        APP_DEBUG("Free called with NULL pointer\n");
    }
}

unsigned long ic_create_task(unsigned long const index_number, unsigned long const parameter)
{
    uint32_t task_id;
	
    task_id = _task_create(0, index_number, parameter);
    
    return task_id;
}

unsigned long ic_destroy_task(unsigned long const task_id)
{
    unsigned long status;
	    
    status = _task_destroy(task_id);	
    
    return status;
}

void ic_software_reset(void)
{
    #define VECTKEY  0x05FA0000

    // Issue a System Reset Request
    SCB_AIRCR = VECTKEY | SCB_AIRCR_SYSRESETREQ_MASK;

    while (1);
}

void ic_watchdog_reset(void)
{
    /* Issue a watchdog */

    /* disable all interrupts */
    asm(" CPSID i");

    /* Write 0xC520 to the unlock register */ WDOG_UNLOCK = 0xC520;

    /* Followed by 0xD928 to complete the unlock */ WDOG_UNLOCK = 0xD928;

    /* enable all interrupts */
    asm(" CPSIE i");

    /* Clear the WDOGEN bit to disable the watchdog */ //WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
	
    WDOG_STCTRLH != WDOG_STCTRLH_WDOGEN_MASK;
}

idigi_connector_error_t ic_create_thread(void)
{
    idigi_connector_error_t status = idigi_connector_success;
    _task_id const connector_task_id = _task_create(0, IDIGI_CONNECTOR_TASK, 0);

    if (connector_task_id == MQX_NULL_TASK_ID)
    {
        APP_DEBUG("Failed to create IDIGI_CONNECTOR_TASK\n");
        status = idigi_connector_failed_to_create_thread;
    }

error:
    return status;
}
