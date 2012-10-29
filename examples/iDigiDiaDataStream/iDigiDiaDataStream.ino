/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <iDigi.h>

extern "C" {
 #include <stdlib.h>
 #include <stdio.h> 
}

// ===========================================================================
// iDigiDiaDataStream.ino: send data samples to iDigi using Dia sample
//                         format, where they can be graphed, nalyzed or
//                         pushed to another system using HTTP or TCP
//                         
// Data uploaded to iDigi can be visualized under Data Services->Data Streams
// or, if you use the /ws/Monitor API you can have it pushed to any
// destination (or destinations) you like by configuring the monitor to
// watch the DiaChannelDataFull topic.
// ===========================================================================

// -------------------------------------------
// ITEMS YOU SHOULD CONFIGURE FOR YOUR ARDUINO
// -------------------------------------------
#define ETHERNET_DHCP 1                          // Set to 1 if you want to use DHCP   
#define IDIGI_SERVER       "my.idigi.com"        // iDigi server hostname to use
#define IDIGI_DEVICE_NAME  "Arduino Mega"        // How your device will be labelled on iDigi
#define IDIGI_VENDOR_ID    0x03000009            // If you don't know what this is, leave it alone :)
byte mac[] =                                     // Set this to the MAC address of your Ethernet shield
    { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };      // iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(192, 168, 1, 42);                   // If not using DHCP, set this IP address
IPAddress gw(192, 168, 1, 1);                    // Set to your router's address
IPAddress nameserver(8, 8, 8, 8);                // Set to your nameserver
IPAddress subnet(255, 255, 255, 0);              // Set your subnet mask
#endif /* ETHERNET_DHCP */

#define SAMPLE_PERIOD 5000                       // How often to record a sample locally
#define UPLOAD_PERIOD 20000                      // How often to upload to iDigi
/// -------------------------------------------
///         END OF CONFIGURATION ITEMS
/// -------------------------------------------


bool idigi_connected = false;
iDigiDiaDataset dataset(4, "arduino");    // iDigiDiaDataset objects store our samples
unsigned long int counter = 0;
long int next_sample_time = 0;
long int next_upload_time = 0;

void setup() {
  String deviceId;
  
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  // Static IP Configuration
  Ethernet.begin(mac, ip, nameserver, gw, subnet);
  Serial.println("Starting iDigi...");
  iDigi.setup(mac, ip, IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#else
  // DHCP Configuration
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
  
  // Calculate times to 
  next_sample_time = ((long) millis()) + SAMPLE_PERIOD;
  next_upload_time = ((long) millis()) + UPLOAD_PERIOD;
}

void loop() {
  long int now = (long) millis();
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

  if (idigi_connected && (now - next_sample_time >= 0) && dataset.spaceAvailable())
  {
    snprintf(value, sizeof(value), "%lu", counter);
    Serial.print("Adding ");
    Serial.print(value);
    Serial.println(" to dataset.");
    dataset.add("counter", value, "count");
    counter++;
    next_sample_time = ((long) millis() + SAMPLE_PERIOD);
  }
  
  if (idigi_connected && (now - next_upload_time >= 0) && dataset.size() > 0)
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
    next_upload_time = ((long) millis()) + UPLOAD_PERIOD;
  }
 
  delay(125);
  iDigi.step();
}

