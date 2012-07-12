/*! @page device_request_sample Device Request Sample
 *
 * @htmlinclude nav.html
 *
 * @section device_request_overview Overview
 *
 * This sample demonstrates how to process data sent from the iDigi Server
 * using the @ref idigi_register_device_request_callbacks "device request callbacks".
 * Commands sent from the iDigi Cloud are used by this application to control
 * the status LED of the Kontron unit.  The status LED is located on top of the unit
 * next to the SIM slot.  You can expand this application to control your device.
 *
 * @section device_request_description Code Flow
 *
 * The routine main() calls application_start() which initializes the iDigi Connector by calling
 * idigi_connector_start(), this will initiate the connection process to the iDigi cloud, if any errors are
 * encountered they are reported to stdout and will show up in the workbench console window.
 *
 * The device request callbacks are registered in application_start() by calling idigi_register_device_request_callbacks().
 * When data is received from the iDigi cloud the callback device_request_callback() is called.
 * The target parameter contains the command to the device, which for this sample can be
 * LED_ON or LED_OFF.  The routine device_request_callback() checks the command received against a
 * list of commands and calls the corresponding command handler routine.
 *
 * The callback device_response_callback is called by the iDigi connector library to get the response
 * to the previous command.  If the command was received OK then the response "iDigi Connector device response OK\n"
 * is sent back to the iDigi cloud.
 *
 * The routines led_on and led_off are the command handlers for the corresponding commands LED_ON and LED_OFF.
 *
 * @section device_request_build Building
 *
 *    1. Click on Wind River Workbench icon, this is used to build, debug and launch the idigi samples
 *       applications.
 *    2. The sample applications appear on the left hand side of the workbench.
 *    3. Click on idigi_device_request
 *    4. Right mouse click and select Build Project.
 *
 * In the Build Console on the lower portion of the screen you can view the build output.
 *
 *
 * @subsection source_files Source Files
 *
 * The following is a list of source files required to build this sample:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</th>
 * <th class="title">Description</th>
 * <th class="title">Location</th>
 * </tr>
 * <tr>
 * <th>main.c</th>
 * <td>Entry point of program, calls application_start()</td>
 * <td>workspace/idigi_device_request</td>
 * </tr>
 * <tr>
 * <th>application.c</th>
 * <td>Contains application_start(), callbacks and routines to handle commands from the iDigi cloud</td>
 * <td>workspace/idigi_device_request</td>
 * </tr>
 * <tr>
 * <th>led_on.py</th>
 * <td>Python script to turn the LED on</td>
 * <td>workspace/idigi_device_request</td>
 * </tr>
 * <tr>
 * <th>led_off.py</th>
 * <td>Python script to turn the LED off</td>
 * <td>workspace/idigi_device_request</td>
 * </tr>
 * <tr>
 * <th>idigi_connector.h</th>
 * <td> iDigi Connector options</td>
 * <td>usr/lib</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @subsection include_paths Include Paths
 *
 * The following include paths are required:
 *
 * @li usr/include
 *
 * @section python_script Python script to control_led
 *
 * This sample provides a simple python script to send a data service device request to developer.idigi.com
 * using @htmlonly <a href="web_services.html">iDigi Web Services.</a> @endhtmlonly
 * To run the python script for this sample:
 *
 * @code
 * python led_on.py <username> <password> <device_id>
 * @endcode
 *
 * @note The devive_id must be in the long format, for example: python led_off.py username password 00000000-00000000-00b338ff-ff0002d8
 * where the MAC address of the device is 00:b3:38:00:02:d8
 *
 * Output from this sample:
 *
 * @code
 * Device request data: received data = "My device request data" for target = "myTarget"
 * Device response data: send response data = My device response data
 * @endcode
 *
 * @htmlinclude terminate.html
 */
