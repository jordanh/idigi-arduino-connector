
#include <SPI.h>
#include <Ethernet.h>
#include <iDigi.h>

extern "C" {
 #include <stdlib.h>
 #include <stdio.h> 
}

#define ETHERNET_DHCP 0
#define IDIGI_VENDOR_ID    0x01000198

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(10, 8, 113, 40);
IPAddress gw(10, 8, 1, 1);
IPAddress dns(10, 10, 8, 62);
IPAddress subnet(255, 255, 0, 0);
#endif /* ETHERNET_DHCP */

bool idigi_connected = false;
int next_upload_time = 0;

void setup() {
  String deviceId;
  
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  Ethernet.begin(mac, ip, dns, gw, subnet);
  Serial.println("Starting iDigi...");
  iDigi.setup(mac, ip, IDIGI_VENDOR_ID);
  Serial.println("iDigi started!");
#else
  Ethernet.begin(mac);
  Serial.println("Starting iDigi...");
  iDigi.setup(mac, Ethernet.localIP(), IDIGI_VENDOR_ID);
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
  
  if (idigi_connected && millis() >= next_upload_time)
  {
    char buf[128] = { 0 };
    size_t len = 0;
    len = snprintf(buf, sizeof(buf), "Arduino up for %d seconds.\r\n",
               millis() / 1000);
    Serial.println("About to upload...");
    iDigi.dataService.putFile("uptime.txt", "text/plain", buf, len);
    Serial.println("Uploaded uptime.txt.\r\n");
    next_upload_time = millis() + 5000;
  }
 
  delay(125);
  iDigi.step();
}
