
#ifndef __ARDUINO_IDIGI_INTERFACE_H__
#define __ARDUINO_IDIGI_INTERFACE_H__

#ifdef __cplusplus
#include <Ethernet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "idigi_api.h"
#include "platform.h"
#ifdef __cplusplus
}
#endif

#define IDIGI_DEFAULT_SERVERHOST    "developer.idigi.com"
#define IDIGI_DEFAULT_DEVICETYPE    "Arduino"
#define IDIGI_DEFAULT_LINKSPEED     10000000
#define IDIGI_DEFAULT_PHONENUMBER   ""

#define IDIGI_MAC_LENGTH            6
#define IDIGI_IP_LENGTH             4
#define IDIGI_DEVICEID_LENGTH       16
#define IDIGI_VENDORID_LENGTH       4
#define IDIGI_SERVERHOST_LENGTH     32
#define IDIGI_DEVICETYPE_LENGTH     32
#define IDIGI_PHONENUMBER_LENGTH    32

#define IDIGI_PRINTF_LENGTH         128

#define AR_IDIGI_DEBUG

#ifdef __cplusplus
/* C++ Interface */

class ArduinoiDigiInterfaceClass {
public:
  ArduinoiDigiInterfaceClass();
  
  /* important interface functions */
  void setup(uint8_t *mac, IPAddress ip, uint32_t vendorId);
  void setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
        char *serverHost);
  void setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
        char *serverHost, char *deviceType);
  idigi_status_t step();
  
  /* network functions */
  int network_connect(char const * const host_name, idigi_network_handle_t **network_handle);
  size_t network_send(idigi_network_handle_t *handle, char *buffer, size_t length);
  size_t network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length);
  int network_connected(idigi_network_handle_t *handle);
  void network_close(idigi_network_handle_t *handle);
  
  /* config functions */
  void setMac(uint8_t *mac);
  void setIp(IPAddress ip);
  void setVendorId(uint32_t vendorId);
  void setDeviceType(const char *deviceType);
  void setServerHost(const char *serverHost);
  void setLinkSpeed(uint32_t linkSpeed);
  void setPhoneNumber(const char *phoneNumber);
  
  void getMac(uint8_t **mac, size_t *length);
  void getIp(uint8_t **ip, size_t *length);
  void getDeviceId(uint8_t **deviceId, size_t *length);
  void getDeviceIdString(String *dest);
  void getVendorId(uint8_t **vendorId, size_t *length);
  char *getDeviceType();
  char *getServerHost();
  idigi_connection_type_t getConnectionType();
  uint32_t getLinkSpeed();
  char *getPhoneNumber();
  
  
private:
  /* iDigi state */
  idigi_handle_t idigi_handle;
  
  /* network state */
  EthernetClient client;
  
  /* configuration state */
  uint8_t _mac[IDIGI_MAC_LENGTH];
  uint8_t _ip[IDIGI_IP_LENGTH];
  uint8_t _deviceId[IDIGI_DEVICEID_LENGTH];
  uint8_t _vendorId[IDIGI_VENDORID_LENGTH];
  char _deviceType[IDIGI_DEVICETYPE_LENGTH];
  char _serverHost[IDIGI_SERVERHOST_LENGTH];
  idigi_connection_type_t _connectionType;
  uint32_t _linkSpeed;
  char _phoneNumber[IDIGI_PHONENUMBER_LENGTH];
  
  /* iDigi connector application callback */
  static idigi_callback_status_t appCallback(idigi_class_t const class_id, idigi_request_t const request_id, void * const request_data,
      size_t const request_length, void * response_data,
      size_t * const response_length);
};

extern ArduinoiDigiInterfaceClass iDigi;
#endif /* __cplusplus */

/* C Interface */
#ifdef __cplusplus
extern "C" {
#endif
  /* debug functions */
#include <stdio.h>
#include <stdarg.h>
  int ar_vprintf(const char *format, va_list ap);
  int ar_printf(const char *format, ...);
  
  /* network functions */
  int ar_network_connect(char const * const host_name, idigi_network_handle_t **network_handle);
  size_t ar_network_send(idigi_network_handle_t *handle, char *buffer, size_t length);
  size_t ar_network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length);
  int ar_network_connected(idigi_network_handle_t *handle);
  void ar_network_close(idigi_network_handle_t *handle);
  
  /* config functions */
  void ar_get_mac(uint8_t **mac, size_t *length);
  void ar_get_ip(uint8_t **ip, size_t *length);
  void ar_get_device_id(uint8_t **deviceId, size_t *length);
  void ar_get_vendor_id(uint8_t **vendorId, size_t *length);
  char *ar_get_device_type();
  char *ar_get_server_host();
  idigi_connection_type_t ar_get_connection_type();
  uint32_t ar_get_link_speed();
  char *ar_get_phone_number();
#ifdef __cplusplus
}
#endif

static void AR_DEBUG_PRINTF(char const * const format, ...)
{
#if defined(AR_IDIGI_DEBUG)
  va_list args;
  
  va_start(args, format);
  ar_printf(format, args);
  va_end(args);
#else
  (void) format;
#endif
}


#endif /* __ARDUINO_IDIGI_INTERFACE_H__ */

