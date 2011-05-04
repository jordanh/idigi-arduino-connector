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
#ifndef _EI_PACKET_H
#define _EI_PACKET_H

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


typedef struct {
	uint16_t	type;
	uint16_t	length;
//	uint8_t	* data;
} idk_packet_t;

typedef struct {
	uint16_t	type;
	uint16_t	length;
	uint8_t		sec_coding;
	uint8_t		disc_payload;
	uint16_t	facility;
	uint8_t		* data;
} idk_facility_packet_t;

/*@}*/
#endif	/* _EI_PACKET_H */
