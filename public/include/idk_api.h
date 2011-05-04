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
#ifndef _IRL_API_H
#define _IRL_API_H

#include <stdio.h>

#include "e_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

//#define DEBUG_PRINTF(...)

#define DEBUG_PRINTF(...)		printf(__VA_ARGS__)

#define IRL_MT_VERSION						2
#define IRL_MT_PORT								3197
#define IRL_MSG_MAX_PACKET_SIZE	1600

#define IRL_DEVICE_TYPE_LENGTH		32
#define IRL_DEVICE_ID_LENGTH			16
#define IRL_VENDOR_ID_LENGTH			4
#define IRL_SERVER_URL_LENGTH		255
#define IRL_MAC_ADDR_LENGTH			6
#define IRL_LINK_SPEED_LENGTH			4

#define IRL_RX_INTERVAL_MIN				5
#define IRL_RX_INTERVAL_MAX				7200
#define IRL_TX_INTERVAL_MIN				5
#define IRL_TX_INTERVAL_MAX				7200
#define IRL_WAIT_COUNT_MIN				2
#define IRL_WAIT_COUNT_MAX				64

#define IRL_MIN(x,y)		(((x) < (y))? (x): (y))
#define IRL_MAX(x,y)		(((x) > (y))? (x): (y))
#define IRL_MILLISECONDS				1000

enum {
	IRL_CONFIG_NETWORK = 8,
	IRL_CONFIG_FIRMWARE_FACILITY,
	IRL_CONFIG_RCI_FACILITY,
	IRL_CONFIG_OS_IFACE
};


#define IRL_NETWORK_CONFIG	(0x1 << IRL_CONFIG_NETWORK)

/* Enumerations for callback to get edp configuration, make socket calls, and make OS calls
 *
 * 	IRL_CONFIG_DEVICE_ID	 		callback must return a pointer to device ID
 *	IRL_CONFIG_VENDOR_ID			callback must return a pointer to vendor ID
 *	IRL_CONFIG_DEVICE_TYPE		callback must return a pointer to device type
 *	IRL_CONFIG_SERVER_URL		callback must return a pointer to server URL
 *	IRL_CONFIG_PASSWORD			callback must return a pointer to password
 *	IRL_CONFIG_CONNECTION_TYPE		callback must return a pointer to connection type which is either WAN or LAN
 *	IRL_CONFIG_MAC_ADDR					callback must return a pointer to MAC address
 *	IRL_CONFIG_LINK_SPEED					callback must return a pointer to link speed
 *	IRL_CONFIG_PHONE_NUM				callback must return a pointer to IrlConfigPhone_t for phone number for WAN connection type
 *	IRL_CONFIG_TX_KEEPALIVE 			callback must return a pointer to TX keepalive
 *	IRL_CONFIG_RX_KEEPALIVE				callback must return a pointer to RX keepalive
 *	IRL_CONFIG_WAIT_COUNT				callback must return a pointer to wait count
 *	IRL_CONFIG_IP_ADDR						callback must return a pointer to IrlConfigIpAddr_t for device ip address
 *	IRL_ERROR_STATUS							callback is called if IRL encounters error from previous callback.
 *																callback is given a pointer to IrlErrorStatus_t for the configuration id and error status.
 *	IRL_DISCONNECTED							callback is called when server disconnects IRL. No data is given to the callback.
 *																If callback returns CONTINUE, it will reconnect the server. If callback returns ABORT, it will exit
 *																IRL function with SERVER DISCONNECTED error.
 *	IRL_REDIRECT									callback is called when server redirects IRL to connect to different server.
 *
 *	IRL_CONFIG_CONNECT						callback is called to establish connection to iDigi server. Pointer to IrlNetworkConnect_t
 *																is given to the callback. callback must set the socket fd if connection is successfully established.
 *	IRL_CONFIG_SEND							callback is called to send data to iDigi server. Pointer to IrlNetworkWrite_t is given to
 *																the callback. If callback encounters EAGAIN error, callback must not return error. It should set
 *																the number of bytes written to 0 and return CONTINUE status for IRL to call this callback again.
 *	IRL_CONFIG_RECEIVE						callback is called to receive data from iDigi server. Pointer to IrlNetworkRead_t is given
 *																to the callback. If callback encounters EAGAIN error, callback must not return error. It should set
 *																the number of bytes received to 0 and return CONTINUE status for IRL to call this callback again.
 *	IRL_CONFIG_SELECT
 *	IRL_CONFIG_CLOSE							callback is called to close the iDigi connection. Pointer to socket_id which is returned by the
 *																IRL_CONFIG_CONNECT callback is given to the callback.
 *
 */
enum {
	IRL_CONFIG_DEVICE_ID,
	IRL_CONFIG_VENDOR_ID,
	IRL_CONFIG_DEVICE_TYPE,

	IRL_CONFIG_SERVER_URL,
	IRL_CONFIG_PASSWORD,
	IRL_CONFIG_CONNECTION_TYPE,
	IRL_CONFIG_MAC_ADDR,
	IRL_CONFIG_LINK_SPEED,
	IRL_CONFIG_PHONE_NUM,
	IRL_CONFIG_TX_KEEPALIVE,
	IRL_CONFIG_RX_KEEPALIVE,
	IRL_CONFIG_WAIT_COUNT,
	IRL_CONFIG_IP_ADDR,

	/* all configuration settings and state should be added above this */
	IRL_ERROR_STATUS,
	IRL_DISCONNECTED,
	IRL_REDIRECT,

	IRL_CONFIG_CONNECT = IRL_NETWORK_CONFIG,
	IRL_CONFIG_SEND,
	IRL_CONFIG_RECEIVE,
	IRL_CONFIG_SELECT,
	IRL_CONFIG_CLOSE

};

#define IRL_CONFIG_MAX	IRL_ERROR_STATUS


#define	IRL_LAN_CONNECTION	0x01
#define IRL_WAN_CONNECTION	0x02


typedef enum irl_status_t {
	IRL_STATUS_CONTINUE,
	IRL_STATUS_ERROR,
	IRL_STATUS_BUSY
} IrlStatus_t;


typedef struct irl_config_phone_t {
	uint8_t		* number;
	size_t			length;
} IrlConfigPhone_t;

typedef enum irl_ip_addr_type_t {
	IRL_IPV4_ADDRESS,
	IRL_IPV6_ADDRESS
} IrlIpAddrType_t;

typedef struct irl_config_ip_addr_t {
	IrlIpAddrType_t 	addr_type;
	uint8_t		        * ip_addr;
} IrlConfigIpAddr_t;

#define IRL_CC_NOT_REDIRECT			0
#define IRL_CC_REDIRECT_SUCCESS		1
#define IRL_CC_REDIRECT_ERROR		2

typedef struct irl_redirect_t {
	char			* destination;
	uint8_t		report_code;
	char			* report_message;
	unsigned	timeout;
} IrlRedirect_t;

/* error status structure
 *
 * This structure is used to callback for IRL_ERROR_STATUS indicating
 * IRL has encountered error.
 *
 * @param config_id		Callback configuration ID which error is found.
 * @param status			Error status of the error.
 *
 */
typedef struct irl_error_status_t {
	unsigned	config_id;
	int				status;
} IrlErrorStatus_t;

/* network connect structure
 *
 * This structure is used in a callback for IRL_CONFIG_CONNECT to
 * establish connection between a device and iDigi server.
 *
 * @param host_name		Pointer to FQDN of iDigi server to connect to.
 * @param port					Port number to connect to.
 * @param socket_fd			Socket FD. Callback must set this socket_fd when
 * 											connection is successfully established.
 *
 */
typedef struct irl_network_connect_t {
	char 			* host_name;
	unsigned	port;
	int		 		socket_fd;
} IrlNetworkConnect_t;

/* Network write structure
 *
 * This structure is used in a callback for IRL_CONFIG_SEND to send
 * data to iDigi server.
 *
 * @param buffer					Pointer to data to be sent to iDigi server.
 * @param length					Length of the data in bytes
 * @param length_written	Length in bytes has been sent to iDigi server.
 * 												Callback must set this.
 * @param socket_fd				Socket FD associated with a connection through the IRL_CONFIG_CONNECT callback.
 * @param timeout				timeout value in second. Callback must return within this
 * 												timeout. This allows IRL to maintenance keepalive process.
 * 												The connection may be disconnected if keepalive fails.
 */
typedef struct irl_network_write_t {
	uint8_t		* buffer;
	size_t			length;
	size_t			length_written;
	int				socket_fd;
	unsigned	timeout;
} IrlNetworkWrite_t;

/* Network read structure
 *
 * This structure is used in a callback for IRL_CONFIG_RECEIVE to receive
 * data from iDigi server.
 *
 * @param buffer		Pointer to memory where received data will be written to.
 * @param length		length buffer in bytes to be received.
 * @param length_written	Actual length in bytes has been received from iDigi server.
 * 												Callback must set this when receiving data from iDigi server.
 * @param socket_fd				Socket FD associated with a connection through the IRL_CONFIG_CONNECT callback.
 * @param timeout				timeout value in second. Callback must return within this
 * 												timeout. This allows IRL to maintenance keepalive process.
 * 												The connection may be disconnected if keepalive fails.
 */
typedef struct irl_network_read_t {
	uint8_t		* buffer;
	size_t			length;
	size_t			length_read;
	int				socket_fd;
	unsigned	timeout;

} IrlNetworkRead_t;

#define IRL_NETWORK_READ_SET				0x1
#define IRL_NETWORK_WRITE_SET			(0x1 << 1)
#define IRL_NETWORK_CALLBACK_SET	(0x1 << 2)
#define IRL_NETWORK_TIMEOUT_SET		(0x1 << 3)

typedef struct irl_network_select_t {
	unsigned		wait_time;
	unsigned		select_set;
	unsigned		actual_set;
	int					socket_fd;
} IrlNetworkSelect_t;

/*
 * IRL callback for EDP configurations, network interface, and os interface.
 */
typedef IrlStatus_t (* irl_callback_t ) (unsigned config_id, void * data);


unsigned long irl_init(irl_callback_t callback);

#define IRL_BUSY												1
#define IRL_SUCCESS										0
#define IRL_INIT_ERR										-1
#define IRL_PARAM_ERR									2
#define IRL_CONFIG_ERR								-3
#define IRL_NETWORK_ERR							-4
#define IRL_INVALID_DATA_LENGTH				-5
#define IRL_INVALID_PAYLOAD_MSG				-6
#define IRL_KEEPALIVE_ERR							-7
#define IRL_SERVER_OVERLOAD_ERR			-8
#define IRL_BAD_VERSION_ERR						-9
#define IRL_INVALID_MESSAGE_ERR				-10
#define IRL_CLOSE_ERR									-11
#define IRL_TIMEOUT_ERR								-12
#define IRL_UNSUPPORTED_SECURITY_ERR		-13
#define IRL_STATE_ERR										-14
#define IRL_MALLOC_ERR									-15
#define IRL_INVALID_DATA									-16
#define IRL_DOWNLOAD_ERR								-17
#define IRL_SERVER_DISCONNECTED				-18

typedef int (* IrlFacilityEnableFunc_t ) (unsigned long irl_handle, void * data);

typedef struct irl_facility_data_t {
	IrlFacilityEnableFunc_t	facility_enalble_function;
	void								* user_data;
} IrlFacilityData_t;

typedef struct irl_data_t {
	uint32_t				version;
	unsigned			facility_count;

	IrlFacilityData_t  * facility_list;
} IrlData_t;

int irl_task(unsigned long handle, IrlData_t const * data);

#define IRL_STOP				0
#define IRL_TERMINATE	1
int irl_stop(unsigned long handle, unsigned stop_flag);

#ifdef __cplusplus
}
#endif

#endif /* _IRL_API_H */
