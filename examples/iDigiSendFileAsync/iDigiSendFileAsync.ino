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
#include <Ethernet.h>
#include <SD.h>
#include <iDigi.h>

extern "C" {
 #include <stdlib.h>
 #include <stdio.h> 
}

// ===========================================================================
// iDigiPutFileAync.ino: periodically upload a file to iDigi bulk storage
//                       using asynchronous interface.
//
// This example is identical to the iDigiPutFile.ino example except that it
// uses the asynchronous file upload mechanism to allow you to upload a
// potentially large file to iDigi in chunks.  In between chunks, your
// application can continue to do other things.
// ===========================================================================

// -------------------------------------------
// ITEMS YOU SHOULD CONFIGURE FOR YOUR ARDUINO
// -------------------------------------------
#define ETHERNET_DHCP 1                          // Set to 1 if you want to use DHCP   
#define IDIGI_SERVER       "my.idigi.com"        // iDigi server hostname to use
#define IDIGI_DEVICE_NAME  "Arduino"             // How your device will be labelled on iDigi
#define IDIGI_VENDOR_ID    0                     // If you don't know what this is, leave it alone :)
byte mac[] =                                     // Set this to the MAC address of your Ethernet shield
    { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };      // iDigi Device ID will be 00000000-00000000-90A2DAFF-FF050057

#if(ETHERNET_DHCP == 0)
IPAddress ip(192, 168, 1, 42);                   // If not using DHCP, set this IP address
IPAddress gw(192, 168, 1, 1);                    // Set to your router's address
IPAddress nameserver(8, 8, 8, 8);                // Set to your nameserver
IPAddress subnet(255, 255, 255, 0);              // Set your subnet mask
#endif /* ETHERNET_DHCP */

#define UPLOAD_PERIOD   10000                    // How often to upload to iDigi
/// -------------------------------------------
///         END OF CONFIGURATION ITEMS
/// -------------------------------------------

char file_data[128] = { 0 };
bool idigi_connected = false;
long int next_upload_time = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  Ethernet.begin(mac, ip, nameserver, gw, subnet);
  Serial.println("Starting iDigi...");
  iDigi.begin(mac, ip, IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#else
  Ethernet.begin(mac);
  Serial.println("Starting iDigi...");
  Serial.print("Ethernet IP: ");
  Serial.println(Ethernet.localIP());    
  iDigi.begin(mac, Ethernet.localIP(), IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#endif /* ETHERNET_DHCP */
  Serial.println("Ethernet started!");
  delay(500);

  Serial.print("iDigi Device ID: ");
  Serial.println(iDigi.getId());
  
  // Set initial time to upload to iDigi:
  next_upload_time = ((long) millis()) + UPLOAD_PERIOD;
}

void loop() {
  long int now = (long) millis();

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
  
  if (idigi_connected && (now - next_upload_time >= 0)) // detect change in iDigi status
  {
    if (!iDigi.dataService.sendFileAsyncFinished())
    {
      // This is how you detect if an upload is already in progress:
      Serial.println("iDigi sendFileAsync() busy, will try later.");
    } else {
      // Create the asynchronous upload request; this will cause the function
      // putFileUptimeData to be called repeatedly until its finished:
      unsigned int result = iDigi.dataService.sendFileAsync("uptime.txt", "text/plain",
                              sendFileUptimeData, IDIGI_DATA_PUT_APPEND);
      if (result == 0)
      {
        Serial.println("iDigi sendFile request submitted.");
      } else {
        Serial.print("Error returned from sendFileAsync(): ");
        Serial.println(result, DEC);
      }
    }
    next_upload_time = ((long) millis()) + UPLOAD_PERIOD;
  }

  iDigi.update();
}

void sendFileUptimeData(iDigiSendFileRequest *request)
{
    // Format the data to send to iDigi, report string length in request:
    request->length = snprintf(file_data, sizeof(file_data), "Arduino up for %lu seconds.\r\n",
                                 millis() / 1000);
                                 
    // Tell iDigi where our formatted data is:
    request->buffer = file_data;
    
    // Tell iDigi there will be no more data to send, so don't bother calling us again:
    request->finished();
    
    Serial.println("About to upload uptime.txt...");
}
