/*! @page file_system_sample File System Sample
 *
 * @htmlinclude nav.html
 *
 * @section fs_sample_overview Overview
 *
 * This sample demonstrates how read and write files and list files or directories on your device, 
 * using the @ref file_system_overview1 "file system API".
 *
 * @section fs_sample_description Code Flow
 *
 * The routine main() in the platform directory initializes the IIK and registers the application callback 
 * app_idigi_callback(), defined in the file application.c.
 *  
 * All file system requests are initiated by the iDigi Device Cloud. When the iDigi Device Cloud sends
 * a file system request, app_idigi_callback() calls app_file_system_handler() routine, located in 
 * the file_system.c file. 
 * 
 * The app_file_system_handler() routine calls the following routins for different file I/O operations: 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Routine</td>
 * <th class="title">Operation</td>
 * <th class="title">Request Id</td>
 * </tr> 
 * <tr> 
 * <td>app_process_file_open()</td><td>Open a file</td><td>@endhtmlonly @ref idigi_file_system_open @htmlonly</td>
 * </tr> 
 * <tr> 
 * <td>app_process_file_lseek()</td><td>Seek file position</td><td>@endhtmlonly @ref idigi_file_system_lseek @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_read()</td><td>Read file data</td><td>@endhtmlonly @ref idigi_file_system_read @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_write()</td><td>Write file data</td><td>@endhtmlonly @ref idigi_file_system_write @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_ftruncate()</td><td>Truncate a file</td><td>@endhtmlonly @ref idigi_file_system_ftruncate @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_close()</td><td>Close a file</td><td>@endhtmlonly @ref idigi_file_system_close @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_rm()</td><td>Remove a file</td><td>@endhtmlonly @ref idigi_file_system_rm @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_opendir()</td><td>Open a directory</td><td>@endhtmlonly @ref idigi_file_system_opendir @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_readdir()</td><td>Read a directory entry</td><td>@endhtmlonly @ref idigi_file_system_readdir @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_closedir()</td><td>Close a directory</td><td>@endhtmlonly @ref idigi_file_system_closedir @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_stat()</td><td>Get a file status</td><td>@endhtmlonly @ref idigi_file_system_stat @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_hash()</td><td>Get a file hash value</td><td>@endhtmlonly @ref idigi_file_system_hash @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_strerror()</td><td>Get an error string</td><td>@endhtmlonly @ref idigi_file_system_strerror @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_msg_error()</td><td>Inform of an error in messaging layer</td><td>@endhtmlonly @ref idigi_file_system_msg_error @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * The included python script can be used to create a file, send that file back to the iDigi Device Cloud 
 * and list the file. The file "test_file.txt" is created in the application directory and the file test is:
 * "iDigi file system sample\n".
 *
 * Please see @ref file_system_overview1 for sequence of callbacks for different requests, received from the 
 * iDigi Device Cloud.
 *
 * @section fs_sample_build Building
 *
 * To build this example for a Linux-based platform you can go into the directory
 * public/run/samples/file_system and type make.  If you are not running Linux you
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
 * <td>application.c</td>
 * <td>Contains application_run() and the application callback</td>
 * <td>samples/device_request</td>
 * </tr>
 * <tr>
 * <td>file_system.c</td>
 * <td>Application callbacks for file system</td>
 * <td>samples/file_system</td>
 * </tr>
 * <tr>
 * <td>file_system.py</td>
 * <td>Python script to write, read, and list a file</td>
 * <td>samples/file_system</td>
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
 * @subsection include_paths Include Paths
 *
 * The following include paths are required:
 *
 * @li public/include
 * @li run/platforms/linux (substitute with your platform)
 *
 * @subsection defines Defines
 *
 * The following defines are used in this sample:
 *
 * @li -DIDIGI_VERSION=0x1010000UL (indicates version 1.1 of the IIK)
 *
 * @section python_script Python script to write, read, and list a file
 *
 * This sample provides a simple python script to write, read, and list a file, 
 * using @htmlonly <a href="web_services.html">iDigi Web Services</a> @endhtmlonly.
 * It sends a request to developer.idigi.com.
 *
 * Run the python script in the sample. 
 *
 * @code
 * python file_system.py <username> <password> <device_id>
 * @endcode
 *
 * Output from this sample looks like:
 *
 * @code
 *
 * Open test_file.txt, 577, returned 4
 * lseek fd 4, offset 0, origin 2 returned 0
 * lseek fd 4, offset 0, origin 0 returned 0
 * write 4, 25, returned 25
 * close 4 returned 0
 * Open test_file.txt, 0, returned 4
 * lseek fd 4, offset 0, origin 2 returned 25
 * lseek fd 4, offset 0, origin 0 returned 0
 * read 4, 496, returned 25
 * read 4, 471, returned 0
 * close 4 returned 0
 * stat for test_file.txt returned 0, filesize 25
 *
 * @endcode
 *
 * Output from the file_system.py looks like:
 *
 * @code
 *
 * Response:
 * <sci_reply version="1.0">
 *    <file_system>
 *       <device id="00000000-00000000-00000000-00000000">
 *          <commands>
 *             <put_file/>
 *             <get_file><data>aURpZ2kgZmlsZSBzeXN0ZW0gc2FtcGxlCg==</data></get_file>
 *             <ls hash="none"><file path="test_file.txt" last_modified="1334009380" size="25"/></ls>
 *          </commands>
 *       </device>
 *    </file_system>
 * </sci_reply>
 *
 * @endcode
 *
 */
