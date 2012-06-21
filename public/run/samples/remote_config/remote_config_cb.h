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

#ifndef REMOTE_CONFIG_CB_H_
#define REMOTE_CONFIG_CB_H_

#include "remote_config.h"
#include "idigi_debug.h"

typedef struct {
    unsigned int group_table_id;
    void * group_context;
} remote_group_session_t;

extern idigi_callback_status_t app_serial_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_serial_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_serial_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_serial_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern void app_serial_group_cancel(void * const context);

extern idigi_callback_status_t app_ethernet_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_ethernet_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_ethernet_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_ethernet_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern void app_ethernet_group_cancel(void * const context);

extern idigi_callback_status_t app_device_time_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_time_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_time_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_device_info_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_info_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_info_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_info_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern void app_device_info_group_cancel(void * const context);

extern idigi_callback_status_t app_debug_info_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_gps_stats_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);

extern idigi_callback_status_t app_system_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_system_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_system_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_system_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern void app_system_group_cancel(void * const context);

extern idigi_callback_status_t app_device_security_group_init(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_security_group_set(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_security_group_get(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern idigi_callback_status_t app_device_security_group_end(idigi_remote_group_request_t const * const request, idigi_remote_group_response_t * const response);
extern void app_device_security_group_cancel(void * const context);


#endif /* REMOTE_CONFIG_CB_H_ */
