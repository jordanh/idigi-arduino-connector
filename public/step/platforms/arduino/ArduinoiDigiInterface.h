
#ifndef __ARDUINO_IDIGI_INTERFACE_H__
#define __ARDUINO_IDIGI_INTERFACE_H__

#include <Ethernet.h>

#include "idigi_api.h"
#include "platform.h"


/* C++ Interface */

class ArduinoiDigiInterfaceClass {
private:
  EthernetClient client;
public:
  /* network functions */
  int network_connect(char const * const host_name, idigi_network_handle_t **network_handle);
  size_t network_send(idigi_network_handle_t *handle, char *buffer, size_t length);
  size_t network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length);
  int network_connected(idigi_network_handle_t *handle);
  void network_close(idigi_network_handle_t *handle);
};

extern ArduinoiDigiInterfaceClass ArduinoiDigiInterface;

/* C Interface */

extern "C" {
  /* network functions */
  int ar_network_connect(char const * const host_name, idigi_network_handle_t **network_handle);
  size_t ar_network_send(idigi_network_handle_t *handle, char *buffer, size_t length);
  size_t ar_network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length);
  int ar_network_connected(idigi_network_handle_t *handle);
  void ar_network_close(idigi_network_handle_t *handle);
}

#endif /* __ARDUINO_IDIGI_INTERFACE_H__ */

