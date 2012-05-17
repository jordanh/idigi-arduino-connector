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

#ifndef REMOTE_CONFIG_CB_H_
#define REMOTE_CONFIG_CB_H_

#include "remote_config.h"

extern idigi_callback_status_t app_keepalive_session_start(idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_session_end(idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_action_start(idigi_remote_group_request_t const * const request,
                                                        idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_action_end(idigi_remote_group_request_t const * const request,
                                                      idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_group_init(idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_group_set(idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_group_get(idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_keepalive_group_end(idigi_remote_group_request_t const * const request,
                                                 idigi_remote_group_response_t * const response);

extern void app_keepalive_session_cancel(void * const context);

#endif /* REMOTE_CONFIG_CB_H_ */
