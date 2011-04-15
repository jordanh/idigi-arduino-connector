#ifndef _EI_PACKET_H
#define _EI_PACKET_H

/*
 * ei_packet.h   Copyright 2000-2003 Embrace Networks Inc.
 */

/**
 * @name ei_packet
 * @memo This provides a few functions and definitions to facilitate
 *       generic variable-length message passing between layers.
 */

/*@{*/

#include "e_types.h"

/*
 * Embrace Device Protocol preallocation needs.
 *
 * The following defines indicate the number of packet buffer preallocation
 * bytes that should be allocated to precede each protocol layer or facility.
 *
 * Preallocation of bytes to precede the bytes of a layer or facility serves
 * to improve the performance of message sending by avoiding the copying of
 * data from small buffers to larger ones as a message is prepared for
 * transmission. This also reduces memory and stack needs.
 */
#define PKT_MT_LENGTH        4	/* size of MTv2 type+length fields */
                              	/* (also suffices for MTv1 length field) */
#define PKT_OP_SECURITY      1	/* size of security layer opcode field */
#define PKT_OP_DISCOVERY     1	/* size of discovery layer opcode field */
#define PKT_OP_MUX           2	/* size of mux layer opcode field */
#define PKT_OP_FACILITY       2	/* size of mux layer opcode field */
#define PKT_PRE_SECURITY     PKT_MT_LENGTH
#define PKT_PRE_DISCOVERY    (PKT_PRE_SECURITY + PKT_OP_SECURITY)
#define PKT_PRE_MUX          (PKT_PRE_DISCOVERY + PKT_OP_DISCOVERY)
#define PKT_PRE_FACILITY     (PKT_PRE_MUX + PKT_OP_MUX)
#define PKT_PRE_FAC_CC       PKT_PRE_FACILITY
#define PKT_PRE_FAC_FS       PKT_PRE_FACILITY
#define PKT_PRE_FAC_FU       PKT_PRE_FACILITY
#define PKT_PRE_FAC_LOG      PKT_PRE_FACILITY
#define PKT_PRE_FAC_MB       PKT_PRE_FACILITY
#define PKT_PRE_FAC_MON      PKT_PRE_FACILITY
#define PKT_PRE_FAC_MP       PKT_PRE_FACILITY
#define PKT_PRE_FAC_MSG      PKT_PRE_FACILITY
#define PKT_PRE_FAC_PIO      PKT_PRE_FACILITY
#define PKT_PRE_FAC_RCI      PKT_PRE_FACILITY
#define PKT_PRE_FAC_TRANS    PKT_PRE_FACILITY
#define PKT_PRE_APPLICATION  PKT_PRE_FAC_MP

/*
 * The following EDP header is the basic protocol header octet sequence for
 * the mux layer and below, with room for the MT message length. This is EDP-
 * specific and must change if the layering of the EDP itself is modified.
 */
#if 0
struct e_edp_header {
	uint8_t used_len;
	uint8_t disc_op;
	uint8_t fac_id[PKT_OP_MUX];
};
typedef struct e_edp_header e_edp_header_t;
#endif

struct e_packet {
	uint8_t	 type;
	uint16_t length;
//	uint16_t alloc_len;
	uint8_t *buf;
//	uint16_t pre_len;
	uint8_t  sec_coding;
//	void    *sec_cxn;
//	e_edp_header_t edp_hdr;
};
typedef struct e_packet e_packet_t;

#if 0
struct e_packet_callback {
	int (*func)(struct e_packet *, void *);
	void *parm;
};
typedef struct e_packet_callback e_packet_callback_t;

/**
 * Transmits a packet to the specified callback function
 * @param p packet to transmit
 * @param pc packet callback structure
 * @return non-negative for success, negative for failure.
 */
int e_packet_send(struct e_packet *p, struct e_packet_callback *pc);

/**
 * Initialize a packet structure: currenly just sets everything to zero.
 * @param p pointer to packet to initialize
 * @return non-negative for success, negative for failure.
 */
int e_packet_init(struct e_packet *p);
#endif
/*@}*/
#endif	/* _EI_PACKET_H */
