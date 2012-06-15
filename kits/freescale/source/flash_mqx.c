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
#include "mqx.h"
#include "fio.h"
#include "flashx.h"
#include "ethernet.h"
#include "pcb.h"
#include "flash_mqx.h"
#include "idigi_config.h"

#ifndef APP_DEBUG
#define APP_DEBUG   _io_printf
#endif

#if defined IDIGI_MAC_ADDRESS && defined IDIGI_DEFAULT_MAC_ADDRESS
const _enet_address device_mac_addr = IDIGI_MAC_ADDRESS;
const _enet_address default_device_mac_addr = IDIGI_DEFAULT_MAC_ADDRESS;
#else
const _enet_address device_mac_addr = { 0x00, 0x40, 0x9d, 0x55, 0x29, 0xad };
const _enet_address default_device_mac_addr = { 0x00, 0x40, 0x9d, 0x55, 0x29, 0xad };
#endif

struct fapp_params_flash fapp_params_current_config;

struct fapp_params_flash fapp_params_default_config =      
{
    FAPP_PARAMS_SIGNATURE,
    {
        FAPP_CFG_PARAMS_IP_ADDR,    /* address */
        FAPP_CFG_PARAMS_IP_MASK,    /* netmask */
        FAPP_CFG_PARAMS_IP_GW,      /* gateway */
        FAPP_CFG_PARAMS_IP_DNS,     /* DNS */
        FAPP_CFG_PARAMS_MAC_ADDR,   /* MAC address */
    },
    {
        FAPP_CFG_PARAMS_BOOT_MODE,          /* boot */
        FAPP_CFG_PARAMS_BOOT_DELAY,         /* boot_delay */
        FAPP_CFG_PARAMS_BOOT_GO_ADDRESS,    /* go_address */                                      
        FAPP_CFG_PARAMS_BOOT_SCRIPT         /* boot_script */
    },
    {
        FAPP_CFG_PARAMS_TFTP_SERVER,            /* tftp_ip */
        FAPP_CFG_PARAMS_TFTP_FILE_TYPE,         /* image_type */  
        FAPP_CFG_PARAMS_TFTP_FILE_RAW_ADDRESS,  /* raw_address */ 
        FAPP_CFG_PARAMS_TFTP_FILE_NAME,         /* image */
    }
};

/* Local configuration parameters.
* Will be overwritten by parameters from flash if FAPP_CFG_PARAMS_READ_FLASH set to 1.
*/
struct fapp_params_boot fapp_params_boot_default_config =
{
FAPP_CFG_PARAMS_BOOT_MODE,          /* mode */
FAPP_CFG_PARAMS_BOOT_DELAY,         /* delay */
FAPP_CFG_PARAMS_BOOT_GO_ADDRESS,    /* go_address */                                      
FAPP_CFG_PARAMS_BOOT_SCRIPT         /* boot_script */
};

struct fapp_params_tftp fapp_params_tftp_default_config =
{
FAPP_CFG_PARAMS_TFTP_SERVER,            /* tftp_ip */
FAPP_CFG_PARAMS_TFTP_FILE_TYPE,         /* image_type */  
FAPP_CFG_PARAMS_TFTP_FILE_RAW_ADDRESS,  /* raw_address */ 
FAPP_CFG_PARAMS_TFTP_FILE_NAME,         /* image */
};
    
#define PFLASH_BLOCK1_BASE        0x00040000
#define FAPP_FLASH_PARAMS_ADDRESS 0x0007F800

#define FLASH_NAME "flashx:bank1"

struct fapp_params_flash *fnet_params = (struct fapp_params_flash *)FAPP_FLASH_PARAMS_ADDRESS;

void _app_setup_flash_params_address_for_writing(void)
{
	uint_32 current_address, sector_size, ioctl_param, num_sectors;
	
    /* Open the flash device */
    flash_file = fopen(FLASH_NAME, NULL);
    if (flash_file == NULL) {
	    return;
    }

    /* Get the size of the flash file */
    fseek(flash_file, 0, IO_SEEK_END);
    APP_DEBUG("\nSize of the flash file: 0x%x Bytes", ftell(flash_file));
    
    /* Get the number of sectors in the Flash File */    
    ioctl(flash_file, FLASH_IOCTL_GET_NUM_SECTORS, &num_sectors);
    APP_DEBUG("\nThe number of sectors in the flash file: %d\n", num_sectors);
    
    /* Get the sector size of the current sector (assuming all sectors are the same size) */
    fseek(flash_file, -FAPP_FLASH_PARAMS_ADDRESS, IO_SEEK_END);    
    ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
    APP_DEBUG("\nThe sector size is: %d\n", sector_size);
    
    if (sector_size == 0)
    {
    	sector_size = 2048;
    	APP_DEBUG("\nThe sector size is: %d\n", sector_size);
    }

    /* Enable sector cache */
    ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    APP_DEBUG("\nFlash sector cache enabled.");

    /* Unprotecting the the FLASH might be required */
    ioctl_param = 0;
    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);
   
    /*
     * Erase the params sector starting at the last sector
     * which is used by the bootloader for configuration data
     */
    
    /* Set the current address of Flash to the beginning of the 2nd bank of the flash file */
    fseek(flash_file, -PFLASH_BLOCK1_BASE + (sector_size * (num_sectors - 1)), IO_SEEK_END);    
    ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_BASE, &current_address);
    APP_DEBUG("\nCurrent address of the flash file: 0x%x\n", current_address);
}

void _app_erase_flash_params(void)
{
	_app_setup_flash_params_address_for_writing();
	
    /* Erase Current Sector */
    ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
    
    fclose(flash_file);
}

int _app_write_default_flash_params(void)
{
    int len, result = 0;

#ifdef IDIGI_USE_STATIC_IP    		
    unsigned long current_address;
    unsigned long current_netmask;
    unsigned long current_gateway;
    unsigned long current_dns;

    htonl(&current_address, IDIGI_DEVICE_IPADDR);
    htonl(&current_netmask, IDIGI_DEVICE_IPMASK);
    htonl(&current_gateway, IDIGI_DEVICE_GATEWAY);
    htonl(&current_dns, IDIGI_DNS_SERVER_IPADDR);
#endif
    
    _app_setup_flash_params_address_for_writing();
    
    /* Get iDigi Default MAC Address */
    memcpy(&fapp_params_default_config.fnet_params.mac[0], (char *)device_mac_addr, sizeof device_mac_addr);

    /* Get other iDigi params */
#ifdef IDIGI_USE_STATIC_IP
    fapp_params_default_config.fnet_params.address = current_address;
    fapp_params_default_config.fnet_params.netmask = current_netmask;
    fapp_params_default_config.fnet_params.gateway = current_gateway;
    fapp_params_default_config.fnet_params.dns = current_dns;
#endif
    
    len = write(flash_file, (unsigned long *)&fapp_params_default_config, sizeof(struct fapp_params_flash));
    if (len != sizeof(struct fapp_params_flash))
    {
    	result = -1;
    }
       
    fclose(flash_file);
    
    return result;
}

void _app_get_flash_params(struct fapp_params_flash * fapp_params_current_config, int size)
{
    memcpy (fapp_params_current_config, fnet_params, size);	
}

int _app_write_flash_params(struct fapp_params_flash * fapp_params_current_config, int size)
{
    int len, result = 0;
	
    _app_setup_flash_params_address_for_writing(); 
    
    len = write(flash_file, (unsigned long *)fapp_params_current_config, size);
    if (len != size)
    {
    	result = -1;
    }
       
    fclose(flash_file);
    
    return result;
}

#if BSPCFG_ENABLE_FLASHX && defined(IDIGI_GET_MAC_FROM_NVRAM)
void Flash_NVRAM_get_mac_address(uint8_t * const address)
{
	int result;
	
    /* Check signature. */
    if(strncmp( fnet_params->signature, FAPP_PARAMS_SIGNATURE, FAPP_PARAMS_SIGNATURE_SIZE ) != 0)
    {
        APP_DEBUG("Flash_NVRAM_get_mac_address: Signature mismatch\n");
       
        _app_erase_flash_params();
		   
        result = _app_write_default_flash_params();
		   
        if (result)
        	APP_DEBUG("Flash_NVRAM_get_mac_address: Error writing default params. Error code: %d\n", _io_ferror(flash_file));
    }
    else
    {
#ifdef IDIGI_USE_STATIC_IP
        unsigned long current_address;
        unsigned long current_netmask;
        unsigned long current_gateway;
        unsigned long current_dns;
 	   
        htonl(&current_address, IDIGI_DEVICE_IPADDR);
        htonl(&current_netmask, IDIGI_DEVICE_IPMASK);
        htonl(&current_gateway, IDIGI_DEVICE_GATEWAY);
        htonl(&current_dns, IDIGI_DNS_SERVER_IPADDR);
#endif
        /* 
         * If the default device_mac_address is the same as the 
         * default_device_mac_address, use the bootloaders MAC address
         */

        if (memcmp((char *)default_device_mac_addr, (char *)device_mac_addr, sizeof device_mac_addr) != 0)
        {
        	APP_DEBUG("Flash_NVRAM_get_mac_address: using iDigi Mac address\n");
        	
        	if (memcmp((char *)&fnet_params->fnet_params.mac[0], (char *)device_mac_addr, sizeof device_mac_addr) == 0)
        	{
        		APP_DEBUG("Flash_NVRAM_get_mac_address: iDigi Mac address already in NVRAM\n");	
        	}
        	else
        	{
                _app_get_flash_params((struct fapp_params_flash *) &fapp_params_current_config, sizeof(struct fapp_params_flash));
 		    
                /* Get iDigi Default MAC Address */
                memcpy(&fapp_params_current_config.fnet_params.mac[0], (char *)device_mac_addr, sizeof device_mac_addr);

                /* Get other iDigi params */
#ifdef IDIGI_USE_STATIC_IP
                fapp_params_current_config.fnet_params.address = current_address;
                fapp_params_current_config.fnet_params.netmask = current_netmask;
                fapp_params_current_config.fnet_params.gateway = current_gateway;
                fapp_params_current_config.fnet_params.dns = current_dns;
#endif
                _app_erase_flash_params();
    
                result = _app_write_flash_params((struct fapp_params_flash *) &fapp_params_current_config, sizeof(struct fapp_params_flash));
 		   
                if (result)
            	    APP_DEBUG("Flash_NVRAM_get_mac_address: Error writing params. Error code: %d\n", _io_ferror(flash_file));
        	}
        }
        else
        {
        	APP_DEBUG("Flash_NVRAM_get_mac_address: using Booloader Mac address\n");
        }
    }
	   
    address[0] = fnet_params->fnet_params.mac[0];
    address[1] = fnet_params->fnet_params.mac[1];
    address[2] = fnet_params->fnet_params.mac[2];
    address[3] = fnet_params->fnet_params.mac[3];
    address[4] = fnet_params->fnet_params.mac[4];
    address[5] = fnet_params->fnet_params.mac[5];
}
#endif

