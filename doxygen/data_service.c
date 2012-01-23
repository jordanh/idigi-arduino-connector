/*! @page data_service Data Service
 *
 * @htmlinclude nav.html
 *
 * @section data_service_overview Data Service Overview
 *
 * The Data Service API is used to send data to and from the iDigi Device Cloud.  Data service transfers
 * are either initiated from the iDigi Device Cloud or the device itself.  The two types of requests are
 * listed below:
 * 
 * @li @ref put_request : Data transfers initiated by the device and used to
 * write files on to the iDigi Device Cloud.  The iDigi Device Cloud may send a status response back
 * indicating the transfer was successful. 
 * @li @ref device_request : Transfers initiated from a web services client connected to the iDigi Device Cloud
 * to the device.  This transfer is used to send data to the device and the device may send a response back.
 *
 * @note See @ref data_service_support under Configuration to disable this data service.
 *
 * @section put_request Put Request
 *
 * @subsection initiate_send Initiate Sending Data
 *
 * The application initiates the Put Request to the iDigi Device Cloud by calling idigi_initiate_action()
 * with @ref idigi_initiate_data_service request and @ref idigi_data_service_put_request_t request_data.
 *
 * The idigi_initiate_action() is called with the following arguments:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>handle</th>
 * <td>@endhtmlonly @ref idigi_handle_t @htmlonly returned from the @endhtmlonly idigi_init() @htmlonly function.</td>
 * </tr>
 * <tr>
 * <th>request</th>
 * <td>@endhtmlonly @ref idigi_initiate_data_service @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> Pointer to @endhtmlonly idigi_data_service_put_request_t @htmlonly structure:</br>
 *      </br> - <b><i>path</i></b> - file path to store the data on device cloud
 *      </br> - <b><i>content_type</i></b> - "text/plain", "text/xml", "application/json", etc
 *      </br> - <b><i>flags</i></b> - @endhtmlonly @ref put_flags @htmlonly
 *      </br> - <b><i>context</i></b> - can be used to identify the Put Request session
 * </td>
 * </tr>
 * <tr>
 * <th>response_data </th>
 * <td> NULL </td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * An example of initiating a device transfer is shown below:
 * @code
 *   static idigi_data_service_put_request_t header;
 *   static char file_path[] = "testdir/testfile.txt";
 *   static char file_type[] = "text/plain";
 *
 *   header.flags = IDIGI_DATA_PUT_APPEND;
 *   header.path  = file_path;
 *   header.content_type = file_type;
 *   header.context = &header;
 *
 *   // Begin a file transfer to the iDigi Cloud
 *   status = idigi_initiate_action(handle, idigi_initiate_data_service, &header, NULL);
 * @endcode
 *
 * This example will invoke the IIK to initiate a data transfer to the iDigi Device 
 * Cloud, the result of this operation creates a file testfile.txt in the testdir directory
 * on the iDigi Device Cloud.  Once the iDigi Device Cloud is ready to receive data 
 * from the device the application callback is called requesting data.
 *
 * @subsection get_data Put Request Callback
 *
 * After calling idigi_initiate_action(), the IIK will make @ref idigi_data_service_put_request "Put Request"
 * @ref idigi_callback_t "callbacks" to retrieve the application data. These callbacks will continue 
 * until the transfer is complete or an error is encountered. @ref IDIGI_MSG_LAST_DATA flag in the client_data 
 * field indicates the last chunk of data.
 *
 * The data to the server (@ref idigi_data_service_type_need_data message type) is sent in the 
 * idigi_data_service_msg_response_t field. 
 * 
 * The response from the server (@ref idigi_data_service_type_have_data message type) is received in the 
 * idigi_data_service_msg_request_t field. Typically the response consists of 
 * the success or error status code and text of the data service session. 
 * 
 * A callback with @ref idigi_data_service_type_error message type is received when message processing error occurs. 
 * This aborts the data service session. 
 *
 * The @ref idigi_data_service_put_request "Put Request"  @ref idigi_callback_t "callback" is called with the following information:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_data_service @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_data_service_put_request @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly idigi_data_service_msg_request_t @htmlonly structure:
 *   <p><b><i>service_context</i></b> </br> - Was set in the request_data argument in a call to
 *      @endhtmlonly @ref idigi_initiate_data_service @htmlonly
 *   <p><b><i>message_type</i></b> 
 *      </br> - @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly - request to send more data to the server. 
 *      </br> - @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly - response from the server.
 *      </br> - @endhtmlonly @ref idigi_data_service_type_error @htmlonly - message processing error. 
 *   <p><b><i>server_data</i></b> for @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly
 *      </br> - Not used.
 *   <p><b><i>server_data</i></b> for @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly points to @endhtmlonly idigi_data_service_block_t @htmlonly structure:
 *      </br> - <b><i>data</i></b> - might contain response from the server.
 *      </br> - <b><i>length_in_bytes</i></b> - size of response from the server.
 *      </br> - <b><i>flags</i></b> - @endhtmlonly @ref data_service_flags @htmlonly: success or error code from the server.
 *   <p><b><i>server_data</i></b> for @endhtmlonly @ref idigi_data_service_type_error @htmlonly points to @endhtmlonly idigi_data_service_block_t @htmlonly structure:
 *      </br> - <b><i>data</i></b> - message processing error code of @endhtmlonly @ref idigi_msg_error_t @htmlonly type.</p>
 * </td></tr>
 * <tr>
 * <th>request_length</th>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> 
 *      </br> - Not used.
 *   <p><b><i>message_status</i></b> 
 *      </br> - Status of @endhtmlonly @ref idigi_msg_error_t @htmlonly type to send to the server on return.
 *   <p><b><i>client_data</i></b> points to @endhtmlonly idigi_data_service_block_t @htmlonly structure:
 *      </br> - <b><i>data</i></b> - pointer to copy data to send to the server.
 *      </br> - <b><i>length_in_bytes</i></b> - size of data buffer on input, actual data size on output.
 *      </br> - <b><i>flags</i></b> - @endhtmlonly @ref data_service_flags @htmlonly to mark first or last chunk of data.
 * </td></tr>
 * <tr>
 * <th>response_length</th>
 * <td>[OUT] Size of @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_continue @htmlonly</th>
 * <td>Continue</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_abort @htmlonly</th>
 * <td>Aborts IIK</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_busy @htmlonly</th>
 * <td>Busy and needs to be called back again</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * An example of an application callback for a put request is shown below:
 *
 * @code
 * idigi_callback_status_t app_data_service_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                                  void const * request_data, size_t const request_length,
 *                                                  void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_data_service_msg_request_t const * const put_request = request_data;
 *    idigi_data_service_msg_response_t * const put_response = response_data;
 *
 *
 *     if (class_id == idigi_class_data_service && request_id.data_service_request == idigi_data_service_device_request) 
 *    {
 *        switch (put_request->message_type)
 *        {
 *        case idigi_data_service_type_need_data: // iDigi Device Cloud requesting data
 *            {
 *                idigi_data_service_block_t * message = put_response->client_data;
 *                char * dptr = message->data;
 *                char buffer[BUFFER_SIZE];
 *
 *                snprintf(buffer, BUFFER_SIZE, "iDigi data service sample [%d]\n", sample_number);
 *                size_t const bytes = strlen(buffer);
 *
 *                // Copy to the IIK's buffer 
 *                memcpy(dptr, buffer, bytes);
 *                message->length_in_bytes = bytes;
 *                message->flags = IDIGI_MSG_LAST_DATA | IDIGI_MSG_FIRST_DATA;
 *                put_response->message_status = idigi_msg_error_none;
 *                    sample_number++;
 *            }
 *            break;
 *
 *        case idigi_data_service_type_have_data: // Response from the iDigi Device Cloud
 *            {
 *                idigi_data_service_block_t * message = put_request->server_data;
 *                uint8_t const * data = message->data;
 *
 *                APP_DEBUG("Received %s response from server\n", 
 *                          ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
 *                if (message->length_in_bytes > 0) 
 *                {
 *                    APP_DEBUG("Server response %s\n", (char *)data);
 *                }
 *            }
 *            break;
 *
 *        case idigi_data_service_type_error: // The iDigi Device Cloud sent back an error
 *            {
 *                idigi_data_service_block_t * message = put_request->server_data;
 *                idigi_msg_error_t const * const error_value = message->data;
 *
 *                APP_DEBUG("Data service error: %d\n", *error_value);
 *            }
 *            break;
 *
 *        default:            
 *            APP_DEBUG("Unexpected command: %d\n", request);
 *            break;
 *        }
 *    }
 *    else
 *    {
 *        APP_DEBUG("Request not supported in this sample: %d\n", request);
 *    }
 *
 * done:
 *    return status;
 * }
 * @endcode
 * 
 * @section device_request Device Request
 *
 * Device requests are data transfers initiated by the server. They are used to
 * send data from the server to the device, the device may send back a reply.
 * The user will receive the application callback when a device request is received from the server.
 * The application callback is continually called with the data from the server
 * until the transfer is complete or an error is encountered.
 *
 * The data from the server (@ref idigi_data_service_type_have_data message type) is in the 
 * idigi_data_service_msg_request_t field, there is either data or an error. 
 * 
 * If the application needs to send back a response (@ref idigi_data_service_type_need_data message type),
 * the application fills in the idigi_data_service_msg_response_t.
 *
 *
 * The @ref idigi_data_service_device_request "Device Request" @ref idigi_callback_t "callback" 
 * is called with the following information: 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_data_service @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_data_service_device_request @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly idigi_data_service_msg_request_t @htmlonly structure:
 *   <p><b><i>service_context</i></b> </br> - pointer to @endhtmlonly @ref idigi_data_service_device_request_t @htmlonly.
 *   <p><b><i>message_type</i></b> 
 *      </br> - @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly - data received from the server.
 *      </br> - @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly - need to send a response to the server. 
 *      </br> - @endhtmlonly @ref idigi_data_service_type_error @htmlonly - message processing error.
 *   <p><b><i>server_data</i></b> for @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly
 *      </br> - Not used.
 *   <p><b><i>server_data</i></b> for @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly points to @endhtmlonly idigi_data_service_block_t @htmlonly structure: 
 *      </br> - <b><i>data</i></b> - pointer to data chunk, received from the server.
 *      </br> - <b><i>length_in_bytes</i></b> - size of received data chunk.
 *      </br> - <b><i>flags</i></b> - @endhtmlonly @ref data_service_flags @htmlonly marking first or last chunk of data. 
 *   <p><b><i>server_data</i></b> for @endhtmlonly @ref idigi_data_service_type_error @htmlonly points to @endhtmlonly idigi_data_service_block_t @htmlonly structure:
 *      </br> - <b><i>data</i></b> - message processing error code of @endhtmlonly @ref idigi_msg_error_t @htmlonly type.</p>
 * </td></tr>
 * <tr>
 * <th>request_length</th>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b>
 *      </br> - Set by a user to be returned on subsequent callbacks. 
 *   <p><b><i>message_status</i></b>
 *      </br> - If an application has encountered an error while processing the callback it can set a status to  
 *              @endhtmlonly @ref idigi_msg_error_t @htmlonly send an error code back to the server, 
 *   <p><b><i>client_data</i></b> points to @endhtmlonly idigi_data_service_block_t @htmlonly structure:
 *      </br> - <b><i>data</i></b> - pointer to copy data to send in response to the server.
 *      </br> - <b><i>length_in_bytes</i></b> - size of data buffer on input, actual data size on output.
 *      </br> - <b><i>flags</i></b> - @endhtmlonly @ref data_service_flags @htmlonly to mark first or last chunk 
 *            of data or to indicate that the callback did not process the message.</p>
 * </td></tr>
 * <tr>
 * <th>response_length</th>
 * <td>[OUT] Size of @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_continue @htmlonly</th>
 * <td>Continue</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_abort @htmlonly</th>
 * <td>Aborts IIK</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_busy @htmlonly</th>
 * <td>Busy and needs to be called back again</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * User uses SCI request to send device request to the server and server sends it
 * to the device.
 * An example of an application callback for a device request is show below:
 *
 * @code
 *
 *
 *
 * idigi_callback_status_t app_data_service_callback(idigi_class_t const request_id,
 *                                                   idigi_request_t const request_id,
 *                                                   void const * request_data, size_t const request_length,
 *                                                   void * response_data, size_t * const response_length)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     idigi_data_service_msg_request_t const * const service_device_request = request_data;
 *
 *     if (class_id == idigi_class_data_service && request_id.data_service_request == idigi_data_service_device_request) 
 *     {   // Device request from iDigi Device Cloud
 *         switch (service_device_request->message_type)
 *         {
 *         case idigi_data_service_type_have_data:
 *             status = process_device_request(request_data, response_data);
 *             break;
 *         case idigi_data_service_type_need_data:
 *             status = process_device_response(request_data, response_data);
 *             break;
 *         case idigi_data_service_type_error:
 *             status = process_device_error(request_data, response_data);
 *             break;
 *         default:
 *             break;
 *         }
 *     }
 *                               
 *     return status;
 * }
 *
 * // supported target name
 * static char const device_request_target[] = "myTarget";
 *
 * static char device_response_data[] = "My device response data";
 *
 * typedef struct device_request_handle {
 *   void * device_handle;
 *   char * response_data;
 *   char * target;
 *   size_t length_in_bytes;
 * } device_request_handle_t;
 *
 * static unsigned int device_request_active_count = 0;
 *
 * // An application function to process data from the server
 * extern void void handle_device_request_data(void * data, size_t length_in_bytes, unsigned int flags);
 *
 *  // Routines to process the device request, response, & error
 *  static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
 *                                                       idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_data_service_device_request_t * server_device_request = request_data->service_context;
 *     idigi_data_service_block_t * server_data = request_data->server_data;
 *     device_request_handle_t * client_device_request = response_data->user_context;
 * 
 *     if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
 *     {
 *         // target should not be null on 1st chunk of data
 *         if (strcmp(server_device_request->target, device_request_target) != 0)
 *         {   
 *             // unsupported target so let's cancel the session
 *             response_data->message_status = idigi_msg_error_cancel;
 *             return idigi_callback_continue;
 *         }
 *
 *         // 1st chunk of device request so let's initialize client_device_request
 *         client_device_request = malloc(sizeof *client_device_request);
 *         client_device_request->length_in_bytes = 0;
 *         client_device_request->response_data = NULL;
 *         client_device_request->device_handle = server_device_request->device_handle;
 *         client_device_request->target = (char *)device_request_target;
 *
 *         // setup the user_context for our device request data
 *         response_data->user_context = client_device_request;  
 *         device_request_active_count++;
 *     }
 * 
 *     // Process data from the server
 *     handle_device_request_data(server_data->data, server_data->length_in_bytes, server_data->flags);
 *
 *     if ((server_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
 *     {   
 *         // No more chunks, setup response data for this target
 *         client_device_request->response_data = device_response_data;
 *         client_device_request->length_in_bytes = strlen(client_device_request->response_data);
 *     }
 *
 *    return idigi_callback_continue;
 * }
 * 
 * static idigi_callback_status_t process_device_response(idigi_data_service_msg_request_t const * const request_data,
 *                                                        idigi_data_service_msg_response_t * const response_data)
 * {
 *     device_request_handle_t * const client_device_request = response_data->user_context;
 *     idigi_data_service_block_t * const client_data = response_data->client_data;
 *
 *     // get number of bytes going to be written to the client data buffer
 *     size_t const bytes = (client_device_request.length_in_bytes < client_data->length_in_bytes) ? 
 *                           client_device_request.length_in_bytes : client_data->length_in_bytes;
 * 
 *      // let's copy the response data to service_response buffer
 *      memcpy(client_data->data, client_device_request->response_data, bytes);
 *      client_device_request->response_data += bytes;
 *      client_device_request->length_in_bytes -= bytes;
 *
 *      client_data->length_in_bytes = bytes;
 *      client_data->flags = (client_device_request->length_in_bytes == 0) ? IDIGI_MSG_LAST_DATA : 0;
 *      if (client_device_request->target == NULL)
 *      {
 *          client_data->flags |= IDIGI_MSG_DATA_NOT_PROCESSED;
 *      }
 *
 *      response_data->message_status = idigi_msg_error_none;
 *
 *      if (client_device_request->length_in_bytes == 0)
 *      {   
 *          // done
 *          device_request_active_count--;
 *          free(client_device_request);
 *      }
 * 
 *     return idigi_callback_continue;
 * }
 * 
 * static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
 *                                                     idigi_data_service_msg_response_t * const response_data)
 * {
 *     device_request_handle_t * const client_device_request = response_data->user_context;
 *     idigi_data_service_block_t * error_data = request_data->server_data;
 *     idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);
 * 
 *     printf("process_device_error: error %d from server\n", error_code);
 *     device_request_active_count--;
 *     free(client_device_request);
 *     return idigi_callback_continue;
 * }
 * 
 * @endcode
 *
 * @section zlib Optional Data Compression Support
 * The IIK has an optional Data Compression switch that reduces the amount of network traffic.  This option requires applications
 * to link with the zlib library and add the zlib header file (zlib.h) to the IIK include path.
 *
 * @note Enabling this option greatly increases the application code size and memory required to execute.
 *
 * If your application requires Data compression, but your development environment does not include the zlib library,
 * you will need to download and build the library.  The zlib home page is located at: http://zlib.net/.   Instructions
 * on how to build zlib are provided with the package.
 *
 * @note The zlib library is required only if your application enables the @ref IDIGI_COMPRESSION "Data Compression switch".
 *
 */


