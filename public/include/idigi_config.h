
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
* 
* @brief Defines the major blocks of functionality used in the IIK 
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

/**
* @}
*/

#ifdef ENABLE_COMPILE_TIME_DATA_PASSING

/**
* @defgroup idigi_config_data_options Hard Code Application Configuration options
* @{
* 
* These optional defines are used to hardcode some of the configuration @ref idigi_callback_t "callbacks".  
* This is useful for reducing codesize footprint when these configuration values are static (which they
* often are).
*    
*/
/** 
 * When defined, this string is hardcode for the @ref device_type instead of the application framework 
 * function @ref get_device_type() (called via the @ref idigi_config_device_type @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_device_type() when IDIGI_DEVICE_TYPE is defined. 
 *  
 * @see @ref device_type
 * @see @ref idigi_config_device_type
 * @see @ref get_device_type
 * @see @ref default_config
 */
#define IDIGI_DEVICE_TYPE                             "IIK Linux Sample"

/** 
 * When defined, this string hardcode for the @ref server_url instead of the application framework 
 * function @ref get_server_url() (called via the @ref idigi_config_server_url @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_server_url() when IDIGI_CLOUD_URL is defined. 
 *  
 * @see @ref server_url
 * @see @ref idigi_config_server_url
 * @see @ref get_server_url() 
 * @see @ref default_config
 */
#define IDIGI_CLOUD_URL                               "developer.idigi.com"

/** 
 * When defined, this string hardcode for the @ref tx_keepalive instead of the application framework 
 * function @ref  get_tx_keepalive_interval() (called via the @ref idigi_config_tx_keepalive @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref  get_tx_keepalive_interval() when IDIGI_TX_KEEPALIVE_IN_SECONDS is defined. 
 *  
 * @see @ref tx_keepalive
 * @see @ref idigi_config_tx_keepalive
 * @see @ref  get_tx_keepalive_interval() 
 * @see @ref default_config
 */
#define IDIGI_TX_KEEPALIVE_IN_SECONDS                 75

/** 
 * When defined, this string hardcode for the @ref rx_keepalive instead of the application framework 
 * function @ref get_rx_keepalive_interval() (called via the @ref idigi_config_rx_keepalive @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_rx_keepalive_interval() when IDIGI_RX_KEEPALIVE_IN_SECONDS is defined. 
 *  
 * @see @ref rx_keepalive
 * @see @ref idigi_config_rx_keepalive
 * @see @ref get_rx_keepalive_interval() 
 * @see @ref default_config
 */
#define IDIGI_RX_KEEPALIVE_IN_SECONDS                 75 

/** 
 * When defined, this string hardcode for the @ref wait_count instead of the application framework 
 * function @ref get_wait_count() (called via the @ref idigi_config_wait_count @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_wait_count() when IDIGI_RX_KEEPALIVE_IN_SECONDS is defined. 
 *  
 * @see @ref wait_count
 * @see @ref idigi_config_wait_count
 * @see @ref get_wait_count() 
 * @see @ref default_config
 */
#define IDIGI_WAIT_COUNT                              5

/** 
 * When defined, this string hardcode for the @ref wait_count instead of the application framework 
 * function @ref get_wait_count() (called via the @ref idigi_config_wait_count @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_wait_count() when IDIGI_RX_KEEPALIVE_IN_SECONDS is defined. 
 *  
 * @see @ref wait_count
 * @see @ref idigi_config_wait_count
 * @see @ref get_wait_count() 
 * @see @ref default_config
 */
#define IDIGI_VENDOR_ID                            0x00000000

/** 
 * When defined, this string hardcode for the @ref max_msg_transactions instead of the application framework 
 * function @ref get_max_message_transactions() (called via the @ref idigi_config_max_transaction @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_max_message_transactions() when IDIGI_MSG_MAX_TRANSACTION is defined. 
 *  
 * @see @ref max_msg_transactions
 * @see @ref idigi_config_max_transaction
 * @see @ref get_max_message_transactions() 
 * @see @ref default_config
 */
#define IDIGI_MSG_MAX_TRANSACTION                  1

/** 
 * When defined, this string hardcode for the @ref connection_type instead of the application framework 
 * function @ref get_connection_type() (called via the @ref idigi_config_connection_type @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_connection_type() when IDIGI_CONNECTION_TYPE is defined.  The
 * values for this define are limited to @ref idigi_lan_connection_type or @ref idigi_wan_connection_type.
 *  
 * @see @ref connection_type
 * @see @ref idigi_config_connection_type
 * @see @ref get_connection_type() 
 * @see @ref idigi_connection_type_t
 * @see @ref default_config
 */
#define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type

/** 
 * When defined, this string hardcode for the @ref link_speed instead of the application framework 
 * function @ref get_link_speed() (called via the @ref idigi_config_link_speed @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_link_speed() when IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND is defined. 
 *  
 * @see @ref link_speed
 * @see @ref idigi_config_link_speed
 * @see @ref get_link_speed() 
 * @see @ref default_config
 */
#define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0

/** 
 * When defined, this string hardcode for the @ref phone_number instead of the application framework 
 * function @ref get_phone_number() (called via the @ref idigi_config_phone_number @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_phone_number() when IDIGI_WAN_PHONE_NUMBER_DIALED is defined. 
 *  
 * @see @ref phone_number
 * @see @ref idigi_config_phone_number
 * @see @ref get_phone_number() 
 * @see @ref default_config
 */
#define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678"

/** 
 * When defined, this string hardcode for the @ref firmware_support instead of the application framework 
 * function @ref get_firmware_support() (called via the @ref idigi_config_firmware_facility @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_firmware_support() when IDIGI_FIRMWARE_SUPPORT is defined. 
 * @note This define is only used when @ref IDIGI_FIRMWARE_SERVICE is defined in @ref idigi_config.h.
 * 
 * @see @ref firmware_support
 * @see @ref idigi_config_firmware_facility
 * @see @ref get_firmware_support() 
 * @see @ref IDIGI_FIRMWARE_SERVICE  
 * @see @ref default_config
 */
#define IDIGI_FIRMWARE_SUPPORT

/** 
 * When defined, this string hardcode for the @ref data_service_support instead of the application framework 
 * function @ref get_data_service_support() (called via the @ref idigi_config_data_service @ref idigi_callback_t "callback" in config.c).   
 * 
 * @note There is no need to implement or port @ref get_data_service_support() when IDIGI_DATA_SERVICE_SUPPORT is defined. 
 * @note This define is only used when @ref IDIGI_DATA_SERVICE is defined in @ref idigi_config.h.
 *  
 * @see @ref data_service_support
 * @see @ref idigi_config_data_service
 * @see @ref get_data_service_support() 
 * @see @ref IDIGI_DATA_SERVICE  
 * @see @ref default_config
 */
#define IDIGI_DATA_SERVICE_SUPPORT

#endif

/**
* @}
*/

#endif
