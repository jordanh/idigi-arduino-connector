
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

char file_data[128] = { 0 };
bool idigi_connected = false;
unsigned long int next_upload_time = 0;

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
    if (iDigi.dataService.putFileAsyncBusy())
    {
      Serial.println("iDigi putFileAsync() busy, delaying 10 seconds.");
    } else {
      unsigned int result = iDigi.dataService.putFileAsync("uptime.txt", "text/plain",
                              putFileUptimeData, IDIGI_DATA_PUT_APPEND);
      if (result == 0)
      {
        Serial.println("iDigi putFile request submitted.");
      } else {
        Serial.print("Error returned from putFileAsync(): ");
        Serial.println(result, DEC);
      }
    }
    next_upload_time = millis() + 10000;
  }
 
  delay(125);
  iDigi.step();
}

void putFileUptimeData(iDigiPutFileRequest *request)
{
    // Format the data to send to iDigi, set length in request:
    request->length = snprintf(file_data, sizeof(file_data), "Arduino up for %lu seconds.\r\n",
                                 millis() / 1000);
                                 
    // Tell iDigi where our formatted data is:
    request->buffer = file_data;
    
    // Tell iDigi there will be no more data to send, so don't bother calling us again:
    request->finished();
    
    Serial.println("About to upload uptime.txt...");
}
