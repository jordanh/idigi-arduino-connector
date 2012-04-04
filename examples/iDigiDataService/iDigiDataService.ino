
#include <SPI.h>
#include <Ethernet.h>
#include <iDigi.h>

extern "C" {
 #include <stdlib.h>
 #include <stdio.h> 
}

#define ETHERNET_DHCP 0

#define IDIGI_VENDOR_ID    0x01000198
#define IDIGI_SERVER       "developer.idigi.com"
#define IDIGI_DEVICE_NAME  "Arduino Mega"

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(10, 8, 113, 40);
IPAddress gw(10, 8, 1, 1);
IPAddress dns(10, 10, 8, 62);
IPAddress subnet(255, 255, 0, 0);
#endif /* ETHERNET_DHCP */

void iDigiRequestHandler(iDigiDataServiceRequest *request);

bool idigi_connected = false;
char response_buffer[128];

void setup() {
  String deviceId;
  
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  Ethernet.begin(mac, ip, dns, gw, subnet);
  Serial.println("Starting iDigi...");
  iDigi.setup(mac, ip, IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#else
  Ethernet.begin(mac);
  Serial.println("Starting iDigi...");
  iDigi.setup(mac, Ethernet.localIP(), IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#endif /* ETHERNET_DHCP */
  Serial.println("Ethernet started!");
  delay(500);

  iDigi.getDeviceIdString(&deviceId);
  Serial.print("iDigi Device ID: ");
  Serial.println(deviceId);
  
  iDigi.dataService.registerHandler(iDigiRequestHandler);
}

void loop() {
  if (idigi_connected ^ iDigi.isConnected())
  {
    idigi_connected = iDigi.isConnected();
    Serial.print("iDigi");
    if (idigi_connected)
    {
      Serial.println(" connected.");
    } else {
      Serial.println(" disconnected.");
    }
  }
  
  iDigi.step();
}

void iDigiRequestHandler(iDigiDataServiceRequest *request)
{
  Serial.print("iDigiRequestHandler(): data chunk for target \"");
  Serial.print(request->getTarget());
  Serial.println("\"");
  Serial.write((const uint8_t *)request->getBuffer(), request->getLength());
  Serial.println("\r\niDigiRequestHandler(): end of chunk");
  
  if (request->isFinished())
  {
    size_t response_len = 0;
    
    if (String(request->getTarget()) == String("millis"))
    {
      response_len = snprintf(response_buffer, sizeof(response_buffer), "%u", millis());
      iDigi.dataService.sendResponse(request, response_buffer, response_len-1);
    }
    else
    {
      response_len = snprintf(response_buffer, sizeof(response_buffer),
               "\r\n\t\tThat's perfectly lovely, thank you.\r\n"
               "\t\tDid you know I've been awake for %u milliseconds?\r\n", millis());
      iDigi.dataService.sendResponse(request, response_buffer, response_len-1);
    }
  }
}
