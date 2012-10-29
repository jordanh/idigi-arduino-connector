
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

#define SAMPLE_PERIOD 15000
#define UPLOAD_PERIOD 60000

#define T_CALIBRATION -3.91

byte mac[] = { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
// iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(10, 101, 1, 142);
IPAddress gw(10, 101, 1, 1);
IPAddress nameserver(8, 8, 8, 8);
IPAddress subnet(255, 255, 255, 0);
#endif /* ETHERNET_DHCP */

bool idigi_connected = false;

iDigiDiaDataset dataset(8, "arduino");
unsigned long int counter = 0;

unsigned long int next_sample_time = 0;
unsigned long int next_upload_time = 0;

void setup() {
  String deviceId;
  
  pinMode(A8, INPUT);
  
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
  
  next_sample_time = millis() + SAMPLE_PERIOD;
  next_upload_time = millis() + UPLOAD_PERIOD;
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long w = (long)f;
  itoa(w, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long decimal = abs((long)((f - w) * p[precision]));
  itoa(decimal, a, 10);
  return ret;
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
    double temperature = fmap(analogRead(8), 0.0, 1023.0, 0.0, 5.0);
    temperature = ((temperature - 0.5)/0.01) + T_CALIBRATION;
    
    ftoa(value, temperature, 2);
    Serial.print("Adding ");
    Serial.print(temperature);
    Serial.println(" to dataset.");
    dataset.add("temperature", value, "C");
    counter++;
    next_sample_time = millis() + SAMPLE_PERIOD;
  }
  
  if (idigi_connected && (now  >= next_upload_time) && dataset.size() > 0)
  {
    Serial.print("putDiaDataSet: about to upload ");
    Serial.print(dataset.size());
    Serial.println(" samples.");
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
    next_upload_time = millis() + UPLOAD_PERIOD;
  }
 
  delay(125);
  iDigi.step();
}

