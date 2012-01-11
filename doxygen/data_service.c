/*! @page data_service Data Service
 *
 * @htmlinclude nav.html
 *
 * @section data_service_overview Data Service Overview
 *
 * The data service API is used to send data to and from the iDigi
 * Device Cloud.  Data service transfers are either initiated by the iDigi
 * Device Cloud or the device, the two types of requests are listed below:
 * 
 * @note See @ref data_service_support under Configuration to enable or
 * disable data service.
 * 
 * @li @b Put @b Requests: Transfers which are initiated by the device and used to
 * write files to the iDigi Device Cloud, the iDigi Device Cloud may send a status response back
 * indicating the transfer was successful. 
 * @li @b Device @b Requests: Transfers which are initiated by the iDigi Device Cloud to the device.
 * This is used to send data to the device and the device may send a response back.
 *
 * @section put_request Put Requests
 *
 * @subsection initiate_send Initiate Sending Data
 *
 * The application initiates the Put Request to the iDigi Device Cloud by calling idigi_initiate_action()
 * with the request parameter set to @ref idigi_initiate_data_service and request_data pointing to
 * a idigi_data_service_put_request_t structure.
 *
 * @note The idigi_initiate_action() does not send any data.  Instead, the @ref idigi_data_service_put_request "Put Request"
 * @ref idigi_callback_t "callback" is used to supply data to the server.
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
 *
 *   /* Kick off the file trasfer to the iDigi Server */
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
 * @ref idigi_callback_t "callbacks" to retrieve the application data.  These callbacks will continue until
 * the @ref idigi_data_service_block_t response_data flag field has the @ref IDIGI_MSG_LAST_DATA bit set.
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
 * <td> [IN] pointer to @endhtmlonly idigi_data_service_msg_request_t @htmlonly
 *   <p>The @endhtmlonly idigi_data_service_msg_request_t @htmlonly contains a response from the server or an error.</p>
 *   <p>The <b><i>"message_type"</i></b> indicates what type of message is received from
 *   the server, in case of an error the @endhtmlonly idigi_data_service_block_t @htmlonly will contain the 
 *   @endhtmlonly @ref idigi_msg_error_t in @htmlonly the data field.</p>
 *   <p>The <b><i>"service_context"</i></b> is pointer to  @endhtmlonly idigi_data_service_put_request_t @htmlonly.
 * </td></tr>
 * <tr>
 * <th>request_length</th>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly
 *   <p>The @endhtmlonly idigi_data_service_msg_response_t @htmlonly field contains status and the data to 
 *   to be sent to the server, the user must copy to the data pointer in idigi_data_service_msg_response_t 
 *   up to the length specified, if the data is less than the length specified the user must update the
 *   length field.</p>
 *   <p>If an error is encountered while processing the callback the application
 *   can send an error back to the server by setting the error in the <b><i>"message_status"</i></b>
 *   of @endhtmlonly idigi_data_service_msg_response_t @htmlonly.</p>
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
 * An example of an application callback for a put request is show below:
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
 *        case idigi_data_service_type_need_data: /* iDigi Device Cloud requesting data */
 *            {
 *                idigi_data_service_block_t * message = put_response->client_data;
 *                char * dptr = message->data;
 *                char buffer[BUFFER_SIZE];
 *
 *                snprintf(buffer, BUFFER_SIZE, "iDigi data service sample [%d]\n", sample_number);
 *                size_t const bytes = strlen(buffer);
 *                /* Copy to the IIK's buffer */
 *                memcpy(dptr, buffer, bytes);
 *                message->length_in_bytes = bytes;
 *                message->flags = IDIGI_MSG_LAST_DATA | IDIGI_MSG_FIRST_DATA;
 *                put_response->message_status = idigi_msg_error_none;
 *                    sample_number++;
 *            }
 *            break;
 *
 *        case idigi_data_service_type_have_data: /* Response fromt he iDigi Device Cloud */
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
 *        case idigi_data_service_type_error: /* The iDigi Device Cloud sent back an error */
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
 * @section device_request  Device requests
 *
 * Device requests are data transfers initiated by the server and are used to
 * send data from the server to the device, the device may send back a reply.
 * The application callback is continually called with the data from the server
 * until the transfer is complete or an error is encountered.
 *
 * The data from the server is in the idigi_data_service_msg_request_t field,
 * there is either data or an error.  If the application needs to send back
 * a response the application fills in the idigi_data_service_msg_response_t.
 *
 * The user will receive the application callback when a device request is received 
 * from the server with the following information:
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
 * <td> [IN] pointer to @endhtmlonly idigi_data_service_msg_request_t @htmlonly
 *   <p>The @endhtmlonly idigi_data_service_msg_request_t @htmlonly contains request data from the server or an error.</p>
 *   <p>The <b><i>"message_type"</i></b> indicates what type of message is received from
 *   the server, in case of an error the @endhtmlonly idigi_data_service_block_t @htmlonly will contain the 
 *   @endhtmlonly @ref idigi_msg_error_t @htmlonly in the data field.</p>
 *   <p>The <b><i>"service_context"</i></b> is pointer to @endhtmlonly @ref idigi_data_service_device_request_t @htmlonly.
 *
 * </td></tr>
 * <tr>
 * <th>request_length</th>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly
 *   <p>The @endhtmlonly idigi_data_service_msg_response_t @htmlonly is where callback updates the status and the
 *   data to be sent to the server, the callback must copy to the data pointer in 
 *   @endhtmlonly idigi_data_service_block_t @htmlonly
 *   up to the length specified, if the data is less than the length specified the user must update the
 *   length field.</p>
 *   <p>If an error is encountered while processing the callback the application
 *   can send an error back to the server by setting the error in the <b><i>"message_status"</i></b>
 *   of @endhtmlonly idigi_data_service_msg_response_t @htmlonly.</p>
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
 * idigi_callback_status_t app_data_service_callback(idigi_class_t const request_id,
                                                     idigi_request_t const request_id,
 *                                                     void const * request_data, size_t const request_length,
 *                                                     void * response_data, size_t * const response_length)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     idigi_data_service_msg_request_t const * const service_device_request = request_data;
 *
 *     if (class_id == idigi_class_data_service && request_id.data_service_request == idigi_data_service_device_request) 
 *     {   /* Device request from iDigi Device Cloud */
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
 * /* Routines to process the device request, response, & error */
 *  static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
 *                                                       idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_data_service_device_request_t * server_device_request = request_data->service_context;
 *     idigi_data_service_block_t * server_data = request_data->server_data;
 * 
 *     if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
 *     {
 *         /* target should not be null on 1st chunk of data */
 *         if (strcmp(server_device_request->target, device_request_target.target) != 0)
 *         {   /* unsupported target. */
 *             response_data->user_context = NULL;
 *             return idigi_callback_continue;
 *         }
 *
 *         /* 1st chunk of device request so let's initialize client_device_request. */
 *         client_device_request.length_in_bytes = 0;
 *         client_device_request.device_handle = server_device_request->device_handle;
 *
 *         /* setup the user_context for our device request data */
 *         response_data->user_context = &client_device_request;  
 * 
 *     }
 *     else if (response_data->user_context != &client_device_request)
 *     {
 *         /* unsupport target. Ignore data */
 *         return idigi_callback_continue;
 *     }
 * 
 *     handle_device_request_data(server_data->data, server_data->length_in_bytes, server_data->flags);
 *
 *    return idigi_callback_continue;
 * }
 * 
 * static idigi_callback_status_t process_device_response(idigi_data_service_msg_request_t const * const request_data,
 *                                                        idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_data_service_block_t * const client_data = response_data->client_data;
 *     /* get number of bytes going to be written to the client data buffer */
 *     size_t const bytes = (client_device_request.length_in_bytes < client_data->length_in_bytes) ? 
 *                           client_device_request.length_in_bytes : client_data->length_in_bytes;
 * 
 *     if (response_data->user_context == NULL)
 *     {
 *         /* unsupported target. let's return not-processed data */
 *         client_data->length_in_bytes = strlen(not_process_response_data);
 *         client_data->flags = IDIGI_MSG_LAST_DATA | IDIGI_MSG_DATA_NOT_PROCESSED;
 *         client_data->data, not_processed_response_data, strlen(not_process_response_data));
 *         return idigi_callback_continue;
 *     }
 *     if (client_device_request.length_in_bytes == 0)
 *     {   /* get response data */
 *         response_device_request_data();
 *     }
 *     /* let's copy the response data to response buffer */
 *     memcpy(client_data->data, client_device_request.response_data, bytes);
 *     client_device_request.response_data += bytes;
 *     client_device_request.length_in_bytes -= bytes;
 * 
 *     client_data->length_in_bytes = bytes;
 *     client_data->flags = (client_device_request.length_in_bytes == 0) ? IDIGI_MSG_LAST_DATA : 0;
 *     if (client_device_request.length_in_bytes == 0)
 *     {
 *         /* done */
 *         done_device_request();
 *     }
 * 
 *     return idigi_callback_continue;
 * }
 * 
 * static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
 *                                                     idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_data_service_block_t * error_data = request_data->server_data;
 *     idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);
 * 
 * 
 *     printf("process_device_error: error %d from server\n", error_code);
 *     done_device_request();
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
 * @note The zlib library is required only if your application enables the Data Compression switch.
 *
 */


