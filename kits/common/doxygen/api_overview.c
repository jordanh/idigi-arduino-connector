/*! @page api_overview API Overview
 *
 * @htmlinclude nav.html
 *
 * @section api1_overview API Overview
 *
 * The iDigi Connector API is used for communication between the client running on the
 * device and the iDigi Device Cloud.  The device can be controlled by the iDigi Device
 * Cloud via <a href="http://www.idigi.com/idigimanagerpro">iDigi Manager Pro</a> or by
 * using @ref web_services "iDigi Web Services".  When commands are received they
 * come in the form of an iDigi device request, for each request the device will be
 * asked to send back a response.  Device requests can be used to send commands to control a device.
 * This API is also used to push files onto the iDigi device cloud.
 * 
 * The API consists of the following routines, click on the name of the routine for more
 * details.
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
 * The function idigi_connector_start() is called during startup to initialize the iDigi connector.
 * The @ref idigi_status_callback_t "status callback function" is passed as an argument and is used to pass the application status messages from
 * the iDigi Device Cloud. The idigi_connector_start() function must be called before all
 * other iDigi Connector APIs.
 *
 * @subsection send_data Send Data: idigi_send_data() 
 * 
 * The function idigi_send_data() is used to write data to files on the iDigi Server.  The file is specified in the
 * path argument passed into this function.  The flags field is used to specify how the data
 * is written to the file.  The flag definitions are listed below:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Flag</th>
 * <th class="title">Description</th>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FLAG_OVERWRITE_DATA @htmlonly</td>
 * <td>Overwrite the file with the new data</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FLAG_ARCHIVE_DATA @htmlonly</td>
 * <td>Keep a history of the changes to the file</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FLAG_APPEND_DATA @htmlonly</td>
 * <td>Append new data to the end of the file</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *  
 * @subsection register_device_request_callbacks Register Device Request Callbacks: idigi_register_device_request_callbacks() 
 *
 * The function idigi_register_device_request_callbacks() is called to register routines which are called when
 * device requests are received.  The device_request_callback() is called to indicate a request has been received
 * and then the device_response_callback() is called to retrieve a response to the request.
 * 
 * Device requests are used to send messages to a device.  The iDigi Connector user can write a web services
 * application to send device requests to the device; the application running on the device then interprets the
 * incoming device requests.
 *
 * @subsection device_request_callback Device Request Callback: device_request_callback() 
 *
 * The user defined function @ref idigi_device_request_callback_t "device_request_callback()" parses the device request received
 * from the iDigi Device Cloud to determine if the target and the data associated with that target is valid.  If both are valid,
 * the user defined function @ref idigi_device_request_callback_t "device_request_callback()" then parses the request
 * received.
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
