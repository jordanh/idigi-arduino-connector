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
 * Routines used for common IC code.
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
} idigi_connector_info_t;

typedef struct
{
    void * next_data;
    size_t bytes_remaining;
    idigi_connector_error_t error;
} idigi_app_send_data_t;

#define IC_SEND_DATA_EVENT 0
#define IC_MAX_NUM_EVENTS  1

idigi_connector_error_t ic_create_event(int const event);
idigi_connector_error_t ic_set_event(int const event);
idigi_connector_error_t ic_get_event(int const event, unsigned long timeout);
idigi_connector_error_t ic_create_thread(void);
void ic_free(void *ptr);
void *ic_malloc(size_t size);

void idigi_connector_thread(unsigned long initial_data);
extern idigi_connector_info_t * idigi_get_app_callbacks(void);

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



