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
// iDigiDataService.ino: shows how to send data from iDigi to your Arduino,
//                       even if its behind a firewall.
//
// Data is sent from iDigi to your Arduino by creating an HTTP POST request
// to http://my.idigi.com/ws/sci with the following payload:
//
//   <sci_request version="1.0">
//      <data_service>
//        <targets>
//          <device id="00000000-00YOUR00-DEVICE00-ID000000"/>
//        </targets>
//        <requests>
//          <device_request target_name="myTarget"> 
//            my payload string
//          </device_request>
//       </requests>
//      </data_service>
//    </sci_request>
//
// Make certain to set your device ID to match the device ID generated
// from your Arduino's MAC address.  It will print it on the serial console
// when the program starts.
//
// The iDigi "Web Service Console" can test this for you and even generate
// code for your own project.
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
/// -------------------------------------------
///         END OF CONFIGURATION ITEMS
/// -------------------------------------------


void iDigiInterrupt(iDigiDataServiceRequest *request);
bool idigi_connected = false;
char response_buffer[128];

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up...");
 
  Serial.println("Starting Ethernet..."); 
#if(ETHERNET_DHCP == 0)
  // Static IP Configuration
  Ethernet.begin(mac, ip, nameserver, gw, subnet);
  Serial.println("Starting iDigi...");
  iDigi.begin(mac, ip, IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
  Serial.println("iDigi started!");
#else
  // DHCP Configuration
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

  // Register a function to be called when we get a data service request
  // from iDigi.  In this case, the function iDigiRequestHandler will be
  // called.  It is defined further down in the file.
  iDigi.dataService.attachInterrupt(iDigiInterrupt);
}

void loop() {
  if (idigi_connected ^ iDigi.isConnected())  // Detect change in iDigi status
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
  
  iDigi.update();  // Run iDigi tasks
}

void iDigiInterrupt(iDigiDataServiceRequest *request)
{
  // We got a chunk of data from iDigi!  Let's print it:
  Serial.print("iDigiRequestHandler(): data chunk for target \"");
  Serial.print(request->getTarget());
  Serial.println("\"");
  Serial.write((const uint8_t *)request->getBuffer(), request->getLength());
  Serial.println("\r\niDigiRequestHandler(): end of chunk");
  
  if (request->isFinished())
  {
    // This is the last chunk, now its our turn to respond if we like.
    size_t response_len = 0;
    
    if (String(request->getTarget()) == String("millis"))
    {
      // If the target string of the request is set to "millis"
      // Let's respond with a simple number of how many milliseconds we've been
      // running:
      response_len = snprintf(response_buffer, sizeof(response_buffer), "%lu", millis());
      // This is how we send our response, notice that the length is "response_len-1" as
      // we don't want to send the null-termination at the end of the string:
      iDigi.dataService.sendResponse(request, response_buffer, response_len-1);
    }
    else
    {
      // For all other target strings, let's respond with a polite message:
      response_len = snprintf(response_buffer, sizeof(response_buffer),
               "\r\n\t\tThat's perfectly lovely, thank you.\r\n"
               "\t\tDid you know I've been awake for %lu milliseconds?\r\n", millis());
      // Send alternative response, see note above on response_len-1:
      iDigi.dataService.sendResponse(request, response_buffer, response_len-1);
    }
  }
}
