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
#include <pthread.h>
#include <stdio.h>
#include "idigi_config.h"
#include "idigi_api.h"
/* #include "platform.h" */
#include "remote_config_cb.h"


typedef struct {
    size_t stacksize;
    unsigned int version;
} thread_info_t;

static thread_info_t idigiThreadInfo = {0, IDIGI_VERSION};

static void idigiThreadStackInit(void)
{
    pthread_attr_t Attributes;
    void *StackTop;
    size_t StackSize;

    /* Get the pthread attributes */
    memset (&Attributes, 0, sizeof (Attributes));
    pthread_getattr_np (pthread_self(), &Attributes);
    /* From the attributes, get the stack info */
    pthread_attr_getstack (&Attributes, &StackTop, &StackSize);
    /* pthread_attr_getguardsize(&Attributes, GuardSize); */

    /* Done with the attributes */
    pthread_attr_destroy (&Attributes);

    idigiThreadInfo.stacksize = StackSize;

    return;
}

idigi_callback_status_t app_debug_info_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{

    ASSERT(response->element_data.element_value != NULL);

    if (idigiThreadInfo.stacksize == 0)
    {
        idigiThreadStackInit();
    }

    switch (request->element.id)
    {
    case idigi_state_debug_info_version:
        response->element_data.element_value->unsigned_integer_value = idigiThreadInfo.version;
        break;
    case idigi_state_debug_info_stacksize:
        response->element_data.element_value->unsigned_integer_value =  idigiThreadInfo.stacksize;
        break;
    }
    return idigi_callback_continue;
}
