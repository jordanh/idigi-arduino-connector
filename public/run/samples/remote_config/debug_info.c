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
    void * stacktop;
    void * stackbottom;
    size_t stacksize;
    char version[12];
} thread_info_t;

thread_info_t idigiThreadInfo = {NULL, NULL, 0, "\0"};

void idigiThreadStackInit(void)
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

    idigiThreadInfo.stacktop = StackTop;
    idigiThreadInfo.stackbottom = (void *)((size_t)StackTop + StackSize);
    idigiThreadInfo.stacksize = StackSize;
    sprintf(idigiThreadInfo.version, "%02x.%02x.%02x.%02x", (((int)IDIGI_VERSION >> 24) & 0xFF), (((int)IDIGI_VERSION >> 16) & 0xFF),
                                                           (((int)IDIGI_VERSION >> 8) & 0xFF), ((int)IDIGI_VERSION & 0xFF));

    return;
}

idigi_callback_status_t app_debug_info_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response)
{

    ASSERT(response->element_data.element_value != NULL);

    switch (request->element.id)
    {
    case idigi_state_debug_info_version:
    {
        char * ptr;
        ptr = idigiThreadInfo.version;
        response->element_data.element_value->string_value = ptr;
        break;
    }
    case idigi_state_debug_info_stacktop:
        response->element_data.element_value->integer_unsigned_value =  (uint32_t)idigiThreadInfo.stacktop;
        break;
    case idigi_state_debug_info_stacksize:
        response->element_data.element_value->integer_unsigned_value =  idigiThreadInfo.stacksize;
        break;
    case idigi_state_debug_info_stackbottom:
        response->element_data.element_value->integer_unsigned_value =  (uint32_t)idigiThreadInfo.stackbottom;
        break;
    }
    return idigi_callback_continue;
}
