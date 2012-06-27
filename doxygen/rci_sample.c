/*! @page rci_sample Remote Configuration Sample
 *
 * @htmlinclude nav.html
 *
 * @section rci_sample_overview Overview
 *
 * This sample demonstrates how to retreive and set device configurations using @ref rci_service "remote configuration callbacks".
 * This is a simple sample which does not write on the device but demonstrates the remote configuration callback.
 *
 * @section connect_description Code Flow
 *
 * The routine main() in the platform directory initializes the iDigi connector and
 * registers the application callback.  In the file application.c is the
 * application defined callback app_idigi_callback(), this routine calls app_remote_config_handler()
 * when a remote command interface (RCI) request is received.
 *
 * A User can use the included python script to send a device request to the device.
 *
 * This sample defines a configuration group with two unsigned integer elements:
 * - The name of the configuration group is @a @b keepalive.
 * - It contains 2 elements and the names of the elements are:
 *      @htmlonly <ol><li><i><b>tx</b></i></li>
 *                      <li><i><b>rx</b></i></li></ol> @endhtmlonly
 *
 *
 * @subsection config_file iDigi connector configuration file
 *
 * The following is the iDigi connector configuration file for the keepalive configuration group:
 * 
 * @code
 *  group setting keepalive "iDigi Connector Keepalive Configuration"
 *     element rx "Rx interval" "Time value which device will send Keep-Alive messages must be between 5 to 7200 seconds" type uint32 min 5 max 7200 unit "in seconds"
 *     element tx "Tx Interval" "Time value which server will send Keep-Alive messages must be between 5 to 7200 seconds" type uint32 min 5 max 7200 unit "in seconds"
 *     
 *     error load_fail "Unable to load the value"
 *     error save_fail "Unable to save the value"
 *     error no_memory "Insufficient memory"
 * @endcode
 *
 * Run @ref rci_tool to generate remote_config.h:
 * @code
 *
 *   java -jar ConfigGenerator.jar username:xxxxxx "Linux Application" 16777216 config.rci
 *
 * @endcode
 *
 * When the iDigi Cloud sends a RCI request to the device, app_remote_config_handler() is called with the following request ID:
 * for:
 * -# @ref idigi_remote_config_session_start
 * -# @ref idigi_remote_config_action_start
 * -# @ref idigi_remote_config_group_start
 * -# @ref idigi_remote_config_group_process
 * -# @ref idigi_remote_config_group_end
 * -# @ref idigi_remote_config_action_end
 * -# @ref idigi_remote_config_session_end
 *
 * The callback with @ref idigi_remote_config_group_process request ID is continually called 
 * unitl all elements in the configuration group is processed.
 *
 * This sample prints out the value of the configuration group in the callback with
 * @ref idigi_remote_config_group_end request id.
 *
 * When the callback is called with @ref idigi_remote_config_session_cancel request ID it indicates
 * an error is encountered and needs to cancel the RCI. In this example it cancels 
 * the request by freeing any memory used.
 *
 * 
 * @section connect_build Building
 *
 * To build this example for a Linux-based platform you can go into the directory
 * public/run/samples/simple_remote_config and type make.  If you are not running Linux you
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
 * <th class="title">Name</th>
 * <th class="title">Description</th>
 * <th class="title">Location</th>
 * </tr>
 * <tr>
 * <th>application.c</th>
 * <td>Contains application_run() and the application defined callback</td>
 * <td>samples/simple_remote_config</td>
 * </tr>
 * <tr>
 * <th>firmware.c</th>
 * <td>Routines used to process the FW download requests</td>
 * <td>samples/simple_remote_config</td>
 * </tr>
 * <tr>
 * <th>keepalive.c</th>
 * <td>Routines used to set and query the keepalive configuration group</td>
 * <td>samples/simple_remote_config</td>
 * </tr>
 * <tr>
 * <th>remote_config_cb.c</th>
 * <td>Routines used to remote configuration callback</td>
 * <td>samples/simple_remote_config</td>
 * </tr>
 * <tr>
 * <th>idigi_api.c</th>
 * <td>Code for the iDigi connector </td>
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
 * @li -DIDIGI_VERSION=0x1020000UL (indicates version 1.1 of the iDigi connector)
 *
 * @section running Running
 *
 * Once successfully built, run the executable, in Linux type ./idigi to run the
 * sample.
 *
 * Remote configuration can be accessed through the
 * @htmlonly <a href="http://www.idigi.com">iDigi Cloud</a>@endhtmlonly.
 *
 * @htmlinclude terminate.html
 *
 */
