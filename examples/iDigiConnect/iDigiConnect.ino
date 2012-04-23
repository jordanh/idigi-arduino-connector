
#include <SPI.h>
#include <Ethernet.h>
#include <iDigi.h>

#define ETHERNET_DHCP 0

#define IDIGI_VENDOR_ID    0x01000198

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(10, 40, 18, 133);
IPAddress gw(10, 40, 18, 1);
IPAddress dns(10, 40, 8, 28);
IPAddress subnet(255, 255, 255, 0);
#endif /* ETHERNET_DHCP */

bool idigi_connected = false;

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

  delay(125);
  iDigi.step(); 
  Serial.println("iDigi step.");
}

