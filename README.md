iDigi Connector for Arduino
===========================


The iDigi Connector is a software library that allows you to connect your Arduino
to the [iDigi Device Cloud][iDigi].  Why would you want to connect your Arduino to iDigi?
With iDigi you can:

  * Control your Arduino sketch remotely from a web app or smartphone, even
    if your Arduino is behind a firewall

  * Upload data from your Arduino to iDigi using a simple, easy to use
    interface

[iDigi]: http://www.idigi.com


Requirements
------------

The iDigi Connector for Arduino requires an [Arduino Mega 2560][2560] or
[Arduino Mega ADK][ADK] with an [Arduino Ethernet Shield][shield].  The
iDigi Connector requires approximately 40k of flash and 2.5k of RAM.  Sorry,
The [Arduino Uno][Uno] is not yet supported due to memory constraints.

[2560]: http://arduino.cc/en/Main/ArduinoBoardMega2560
[ADK]: http://arduino.cc/en/Main/ArduinoBoardADK
[shield]: http://arduino.cc/en/Main/ArduinoEthernetShield
[Uno]: http://arduino.cc/en/Main/ArduinoBoardUno

You must have also installed the [Arduino Software][software], version 1.0
or later.

[software]: http://arduino.cc/en/Main/Software


Installation
------------

To install the iDigi Connector for Arduino, you'll need to install it as
a library for the Arduino IDE.  In order to install it, follow the
instructions from Arduino.cc about *Contributed Libraries* on
[this page][instructions]; or, follow the instructions below:

> If you're using one of these libraries, you need to install it
> first. To do so, download the library and unzip it. It should be in
> a folder of its own, and will typically contain at least two files,
> one with a .h suffix and one with a .cpp suffix. Open your Arduino
> sketchbook folder.  If there is already a folder there called
> libraries, place the library folder in there.  If not, create a
> folder called libraries in the sketchbook folder, and drop the
> library folder in there. Then re-start the Arduino programming
> environment, and you should see your new library in the Sketch >
> Import Library menu.

[instructions]: http://arduino.cc/it/Reference/Libraries

Usage
-----

Using the iDigi Connector for Arduino is easy! To use it, do the following:

  1. Add the following #include statements at the top of your sketch:

```c++
#include <SPI.h>
#include <Ethernet.h>
#include <iDigi.h>
```

  2. In your sketch's setup() function, call iDigi.setup() with your Ethernet
     sheild's MAC address, your current IP address, and your iDigi assigned
     "Vendor ID". If you don't have a Vendor ID, just set it to 0:

```c++
byte mac[] =               { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
IPAddress                  ip(10, 8, 113, 40);
IPAddress                  gw(10, 8, 1, 1);
IPAddress                  dns(10, 10, 8, 62);
IPAddress                  subnet(255, 255, 0, 0);
#define IDIGI_VENDOR_ID    0

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up...");
  Ethernet.begin(mac, ip, dns, gw, subnet);
  iDigi.setup(mac, ip, IDIGI_VENDOR_ID, IDIGI_SERVER, IDIGI_DEVICE_NAME);
}
```

  3. Use the other high-level iDigi Connector for Arduino functions such
     as dataService.putFile(), dataService.registerHandler() in order to
     interact with the world! At the end of each loop(), ensure that you
     call iDigi.step():

```c++
void loop()
{
  if (iDigi.isConnected())
  {
    // Upload a test message to the file, "hello.txt" on iDigi:
    size_t uploaded = iDigi.dataService.putFile("hello.txt", "text/plain",
  			"Hello, world!\r\n", 16, IDIGI_DATA_PUT_APPEND);
  }
  // interact with iDigi:
  iDigi.step();
  delay(1000);
}
```

High-Level Function Reference
-----------------------------

### Basic Functions

#### iDigi.setup()

Called to setup the iDigi Connector for Arduino library.

Paramters include the Ethernet Shield's MAC address and other information
used to describe your Arduino to the iDigi Device Cloud platform.

See example: iDigiConnect


#### iDigi.getDeviceIdString()

Retrieves the current iDigi Device ID which is used to uniquely identify
your Arduino.

This iDigi Device ID is used as an address to send your Arduino messages
from iDigi.

See example: iDigiConnect


#### iDigi.isConnected()

Returns true if your Arduino is currently connected to iDigi.

See example: iDigiConnect


#### iDigi.step()

Must be called at the end of every loop().

iDigi.step() keeps the connection with iDigi active by executing any
outstanding network operations.

See example: iDigiConnect


### Sending Data to iDigi

Files appear under iDigi's *Data Services* folder nested in a folder
containing your device's iDigi Device ID (see iDigi.getDeviceIdString()).

#### iDigi.dataService.putFile()

Sends a file to iDigi.

If you need to send a large file, consider using
iDigi.dataService.putFileAsync().

See example: iDigiPutFile


#### iDigi.dataService.putFileAsync()

Sends a file to iDigi asynchronously (a piece at a time).

Each piece of the file is requested by the iDigi Connector library
from your sketch.  You pass a function to putFileAsync() which will
be called repeatedly until you have no more data to upload.

See example: iDigiPutFileAsync


#### iDigi.dataService.putFileAsyncBusy()

A function used to indicate to your sketch that a file is uploading in
the background.

While this function returns true, no calls to iDigi.dataService.putFileAsync()
are aloud.

See example: iDigiPutFileAynsc


### Receiving Data from iDigi

You can send data to your Arduino from anywhere in the world by using iDigi's
simple web services.  To experiment with these web services simply log into
your free iDigi account, navigate to the *Web Services Console*, add your
Arduino's iDigi Device ID as a target, and select the
"SCI->Data Services" example from the *Examples* drop-down.


#### iDigi.dataService.registerHandler()

Called with the name of a function to call when iDigi has data for your
sketch.

See example: iDigiDataServices

#### iDigi.dataService.sendResponse()

Called from your sketch's function which handles requests from iDigi.  You
call this function when you want to send a response to iDigi.

See example: iDigiDataServices


License
-------

This software is open-source software.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.


