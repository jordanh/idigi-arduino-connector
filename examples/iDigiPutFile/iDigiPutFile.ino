
#include <SPI.h>
#include <Ethernet.h>
#include <iDigi.h>

extern "C" {
 #include <stdlib.h>
 #include <stdio.h> 
}

#define ETHERNET_DHCP 0

#define IDIGI_VENDOR_ID    0x03000009
#define IDIGI_SERVER       "my.idigi.com"
#define IDIGI_DEVICE_NAME  "Arduino Mega"

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(10, 101, 1, 142);
IPAddress gw(10, 101, 1, 1);
IPAddress nameserver(8, 8, 8, 8);
IPAddress subnet(255, 255, 255, 0);
#endif /* ETHERNET_DHCP */

bool idigi_connected = false;
unsigned long int next_upload_time = 0;

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
  iDigi.setup(mac, Ethernet.localIP(), IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#endif /* ETHERNET_DHCP */
  Serial.println("Ethernet started!");
  delay(500);

  iDigi.getDeviceIdString(&deviceId);
  Serial.print("iDigi Device ID: ");
  Serial.println(deviceId);
  
  next_upload_time = millis() + 10000;
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
  
  if (idigi_connected && (millis() >= next_upload_time))
  {
    char file_data[128] = { 0 };
    size_t len = snprintf(file_data, sizeof(file_data), "Arduino up for %lu seconds.\r\n",
                                 millis() / 1000);
    size_t uploaded = iDigi.dataService.putFile("uptime.txt", "text/plain",
                                                file_data, len, IDIGI_DATA_PUT_APPEND);
    
    if (uploaded > 0)
    {
      Serial.print(uploaded);
      Serial.println(" bytes uploaded in uptime.txt to iDigi.");
    } else
    {
      Serial.print("Error uploading uptime.txt: ");
      Serial.println(-uploaded);
    }
    next_upload_time = millis() + 10000;
  }
 
  delay(125);
  iDigi.step();
}

