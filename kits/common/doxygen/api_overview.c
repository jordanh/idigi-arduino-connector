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
 *  The functions available in the iDigi connector API are listed below, click on
 *  the name of the routine for more details.
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
 * The status callback function is passed as an argument.  The @ref idigi_status_callback_t 
 * "status_callback" is the callback used when there are asynchronous error messages from
 * the iDigi Device Cloud. After returning this callback, the application can restart the iDigi connector
 * by calling idigi_connector_start().  The idigi_connector_start() function must be called before all
 * other iDigi connector APIs.
 *
 * @subsection send_data Send Data: idigi_send_data() 
 * 
 * The function idigi_send_data() is used to write data to a file on the iDigi Server specified by parameters path, file-name,
 * content-type, data to be written to the file, length of the data and flags.  The flags are used to specify how the data
 * will be written to the file.  The flag definitions are listed below:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Flag</th>
 * <th class="title">Description</th>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FLAG_OVERWRITE_DATA @htmlonly</td>
 * <td>Tells the iDigi Device Cloud to overwrite the file with the new data</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FLAG_ARCHIVE_DATA @htmlonly</td>
 * <td>Tells the iDigi Device Cloud to keep a history of the changes to the file</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FLAG_APPEND_DATA @htmlonly</td>
 * <td>Tells the iDigi Device Cloud to append the new data to the existing file</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *  
 * @subsection register_device_request_callbacks Register Device Request Callbacks: idigi_register_device_request_callbacks() 
 *
 * The function idigi_register_device_request_callbacks() must be called in order to receive and to respond to device requests
 * from the iDigi Device Cloud.  You must register a device_request_callback and a device_response_callback.
 * 
 * Device requests are used to control and send messages to a device.  The iDigi Connector user can write a web services
 * application to send device requests to the device; the application running on the device then interprets the
 * incoming requests.
 *
 * @subsection device_request_callback Device Request Callback: device_request_callback() 
 *
 * The user defined function @ref idigi_device_request_callback_t "device_request_callback()" parses the device request received
 * from the iDigi Device Cloud to determine if the target and the data associated with that target is valid.  If both are valid,
 * the user defined function @ref idigi_device_request_callback_t "device_request_callback()" can turn on/off an Led, set the
 * state of a GPIO pin, or set the value of a variable.
 *
 * @subsection device_response_callback Device Response Callback: device_response_callback() 
 *
 * The user defined function @ref idigi_device_response_callback_t "device_response_callback()" checks the result of the @ref
 * idigi_device_request_callback_t "device_request_callback()", and sets up the response_data based on that result.  The
 * Application needs to check the error value in the response_data before returning the response.
 *
 * @htmlinclude terminate.html
 *
 */
