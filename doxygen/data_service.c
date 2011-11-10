/*! @page data_service Data Service
 *
 * @htmlinclude nav.html
 *
 * @section data_service_overview Data Service Overview
 *
 * The data service is used to send data to and from a device to the iDigi
 * server, the data sent to the server can be stored on a file on the server.
 * The data service has two mechanisms one for device intiaited transfers and the
 * second for server initiated transfers.
 *
 * @section send_data Device initiated data transfers
 *
 * Device initiated transfers use an idigi_put_request to transfer data to the
 * server.
 *
 * There is one class ID for the data service: @ref idigi_class_data_service
 *
 * @subsection initiate_send Initiate Sending Data
 *
 * For a put request the application initiates the data transfer to the server.
 * The applicication initiates the transfer to the server by calling idigi_initiate_action()
 * with the request field equal to @ref idigi_initiate_data_service and request_data points to
 * a idigi_data_put_header_t structure.
 *
 * @subsection get_data Callback to get more data
 *
 * After calling idigi_initiate_action() the user will keep getting a callback until
 * the application sets a flag indicating there is no more data to be sent.
 *
 * The callback is called with the following information:
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_data_service @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_data_service_put_request @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>pointer to @endhtmlonly [IN] idigi_data_put_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>pointer to @endhtmlonly [IN/OUT] idigi_data_put_response_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * The idigi_data_put_request_t  indicates wether the server is requesting more
 * data, is sending data to the application (a response to our put request) or if
 * an error was encountered.
 *
 * The idigi_data_put_response_t has two diffenent meanings: if the idigi_data_put_request_t
 * indicates that the server is requesting data, then the user fills in this data
 * structure, if the server is indicating we have data then the application has
 * to copy the data to their space.
 * 
 * @section service_request  Server initiated data transfers
 *
 * The data service request is a initiated by the server.  
 * When the server sends a device request, the IIK calls the application-defined 
 * callback with the target name and request data.  It continues calling the application-defined 
 * callback for each chunk of request data until last chunk of request data. Then, 
 * the IIK calls the callback for response data for device response. The 
 * data service continues calling the callback for response data until it indicates 
 * last chunk of response data.
 *
 * The application callback is called with the following information:
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_data_service @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_data_service_put_request @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>pointer to @endhtmlonly idigi_data_service_device_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_data_service_device_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>pointer to @endhtmlonly idigi_data_service_device_response_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * The structure idigi_data_service_device_request_t contains a request_type
 *
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 */


