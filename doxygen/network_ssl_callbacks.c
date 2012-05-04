
/*! @page network_ssl_callbacks Network Callbacks for SSL connection
 *
 * @htmlinclude nav.html
 *
 * @section net_ssl Network Callbacks
 * From the top level these callbacks are analogous to the @ref network_callbacks. Internally they provide
 * interface to securely connect to iDigi Cloud on SSL. Any application can make use of these APIs to
 * obtain secure connection just by including network_ssl.c instead of network.c. The sample application
 * @ref connect_on_ssl is created to demonstrate how easy to enable SSL connections. Like @ref network_callbacks,
 * the SSL network interface module will provide the following callback APIs 
 * 
 *  -# @ref ssl_connect
 *  -# @ref ssl_send
 *  -# @ref ssl_receive
 *  -# @ref ssl_close
 * 
 * The SSL network callback APIs use openssl functions to communicate with iDigi server over SSL connection.
 * These APIs can be found in network_ssl.c under platforms/<i>my_platform</i> to demonstrate how one can
 * write the network layer callback APIs to connect, send, receive and close the SSL connection.
 * As far as the top level calls are concerned there is no change between the SSL and non-SSL connection.
 *
 * @section ssl_connect Connect
 *
 * This callback is called to establish SSL connection between ICC and the iDigi Cloud.
 * Callback is responsible to setup any socket options and SSL specific initial settings.
 * 
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_network_connect() in network_ssl.c.
 *
 * @see app_network_connect()
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
 * <td>Pointer to server URL that callback will make connection to on @endhtmlonly @ref IDIGI_SSL_PORT @htmlonly for secure communication </td>
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
 * #include "idigi_api.h"
 * #include "platform.h"
 *
 * #define APP_SSL_CA_CERT   "./public/include/idigi-ca-cert-public.crt"
 *
 * typedef struct
 * {
 *    int sfd;
 *    SSL_CTX * ctx;
 *    SSL * ssl;
 * } app_ssl_t;
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     /* resolve address */
 *     /* create socket fd, set socket option for keep alive and no delay */
 *     /* connect to IDIGI_SSL_PORT on iDigi Cloud */
 *
 *    SSL_library_init();
 *    OpenSSL_add_all_algorithms();
 *    SSL_load_error_strings();
 *    ssl_ptr->sfd = fd;
 *    ssl_ptr->ctx = SSL_CTX_new(TLSv1_client_method());    
 *    if (ssl_ptr->ctx == NULL)
 *    {
 *        ERR_print_errors_fp(stderr);
 *        goto error;
 *    } 
 *
 *    ssl_ptr->ssl = SSL_new(ssl_ptr->ctx);
 *    if (ssl_ptr->ssl == NULL)
 *    {
 *        ERR_print_errors_fp(stderr);
 *        goto error;
 *    }
 *
 *    SSL_set_fd(ssl_ptr->ssl, ssl_ptr->sfd);
 *
 *    ret = SSL_CTX_load_verify_locations(ctx, APP_SSL_CA_CERT, NULL);
 *    if (ret != 1) 
 *    {
 *        APP_DEBUG("Failed to load CA cert [%d]\n", ret);
 *        ERR_print_errors_fp(stderr);
 *        goto error;
 *    }
 *    
 *    SSL_set_options(ssl_ptr->ssl, SSL_OP_ALL);
 *    if (SSL_connect(ssl_ptr->ssl) <= 0)
 *    {
 *        ERR_print_errors_fp(stderr);
 *        goto error;
 *    }
 *
 *    {
 *        X509 * const server_cert = SSL_get_peer_certificate(ssl);
 *
 *        if (server_cert == NULL)
 *        {
 *            APP_DEBUG("No server certificate is provided\n");
 *            goto done;
 *        }
 *
 *        ret = SSL_get_verify_result(ssl);
 *        if (ret !=  X509_V_OK)
 *        {
 *            APP_DEBUG("Server certificate is invalid [%d]\n", ret);
 *           goto done;
 *        }
 *    }
 *
 * }
 *
 * @endcode
 *
 * @section ssl_send Send
 *
 * Callback is called to send data to the iDigi Cloud over SSL connection. This function may 
 * block up to the timeout specified in the idigi_write_request_t. If it encounters EAGAIN 
 * or EWOULDBLOCK error, 0 bytes must be returned and IIK will continue calling this function.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_network_send() in network.c.
 *
 * @see app_network_send()
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
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     app_ssl_t * const ssl_ptr = (app_ssl_t *)write_data->network_handle;
 *     int const bytes_sent = SSL_write(ssl_ptr->ssl, write_data->buffer, write_data->length);
 *
 *     if (bytes_sent <= 0)
 *     {
 *         /* call SSL_get_error() to find out exact cause */
 *         APP_DEBUG("SSL_write failed %d\n", bytes_sent);
 *         status = idigi_callback_abort;
 *     }
 *
 *     *sent_length = bytes_sent;
 *     return status;
 * }
 * @endcode
 *
 * @section ssl_receive Receive
 *
 * Callback is called to receive a specified number of bytes of data from the iDigi 
 * Cloud.  This function may block up to the timeout specified in the idigi_read_request_t.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_network_receive() in network_ssl.c.
 *
 * @note In the multithreaded model (idigi_run()) this is the point where the ICC
 * will relinquish control of the CPU.
 *
 * @see app_network_receive()
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
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     app_ssl_t * const ssl_ptr = (app_ssl_t *)read_data->network_handle;
 *     int bytes_read = 0;
 *
 *     *read_length = 0;
 *     if (SSL_pending(ssl_ptr->ssl) == 0)
 *     {
 *         int ready;
 *         struct timeval timeout;
 *         fd_set read_set;
 *
 *         timeout.tv_sec = read_data->timeout;
 *         timeout.tv_usec = 0;
 *
 *         FD_ZERO(&read_set);
 *         FD_SET(ssl_ptr->sfd, &read_set);
 *
 *         /* Blocking point for IIK */
 *         ready = select(ssl_ptr->sfd + 1, &read_set, NULL, NULL, &timeout);
 *         if (ready == 0)
 *         {
 *             status = idigi_callback_busy;
 *             goto done;
 *         }
 *
 *         if (ready < 0)
 *         {
 *            APP_DEBUG("app_network_receive: select failed\n");
 *            goto done;
 *         }
 *     }
 *
 *     bytes_read = SSL_read(ssl_ptr->ssl, read_data->buffer, (int)read_data->length);
 *     if (bytes_read <= 0)
 *     {
 *         /* EOF on input: the connection was closed. */
 *         APP_DEBUG("SSL_read failed %d\n", bytes_read);
 *         status = idigi_callback_abort;
 *     }
 *
 *     *read_length = (size_t)bytes_read;
 *
 * done:
 *     return status;
 * }
 *
 * @endcode
 *
 * @section ssl_close Close
 *
 * Callback is called to close a connection between a device and the iDigi Cloud. This API
 * should release all the SSL based allocation.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_network_close() in network_ssl.c.
 *
 * @see app_network_close()
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
 * <td>Callback was unable to close the connection and callback aborted ICC</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     app_ssl_t * const ssl_ptr = (app_ssl_t *)handle;
 *
 *     /* send close notify to peer */
 *     if (SSL_shutdown(ssl_ptr->ssl) == 0) 
 *         SSL_shutdown(ssl_ptr->ssl);  /* wait for peer's close notify */
 *
 *     if (ssl_ptr->ssl != NULL) 
 *     {
 *         SSL_free(ssl_ptr->ssl);
 *         ssl_ptr->ssl = NULL;
 *     }
 *
 *     if (ssl_ptr->ctx != NULL) 
 *     {
 *         SSL_CTX_free(ssl_ptr->ctx);
 *         ssl_ptr->ctx = NULL;
 *     }
 *
 *     if (ssl_ptr->sfd != -1) 
 *     {
 *         close(ssl_ptr->sfd);
 *         ssl_ptr->sfd = -1;
 *     }
 *     return status;
 * }
 *
 * @endcode
 *
 */
