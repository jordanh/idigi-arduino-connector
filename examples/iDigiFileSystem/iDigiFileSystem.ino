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
// iDigiDiaDataStreamT.ino: send temperature data samples to iDigi using Dia
//                          sample format
//                         
// To use this sample you must have a simple temperature sensor such as the
// Microchip MCP9700 (or similar).  Temperature sensors such as the MCP9700
// are called thermistors.  They usually have three pins.  Two pins are
// connected between power and ground and a third acts the sensor output
// reading.  In this sample, the output is connected to an analog input
// pin of the Arduino.
// 
// This sample collects the analog input values, transforms them to a
// temperature and then periodically uploads these sample to iDigi.  The
// data may be viewed by logging into your iDigi account and navigating
// to Data Services->Data Streams.
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

#define SD_CHIP_SELECT  4                        // Your Arduino's SD CS pin
#define SD_SS_PIN       53                       // Your Arduino's SPI SS pin
/// -------------------------------------------
///         END OF CONFIGURATION ITEMS
/// -------------------------------------------


bool idigi_connected = false;

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
  
  // This function enables your SD FAT filesystem for sharing with iDigi:
  iDigi.fileSystem.enableSharing(SD_CHIP_SELECT, SD_SS_PIN);
}

void loop() {
  if (idigi_connected ^ iDigi.isConnected())  // detect change in iDigi status
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

