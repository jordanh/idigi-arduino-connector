/*! @page api_overview API Overview
 *
 * @htmlinclude nav.html
 *
 * @section api1_overview API Overview
 *
 * The iDigi connector API is used to send data to and from the iDigi Device Cloud.
 * Data transfers are either initiated from the iDigi Device Cloud or the device itself.
 * When data is sent to the iDigi Device Cloud it is sent as a file.  When data is
 * sent from the iDigi Device Cloud it is sent as a device request.  This device
 * request can be used to turn on/off an Led, set the state of a GPIO pin, or
 * set the value of a variable. It is the applications responsibility to parse
 * and evaluate the device request.
 * 
 *  The functions available in the iDigi connector API are listed below:
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
 * <tr>
 * <td>@endhtmlonly @ref idigi_device_request_callback_t "device_request_callback()" @htmlonly</td>
 * <td>Handles the device request callback</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_device_response_callback_t "device_response_callback()" @htmlonly</td>
 * <td>Prepares the response to the device request</td>
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
 * @subsection device_request_callback Device Request Callback: device_request_callback() 
 *
 * The user defined function @ref idigi_device_request_callback_t "device_request_callback()" parses the device request received
 * from the iDigi Device Cloud.
 *
 * @subsection device_response_callback Device Response Callback: device_response_callback() 
 *
 * The user defined function @ref idigi_device_response_callback_t "device_response_callback()" Prepares the response to the device
 * request to be sent to the iDigi Device Cloud.
 *
 * @htmlinclude terminate.html
 *
 */
