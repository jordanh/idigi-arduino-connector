/****************************************************************************
* 
*   This file contains MQX only stationery code.
*
****************************************************************************/
#include "main.h"
#include "platform.h"
#include "os_support.h"
#include "idigi_config.h"

#if !BSPCFG_ENABLE_FLASHX && (defined IDIGI_FIRMWARE_SERVICE)
#error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{ 
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK, Main_task, 2048, 9, "main", MQX_AUTO_START_TASK},
   {IDIGI_CONNECTOR_TASK, idigi_connector_thread, 4096, 10, "iDigi_connector", 0},
#if (defined IDIGI_FIRMWARE_SERVICE)
    {IDIGI_FLASH_TASK, idigi_flash_task, 4096, 10, "iDigi_flash", 0},
#endif
   {0,           0,           0,     0,   0,      0,                 }
};

static uint_32 network_start(void)
{
    _enet_address * mac_addr = NULL;
    IPCFG_IP_ADDRESS_DATA ip_data;
    uint_32 result = RTCS_create();

    if (result != RTCS_OK) 
    {
        APP_DEBUG("RTCS failed to initialize, error = %X", result);
        goto error;
    }

    {
        size_t size;

        if (app_get_mac_addr((uint8_t const ** const)&mac_addr, &size) != idigi_callback_continue)
        {
            APP_DEBUG("Failed to get device MAC address");
            goto error;
        }

        APP_DEBUG("MAC Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\r\n",
                  (*mac_addr)[0], (*mac_addr)[1], (*mac_addr)[2], (*mac_addr)[3], (*mac_addr)[4], (*mac_addr)[5]);
    }

    result = ipcfg_init_device (ENET_DEVICE, *mac_addr);
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
    uint_32 const result = network_start();

    if (result == RTCS_OK)
    {
        int const status = application_start();

        if (status != 0)
        {
            APP_DEBUG("application_start failed %d\n", status);
        }
    }

error:
    _task_block();
}

/* EOF */
