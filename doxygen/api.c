/*! @page api API
 *
 * @htmlinclude nav.html
 *
 * @section api_overview API Overview
 * The IIK uses a single application-defined @ref idigi_callback_t "callback" function to process socket events, 
 * handle operating system (OS) calls, return device configuration data, and process firmware updates. 
 * The callback is also used to report error status and event notification. The samples provided 
 * within the IIK show examples of how to use the API.  The file public/include/idigi_api.h contains the 
 * API defined in this chapter.
 *
 *The IIK will invoke the application defined callback in used to perform the following actions:
 *   @li Obtain configuration information
 *   @li Notify the application of critical events and status
 *   @li Firmware download
 *   @li Network calls
 *
 * 
 * @subsection initialization Initialization
 * 
 * The API idigi_init() is called once upon startup to allocate and initialize the IIK. 
 * It takes the application defined callback function as an argument; this @ref idigi_callback_t "callback" is used by the IIK to communicate with the application. 
 * This function must be called before all other IIK APIs.
 *
 * @subsection single_threaded Single-threaded model
 * 
 * The function idigi_step() is called to start and run the IIK. This function performs a sequence of operations or events and returns control back to the caller. 
 * This allows a caller to perform other tasks, especially in single-threaded system. A caller must call this API again to continue IIK operations. 
 * The connection is already terminated when idigi_step returns an error, idigi_step will try reconnecting to the iDigi Device Cloud if it's called again.
 * The idigi_step() performs the following operations:
 *  -# Establish a connection with the iDigi Device Cloud.
 *  -# Wait for incoming messages from the iDigi Device Cloud.
 *  -# Invoke and pass message to the appropriate process (such as firmware access facility).
 *
 * 
 * @subsection multi_threaded Multi-threaded model
 *
 * The function idigi_run() is similar to idigi_step() except it doesn't return control back to 
 * caller unless IIK encounters an error. This function should be executed as a separated thread.
 *
 * 
 * @subsection iik_callback Application Callback
 * 
 * A single callback @ref idigi_callback_t "idigi_callback_t" is used to interface between an application and the IIK. Class and request 
 * enumeration values are used to identify the callback action. The application must perform 
 * the appropriate action and return the corresponding data according to the class and request 
 * enumeration values. The callback actions are listed below:
 *
 *
 * @section os_interface OS Interface
 *
 * @subsection os_malloc Malloc
 *          Dynamically allocate memory
 *
 *          class_id idigi_class_operating_system class ID
 *          request_id idigi_os_malloc request ID
 *          request_data Pointer to number of bytes to be allocated
 *          request_length Specifies the size of *request_data which is size of size_t.
 *          response_data Callback returns a pointer to memory for allocated address
 *          response_length ignore
 *
 *          idigi_callback_continue Callback successfully allocated memory
 *          idigi_callback_abort Callback was unable to allocate memory and callback aborts IIK
 *          idigi_callback_busy Memory is not available at this time and needs to be called back again
 *          idigi_callback_t
 *          idigi_os_request_t
 * 
 *
 * @subsection os_get_system_time Get System Up Time
 *
 *  Return system up time in seconds. It is the time that a device has been up and running.
 *  class_id: idigi_class_operating_system class ID
 *  request_id idigi_os_system_up_time request ID
 *  request_data NULL
 *  request_length 0
 *   response_data Pointer to uint32_t integer memory where callback writes the system up time to (in seconds)
 *   response_length NULL Ignore
 *
 * idigi_callback_continue Callback returned system up time
 * idigi_callback_abort Callback was unable to get system time
 * idigi_callback_t
 * idigi_os_request_t
 * 
 *
 * @subsection os_free Free
 *  Free dynamically allocate memory
 *  class_id: idigi_class_operating_system class ID
 *  request_id idigi_os_malloc request ID
 *  request_data Pointer to number of bytes to be allocated
 *  request_length Specifies the size of *request_data which is size of size_t.
 *   response_data Callback returns a pointer to memory for allocated address
 *   response_length ignore
 *
 * idigi_callback_continue Callback successfully allocated memory
 * idigi_callback_abort Callback was unable to allocate memory and callback aborts IIK
 * idigi_callback_busy Memory is not available at this time and needs to be called back again
 * idigi_callback_t
 * idigi_os_request_t
 * 
 * @section firmware_access Firmware Access Facility
 *
 * The firmware access facility is an optional facility for applications to update their firmware. 
 * The IIK invokes the application defined callback to query whether this firmware access facility is supported or not.
 * The firmware access facility uses a target number to distinguish application dependant firmware images. 
 * For example, if a standalone bootloader is separate from an application image, or a default file system image.
 * Applications define an image to each target except target 0 (target 0 must be the firmware image that is running the IIK). 
 * Only one firmware upgrade can be in progress at any given time. The IIK will send a firmware target list to the iDigi 
 * Device Cloud to identify the number of target applications and the version number of each target.
 *
 * The firmware portion of the application defined callback is used for the following:
 *      @li To obtain the number of firmware downloadable images.
 *      @li To obtain firmware image information and descriptions.
 *      @li To process firmware image upgrades.
 *      @li To reset firmware images.
 *
 * A typical application defined callback sequence for downloading a firmware image would include:
 *      -# IIK calls application defined callback to return firmware information which includes the firmware version number, maximum size of an image, firmware description, and file name spec.
 *      -# IIK calls application defined callback to initiate firmware download.
 *      -# IIK calls application defined callback to receive firmware binary data blocks.
 *      -# Repeat step 3 until the entire firmware image data is completed.
 *      -# IIK calls application defined callback to complete firmware download.
 *      -# IIK calls application defined callback to reset and reboot the target to begin executing the new firmware.
 *      -# IIK calls application defined callback to disconnect the current connection.
 *
 * Applications may choose to begin writing their downloaded image to Flash either 
 * during the Firmware Binary or Firmware Download Complete message. This is an application level 
 * decision with clear tradeoffs (memory needs versus power loss protection). If you write during 
 * the Firmware Binary data, you might lose communication or power at some point during the data transfer. 
 * If protection is built in (i.e., back up images) this would not be problem. If protections are not built in, 
 * applications might choose to place the entire image contents into RAM before writing to Flash. 
 * Back up image protection would still protect against power loss after your write occurred, but the communication 
 * loss or corruption could be avoided by having a making a checksum test before starting your Flash write.
 * Lastly, no application defined callback can block IIK execution. This application layer design requirement 
 * is most sensitive when considering writing firmware.
 *
 *
 * @section data_facility Sending data to the idigi Device Cloud
 * 
 * The function idigi_initiate_action() is used to request IIK to perform an action. 
 * This is used to send data from the device to the iDigi Device Cloud and to 
 * terminate the IIK library.
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 *
 */

