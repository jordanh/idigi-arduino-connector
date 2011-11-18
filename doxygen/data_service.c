/*! @page data_service Data Service
 *
 * @htmlinclude nav.html
 *
 * @section data_service_overview Data Service Overview
 *
 * The data service API is used to send data to and from the iDigi
 * server.  The data service has two mechanisms one for device intiaited transfers and the
 * second for server initiated transfers.
 * 
 *  @li @b Put @b requests: Transfers which are initiated by the device and is used to
 * write files to the iDigi server, the server may send a status response back
 * indicating the transfer was successfull.
 *  @li @b Device @b requests: Transfers which are initiated by the server to the device,
 * this is used to send data to the device and the device may send a response back.
 *
 * @section send_data Put Requests
 *
 * @subsection initiate_send Initiate Sending Data
 *
 * The applicication initiates the put request to the server by calling idigi_initiate_action()
 * with the request parameter set to @ref idigi_initiate_data_service and request_data points to
 * a idigi_data_service_put_request_t structure.
 *
 * An example of initiating a device transfer is shown below:
 * @code
 *   static idigi_data_service_put_request_t header;
 *   static char file_path[] = "test/test.txt";
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
 * This example will instuct the IIK to initiate a file transfer to the iDigi server, the IIK
 * will create a file test.txt in the the test directory on the server.  Once the server is
 * ready to receive more data the application callback will be called to get more 
 * data from the application.  The application callback is continually called until
 * the application indicates to the server there is no more data to send, it does this
 * by setting the @ref IDIGI_MSG_LAST_DATA bit in the flag field of the @ref idigi_data_service_block_t
 * which is in the client_data field in the response_data returned by the callback.
 *
 * @subsection get_data Callback to get more data
 *
 * After calling idigi_initiate_action() the user will keep getting a callback until
 * the application sets the flag indicating there is no more data to be sent.
 *
 * The put request callback is called with the following information:
 *
 * @htmlonly
 * <table class="apitable" >
 * <tr>
 * <th class="title">Name</th>
 * <th class="title">Description</th>
 * </tr>
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
 *   <p>The <b><i>idigi_data_service_msg_request_t</i></b> contains a response from the server or an error.</p>
 *
 *   <dl><dd>The <b>"service_context"</b> is pointer to @endhtmlonly idigi_data_service_put_request_t @htmlonly which 
 *   is given in @endhtmlonly idigi_initiate_action @htmlonly call.</dd>
 *
 *   <dd>The <b>"message_type"</b> indicates what type of message is: 
 *   <ul><li> @endhtmlonly @ref idigi_data_service_type_error @htmlonly - Error is encounted and the 
 *   <b>"server_data"</b> will contain the error value @endhtmlonly @ref idigi_msg_error_t @htmlonly in the data field.</li>
 *   <li> @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly - The <b>"server_data"</b> will contain
 *   the response data from server in the data field.</li>
 *   <li> @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly - Request the callback to return data 
 *   to be sent to the server. Data is returned in <b>"response_data"</b>.</li></ul></dd>
 *
 *   <dd>The <b>"server_data"</b> is pointer to @endhtmlonly idigi_data_service_block_t @htmlonly which contains
 *   response data from server when <b>"message_type"</b> is @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly,
 *   @endhtmlonly @ref idigi_msg_error_t @htmlonly from server when <b>"message_type"</b> is
 *   @endhtmlonly @ref idigi_data_service_type_error @htmlonly, or null when 
 *   <b>"message_type"</b> is @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly.
 *   <ul><li><b>"data"</b> contains response data or @endhtmlonly @ref idigi_msg_error_t @htmlonly.</li>
 *   <li><b>"length_in_bytes"</b> contains number of bytes of response data</li>
 *   <li><b>"flags"</b> contains @endhtmlonly @ref IDIGI_MSG_FIRST_DATA @htmlonly bit flag for initial response
 *   data and @endhtmlonly @ref IDIGI_MSG_LAST_DATA @htmlonly bit flag for last chunk of response data.</li></ul></dd></dl>
 * </td></tr>
 * <tr>
 * <th>request_length</th>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly
 *   <p>The <b><i>idigi_data_service_msg_response_t</i></b> field contains status and the data to 
 *   to be sent to the server.</p>
 * 
 *   <dl><dd>The <b>"user_context"</b> is not used.</dd>
 *   <dd> The <b>"message_status"</b> is where callback writes @endhtmlonly @ref idigi_msg_error_t @htmlonly 
 *   when an error is encountered while processing the data. This will cancel and delete the message.</dd>
 *   <dd> The <b>"client_data"</b> is pointer to @endhtmlonly idigi_data_service_block_t @htmlonly when 
 *   <b>"message_type"</b> in <i>request_data</i> is <b>idigi_data_service_type_need_data</b>.
 *   The callback must copy data to the data pointer up to the length specified,
 *	 if the data is less than the length specified the callback must update the length filed. 
 *   <ul><li><b>"data"</b> contains request data, or @endhtmlonly @ref idigi_msg_error_t @htmlonly.
 *   </li>
 *   <li><b>"length_in_bytes"</b> contains number of bytes of data</li>
 *   <li><b>"flags"</b> contains @endhtmlonly @ref IDIGI_MSG_FIRST_DATA @htmlonly bit flag for initial request
 *   data and @endhtmlonly @ref IDIGI_MSG_LAST_DATA @htmlonly bit flag for last chunk of request data.
 *   The callback is continually called to process data from server until IDIGI_MSG_LAST_DATA bit flag is set.
 *   The callback is called with @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly <b>"message_type"</b>
 *    after the callback is called to process the last chunk of data for 
 *    @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly <b>"message_type"</b>.
 * </li></ul></dd></dl>
 * </td></tr>
 * <tr>
 * <th>response_length</th>
 * <td>[OUT] Size of @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * An example of an application callback for a put request is show below:
 *
 * @code
 * idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
 *                                                  void const * request_data, size_t const request_length,
 *                                                  void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_data_service_msg_request_t const * const put_request = request_data;
 *    idigi_data_service_msg_response_t * const put_response = response_data;
 *
 *    UNUSED_PARAMETER(request_length);
 *    UNUSED_PARAMETER(response_length);
 *
 *    if ((put_request == NULL) || (put_response == NULL))
 *    {
 *         APP_DEBUG("Invalid request_data [%p] or response_data[%p]\n", request_data, response_data);
 *         goto done;
 *    }
 *
 *    if (request == idigi_data_service_put_request)
 *    {
 *        switch (put_request->message_type)
 *        {
 *        case idigi_data_service_type_need_data:
 *            {
 *                idigi_data_service_block_t * message = put_response->client_data;
 *                char * dptr = message->data;
 *                char const * const buffer = "Welcome to iDigi data service sample!\n";
 *                size_t const bytes = strlen(buffer);
 *
 *                /* Copy to the IIK's buffer */
 *                memcpy(dptr, buffer, bytes);
 *                message->length_in_bytes = bytes;
 *                message->flags = IDIGI_MSG_LAST_DATA | IDIGI_MSG_FIRST_DATA;
 *                put_response->message_status = idigi_msg_error_none;
 *            }
 *            break;
 *
 *        case idigi_data_service_type_have_data:
 *            {
 *                idigi_data_service_block_t * message = put_request->server_data;
 *                uint8_t const * data = message->data;
 *
 *                APP_DEBUG("Received %s response from server\n", ((message->flags & IDIGI_MSG_RESP_SUCCESS) != 0) ? "success" : "error");
 *                if (message->length_in_bytes > 0) 
 *                {
 *                    APP_DEBUG("Server response %s\n", (char *)data);
 *                }
 *            }
 *            break;
 *
 *        case idigi_data_service_type_error:
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
 * a reponse the application fills in the idigi_data_service_msg_response_t.
 *
 * The user will recieve the application callback when a device request is recieved 
 * from the server with the following information:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</th>
 * <th class="title">Description</th>
 * </tr>
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
 *   <p>The <b><i>idigi_data_service_msg_request_t</i></b> contains a request data from the server or an error.</p>
 *
 *   <dl><dd>The <b>"service_context"</b> is pointer to @endhtmlonly idigi_data_service_device_request_t @htmlonly which 
 *   contains a handle and target name of the request:
 *   <ul><li><b>"device_handle"</b> - is handle of the request</li>
 *   <li><b>"target_name"</b> contains nul terminated target name of the request. The target name
 *   is only provided on the first chunk of data. Otherwise, it's null.</li></ul></dd>
 *
 *   <dd>The <b>"message_type"</b> indicates what type of message is: 
 *   <ul><li> @endhtmlonly @ref idigi_data_service_type_error @htmlonly - Error is encounted and the 
 *   <b>"server_data"</b> will contain the @endhtmlonly @ref idigi_msg_error_t @htmlonly in the data field.</li>
 *   <li> @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly - The <b>"server_data"</b> contains
 *   the request data from server in the data field.</li>
 *   <li> @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly - Request the callback to return data 
 *   to be sent to the server. Data is returned in <b>"response_data"</b>.</li></ul></dd>
 *
 *   <dd>The <b>"server_data"</b> is pointer to @endhtmlonly idigi_data_service_block_t @htmlonly which contains
 *   request data from server when <b>"message_type"</b> is @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly,
 *   @endhtmlonly @ref idigi_msg_error_t @htmlonly from server when <b>"message_type"</b> is
 *   @endhtmlonly @ref idigi_data_service_type_error @htmlonly, or null  when 
 *   <b>"message_type"</b> is @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly.
 *   <ul><li><b>"data"</b> contains request data, or @endhtmlonly @ref idigi_msg_error_t @htmlonly.
 *   </li>
 *   <li><b>"length_in_bytes"</b> contains number of bytes of data</li>
 *   <li><b>"flags"</b> contains @endhtmlonly @ref IDIGI_MSG_FIRST_DATA @htmlonly bit flag for initial request
 *   data and @endhtmlonly @ref IDIGI_MSG_LAST_DATA @htmlonly bit flag for last chunk of request data.
 *   The callback is continually called to process data from server until IDIGI_MSG_LAST_DATA bit flag is set.
 *   The callback is called with @endhtmlonly @ref idigi_data_service_type_need_data @htmlonly <b>"message_type"</b>
 *   after the callback is called to process the last chunk of data for 
 *   @endhtmlonly @ref idigi_data_service_type_have_data @htmlonly <b>"message_type"</b>.
 * </li></ul></dd></dl>
 * </td></tr>
 * <tr>
 * <th>request_length</th>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly
 *   <p>The <b><i>idigi_data_service_msg_response_t</i></b> field contains status and the data to 
 *   to be sent to the server.</p>
 * 
 *   <dl><dd>The <b>"user_context"</b> is where callback writes its own context which will be 
 *   returned on subsequent callbacks for its reference.</dd>
 *
 *   <dd>The <b>"message_status"</b> is where callback writes @endhtmlonly @ref idigi_msg_error_t @htmlonly 
 *   when an error is encountered while processing the data. This will cancel and delete the message.</dd>
 *
 *   <dd>The <b>"client_data"</b> is pointer to @endhtmlonly idigi_data_service_block_t @htmlonly when 
 *   <b>"message_type"</b> in <i>request_data</i> is <b>idigi_data_service_type_need_data</b>.
 *   The callback must copy data to the data pointer up to the length specified,
 *	 if the data is less than the length specified the callback must update the length filed.
 *   <ul><li><b>"data"</b> is where callback writes data to</li>
 *   <li><b>"length_in_bytes"</b> contains number of bytes of available in data. Callback updates this 
 *   with actual number of bytes written to the data</li>
 *   <li><b>"flags"</b> is where callback writes @endhtmlonly @ref IDIGI_MSG_LAST_DATA @htmlonly bit flag for
 *    last chunk of data to be sent to server. The callback is continually called to get response data
 *    until last chunk of data.</li></ul></dd></dl>
* </td></tr>
 * <tr>
 * <th>response_length</th>
 * <td>[OUT] Size of @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * An example of an application callback for a put request is show below:
 *
 * @code
 *  static idigi_callback_status_t process_device_request(idigi_data_service_msg_request_t const * const request_data,
 *                                                       idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     idigi_data_service_device_request_t * server_device_request = request_data->service_context;
 *     idigi_data_service_block_t * server_data = request_data->server_data;
 * 
 *     device_request_handle_t * client_device_request = response_data->user_context;
 * 
 *     ASSERT(server_data != NULL);
 * 
 *     if ((server_data->flags & IDIGI_MSG_FIRST_DATA) == IDIGI_MSG_FIRST_DATA)
 *     {
 *         /* 1st chunk of device request so let's allocate memory for it
 *          * and setup user_context for the client_device_request.
 *          */
 *         void * ptr;
 * 
 *         bool const is_ok = os_malloc(sizeof *client_device_request, &ptr);
 *         if (!is_ok || ptr == NULL)
 *         {
 *             /* no memeory so cancel this request */
 *             APP_DEBUG("process_device_request: malloc fails for device request on session %p\n", server_device_request->device_handle);
 *             response_data->message_status = idigi_msg_error_memory;
 *             goto done;
 *         }
 * 
 *         client_device_request = ptr;
 *         client_device_request->length_in_bytes = 0;
 *         client_device_request->response_data = NULL;
 *         client_device_request->device_handle = server_device_request->device_handle;
 *         device_request_active_count++;
 * 
 *         response_data->user_context = client_device_request;  /* setup the user_context */
 * 
 *         ASSERT(server_device_request->target != NULL);
 *         if (strcmp(server_device_request->target, device_request_target) == 0)
 *         {
 *             client_device_request->target = (char *)device_request_target;
 *             client_device_request->response_data = device_response_data;
 *         }
 *         else
 *         {
 *             client_device_request->target = NULL;
 *             client_device_request->response_data = device_response_error_data;
 *         }
 * 
 *     }
 *     else
 *     {
 *         /* device request should be our user_context */
 *         ASSERT(client_device_request != NULL);
 *     }
 * 
 *     {
 *         /* prints device request data */
 *         char * device_request_data = server_data->data;
 *         if (client_device_request->target != NULL)
 *         {
 *             APP_DEBUG("Device request data: received data = \"%.*s\" for target = \"%s\"\n", server_data->length_in_bytes,
 *                     device_request_data, client_device_request->target);
 *         }
 *         else
 *         {
 *             APP_DEBUG("Device request data: received data = \"%.*s\" for unknown target\n", server_data->length_in_bytes,
 *                     device_request_data);
 * 
 *         }
 *     }
 * 
 *     client_device_request->length_in_bytes += server_data->length_in_bytes;
 * 
 *     if ((server_data->flags & IDIGI_MSG_LAST_DATA) == IDIGI_MSG_LAST_DATA)
 *     {   /* No more chunk. setup the response length to be sent */
 *         client_device_request->length_in_bytes = strlen(client_device_request->response_data);
 *     }
 * 
 *     response_data->message_status = idigi_msg_error_none;
 * 
 * done:
 *     return status;
 * }
 * 
 * static idigi_callback_status_t process_device_response(idigi_data_service_msg_request_t const * const request_data,
 *                                                        idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     device_request_handle_t * const client_device_request = response_data->user_context;
 * 
 *     UNUSED_PARAMETER(request_data);
 * 
 *     ASSERT(response_data->client_data != NULL);
 *     ASSERT(client_device_request != NULL); /* we use user_context for our client_device_request */
 *     ASSERT(server_device_request->device_handle != client_device_request->device_handle);
 * 
 *     {
 *         idigi_data_service_block_t * const client_data = response_data->client_data;
 *         /* get number of bytes written to the client data buffer */
 *         size_t const bytes = (client_device_request->length_in_bytes < client_data->length_in_bytes) ? client_device_request->length_in_bytes : client_data->length_in_bytes;
 * 
 *         APP_DEBUG("Device response data: send response data = %.*s\n", bytes, client_device_request->response_data);
 * 
 *         /* let's copy the response data to service_response buffer */
 *         memcpy(client_data->data, client_device_request->response_data, bytes);
 *         client_device_request->response_data += bytes;
 *         client_device_request->length_in_bytes -= bytes;
 * 
 *         client_data->length_in_bytes = bytes;
 *         client_data->flags = (client_device_request->length_in_bytes == 0) ? IDIGI_MSG_LAST_DATA : 0;
 *         if (client_device_request->target == NULL)
 *         {
 *             client_data->flags |= IDIGI_MSG_DATA_NOT_PROCESSED;
 *         }
 *     }
 * 
 *     response_data->message_status = idigi_msg_error_none;
 * 
 *     if (client_device_request->length_in_bytes == 0)
 *     {   /* done */
 *         device_request_active_count--;
 *         os_free(client_device_request);
 *     }
 *     return status;
 * }
 * 
 * static idigi_callback_status_t process_device_error(idigi_data_service_msg_request_t const * const request_data,
 *                                                     idigi_data_service_msg_response_t * const response_data)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     device_request_handle_t * const client_device_request = response_data->user_context;
 *     idigi_data_service_block_t * error_data = request_data->server_data;
 *     idigi_msg_error_t const error_code = *((idigi_msg_error_t *)error_data->data);
 * 
 * 
 *     APP_DEBUG("process_device_error: handle %p error %d from server\n",
 *                 client_device_request->device_handle, error_code);
 * 
 *     device_request_active_count--;
 *     os_free(client_device_request);
 * 
 *     return status;
 * }
 * 
 * idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t const request,
 *                                                       void const * request_data, size_t const request_length,
 *                                                       void * response_data, size_t * const response_length)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     idigi_data_service_msg_request_t const * const service_device_request = request_data;
 * 
 *     UNUSED_PARAMETER(request_length);
 *     UNUSED_PARAMETER(response_length);
 * 
 *     if (request == idigi_data_service_device_request)
 *     {
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
 *             APP_DEBUG("idigi_put_request_callback: unknown message type %d for idigi_data_service_device_request\n", service_device_request->message_type);
 *             break;
 *         }
 *     }
 *     else
 *     {
 *         APP_DEBUG("Unsupported %d  (Only support idigi_data_service_device_request)\n", request);
 *     }
 * 
 *     return status;
 * }
 * @endcode
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 */


