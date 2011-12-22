/*! @page send_data Send Data Sample
 *
 * @htmlinclude nav.html
 *
 * @section send_data_overview Overview
 *
 * This sample demonstrates how to send data from a device to the iDigi Server.
 * Using the @ref data_service_overview "data service api" this application writes data to a file on the
 * idigi server.
 *
 * @section connect_description Code Flow
 *
 * The routine main() in the platform directory initializes the IIK and
 * registers the application callback.  In the file application.c is the
 * application defined callback idigi_callback(), this routine calls idigi_data_service_callback()
 * when a data service request is received.
 *
 * The routine application_run() (or application_step()) which is called after initialization calls the routine
 * send_put_request() to initiate the data transfer to the iDigi Cloud.  This routine
 * initiates the data transfer by calling idigi_initiate_action(); once the server is 
 * ready to receive data the idigi_data_service_callback() routine is called, the callback then returns
 * the string "Welcome to iDigi data service sample!\n" back to the iDigi Cloud.  This data is @b copied
 * @b into @b the @b callbacks @b buffer, the flags @ref IDIGI_MSG_LAST_DATA and @ref IDIGI_MSG_FIRST_DATA
 * indicate this is the only message in this data transfer, by modifying these flags
 * you could modify this sample to send large amounts of data back to the server.
 *
 * If idigi_data_service_callback() is called with @ref idigi_data_service_type_have_data then this
 * indicates a response from the server, @ref idigi_data_service_type_error would indicate
 * an error is sent back from the server.  In this example send_put_request() is called
 * once a second from application_run() (or application_step()).
 *
 * @section connect_build Building
 *
 * To build this example for a linux based platform you can go into the directory
 * public/run/samples/send_data and type make.  If you are not running Linux you
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
 * <th>application.c</th>
 * <td>Contains applicat_start() and the application callback</td>
 * <td>samples/send_data</td>
 * </tr>
 * <tr>
 * <th>put_request.c</th>
 * <td>data service callback and send_put_request() which starts the data transfer</td>
 * <td>samples/send_data</td>
 * </tr>
 * <th>get_file.py</th>
 * <td>Python script to retrieve a file</td>
 * <td>samples/send_data</td>
 * </tr>
 * <tr>
 * <th>idigi_api.c</th>
 * <td>Code for the IIK </td>
 * <td>private</td>
 * </tr>
 * <tr>
 * <th>os.c</th>
 * <td>Operating system calls</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <th>network.c</th>
 * <td> Network interface </td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <th>config.c</th>
 * <td>Configuration routines</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <th>main.o</th>
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
 * @section web_gui Viewing results from iDigi Device Cloud
 *
 * To view the results for this sample login to the iDigi Device Cloud as described
 * in the @ref add_your_device_to_the_cloud "Getting Started" section. 
 *
 * Once logged in click on storage from within the left navigation panel as shown
 * below. Double clicks to the device folder.
 *
 * @image html sample1.png
 *
 * This samples creates test.txt under test directory. Double clicks to open the test directory.
 *
 * @image html send_data_sample3.png
 *
 * The file is then listed as shown (below). You can click on a 
 * file to view its contents, you will see the file test.txt in the test directory
 * after you run this sample.
 *
 * @image html sample2.png
 *
 * @section python_script Python script to retrive a file from iDigi Device Cloud storage
 *
 * This sample provides a simple python script to retrive the test.txt file from developer.idigi.com
 * after a device has put the test.txt into the server.
 *
 * Run the python script in the sample. 
 *
 * @code
 * python get_file.py <username> <password> <device_id>
 * @endcode
 *
 * The script prints the content of test.txt.
 *
 *
 */
