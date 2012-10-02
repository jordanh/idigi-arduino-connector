/*
 * Copyright (c) 1996-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include "iDigiConnector.h"

extern "C" {
#include "idigi_api.h"
#include <stdlib.h>
}

#include <SPI.h>
#include <Ethernet.h>

/* C++ */
iDigiConnectorClass::iDigiConnectorClass()
{  
  memset(&_mac, 0, sizeof(_mac));
  memset(&_ip, 0, sizeof(_ip));
  memset(&_deviceId, 0, sizeof(_deviceId));
  memset(&_vendorId, 0, sizeof(_vendorId));
  // setServerHost((const char *) IDIGI_DEFAULT_SERVERHOST);
  // setDeviceType((const char *) IDIGI_DEFAULT_DEVICETYPE);
  _connectionType = idigi_lan_connection_type;
  _linkSpeed = IDIGI_DEFAULT_LINKSPEED;
  // setPhoneNumber((const char *) IDIGI_DEFAULT_PHONENUMBER);
  
  idigi_handle = NULL;
  connected = false;
}

/* important interface functions */
void iDigiConnectorClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId)
{
#if 0
  setMac(mac);
  setIp(ip);
  setVendorId(vendorId);
  
  idigi_handle = idigi_init((idigi_callback_t) &iDigiConnectorClass::appCallback);
  AR_DEBUG_PRINTF("idigi_init complete.\r\n"); 
#endif 
}

void iDigiConnectorClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
                                       char *serverHost)
{
#if 0
  setServerHost(serverHost);
  setup(mac, ip, vendorId);
#endif
}

void iDigiConnectorClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
                                       char *serverHost, char *deviceType)
{
#if 0
  setDeviceType(deviceType);
  setup(mac, ip, vendorId, serverHost);
#endif
}

bool iDigiConnectorClass::isConnected()
{
  return connected;
}

idigi_status_t iDigiConnectorClass::step()
{
  return idigi_step(idigi_handle);
}

/* iDigi Connector callback */
idigi_callback_status_t iDigiConnectorClass::appCallback(
                            idigi_class_t const class_id,
                            idigi_request_t const request_id, void * const request_data,
                            size_t const request_length, void * response_data,
                            size_t * const response_length)
{
  idigi_callback_status_t   status = idigi_callback_continue;
  //AR_DEBUG_PRINTF("iDigi.appCallback(): %d %d\n", class_id, request_id);
  switch (class_id)
  {
    case idigi_class_config:
      if (request_id.config_request == idigi_config_ip_addr)
      {
        // HACK: from testing we know this callback is called if we are connected:
        iDigi.connected = true;
      }      
      status = app_config_handler(request_id.config_request, request_data, request_length, response_data, response_length);
      break;
#if 0
    case idigi_class_operating_system:
      status = app_os_handler(request_id.os_request, request_data, request_length, response_data, response_length);
      break;
    case idigi_class_network:
      status = app_network_handler(request_id.network_request, request_data, request_length, response_data, response_length);
      break;
    case idigi_class_data_service:
      status = dataService.appReqHandler(request_id.data_service_request, request_data, request_length, response_data, response_length);
#endif
    default:
      /* not supported */
      break;
  }
  return status;
}

/* misc accessor functions */
idigi_handle_t iDigiConnectorClass::getHandle()
{
  return idigi_handle;
}

/* global and static member initialization */
iDigiConnectorClass iDigi;

extern "C" {
/* C Interface */
static char printf_buf[IDIGI_PRINTF_LENGTH];

/* debug functions */
int app_vprintf(const char *format, va_list ap)
{
  int ret = 0;
  
  ret = vsnprintf(printf_buf, sizeof(printf_buf), format, ap);
  Serial.print(printf_buf);
  
  return ret;
}

int app_printf(const char *format, ...)
{
  int ret = 0;
  
  va_list argptr;
  va_start(argptr, format);  
  ret = vsnprintf(printf_buf, sizeof(printf_buf), format, argptr);
  va_end(argptr);
  
  Serial.print(printf_buf);
    
  return ret;
}
} /* /extern "C" */