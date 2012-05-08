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

typedef struct {
    unsigned int group_table_id;
    void * group_context;
} remote_group_session_t;

extern int app_os_malloc(size_t const size, void ** ptr);
extern void app_os_free(void * const ptr);

extern idigi_callback_status_t app_serial_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_serial_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_serial_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_serial_group_end(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern void app_serial_group_cancel(void * context);

extern idigi_callback_status_t app_ethernet_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_ethernet_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_ethernet_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_ethernet_group_end(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern void app_ethernet_group_cancel(void * context);

extern idigi_callback_status_t app_device_stats_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_device_stats_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_device_stats_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);

extern idigi_callback_status_t app_device_info_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_device_info_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_device_info_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_device_info_group_end(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern void app_device_info_group_cancel(void * context);

extern idigi_callback_status_t app_debug_info_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_gps_stats_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);

extern idigi_callback_status_t app_system_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_system_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);

extern idigi_callback_status_t app_device_security_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);
extern idigi_callback_status_t app_device_security_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response);


#endif /* REMOTE_CONFIG_CB_H_ */
