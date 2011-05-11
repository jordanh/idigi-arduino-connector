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
#ifndef EH_TCP_H_
#define EH_TCP_H_

#include <netinet/in.h>
#include "idk_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

idk_callback_status_t network_connect(idk_connect_request_t * connect_data);
idk_callback_status_t network_send(idk_write_request_t * write_data, size_t * sent_length);
idk_callback_status_t network_receive(idk_read_request_t * read_data, size_t * read_length);
idk_callback_status_t network_close(idk_network_handle_t * fd);
uint8_t network_select(idk_network_handle_t fd, uint8_t select_set, unsigned wait_time);

#ifdef __cplusplus
}
#endif

#endif /* EH_TCP_H_ */
