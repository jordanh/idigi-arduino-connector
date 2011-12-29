
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
 *  When defined, the system is little endian.  
 *  
 *  @note For Big endian systems, this line should be commented out.
 */
#define IDIGI_LITTLE_ENDIAN

/** 
 * If defined, IIK includes debug support.  IIK state and debug data is displayed using the
 * @ref USER_DEBUG_VPRINTF tags.  Macro error testing is compiled and implemented using the
 * @ref ASSERT macro.
 * 
 * These can be eliminated in a product release, which will greatly reduce memory codespace and 
 * RAM needs.  
 * 
 * @see USER_DEBUG_VPRINTF
 * @see ASSERT
 */
#define IDIGI_DEBUG

/** 
 * If defined, IIK includes the @ref firmware_download "Firmware Download Service".  
 * 
 * @see @ref firmware_support
 */
#define IDIGI_FIRMWARE_SERVICE


/** 
 * If defined, the IIK includes the @ref zlib "Data Compression" used with the  @ref data_service. 
 * @note When included, this requires the zlib library.
 * @see @ref data_service
 * @see @ref IDIGI_DATA_SERVICE
 * 
 */
#define IDIGI_COMPRESSION

/** 
 * If defined, IIK includes the @ref data_service.  
 * @see @ref data_service_support
 * @see @ref zlib
 * @see @ref IDIGI_COMPRESSION
 */
#define IDIGI_DATA_SERVICE

#ifdef ENABLE_COMPILE_TIME_DATA_PASSING

/** 
 * If defined, this eliminates the callback used to retrieve the @ref device_type.
 * 
 * @see @ref device_type API Configuration Callback
 * @see @ref get_device_type
 * @see @ref default_config
 *   
 */
#define IDIGI_DEVICE_TYPE               "IIK Linux Sample"

/* #define IDIGI_CLOUD_URL                 "developer.idigi.com" */
/* #define IDIGI_TX_KEEPALIVE_IN_SECONDS   75 */
/* #define IDIGI_RX_KEEPALIVE_IN_SECONDS   75 */
/* #define IDIGI_WAIT_COUNT                5 */
/* #define IDIGI_VENDOR_ID                 0x00000000 */
/* #define IDIGI_MSG_MAX_TRANSACTION       1 */


/* #define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type */
/* #define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0 */
/* #define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678" */

/* #define IDIGI_FIRMWARE_SUPPORT */
/* #define IDIGI_DATA_SERVICE_SUPPORT */

#endif

/**
* @}
*/

#endif
