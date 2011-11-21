/*! @page device_request Data Service Device Request Sample
 *
 * @htmlinclude nav.html
 *
 * @section device_request_overview Overview
 *
 * This sample demonstrates how to process data sent from the iDigi Server.
 * Using the @ref data_service_overview "data service api" this application prints
 * out the data sent from idigi server.
 *
 * @section connect_description Code Flow
 *
 * The routine main() in the platform directory initializes the IIK and
 * registers the application callback.  In the file application.c is the
 * application defined callback idigi_callback(), this routine calls idigi_data_service_callback()
 * when a device request is received.
 *
 * User can use python script to send device request to the device.
 *
 * When server sends device request to the device, idigi_data_service_callback() is called 
 * with @ref idigi_data_service_type_have_data message type to process the data. This callback
 * is continually called with this message type until the last chunk of data is processed. This 
 * sample exmains the given target and prints out the device request data. It cancels 
 * the device request if the target name is not "myTarget". 
 *
 * After the callback is called with @ref idigi_data_service_type_have_data message type for the last chunk of data,
 * the callback is called with @ref idigi_data_service_type_need_data message type to send response
 * data back to iDigi server. This sample sends and prints out the request and response data.
 *
 * When the callback is called with @ref idigi_data_service_type_error message type it indicates
 * an error is encountered and needs to cancel the device request. In this example it cancels 
 * the request by freeing any memory used.
 *
 * @section python_script Python script
 *
 * This sample provides a simple python script to send a device request to developer.idigi.com. 
 * You need to edit the username, password, and the device ID in the device_request.py file.
 *
 * @code
 *
 * # device_request.py
 * # -------------------------------------------------
 * # The following lines require manual changes
 * username = "YourUsername" # enter your username
 * password = "YourPassword" # enter your password
 * device_id = "Target Device Id" # enter device id of targetimport httplib
 * # -------------------------------------------------
 *
 * @endcode
 *
 * @section connect_build Building
 *
 * To build this example for a linux based platform you can go into the directory
 * public/run/samples/device_request and type make.  If you are not running Linux you
 * will need to setup your build system with the information described below
 * and then build the image for your platform.
 *
 * @subsection Source Files
 *
 * The following is a list of source files required to build this sample:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</td>
 * <th class="title">Description</td>
 * <th class="title">Location</td>
 * </tr>
 * <tr>
 * <td>application.c</td>
 * <td>Contains applicat_start() and the application callback</td>
 * <td>samples/device_request</td>
 * </tr>
 * <tr>
 * <td>device_request.c</td>
 * <td>Rountines used to process the data service device requests</td>
 * <td>samples/device_request</td>
 * </tr>
 * <tr>
 * <td>device_request.py</td>
 * <td>Python script to send device request</td>
 * <td>samples/device_request</td>
 * </tr>
 * <tr>
 * <td>idigi_api.c</td>
 * <td>Code for the IIK </td>
 * <td>private</td>
 * </tr>
 * <tr>
 * <td>os.c</td>
 * <td>Operating system calls</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>network.c</td>
 * <td> Network interface </td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>config.c</td>
 * <td>Configuration routines</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>main.o</td>
 * <td>Starting point of program, dependent on build environment</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @subsection Include Paths
 *
 * The following include paths are required:
 *
 * @li public/include
 * @li run/platforms/linux (substitute with your platform)
 *
 * @subsection Defines
 *
 * The following defines are used in this sample:
 *
 * @li -DIDIGI_VERSION=0x1010000UL (indicates version 1.1 of the IIK)
 *
 * @section Results
 *
 * Run the python script in the sample. 
 *
 * This sample prints "My device request data" and "myTarget" for the device request.
 *
 * The python script prints "My device response data" for the device response data.
 *
 *
 */
