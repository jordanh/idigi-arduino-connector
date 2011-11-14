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
 *  @li @b Device @b requests: Trransfers which are initiated by the server to the device,
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
 * <td> [IN] pointer to @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>[OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * The idigi_data_service_msg_response_t field contains the data to be sent back
 * to the server, the user must copy to the data pointer in idigi_data_service_msg_response_t 
 * up to the length specified, if the data is less than the length specified the user must update the
 * length field.
 *
 * The @ref idigi_data_service_msg_request_t contains a response from the server or an error.
 * The @ref idigi_data_service_type_t indicates what type of message is received from
 * the server, in case of an error the idigi_data_service_block_t will contain the error value
 * idigi_msg_error_t in the data field.
 *
 *
 * If an error is encountered while processing the callback the application
 * can send an error back to the server by setting the error in the @ref idigi_msg_error_t "message_status"
 * of @ref  idigi_data_service_msg_response_t.
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
 * The application callback is called with the data from the server
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
 * <td>@endhtmlonly @ref idigi_data_service_device_request @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td> [IN] pointer to @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>[IN] Size of @endhtmlonly idigi_data_service_msg_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> [OUT] pointer to @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly idigi_data_service_msg_response_t @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * The request_data and response_data fields are the same as described above for
 * put requests.  The request_data contains a @ref idigi_data_service_msg_request_t 
 * structure which contains the data from the server.  Depending on the request
 * the application may send back a reply, if a reply is sent back it is copied
 * into the  idigi_data_service_msg_response_t field in the response_data.
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 */


