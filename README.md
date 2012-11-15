iDigi Connector for Arduino
===========================

http://spaces.atmel.com/gf/project/idigi-arduino/

The iDigi Connector is a software library that allows you to connect your
Arduino to the [iDigi Device Cloud][iDigi].  Why would you want to connect
your Arduino to iDigi?

With iDigi you can:

  * Control your Arduino sketch remotely from a web app or smart phone, even
    if your Arduino is behind a firewall

  * Upload data from your Arduino to iDigi using a simple, easy to use
    interface--either raw files or data streams which can be graphed or
    visualized

  * Configure iDigi to push data to your applications using its HTTP
    Push Monitor API

  * Upload and download files from your Arduino's remote SD card

[iDigi]: http://www.idigi.com


Requirements
------------

The iDigi Connector for Arduino requires an [Arduino Mega 2560][2560],
[Arduino Mega ADK][ADK], or [Arduino Due][Due] with an [Arduino Ethernet Shield][shield].  The iDigi Connector requires approximately 40k of flash and 2.5k of RAM.  Sorry, the [Arduino Uno][Uno] and similar are not yet supported due to memory constraints.

[2560]: http://arduino.cc/en/Main/ArduinoBoardMega2560
[ADK]: http://arduino.cc/en/Main/ArduinoBoardADK
[Due]: http://arduino.cc/en/Main/ArduinoBoardDue
[shield]: http://arduino.cc/en/Main/ArduinoEthernetShield
[Uno]: http://arduino.cc/en/Main/ArduinoBoardUno

For the Mega you must have also installed the [Arduino IDE Software][software], version 1.0 or later.  

If you are using a Due, Arduino IDE version 1.5 or later is required.

[software]: http://arduino.cc/en/Main/Software


Installation
------------

To install the iDigi Connector for Arduino, you'll need to install it as
a library for the Arduino Software IDE.  In order to install it, follow the
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
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <iDigi.h>
```

  2. In your sketch's setup() function, call iDigi.begin() with your Ethernet
     sheild's MAC address, your IP address, a vendor id of 0, the
     iDigi server you'd like to connect to, and the name you'd like your
     Arduino to show up as on iDigi:

  (If you'd rather use DHCP, refer to one of the examples included in the library)

```c++

byte mac[] =
    { 0x90, 0xA2, 0xDA, 0x05, 0x00, 0x57 };
IPAddress ip(192, 168, 1, 42);
IPAddress gw(192, 168, 1, 1);
IPAddress nameserver(8, 8, 8, 8);
IPAddress subnet(255, 255, 255, 0);
#define IDIGI_SERVER       "my.idigi.com"
#define IDIGI_DEVICE_NAME  "Arduino"
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
     as dataService.putFile(), dataService.attachInterrupt() in order to
     interact with the world! At the end of each loop(), ensure that you
     call iDigi.update():

```c++
void loop()
{
  if (iDigi.isConnected())
  {
    // Upload a test message to the file, "hello.txt" on iDigi:
    size_t uploaded = iDigi.dataService.putFile("hello.txt", "text/plain",
  			"Hello, world!\r\n", 16, IDIGI_DATA_PUT_APPEND);

    delay(1000);
  }
  // interact with iDigi:
  iDigi.update();
}
```

High-Level Function Reference
-----------------------------

### Basic Functions

#### iDigi.begin()

Called to start the iDigi Connector for Arduino library.

Parameters include the Ethernet Shield's MAC address and other information
used to describe your Arduino to the iDigi Device Cloud platform.

See example: iDigiConnect


#### iDigi.getId()

Retrieves the current iDigi Device ID which is used to uniquely identify
your Arduino as a string.

This iDigi Device ID is used as an address to send your Arduino messages
from iDigi.

See example: iDigiConnect


#### iDigi.isConnected()

Returns true if your Arduino is currently connected to iDigi.

See example: iDigiConnect


#### iDigi.update()

Must be called at the end of every loop().

iDigi.update() keeps the connection with iDigi active by executing any
outstanding network operations or calling functions you may have
registered with dataService.attachInterrupt() or
dataService.sendFileAsync() (see below).

See example: iDigiConnect


### Sending Data to iDigi

Files appear under iDigi's *Data Services* folder nested in a folder
containing your device's iDigi Device ID (see iDigi.getId()).

#### iDigi.dataService.sendFile()

Sends a file to iDigi.

If you need to send a large file, consider using
iDigi.dataService.sendFileAsync().

See example: iDigiSendFile


#### iDigi.dataService.sendFileAsync()

Sends a file to iDigi asynchronously, a piece at a time.

First, you pass a function to sendFileAsync()
which will return immediately.  Next, you call iDigi.update() as
normal.  Each time iDigi.update() is called the function you passed
to sendFileAsync() will be called to retrieve a piece of the file.
until you indicate have no more data to upload.

See example: iDigiSendFileAsync


#### iDigi.dataService.sendFileAsyncFinished()

A function can use check if your sketch is finished uploading a file to
iDigi in the background.

While this function returns false, no calls to 
iDigi.dataService.sendFileAsync() are aloud.

See example: iDigiSendFileAynsc


#### iDigi.dataService.iDigi.dataService.sendDataset()

Sends a batch of time-series samples (such as sensor information) to
iDigi for graphing, storage and analytics.  As your sketch executes,
you add your samples to a iDigiDataset object.  When you want to
upload them to iDigi you call the sendDataset() function.

See examples: iDigiDataSet, iDigiDataSetT


### Receiving Data from iDigi

You can send data to your Arduino from anywhere in the world by using iDigi's
simple web services.  To experiment with these web services simply log into
your free iDigi account, navigate to the *Web Services Console*, add your
Arduino's iDigi Device ID as a target, and select the
"SCI->Data Services" example from the *Examples* drop-down.

#### iDigi.dataService.attachInterrupt()

Called with the name of a function to call when iDigi has data for your
sketch.

See example: iDigiDataService

#### iDigi.dataService.sendResponse()

Called from your sketch's function which handles requests from iDigi.  You
call this function when you want to send a response to iDigi.

See example: iDigiDataService


### Managing an SD Card Remotely

You can use iDigi to manage a remote SD card connected to your Arduino.  This can be used to update elements of a sketch remotely--such as sound or
other media files--or use it to fetch log files or other items you have
written.

#### iDigi.fileSystem.enableSharing()

Call this function in order to allow iDigi to have access to your SD card
remotely.  You must pass in the "chip select" used to enable the SD card
(usually 4) and the SPI "slave select" (usually 53) pin numbers.

See example: iDigiFileSystem

#### iDigi.fileSystem.disableSharing()

Call this function in order to disable sharing the SD card with iDigi.

See example: iDigiFileSystem


Notes for Advanced Developers
-----------------------------

### Contributing

If you have an idea for an enhancement or an example you'd like to contribute to the iDigi Connector for Arduino we'd love to have you involved!  The
official distribution site for the iDigi Connector for Arduino is hosted at Atmel Spaces: http://spaces.atmel.com/gf/project/idigi-arduino/

If you have a patch you'd like to submit we're also on GitHub:

  https://github.com/jordanh/idigi-arduino-connector

### Working with the Source Repository

If you clone the source repository, you'll need to symlink some directories into your Arduino library directory.

Let's assume that the iDigi Connector for Arduino source has been cloned into a directory `$HOME/source/idigi-connector-arduino` and that your Arduino library is in `$HOME/Documents/Arduino/libraries`.  You'll need to execute this sequence of commands to be able to make changes to the library and test it within the Arduino IDE easily:

```
$ cd $HOME/Documents/Arduino/libraries
$ mkdir idigi ; cd idigi
$ ln -s $HOME/source/idigi-connector-arduino/examples ./examples
$ ln -s $HOME/source/idigi-connector-arduino/utility ./utility
$ ln -s $HOME/source/idigi-connector-arduino/iDigi.h .
$ ln -s $HOME/source/idigi-connector-arduino/keywords.txt .
$ ln -s $HOME/source/idigi-connector-arduino/README.md .
```

### Generating a Library Archive

If you want to share your own special version of the iDigi Connector for Arduino with a friend you'll need to run the `build_dist.sh` tool included in the source repository.  Running the command with the `--help` option will tell you how to use it:

```
$ ./build_dist.sh --help
Usage: build_dist.sh [options] [arguments]

Options:
  -o, --output_file   output ZIP archive filename
  -h, --help           display this help and exit
```

All you need to do is tell it where you'd like to output the ZIP archive and you'll be sharing your custom iDigi Connector for Arduino in no time!

```
$ ./build_dist.sh -o my-idigi-connector-arduino.zip
Copying README.md to /var/folders/yw/gz3_59bs5dd182mt977q2tc80000gn/T/build_dist.sh.ykzWmTJu/idigi_connector_arduino/iDigi
Copying iDigi.h /var/folders/yw/gz3_59bs5dd182mt977q2tc80000gn/T/build_dist.sh.ykzWmTJu/idigi_connector_arduino/iDigi
Copying keywords.txt /var/folders/yw/gz3_59bs5dd182mt977q2tc80000gn/T/build_dist.sh.ykzWmTJu/idigi_connector_arduino/iDigi
Copying examples/ /var/folders/yw/gz3_59bs5dd182mt977q2tc80000gn/T/build_dist.sh.ykzWmTJu/idigi_connector_arduino/iDigi
Copy utility/ /var/folders/yw/gz3_59bs5dd182mt977q2tc80000gn/T/build_dist.sh.ykzWmTJu/idigi_connector_arduino/iDigi
Compressing to /Users/jordanh/Source/Repositories/spaces.atmel.com/idigi-arduino.git/my-idigi-connector-arduino.zip...
  adding: idigi_connector_arduino/ (stored 0%)
  adding: idigi_connector_arduino/iDigi/ (stored 0%)
  ...
Cleaning up...
$
```


License
-------

This software is open-source software.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.


