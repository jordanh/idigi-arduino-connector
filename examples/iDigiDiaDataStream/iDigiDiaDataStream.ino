
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
IPAddress ip(10, 8, 113, 101);
IPAddress gw(10, 8, 1, 1);
IPAddress nameserver(8, 8, 8, 8);
IPAddress subnet(255, 255, 0, 0);
#endif /* ETHERNET_DHCP */

bool idigi_connected = false;

iDigiDiaDataset dataset(4, "arduino");
unsigned long int counter = 0;

unsigned long int next_sample_time = 0;
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
  
  next_sample_time = millis() + 500;
  next_upload_time = millis() + 2000;
}

void loop() {
  unsigned long int now = millis();
  char value[32] = { 0 };
  
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
  
  if (idigi_connected && (now >= next_sample_time) && dataset.spaceAvailable())
  {
    snprintf(value, sizeof(value), "%lu", counter);
    Serial.print("Adding ");
    Serial.print(value);
    Serial.println(" to dataset.");
    dataset.add("counter", value, "count");
    counter++;
    next_sample_time = millis() + 1000;
  }
  
  if (idigi_connected && (now  >= next_upload_time) && dataset.size() > 0)
  {
    size_t bytesUploaded = iDigi.dataService.putDiaDataset(&dataset);
    
    if (bytesUploaded < 0)
    {
      Serial.print("putDiaDataSet: error returned from putDiaDataSet(");
      Serial.print(bytesUploaded, DEC);
      Serial.println(")");
      
      return;
    } else {
      Serial.print("putDiaDataSet: uploaded ");
      Serial.print(bytesUploaded, DEC);
      Serial.println(" bytes");
    }

    dataset.clear();
    next_upload_time = millis() + 10000;
  }
 
  delay(125);
  iDigi.step();
}

