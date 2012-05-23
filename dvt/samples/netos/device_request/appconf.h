/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */


#ifndef appconf_h

#define appconf_h

#include <stdio.h>
#include <stdlib.h>
#include "appconf_api.h"
#include "wln_api.h"
#include "wps_def.h"
/*ad*
 * Set to run POST at start up.
 *
 * Set APP_POST to TRUE if you want the BSP to run the Power On Self Test
 * at startup.  Set APP_POST to FALSE to skip the POST.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 1.0
*ad*/
extern const int APP_POST;


/*ad*
 * Set to initialize the C++ runtime environment at start up.
 *
 * APP_CPP determines whether or not the application is a C++ based application.  If the
 * application is C++ based, set APP_CPP to TRUE, otherwise set to FALSE or leave undefined.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 1.0
*ad*/
#define APP_CPP                 FALSE


/*ad*
 * APP_ERROR_HANDLER defines the user error handler for fatal errors.  It should either be set
 * to NULL, or set to the name of a routine that should be called if a fatal error occurs.  The
 * routine should be of the form:
 *
 *      void error_handler (char *errorDescription, int errorCode, int errorSubcode)
 *
 * If APP_ERROR_HANDLER is NULL, then the default error handler will be used.  This error handler
 * blinks the LEDs on the development board in an infinite loop.  The LEDs are blinked in a pattern
 * that indicates the errorType.  
 *
 * @external
 * @category ApplicationConfiguration
 * @since 1.0
 * @deprecated  6.0
*ad*/
#define APP_ERROR_HANDLER           NULL


/*ad*
 * APP_ROOT_STACK_STACK sets the size of the root thread's stack.  The root thread is the
 * thread that calls calls applicationStart.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_ROOT_STACK_SIZE     (2*12288)

/*ad*
 * APP_ROOT_PRIORITY sets the priority level for the root thread.  The root thread is the
 * thread that calls calls applicationStart.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_ROOT_PRIORITY           16


/*ad*
 * APP_DEFAULT_API_PRIORITY determines the priority level threads internal to the
 * NET+Works APIs will be use.  The global variable bsp_default_api_priority is
 * loaded with this value.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_DEFAULT_API_PRIORITY    (NATranslatePriorityLevel(naDefaultPriority))


/*ad*
 * APP_FILE_SYSTEM_SIZE determines the numbers of file the Advanced Web server(AWS)
 * will allocated for file system. The default value is 9, the number of connections 
 * The AWS alocated. Increasing APP_FILE_SYSTEM_SIZE will reduce memory available. 
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_FILE_SYSTEM_SIZE          9



/*ad*
 * APP_DIALOG_APP_NAME determines the name of the current application program.  This name
 * is displayed at the top of the configuration dialog.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_DIALOG_APP_NAME         "Device Request Application"


/*ad*
 * APP_ROOT_PASSWORD determines the root account password.  This account has all privileges and
 * can be used for any service requiring a login (i.e. FTP, telnet).  The password set here is
 * a default value that is used if the rootPassword field in NVRAM is empty.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_ROOT_PASSWORD           "password"


/*ad*
 * APP_ADMINISTRATORS_ACCOUNT is a username used for a secure gateway account.  This account has 
 * privileges only for a gateway (so it can not be used for insecure applications like FTP or
 * Telnet).  This initial password is also APP_ADMINISTRATORS_ACCOUNT, but can be changed on
 * the dialog console, as is persistent via NVRAM.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_ADMINISTRATORS_ACCOUNT  "admin"

#if defined(HAVE_SECURITY_API) 

/*ad*
 * APP_FIXED_GATEWAY_KEY is the fixed key used for the secure gateway.  This Key can be used  
 * at the remote side to when using Fixed Key distribution.
 *
 * This feature cannot be used unless you have purchased the security API.  You must leave this
 * value commented out to avoid build errors unless you have installed the security API.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_FIXED_GATEWAY_KEY       "TESTKEY"


#endif /* defined(HAVE_SECURITY_API) */

#if defined(AUTODOC_VERSION)
/*ad*
 * APP_USE_NETWORK_TIME_PROTOCOL uses the NVRAM configuration to store Network Time Protocol
 * Servers used for the NET+OS SNTP Client on a NAstartSntp() call.  If this constant is
 * set, then the BSP will start the SNTP client.  The C Library time() functions can be
 * used to read the time.
 *
 * @note    The BSP will call customizeErrorHandler if APP_USE_NETWORK_TIME_PROTOCOL is
 *          defined, but no SNTP server address has been set.
 *
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_USE_NETWORK_TIME_PROTOCOL
#endif
/* #define APP_USE_NETWORK_TIME_PROTOCOL */


/*ad*
 * You must set APP_USE_NVRAM to control whether your application read its configuration 
 * parameters from NVRAM, or uses the values defined in this file.
 *
 * <list type="unordered">
 *  <item>Set APP_USE_NVRAM to APP_FOR_ALL_PARAMETERS if all configuration parameters
 *        should be read from NVRAM.  The values in this file will only be used if
 *        the contents of NVRAM are corrupted, or if a hardware failure prevents us from 
 *        reading it.</item>
 *  <item>Set APP_USE_NVRAM to APP_DO_NOT_USE_NVRAM if all configuration parameters 
 *        should be read from this file.</item>
 *  <item>Set APP_USE_NVRAM to APP_FOR_MAC_ADDRESS_ONLY if all configuration parameters
 *        except the MAC address should be read from NVRAM.  The MAC address will be read
 *        from this file only if NVRAM is unreadable or corrupted.</item>
 * </list>
 *
 * @external
 * @category general_purpose 
 * @since 1.0
*ad*/
#define APP_USE_NVRAM             APP_FOR_ALL_PARAMETERS

/*ad*
 * APP_NET_HEAP_SIZE sets the TCP/IP stack heap size for dynamic allocations.  The TCP/IP 
 * allocates all packet buffers from this piece of memory.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_NET_HEAP_SIZE	        0x120000

/*ad*
 * PPP is enabled on COM1 when APP_ENABLE_PPP1 is nonzero.
 * Note when this is defined BSP_DIALOG_PORT and BSP_STDIO_PORT must not use
 * COM1 ("/com/0")
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_ENABLE_PPP1           0

/*ad*
 * PPP is enabled on COM2 when APP_ENABLE_PPP2 is nonzero.
 * Note when this is defined BSP_DIALOG_PORT and BSP_STDIO_PORT must not use
 * COM2 ("/com/1")
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_ENABLE_PPP2          0


/*ad*
 * APP_IP_ADDRESS will be used as the default IP address if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category general_purpose 
 * @since 3.0
*ad*/
#define APP_IP_ADDRESS              "192.168.187.163"


/*ad*
 * APP_IP_SUBNET_MASK will be used as the default subnet mask if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category general_purpose 
 * @since 3.0
*ad*/
#define APP_IP_SUBNET_MASK          "255.255.255.0"

/*ad*
 * APP_IP_GATEWAY will be used as the default gateway if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.  If there is no gateway, then set APP_IP_GATEWAY to "0.0.0.0".
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category general_purpose 
 * @since 3.0
*ad*/
#define APP_IP_GATEWAY              "192.168.187.218"

/*ad*
 * APP_PRIMARY_DNS_ADDRESS will be used as the default primary DNS address if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.  If there is no primary DNS address, then set APP_SECONDARY_DNS_ADDRESS to "0.0.0.0".
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_PRIMARY_DNS_ADDRESS   "192.168.187.1"

/*ad*
 * APP_SECONDARY_DNS_ADDRESS will be used as the default secondary DNS address if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.  If there is no primary DNS address, then set APP_SECONDARY_DNS_ADDRESS to "0.0.0.0".
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_SECONDARY_DNS_ADDRESS   "192.168.187.2"

/*ad*
 * APP_SNTP_PRIMARY determine the unit's default primary 
 * SNTP servers.  All values should be specified in dotted notation as strings.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_SNTP_PRIMARY            "0.0.0.0"


/*ad*
 * APP_SNTP_SECONDARY determine the unit's default secondary
 * SNTP servers.  All values should be specified in dotted notation as strings.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_SNTP_SECONDARY          "0.0.0.0"







/*ad*
 * APP_BSP_SERIAL_NUM determines the default serial number to use if APP_USE_NVRAM is 0,
 * or if NVRAM is uninitialized.  The serial number should consist of 1 alpha character
 * followed by 8 digits.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_BSP_SERIAL_NUM	        "N99999999"


/*ad*
 * APP_BSP_MAC_ADDRESS determines the default Ethernet MAC address to use if 
 * APP_USE_NVRAM is 0, or if NVRAM is uninitialized.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 6.0
*ad*/
#define APP_BSP_MAC_ADDRESS         "00:40:9d:43:35:97"


/*ad*
 * APP_2ND_INTERFACE_MAC_ADDRESS determines the default MAC address to use 
 * on the second network interface if APP_USE_NVRAM is 0, or if NVRAM is uninitialized. 
 * APP_2ND_INTERFACE_MAC_ADDRESS is used only if the board has two network interfaces.
 * If the board has one Ethernet interface and one wireless interface, the connectcorewi9c_a
 * for example, then APP_2ND_INTERFACE_MAC_ADDRESS sets the MAC address on the
 * wireless interface.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 6.0
*ad*/
#define APP_2ND_INTERFACE_MAC_ADDRESS         "00:40:9d:43:35:98"

/*ad*
 * APP_2ND_INTERFACE_IP_ADDRESS will be used as the default IP address on the second network
 * interface if APP_USE_NVRAM is set to 0, or if NVRAM has not been initialized.  
 * This value is only used on boards that have two network interfaces, such as the 
 * connectcorewi9c_a.  If one interface is Ethernet, and the other is wireless, then the
 * wireless interface is the second interface.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category general_purpose 
 * @since 3.0
*ad*/
#define APP_2ND_INTERFACE_IP_ADDRESS              "192.168.187.164"


/*ad*
 * APP_2ND_INTERFACE_IP_SUBNET_MASK will be used as the default subnet mask on the second 
 * network interface if APP_USE_NVRAM is set to 0, or if NVRAM has not been initialized.
 * This value is only used on boards that have two network interfaces, such as the 
 * connectcorewi9c_a.  If one interface is Ethernet, and the other is wireless, then the
 * wireless interface is the second interface.
 * 
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category general_purpose 
 * @since 3.0
*ad*/
#define APP_2ND_INTERFACE_IP_SUBNET_MASK          "255.255.255.0"

/*ad*
 * APP_2ND_INTERFACE_IP_GATEWAY will be used as the default gateway on the second interface 
 * if APP_USE_NVRAM is set to 0, or if NVRAM has not been initialized.  
 * If there is no gateway, then set APP_IP_GATEWAY to "0.0.0.0".
 * This value is only used on boards that have two network interfaces, such as the 
 * connectcorewi9c_a.  If one interface is Ethernet, and the other is wireless, then the
 * wireless interface is the second interface.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category general_purpose 
 * @since 3.0
*ad*/
#define APP_2ND_INTERFACE_IP_GATEWAY              "192.168.187.218"

/*ad*
 * APP_2ND_INTERFACE_PRIMARY_DNS_ADDRESS will be used as the default primary DNS address on the second interface 
 * if APP_USE_NVRAM is set to 0, or if NVRAM has not been initialized.  
 * If there is no DNS, then set APP_2ND_INTERFACE_PRIMARY_DNS_ADDRESS to "0.0.0.0".
 * This value is only used on boards that have two network interfaces, such as the 
 * connectcorewi9c_a.  If one interface is Ethernet, and the other is wireless, then the
 * wireless interface is the second interface.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.5
*ad*/
#define APP_2ND_INTERFACE_PRIMARY_DNS_ADDRESS   "192.168.187.1"

/*ad*
 * APP_2ND_INTERFACE_SECONDARY_DNS_ADDRESS will be used as the default secondary DNS address on the second interface 
 * if APP_USE_NVRAM is set to 0, or if NVRAM has not been initialized.  
 * If there is no DNS, then set APP_2ND_INTERFACE_SECONDARY_DNS_ADDRESS to "0.0.0.0".
 * This value is only used on boards that have two network interfaces, such as the 
 * connectcorewi9c_a.  If one interface is Ethernet, and the other is wireless, then the
 * wireless interface is the second interface.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.5
*ad*/
#define APP_2ND_INTERFACE_SECONDARY_DNS_ADDRESS   "192.168.187.2"

/*ad*
 * APP_WLN_BSS_TYPE determines the BSS type when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to one of APP_WLN_BSS_ESS, APP_WLN_BSS_IBSS, APP_WLN_BSS_IBSS_JOIN,
 * or APP_WLN_BSS_ANY, or APP_WLN_BSS_NONE.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
 * @see @link wireless:APP_WLN_BSS_Values
 *
 * @note    Do not set APP_WLN_BSS_TYPE to one of the values in the enumerated type
 *          wln_bss.  The C preprocessor does not understand enumerated types and will
 *          parse an enumerated value as zero.
*ad*/
#define APP_WLN_BSS_TYPE            APP_WLN_BSS_ESS


/*ad*
 * APP_WLN_SSID determines the service set ID when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a quoted string, or NULL to search for an SSID.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_SSID                NULL





/*ad*
 * APP_WLN_CHANNEL determines the 802.11 channel when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * 
 * <list type="unordered">
 *  <item>Set APP_WLN_CHANNEL to SCAN to search for a channel.</item>
 *  <item>Set APP_WLN_CHANNEL to a value between 1 and 14 to select a channel
 *        in the 2.4GHz band (802.11b/g).</item>
 *  <item>Set APP_WLN_CHANNEL to a one of these values for band A channels:
 *        A8, A12, A16, A34, A36, A38, A40, A42, A44, A46, A48, A52, A56,
 *        A60, A64, A100, A104, A108, A112, A116, A120, A124, A128, A132,
 *        A136, A140, A149, A153, A157, A161, or A165.</item>
 * </list>
 *          
 * If the value of APP_WLN_CHANNEL is invalid, the default configuration will be
 * to scan all supported channels.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.0
*ad*/
#define APP_WLN_CHANNEL                 SCAN


/*ad*
 * APP_WLN_80211_OPTIONS sets the default options for 802.11d multi domain support,
 * and 802.11h spectrum management.
 * 
 * <list type="unordered">
 *  <item>Set APP_WLN_80211_OPTIONS to @link APP_WLN_80211_OPTION_NONE if neither 802.11d multi 
 *        domain support or 802.11h spectrum management should be enabled.</item>
 *  <item>Set APP_WLN_80211_OPTIONS to @link APP_WLN_80211_OPTION_MULTI_DOMAIN if 802.11d 
 *        multi domain support should be enabled.</item>
 *  <item>Set APP_WLN_80211_OPTIONS to @link APP_WLN_80211_OPTION_SPECTRUM_MANAGEMENT if
 *        both 802.11d multi domain support and 802.11h spectrum management should
 *        be turned on.</item>
 * </list>
 *          
 * This option is ignored if 802.11a is not supported by the hardware platform.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.4
*ad*/
#define APP_WLN_80211_OPTIONS   APP_WLN_80211_OPTION_NONE


/*ad*
 * APP_WLN_80211_BAND sets the default band for the 802.11 radio.
 * 
 * <list type="unordered">
 *  <item>Set APP_WLN_80211_BAND to APP_WLN_80211_BAND_ALL to enable support for
 *        all bands that the hardware can support.</item>
 *  <item>Set APP_WLN_80211_BAND to APP_WLN_80211_BAND_A to enable support for
 *        the 802.11a band.  The 802.11a band is only supported on the 
 *        Digi Connect WIEM 9210 and ConnectCore Wi-9P 9215 platforms.</item>
 *  <item>Set APP_WLN_80211_BAND to APP_WLN_80211_BAND_B to enable support for
 *        the 802.11b band.</item>
 *  <item>Set APP_WLN_80211_BAND to APP_WLN_80211_BAND_BG to enable support for
 *        the 802.11b and 802.11g bands.</item>
 *  <item>Set APP_WLN_80211_BAND to APP_WLN_80211_BAND_DEFAULT to enable support for
 *        the 802.11b on the Digi Connect Wi-EM, Digi Connect Wi-ME, and Digi Connect Wi-SP 
 *        platforms, and 802.11b and 802.11g bands
 *        on the Digi Connect WIEM 9210, ConnectCore Wi-9C, and ConnectCore Wi-9P 
 *        9215 platforms.</item>
 * </list>
 *          
 * If APP_WLN_80211_BAND is set incorrectly for the current platform platform, it
 * will be defaulted to APP_WLN_80211_BAND_DEFAULT.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.4
*ad*/
#define APP_WLN_80211_BAND  APP_WLN_80211_BAND_DEFAULT





/*ad*
 * APP_WLN_AUTHENTICATION_TYPE determines the authentication type when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to any combination of these values or'ed together:  WLN_AUTH_ANY, WLN_AUTH_OPEN, 
 * WLN_AUTH_SHAREDKEY, WLN_AUTH_WEP_8021X, WLN_AUTH_WPA_PSK, WLN_AUTH_WPA_8021X, or WLN_AUTH_LEAP.  
 *
 * @external
 * @category general_purpose 
 * @since 7.0
 * @see @link wireless:WirelessAuthenParam
*ad*/
#define APP_WLN_AUTHENTICATION_TYPE     WLN_AUTH_OPEN


/*ad*
 * APP_WLN_ENCRYPTION_TYPE determines the encryption type when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to any combination of these values or'ed together:  WLN_ENCR_ANY, WLN_ENCR_OPEN,
 * WLN_ENCR_WEP, WLN_ENCR_TKIP, or WLN_ENCR_CCMP.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
 * @see @link wireless:EncryptValues
*ad*/
#define APP_WLN_ENCRYPTION_TYPE         WLN_ENCR_OPEN


/*ad*
 * APP_WLN_KEY_ID determines the default wireless key when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a value between 0 and 3 to select one of APP_WLN_KEY_0, APP_WLN_KEY_1
 * APP_WLN_KEY_2, or APP_WLN_KEY_3.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_KEY_ID                  0


/*ad*
 * APP_WLN_KEY_0 determines the default value of wireless key 0 when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a 10 or 26 digit quoted hexadecimal number.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_KEY_0                   "012345ABCD"

/*ad*
 * APP_WLN_KEY_1 determines the default value of wireless key 1 when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a 5 or 13 digit hexadecimal number.  All keys must be the same length.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_KEY_1                   "012345ABCD"

/*ad*
 * APP_WLN_KEY_2 determines the default value of wireless key 2 when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a 5 or 13 digit hexadecimal number.  All keys must be the same length.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_KEY_2                   "012345ABCD"

/*ad*
 * APP_WLN_KEY_2 determines the default value of wireless key 2 when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a 5 or 13 digit hexadecimal number.  All keys must be the same length.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_KEY_3                   "012345ABCD"


/*ad*
 * APP_WLN_PASSPHRASE is used with the value of APP_WLN_SSID to generate a PSK when NVRAM is not
 * used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to a nul terminated string, between 8 and 63 characters long, to generate an
 * new PSK.  Generation of this key is processor intensive and will create a 30 second delay
 * when the unit comes up.
 *
 * Set APP_WLN_PASSPHRASE to NULL, or leave it undefined if you do not wish to generate a
 * new PSK.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_PASSPHRASE              NULL



/*ad*
 * Set APP_WLN_PSK to the wireless PSK value.  This is a 32-byte value that must be specified as a 
 * quoted 64 digit hexadecimal value.  
 *
 * Set this value to NULL if you do not want to specify the PSK.  In this case, the PSK will be
 * generated using the value of APP_WLN_PASSPHRASE.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_PSK  NULL






/*ad*
 * APP_WLN_EAP_METHODS determines which authentication methods may be used when NVRAM is not used, 
 * or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * Set this value to any combination of the values described in @link wireless:EAPValues or'ed 
 * together.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
 * @see @link wireless:EAPValues
*ad*/
#define APP_WLN_EAP_METHODS             WLN_EAP_ANY


/*ad*
 * APP_WLN_IDENTITY determines the user identity (login) when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * The identity is required if you are using dynamic WEP.
 * Set this value a nul terminated string.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_IDENTITY                "username"


/*ad*
 * APP_WLN_PASSWORD determines the user password when NVRAM is not used, or is corrupted.
 * This value is only relevant on platforms with an 802.11 network interface.
 * The password is required if you are using dynamic WEP.
 * Set this value a nul terminated string.
 *
 * @external
 * @category general_purpose 
 * @since 7.0
*ad*/
#define APP_WLN_PASSWORD                "password"


/*ad*
 * APP_WLN_COUNTRY_CODE determines the default wireless country code. This 
 * value is only relevant on platforms with an 802.11 network interface. Based
 * on the country code value, channel mask will be selected. 
 *
 * @external
 * @category general_purpose 
 * @include     "appconf.h"
 * @since 7.2
*ad*/
#define APP_WLN_COUNTRY_CODE    COUNTRY_UNITED_STATES 



/*ad*
 * APP_BSP_DELAY determines the amount of time to wait the configuration dialog will
 * wait for user input before continuing the BSP startup.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_BSP_DELAY		        5


/*ad*
 * Set this constant to indicate which interfaces should use static values.
 *
 * Each network interface needs to be assigned an IP Address, subnet mask,
 * and gateway.  These values can come from appconf.h, or the TCP/IP stack
 * can attempt to get these configuration settings from the network.
 *
 * The value of this constant determines the default configuration settings
 * for IAM.  These defaults are used if APP_USE_NVRAM is NOT set to 
 * APP_FOR_ALL_PARAMETERS, or if NVRAM is unreadable.  You must set APP_USE_STATIC_IP
 * to indicate whether the default setting is to use the configuration parameters
 * in appconf.h, or if the TCP/IP stack should try to get configuration settings
 * for the interface from the network.
 *
 * <list type="unordered">
 *  <item>Set APP_USE_STATIC_IP to APP_ON_NO_INTERFACE if the values in 
 *        appconf.h should not be used.  The default setting will be for the
 *        network stack to get configuration values from the network using 
 *        DHCP.</item>
 *  <item>Set APP_USE_STATIC_IP to APP_ON_ETHERNET_INTERFACE if the values in
 *        appconf.h should be used for the Ethernet interface, but not for the
 *        wireless interface.</item>
 *  <item>Set APP_USE_STATIC_IP to APP_ON_WIRELESS_INTERFACE if the values in
 *        appconf.h should be used for the wireless interface, but not for the
 *        Ethernet interface.</item>
 *  <item>Set APP_USE_STATIC_IP to APP_ON_ALL_INTERFACES if the values in
 *        appconf.h should be used for both the Ethernet and wireless interfaces.
 *        </item>
 * </list>
 *
 * @external
 * @include     "appconf.h"
 * @category ApplicationConfiguration 
 * @since 6.0
 * @see @link APP_Interface_Values
*ad*/
#define APP_USE_STATIC_IP            APP_ON_NO_INTERFACE


/*ad*
 * APP_DEFAULT_ETHERNET_DUPLEX determines the default Ethernet duplex setting.  Normally,
 * the PHY will negotiate the duplex setting with the hub or switch.  However, some hubs
 * do not support autonegotiation, or allow the end user to disable it.  In this case,
 * there is no way for the PHY to determine what duplex setting should be used.  It will
 * use the value stored in NVRAM.  The value of APP_DEFAULT_ETHERNET_DUPLEX will be
 * written to the NVRAM field when it is first initialized.
 *
 * Legal values are bpHalfDuplex, bpFullDuplex, and bpDefaultDuplex.  The recommended 
 * setting is bpDefaultDuplex.  This uses the default duplex setting supported by the
 * Ethernet PHY.  Most PHYs will default to half duplex.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 6.0
 * @see @link bpDuplexLevelType
*ad*/
#define APP_DEFAULT_ETHERNET_DUPLEX  bpDefaultDuplex


/*ad*
 * Set this constant to indicate which interfaces should use Auto-IP.
 *
 * The TCP/IP stack can derive an IP address for an interface using a protocol named
 * Auto-IP.  However, this may be undesirable because:
 *
 * <list type="ordered">
 *  <item>The addresses created by Auto-IP are not routable.  This means that packets 
 *        sent to network stations with these addresses will be discarded by routers.
 *        Therefore, Auto-IP addresses are only usable on the local network.</item>
 *  <item>The Auto-IP process is usually faster than DHCP.  So, if Auto-IP and
 *        DHCP are both enabled, the system will come up with an Auto-IP address before
 *        it gets an address from DHCP (which is routable).</item>
 *  <item>This may cause applications to open and bind sockets to the Auto-IP address.  These
 *        sockets will not be usable in many cases because they cannot be used to
 *        communicate with nodes outside of the local network.</item>
 * </list>
 *
 * You must set APP_ENABLE_AUTO_IP to indicate the default configuration for 
 * Auto-IP.  This configuration will be used if APP_USE_NVRAM is NOT set to 
 * APP_FOR_ALL_PARAMETERS, or if NVRAM is unreadable.  
 *
 * <list type="unordered">
 *  <item>Set APP_ENABLE_AUTO_IP to APP_ON_NO_INTERFACE if Auto-IP should not
 *        be used at all.</item>
 *  <item>Set APP_ENABLE_AUTO_IP to APP_ON_ETHERNET_INTERFACE if Auto-IP should
 *        be used on Ethernet, but not wireless.</item>
 *  <item>Set APP_ENABLE_AUTO_IP to APP_ON_WIRELESS_INTERFACE if Auto-IP should
 *        be used on wireless, but not Ethernet.</item>
 *  <item>Set APP_ENABLE_AUTO_IP to APP_ON_ALL_INTERFACES if Auto-IP should be
 *        used on all interfaces.
 *        </item>
 * </list>
 *  
 * @external
 * @include     "appconf.h"
 * @category ApplicationConfiguration
 * @since 6.0
 * @see @link APP_Interface_Values
*ad*/
#define APP_ENABLE_AUTO_IP  APP_ON_ALL_INTERFACES

/*ad*
 * APP_MANUFACTURER defines the name of the manufacturer.  
 *
 * @external
 * @category ApplicationConfiguration
 * @since 7.4
*ad*/
#define APP_MANUFACTURER        NULL

/*ad*
 * APP_MODEL_NAME defines the model name.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 7.4
*ad*/
#define APP_MODEL_NAME          NULL

/*ad*
 * APP_MODEL_NUMBER defines model number.  
 *
 * @external
 * @category ApplicationConfiguration
 * @since 7.4
*ad*/
#define APP_MODEL_NUMBER        NULL
/*ad*
 * APP_DEVICE_NAME defines device name.  
 *
 * @external
 * @category ApplicationConfiguration
 * @since 7.4
*ad*/
#define APP_DEVICE_NAME         NULL

/*ad*
 * APP_DEVICE_TYPE defines the type of the device.
 * @see @link NaWpsDeviceTypes  
 *
 * @external
 * @category ApplicationConfiguration
 * @since 7.4
*ad*/
#define APP_DEVICE_TYPE         WPS_DEVICE_TYPE_NONE

/*ad*
 * APP_IPV6_ADDRESS will be used as the default static IPv6 address, if APP_USE_NVRAM 
 * is set to 0 (or if NVRAM has not been initialized) and @link APP_USE_STATIC_IPV6 
 * is enabled for the interface.
 *
 * This value should be specified as a properly formatted IPv6 address string.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.5
*ad*/
#define	APP_IPV6_ADDRESS		"::"

/*ad*
 * APP_IPV6_PREFIX_LEN will be used as network prefix length for 
 * the default static IPv6 address, if APP_USE_NVRAM 
 * is set to 0 (or if NVRAM has not been initialized) and @link APP_USE_STATIC_IPV6 
 * is enabled for the interface.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.5
*ad*/
#define	APP_IPV6_PREFIX_LEN		64

/*ad*
 * APP_2ND_INTERFACE_IPV6_ADDRESS will be used as the default static IPv6 address for
 * the 2nd netword interface (on the system with 2 network interfaces), if APP_USE_NVRAM 
 * is set to 0, or if NVRAM has not been initialized and @link APP_USE_STATIC_IPV6 
 * is enabled for the interface.
 *
 * This value should be specified as a properly formatted IPv6 address string.  
 *
 * @external
 * @category general_purpose 
 * @since 7.5
*ad*/
#define	APP_2ND_INTERFACE_IPV6_ADDRESS		"::"

/*ad*
 * APP_IPV6_PREFIX_LEN will be used as network prefix length for 
 * the default 2nd interface static IPv6 address, if APP_USE_NVRAM 
 * is set to 0 (or if NVRAM has not been initialized) and @link APP_USE_STATIC_IPV6 
 * is enabled for the interface.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 7.5
*ad*/
#define	APP_2ND_INTERFACE_IPV6_PREFIX_LEN	64

/*ad*
 * Set this constant to indicate which interfaces should configure a Static IPv6 address.
 *
 * The value of this constant determines the default configuration settings
 * for IAM.  These defaults are used if APP_USE_NVRAM is NOT set to 
 * APP_FOR_ALL_PARAMETERS, or if NVRAM is unreadable. 
 *
 * <list type="unordered">
 *  <item>Set APP_USE_STATIC_IPV6 to APP_ON_NO_INTERFACE to disable Static IPv6
 *		  address configuration for all interfaces. </item>
 *  <item>Set APP_USE_STATIC_IPV6 to APP_ON_ETHERNET_INTERFACE to enable 
 *        Static IPv6 address configuration for the Ethernet interface, 
 *		  but not for the wireless interface.</item>
 *  <item>Set APP_USE_STATIC_IPV6 to APP_ON_WIRELESS_INTERFACE to enable 
 *        Static IPv6 address configuration for the wireless interface, 
 *		  but not for the Ethernet interface.</item>
 *  <item>Set APP_USE_STATIC_IPV6 to APP_ON_ALL_INTERFACES to enable 
 *        Static IPv6 address configuration for both Ethernet and wireless interfaces. 
 *        </item>
 * </list>
 *
 * @external
 * @include     "appconf.h"
 * @category ApplicationConfiguration 
 * @since 7.5
 * @see @link APP_Interface_Values
 * @see @link APP_IPV6_ADDRESS
 * @see @link APP_IPV6_PREFIX_LEN
 * @see @link APP_2ND_INTERFACE_IPV6_ADDRESS
 * @see @link APP_2ND_INTERFACE_IPV6_PREFIX_LEN
*ad*/
#define	APP_USE_STATIC_IPV6		APP_ON_NO_INTERFACE

/*ad*
 * Set this constant to indicate which interfaces should configure a DHCPv6 address.
 *
 * The value of this constant determines the default configuration settings
 * for IAM.  These defaults are used if APP_USE_NVRAM is NOT set to 
 * APP_FOR_ALL_PARAMETERS, or if NVRAM is unreadable. 
 *
 * <list type="unordered">
 *  <item>Set APP_USE_DHCPV6 to APP_ON_NO_INTERFACE to disable DHCPv6
 *		  address configuration for all interfaces. </item>
 *  <item>Set APP_USE_DHCPV6 to APP_ON_ETHERNET_INTERFACE to enable 
 *        DHCPv6 address configuration for the Ethernet interface, 
 *		  but not for the wireless interface.</item>
 *  <item>Set APP_USE_DHCPV6 to APP_ON_WIRELESS_INTERFACE to enable 
 *        DHCPv6 address configuration for the wireless interface, 
 *		  but not for the Ethernet interface.</item>
 *  <item>Set APP_USE_DHCPV6 to APP_ON_ALL_INTERFACES to enable 
 *        DHCPv6 address configuration for both Ethernet and wireless interfaces. 
 *        </item>
 * </list>
 *
 * @external
 * @include     "appconf.h"
 * @category ApplicationConfiguration 
 * @since 7.5
 * @see @link APP_Interface_Values
*ad*/
#define	APP_USE_DHCPV6			APP_ON_NO_INTERFACE


#endif


