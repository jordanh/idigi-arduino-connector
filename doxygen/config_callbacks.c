/*! @page config_callbacks Configuration Callbacks
 *
 * @htmlinclude nav.html
 *
 * @section config Configuration Callbacks
 * The iDigi connector interfaces to the platform's configuration through the callbacks listed below.  These
 * are called through the application callback described in the @ref api1_overview.
 * 
 *  -# @ref device_id
 *  -# @ref vendor_id
 *  -# @ref device_type
 *  -# @ref server_url
 *  -# @ref connection_type
 *  -# @ref mac_address
 *  -# @ref link_speed
 *  -# @ref phone_number
 *  -# @ref tx_keepalive
 *  -# @ref rx_keepalive
 *  -# @ref wait_count
 *  -# @ref ip_address
 *  -# @ref error_status
 *  -# @ref firmware_support
 *  -# @ref data_service_support
 *  -# @ref file_system_support
 *  -# @ref rci_support
 *  -# @ref max_msg_transactions
 *  -# @ref device_id_method
 *  -# @ref imei_number
 *
 * @section device_id Device ID
 * Returns a unique Device ID used to identify the device.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_device_id() in config.c.
 *
 * Device IDs are a globally unique identifier for iDigi clients.  The Device ID is a
 * 16-octet value derived from the MAC address of a network interface on the client.
 * The mapping from MAC address to Device ID consists of inserting "FFFF" in the middle
 * of the MAC and setting all other bytes of the Device ID to 0.
 * For Example:
 * MAC Address 12:34:56:78:9A:BC, would map to a Device ID: 00000000-123456FF-FF789ABC.
 * If a client has more than one network interface, it does not matter to the iDigi Device Cloud which
 * network interface MAC is used for the basis of the Device ID.  If the MAC is read
 * directly from the network interface to generate the client's Device ID, care must be
 * taken to always use the same network interface's MAC since there is a unique mapping
 * between a device and a Device ID.
 *
 * @see @ref add_your_device_to_the_cloud "Adding your Device to the iDigi Device Cloud"
 * @see app_get_device_id()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_device_id @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to 16-byte device ID. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of the device ID</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned device ID</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get device ID and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_device_id)
 *     {
 *         static uint8_t my_device_id[DEVICE_ID_LENGTH];
 *         uint8_t ** response_device_id = (uint8_t **)response_data;
 *
 *          // just uses the MAC address to format the device ID
 *          my_device_id[8] = my_device_mac_addr[0];
 *          my_device_id[9] = my_device_mac_addr[1];
 *          my_device_id[10] = my_device_mac_addr[2];
 *          my_device_id[11] = 0xFF;
 *          my_device_id[12] = 0xFF;
 *          my_device_id[13] = my_device_mac_addr[3];
 *          my_device_id[14] = my_device_mac_addr[4];
 *          my_device_id[15] = my_device_mac_addr[5];
 *
 *          // return the address of my_device_id
 *          *response_device_id   = my_device_id;
 *          *response_length = sizeof my_device_id;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 *
 * @section vendor_id Vendor ID
 *
 * Return vendor ID which is a unique code identifying the manufacturer of a device. 
 * Vendor IDs are assigned to manufacturers by the iDigi Device Cloud. 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_vendor_id() in config.c.
 *
 * @note If @ref IDIGI_VENDOR_ID configuration is defined in @ref idigi_config.h, this callback
 * will not be called. See @ref idigi_config_data_options
 *
 * @see @ref idigi_vendor_id "Obtaining an iDigi Vendor ID"
 * @see app_get_vendor_id()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_vendor_id @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to 4-byte vendor ID. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of the vendor ID</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned vendor ID</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get vendor ID and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_vendor_id)
 *     {
 *         extern const uint8_t my_device_vendor_id[VENDOR_ID_LENGTH];
 *         uint8_t **id = (uint8_t **)response_data;
 *
 *         *id   = (uint8_t *)&my_device_vendor_id[0];
 *         *response_length = sizeof my_device_vendor_id;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section device_type Device Type
 *
 * Returns a pointer to the device type which is an iso-8859-1 encoded string.
 * This string should be chosen by the device manufacturer as a name that uniquely
 * identifies this model of device  to the server. When the server finds two devices
 * with the same device type, it can infer that they are the same product and
 * product-scoped data may be shared among all devices with this device type.
 * A device's type cannot be an empty string, nor contain only whitespace.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_device_type() in config.c.
 *
 * @note If @ref IDIGI_DEVICE_TYPE configuration is defined in @ref idigi_config.h, this callback 
 * will not be called.
 *
 * @see @ref idigi_config_data_options
 * @see app_get_device_type()
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_device_type @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to an ASCII device type string. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the length to the device type. Maximum is 32 bytes.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned device type</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get device type and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_device_type)
 *     {
 *         // Return pointer to device type.
 *         extern const char device_type[];
 *         char ** type = (char **)response_data;
 *         *type = (char *)&device_type[0];
 *         *response_length = sizeof device_type-1;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section server_url The iDigi Device Cloud Server URL
 *
 * Return the iDigi Device Cloud FQDN.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_server_url() in config.c.
 *
 * @note If @ref IDIGI_CLOUD_URL configuration is defined in @ref idigi_config.h, this callback 
 * will not be called. See @ref idigi_config_data_options
 * 
 * @see app_get_server_url()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_server_url @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to FQDN of the iDigi Device Cloud to be connected. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the length of the server URL. Maximum is 255 bytes..</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned server URL</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get server URL and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_server_url)
 *     {
 *         // Return pointer to server url.
 *         static const char idigi_server_url[] = "developer.idigi.com";
 *         char ** url = (char **)response_data;
 *
 *         *url = (char *)&idigi_server_url[0];
 *         *response_length = sizeof idigi_server_url -1;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section connection_type Connection Type
 *
 * Return LAN or WAN connection type. 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_connection_type() in config.c.
 *
 * @note If @ref IDIGI_CONNECTION_TYPE configuration is defined in @ref idigi_config.h, this callback 
 * will not be called. See @ref idigi_config_data_options
 *
 * @see app_get_connection_type()
 *
 * @htmlonly
 * <table class="apitable">
 *   <tr>
 *     <th colspan="2" class="title">Arguments</th>
 *   </tr>
 *   <tr>
 *     <th class="subtitle">Name</th> <th class="subtitle">Description</th>
 *    </tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_connection_type @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>Callback returns a pointer to @endhtmlonly @ref idigi_lan_connection_type @htmlonly for LAN connection type or @endhtmlonly @ref idigi_wan_connection_type @htmlonly for WAN connection type. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned connection type</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get connection type and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_connection_type)
 *     {
 *         static idigi_connection_type_t  device_connection_type = idigi_lan_connection_type;
 *         idigi_connection_type_t ** type = (idigi_connection_type_t **)response_data;
 *         *type = &device_connection_type;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section mac_address MAC Address
 *
 * Return the device's MAC address. 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_mac_addr() in config.c.
 *
 * @see app_get_mac_addr()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_mac_addr @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>Callback returns pointer to 6-byte MAC address</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td> Pointer to memory where callback writes the size of MAC address. It must be 6 bytes. </td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned MAC address</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get MAC address and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_mac_addr)
 *     {
 *         extern uint8_t device_mac_addr[];
 *         uint8_t ** mac_addr = (uint8_t **)response_data;
 *
 *         *mac_addr = (uint8_t *)&device_mac_addr[0];
 *         *response_length = sizeof device_mac_addr;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section link_speed Link Speed
 *
 * Return link speed for a WAN connection type. If connection 
 * type is LAN, iDigi connector will not request link speed configuration.. 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_link_speed() in config.c.
 *
 * @note If @ref IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND configuration is defined in @ref idigi_config.h, 
 * this callback will not be called. See @ref idigi_config_data_options
 *
 * @see app_get_link_speed()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_link_speed @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to 4-byte integer link speed. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of link speed. It must be 4 bytes</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned link speed</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get link speed and callback aborted the iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_link_speed)
 *     {
 *         static uint32_t wan_speed = 19200;
 *         uint32_t **speed = (uint32_t **)response_data;
 *         *speed = &wan_speed;
 *         *response_length = sizeof wan_speed;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section phone_number Phone Number
 *
 * Return the phone number dialed for a WAN connection type. 
 * The phone number is a variable length, non NUL-terminated string.
 * If connection type is LAN, iDigi connector will not request phone number 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_phone_number() in config.c.
 *
 * @note If @ref IDIGI_WAN_PHONE_NUMBER_DIALED configuration is defined in @ref idigi_config.h, 
 * this callback will not be called.  See @ref idigi_config_data_options
 *
 * @see app_get_phone_number()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 *   <th colspan="2" class="title">Arguments</th>
 * </tr>
 * <tr>
 *   <th class="subtitle">Name</th> <th class="subtitle">Description</th>
 * </tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_phone_number @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to memory which contains the phone number dialed and any dialing prefixes </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of the phone number in bytes</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned phone number</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get phone number and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_phone_number)
 *     {
 *         static char wan_phone_number[] = "0000000000";
 *         char **phone = (char **)response_data;
 *         *phonse = (char *)&wan_phone_number[0];
 *         *response_length = sizeof wan_phone_number - 1;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section tx_keepalive TX Keepalive Interval
 *
 * Return TX keepalive interval in seconds. This tells how 
 * often the iDigi Device Cloud sends keepalive messages to the device.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_tx_keepalive_interval() in config.c.
 *
 * @note If @ref IDIGI_TX_KEEPALIVE_IN_SECONDS is defined in @ref idigi_config.h, 
 * this callback will not be called.  See @ref idigi_config_data_options
 *
 * @see app_get_tx_keepalive_interval()
 *
 * @htmlonly
 * <table class="apitable">
 *   <tr>
 *     <th colspan="2" class="title">Arguments</th>
 *   </tr>
 *   <tr>
 *     <th class="subtitle">Name</th>
 *     <th class="subtitle">Description</th>
 *   </tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_tx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to 2-byte integer TX keepalive interval in seconds. It must be between 5 and 7200 seconds. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of TX keepalive. It must be 2 bytes.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned TX keepalive interval</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get TX keepalive interval and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_tx_keepalive)
 *     {
 *         static uint16_t my_device_tx_keepalive_interval = 60;
 *         uint16_t ** interval = (uint16_t **)response_data;
 *         *interval = (uint16_t *)&my_device_tx_keepalive_interval;
 *         *response_length = sizeof my_device_tx_keepalive_interval;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section rx_keepalive RX Keepalive Interval
 *
 * Return RX keepalive interval in seconds. This tells how 
 * often iDigi connector sends keepalive messages to the iDigi Device Cloud (device to the iDigi Device Cloud).
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_rx_keepalive_interval() in config.c.
 *
 * @note If @ref IDIGI_RX_KEEPALIVE_IN_SECONDS configuration is defined in @ref idigi_config.h, 
 * this callback will not be called.  See @ref idigi_config_data_options
 *
 * @see app_get_rx_keepalive_interval()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_rx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td>Callback returns pointer to 2-byte integer RX keepalive interval in seconds. It must be between 5 and 7200 seconds. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of RX keepalive. It must be 2 bytes.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned RX keepalive interval</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get RX keepalive interval and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_rx_keepalive)
 *     {
 *         static uint16_t my_device_rx_keepalive_interval = 60;
 *         uint16_t ** interval = (uint16_t **)response_data;
 *         *interval = (uint16_t *)&my_device_rx_keepalive_interval;
 *         *response_length = sizeof my_device_rx_keepalive_interval;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section wait_count Wait Count
 *
 * Return the number of times of not receiving a keepalive message which a connection 
 * should be considered lost. 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_wait_count() in config.c.
 *
 * @note If @ref IDIGI_WAIT_COUNT configuration is defined in @ref idigi_config.h, 
 * this callback will not be called.  See @ref idigi_config_data_options
 *
 * @see app_get_wait_count()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_wait_count @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns a pointer to 2-byte wait count. It must be between 2 and 63 times. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory where callback writes the size of wait count. It must be 2 bytes.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned wait count</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get wait count and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_wait_count)
 *     {
 *         static uint16_t my_wait_count = 10;
 *         uint16_t ** count = (uint16_t **)response_data;
 *         *count = (uint16_t *)&my_wait_count;
 *         *response_length = sizeof my_wait_count;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section ip_address Device IP Address
 *
 * Return a unique device IP address. The length of the IP 
 * address must be either 4-octet for an IPv4 or 16-octet for an IPv6 address. 
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_ip_address() in config.c.
 *
 * @see app_get_ip_address()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_ip_addr @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback returns pointer to IP address. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td> Pointer to memory where callback writes the size of IP address. 
 *     It returns 16 bytes for an IPv6 address or 4 bytes for an IPv4 address.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned IP address</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get IP address and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_ip_addr)
 *     {
 *         extern uint8_t my_ip_address[];
 *         uint8_t ** ip_address = (uint8_t **)response_data;
 *
 *         *ip_address = (uint8_t *)&my_ip_address[0];
 *         *response_length = sizeof my_ip_address;
  *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section error_status  Error Status Notification
 *
 * This callback is called to notify the user that iDigi connector encountered an error. When iDigi connector finds an
 * error, iDigi connector will call this callback indicating the error status
 *
 * @note If @ref IDIGI_DEBUG  is not defined in @ref idigi_config.h, this callback 
 * will not be called.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_error_status @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer to @endhtmlonly @ref idigi_error_status_t @htmlonly containing
 *     the class id, request id, and error status which iDigi connector encountered error with.</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> Size of idigi_error_status_t</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>None</td>
 * <td>None</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_error_status)
 *     {
 *         idigi_error_status_t * error_data = request_data;
 *         printf("idigi_error_status: unsupport class_id = %d request_id = %d status = %d\n",
 *                   error_data->class_id, error_data->request_id.config_request, error_data->status);
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 * @section firmware_support  Firmware Access Support
 *
 * Return @ref idigi_service_supported_status_t status to enable or disable Firmware 
 * download capability. If it's supported, callback for @ref idigi_firmware_request_t 
 * must be implemented for firmware download. This callback allows application to enable
 * or disable firmware download capability during runtime.
 *
 * @note If @ref IDIGI_FIRMWARE_SERVICE configuration is not defined in @ref idigi_config.h, this callback 
 * will not be called and Firmware Download is not supported. iDigi connector does not include firmware support.
 *
 * @note If @ref IDIGI_FIRMWARE_SUPPORT is defined in @ref idigi_config.h, this callback is not needed. 
 * It enables firmware download capability. See @ref idigi_config_data_options. IDIGI_FIRMWARE_SERVICE must be defined.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_firmware_facility @htmlonly</td>
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
 * <td> Pointer to memory where callback writes @endhtmlonly @ref idigi_service_supported_status_t @htmlonly 
 * status to support or not support firmware access facility</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned the status</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_firmware_facility)
 *     {
 *         *((idigi_service_supported_status_t *)response_data) = idigi_service_supported;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section data_service_support  Data Service Support
 *
 * Return @ref idigi_service_supported_status_t status to enable or disable data service 
 * capability. If it's supported, callback for @ref idigi_data_service_request_t must be 
 * implemented for data service. This callback allows application to enable
 * or disable data service capability during runtime.
 *
 * @note If @ref IDIGI_DATA_SERVICE configuration is not defined in @ref idigi_config.h, this callback 
 * will not be called and Data Service is not supported. iDigi connector does not include data service.
 *
 * @note If @ref IDIGI_DATA_SERVICE_SUPPORT is defined in @ref idigi_config.h, this callback is not needed.
 * It enables data service capability. See @ref idigi_config_data_options. IDIGI_DATA_SERVICE must be defined.
 *
 * @note Define @ref IDIGI_COMPRESSION in @ref idigi_config.h for compression transfer.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_data_service @htmlonly</td>
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
 * <td> Pointer to memory where callback writes @endhtmlonly @ref idigi_service_supported_status_t @htmlonly 
 * status to support or not support data service</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned the status</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_data_service)
 *     {
 *         *((idigi_service_supported_status_t *)response_data) = idigi_service_supported;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section file_system_support  File System Support
 *
 * Return @ref idigi_service_supported_status_t status to enable or disable file system 
 * capability. If it's supported, callback for @ref idigi_file_system_request_t must be 
 * implemented for file system. This callback allows application to enable
 * or disable file system capability during runtime.
 *
 * @note If @ref IDIGI_FILE_SYSTEM configuration is not defined in @ref idigi_config.h, this callback 
 * will not be called and File System is not supported. iDigi connector does not include file system.
 *
 * @note If @ref IDIGI_FILE_SYSTEM_SUPPORT is defined in @ref idigi_config.h, this callback is not needed.
 * It enables file system capability. See @ref idigi_config_data_options. IDIGI_FILE_SYSTEM must be defined.
 *
 * @note Define @ref IDIGI_COMPRESSION in @ref idigi_config.h for compression transfer.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_file_system @htmlonly</td>
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
 * <td> Pointer to memory where callback writes @endhtmlonly @ref idigi_service_supported_status_t @htmlonly 
 * status to support or not support file_system</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned the status</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_data_service)
 *     {
 *         *((idigi_service_supported_status_t *)response_data) = idigi_service_supported;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section rci_support  Remote Configuration Support
 *
 * Return @ref idigi_service_supported_status_t status to enable or disable remote configuration 
 * capability. If it's supported, callback for @ref idigi_remote_config_request_t must be 
 * implemented for remote configuration. This callback allows application to enable
 * or disable remote configuration capability during runtime.
 *
 * @note If @ref IDIGI_RCI_SERVICE is not defined in @ref idigi_config.h, this callback 
 * will not be called and remote configuration is not supported. iDigi Connector does not include remote configuration.
 *
 * @note If @ref IDIGI_RCI_SERVICE is defined in @ref idigi_config.h, this callback is not needed.
 * It enables remote configuration capability. See @ref idigi_config_data_options. 
 * @see IDIGI_REMOTE_CONFIGURATION_SUPPORT
 *
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_remote_configuration @htmlonly</td>
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
 * <td> Pointer to memory where callback writes @endhtmlonly @ref idigi_service_supported_status_t @htmlonly 
 * status to support or not support remote configuration</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned the status</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_remote_configuration)
 *     {
 *         *((idigi_service_supported_status_t *)response_data) = idigi_service_supported;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section max_msg_transactions Maximum Message Transactions
 *
 * Return maximum simultaneous transactions for data service and file system to receive message from the
 * iDigi Device Cloud. This configuration is required if @ref data_service_support or @ref file_system_support
 * is enabled. 
 *
 * @note If @ref IDIGI_MSG_MAX_TRANSACTION configuration is defined in @ref idigi_config.h, this callback 
 * will not be called. See @ref idigi_config_data_options
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_max_transaction @htmlonly</td>
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
 * <td> Pointer to unsigned int memory where callback writes maximum transactions.
 * It must be between 1 to 255. Use 0 for unlimited transactions.</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned maximum transactions</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_max_transaction)
 *     {
 *          *((unsigned int *)response_data) = IDIGI_MAX_MSG_TRANSACTIONS;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section device_id_method Device ID Method
 * Returns device ID method on how to obtain device ID.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_device_id_method() in config.c.
 *
 * @see @ref add_your_device_to_the_cloud "Adding your Device to the iDigi Device Cloud"
 * @see app_get_device_id()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_device_id_method @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Pointer to memory where callback writes <i> <b> idigi_auto_device_id_method </b> </i>or
 *      <i> <b> idigi_manual_device_id_method. </b> </i> @endhtmlonly
 *                          - @ref idigi_auto_device_id_method to generate device ID from
 *                                  - @ref mac_address callback for @ref idigi_lan_connection_type or  
 *                                  - @ref imei_number callback for @ref idigi_wan_connection_type
 *                                  .
 *                          - @ref idigi_manual_device_id_method to obtain device ID from
 *                                  @ref device_id callback
 *                          .
 *  @htmlonly Note: The @endhtmlonly @ref device_id @htmlonly callback will not be called 
 *  if <i> <b> idigi_auto_device_id_method </b> </i> is returned.
 * </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory which contains the size of the response_data</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned device ID method</td></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get device ID method and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_device_id_method)
 *     {
 *         idigi_device_id_method_t * const device_id_method = response_data;
 *
 *         *device_id_method = idigi_auto_device_id_method;
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section imei_number IMEI number
 * Returns IMEI number for generating device ID. This is called if @ref device_id_method callback 
 * returns @ref idigi_auto_device_id_method for @ref idigi_wan_connection_type connection type.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_get_imei_number() in config.c.
 *
 * @see @ref add_your_device_to_the_cloud "Adding your Device to the iDigi Device Cloud"
 * @see app_get_device_id()
 * @see @ref device_id_method for device ID Method
 * @see @ref connection_type for connection type.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_config_imei_number @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Callback writes 14 IMEI decimal digits plus 1 check digit. Each nibble
 *      corresponds a decimal digit and most upper nibble must be 0. </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>Pointer to memory which contains the size of the response_data</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned IMEI number</td></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get device ID method and callback aborted iDigi connector</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void const * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_config && request_id.config_request == idigi_config_imei_number)
 *     {
 *             /* Each nibble corresponds a decimal digit.
 *              * Most upper nibble must be 0.
 *              */
 *             char  const app_imei_number[] = "000000-00-000000-0";
 *             int i = sizeof app_imei_number -1;
 *             int index = *size -1;
 * 
 *             while (i > 0)
 *             {
 *                 int n = 0;
 * 
 *                 imei_number[index] = 0;
 * 
 *                 while (n < 2 && i > 0)
 *                 {
 *                     i--;
 *                     if (app_imei_number[i] != '-')
 *                     {
 *                         imei_number[index] += ((app_imei_number[i] - '0') << (n * 4));
 *                         n++;
 *                     }
 *                 }
 *                 index--;
 *             }
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @htmlinclude terminate.html
 */
