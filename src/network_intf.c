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
#include <string.h>
#include "network_intf.h"
#include "config_intf.h"
#include "os_intf.h"
#include "layer.h"

#include "bele.h"
#include "ei_security.h"
//#include "ei_msg.h"
#include "ei_discover.h"
//#include "irl_cc.h"


int check_interval_limit(IrlSetting_t * irl_ptr, uint32_t start, uint32_t limit)
{
#define LIMIT_VALID		1
#define LIMIT_INVALID	0

	uint32_t	elapsed;
	int			rc = LIMIT_VALID;
	if (limit > 0)
	{
		rc = irl_get_system_time(irl_ptr, &elapsed);
		if (rc == IRL_SUCCESS)
		{
			elapsed -= start;
			rc = (elapsed < limit) ? LIMIT_VALID : LIMIT_INVALID;
		}
	}
	return rc;
}

int irl_packet_init(struct e_packet * p)
{
	p->length = 0;
//	p->alloc_len = 0;
	p->buf = NULL;
//	p->pre_len = 0;
	p->sec_coding = SECURITY_PROTO_NONE;
//	p->sec_cxn = NULL;
//	p->edp_hdr.used_len = 0;
	return 0;
}


int irl_select(IrlSetting_t * irl_ptr, unsigned set, unsigned * actual_set)
{
	IrlNetworkSelect_t select_data;
	unsigned				  	config_id;
	IrlStatus_t					status;
	int							rc = IRL_SUCCESS;
	uint16_t					rx_keepalive;
	uint16_t					tx_keepalive;
	uint32_t					time_stamp;

	if (irl_ptr->connection.socket_fd < 0)
	{
	    goto _ret;
	}
	if (irl_ptr->edp_state > IRL_DISCOVERY_LAYER)
	{
		if (irl_get_system_time(irl_ptr, &time_stamp) != IRL_SUCCESS)
		{
			status = IRL_STATUS_ERROR;
			goto _ret;
		}

		rx_keepalive = (GET_RX_KEEPALIVE(irl_ptr) * IRL_MILLISECONDS) - (time_stamp - irl_ptr->rx_ka_time);
		tx_keepalive = (GET_TX_KEEPALIVE(irl_ptr) * IRL_MILLISECONDS) - (time_stamp - irl_ptr->tx_ka_time);

		select_data.wait_time = IRL_MIN(rx_keepalive, tx_keepalive);

	}
	else
	{
		select_data.wait_time = 0;
		select_data.wait_time = 2;
	}
	select_data.select_set = set;
	select_data.actual_set = 0;
	select_data.socket_fd = irl_ptr->connection.socket_fd;


	config_id = IRL_CONFIG_SELECT;
	status = irl_get_config(irl_ptr, config_id, &select_data);
	if (status != IRL_STATUS_CONTINUE && status != IRL_STATUS_BUSY)
	{
		rc = IRL_CONFIG_ERR;
	}

	*actual_set = select_data.actual_set;
_ret:
	return rc;
}

int irl_send(IrlSetting_t * irl_ptr, int socket_fd, uint8_t * buffer, size_t length)
{
	int							rc = IRL_NETWORK_ERR;
	IrlStatus_t					status;
	IrlNetworkWrite_t	write_data;
	uint16_t					tx_keepalive;
	uint16_t					rx_keepalive;
	uint32_t					time_stamp;

	if (irl_ptr->network_busy || socket_fd < 0)
	{
		/* don't do any network activity */
		rc = 0;
		goto _ret;
	}
	rx_keepalive = GET_RX_KEEPALIVE(irl_ptr);
	tx_keepalive = GET_TX_KEEPALIVE(irl_ptr);
	if (irl_get_system_time(irl_ptr, &time_stamp) != IRL_SUCCESS)
	{
		status = IRL_STATUS_ERROR;
		goto _ret;
	}

	rx_keepalive = (GET_RX_KEEPALIVE(irl_ptr) * IRL_MILLISECONDS) - (time_stamp - irl_ptr->rx_ka_time);
	tx_keepalive = (GET_TX_KEEPALIVE(irl_ptr) * IRL_MILLISECONDS) - (time_stamp - irl_ptr->tx_ka_time);

	write_data.buffer = buffer;
	write_data.length = length;
	write_data.socket_fd = socket_fd;
	write_data.length_written = 0;
	write_data.timeout = IRL_MIN(tx_keepalive, rx_keepalive)/IRL_MILLISECONDS;
	printf("irl_send: timeout = %d\n", write_data.timeout);

	status = irl_get_config(irl_ptr, IRL_CONFIG_SEND, &write_data);
	if (status == IRL_STATUS_CONTINUE) {
		if (write_data.length_written > 0)
		{
			/* Retain the "last (RX) message send" time. */
			rc = irl_get_system_time(irl_ptr, &irl_ptr->rx_ka_time);
			if (rc == IRL_SUCCESS)
			{
				rc = write_data.length_written;
			}
		}
		else
		{
			rc = 0;
		}
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = 0;
	}

_ret:
	return rc;
}

int irl_send_no_security_packet(IrlSetting_t * irl_ptr, struct e_packet * p, uint16_t type)
{
	int16_t length;
	int 	rc = IRL_NETWORK_ERR;

#if 0
	/*
	 * We need to prepend two bytes to the packet. If there is insufficient
	 * space for that, reject the packet.
	 */
	if (p->pre_len < PKT_MT_LENGTH) {
		rc = IRL_INVALID_DATA_LENGTH;
		goto _ret;
	}
#endif

	/* Adjust the packet header fields. */
	length = TO_BE16(p->length);

	if (irl_ptr->edp_version == 1)
	{
		/*
		 * MTv1...
		 * MT version 1 has a 2-octet length field only.
		 */
		p->length += 2;
//		p->alloc_len += 2;
//		p->pre_len -= 2;
		p->buf -= 2;
		*((uint16_t *)(&p->buf[0])) = (uint16_t)length;
	}
	else
	{
		/*
		 * MTv2 (and later)...
		 * MT version 2 has a 2-octet type field before the 2-octet length.
		 */
		uint16_t msg_type;
		p->length 	 	+= 4;
//p->alloc_len 	+= 4;
//p->pre_len 		-= 4;
		p->buf 			-= 4;

		msg_type = type;
		msg_type = TO_BE16(type);

		*((uint16_t *)(&p->buf[0])) = msg_type;
		*((uint16_t *)(&p->buf[2])) = (uint16_t)length;
	}

	irl_ptr->send_packet.ptr = p->buf;
	irl_ptr->send_packet.total_length = p->length;
	irl_ptr->send_packet.length = 0;

	rc = irl_send(irl_ptr, irl_ptr->connection.socket_fd, p->buf, p->length);
	if (rc > 0)
	{
		irl_ptr->send_packet.total_length -= rc;
		irl_ptr->send_packet.length += rc;
		rc = IRL_SUCCESS;
	}

	return rc;
}

int irl_send_packet_init(IrlSetting_t * irl_ptr, struct e_packet * p, unsigned pre_length)
{
	int rc = IRL_BUSY;

 	if (irl_ptr->send_packet.total_length == 0)
 	{
		irl_packet_init(p);

		p->buf = irl_ptr->send_packet.buffer + pre_length;
//p->alloc_len = sizeof(irl_ptr->send_packet.buffer) - pre_length;
//p->pre_len = pre_length;
		rc = IRL_SUCCESS;
	}
	else
	{
		DEBUG_PRINTF("irl_send_packet_init: send still pending %d\n", (int)irl_ptr->send_packet.total_length);
	}

	return rc;
}


int irl_send_packet_status(IrlSetting_t * irl_ptr, int * send_status)
{
	uint8_t * buf;
	size_t	length;
	int		rc = IRL_SUCCESS;

	* send_status = IRL_NETWORK_BUFFER_COMPLETE;
 	if (irl_ptr->send_packet.total_length > 0)
 	{
		buf = irl_ptr->send_packet.ptr + irl_ptr->send_packet.length;
		length = irl_ptr->send_packet.total_length;

		rc = irl_send(irl_ptr, irl_ptr->connection.socket_fd, buf, length);
		if (rc > 0)
		{
			irl_ptr->send_packet.total_length -= rc;
			irl_ptr->send_packet.length += rc;
		}
	}

	if (irl_ptr->send_packet.total_length > 0)
	{
		*send_status = IRL_NETWORK_BUFFER_PENDING;
	}

	return rc;
}
int irl_send_packet(IrlSetting_t * irl_ptr, struct e_packet * p, uint16_t type)
{
	int 	rc = IRL_UNSUPPORTED_SECURITY_ERR;
	int		send_status;

	if (irl_ptr->send_packet.total_length == 0)
	{
		if (p->sec_coding == SECURITY_PROTO_NONE)
		{
			rc = irl_send_no_security_packet(irl_ptr, p, type);
			if (rc != IRL_SUCCESS)
			{
			    goto _err;
			}
		}

		rc = irl_send_packet_status(irl_ptr, &send_status);

_err:
		if (rc != IRL_SUCCESS)
		{
			DEBUG_PRINTF("irl_send_packet: error %d\n", rc);
			/* error */
		}
		else if (send_status != IRL_NETWORK_BUFFER_COMPLETE)
		{
			DEBUG_PRINTF("irl_send_packet: send pending\n");
			rc = IRL_BUSY;
		}
	}
	else
	{
		rc = IRL_BUSY;
	}

	return rc;
}
int irl_send_rx_keepalive(IrlSetting_t * irl_ptr)
{
#define IRL_MTV2_VERSION			2
	int 						rc = IRL_NETWORK_ERR;
	IrlStatus_t			status;
	struct e_packet 	pkt;
	uint16_t				rx_keepalive;

//	rx_keepalive = (uint16_t *)irl_ptr->config.data[IRL_CONFIG_RX_KEEPALIVE];
//	wait_count = (uint8_t *)irl_ptr->config.data[IRL_CONFIG_WAIT_COUNT];
	rx_keepalive = GET_RX_KEEPALIVE(irl_ptr);

	rc = check_interval_limit(irl_ptr, irl_ptr->rx_ka_time, (rx_keepalive * IRL_MILLISECONDS));
	if (rc == 1)
	{
		/* not expired yet. no need to send rx keepalive */
		rc = IRL_SUCCESS;
		goto _ret;
	}
	else if (rc < 0)
	{
		/* something's wrong */
		DEBUG_PRINTF("irl_send_rx_keepalive: check interval limit fails %d\n", rc);
		goto _ret;
	}

	if (irl_ptr->send_packet.total_length > 0)
	{
		/* time to send rx keepalive but send is still pending */
		status = irl_error_status(irl_ptr->callback, IRL_CONFIG_SEND, IRL_KEEPALIVE_ERR);
		if (status == IRL_STATUS_ERROR)
		{
			rc = IRL_KEEPALIVE_ERR;
		}
		else
		{
			rc = IRL_SUCCESS;
		}
		goto _ret;
	}
#if 0

	rc = irl_get_system_time(irl_ptr, &cur_time);
	if (rc != IRL_SUCCESS) goto _ret;

	if ((cur_time - irl_ptr->rx_ka_time) < (irl_ptr->rx_keepalive * 1000 * irl_ptr->wait_count)) {
		/* not expired yet */
		rc = IRL_SUCCESS;
		goto _ret;
	}
#endif

	DEBUG_PRINTF("irl_send_rx_keepalive: time to send Rx keepalive\n");

	irl_send_packet_init(irl_ptr, &pkt, PKT_MT_LENGTH);

	pkt.length = 0;

	if (irl_ptr->edp_version == IRL_MTV2_VERSION)
	{
		uint32_t	version;
		/*
		 * MTv2 (and later)...
		 * MT version 2 has a 2-octet type field before the 2-octet length.
		 */
		pkt.length = 4;
		version = TO_BE32(irl_ptr->edp_version);
		memcpy(pkt.buf, &version, 4);
	}
	else
	{
		/*
		 * MTv1...
		 * MT version 1 has a 2-octet length field only.
		 */
		pkt.length = 0;
	}



	rc = irl_send_packet(irl_ptr, &pkt, E_MSG_MT2_TYPE_KA_KEEPALIVE);

_ret:
	return rc;
}

int irl_send_facility_layer(IrlSetting_t * irl_ptr, struct e_packet * p, uint16_t facility, uint8_t sec_coding)
{
	int 	rc;
	uint16_t	facility_num;
//	uint8_t	sec_coding = SECURITY_PROTO_NONE;

	p->buf -= 4;
//p->alloc_len += 4;
//p->pre_len -= 4;
	p->length += 4;

/* Future: add security coding process */

	p->buf[0] = sec_coding;
	p->buf[1] = DISC_OP_PAYLOAD;

	facility_num = TO_BE16(facility);
	memcpy((void *)&p->buf[2], (void *)&facility_num, sizeof facility_num);

	rc = irl_send_packet(irl_ptr, p, E_MSG_MT2_TYPE_PAYLOAD);
	if (rc != IRL_SUCCESS)
	{
		DEBUG_PRINTF("irl_send_facility_layer: irl_snd_packet returns %d\n", rc);
		/* either error or send pending */
		goto _ret;
	}

_ret:
	return rc;
}

unsigned receive_timeout = 0;
unsigned receive_count = 0;

int irl_receive(IrlSetting_t * irl_ptr, int socket_fd, uint8_t * buffer, size_t length)
{
	int 			rc = IRL_SUCCESS;
	IrlStatus_t		status;
	IrlNetworkRead_t	read_data;
	uint16_t		tx_keepalive;
	uint16_t		rx_keepalive;
	uint8_t			wait_count;
	uint32_t		time_stamp;

    if (socket_fd < 0)
    {
        goto _ret;
    }

	tx_keepalive = GET_TX_KEEPALIVE(irl_ptr);
	rx_keepalive = GET_RX_KEEPALIVE(irl_ptr);
	wait_count = GET_WAIT_COUNT(irl_ptr);

	if (irl_ptr->network_busy )
	{
		goto _ka_check;
	}

	if (irl_get_system_time(irl_ptr, &time_stamp) != IRL_SUCCESS)
	{
		rc = IRL_CONFIG_ERR;
		goto _ret;
	}

	read_data.timeout = (IRL_MIN(((rx_keepalive * IRL_MILLISECONDS) - (time_stamp - irl_ptr->rx_ka_time)),
									 	 	 	  ((tx_keepalive * IRL_MILLISECONDS) - (time_stamp - irl_ptr->tx_ka_time)))/
									 IRL_MILLISECONDS);

	if (receive_timeout != read_data.timeout)
	{
//		printf("irl_receive: timeout = %d count %d\n", read_data.timeout, receive_count);
		receive_timeout = read_data.timeout;
	}
	else
			receive_count++;

	read_data.buffer = buffer;
	read_data.length = length;
	read_data.socket_fd = socket_fd;
	read_data.length_read = 0;

	status = irl_get_config(irl_ptr, IRL_CONFIG_RECEIVE, &read_data);
	if (status == IRL_STATUS_CONTINUE || status == IRL_STATUS_BUSY)
	{
		if (status == IRL_STATUS_CONTINUE && read_data.length_read > 0)
		{
			/* Retain the "last (TX) message send" time. */
			rc = irl_get_system_time(irl_ptr, &irl_ptr->tx_ka_time);
			if (rc == IRL_SUCCESS)
			{
				rc = read_data.length_read;
			}

		}
		else
		{
_ka_check:
			rc = 0;
			if (tx_keepalive > 0)
			{
				rc = check_interval_limit(irl_ptr, irl_ptr->tx_ka_time, (tx_keepalive * IRL_MILLISECONDS * wait_count));
				if (rc == 1)
				{
					/* tx keepalive not expired yet */
					rc = 0;
				}
				else if (rc == 0)
				{
					/*
					 * We haven't received a message
					 * of any kind for the configured maximum interval, so we must
					 * mark this connection in error and return that status.
					 *
					 * Note: this inactivity check applies only for MTv2 and later.
					 * For MTv1, the client sends keep-alives, but the server does
					 * not send them (nor must the client expect them). For MTv1,
					 * the data member tx_keepalive_ms is always 0 (zero), so this
					 * keep-alive failure check never triggers.
					 *
					 */
					rc = IRL_KEEPALIVE_ERR;
					irl_error_status(irl_ptr->callback, IRL_CONFIG_RECEIVE, IRL_KEEPALIVE_ERR);
					DEBUG_PRINTF("irl_receive: keepalive fail\n");
				}
			}
		}

	}
	else
	{
		rc = IRL_NETWORK_ERR;
	}
_ret:
	return rc;
}

void irl_receive_init(IrlSetting_t * irl_ptr)
{
	irl_ptr->receive_packet.packet_type = 0;
	irl_ptr->receive_packet.packet_length = 0;
	irl_ptr->receive_packet.length = 0;
	irl_ptr->receive_packet.total_length = 0;
	irl_ptr->receive_packet.index = 0;
	irl_ptr->receive_packet.enabled = TRUE;
}

static int irl_receive_status(IrlSetting_t * irl_ptr, int * receive_status)
{
	uint8_t * buf;
	size_t	length;
	int		rc = IRL_SUCCESS;
	int		read_length;

	*receive_status = IRL_NETWORK_BUFFER_COMPLETE;
 	if (irl_ptr->receive_packet.length < irl_ptr->receive_packet.total_length)
 	{
		buf = irl_ptr->receive_packet.ptr + irl_ptr->receive_packet.length;
		length = irl_ptr->receive_packet.total_length - irl_ptr->receive_packet.length;

		read_length = irl_receive(irl_ptr, irl_ptr->connection.socket_fd, buf, length);
		if (read_length > 0)
		{
			DEBUG_PRINTF("irl_receive_status: read_length = %d\n", read_length);
			irl_ptr->receive_packet.length += read_length;
		}
		else if (read_length < 0)
		{
			rc = IRL_NETWORK_ERR;
		}
	}

	if (irl_ptr->receive_packet.total_length > 0)
	{
		if (irl_ptr->receive_packet.length < irl_ptr->receive_packet.total_length)
		{
			*receive_status = IRL_NETWORK_BUFFER_PENDING;
		}
		else if (irl_ptr->receive_packet.length > irl_ptr->receive_packet.total_length)
		{
			/* something's wrong */
			DEBUG_PRINTF("irl_receive_packet_status: length receive > request length !!!\n");
		}

	}
//	DEBUG_PRINTF("irl_receive_status: length = %d total_length %d status %d\n", irl_ptr->receive_packet.length,
//			irl_ptr->receive_packet.total_length, *receive_status);
	return rc;
}


int irl_receive_packet(IrlSetting_t * irl_ptr, struct e_packet * p)
{
	int 		rc = IRL_SUCCESS;
//	int16_t 	len;
	uint16_t 	type_val;
//	IrlStatus_t	status;
	int			receive_status;
//	IrlNetworkRead_t		read_data;

	if (p == NULL)
	{
		DEBUG_PRINTF("irl_receive_packet: packet is NULL\n");
		rc = IRL_PARAM_ERR;
		goto _ret;
	}
	rc = irl_receive_status(irl_ptr, &receive_status);
	if (rc != IRL_SUCCESS || receive_status == IRL_NETWORK_BUFFER_PENDING)
	{
		goto _ret;
	}
	if (irl_ptr->receive_packet.index == 4)
	{
		goto _ret;
	}

	irl_ptr->receive_packet.index++;


	if (irl_ptr->edp_version == 1)
	{
		/*
		 * Default the MT message type.
		 *
		 * For the original MT version 1, all messages are payload messages. A
		 * payload message of length zero is a keep-alive message. We accommodate
		 * that below, after we successfully read the length field value of zero.
		 */
		irl_ptr->receive_packet.packet_type = E_MSG_MT2_TYPE_PAYLOAD;

		if (irl_ptr->receive_packet.index == 1)
		{
			irl_ptr->receive_packet.packet = p;
			/*
			 * Read the MT message length (2 bytes).
			 */
			irl_ptr->receive_packet.ptr = (uint8_t *)&irl_ptr->receive_packet.packet_length;
			irl_ptr->receive_packet.length = 0;
			irl_ptr->receive_packet.total_length = sizeof irl_ptr->receive_packet.packet_length;

			rc = irl_receive(irl_ptr, irl_ptr->connection.socket_fd,
							 irl_ptr->receive_packet.ptr,
							 irl_ptr->receive_packet.total_length);
			if (rc < 0)
			{
				goto _ret;
			}

			irl_ptr->receive_packet.length += rc;

			rc = irl_receive_status(irl_ptr, &receive_status);
			if (rc != IRL_SUCCESS || receive_status == IRL_NETWORK_BUFFER_PENDING)
			{
				goto _ret;
			}
			irl_ptr->receive_packet.index++;  /* skip type message */
		}


		if (irl_ptr->receive_packet.index == 2)
		{
			/* got the length of the message */
			p->length = FROM_BE16(irl_ptr->receive_packet.packet_length);
			irl_ptr->receive_packet.packet_length = p->length;

			if (irl_ptr->receive_packet.packet_length == 0)
			{
				irl_ptr->receive_packet.packet_type = E_MSG_MT2_TYPE_KA_KEEPALIVE;
			}
			irl_ptr->receive_packet.index++;  /* skip type message */
		}

		goto common_recv;
	}

	/*
	 * Read the MT message type.
	 *
	 * For MT version 2, there are numerous message types. Some of these
	 * messages require special handling, in that they may be legacy EDP
	 * version response message varieties. These messages are sent by
	 * servers that do not support MTv2. Since the client doesn't support
	 * both MTv1 and MTv2 concurrently, an MTv2 client must terminate its
	 * MT connection if it finds the server to be incompatible insofar as
	 * the MT version is concerned.
	 *
	 * We only accept messages of the expected types from the server. Any
	 * message other than an expected type is handled as an error, and an
	 * error is returned to the caller. This must be done since any unknown
	 * message type cannot be correctly parsed for length and discarded from
	 * the input stream.
	 */

	if (irl_ptr->receive_packet.index == 1)
	{
		irl_ptr->receive_packet.packet = p;

		/* read the type of the message */
		irl_ptr->receive_packet.ptr = (uint8_t *)&irl_ptr->receive_packet.packet_type;
		irl_ptr->receive_packet.length = 0;
		irl_ptr->receive_packet.total_length = sizeof irl_ptr->receive_packet.packet_type;
		rc = irl_receive(irl_ptr, irl_ptr->connection.socket_fd,
						 irl_ptr->receive_packet.ptr,
						 irl_ptr->receive_packet.total_length);
		if (rc < 0)
		{
			goto _ret;
		}

		irl_ptr->receive_packet.length += rc;

		rc = irl_receive_status(irl_ptr, &receive_status);
		if (rc != IRL_SUCCESS || receive_status == IRL_NETWORK_BUFFER_PENDING)
		{
			goto _ret;
		}
		irl_ptr->receive_packet.index++;

	}


	if (irl_ptr->receive_packet.index == 2)
	{

		/* got message type let's get to message length */

		/* make sure we support the message type */
		type_val = FROM_BE16(irl_ptr->receive_packet.packet_type);
		irl_ptr->receive_packet.packet_type = type_val;

		switch (type_val)
		{
			/* Expected MTv2 message types... */
			case E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP:
				DEBUG_PRINTF("irl_receive_packet: E_MSG_MT2_TYPE_LEGACY_EDP_VER_RESP 0x%x\n", (unsigned) type_val);
				/*
				 * Obtain the MT message length (2 bytes).
				 * Note that legacy EDP version response messages do not have a length
				 * field. There is just a single byte of data remaining in the stream
				 * for this MT message type, so we provide a dummy length value of 1 in
				 * this case. All other MT message types do have a length field, which
				 * we will validate according to message type after we read the length
				 * field bytes.
				 */
				/* Supply a length of 1 byte. */
				irl_ptr->receive_packet.length = 1;
				break;
			case E_MSG_MT2_TYPE_VERSION_OK:
				DEBUG_PRINTF("irl_receive_packet: E_MSG_MT2_TYPE_VERSION_OK 0x%x\n", (unsigned) type_val);
				break;
			case E_MSG_MT2_TYPE_VERSION_BAD:
				DEBUG_PRINTF("irl_receive_packet: E_MSG_MT2_TYPE_VERSION_BAD 0x%x\n", (unsigned) type_val);
				break;
			case E_MSG_MT2_TYPE_SERVER_OVERLOAD:
				DEBUG_PRINTF("irl_receive_packet: E_MSG_MT2_TYPE_SERVER_OVERLOAD 0x%x\n", (unsigned) type_val);
				break;
			case E_MSG_MT2_TYPE_KA_KEEPALIVE:
				DEBUG_PRINTF("irl_receive_packet: E_MSG_MT2_TYPE_KA_KEEPALIVE 0x%x\n", (unsigned) type_val);
				break;
			case E_MSG_MT2_TYPE_PAYLOAD:
				DEBUG_PRINTF("irl_receive_packet: E_MSG_MT2_TYPE_PAYLOAD 0x%x\n", (unsigned) type_val);
				break;
			/* Unexpected/unknown MTv2 message types... */
			case E_MSG_MT2_TYPE_VERSION:
		//	case E_MSG_MT2_TYPE_LEGACY_EDP_VERSION: // same as E_MSG_MT2_TYPE_VERSION
			case E_MSG_MT2_TYPE_KA_RX_INTERVAL:
			case E_MSG_MT2_TYPE_KA_TX_INTERVAL:
			case E_MSG_MT2_TYPE_KA_WAIT:
			default:
				DEBUG_PRINTF("irl_receive_packet: error type 0x%x\n", (unsigned) type_val);
				irl_error_status(irl_ptr->callback, IRL_CONFIG_RECEIVE, IRL_INVALID_MESSAGE_ERR);
				rc =  IRL_INVALID_MESSAGE_ERR;
				goto _ret;
		}


		irl_ptr->receive_packet.ptr = (uint8_t *)&irl_ptr->receive_packet.packet_length;
		irl_ptr->receive_packet.length = 0;
		irl_ptr->receive_packet.total_length = sizeof irl_ptr->receive_packet.packet_length;

		rc = irl_receive(irl_ptr, irl_ptr->connection.socket_fd,
						 irl_ptr->receive_packet.ptr,
						 irl_ptr->receive_packet.total_length);
		if (rc < 0)
		{
			goto _ret;
		}

		irl_ptr->receive_packet.length += rc;

		rc = irl_receive_status(irl_ptr, &receive_status);
		if (rc != IRL_SUCCESS || receive_status == IRL_NETWORK_BUFFER_PENDING)
		{
			goto _ret;
		}
		irl_ptr->receive_packet.index++;
	}



	if (irl_ptr->receive_packet.index == 3)
	{
		p->length = FROM_BE16(irl_ptr->receive_packet.packet_length);
		irl_ptr->receive_packet.packet_length = p->length;
		if (irl_ptr->receive_packet.packet_type != E_MSG_MT2_TYPE_PAYLOAD)
		{
			/*
			 * For all but payload messages, the length field value should be
			 * zero, as there is no data accompanying the message. The MT
			 * messages to which this applies here are:
			 *    E_MSG_MT2_TYPE_VERSION_OK
			 *    E_MSG_MT2_TYPE_VERSION_BAD
			 *    E_MSG_MT2_TYPE_SERVER_OVERLOAD
			 *    E_MSG_MT2_TYPE_KA_KEEPALIVE
			 */
			if (irl_ptr->receive_packet.packet_length != 0)
			{
				irl_error_status(irl_ptr->callback, IRL_CONFIG_RECEIVE, IRL_INVALID_PAYLOAD_MSG);
				rc = IRL_INVALID_PAYLOAD_MSG;
				goto _ret;
			}
		}
	    DEBUG_PRINTF("irl_receive_packet: length field %d\n", p->length);
	}



common_recv:
	if (irl_ptr->receive_packet.index == 3)
	{
		irl_ptr->receive_packet.total_length = p->length;

		if (p->length == 0)
		{
			irl_ptr->receive_packet.index = 4;
		}

#if 0
		/*
		 * Verify that this message will fit into the supplied buffer.
		 */
		if (irl_ptr->receive_packet.total_length < p->length) {
			/*
			 * The message length exceeds the buffer size: error out.
			 *
			 * Recovery is possible but questionable: we don't know whether
			 * we're simply out of sync with the TCP byte stream or this
			 * message is simply wrong in being too large. The only thing
			 * we can be certain of is that there's a problem, so we quit.
			 */
			irl_error_status(irl_ptr->callback, IRL_CONFIG_RECEIVE, IRL_INVALID_DATA_LENGTH);
			irl_receive_init(irl_ptr);
			rc = IRL_INVALID_DATA_LENGTH;
			goto _ret;
		}


		/*
		 * Store the length value in the buffer, before the data, if it fits.
		 */
		if (p->pre_len >= sizeof(uint16_t)) {
			/* Save the length (in host byte order), if there's room for it. */
			uint16_t *p16 = (uint16_t *)(p->buf - sizeof(uint16_t));
			*p16 = (uint16_t)p->length;
		}
#endif
		/*
		 * Read the actual message data bytes into the packet buffer.
		 */
		if (p->length > 0)
		{

			p->buf = irl_ptr->receive_packet.ptr = (uint8_t *)&irl_ptr->receive_packet.buffer[0];
#if 0
			irl_ptr->receive_packet.ptr = (uint8_t *)p->buf;
#endif
			irl_ptr->receive_packet.length = 0;
			irl_ptr->receive_packet.total_length = p->length;
			rc = irl_receive(irl_ptr, irl_ptr->connection.socket_fd,
							 irl_ptr->receive_packet.ptr,
							 irl_ptr->receive_packet.total_length);
			if (rc < 0)
			{
				goto _ret;
			}

			irl_ptr->receive_packet.length += rc;

			rc = irl_receive_status(irl_ptr, &receive_status);
			if (rc != IRL_SUCCESS || receive_status == IRL_NETWORK_BUFFER_PENDING)
			{
				goto _ret;
			}
			irl_ptr->receive_packet.index++;
		}

	}


	if (irl_ptr->receive_packet.index == 4)
	{
		p->type = irl_ptr->receive_packet.packet_type;
	}

_ret:
	return rc;
}

int irl_receive_packet_status(IrlSetting_t * irl_ptr, int * receive_status)
{
//	uint8_t * buf;
//	size_t	length;
	int		rc = IRL_SUCCESS;

	*receive_status = IRL_NETWORK_BUFFER_PENDING;
	rc = irl_receive_packet(irl_ptr, irl_ptr->receive_packet.packet);
	if (rc == IRL_SUCCESS && irl_ptr->receive_packet.index == 4)
	{
		*receive_status = IRL_NETWORK_BUFFER_COMPLETE;
	}

	return rc;
}


int irl_connect_server(IrlSetting_t * irl_ptr, char * server_url, unsigned port)
{
	int			rc = IRL_CONFIG_ERR;
	IrlStatus_t	status;
	unsigned 	config_id;

	irl_ptr->connection.host_name = server_url;
	irl_ptr->connection.port = port;
	irl_ptr->connection.socket_fd = -1;

	config_id = IRL_CONFIG_CONNECT;
	status = irl_get_config(irl_ptr, config_id, &irl_ptr->connection);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;
}

int irl_close(IrlSetting_t * irl_ptr)
{
	int			rc = IRL_SUCCESS;
	IrlStatus_t	status;
	unsigned 	config_id;

	if (irl_ptr->connection.socket_fd >= 0)
	{
		config_id = IRL_CONFIG_CLOSE;
		status = irl_get_config(irl_ptr, config_id, &irl_ptr->connection.socket_fd);
		if (status == IRL_STATUS_CONTINUE)
		{
			DEBUG_PRINTF("irl_close: close %d fd\n", irl_ptr->connection.socket_fd);
			irl_ptr->connection.socket_fd = -1;
		}
		else if (status == IRL_STATUS_BUSY)
		{
			DEBUG_PRINTF("irl_close: close busy\n");
			rc = IRL_BUSY;
		}
		else
		{
			rc = IRL_CONFIG_ERR;
		}
	}
	return rc;
}

