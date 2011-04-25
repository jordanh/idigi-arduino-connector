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

#ifndef IRL_DEF_H_
#define IRL_DEF_H_

#include "irl_api.h"
#include "ei_packet.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct irl_setting_t IrlSetting_t;
typedef struct irl_facility_handle_t IrlFacilityHandle_t;

typedef int (* irl_facility_process_cb_t) (IrlSetting_t * irl_ptr, struct irl_facility_handle_t * fac_ptr, struct e_packet * p);

typedef struct irl_facility_packet_t {
	e_boolean_t							active;
	uint8_t									buffer[IRL_MSG_MAX_PACKET_SIZE];
	struct e_packet						packet;
	struct irl_facility_packet_t	* next;
} IrlFacilityPacket_t;

struct irl_facility_handle_t {
	IrlFacilityEnableFunc_t			facility_enalble_function;
	void										* user_data;
	void										* facility_data;

	unsigned 							facility_id;
	irl_facility_process_cb_t		process_function;

	IrlFacilityPacket_t				* packet;
	unsigned							packet_count;
	int										state;

	struct irl_facility_handle_t	* next;
};

#define	IRL_HANDLE_INACTIVE		0
#define	IRL_HANDLE_ACTIVE			1
#define	IRL_HANDLE_STOP				2
#define IRL_HANDLE_TERMINATE	3


struct irl_setting_t {
	uint32_t		edp_version;
	int				active_state;

	irl_callback_t   callback;

	struct {
   		void 			* data[IRL_CONFIG_MAX];
		unsigned		id;
	} config;

	unsigned						active_facility_idx;
	unsigned						facility_count;
	IrlFacilityHandle_t 	 		* facility_list;
	IrlFacilityHandle_t			* active_facility;


#if 0
	uint8_t		device_id[IRL_DEVICE_ID_LENGTH];
	uint8_t		vendor_id[IRL_VENDOR_ID_LENGTH];
//	char				device_type[IRL_DEVICE_TYPE_LENGTH];
	char			* device_type;

	char			* server_url;

	uint16_t		tx_keepalive;
	uint16_t		rx_keepalive;
	uint8_t			wait_count;
#endif

	uint32_t		rx_ka_time;
	uint32_t		tx_ka_time;
	uint8_t			current_wait_count;
	uint8_t			security_form;
	// uint8_t 		ka_buf[PKT_MT_LENGTH];

	int							edp_state;
	int							layer_state;
    int							facility_state;
    uint16_t					network_busy;
	IrlNetworkConnect_t	connection;

	struct {
		uint8_t			* ptr;
		uint8_t			buffer[IRL_MSG_MAX_PACKET_SIZE];
		size_t				length;
		size_t				total_length;
	} send_packet;

	struct {
		int							index;
		uint8_t					buffer[IRL_MSG_MAX_PACKET_SIZE];
		uint16_t					packet_type;
		uint16_t					packet_length;
		size_t						length;
		size_t						total_length;
		uint8_t					* ptr;
		struct e_packet		* packet;
		e_boolean_t			enabled;
	} receive_packet;

	struct e_packet		data_packet;
	int							callback_event;
	int							return_code;

};


#define GET_RX_KEEPALIVE(x) *((uint16_t *)(((IrlSetting_t *)(x))->config.data[IRL_CONFIG_RX_KEEPALIVE]))
#define GET_TX_KEEPALIVE(x) *((uint16_t *)(((IrlSetting_t *)(x))->config.data[IRL_CONFIG_TX_KEEPALIVE]))
#define GET_WAIT_COUNT(x) 	*((uint8_t *)(((IrlSetting_t *)(x))->config.data[IRL_CONFIG_WAIT_COUNT]))


void irl_init_setting(IrlSetting_t * irl_ptr);

#ifdef __cplusplus
extern "C"
}
#endif

#endif /* IRL_DEF_H_ */
