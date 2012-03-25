#include <SPI.h>
#include <Ethernet.h>

#include <iDigi.h>

#define ETHERNET_DHCP 0

#define IDIGI_VENDOR_ID    0x01000198

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
byte ip[] = { 10, 8, 113, 26 };
byte gw[] = { 10, 8, 1, 1 };
byte mask[] = { 255, 255, 0, 0 };
byte server[] = { 10, 10, 32, 161 };
#endif /* ETHERNET_DHCP */

void setup() {  
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  Ethernet.begin(mac, ip, gw, mask);
  iDigi.setup(mac, ip, IDIGI_VENDOR_ID);
#else
  Ethernet.begin(mac);
#endif /* ETHERNET_DHCP */
  Serial.println("Ethernet started!");
}

void loop() {
  delay(100);
  iDigi.step(); 
}
