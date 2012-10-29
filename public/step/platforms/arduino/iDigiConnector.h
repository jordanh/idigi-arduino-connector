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

#ifndef __IDIGI_CONNECTOR_H__
#define __IDIGI_CONNECTOR_H__

#include <Ethernet.h>

extern "C" {
#include "idigi_api.h"
#include "idigi_config.h"
}

#include "iDigiDataService.h"
#include "iDigiFileSystem.h"

class iDigiConnectorClass {
  friend class iDigiDataService;

public:
  iDigiConnectorClass();
  
  /* important interface functions */
  void setup(uint8_t *mac, IPAddress ip, uint32_t vendorId);
  void setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
        char *serverHost);
  void setup(uint8_t *mac, IPAddress ip, uint32_t vendorId,
        char *serverHost, char *deviceType);
  bool isConnected();
  idigi_status_t step();
  
  /* iDigi Connector callback */
  idigi_callback_status_t appCallback(idigi_class_t const class_id,
                                      idigi_request_t const request_id,
                                      void * const request_data,
                                      size_t const request_length, void * response_data,
                                      size_t * const response_length);

  static idigi_callback_status_t appCallbackWrapper(idigi_class_t const class_id,
                                      idigi_request_t const request_id,
                                      void * const request_data,
                                      size_t const request_length, void * response_data,
                                      size_t * const response_length);
  

  /* cloud file interface */
  iDigiDataService dataService;
  iDigiFileSystem fileSystem;
  
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
  bool connected;

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

  /* private interface methods */
  idigi_handle_t getHandle();

  /* Define in iDigiConnectorConfic.cpp */
  idigi_callback_status_t app_get_ip_address(uint8_t const ** ip_address, size_t * const size);
  idigi_callback_status_t app_get_mac_addr(uint8_t const ** addr, size_t * const size);
  idigi_callback_status_t app_get_device_id(uint8_t const ** id, size_t * const size);
  idigi_callback_status_t app_get_vendor_id(uint8_t const ** id, size_t * const size);
  idigi_callback_status_t app_get_device_type(char const ** type, size_t * const size);
  idigi_callback_status_t app_get_server_url(char const ** url, size_t * const size);
  idigi_callback_status_t app_get_connection_type(idigi_connection_type_t const ** type);
  idigi_callback_status_t app_get_link_speed(uint32_t const ** speed, size_t * const size);
  idigi_callback_status_t app_get_phone_number(char const ** number, size_t * const size);
  idigi_callback_status_t app_get_tx_keepalive_interval(uint16_t const ** interval, size_t * const size);
  idigi_callback_status_t app_get_rx_keepalive_interval(uint16_t const ** interval, size_t * const size);
  idigi_callback_status_t app_get_wait_count(uint16_t const ** count, size_t * const size);
  idigi_callback_status_t app_get_firmware_support(idigi_service_supported_status_t * const isSupported);
  idigi_callback_status_t app_get_data_service_support(idigi_service_supported_status_t * const isSupported);
  idigi_callback_status_t app_get_file_system_support(idigi_service_supported_status_t * const isSupported);
  idigi_callback_status_t app_get_remote_configuration_support(idigi_service_supported_status_t * const isSupported);
  idigi_callback_status_t app_get_max_message_transactions(unsigned int * const transCount);
  idigi_callback_status_t app_get_device_id_method(idigi_device_id_method_t * const method);
  void app_config_error(idigi_error_status_t const * error_data);
  idigi_callback_status_t app_config_handler(idigi_config_request_t const request,
                                              void const * const request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * const response_length);

  /* Defined in iDigiConnectorOs.cpp */
  int app_os_malloc(size_t const size, void ** ptr);
  void app_os_free(void * const ptr);
  int app_os_get_system_time(unsigned long * const uptime);
  void app_os_sleep(unsigned int const timeout_in_seconds);
  idigi_callback_status_t app_os_handler(idigi_os_request_t const request,
                                        void * const request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length);

  /* Defined in iDigiNetwork.cpp */
  idigi_callback_status_t app_network_connect(char * const host_name, size_t const length, idigi_network_handle_t ** network_handle);
  bool app_network_connected(idigi_network_handle_t * network_handle);
  idigi_callback_status_t app_network_send(idigi_write_request_t * const write_data, size_t * sent_length);
  idigi_callback_status_t app_network_receive(idigi_read_request_t * read_data, size_t * read_length);
  idigi_callback_status_t app_network_close(idigi_network_handle_t * const fd);
  int app_server_disconnected(void);
  int app_server_reboot(void);
  idigi_callback_status_t app_network_handler(idigi_network_request_t const request,
                                            void * const request_data, size_t const request_length,
                                            void * response_data, size_t * const response_length);

};

extern iDigiConnectorClass iDigi;

extern "C"
{
  int app_vprintf(const char *format, va_list ap);
}

#endif /* __IDIGI_CONNECTOR_H__ */
