/*! @page api_overview API Overview
 *
 * @htmlinclude nav.html
 *
 * @section api1_overview API Overview
 *
 * The iDigi connector API includes two major software interfaces:
 *      @li iDigi connector function calls
 *      @li iDigi connector application-defined callback
 * 
 *  The functions available in the iDigi connector are listed below:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Routine</th>
 * <th class="title">Description</th>
 * </tr>
 * <tr>
 * <td>@endhtmlonly idigi_connector_start() @htmlonly</td>
 * <td>Start the iDigi connector</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly idigi_send_data() @htmlonly</td>
 * <td>Send data to the iDigi Device Cloud</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly idigi_register_device_request_callbacks() @htmlonly</td>
 * <td>Registers the device request callbacks</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * 
 * @subsection initialization Initialization: idigi_connector_start() 
 * 
 * The function idigi_connector_start() is called once at startup to initialize the iDigi connector.
 * The status callback function is passed as an argument; the @ref idigi_status_callback_t 
 * "status_callback" is used by the iDigi connector to communicate with the application. 
 * The idigi_connector_start() function must be called before all other iDigi connector APIs.
 *
 * @subsection send_data Send Data: idigi_send_data() 
 * 
 * The function idigi_send_data() is used to write data to a file on the iDigi Server.
 *
 * @subsection register_device_request_callbacks Register Device Request Callbacks: idigi_register_device_request_callbacks() 
 *
 * The function idigi_register_device_request_callbacks() must be called in order to receive device requests from the iDigi
 * Device Cloud.  You must register a device_request_callback and a device_response_callback.
 *
 * @htmlinclude terminate.html
 *
 */
