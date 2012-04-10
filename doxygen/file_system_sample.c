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
 * a file system request app_idigi_callback() calls app_file_system_handler() to perform file I/O functions.
 *
 * The app_file_system_handler() callback and routines it calls to perform file I/O functions are defined in
 * the file_system.c file.
 * 
 * The included python script can be used to create a file, send that file back to the iDigi Device Cloud 
 * and list the file. The file "test_file.txt" is created in the application directory and the file test is:
 * "iDigi file system sample\n".
 *
 * When the iDigi Cloud sends the request to get a file, the following routines are called:
 * -# app_process_file_open() to open a file.
 * -# app_process_file_lseek() to set file position.
 * -# app_process_file_read() called repeatedly to read file data.
 * -# app_process_file_close() to close the file.
 *
 * IIK calls app_process_file_lseek() twice: to check if the position is valid and to set the correct position. 
 *
 * This sample does not allocate memory and does not use user_context when it processes a request to get a file, 
 * but it it was, it would need to free all memory in @ref idigi_file_system_close callback.
 *
 * When the iDigi Cloud sends the request to put a file, the following routines are called:
 * -# app_process_file_open() to open a file.
 * -# app_process_file_lseek() to set file position.
 * -# app_process_file_write() called repeatedly to write file data.
 * -# app_process_file_ftruncate() to truncate a file, if was writing to existing file and truncate was requested.
 * -# app_process_file_close() to close the file.
 *
 * IIIK calls app_process_file_lseek() twice: to check if the position is valid and to set the correct position. 
 *
 * This sample does not allocate memory and does not use user_context when it processes a request to put a file, 
 * but it it was, it would need to free all memory in app_process_file_close() routine.
 *
 * When the iDigi Cloud sends the request to remove a file, app_process_file_rm() is called.
 *
 * When the iDigi Cloud sends the request to get a file listing, the following routines are called:
 * -# app_process_file_stat() to get a file status.
 * -# app_process_file_hash() to get a file hash value, if requested and supported. 
 *
 * To request the file hash value IIK sets hash_alg in @ref idigi_file_stat_request_t. 
 * The app_process_file_stat() callback sets hash_alg in the @ref idigi_file_stat_t structure of the response 
 * to the actual supported value.
 *
 * This sample does not support file hash values.
 *
 * If the application needs more than one pass to calculate the hash value, it can keep returning
 * @ref idigi_callback_busy and the callback will be repeated until it is ready to return the hash value. 
 * It might open a file, allocate memory for intermidiate data and store in the user_context of 
 * the response structure. When the callback has finished the hash calculations it must close the file, 
 * free allocated memory and return @ref idigi_callback_continue. 
 *
 * When the iDigi Cloud sends the request to get a directory listing, the following routines are called:
 * -# app_process_file_stat() to get a directory status.
 * -# app_process_file_opendir() to open a directory.
 * -# For each directory entry:
 * -# - app_process_file_readdir() to read the directory entry name.
 * -# - app_process_file_stat() to get entry status.
 * -# - app_process_file_hash() to get a hash value for a regular file, if the hash value is
 * requested and supported for the whole directory.
 * -# app_process_file_closedir() to close the directory, when all directory entries are processed.
 *
 * If a file I/O error occurs, the callback should set errnum in the @ref idigi_file_error_data_t of the response
 * to the system errno value and set a error_status to an error value of @ref idigi_file_error_status_t type.
 * This action will follow with:
 * -# app_process_file_close() or app_process_file_closedir(), if required. 
 * -# app_process_file_strerror() to get an error string description based on the storred errno.
 * -# Sending an error response to the iDigi Device Cloud, which completes the session.
 *
 * If a callback returns @ref idigi_file_user_cancel the session will be calceled without sending an error response
 * to the iDigi Device Cloud. Callbacks to close a file or directory will be invoked, if required.
 *
 * If the callback sets an error status when part of the response was already sent to the iDigi Device Cloud, 
 * the error status is treated as @ref idigi_file_user_cancel.
 *
 * If an error has occured in messaging layer, the following routines.are called, prior to cancelling the session: 
 * -# app_process_file_close() or app_process_file_closedir(), if required. 
 * -# app_process_file_msg_error() to inform of an error.  
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
 * using @htmlonly <a href="web_services.html">iDigi Web Services.</a> @endhtmlonly.
 * It sends a request to developer.idigi.com.
 *
 * Run the python script in the sample. 
 *
 * @code
 * python file_system.py <username> <password> <device_id>
 * @endcode
 *
 * Output from this sample is similar to:
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
 * Output from the file_system.py is similar to:
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
