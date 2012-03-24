
#include "ArduinoiDigiInterface.h"
#include "idigi_api.h"

#include <SPI.h>
#include <Ethernet.h>

/* C++ */
int ArduinoiDigiInterfaceClass::network_connect(char const * const host_name, idigi_network_handle_t **network_handle)
{
  int rc = client.connect(host_name, IDIGI_PORT); 
  *network_handle = (idigi_network_handle_t *) &client;
  return rc;
}

size_t ArduinoiDigiInterfaceClass::network_send(idigi_network_handle_t *handle, char *buffer, size_t length)
{
  EthernetClient *client = (EthernetClient *) handle;
  size_t sent = client->write((const uint8_t *) buffer, length);
  return sent;
}

size_t ArduinoiDigiInterfaceClass::network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length)
{
  EthernetClient *client = (EthernetClient *) handle;
  int time_stop = millis() + timeout_sec * 1000;
  
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
  return ((EthernetClient *) handle)->connected();
}

void ArduinoiDigiInterfaceClass::network_close(idigi_network_handle_t *handle)
{
  ((EthernetClient *) handle)->stop();
}
                                           
ArduinoiDigiInterfaceClass ArduinoiDigiInterface;

/* C Interface */
int ar_network_connect(char const * const host_name, idigi_network_handle_t **network_handle)
{
  return ArduinoiDigiInterface.network_connect(host_name, network_handle);
}

size_t ar_network_send(idigi_network_handle_t *handle, char *buffer, size_t length)
{
  return ArduinoiDigiInterface.network_send(handle, buffer, length);
}

size_t ar_network_recv(idigi_network_handle_t *handle, int timeout_sec, char *buffer, size_t length)
{
  return ArduinoiDigiInterface.network_recv(handle, timeout_sec, buffer, length);
}

int ar_network_connected(idigi_network_handle_t *handle)
{
  return ArduinoiDigiInterface.network_connected(handle);
}

void ar_network_close(idigi_network_handle_t *handle)
{
  ArduinoiDigiInterface.network_close(handle);
}

