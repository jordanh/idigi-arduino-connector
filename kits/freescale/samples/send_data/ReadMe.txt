
  o----------------------------o
  | Send Data to iDigi Example |
  o----------------------------o

  Compatible platforms
  --------------------
  * TWRK60n512 Board (current)

  Introduction
  ------------
  This sample sends raw data to iDigi Cloud when push button SW1
  is pushed once. The sent data will be stored in a specified file
  path on the Cloud.

  Requirements
  ------------
  To run this example you will need:
    * Freescale Tower Kit with Ethernet connection.
    * An iDigi account with the device registered in.

  Example setup
  -------------
  1) Make sure the hardware is set up correctly:
       a) The device is powered on.
       b) The device is connected to the Local Area Network (LAN) by the
          Ethernet cable and has Internet access.

  2) Configure the iDigi settings using the 'iDigi Connector Configurator'.
     To open it double-click on the 'idigi_config.xml' file of the project:

       * Select 'General' and configure:
           - The 'MAC Address' of your device.
           - The 'Vendor ID' of your iDigi account.

       * Save the changes by selecting 'File > Save'.

  3) Go to 'Start > Programs > P&E OSBDM OSJTAG Virtual Serial Toolkit
     > Utilities > Terminal Utility' to open the Terminal window.

     Select 'USB Port' from the 'Port' combo box and configure the following
     connection parameters:
         Baudrate: 115200
         Parity:   None
         Bits:     8

     Click the 'Open Serial Port' button to open the port. The message 'Open'
     will appear next to the 'Bits' combo box.

  4) Make sure the Eclipse IDE has the iDigi settings configured correctly.
     To do so, go to 'Window > Preferences' and select the 'iDigi
     Configuration' group. Ensure the 'User name' and 'Password' values are
     configured with your iDigi account credentials.

  Running the example
  -------------------
  The example is already configured, so all you need to do is to build and
  launch the application.

  While it is running, you can press the push button 'SW1' to send raw data to
  the cloud. You can see the console output on the terminal window for the
  device status.

  To verify that the text file has been uploaded to iDigi successfully, follow
  these steps:
    1) Press the 'iDigiME' button of the toolbar to access to your iDigi
       dashboard.
    2) Once there, go to the 'iDigi Manager Pro' section and access the 'Data
       Services' item from the top menu.
    3) Open the storage folder corresponding with your Device ID.

       NOTE: Use the table in 'iDigi Manager Pro > Devices' section to find
       out the Device ID of your board.

    4) You should be able to see a file called 'test.txt' under 'test/'. Open it
       and verify it contains the text you sent with the application.

  Tested On
  ---------
  TWRK60n512 Board

