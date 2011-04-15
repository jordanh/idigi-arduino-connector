/*
 * irl_rci.c
 *
 *  Created on: Apr 13, 2011
 *      Author: mlchan
 */
#include <string.h>

#include "irl_rci.h"
#include "ei_msg.h"
#include "ei_security.h"
#include "network_intf.h"
#include "bele.h"

int rci_send(struct irl_setting_t * irl_ptr, struct e_packet * p)
{
	int 	rc;

	rc = irl_send_facility_layer(irl_ptr, p, E_MSG_FAC_RCI_NUM, SECURITY_PROTO_NONE);

	return rc;
}

char * no_query_state_response = "<rci_replay version=\"1.1\"> <query_state/> </rci_reply>";
int rci_process_function(struct irl_setting_t * irl_ptr, struct irl_facility_handle_t * fac_ptr, struct e_packet * p)
{
	int 				rc = IRL_SUCCESS;
	struct e_packet		pkt;
	uint8_t				* buf;
	uint32_t			length;

	DEBUG_TRACE("rci_process_function: fake response\n");
	irl_send_packet_init(irl_ptr, &pkt, PKT_PRE_FACILITY);

	buf = pkt.buf;
	*buf++ = 0x04;
	*buf++ = 0x00;
	length = TO_BE32(strlen(no_query_state_response));
	memcpy(buf, &length, sizeof length);
	buf += sizeof length;

		/* now add this target to the target list message */
	memcpy(buf, no_query_state_response, strlen(no_query_state_response));
	buf += strlen(no_query_state_response);
	pkt.length = buf - pkt.buf;

	rc = rci_send(irl_ptr, &pkt);

	return rc;
}
