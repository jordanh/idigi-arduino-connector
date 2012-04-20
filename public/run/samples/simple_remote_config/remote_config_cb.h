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

typedef idigi_callback_status_t(* remote_group_init_cb_t) (idigi_remote_group_request_t * request, idigi_remote_group_response_t * response, void ** group_context);
typedef idigi_callback_status_t(* remote_group_cb_t) (idigi_remote_group_request_t * request, idigi_remote_group_response_t * response, void * group_context);
typedef void (* remote_group_cancel_cb_t) (void * context);

typedef struct remote_group_table {
    remote_group_init_cb_t init_cb;
    remote_group_cb_t set_cb;
    remote_group_cb_t get_cb;
    remote_group_cb_t end_cb;
    remote_group_cancel_cb_t cancel_cb;
} remote_group_table_t;


typedef struct {
    void * session_context;
    void * group_context;
    remote_group_table_t * group;
} remote_config_session_t;

extern int app_os_malloc(size_t const size, void ** ptr);
extern void app_os_free(void * const ptr);



#endif /* REMOTE_CONFIG_CB_H_ */
