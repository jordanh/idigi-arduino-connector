/*
* Copyright (c) 2011, 2012 Digi International Inc.,
* All rights not expressly granted are reserved.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
* =======================================================================
*/
#ifndef FLASH_MQX_H_
#define FLASH_MQX_H_

extern MQX_FILE_PTR flash_file;

/* Defines for NVRAM Flash storage */

/**************************************************************************/ /*!
 * @brief Parameters-version string.@n
 * It defines version of the parameter structure saved in a persistent storage.
 ******************************************************************************/
#define FAPP_PARAMS_VERSION                 "01" /* Changed on any change in the param. structures.*/

/**************************************************************************/ /*!
 * @brief Signature string value.@n
 * It's used for simple check if configuration structure is present 
 * in a persistant storage.
 ******************************************************************************/
#define FAPP_PARAMS_SIGNATURE               "FNET"FAPP_PARAMS_VERSION

/**************************************************************************/ /*!
 * @brief The maximum length of the signature.
 ******************************************************************************/
#define FAPP_PARAMS_SIGNATURE_SIZE          (12)

/**************************************************************************/ /*!
 * @brief The maximum length of the boot-script.
 ******************************************************************************/
#define FAPP_PARAMS_BOOT_SCRIPT_SIZE        (60)

/**************************************************************************/ /*!
 * @brief The maximum length of the @c file_name field of the 
 * @ref fapp_params_tftp structure.
 ******************************************************************************/
#define FAPP_PARAMS_TFTP_FILE_NAME_SIZE     (40)


/**************************************************************************/ /*!
 * @brief Boot mode.
 ******************************************************************************/
typedef enum
{
    FAPP_PARAMS_BOOT_MODE_STOP      = (0),  /**< @brief Stop at the shell prompt. 
                                             */
    FAPP_PARAMS_BOOT_MODE_GO        = (1),  /**< @brief Boot from flash. @n
                                             * The entry point address is defined 
                                             * by @ref fapp_params_boot.go_address.
                                             * The boot will be automatically 
                                             * started after @ref @ref fapp_params_boot.delay 
                                             * seconds.
                                             */
    FAPP_PARAMS_BOOT_MODE_SCRIPT    = (2)   /**< @brief Start boot-script. @n
                                             * The boot-script commands located 
                                             * in the @ref fapp_params_boot.script
                                             * will be automatically started after 
                                             * @ref @ref fapp_params_boot.delay 
                                             * seconds.  
                                             */
} 
fapp_params_boot_mode_t;


/**************************************************************************/ /*!
 * @brief Image-file type.
 ******************************************************************************/
typedef enum
{
    FAPP_PARAMS_TFTP_FILE_TYPE_RAW  = (0),  /**< @brief Raw binary file. 
                                             */
    FAPP_PARAMS_TFTP_FILE_TYPE_BIN  = (1),  /**< @brief CodeWarrior binary file. 
                                             */
    FAPP_PARAMS_TFTP_FILE_TYPE_SREC = (2)   /**< @brief SREC file.
                                             */
} 
fapp_params_tftp_file_type_t;

#define FNET_MK (1)

/* To use _Pragma(), enable C99 Extensions. */

#pragma c99 on

/* For CW10.1 ARM compiler. After CW10.2 Release is obsolete. */
#if FNET_MK
	#define FNET_COMP_PACKED __attribute__ ((aligned (1)))
#endif

#if 1 /* Newest CWs pragma */
	#define FNET_COMP_PACKED_BEGIN  _Pragma("pack(1)")
	#define FNET_COMP_PACKED_END    _Pragma("pack()")
#else /* Old CWs pragma */
	#define FNET_COMP_PACKED_BEGIN  _Pragma("options align = packed")
	#define FNET_COMP_PACKED_END    _Pragma("options align = reset")
#endif

FNET_COMP_PACKED_BEGIN

/**************************************************************************/ /*!
 * @brief Application parameters structure used to save the FNET Stack 
 * specific configuration to a persistent storage.
 ******************************************************************************/
struct fapp_params_fnet
{
    unsigned long address 	FNET_COMP_PACKED;	/**< @brief Application IP address. 
                            					 */
    unsigned long netmask 	FNET_COMP_PACKED;  	/**< @brief Netmask. 
                            				     */
    unsigned long gateway 	FNET_COMP_PACKED;  	/**< @brief Gateway IP address. 
     	 	 	 	 	 	 	 	 	 	 	 */
    unsigned long dns 		FNET_COMP_PACKED;	/**< @brief DNS server address. 
	 	 	 	                                 */                            
    unsigned char mac[6] 	FNET_COMP_PACKED;   /**< @brief Ethernet MAC address. 
                                                 */
    unsigned char _pad[2] 	FNET_COMP_PACKED;  	/**< @brief NOT USED. It is used just only for padding. 
                                                 */                            
};

/**************************************************************************/ /*!
 * @brief Application parameter structure used to save the bootloader 
 * specific configuration to a persistent storage.
 ******************************************************************************/
struct fapp_params_boot
{
    unsigned long mode	FNET_COMP_PACKED;     /**< @brief Boot mode defined by the 
                             * @ref fapp_params_boot_mode_t.
                             */
    unsigned long delay FNET_COMP_PACKED;    /**< @brief Boot delay.@n
											 * After bootup, the bootloader will wait 
											 * this number of seconds before it executes 
											 * the boot-mode defined by @c mode field.
											 * During this time a countdown is printed, 
											 * which can be interrupted by pressing any key. @n
											 * Set this variable to 0, to boot without delay. @n
											 * It's ignored for the @ref FAPP_PARAMS_BOOT_MODE_STOP
											 * mode.
											 */
    unsigned long go_address FNET_COMP_PACKED; /**< @brief Default entry point address
                             * to start execution at. @n
                             * It is used by bootloader in @ref FAPP_PARAMS_BOOT_MODE_GO
                             * mode as the default entry point. Also it is used 
                             * as the default address for the "go" shell command 
                             * if no address is provided to. 
                             */                                      
    char script[FAPP_PARAMS_BOOT_SCRIPT_SIZE] FNET_COMP_PACKED; /**< @brief Command script string. @n
                             * It is automatically executed when the 
                             * @ref FAPP_PARAMS_BOOT_MODE_SCRIPT mode
                             * is set and the initial countdown is not interrupted. @n
                             * This script may contain any command supported by 
                             * the application shell. The commands must be split 
                             * by semicolon operator. @n
                             * The string must be NUL-terminated.
                             */
};

/**************************************************************************/ /*!
 * @brief Application parameter structure used to save the TFTP loader 
 * specific configuration to a persistent storage.
 ******************************************************************************/
struct fapp_params_tftp
{
    unsigned long server FNET_COMP_PACKED;           /**< @brief This is the default TFTP server 
                                     * IP address to be used for network download 
                                     * if no address is provided to the "tftp" 
                                     * shell command.
                                     */
    unsigned long file_type FNET_COMP_PACKED;        /**< @brief This is the default file type, defined 
                                     * by the @ref fapp_params_tftp_file_type_t, 
                                     * to be used for network download if no 
                                     * type is provided to the "tftp" shell command.
                                     */                            
    unsigned long file_raw_address FNET_COMP_PACKED; /**< @brief Load address for raw-binary file 
                                     * for the TFTP loader. @n
                                     * It's used only if @c file_type is set to 
                                     * @ref FAPP_PARAMS_TFTP_FILE_TYPE_RAW.
                                     */                
    char file_name[FAPP_PARAMS_TFTP_FILE_NAME_SIZE] FNET_COMP_PACKED; /**< @brief This is the default file name
                                     * to be loaded by TFTP loader if no 
                                     * file name is provided to the "tftp" shell command. @n
                                     * The string must be NUL-terminated.
                                     */
};

/**************************************************************************/ /*!
 * @brief Main application  parameter structure used to save the
 * application specific configuration to a persistent storage.
 ******************************************************************************/
struct fapp_params_flash
{
    char signature[FAPP_PARAMS_SIGNATURE_SIZE] FNET_COMP_PACKED; /**< @brief Signature string.@n
                                             * It's used for simple check if configuration 
                                             * structure is present in a persistent storage. 
                                             */
    struct fapp_params_fnet fnet_params FNET_COMP_PACKED;    /**< @brief FNET TCP/IP stack specific 
                                             * configuration parameters.
                                             */
    struct fapp_params_boot boot_params FNET_COMP_PACKED;    /**< @brief Bootloader specific 
                                             * configuration parameters 
                                             */
    struct fapp_params_tftp tftp_params FNET_COMP_PACKED;    /**< @brief TFTP loader specific 
                                             * configuration parameters.  
                                             */
};

FNET_COMP_PACKED_END

#define FNET_CFG_ETH_IP_ADDR            0
#define FNET_CFG_ETH_IP_MASK            0
#define FNET_CFG_ETH_IP_GW              0
#define FNET_CFG_ETH_IP_DNS             0
#define FAPP_PARAMS_BOOT_MODE_STOP      (0)
#define FAPP_PARAMS_TFTP_FILE_TYPE_SREC (2)

/* Default parameters values*/
#define FAPP_CFG_PARAMS_BOOT_DELAY               (5) /*sec*/
#define FAPP_CFG_PARAMS_BOOT_GO_ADDRESS          (0)
#define FAPP_CFG_PARAMS_TFTP_FILE_RAW_ADDRESS    (0)
#define FAPP_CFG_PARAMS_MAC_ADDR                 (0) /* Defined by FNET */

#define FAPP_CFG_PARAMS_IP_ADDR             FNET_CFG_ETH_IP_ADDR    /* Defined by FNET */
#define FAPP_CFG_PARAMS_IP_MASK             FNET_CFG_ETH_IP_MASK    /* Defined by FNET */
#define FAPP_CFG_PARAMS_IP_GW               FNET_CFG_ETH_IP_GW      /* Defined by FNET */
#define FAPP_CFG_PARAMS_IP_DNS              FNET_CFG_ETH_IP_DNS

#define FAPP_CFG_PARAMS_TFTP_FILE_NAME  "shell_boot_intflash.elf.S19"
#define FAPP_CFG_PARAMS_TFTP_SERVER     0
#define FAPP_CFG_PARAMS_BOOT_MODE       FAPP_PARAMS_BOOT_MODE_STOP 
#define FAPP_CFG_PARAMS_BOOT_SCRIPT     "dhcp; erase all; tftp; set boot go; save; go"
#define FAPP_CFG_PARAMS_TFTP_FILE_TYPE  FAPP_PARAMS_TFTP_FILE_TYPE_SREC

#endif /* FLASH_MQX_H_ */
