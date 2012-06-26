
  o------------------------------o
  | Device Request iDigi Example |
  o------------------------------o

  Compatible platforms
  --------------------
  * TWRK60n512 Board (current)

  Introduction
  ------------
  This sample demonstrates how one can use the iDigi cloud to control their remote device.
  The sample allows user to turn ON or OFF the LEDs (LED1 or LED2).

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

  To verify that that file with your message has been sent to iDigi follow
  these steps:
    1) Press the 'iDigiME' button of the toolbar to access to your iDigi
       dashboard.
    2) Once there, go to the 'Web Services Console' section.
    3) Select 'SCI Targets' to open the 'Select devices to be used in examples'
       dialog. Use the 'Add Targets' combo box to look for the right Device ID,
       click 'Add' and then 'OK'.

       NOTE: Use the table in 'iDigi Manager Pro > Devices' section to find
       out the Device ID of your board.

    4) Select the option 'Examples > SCI > Data Service > Send Request', on the
       left panel a SCI request will be automatically generated.
    5) Make sure the Device ID is correct.
    6) Replace the target with "LED1" or "LED2" and the payload with ON or
       OFF, i.e., to turn on LED1 the request should be:

       <sci_request version="1.0">
          <data_service>
             <targets>
                <device id="00000000-00000000-XXXXXXFF-FFXXXXXX"/>
             </targets>
             <requests>
                <device_request target_name="LED1">
                   ON
                </device_request>
             </requests>
          </data_service>
       </sci_request>

    7) Select 'Send' to see the request coming in the device. You should see
       the console output on the terminal window for the received request:

       Turning ON LED1
       LED1 action is Success

    8) Observe the response returned from the device on the 'Web Services
       Responses' on the right. Click 'Click to examine' to see both request
       and response, the last will be something similar to:

       <sci_reply version="1.0">
          <data_service>
             <device id="00000000-00000000-XXXXXXFF-FFXXXXXX">
                <requests>
                   <device_request target_name="LED1" status="1">Success</device_request>
                </requests>
             </device>
          </data_service>
       </sci_reply>

  Tested On
  ---------
  TWRK60n512 Board

