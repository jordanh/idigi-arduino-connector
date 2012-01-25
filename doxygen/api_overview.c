/*! @page api_overview Public API Overview
 *
 * @htmlinclude nav.html
 *
 * @section api1_overview Public API Overview
 *
 * The IIK API includes two major software interfaces:
 *      @li IIK function calls
 *      @li IIK application-defined callback
 * 
 *  The functions available in the IIK are listed below:
  * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Routine</td>
 * <th class="title">Description</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly idigi_init() @htmlonly</th>
 * <td>Start the IIK</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly idigi_step() @htmlonly</th>
 * <td>Execute the IIK and return</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly idigi_run() @htmlonly</th>
 * <td>Start the IIK and do not return</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly idigi_initiate_action() @htmlonly</th>
 * <td>Begin an IIK operation, like starting a data transfer</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * 
 * @subsection initialization Initialization: idigi_init() 
 * 
 * The function idigi_init() is called once at startup to  initialize the IIK.
 * The application-defined callback function is passed as an argument; the @ref idigi_callback_t 
 * "application-defined callback" is used by the IIK to communicate with the application. 
 * The idigi_init() function must be called before all other IIK APIs.
 *
 * @subsection single_threaded Single-threaded model: idigi_step() 
 * 
 * The function idigi_step() is called to perform a sequence of operations or events 
 * and then returns control back to the caller.  This allows a caller to perform other tasks 
 * within a single thread or round robin based control loop.  A caller must call
 * this API repeatedly to continue IIK operations.
 *
 * The idigi_step() performs the following operations:
 *  -# Establish and maintain a connection with the iDigi Developer Cloud.
 *  -# Wait for incoming messages from the iDigi Developer Cloud.
 *  -# Send outstanding messages to the iDigi Developer Cloud.
 *
 * @subsection multi_threaded Multi-threaded model: idigi_run() 
 *
 * The function idigi_run() is similar to idigi_step() except it blocks and doesn't return control back to 
 * caller unless the IIK encounters an error.  This function is typically executed as a separate thread.
 * This is the recommended method of running the IIK within in a multithreaded
 * environment.
 *
 * @note To relinquish control of the CPU in a multithreaded environment the IIK
 * calls the receive (@ref app_network_receive) callback with a timeout or will call
 * OS sleep (@ref app_os_sleep).
 *
 * @subsection initiate_action Initiate an action: idigi_initiate_action() 
 *
 * The function idigi_initiate_action() is used to indicate to the IIK to perform a 
 * device-requested action.  The currently supported actions are:
 *
 *  @li Initiate a data transfer to the server, described in the data service API
 *  @li Terminate the IIK
 *
 * @subsection iik_callback Application Callback
 * 
 * The IIK uses a single application-defined @ref idigi_callback_t "callback" function to process network events, 
 * handle operating system (OS) calls, return device configuration data, and process firmware updates. 
 * The callback is also used to report error status and event notification.  The application-defined
 * @ref idigi_callback_t "callback" is passed into idigi_init() upon startup.  When the IIK needs to allocate
 * memory, access the network or perform other actions the application defined callback
 * is called.
 *
 * The IIK will invoke the application-defined callback to perform the following actions:
 *   @li Obtain configuration information
 *   @li OS calls
 *   @li Networking calls
 *   @li Notify the application of critical events and status
 *   @li Firmware download
 *   @li Handle a device request
 *   @li Request more data when sending data to the iDigi Developer Cloud
 *
 * @b Class @b ID and @b Request @b ID
 * enumeration values are used to identify the callback action. The application must perform and return
 * the appropriate action and return data, according to the class and request 
 * enumeration values. 
 *
 * Each callback is passed in a Class ID which is defined by the enumerated type
 * @ref idigi_class_t.  For each Class ID there is a set of Request ID's, for example the
 * @ref idigi_class_operating_system the corresponding Request ID's are defined
 * by the enum @ref idigi_os_request_t.
 *
 * The parameters for the application-defined callback: request_data, request_length, 
 * response_data and response_length are defined by the corresponding class_id and 
 * the request_id. There is a section in the documentation for each class of callbacks.
 * For example, the Operating System callbacks are described on the @ref os_callbacks
 * "Operating System" page.
 *
 *
 *
 */
