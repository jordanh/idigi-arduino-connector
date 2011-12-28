
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

/**
 * @file
 *  @brief IIK configuration options.
 *
 */
#ifndef IDIGI_CONFIG_H_
#define IDIGI_CONFIG_H_

/**
* @defgroup idigi_config_options iDigi Configuration Options
* @{
*/
/** 
 *  If defined the system is little endian, comment out for big endian.
 */
#define IDIGI_LITTLE_ENDIAN

#ifdef USE_THE_MAKEFILE_INSTEAD
/** 
 *  If defined the debugging is enabled in the IIK.
 * @see USER_DEBUG_VPRINTF
 * @see ASSERT
 */
#define IDIGI_DEBUG

/** 
 * If defined the firmware service is included in IIK.
 * @see @ref firmware_support
 */
#define IDIGI_FIRMWARE_SERVICE


/** 
 * If defined zlib compression is enabled for data service.
 */
#define IDIGI_COMPRESSION

/** 
 * If defined the data service is included in IIK.
 * @see @ref data_service_support
 */
#define IDIGI_DATA_SERVICE
#endif

/**
* @}
*/
#ifdef USE_THE_DEFINES
#define IDIGI_DEVICE_TYPE               "IIK Linux Sample"
#define IDIGI_CLOUD_URL                 "developer.idigi.com" 
#define IDIGI_TX_KEEPALIVE_IN_SECONDS   75 
#define IDIGI_RX_KEEPALIVE_IN_SECONDS   75 
#define IDIGI_WAIT_COUNT                5 
#define IDIGI_VENDOR_ID                 0x00000000 
#define IDIGI_MSG_MAX_TRANSACTION       1 


#define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type 
#define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0 
#define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678"

#define IDIGI_FIRMWARE_SUPPORT 
#define IDIGI_DATA_SERVICE_SUPPORT 
#endif

#endif
