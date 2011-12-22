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
 * routines which process the firmware download requests.  
 *
 * This sample defines all supported image files with .a and .bin file extensions.
 * The file extension is only validated when updataing firmware from iDigi Device Cloud.
 *
 * @code
 *
 * /* list of all supported firmware target info */
 * static firmware_list_t fimware_list[] = {
 *   /* version     code_size     name_spec          description */
 *   {0x01000000, (uint32_t)-1, ".*\\.a",            "Library Image"}, /* any .a image */
 *   {0x00000100, (uint32_t)-1, ".*\\.[bB][iI][nN]", "Binary Image" }  /* any .bin image */
 * };
 *
 * @endcode
 *
 * The routine firmware_download_request() is called when the download is first
 * initiated, it receives information about the download and retuns @ref idigi_fw_success
 * status indicating that is ready to accept the FW download.
 *
 * The routine firmware_image_data() is called repeatidly with the image data
 * until the download is complete, the routine firmware_download_complete()
 * is called when the download is complete.  At this point the FW can veirfy
 * that the downloaded image is valid.
 *
 * When firmware_download_complete() returns @ref idigi_fw_download_success status,
 * it indicates the firmware image has been updated. iDigi Device Cloud will reset the unit,
 * the routine firmware_reset() is called for resetting.
 * 
 * The routine firmware_download_abort() is called when idigi server encounters error.
 * 
 * @section connect_build Building
 *
 * To build this example for a linux based platform you can go into the directory
 * public/run/samples/firmware_download and type make.  If you are not running Linux you
 * will need to setup your build system with the information described below
 * and then build the image for your platform.
 *
 * @subsection source_files Source Files
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
 * <th>update_firmware.py</th>
 * <td>Python script for firmware download</td>
 * <td>samples/firmware_download</td>
 * </tr>
 * <tr>
 * <th>query_firmware.py</th>
 * <td>Python script for query supported firmware information</td>
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
 * @subsection include_paths Include Paths
 *
 * The following include paths are required:
 *
 * @li public/include
 * @li run/platforms/@a my_platform
 *
 * @subsection defines Defines
 *
 * The following defines are used in this sample:
 *
 * @li -DIDIGI_VERSION=0x1010000UL (indicates version 1.1 of the IIK)
 *
 * @section running Running
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
 * @subsection fw_python Firmware Download using python script
 *
 * This samples provides two simple python scripts to update firmware image and query
 * a list of firmware target information using
 * @htmlonly <a href="web_services.html">iDigi Web Services.</a> @endhtmlonly
 * It sends a request to developer.idigi.com.
 * 
 * update_firmware.py opens and sends "image.a" to the device.
 *
 *
 * To execute update_firmware.py python script:
 *
 * @code
 * python update_firmware.py <username> <password> <device_id>
 * @endcode
 *
 * An output of update_firmware.py:
 *
 * @code
 *
 * Response:
 * <sci_reply version="1.0">
 *     <update_firmware>
 *         <device id="00000000-00000000-00000000-00000000">
 *         <submitted/>
 *         </device>
 *     </update_firmware>
 * </sci_reply>
 *
 * @endcode
 *
 * To execute query_firmware.py python script:
 *
 * @code
 * python query_firmware.py <username> <password> <device_id>
 * @endcode
 *
 * An output of query_firmware.py:
 * @code
 *
 * Response:
 * <sci_reply version="1.0">
 *    <query_firmware_targets>
 *    <device id="00000000-00000000-00000000-00000000">
 *       <targets>
 *           <target number="0">
 *               <name>Library Image</name>
 *               <pattern>.*\.a</pattern>
 *               <version>1.0.0.0</version>
 *               <code_size>4294967295</code_size>
 *           </target>
 *           <target number="1">
 *               <name>Binary Image</name>
 *               <pattern>.*\.[bB][iI][nN]</pattern>
 *               <version>0.0.1.0</version>
 *               <code_size>4294967295</code_size>
 *           </target>
 *         </targets>
 *       </device>
 *   </query_firmware_targets>
 * </sci_reply>
 *
 * @endcode
 *
 *
 *
 */
