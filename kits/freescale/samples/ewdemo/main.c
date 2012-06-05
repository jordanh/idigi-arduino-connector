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
#include "main.h"
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "idigi_connector.h"

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if !BSPCFG_ENABLE_FLASHX && (defined(IDIGI_FIRMWARE_SERVICE) || (BOOTLOADER_ENABLED == 1))
#error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK, Main_task, 2048, 9, "main", MQX_AUTO_START_TASK},
   {IDIGI_CONNECTOR_TASK, idigi_connector_thread, 4096, 9, "iDigi_connector", 0},
#if (defined IDIGI_FIRMWARE_SERVICE)
   {IDIGI_FLASH_TASK,     idigi_flash_task,     2048,   8, "idigi_flash",     0,},
#endif
   {ADC_TASK,             ADC_Task,              400,  10, "ADC",             0,},
   {ACCEL_TASK,           Accel_Task,            400,  10, "Accelerometer",   0,},
   {IDIGI_LED_TASK,       idigi_led_task,       1024,  10, "idigi_led",       0,},
   {IDIGI_TOUCH_TASK,     idigi_touch_pad_task, 1024,  10, "idigi_touch",     0,},
   {IDIGI_BUTTON_TASK,    idigi_button_task,    2048,  10, "idigi_button",    0},
   {IDIGI_UTILITY_1,      idigi_utility_task1,   400,  10, "idigi_utility1",  0,},
   {IDIGI_UTILITY_2,      idigi_utility_task2,   400,  10, "idigi_utility2",  0 },
   {IDIGI_APP_TASK,       idigi_app_run_task,   2000,  10,  "idigi_app_run",   0,},
   {IDIGI_GPIO_TASK,      idigi_gpio_pulse_task,1500,  10, "idigi_gpio",      0 },
   {IDIGI_CPU_USAGE_TASK, idigi_cpu_usage,       400,  10, "idigi_cpu",       0 },
   {0,                    0,                     0,    0,   0,      0,         }
};

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
	APP_DEBUG("Got an event from iDigi Connector %d[%s]\n", status, status_message);
}

static uint_32 start_network(void)
{
	IPCFG_IP_ADDRESS_DATA ip_data;
    _enet_address mac_addr = IDIGI_MAC_ADDRESS;
    uint_32 result = RTCS_create();
	
    if (result != RTCS_OK) 
    {
		APP_DEBUG("RTCS failed to initialize, error = %X", result);
		goto error;
	}

#if (defined IDIGI_FIRMWARE_SERVICE) || (BOOTLOADER_ENABLED == 1)
    Flash_NVRAM_get_mac_address(mac_addr);
#endif

	result = ipcfg_init_device (ENET_DEVICE, mac_addr);
	if (result != RTCS_OK) 
	{
		APP_DEBUG("Failed to initialize Ethernet device, error = %X", result);
		goto error;
	}

	APP_DEBUG("Waiting for Ethernet cable plug in ... ");
	while(!ipcfg_get_link_active(ENET_DEVICE)) {};
	APP_DEBUG("Cable connected\n");

#if (defined IDIGI_USE_STATIC_IP)
    ip_data.ip = IDIGI_DEVICE_IPADDR;
    ip_data.mask = IDIGI_DEVICE_IPMASK;
    ip_data.gateway = IDIGI_DEVICE_GATEWAY;

    ipcfg_add_dns_ip(ENET_DEVICE, IDIGI_DNS_SERVER_IPADDR);
    APP_DEBUG("Setting static IP address ... ");
    result = ipcfg_bind_staticip (ENET_DEVICE, &ip_data);
#else
	APP_DEBUG("Contacting DHCP server ... ");
	result = ipcfg_bind_dhcp_wait(ENET_DEVICE, FALSE, &ip_data);
#endif

    if (result != IPCFG_ERROR_OK) 
    {
    	APP_DEBUG("\nRTCS failed to bind interface with IPv4, error = %X", result);
    	goto error;
    }
    else
    {
	    APP_DEBUG("Done\n");	   
    }

    ipcfg_get_ip(ENET_DEVICE, &ip_data);
    APP_DEBUG("\nIP Address      : %d.%d.%d.%d\n",IPBYTES(ip_data.ip));
    APP_DEBUG("\nSubnet Address  : %d.%d.%d.%d\n",IPBYTES(ip_data.mask));
    APP_DEBUG("\nGateway Address : %d.%d.%d.%d\n",IPBYTES(ip_data.gateway));
    APP_DEBUG("\nDNS Address     : %d.%d.%d.%d\n",IPBYTES(ipcfg_get_dns_ip(ENET_DEVICE,0)));
    result = RTCS_OK;

error:
    return result;
}

/*TASK*-----------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*    This starts iDigi Connector
*
*END*-----------------------------------------------------*/

void Main_task(uint_32 initial_data)
{	  
	uint_32 const result = start_network();
	
	if (result == RTCS_OK)
	{
		idigi_connector_error_t const status = idigi_connector_start(idigi_status);
		
		if (status != idigi_connector_success)
		{
			APP_DEBUG("idigi_connector_start failed %d\n", status);
		}
	}
	
	InitializeK60TowerDemo();
   
error:
	_task_block();
}

/* EOF */
