
#include "ArduinoiDigiInterface.h"

extern "C" {
#include "idigi_api.h"
#include <stdlib.h>
}

#include <SPI.h>
#include <Ethernet.h>

/* C++ */
ArduinoiDigiInterfaceClass::ArduinoiDigiInterfaceClass()
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
}

/* important interface functions */
void ArduinoiDigiInterfaceClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId)
{
  setMac(mac);
  setIp(ip);
  setVendorId(vendorId);
  
  idigi_handle = idigi_init((idigi_callback_t) appCallback);
  AR_DEBUG_PRINTF("idigi_init complete.\r\n");  
}

void ArduinoiDigiInterfaceClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
                                       char *serverHost)
{
  setServerHost(serverHost);
  setup(mac, ip, vendorId);
}

void ArduinoiDigiInterfaceClass::setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
                                       char *serverHost, char *deviceType)
{
  setDeviceType(deviceType);
  setup(mac, ip, vendorId, serverHost);
}

idigi_status_t ArduinoiDigiInterfaceClass::step()
{
  return idigi_step(idigi_handle);
}

/* iDigi connector application callback */
idigi_callback_status_t ArduinoiDigiInterfaceClass::appCallback(
                            idigi_class_t const class_id,
                            idigi_request_t const request_id, void * const request_data,
                            size_t const request_length, void * response_data,
                            size_t * const response_length)
{
  idigi_callback_status_t   status = idigi_callback_continue;
  
  switch (class_id)
  {
    case idigi_class_config:
      status = app_config_handler(request_id.config_request, request_data, request_length, response_data, response_length);
      break;
    case idigi_class_operating_system:
      status = app_os_handler(request_id.os_request, request_data, request_length, response_data, response_length);
      break;
    case idigi_class_network:
      status = app_network_handler(request_id.network_request, request_data, request_length, response_data, response_length);
      break;
    default:
      /* not supported */
      break;
  }
  return status;
}

/* network functions */

int ArduinoiDigiInterfaceClass::network_connect(char const * const host_name, idigi_network_handle_t **network_handle)
{
  int rc = client.connect(host_name, IDIGI_PORT);
  AR_DEBUG_PRINTF("network_connect: %s\r\n", host_name);
  *network_handle = (idigi_network_handle_t *) &client;
  return rc;
}

size_t ArduinoiDigiInterfaceClass::network_send(idigi_network_handle_t *handle, char *buffer, size_t length)
{
  EthernetClient *client = (EthernetClient *) handle;
  AR_DEBUG_PRINTF("network_send: len = %d\r\n", length);
  size_t sent = client->write((const uint8_t *) buffer, length);
  return sent;
}

size_t ArduinoiDigiInterfaceClass::network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length)
{
  EthernetClient *client = (EthernetClient *) handle;
  unsigned int time_stop = millis() + timeout_sec * 1000;
  AR_DEBUG_PRINTF("network_recv: timeout_sec = %d\r\n");
  while (time_stop > millis()) {
    if (!client->available())
      delay(100); /* wait for data */
  }
  
  if (!client->available())
    return 0;
  
  return client->read((uint8_t *) buffer, length);
}

int ArduinoiDigiInterfaceClass::network_connected(idigi_network_handle_t *handle)
{
  AR_DEBUG_PRINTF("network_connected: %d\r\n", ((EthernetClient *) handle)->connected());
  return ((EthernetClient *) handle)->connected();
}

void ArduinoiDigiInterfaceClass::network_close(idigi_network_handle_t *handle)
{
  AR_DEBUG_PRINTF("network_close\r\n");
  ((EthernetClient *) handle)->stop();
}

/* config functions */
void ArduinoiDigiInterfaceClass::setMac(uint8_t *mac)
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

void ArduinoiDigiInterfaceClass::setIp(IPAddress ip)
{
//  AR_DEBUG_PRINTF("setIp()\r\n");
  _ip[0] = ip[0];
  _ip[1] = ip[1];
  _ip[2] = ip[2];
  _ip[3] = ip[3];
}

void ArduinoiDigiInterfaceClass::setVendorId(uint32_t vendorId)
{
//  AR_DEBUG_PRINTF("setVendorId()\r\n");
  _vendorId[3] = (vendorId & 0x000000ff);   
  _vendorId[2] = (vendorId & 0x0000ff00) >> 8;
  _vendorId[1] = (vendorId & 0x00ff0000) >> 16;
  _vendorId[0] = (vendorId & 0xff000000) >> 24;
}

void ArduinoiDigiInterfaceClass::setDeviceType(const char *deviceType)
{
//  AR_DEBUG_PRINTF("deviceType(): %s\r\n", deviceType);
  strlcpy(_deviceType, deviceType, sizeof(_deviceType));
}

void ArduinoiDigiInterfaceClass::setServerHost(const char *serverHost)
{
  AR_DEBUG_PRINTF("setServerHost(): %s\r\n", serverHost);
  strlcpy(_serverHost, serverHost, sizeof(_serverHost));
}

void ArduinoiDigiInterfaceClass::setLinkSpeed(uint32_t linkSpeed)
{
//  AR_DEBUG_PRINTF("setLinkSpeed(): %d\r\n", linkSpeed);
  _linkSpeed = linkSpeed;
}

void ArduinoiDigiInterfaceClass::setPhoneNumber(const char *phoneNumber)
{
//  AR_DEBUG_PRINTF("setPhoneNumber(): %d\r\n", phoneNumber);
  strlcpy(_phoneNumber, phoneNumber, sizeof(_phoneNumber));
}

void ArduinoiDigiInterfaceClass::getMac(uint8_t **mac, size_t *length)
{
//  AR_DEBUG_PRINTF("getMac()\r\n");
  *mac = _mac;
  *length = sizeof(_mac);
}

void ArduinoiDigiInterfaceClass::getIp(uint8_t **ip, size_t *length)
{
//  AR_DEBUG_PRINTF("getIp()\r\n");
  *ip = _ip;
  *length = sizeof(_ip);
}

void ArduinoiDigiInterfaceClass::getDeviceId(uint8_t **deviceId, size_t *length)
{
//  AR_DEBUG_PRINTF("getDeviceId()\r\n");
  *deviceId = _deviceId;
  *length = sizeof(_deviceId);
}

void ArduinoiDigiInterfaceClass::getDeviceIdString(String *dest)
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

void ArduinoiDigiInterfaceClass::getVendorId(uint8_t **vendorId, size_t *length)
{
//  AR_DEBUG_PRINTF("getVendorId()\r\n");
  *vendorId = _vendorId;
  *length = sizeof(_vendorId);
}

char *ArduinoiDigiInterfaceClass::getDeviceType()
{
//  AR_DEBUG_PRINTF("getDeviceType()\r\n");
  return _deviceType;
}

char *ArduinoiDigiInterfaceClass::getServerHost()
{
  AR_DEBUG_PRINTF("getServerHost(): %s\r\n", _serverHost);
  return _serverHost;
}

idigi_connection_type_t ArduinoiDigiInterfaceClass::getConnectionType()
{
//  AR_DEBUG_PRINTF("getConnectionType()\r\n");
  return _connectionType;
}

uint32_t ArduinoiDigiInterfaceClass::getLinkSpeed()
{
//  AR_DEBUG_PRINTF("getLinkSpeed()\r\n");
  return _linkSpeed;
}

char *ArduinoiDigiInterfaceClass::getPhoneNumber()
{
//  AR_DEBUG_PRINTF("getPhoneNumber()\r\n");
  return _phoneNumber;
}

ArduinoiDigiInterfaceClass iDigi;

extern "C" {
/* C Interface */
static char printf_buf[IDIGI_PRINTF_LENGTH];

/* debug functions */
int ar_vprintf(const char *format, va_list ap)
{
  int ret = 0;
  
  ret = vsnprintf(printf_buf, sizeof(printf_buf), format, ap);
  Serial.print(printf_buf);
  
  return ret;
}

int ar_printf(const char *format, ...)
{
  int ret = 0;
  
  va_list argptr;
  va_start(argptr, format);  
  ret = vsnprintf(printf_buf, sizeof(printf_buf), format, argptr);
  va_end(argptr);
  
  Serial.print(printf_buf);
    
  return ret;
}

/* network functions */
int ar_network_connect(char const * const host_name, idigi_network_handle_t **network_handle)
{
  return iDigi.network_connect(host_name, network_handle);
}

size_t ar_network_send(idigi_network_handle_t *handle, char *buffer, size_t length)
{
  return iDigi.network_send(handle, buffer, length);
}

size_t ar_network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length)
{
  return iDigi.network_recv(handle, timeout_sec, buffer, length);
}

int ar_network_connected(idigi_network_handle_t *handle)
{
  return iDigi.network_connected(handle);
}

void ar_network_close(idigi_network_handle_t *handle)
{
  iDigi.network_close(handle);
}

/* configuration functions */
void ar_get_mac(uint8_t **mac, size_t *length)
{
  return iDigi.getMac(mac, length);
}

void ar_get_ip(uint8_t **ip, size_t *length)
{
  return iDigi.getIp(ip, length);
}

void ar_get_device_id(uint8_t **deviceId, size_t *length)
{
  return iDigi.getDeviceId(deviceId, length);
}

void ar_get_vendor_id(uint8_t **vendorId, size_t *length)
{
  return iDigi.getVendorId(vendorId, length);
}

char *ar_get_device_type()
{
  return iDigi.getDeviceType();
}

char *ar_get_server_host()
{
  return iDigi.getServerHost();
}

idigi_connection_type_t ar_get_connection_type()
{
  return iDigi.getConnectionType();
}

uint32_t ar_get_link_speed()
{
  return iDigi.getLinkSpeed();
}

char *ar_get_phone_number()
{
  return iDigi.getPhoneNumber();
}
} /* extern "C" */
