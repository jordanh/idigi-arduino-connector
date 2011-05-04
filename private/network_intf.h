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
#ifndef _NETWORK_INTF_H
#define _NETWORK_INTF_H

//#define WINVER	0x0501

//#include <stdio.h>

//#include "e_network.h"
#include "config_intf.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IRL_PROTOCOL_VERSION	0x120


/*
 * MT version 2 message type defines.
 * Refer to EDP specification rev. 14.2 for a description of MT version 2.
 */
#define E_MSG_MT2_TYPE_VERSION								0x0004 /* C -> S */
#define E_MSG_MT2_TYPE_LEGACY_EDP_VERSION		0x0004 /* C -> S */
#define E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP	0x0001 /* C <- S */
#define E_MSG_MT2_TYPE_VERSION_OK						0x0010 /* C <- S */
#define E_MSG_MT2_TYPE_VERSION_BAD					0x0011 /* C <- S */
#define E_MSG_MT2_TYPE_SERVER_OVERLOAD			0x0012 /* C <- S */
#define E_MSG_MT2_TYPE_KA_RX_INTERVAL				0x0020 /* C -> S */
#define E_MSG_MT2_TYPE_KA_TX_INTERVAL				0x0021 /* C -> S */
#define E_MSG_MT2_TYPE_KA_WAIT								0x0022 /* C -> S */
#define E_MSG_MT2_TYPE_KA_KEEPALIVE					0x0030 /* bi-directional */
#define E_MSG_MT2_TYPE_PAYLOAD								0x0040 /* bi-directional */


enum {
	IRL_NETWORK_BUFFER_COMPLETE,
	IRL_NETWORK_BUFFER_PENDING,
	IRL_NETWORK_RECEIVED_MESSAGE
};


#define  IRL_IS_SELECT_SET(x, y)		(x & y)


int irl_select(IrlSetting_t * irl_ptr, unsigned set, unsigned * actual_set);
int irl_connect_server(IrlSetting_t * irl_ptr, char * server_url, unsigned port);
int irl_close(IrlSetting_t * irl_ptr);

int irl_send(IrlSetting_t * irl_ptr, int socket_fd, uint8_t * buffer, size_t length);

int irl_send_packet_init(IrlSetting_t * irl_ptr, struct e_packet * p, unsigned pre_length);
int irl_send_packet(IrlSetting_t * irl_ptr, struct e_packet * p, uint16_t type);
int irl_send_packet_status(IrlSetting_t * irl_ptr, int * send_status);
int irl_send_facility_layer(IrlSetting_t * irl_ptr, struct e_packet * p, uint16_t facility, uint8_t sec_coding);
int irl_send_rx_keepalive(IrlSetting_t * irl_ptr);

void irl_receive_init(IrlSetting_t * irl_ptr);
int irl_packet_init(struct e_packet * p);
int irl_receive_packet(IrlSetting_t * irl_ptr, struct e_packet * p);
int irl_receive_packet_status(IrlSetting_t * irl_ptr, int * receive_status);


#ifdef __cplusplus
}
#endif

#endif /* _NETWORK_INTF_H */
