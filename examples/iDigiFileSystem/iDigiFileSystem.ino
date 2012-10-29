
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <iDigi.h>

extern "C" {
 #include <stdlib.h>
 #include <stdio.h> 
}

#define ETHERNET_DHCP   1
#define SD_CHIP_SELECT  4
#define SD_SS_PIN       53

#define IDIGI_VENDOR_ID    0x03000009
#define IDIGI_SERVER       "my.idigi.com"
#define IDIGI_DEVICE_NAME  "Arduino Mega"

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(192, 168, 1, 3);
IPAddress gw(192, 168, 1, 240);
IPAddress nameserver(8, 8, 8, 8);
IPAddress subnet(255, 255, 255, 0);
#endif /* ETHERNET_DHCP */

bool idigi_connected = false;

void setup() {
  String deviceId;
  
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  Ethernet.begin(mac, ip, nameserver, gw, subnet);
  Serial.println("Starting iDigi...");
  iDigi.setup(mac, ip, IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#else
  Ethernet.begin(mac);
  Serial.println("Starting iDigi...");
  Serial.print("LocalIP: ");
  Serial.println(Ethernet.localIP());
  iDigi.setup(mac, Ethernet.localIP(), IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#endif /* ETHERNET_DHCP */
  Serial.println("Ethernet started!");
  delay(500);

  iDigi.getDeviceIdString(&deviceId);
  Serial.print("iDigi Device ID: ");
  Serial.println(deviceId);
  
  iDigi.fileSystem.enableSharing(SD_CHIP_SELECT, SD_SS_PIN);
}

void loop() {
  unsigned long int now = millis();
  
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

