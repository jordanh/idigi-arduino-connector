
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
 * This callback is called to establish connection between IIK and the iDigi Device Cloud. 
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
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_network && request_id.network_request == idigi_network_connect)
 *     {
 *         static idigi_network_handle_t idigi_fd = -1;
 *         struct sockaddr_in sin;
 *         in_addr_t ip_addr;
 *         int opt=1;
 *
 *        idigi_network_handle_t ** network_handle = (idigi_network_handle_t **)response_data;
 *
 *         if (idigi_fd == -1)
 *         {
 *             if (dns_resolve_name((char *)request_data, request_length, &ip_addr) != 0)
 *             {
 *                 return idigi_callback_abort;
 *             }
 *
 *             idigi_fd = socket(AF_INET, SOCK_STREAM, 0);
 *             if (idigi_fd >= 0)
 *             {
 *                 int enabled = 1;
 *
 *                 if (setsockopt(idigi_fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&enabled, sizeof(enabled)) < 0)
 *                {
 *                    perror("setsockopt SO_KEEPALIVE failed");
 *                }
 *
 *                if (setsockopt(idigi_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, sizeof(enabled)) < 0)
 *                {
 *                    perror("setsockopt TCP_NODELAY failed");
 *                }
 *            }
 *            else
 *            {
 *                perror("Could not open socket");
 *                return idigi_callback_abort;
 *            }
 *
 *            /* set non-blocking socket */
 *            if (ioctl(idigi_fd, FIONBIO, &opt) < 0)
 *            {
 *                perror("FIONBIO failed");
 *                return idigi_callback_abort;
 *            }
 *
 *            memset((char *)&sin, 0, sizeof(sin));
 *            memcpy(&sin.sin_addr, &ip_addr, sizeof sin.sin_addr);
 *            sin.sin_port   = htons(IDIGI_PORT);
 *            sin.sin_family = AF_INET;
 *            if (connect(idigi_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
 *            {
 *                 perror("connect() failed");
 *                return idigi_callback_abort;
 *            }
 *        }
 *
 *        *network_handle = &idigi_fd;
 *        *response_length = sizeof idigi_fd;

 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section send Send
 *
 * Callback is called to send data to the iDigi Device Cloud. This function may block up to
 * the timeout specified in the idigi_write_request_t. If it encounters EAGAIN or EWOULDBLOCK
 * error, 0 bytes must be returned and IIK will continue calling this function.
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
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_network && request_id.network_request == idigi_network_send)
 *     {
 *          idigi_write_request_t const * const write_data = (idigi_write_request_t *)request_data;
 *          size_t * return_bytes_written = (size_t *)response_data;
 *          int bytes_sent;
 *
 *          bytes_sent = send(*write_data->network_handle, (char *)write_data->buffer,
 *                       write_data->length, 0);
 *          if (bytes_sent < 0)
 *          {
 *              if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
 *              {
 *                  return idigi_callback_busy;
 *              }
 *              else
 *              {
 *                  perror("network_send: send() failed");
 *                  return idigi_callback_abort;
 *              }
 *          }
 *          *return_bytes_written = bytes_sent;
 *          return idigi_callback_continue;
 *     }
 * }
 * @endcode
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
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_network && request_id.network_request == idigi_network_receive)
 *     {
 *           idigi_read_request_t * read_data = (idigi_read_request_t *)request_data;
 *           size_t * return_bytes_read = (size_t *)response_data;
 *          int bytes_received;
 *
 *          bytes_received = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);
 *          if (bytes_received == 0)
 *          {
 *              /* EOF on input: the connection was closed. */
 *              return idigi_callback_abort;
 *          }
 *          else if (bytes_received < 0)
 *          {
 *              /* An error of some sort occurred: handle it appropriately. */
 *              if (errno == EAGAIN || errno == EWOULDBLOCK)
 *              {
 *                  return idigi_callback_busy;
 *              }
 *              else
 *              {
 *                  perror("network_receive: recv() failed");
 *                  /* if not timeout (no data) return an error */
 *                  return idigi_callback_abort;
 *              }
 *          }
 *          *return_bytes_read = (size_t)bytes_received;
 *          return idigi_callback_continue;
 *     }
 * }
 *
 * @endcode
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
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_network && request_id.network_request == idigi_network_close)
 *     {
 *          idigi_network_handle_t * const fd = (idigi_network_handle_t *)request_data;
 *          if (close(*fd) < 0)
 *          {
 *              perror("close() failed");
 *          }
 *          return idigi_callback_continue;
 *     }
 * }
 *
 * @endcode
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
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_network && request_id.network_request == idigi_network_disconnected)
 *     {
 *          /* we've been disconnected. Just return continue to reconnect */
 *          return idigi_callback_continue;
 *     }
 * }
 *
 * @endcode
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
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_network && request_id.network_request == idigi_network_reboot)
 *     {
 *          server_reboot();
 *          return idigi_callback_continue;
 *     }
 * }
 *
 * @endcode
 *
 */
