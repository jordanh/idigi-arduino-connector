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


#ifndef LAYER_H_
#define LAYER_H_

#include "irl_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
	IRL_FACILITY_LAYER,
	IRL_DONE
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

int irl_clean_facility_packet(IrlSetting_t * irl_ptr);

void irl_set_edp_state(IrlSetting_t * irl_ptr, int state);
int irl_edp_configuration_layer(IrlSetting_t * irl_ptr);

int irl_communication_layer(IrlSetting_t * irl_ptr);
int irl_initialization_layer(IrlSetting_t * irl_ptr);
int irl_security_layer(IrlSetting_t * irl_ptr);
int irl_discovery_layer(IrlSetting_t * irl_ptr);
int irl_facility_layer(IrlSetting_t * irl_ptr);


#ifdef __cplusplus
}
#endif

#endif /* LAYER_H_ */
