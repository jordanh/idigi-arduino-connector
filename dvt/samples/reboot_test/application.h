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

#ifndef APPLICATION_H_
#define APPLICATION_H_

typedef enum {
    no_delay_receive,
    start_delay_receive,
    delay_receive_started,
    delay_receive_done
} delay_receive_state_t;

typedef enum {
    no_reboot_received,
    reboot_received,
    receive_reset,
    receive_error,
    send_error,
    reboot_timeout
} reboot_state_t;

extern delay_receive_state_t delay_receive_state;
extern reboot_state_t reboot_state;
extern int reboot_errno;

#endif /* APPLICATION_H_ */
