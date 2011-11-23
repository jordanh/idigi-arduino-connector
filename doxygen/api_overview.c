/*! @page api_overview API Overview
 *
 * @htmlinclude nav.html
 *
 * @section api1_overview API Overview
 *
 * The IIK uses a single application-defined @ref idigi_callback_t "callback" function to process network events, 
 * handle operating system (OS) calls, return device configuration data, and process firmware updates. 
 * The callback is also used to report error status and event notification.  The application defined 
 * callback is passed into idigi_init() upon startup.  When the IIK needs to allocate 
 * memory, access the network or perform other actions the application defined callback
 * is called.
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
 * <td>Tell the IIK to perform some action such as start a data transfer</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * The IIK will invoke the application defined callback to perform the following actions:
 *   @li Obtain configuration information
 *   @li OS calls
 *   @li Networking calls
 *   @li Notify the application of critical events and status
 *   @li Firmware download
 *   @li Send a device request
 *   @li Request more data when sending data to the server
 * 
 * @subsection initialization Initialization
 * 
 * The API idigi_init() is called once upon startup to  initialize the IIK. 
 * It takes the application defined callback function as an argument; this @ref idigi_callback_t 
 * "callback" is used by the IIK to communicate with the application. 
 * This function must be called before all other IIK APIs.
 *
 * @subsection single_threaded Single-threaded model
 * 
 * The function idigi_step() is called to perform a sequence of operations or events and returns control back to the caller. 
 * This allows a caller to perform other tasks in single-threaded system. A caller must call this API again to continue IIK operations. 
 * The connection is already terminated when idigi_step returns an error, idigi_step will try reconnecting to the iDigi Device Cloud if it's called again.
 * The idigi_step() performs the following operations:
 *  -# Establish a connection with the iDigi Device Cloud.
 *  -# Wait for incoming messages from the iDigi Device Cloud.
 *  -# Send outstanding messages to the iDigi Device Cloud.
 *
 * @subsection multi_threaded Multi-threaded model
 *
 * The function idigi_run() is similar to idigi_step() except it doesn't return control back to 
 * caller unless IIK encounters an error. This function should be executed as a separated thread.
 * This is the suggested method of running the IIK when running in a multithreaded
 * environment.
 *
 * @note To relinquish contol of the CPU in a multithreaded envirornment the IIK
 * call the receive (@ref idigi_class_network) callback with a timeout or will call
 * OS sleep (@ref idigi_class_operating_system).
 *
 * @subsection iik_callback Application Callback
 * 
 * A single callback @ref idigi_callback_t "idigi_callback_t" is used to interface between an application and the IIK. Class and request 
 * enumeration values are used to identify the callback action. The application must perform 
 * the appropriate action and return the corresponding data according to the class and request 
 * enumeration values. 
 *
 * Each callback is passed in a class ID which is defined in the enumerated type
 * @ref idigi_class_t.  For each class ID there is a set request ID, for example the
 * @ref idigi_class_operating_system the corresponding request id's are defined
 * by the enum idigi_os_request_t.
 *
 * The parameters request_data, request_length, response_data and response_length
 * are defined by the class_id and the request_id, refer to the page for
 * the API for detailed descriptions of the callback parameters.
 *
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 *
 */
