/*! @page config_callbacks Configuration Callbacks
 *
 * @htmlinclude nav.html
 *
 * @section config Configuration Callbacks
 * The IIK interafaces to the platforms configuration through the callbacks listed below, these
 * are called through the application callback described in the @ref api1_overview.
 * 
 *  -# @ref device_id
 *  -# @ref vendor_id
 *  -# @ref device_type
 *  -# @ref server_url
 *  -# @ref connection_type
 *  -# @ref link_speed
 *  -# @ref phone_number
 *  -# @ref tx_keepalive
 *  -# @ref rx_keepalive
 *  -# @ref wait_count
 *  -# @ref ip_address
 *  -# @ref mac_address
 *
 * @section device_id Device ID
 *
 * Return a unique device ID which is used to identify the device.
 * See the section: @ref step4
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_device_id @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to 16-byte device ID. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of the device ID</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned device ID</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get device ID and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section vendor_id Vendor ID
 *
 * Return vendor ID which is a unique code identifying the manufacturer of a device. 
 * Vendor IDs are assigned to manufacturers by iDigi. 
 * See the section: @ref step4
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_vendor_id @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to 4-byte vendor ID. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of the vendor ID</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned vendor ID</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get vendor ID and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section device_type Device Type
 *
 * Return device type which is an iso-8859-1 encoded string 
 * that identifies the type of the device. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_device_type @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to an ASCII device type string. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the length to the device type. Maximum is 32 bytes.</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned device type</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get device type and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section server_url iDigi Server URL
 *
 * Return the iDigi Device Cloud FQDN.
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_server_url @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to FQDN of iDigi server to be connected. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the length of the server URL. Maximum is 255 bytes..</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned device ID</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get server URL and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section connection_type Connection Type
 *
 * Return LAN or WAN connection type. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_connection_type @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Return a pointer to @endhtmlonly @ref idigi_lan_connection_type @htmlonly for LAN connection type or @endhtmlonly @ref idigi_wan_connection_type @htmlonly for WAN connection type. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned connection type</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get connection type and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned MAC address</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get MAC address and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section link_speed Link Speed
 *
 * Return link speed for a WAN connection type. If connection 
 * type is LAN, IIK will not request link speed configuration.. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_link_speed @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to 4-byte integer link speed. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of link speed. It must be 4 bytes</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned link speed</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get link speed and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section phone_number Phone Number
 *
 * Return the phone number dialed for a WAN connection type. 
 * If connection type is LAN, IIK will not request phone number 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_phone_number @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to memory which contains the phone number dialed and any dialing prefixes </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of the phone number in bytes</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned phone number</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get phone number and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section tx_keepalive TX Keepalive Interval
 *
 * Return TX keepalive interval in seconds. This tells how 
 * often the iDigi Device Cloud sends keepalive messages to the device. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_tx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to TX keepalive interval in seconds. It must be between 5 and 7200 seconds. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of TX keepalive. It must be 2 bytes.</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned TX keepalive interval</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get TX keepalive interval and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section rx_keepalive RX Keepalive Interval
 *
 * Return RX keepalive interval in seconds. This tells how 
 * often the IIK sends keepalive messages to the iDigi Device Cloud (device to the iDigi Device Cloud). 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_rx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Return pointer to RX keepalive interval in seconds. It must be between 5 and 7200 seconds. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of RX keepalive. It must be 2 bytes.</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned RX keepalive interval</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get RX keepalive interval and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section wait_count Wait Count
 *
 * Return the number of times of not receiving a keepalive message which a connection 
 * should be considered lost. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_wait_count @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Return a pointer to wait count. It must be between 2 and 63 bytes. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of wait count. It must be 2 bytes.</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned wait count</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get wait count and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section ip_address Device IP Address
 *
 * Return a unique device IP address. The length of the IP 
 * address must be either 4-octet for an IPv4 or 16-octet for an IPv6 address. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_ip_addr @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Callback returns pointer to IP address. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the size of IP address. It returns 16 bytes for an IPv6 address or 4 bytes for an IPv4 address.</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Return Values:
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Value</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned IP address</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to get IP address and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section mac_address MAC Address
 *
 * Return the device's MAC address. 
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_config @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_config_mac_addr @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to 6-byte MAC address</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Pointer to memory where callback writes the size of MAC address. It must be 6 bytes. </td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 */
