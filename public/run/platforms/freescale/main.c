/****************************************************************************
* 
*   This file contains MQX only stationery code.
*
****************************************************************************/
#include "main.h"
#include "idigi_api.h"
#include "platform.h"
#include "os_support.h"

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{ 
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK, Main_task, 2048, 9, "main", MQX_AUTO_START_TASK},
   {IDIGI_CONNECTOR_TASK, idigi_connector_thread, 4096, 10, "iDigi_connector", 0},
   {0,           0,           0,     0,   0,      0,                 }
};

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    APP_DEBUG("Got an event from iDigi Connector %d[%s]\n", status, status_message);
}

static uint_32 start_network(void)
{
    IPCFG_IP_ADDRESS_DATA ip_data;
    uint_32 result = RTCS_create();

    if (result != RTCS_OK) 
    {
        APP_DEBUG("RTCS failed to initialize, error = %X", result);
        goto error;
    }

    result = ipcfg_init_device (ENET_DEVICE, device_mac_addr);
    if (result != RTCS_OK)
    {
        APP_DEBUG("Failed to initialize Ethernet device, error = %X", result);
        goto error;
    }

    APP_DEBUG("Waiting for Ethernet cable plug in ... ");
    while(!ipcfg_get_link_active(ENET_DEVICE)) {};
    APP_DEBUG("Cable connected\n");

    APP_DEBUG("Contacting DHCP server ... ");
    result = ipcfg_bind_dhcp_wait(ENET_DEVICE, FALSE, &ip_data);
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

error:
    _task_block();
}

/* EOF */
