/*
 * irl_def.h
 *
 *  Created on: Apr 8, 2011
 *      Author: mlchan
 */

#ifndef IRL_DEF_H_
#define IRL_DEF_H_

#include "irl_api.h"
#include "ei_packet.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct irl_setting_t;
struct irl_facility_handle_t;

typedef int (* irl_facility_process_cb_t)(struct irl_setting_t * irl_ptr, struct irl_facility_handle_t * fac_ptr, struct e_packet * p);

struct irl_facility_handle_t {
	IrlFacilityEnableFunc_t			facility_enalble_function;
	void							* user_data;
	void							* facility_data;


	unsigned 						facility_id;
	irl_facility_process_cb_t		process_function;
	struct e_packet					packet;
	uint8_t							buffer[IRL_MSG_MAX_PACKET_SIZE];


	int								state;
	struct irl_facility_handle_t	* next;
};

#define	IRL_HANDLE_INACTIVE	0
#define	IRL_HANDLE_ACTIVE	1
#define	IRL_HANDLE_STOP		2
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
	struct irl_facility_handle_t 	* facility_list;
	struct irl_facility_handle_t	* active_facility;


#if 0
    uint8_t         device_id[IRL_DEVICE_ID_LENGTH];
    uint8_t         vendor_id[IRL_VENDOR_ID_LENGTH];
//    char            device_type[IRL_DEVICE_TYPE_LENGTH];
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
	struct irl_network_connect_t	connection;

	struct {
		uint8_t			* ptr;
		uint8_t			buffer[IRL_MSG_MAX_PACKET_SIZE];
		size_t			length;
		size_t			total_length;
	} send_packet;

	struct {
		int				index;
		uint8_t			buffer[IRL_MSG_MAX_PACKET_SIZE];
		uint16_t		packet_type;
		uint16_t		packet_length;
		size_t			length;
		size_t			total_length;
		uint8_t			* ptr;
		struct e_packet	* packet;
		e_boolean_t		enabled;
	} receive_packet;

	struct e_packet		data_packet;
	int					callback_event;

};


#if 1
#define GET_RX_KEEPALIVE(x) *((uint16_t *)(((struct irl_setting_t *)(x))->config.data[IRL_CONFIG_RX_KEEPALIVE]))
#define GET_TX_KEEPALIVE(x) *((uint16_t *)(((struct irl_setting_t *)(x))->config.data[IRL_CONFIG_TX_KEEPALIVE]))
#define GET_WAIT_COUNT(x) 	*((uint8_t *)(((struct irl_setting_t *)(x))->config.data[IRL_CONFIG_WAIT_COUNT]))
#endif

int irl_add_facility(struct irl_setting_t * irl_ptr, void * user_data, unsigned facility_id, void * facility_data, irl_facility_process_cb_t process_cb);
struct irl_facility_handle_t * irl_get_facility_handle(struct irl_setting_t * irl_ptr, void * user_data);
int irl_del_facility_handle(struct irl_setting_t * irl_ptr, void * user_data);
int irl_add_facility_handle(struct irl_setting_t * irl_ptr, void * user_data, struct irl_facility_handle_t ** fac_handle);

void irl_init_setting(struct irl_setting_t * irl_ptr);
int irl_edp_configuration_layer(struct irl_setting_t * irl_ptr);

#ifdef __cplusplus
extern "C"
}
#endif

#endif /* IRL_DEF_H_ */
