/*! @page os_callbacks OS Callbacks
 *
 * @htmlinclude nav.html
 *
 * @section callbacks OS Callbacks
 * The IIK interafaces to the platforms OS through the callbacks listed below, these
 * are called through the application callback described in the @ref api1_overview.
 * 
 *  -# @ref malloc
 *  -# @ref free
 *  -# @ref uptime
 *  -# @ref sleep
 *
 * @section malloc malloc
 * Callback used to dynamically allocate memory.
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_os_malloc @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to number of bytes to be allocated </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> Specifies the size of *request_data which is size of size_t.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Returns a pointer to memory for allocated address </td>
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
 * <td>Callback successfully allocated memory</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback was unable to allocate memory and callback aborts IIK</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Memory is not available at this time and needs to be called back again</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section free free
 *
 * Callback is called to free previous allocated memory.
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_os_free @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer address to be freed </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> Specifies the size of *request_data which is size of void*.</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> N/A </td>
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
 * <td>None</td>
 * <td>None</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section uptime System Uptime
 * This callback is called to return system up time in seconds. It is the time 
 * that a device has been up and running..
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_os_system_up_time @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>N/A </td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> Pointer to uint32_t integer memory where callback writes the system up time to (in seconds) </td>
 * </tr>
 * <tr>
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
 * <td>Callback successfully returned the system time</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Error occurred and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * @section sleep Sleep
 * This callback is called relinquish control of the CPU (multithreaded model).
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Name</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_os_sleep @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Number of seconds to sleep up to (the user can sleep less)</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td> N/A</td>
 * </tr>
 * <tr>
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
 * <td>Callback successfully was able to sleep</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Error occurred and callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 */
