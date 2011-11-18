/*! @page fw_download_sample Firmware Download Sample
 *
 * @htmlinclude nav.html
 *
 * @section fw_download_sample_overview Overview
 *
 * This sample demonstrates how to download firmware to your device using the 
 * @ref fw_overview1 "firmware download API".  This is a generic sample which 
 * does not update the firmware on the device but demonstrates how to use this
 * API.
 *
 * @section connect_description Code Flow
 *
 * The file application.c contains the application defined callback idigi_callback()
 * this callback in turn calls idigi_firmware_callback() when a firmware request
 * is received.  All interaction with the IIK is through the callback in this sample;
 * the FW download is initiated by the server.  The file firmware.c contains the
 * routines which process the firware download requests.  
 *
 * The routine firmware_download_request() is called when the download is first
 * initiated, it receives information about the download and retuns @ref idigi_fw_success
 * indicating that is ready to accept the FW download.
 *
 * The routine firmware_image_data() is called repeatidly with the image data
 * until the download is complete, the routine firmware_download_complete()
 * is called when the download is complete.  At this point the FW can veirfy
 * that the downloaded image is valid.
 *
 *
 * @section connect_build Building
 *
 * To build this example for a linux based platform you can go into the directory
 * public/run/samples/firmware_download and type make.  If you are not running Linux you
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
 * <td>Contains applicat_start() and the application defined callback</td>
 * <td>samples/firmware_download</td>
 * </tr>
 * <tr>
 * <th>firmware.c</th>
 * <td>Routines used to process the FW download requests</td>
 * <td>samples/firmware_download</td>
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
 * <th>main.c</th>
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
 * @li run/platforms/@a my_platform
 *
 * @subsection Defines
 *
 * The following defines are used in this sample:
 *
 * @li -DIDIGI_VERSION=0x1010000UL (indicates version 1.1 of the IIK)
 *
 * @section Running
 *
 * Once successfully built, run the executable, in Linux type ./idigi to run the
 * sample.
 *
 * Firmware downloads can be initiated by the server using the iDigi Device Cloud
 * GUI or by issuing a @ref services "SCI update_firmare operation".
 *
 * @subsection fw_gui Firmware Download using iDigi Device Cloud GUI
 *
 * Login to the iDigi Device cloud at http://www.idigi.com  and click on your device 
 * (described in the @ref idigi_login "Getting Started Section").
 * Once you are logged in and see your device Click the Refresh button. 
 * The device will reconnect to the iDigi Developer Cloud.
 *
 * @image html sample3.png
 *
 * Then click the Firmware Update button to update your firmware, and then select 
 * the file that is your firmare image on your local machine.  The image will then
 * start to download to your machine, the GUI will indicate when it's complete.
 *
 * @note The firmware image name must match the regular expression given to 
 * filter names, see @ref fw_namespec.
 *
 * @image html sample4.png
 * 
 * The results in this application are displayed in the output console of the
 * application, since this is only a sample the fiwmware is not updated on the
 * device.  The application prints out the target, code size and checksum of the
 * downloaded image.
 *
 * @subsection fw_sci Firmware Download using iDigi Web Services
 *
 *
 *
 */
