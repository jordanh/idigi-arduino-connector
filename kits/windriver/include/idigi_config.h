
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
  *  @brief IIK configuration options
  *
  */
#ifndef __IDIGI_CONFIG_H_
#define __IDIGI_CONFIG_H_


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
 * When defined, IIK private library includes debug support.  IIK state and debug data will be displayed using the
 * @ref USER_DEBUG_VPRINTF tags.  Macro error testing is compiled and implemented using the
 * @ref ASSERT macro.
 *
 * This debug feature can be eliminated in a product release, which will greatly reduce memory codespace and
 * RAM needs, by changing this line in idigi_config.h:
 *
 * @code
 * #define IDIGI_DEBUG
 * @endcode
 *
 * To this:
 * @code
 * //#define IDIGI_DEBUG
 * @endcode
 *
 * @see USER_DEBUG_VPRINTF
 * @see ASSERT
 */
#define IDIGI_DEBUG

/**
 * When defined, IIK private library includes the @ref firmware_download "Firmware Download Service".
 *
 * To disable the @ref firmware_download "Firmware Download Service" feature, change this line in idigi_config.h:
 *
 * @code
 * #define IDIGI_FIRMWARE_SERVICE
 * @endcode
 *
 * To this:
 * @code
 * //#define IDIGI_FIRMWARE_SERVICE
 * @endcode
 *
 * @see @ref firmware_support
 */
//#define IDIGI_FIRMWARE_SERVICE


/**
 * This is a compilation convenience definition.  See @ref IDIGI_COMPRESSION.
 *
 */
#define IDIGI_COMPRESSION
#if (!defined (IDIGI_NO_COMPRESSION) || defined(DIGI_REQUIRED_TO_GENERATE_INTERNAL_DOCUMENATION))
/**
 * When defined, the IIK includes the @ref zlib "compression" support used with the @ref data_service.
 *
 * By default, compression support is disabled.  To enable Optional Data Compression Support, change this line in idigi_config.h:
 * By default, optional @ref zlib "compression" support is disabled.
 *
 * To enable @ref zlib "compression", change the following from idigi_config.h:
 *
 * @code
 * #define IDIGI_NO_COMPRESSION
 * #if (!defined (IDIGI_NO_COMPRESSION) || defined(DIGI_REQUIRED_TO_GENERATE_INTERNAL_DOCUMENATION))
 *
 * ...
 *
 * #define IDIGI_COMPRESSION
 * #endif
 * @endcode
 *
 * To this:
 * @code
 * //#define IDIGI_NO_COMPRESSION
 * #if (!defined (IDIGI_NO_COMPRESSION) || defined(DIGI_REQUIRED_TO_GENERATE_INTERNAL_DOCUMENATION))
 *
 * ...
 *
 * #define IDIGI_COMPRESSION
 * #endif
 * @endcode
 *
 * Or just this:
 * @code
 * #define IDIGI_COMPRESSION
 * @endcode
 *
 * @note When included, this requires the @ref zlib "zlib" library.
 *
 * @see @ref data_service
 * @see @ref IDIGI_DATA_SERVICE
 * @see @ref IDIGI_FILE_SYSTEM
 * @see @ref IDIGI_NO_COMPRESSION
 *
 */
#define IDIGI_COMPRESSION
#endif

/**
 * If defined, IIK includes the @ref data_service.
 * To disable the @ref data_service feature, change this line in idigi_config.h:
 *
 * @code
 * #define IDIGI_DATA_SERVICE
 * @endcode
 *
 * To this:
 * @code
 * //#define IDIGI_DATA_SERVICE
 * @endcode
 *
 * @see @ref data_service_support
 * @see @ref zlib
 * @see @ref IDIGI_NO_COMPRESSION
 */
#define IDIGI_DATA_SERVICE

/**
 * This is a compilation convenience definition.  See @ref IDIGI_FILE_SYSTEM.
 *
 */
#define IDIGI_FILE_SYSTEM
#define IDIGI_FILE_SYSTEM_MAX_PATH_LENGTH   256
#if (!defined (IDIGI_NO_FILE_SYSTEM) || defined(DIGI_REQUIRED_TO_GENERATE_INTERNAL_DOCUMENATION))
/**
 * If defined, IIK includes the @ref file_system.
 * To enable the @ref file_system feature change this line in idigi_config.h:
 *
 * @code
 * #define IDIGI_NO_FILE_SYSTEM
 * @endcode
 *
 * To this:
 * @code
 * #define IDIGI_FILE_SYSTEM
 * @endcode
 *
 * @see @ref file_system_support
 * @see @ref zlib
 * @see @ref IDIGI_NO_COMPRESSION
 */
#define IDIGI_FILE_SYSTEM
#endif

#define IDIGI_RCI_SERVICE
/**
 * This is used to define the maximum content length in bytes of an element's value for the remote_configuration _service,
 * IC includes the remote_configuration_service that allows user remote configuration, control, and information
 * exchange between a device and iDigi Cloud.
 * To disable the remote_configuration feature, define this to 0 in idigi_config.h:
 *
 * @code
 * #define IDIGI_RCI_MAXIMUM_CONTENT_LENGTH 256
 * @endcode
 *
 * To this:
 * @code
 * #define IDIGI_RCI_MAXIMUM_CONTENT_LENGTH  0
 * @endcode
 *
 * @see @ref app_get_remote_configuration_support()
 */
#define IDIGI_RCI_MAXIMUM_CONTENT_LENGTH    1024
/**
 * @}*/

#if ENABLE_COMPILE_TIME_DATA_PASSING

/**
 * @defgroup idigi_config_data_options Hard Coded Application Configuration options
 * @{
 *
 *  @brief Hard Coded Application Configuration options is an optional method where configuration is pulled from hardcode
 *  defined at compile time instead of the @ref idigi_callback_t "Application callback" method.
 *
 *  If the following configurations in @ref idigi_config.h are defined, the code to make the @ref idigi_callback_t "application callbacks"
 *  is uncompiled and the hard-coded values are used instead:
 *
 * @code
 *     #define IDIGI_DEVICE_TYPE                          "IIK Linux Sample"
 *     #define IDIGI_CLOUD_URL                            "developer.idigi.com"
 *     #define IDIGI_TX_KEEPALIVE_IN_SECONDS              75
 *     #define IDIGI_RX_KEEPALIVE_IN_SECONDS              75
 *     #define IDIGI_WAIT_COUNT                           10
 *     #define IDIGI_VENDOR_ID                            0x01006113
 *     #define IDIGI_MSG_MAX_TRANSACTION                  1
 *     #define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type
 *     #define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0
 *     #define IDIGI_WAN_PHONE_NUMBER_DIALED              "617-731-1018"
 *     #define IDIGI_FIRMWARE_SUPPORT
 *     #define IDIGI_DATA_SERVICE_SUPPORT
 *     #define IDIGI_FILE_SYSTEM_SUPPORT
 * @endcode
 *
 * This option is useful for reducing the IIK code space for those applications that are sensitive to memory usage.  The following
 * table describes the each define and the callback cross reference:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Configuration Defines</td>
 * <th class="title">Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DEVICE_TYPE @htmlonly</td>
 * <td>Device type in iso-8859-1 encoded string to identify the device.
 * See @endhtmlonly @ref device_type @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_CLOUD_URL @htmlonly </td>
 * <td>iDigi Device Cloud FQDN.
 * See @endhtmlonly @ref server_url @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_TX_KEEPALIVE_IN_SECONDS @htmlonly </td>
 * <td>TX Keepalive interval in seconds between 5 and 7200 seconds.
 * See @endhtmlonly @ref tx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_RX_KEEPALIVE_IN_SECONDS @htmlonly </td>
 * <td>RX Keepalive interval in seconds between 5 and 7200 seconds.
 * See @endhtmlonly @ref rx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_WAIT_COUNT @htmlonly </td>
 * <td>Maximum number of consecutive missing keepalive messages for a
 * connection to be considered lost.  Valid range is greater than 1 and less than 64.
 * See @endhtmlonly @ref wait_count @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_VENDOR_ID @htmlonly </td>
 * <td>Vendor ID from iDigi Account.
 * See @endhtmlonly @ref vendor_id @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_MSG_MAX_TRANSACTION @htmlonly </td>
 * <td>Maximum simultaneous transactions for data service receiving message.
 * See @endhtmlonly @ref max_msg_transactions @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_CONNECTION_TYPE @htmlonly </td>
 * <td> @endhtmlonly @ref idigi_lan_connection_type @htmlonly for LAN connection or
 * @endhtmlonly @ref idigi_wan_connection_type @htmlonly WAN connection.
 * See @endhtmlonly @ref connection_type @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND @htmlonly </td>
 * <td>Link speed for WAN connection.
 * See @endhtmlonly @ref link_speed @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_WAN_PHONE_NUMBER_DIALED @htmlonly </td>
 * <td>String represetnation of a WAN cellular phone connection.
 * See @endhtmlonly @ref phone_number @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FIRMWARE_SUPPORT @htmlonly </td>
 * <td>If defined it enables firmware download capability.
 * See @endhtmlonly @ref firmware_support @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DATA_SERVICE_SUPPORT @htmlonly </td>
 * <td>If defined it enables data service capability.
 * See @endhtmlonly @ref data_service_support @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 *
 */
/**
 * When defined, this string is hardcode for the @ref device_type instead of the application framework
 * function @ref app_get_device_type (called via the @ref idigi_config_device_type @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_device_type when IDIGI_DEVICE_TYPE is defined.
 *
 * @see @ref device_type
 * @see @ref idigi_config_device_type
 * @see @ref app_get_device_type
 */
#define IDIGI_DEVICE_TYPE                             "IIK Linux Sample"

/**
 * When defined, this string hardcode for the @ref server_url instead of the application framework
 * function @ref app_get_server_url (called via the @ref idigi_config_server_url @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_server_url when IDIGI_CLOUD_URL is defined.
 *
 * @see @ref server_url
 * @see @ref idigi_config_server_url
 * @see @ref app_get_server_url()
 */
#define IDIGI_CLOUD_URL                               "developer.idigi.com"

/**
 * When defined, this string hardcode for the @ref tx_keepalive instead of the application framework
 * function @ref  app_get_tx_keepalive_interval() (called via the @ref idigi_config_tx_keepalive @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref  app_get_tx_keepalive_interval() when IDIGI_TX_KEEPALIVE_IN_SECONDS is defined.
 *
 * @see @ref tx_keepalive
 * @see @ref idigi_config_tx_keepalive
 * @see @ref  app_get_tx_keepalive_interval()
 */
#define IDIGI_TX_KEEPALIVE_IN_SECONDS                 75

/**
 * When defined, this string hardcode for the @ref rx_keepalive instead of the application framework
 * function @ref app_get_rx_keepalive_interval() (called via the @ref idigi_config_rx_keepalive @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_rx_keepalive_interval() when IDIGI_RX_KEEPALIVE_IN_SECONDS is defined.
 *
 * @see @ref rx_keepalive
 * @see @ref idigi_config_rx_keepalive
 * @see @ref app_get_rx_keepalive_interval()
 */
#define IDIGI_RX_KEEPALIVE_IN_SECONDS                 75

/**
 * When defined, this string hardcode for the @ref wait_count instead of the application framework
 * function @ref app_get_wait_count() (called via the @ref idigi_config_wait_count @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_wait_count() when IDIGI_RX_KEEPALIVE_IN_SECONDS is defined.
 *
 * @see @ref wait_count
 * @see @ref idigi_config_wait_count
 * @see @ref app_get_wait_count()
 */
#define IDIGI_WAIT_COUNT                              5

/**
 * When defined, this 4 byte value is hardcode for the @ref vendor_id instead of the application framework
 * function @ref app_get_vendor_id() (called via the @ref idigi_config_vendor_id @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_wait_count() when IDIGI_RX_KEEPALIVE_IN_SECONDS is defined.
 *
 * @see @ref vendor_id
 * @see @ref idigi_config_vendor_id
 * @see @ref app_get_vendor_id()
 * @see @ref idigi_vendor_id "Obtaining an iDigi Vendor ID"
 */
#define IDIGI_VENDOR_ID                            0x00000000

/**
 * When defined, this string hardcode for the @ref max_msg_transactions instead of the application framework
 * function @ref app_get_max_message_transactions() (called via the @ref idigi_config_max_transaction @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_max_message_transactions() when IDIGI_MSG_MAX_TRANSACTION is defined.
 *
 * @see @ref max_msg_transactions
 * @see @ref idigi_config_max_transaction
 * @see @ref app_get_max_message_transactions()
 */
#define IDIGI_MSG_MAX_TRANSACTION                  1

/**
 * When defined, this string hardcode for the @ref connection_type instead of the application framework
 * function @ref app_get_connection_type() (called via the @ref idigi_config_connection_type @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_connection_type() when IDIGI_CONNECTION_TYPE is defined.  The
 * values for this define are limited to @ref idigi_lan_connection_type or @ref idigi_wan_connection_type.
 *
 * @see @ref connection_type
 * @see @ref idigi_config_connection_type
 * @see @ref app_get_connection_type()
 * @see @ref idigi_connection_type_t
 */
#define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type

/**
 * When defined, this string hardcode for the @ref link_speed instead of the application framework
 * function @ref app_get_link_speed() (called via the @ref idigi_config_link_speed @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_link_speed() when IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND is defined.
 *
 * @see @ref link_speed
 * @see @ref idigi_config_link_speed
 * @see @ref app_get_link_speed()
 */
#define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0

/**
 * When defined, this string hardcode for the @ref phone_number instead of the application framework
 * function @ref app_get_phone_number() (called via the @ref idigi_config_phone_number @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_phone_number() when IDIGI_WAN_PHONE_NUMBER_DIALED is defined.
 *
 * @see @ref phone_number
 * @see @ref idigi_config_phone_number
 * @see @ref app_get_phone_number()
 */
#define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678"

/**
 * When defined, this string hardcode for the @ref firmware_support instead of the application framework
 * function @ref app_get_firmware_support() (called via the @ref idigi_config_firmware_facility @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_firmware_support() when IDIGI_FIRMWARE_SUPPORT is defined.
 * @note This define is only used when @ref IDIGI_FIRMWARE_SERVICE is defined in @ref idigi_config.h.
 *
 * @see @ref firmware_support
 * @see @ref idigi_config_firmware_facility
 * @see @ref app_get_firmware_support()
 * @see @ref IDIGI_FIRMWARE_SERVICE
 */
#define IDIGI_FIRMWARE_SUPPORT

/**
 * When defined, this string hardcode for the @ref data_service_support instead of the application framework
 * function @ref app_get_data_service_support() (called via the @ref idigi_config_data_service @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_data_service_support() when IDIGI_DATA_SERVICE_SUPPORT is defined.
 * @note This define is only used when @ref IDIGI_DATA_SERVICE is defined in @ref idigi_config.h.
 *
 * @see @ref data_service_support
 * @see @ref idigi_config_data_service
 * @see @ref app_get_data_service_support()
 * @see @ref IDIGI_DATA_SERVICE
 */
#define IDIGI_DATA_SERVICE_SUPPORT

/**
 * When defined, this string hardcode for the @ref file_system_support instead of the application framework
 * function @ref app_get_file_system_support() (called via the @ref idigi_config_file_system @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_file_system_support() when IDIGI_FILE_SYSTEM_SUPPORT is defined.
 * @note This define is only used when @ref IDIGI_FILE_SYSTEM is defined in @ref idigi_config.h.
 *
 * @see @ref file_system_support
 * @see @ref idigi_config_file_system
 * @see @ref app_get_file_system_support()
 * @see @ref IDIGI_FILE_SYSTEM
 */
#define IDIGI_FILE_SYSTEM_SUPPORT

/**
 * When defined, this string hardcode for the remote_configuration_support instead of the application framework
 * function @ref app_get_remote_configuration_support() (called via the @ref idigi_config_remote_configuration @ref idigi_callback_t "callback" in config.c).
 *
 * @note There is no need to implement or port @ref app_get_remote_configuration_support() when IDIGI_REMOTE_CONFIGURATION_SUPPORT is defined.
 * @note This define is only used when @ref IDIGI_RCI_MAXIMUM_CONTENT_LENGTH is defined a non-zero in @ref idigi_config.h.
 *
 * @see @ref idigi_config_remote_configuration
 * @see @ref app_get_remote_configuration_support()
 * @see @ref IDIGI_RCI_MAXIMUM_CONTENT_LENGTH
 */
#define IDIGI_REMOTE_CONFIGURATION_SUPPORT

#endif

/**
* @}
*/


#endif
