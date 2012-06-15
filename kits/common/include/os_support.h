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

#ifndef _OS_SUPPORT_H
#define _OS_SUPPORT_H

#include <stdlib.h>
#include "idigi_connector.h"

typedef struct
{
    idigi_status_callback_t status_callback;
    idigi_device_request_callback_t device_request;
    idigi_device_response_callback_t device_response;
} idigi_connector_callbacks_t;

#define IC_SEND_DATA_EVENT 0
#define IC_MAX_NUM_EVENTS  1

idigi_connector_error_t ic_create_event(int const event);
idigi_connector_error_t ic_set_event(int const event, unsigned long const event_bit);
idigi_connector_error_t ic_get_event(int const event, unsigned long const event_bit, unsigned long timeout);
idigi_connector_error_t ic_clear_event(int const event, unsigned long const event_bit);
idigi_connector_error_t ic_create_thread(void);
void ic_free(void *ptr);
void *ic_malloc(size_t size);
void ic_software_reset(void);
void ic_watchdog_reset(void);

void idigi_connector_thread(unsigned long initial_data);
extern idigi_connector_callbacks_t * idigi_get_app_callbacks(void);
extern unsigned long start_system_up_time;

#define UNUSED_PARAMETER(x)     ((void)x)

#define ON_FALSE_DO_(cond, code)        do { if (!(cond)) {code;} } while (0)

#if defined(IDIGI_DEBUG)
#define ON_ASSERT_DO_(cond, code, output)   ON_FALSE_DO_((cond), {ASSERT(cond); code;})
#else
#define ON_ASSERT_DO_(cond, code, output)   ON_FALSE_DO_((cond), {code})
#endif

#define ASSERT_GOTO(cond, label)    ON_ASSERT_DO_((cond), {goto label;}, {})
#define CONFIRM(cond)               do { switch(0) {case 0: case (cond):;} } while (0)

#endif



