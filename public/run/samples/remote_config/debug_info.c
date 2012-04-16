/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
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
#include <pthread.h>
#include <stdio.h>
#include "idigi_config.h"
#include "idigi_api.h"
/* #include "platform.h" */
#include "remote_config.h"


typedef struct {
    void * stacktop;
    void * stackbottom;
    size_t stacksize;
    char version[12];
} thread_info_t;

thread_info_t idigiThreadInfo = {NULL, NULL, 0, "\0"};

extern size_t total_malloc_size;

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
    sprintf(idigiThreadInfo.version, "%02x.%02x.%02x.%02x", (((int)IDIGI_VERSION_1100 >> 24) & 0xFF), (((int)IDIGI_VERSION_1100 >> 16) & 0xFF),
                                                           (((int)IDIGI_VERSION_1100 >> 8) & 0xFF), ((int)IDIGI_VERSION_1100 & 0xFF));

    return;
}

idigi_callback_status_t app_debug_info_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{

    ASSERT(response->element_data.element_value != NULL);

    switch (request->element.id)
    {
    case idigi_state_debug_info_version:
    {
        char * ptr;
        ptr = idigiThreadInfo.version;
        response->element_data.element_value->string_value.buffer = ptr;
        response->element_data.element_value->string_value.length_in_bytes = sizeof(idigiThreadInfo.version);
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
    case idigi_state_debug_info_usedmem:
        response->element_data.element_value->integer_unsigned_value =  total_malloc_size;

    }
    return idigi_callback_continue;
}
