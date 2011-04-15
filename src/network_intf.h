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
#define E_MSG_MT2_TYPE_VERSION				0x0004 /* C -> S */
#define E_MSG_MT2_TYPE_LEGACY_EDP_VERSION	0x0004 /* C -> S */
#define E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP	0x0001 /* C <- S */
#define E_MSG_MT2_TYPE_VERSION_OK			0x0010 /* C <- S */
#define E_MSG_MT2_TYPE_VERSION_BAD			0x0011 /* C <- S */
#define E_MSG_MT2_TYPE_SERVER_OVERLOAD		0x0012 /* C <- S */
#define E_MSG_MT2_TYPE_KA_RX_INTERVAL		0x0020 /* C -> S */
#define E_MSG_MT2_TYPE_KA_TX_INTERVAL		0x0021 /* C -> S */
#define E_MSG_MT2_TYPE_KA_WAIT				0x0022 /* C -> S */
#define E_MSG_MT2_TYPE_KA_KEEPALIVE			0x0030 /* bi-directional */
#define E_MSG_MT2_TYPE_PAYLOAD				0x0040 /* bi-directional */


enum {
	IRL_NETWORK_BUFFER_COMPLETE,
	IRL_NETWORK_BUFFER_PENDING,
	IRL_NETWORK_RECEIVED_MESSAGE
};

enum {
	IRL_LAYER_INIT,
	IRL_LAYER_CONNECT,
	IRL_LAYER_REDIRECT,
	IRL_COMMUNICATION_LAYER_VERSION,
	IRL_COMMUNICATION_LAYER_VERSION1,
	IRL_COMMUNICATION_LAYER_KA_PARAMS,
	IRL_LAYER_RECEIVE_PENDING,
	IRL_LAYER_RECEIVE_DATA,
	IRL_LAYER_SEND_PENDING,
	IRL_SECURITY_LAYER_DEVICE_ID,
	IRL_SECURITY_LAYER_SERVER_URL,
	IRL_SECURITY_LAYER_PASSWORD,
	IRL_LAYER_DISCOVERY_DEVICE_TYPE,
	IRL_LAYER_DISCOVERY_FACILITY_INIT,
//	IRL_LAYER_DISCOVERY_CC_INIT,
	IRL_LAYER_DISCOVERY_FACILITY,
	IRL_LAYER_DISCOVERY_COMPLETE,
	IRL_LAYER_DONE
};

enum {
	IRL_FACILITY_INIT,
	IRL_FACILITY_INIT_DONE,
	IRL_FACILITY_PROCESS,
	IRL_FACILITY_DONE
};

enum {
	IRL_INIT,
	IRL_COMMUNICATION_LAYER,
	IRL_INITIALIZATION_LAYER,
	IRL_SECURITY_LAYER,
	IRL_DISCOVERY_LAYER,
	IRL_FACILITY_LAYER
};

enum {
	IRL_RECEIVE_LENGTH,
	IRL_RECEIVE_LENGTH_COMPLETE,
	IRL_RECEIVE_TYPE,
	IRL_RECEIVE_TYPE_COMPLETE,
	IRL_RECEIVE_DATA,
	IRL_RECEIVE_DATA_COMPLETE,
	IRL_RECEIVE_COMPLETE
};

#define  IRL_IS_SELECT_SET(x, y)		(x & y)

void irl_set_edp_state(struct irl_setting_t * irl_ptr, int state);

unsigned irl_get_network_set(struct irl_setting_t * irl_ptr);
int irl_select(struct irl_setting_t * irl_ptr, unsigned set, unsigned * actual_set);
int irl_connect_server(struct irl_setting_t * irl_ptr, char * server_url, unsigned port);
int irl_close(struct irl_setting_t * irl_ptr);

int irl_send_packet_init(struct irl_setting_t * irl_ptr, struct e_packet * p, unsigned pre_length);
int irl_send_packet(struct irl_setting_t * irl_ptr, struct e_packet * p, uint16_t type);
int irl_send_packet_status(struct irl_setting_t * irl_ptr, int * send_status);
int irl_send_facility_layer(struct irl_setting_t * irl_ptr, struct e_packet * p, uint16_t facility, uint8_t sec_coding);

void irl_receive_init(struct irl_setting_t * irl_ptr);
int irl_packet_init(struct e_packet * p);
int irl_receive_packet(struct irl_setting_t * irl_ptr, struct e_packet * p);
int irl_receive_packet_status(struct irl_setting_t * irl_ptr, int * receive_status);
int irl_send_rx_keepalive(struct irl_setting_t * irl_ptr);

int irl_communication_layer(struct irl_setting_t * irl_ptr);
int irl_initialization_layer(struct irl_setting_t * irl_ptr);
int irl_security_layer(struct irl_setting_t * irl_ptr);
int irl_discovery_layer(struct irl_setting_t * irl_ptr);
int irl_facility_layer(struct irl_setting_t * irl_ptr);


#ifdef __cplusplus
}
#endif

#endif /* _NETWORK_INTF_H */
