
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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <stdbool.h>

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

typedef struct 
{
    idigi_config_request_t request;
    bool initiate_test;
    int(*fptr)(void **, size_t, void **, size_t *);
	int status;
	int size;
} forcedErrorInfo_t;

extern forcedErrorInfo_t forcedErrorTable[];

extern int app_get_forced_error_ip_address(void **, size_t, void **, size_t *);
extern int app_get_forced_error_rx_keepalive_interval(void **, size_t, void **, size_t *);
extern int app_get_forced_error_device_type(void **, size_t, void **, size_t *);

extern delay_receive_state_t delay_receive_state;
extern reboot_state_t reboot_state;
extern int reboot_errno;

extern idigi_handle_t idigi_handle;

#endif /* APPLICATION_H_ */
