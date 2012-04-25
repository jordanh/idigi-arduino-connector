/*! @page file_system_sample File System Sample
 *
 * @htmlinclude nav.html
 *
 * @section fs_sample_overview Overview
 *
 * This sample demonstrates how to read and write files and list files or directories on your device, 
 * using the @ref file_system_overview1 "file system API".
 *
 * @section fs_sample_description Code Flow
 *
 * The routine main() in the platform directory initializes the IIK and registers the application callback. 
 * The application defined callback app_idigi_callback() in the application.c file 
 * calls app_file_system_handler() when a file system request is received. The app_file_system_handler() callback 
 * is defined in the file_system.c file. 
 *
 * All file system requests are initiated by the iDigi Device Cloud. 
 * 
 * The app_file_system_handler() routine calls the following routines to perform different file I/O operations: 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Routine</td>
 * <th class="title">Operation</td>
 * <th class="title">Request Id</td>
 * </tr> 
 * <tr> 
 * <td>app_process_file_open()</td><td>@endhtmlonly @ref file_system_open @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_open @htmlonly</td>
 * </tr> 
 * <tr> 
 * <td>app_process_file_lseek()</td><td>@endhtmlonly @ref file_system_lseek @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_lseek @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_read()</td><td>@endhtmlonly @ref file_system_read @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_read @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_write()</td><td>@endhtmlonly @ref file_system_write @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_write @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_ftruncate()</td><td>@endhtmlonly @ref file_system_ftruncate @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_ftruncate @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_close()</td><td>@endhtmlonly @ref file_system_close @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_close @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_rm()</td><td>@endhtmlonly @ref file_system_rm @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_rm @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_opendir()</td><td>@endhtmlonly @ref file_system_opendir @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_opendir @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_readdir()</td><td>@endhtmlonly @ref file_system_readdir @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_readdir @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_closedir()</td><td>@endhtmlonly @ref file_system_closedir @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_closedir @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_stat()</td><td>@endhtmlonly @ref file_system_stat @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_stat @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_hash()</td><td>@endhtmlonly @ref file_system_hash @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_hash @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_strerror()</td><td>@endhtmlonly @ref file_system_strerror @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_strerror @htmlonly</td>
 * </tr>
 * <tr> 
 * <td>app_process_file_msg_error()</td><td>@endhtmlonly @ref file_system_msg_error @htmlonly</td><td>@endhtmlonly @ref idigi_file_system_msg_error @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Please read @ref file_system_overview1 "file system overview" about the typical callback sequences for different 
 * file system requests, received from the iDigi Device Cloud.
 *
 * @section fs_sample_ls Listing a Directory
 *
 * The sample uses the reentrant readdir_r() function in the @ref file_system_readdir "app_process_file_readdir()" callback. 
 * It uses the application defined app_dir_data_t structure to store a directory entry and a pointer to a directory stream.
 *
 * @code
 *
 * typedef struct
 * {
 *     DIR * dirp;
 *     struct dirent dir_entry;
 * 
 * } app_dir_data_t;
 *
 * @endcode
 *
 * The sample allocates this structure in the @ref file_system_opendir "app_process_file_opendir()" callback after the successful 
 * opendir() call and returns the pointer to app_dir_data_t structure in the handle field of the @ref idigi_file_open_response_t 
 * "response_data" argument. 
 * 
 * This handle is provided in the @ref idigi_file_request_t "request_data" argument of the @ref file_system_readdir 
 * "app_process_file_readdir()" callback, where the dirp and dir_entry are used in the readdir_r() call.
 *
 * The same handle is provided in the @ref idigi_file_request_t "request_data" argument of the
 * @ref file_system_closedir "app_process_file_closedir()" callback, where the dirp is used in the closedir() call.
 *
 * The @ref file_system_closedir "app_process_file_closedir()" callback frees the app_dir_data_t memory.
 * 
 * @section fs_sample_hash_support File Hash Values Support
 *
 * By default the file system sample does not support file hash values. Md5 support can be enabled using the following define:
 * 
 * -DAPP_ENABLE_MD5=true
 *
 * When Md5 support is enabled the sample uses the uses MD5_Init(), MD5_Update(), and MD5_Final() functions 
 * from the openssl library. 
 *
 * The sample allocates memory to store the hash context, the file descriptor, and the file data buffer. It stores 
 * the pointer to this memory in the user_context field, provided to all application callbacks in the response_data 
 * structure.
 *
 * @code
 *
 * typedef struct
 * {
 *     MD5_CTX md5;
 *     char buf[APP_MD5_BUFFER_SIZE];
 *     unsigned int flags; // was ls issued for a single file or for a directory
 *     int fd;
 *
 * } app_md5_ctx;
 *
 * @endcode
 *  
 * To calculate a file hash value for a single file:
 *  -# @ref file_system_stat "app_process_file_stat()" callback allocates memory for the app_md5_ctx structue.
 *  -# @ref file_system_hash "app_process_file_hash()" callback:
 *      -# opens a file,
 *      -# reads file data and calculates the hash value,
 *      -# closes the file,
 *      -# frees the app_md5_ctx memory.
 *
 * To calculate a file hash values for directory entries:
 *  -# The initial @ref file_system_stat "app_process_file_stat()" callback, called with the directory path, allocates memory
 *          for the app_md5_ctx structue.
 *  -# @ref file_system_hash "app_process_file_hash()" callback for each regular file in the directory: 
 *      -# opens a file,
 *      -# reads file data and calculates the hash value,
 *      -# closes the file.
 *  -# @ref file_system_closedir "app_process_file_closedir()" callback frees the the app_md5_ctx memory.
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
 * The file "test_file.txt" is created in the current directory. The file data is: "iDigi file system sample\n".
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
 * write 4, 25, returned 25
 * close 4 returned 0
 * Open test_file.txt, 0, returned 4
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
