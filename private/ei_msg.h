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
#ifndef _EI_MSG_H
#define _EI_MSG_H

/*
 * ei_msg.h   Copyright 2000-2005 Embrace Networks Inc.
 */

/**
 * @name ei_msg
 * @memo Message-passing layer, utility functions.
 */

/*@{*/

#include "e_types.h"
//#include "e_network.h"


/*
 * Messages over TCP (MT) version.
 *   1   indicates the original MT (version 1)
 *   2   indicates MT version 2 (introduced in EDBP version 4.0)
 */
//#define E_MSG_MT_VERSION 1
//#define E_MSG_MT_VERSION 2



//#include "ei_packet.h"

#define E_MSG_SOCKET_RETRY_DELAY	200UL 	/* milliseconds */
#define E_MSG_SOCKET_RETRY_COUNT	25
#define E_MSG_CONNECT_RETRY_DELAY	2000UL 	/* milliseconds */
#define E_MSG_CONNECT_RETRY_COUNT	3
#define E_MSG_TCP_CONNECT_TIMEOUT	30		/* seconds */
#define E_MSG_LISTEN_MAX_DELAY		(uint16_t)-1	/* no timeout */

#define	E_MSG_PROTO_TCP			0
#define	E_MSG_PROTO_HTTP		1
#define	E_MSG_PROTO_SERIAL		2
#define E_MSG_PROTO_MAX			3

#define E_MSG_SOCKET_FD_INVALID	(uint32_t)-1

#define	E_MSG_MT_SERVER_PORT	3197 /* assigned server port from IANA */
#define E_MSG_MTSSL_SERVER_PORT 3199
#define	E_MSG_MT_CLIENT_PORT	3198 /* assigned client port from IANA */

#define E_MSG_HTTP_DEFAULT_PORT	80
#define E_MSG_HTTP_PACKET_MIME	"application/x-mdh"
#define E_MSG_HTTP_CR			0x0d
#define E_MSG_HTTP_LF			0x0a

#define E_MSG_HTTP_PERSISTENT_CONN 1 /* 1=persistent, 0=non-persistent */

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

#define E_MSG_FAC_FW_NUM		   0x0070
#define E_MSG_FAC_RCI_NUM          0x00a0
#define E_MSG_FAC_MSG_NUM          0x00c0
//#define E_MSG_FAC_DEV_LOOP_NUM     0xff00
//#define E_MSG_FAC_SERVER_LOOP_NUM  0xff01
#define E_MSG_FAC_CC_NUM           0xffff

/*
 * MT keepalive-related defines.
 * Interval defines are specified in seconds.
 * Note: the full MT keepalive capability was introduced in EDBP 4.0 (MTv2).
 */
#define E_MSG_MT_KA_RX_INTV_MIN      5
#define E_MSG_MT_KA_RX_INTV_MAX      65535
#define E_MSG_MT_KA_RX_INTV_DEFAULT  60
#define E_MSG_MT_KA_TX_INTV_MIN      5
#define E_MSG_MT_KA_TX_INTV_MAX      65535
#define E_MSG_MT_KA_TX_INTV_DEFAULT  75
#define E_MSG_MT_KA_WAIT_MIN         2
#define E_MSG_MT_KA_WAIT_MAX         64
#define E_MSG_MT_KA_WAIT_DEFAULT     3

/*
 * MDH keepalive-related defines.
 * Interval defines are specified in seconds.
 */
#define E_MSG_MDH_KA_RX_INTV_MIN     30
#define E_MSG_MDH_KA_RX_INTV_MAX     65535
#define E_MSG_MDH_KA_RX_INTV_DEFAULT 90
#define E_MSG_MDH_KA_TX_INTV_MIN     30
#define E_MSG_MDH_KA_TX_INTV_MAX     65535
#define E_MSG_MDH_KA_TX_INTV_DEFAULT 90
#define E_MSG_MDH_KA_WAIT_MIN        2
#define E_MSG_MDH_KA_WAIT_MAX        64
#define E_MSG_MDH_KA_WAIT_DEFAULT    3

#if 0
/*
 * Error return values.
 */
#define E_MSG_ERR_SERVER_OVERLOAD	-503	/* Server overload */
#define E_MSG_ERR_KEEPALIVE_FAIL	-504	/* Keep-alive failure */
#define E_MSG_ERR_MAX_RETRIES		-505	/* Too many retries */
#define E_MSG_ERR_INVALID_MESSAGE	-506	/* Invalid message type */
#define E_MSG_ERR_RCVD_MSG_TOO_BIG	-507	/* Rcvd msg too big for buffer */
#define E_MSG_ERR_RCVD_MSG_BAD_SIZE	-508	/* Rcvd msg size wrong for type */
#define E_MSG_ERR_BAD_MT_VERSION	-509	/* MT version number is bad */
#define E_MSG_ERR_REQUEST_CANCELED	-510	/* Request was canceled */
#define E_MSG_ERR_CONN_IDLE_TIMEOUT	-511	/* Connection idle timeout */
#endif


/*@}*/

#endif	/* _EI_MSG_H */
