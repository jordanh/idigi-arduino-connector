
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

#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <stdio.h>

#define APP_DEBUG  printf

#define UNUSED_PARAMETER(x)     ((void)x)

/* Callbacks for this platform */
extern idigi_callback_status_t idigi_os_callback(idigi_os_request_t const request,
                           void * const request_data, size_t const request_length,
                           void * response_data, size_t * const response_length);


idigi_callback_status_t idigi_network_callback(idigi_network_request_t const request,
                         void * const request_data, size_t const request_length,
                         void * response_data, size_t * const response_length);

idigi_callback_status_t idigi_config_callback(idigi_config_request_t const request,
                                              void * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * const response_length);

#endif /* _PLATFORM_H */
