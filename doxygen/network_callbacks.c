
/*! @page network_callbacks Network Callbacks
 *
 * @htmlinclude nav.html
 *
 * @section net Network Callbacks
 * The IIK interafaces to the platforms network interface through the callbacks listed below, these
 * are called through the application callback described in the @ref api1_overview.
 * 
 *  -# @ref connect
 *  -# @ref send
 *  -# @ref receive
 *  -# @ref close
 *  -# @ref disconnected
 *  -# @ref reboot
 *
 * @section connect Connect
 *
 * Callback is called to establish connection between IIK and the iDigi Device Cloud. 
 * Callback is responsible to setup any socket options.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_network @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_network_connect @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer to server URL that callback will make connection to on @endhtmlonly @ref IDIGI_PORT @htmlonly for non-secure port number or @endhtmlonly @ref IDIGI_SSL_PORT @htmlonly for secure port number </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> Length of the server URL.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns the pointer to  @endhtmlonly @ref idigi_network_handle_t @htmlonly which is used throughout network socket callback calls. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of idigi_network_handle_t</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully established a connection</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to connect to the server and callback aborted IIK</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Callback is busy connecting and needs to be called back again</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section send Send
 *
 * Callback is called to send data to the iDigi Device Cloud. This function must 
 * not block. If it encounters EAGAIN or EWOULDBLOCK error, 0 bytes must be returned 
 * and IIK will continue calling this function. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_network @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_network_send @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer to @endhtmlonly idigi_write_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> Size of @endhtmlonly idigi_write_request_t. @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Pointer to size_t type memory where callback writes number of bytes sent to the iDigi server. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully sent data to server</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to send data to server and callback aborts IIK</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Callback is busy and needs to be called again to send data</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section receive Receive
 *
 * Callback is called to receive a specified number of bytes of data from the iDigi 
 * Device Cloud.  This function may block up to the timeout specified in the
 * idigi_read_request_t.
 *
 * @note In the multithreaded model (idigi_run()) this is the point where the IIK
 * will relinquish control of the CPU.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_network @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_network_receive @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer to @endhtmlonly idigi_read_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> Size of @endhtmlonly idigi_read_request_t. @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Pointer to size_t type memory where callback writes number of bytes received from the iDigi server. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully received number of requested bytes</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to receive data from server and callback aborts IIK</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Callback is busy and needs to be called again to receive data</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section close Close
 *
 * Callback is called to close a connection between a device and the iDigi Device server
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_network @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_network_close @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer to @endhtmlonly idigi_network_handle_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> Size of @endhtmlonly idigi_network_handle_t. @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> N/A </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully closed the connection</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to close the connection and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section disconnected Connection disconnected
 *
 * Callback is called when the iDigi server has terminated the connection. 
 * IIK calls this callback to notify that the iDigi server has disconnected 
 * the connection. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_network @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_network_disconnected @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> N/A </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>IIK will continue and establish the connection again</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>IIK will abort and return idigi_disconnected error status</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section reboot Reboot
 *
 * Callback is called when the iDigi server requests that the device reboots. 
 * The device should reboot and this callback should not return. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_network @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_network_reboot @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> N/A </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>No return</td>
 * <td>none</td>
 * </tr>
 * </table>
 * @endhtmlonly
 */
