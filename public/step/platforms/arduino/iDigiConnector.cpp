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


iDigiConnectorClass::iDigiConnectorClass()
{  
  memset(&_mac, 0, sizeof(_mac));
  memset(&_ip, 0, sizeof(_ip));
  memset(&_deviceId, 0, sizeof(_deviceId));
  memset(&_vendorId, 0, sizeof(_vendorId));
  setServerHost((const char *) IDIGI_DEFAULT_SERVERHOST);
  setDeviceType((const char *) IDIGI_DEFAULT_DEVICETYPE);
  _connectionType = idigi_lan_connection_type;
  _linkSpeed = IDIGI_DEFAULT_LINKSPEED;
  setPhoneNumber((const char *) IDIGI_DEFAULT_PHONENUMBER);
  
  idigi_handle = NULL;
  connected = false;
}

/* important interface functions */
void iDigiConnectorClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId)
{
  setMac(mac);
  setIp(ip);
  setVendorId(vendorId);
  
  idigi_handle = idigi_init((idigi_callback_t) &iDigiConnectorClass::appCallbackWrapper);
  APP_DEBUG("idigi_init complete.\r\n"); 
}

void iDigiConnectorClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
                                       char *serverHost)
{
  setServerHost(serverHost);
  setup(mac, ip, vendorId);
}

void iDigiConnectorClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
                                       char *serverHost, char *deviceType)
{
  setDeviceType(deviceType);
  setup(mac, ip, vendorId, serverHost);
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
  //APP_DEBUG("iDigi.appCallback(): %d %d\n", class_id, request_id);
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
    case idigi_class_operating_system:
      status = app_os_handler(request_id.os_request, request_data, request_length, response_data, response_length);
      break;
    case idigi_class_network:
      status = app_network_handler(request_id.network_request, request_data, request_length, response_data, response_length);
      break;
    case idigi_class_data_service:
      status = dataService.appReqHandler(request_id.data_service_request, request_data, request_length, response_data, response_length);
    default:
      /* not supported */
      break;
  }
  return status;
}

idigi_callback_status_t iDigiConnectorClass::appCallbackWrapper(
                            idigi_class_t const class_id,
                            idigi_request_t const request_id, void * const request_data,
                            size_t const request_length, void * response_data,
                            size_t * const response_length)
{
  iDigiConnectorClass *mySelf = &iDigi;
  return mySelf->appCallback(class_id, request_id, request_data, request_length, response_data, response_length);
}

/* config functions */
void iDigiConnectorClass::setMac(uint8_t *mac)
{
//  AR_DEBUG_PRINTF("setMac()\r\n");
  memcpy(&_mac, mac, sizeof(_mac));
  _deviceId[8] = mac[0];
  _deviceId[9] = mac[1];
  _deviceId[10] = mac[2];
  _deviceId[11] = 0xFF;
  _deviceId[12] = 0xFF;
  _deviceId[13] = mac[3];
  _deviceId[14] = mac[4];
  _deviceId[15] = mac[5];  
}

void iDigiConnectorClass::setIp(IPAddress ip)
{
//  AR_DEBUG_PRINTF("setIp()\r\n");
  _ip[0] = ip[0];
  _ip[1] = ip[1];
  _ip[2] = ip[2];
  _ip[3] = ip[3];
}

void iDigiConnectorClass::setVendorId(uint32_t vendorId)
{
//  AR_DEBUG_PRINTF("setVendorId()\r\n");
  _vendorId[3] = (vendorId & 0x000000ff);   
  _vendorId[2] = (vendorId & 0x0000ff00) >> 8;
  _vendorId[1] = (vendorId & 0x00ff0000) >> 16;
  _vendorId[0] = (vendorId & 0xff000000) >> 24;
}

void iDigiConnectorClass::setDeviceType(const char *deviceType)
{
//  AR_DEBUG_PRINTF("deviceType(): %s\r\n", deviceType);
  strlcpy(_deviceType, deviceType, sizeof(_deviceType));
}

void iDigiConnectorClass::setServerHost(const char *serverHost)
{
//  AR_DEBUG_PRINTF("setServerHost(): %s\r\n", serverHost);
  strlcpy(_serverHost, serverHost, sizeof(_serverHost));
}

void iDigiConnectorClass::setLinkSpeed(uint32_t linkSpeed)
{
//  AR_DEBUG_PRINTF("setLinkSpeed(): %d\r\n", linkSpeed);
  _linkSpeed = linkSpeed;
}

void iDigiConnectorClass::setPhoneNumber(const char *phoneNumber)
{
//  AR_DEBUG_PRINTF("setPhoneNumber(): %d\r\n", phoneNumber);
  strlcpy(_phoneNumber, phoneNumber, sizeof(_phoneNumber));
}

void iDigiConnectorClass::getMac(uint8_t **mac, size_t *length)
{
//  AR_DEBUG_PRINTF("getMac()\r\n");
  *mac = _mac;
  *length = sizeof(_mac);
}

void iDigiConnectorClass::getIp(uint8_t **ip, size_t *length)
{
//  AR_DEBUG_PRINTF("getIp()\r\n");
  *ip = _ip;
  *length = sizeof(_ip);
}

void iDigiConnectorClass::getDeviceId(uint8_t **deviceId, size_t *length)
{
//  AR_DEBUG_PRINTF("getDeviceId()\r\n");
  *deviceId = _deviceId;
  *length = sizeof(_deviceId);
}

void iDigiConnectorClass::getDeviceIdString(String *dest)
{
//  AR_DEBUG_PRINTF("getDeviceIdString()\r\n");
  for (uint8_t i = 0; i < sizeof(_deviceId); i++)
  {
    char formatBuf[3];
    sprintf(formatBuf, "%02X", _deviceId[i]);
    *dest += formatBuf;
    if (i && (i+1) % 4 == 0 && (i+1) < (uint8_t) sizeof(_deviceId))
      *dest += String("-");
  }
}

void iDigiConnectorClass::getVendorId(uint8_t **vendorId, size_t *length)
{
//  AR_DEBUG_PRINTF("getVendorId()\r\n");
  *vendorId = _vendorId;
  *length = sizeof(_vendorId);
}

char *iDigiConnectorClass::getDeviceType()
{
//  AR_DEBUG_PRINTF("getDeviceType()\r\n");
  return _deviceType;
}

char *iDigiConnectorClass::getServerHost()
{
//  AR_DEBUG_PRINTF("getServerHost(): %s\r\n", _serverHost);
  return _serverHost;
}

idigi_connection_type_t iDigiConnectorClass::getConnectionType()
{
//  AR_DEBUG_PRINTF("getConnectionType()\r\n");
  return _connectionType;
}

uint32_t iDigiConnectorClass::getLinkSpeed()
{
//  AR_DEBUG_PRINTF("getLinkSpeed()\r\n");
  return _linkSpeed;
}

char *iDigiConnectorClass::getPhoneNumber()
{
//  AR_DEBUG_PRINTF("getPhoneNumber()\r\n");
  return _phoneNumber;
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