/*! @page os_callbacks Operating System Callbacks
 *
 * @htmlinclude nav.html
 *
 * @section os_callbacks OS Callbacks
 * The IIK interfaces to the platform's OS through the callbacks listed below.  These
 * are called through the application callback described in the @ref api1_overview.
 * The class id is @ref idigi_class_operating_system.
 * 
 *  -# @ref malloc
 *  -# @ref free
 *  -# @ref uptime
 *  -# @ref sleep
 *
 * @section malloc malloc
 * Callback is used to dynamically allocate memory.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_os_malloc() in os.c.
 *
 * @see app_os_malloc()
 * @see app_os_free()
 * @see @ref idigi_os_free
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_os_malloc @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer to number of bytes to be allocated </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> The size of *request_data which is sizeof size_t.</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> Returns a pointer to memory for allocated address </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td>N/A</td>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_continue @htmlonly</th>
 * <td>Callback successfully allocated memory</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_abort @htmlonly</th>
 * <td>Callback was unable to allocate memory and callback aborts IIK</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_busy @htmlonly</th>
 * <td>Memory is not available at this time and needs to be called back again</td>
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
 *     if (class_id == idigi_class_operating_system && request_id.os_request == idigi_os_malloc)
 *     {
 *         size_t size = *(size_t *)request_data;
 *         void ** ptr = (void **)response_data;
 *
 *         *ptr = malloc(size);
 *         if (*ptr == NULL)
 *         {
 *             return idigi_callback_abort;
 *         }
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section free free
 *
 * Callback is called to free previously allocated memory.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_os_free() in os.c.
 *
 * @see app_os_free()
 * @see app_os_malloc()
 * @see @ref idigi_os_malloc
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_os_free @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Pointer address to be freed </td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> N/A </td>
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
 * <th>None</th>
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
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id == idigi_class_operating_system && request_id.os_request == idigi_os_free)
 *     {
 *         free((void *)request_data);
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section uptime System Uptime
 * This callback is called to return system up time in seconds. It is the time 
 * that a device has been up and running.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_os_get_system_time() in os.c.
 *
 * @see app_os_get_system_time()
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_os_system_up_time @htmlonly</td>
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
 * <td> Pointer to unsigned long integer memory where callback writes the system up time to (in seconds) </td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td> N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_continue @htmlonly</th>
 * <td>Callback successfully returned the system time</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_abort @htmlonly</th>
 * <td>Error occurred and callback aborted IIK</td>
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
 *     if (class_id == idigi_class_operating_system && request_id.os_request == idigi_os_system_up_time)
 *     {
 *         time((time_t *)response_data);
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section sleep Sleep
 * This callback is called to relinquish control in the @ref threading "multi-threaded" idigi_run() model.
 *
 * This callback is trapped in application.c, in the @b Sample section of @ref AppStructure "Public Application Framework"
 * and implemented in the @b Platform function app_os_sleep() in os.c.
 *
 * @see app_os_sleep()
 * @see @ref threading "Multi-threading and the IIK Threading Model"
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>class_id</th>
 * <td>@endhtmlonly @ref idigi_class_operating_system @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_id</th>
 * <td>@endhtmlonly @ref idigi_os_sleep @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td>Number of seconds to sleep up to (the user can sleep less)</td>
 * </tr>
 * <tr>
 * <th>request_length</th>
 * <td> The size of request_data which is unsigned int </td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> N/A</td>
 * </tr>
 * <tr>
 * <th>response_length</th>
 * <td> N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_continue @htmlonly</th>
 * <td>Callback successfully was able to sleep</td>
 * </tr>
 * <tr>
 * <th>@endhtmlonly @ref idigi_callback_abort @htmlonly</th>
 * <td>Error occurred and callback aborted IIK</td>
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
 *     if (class_id == idigi_class_operating_system && request_id.os_request == idigi_os_sleep)
 *     {
 *         unsigned int *timeout_seconds = request_data;
 *         usleep(*timeout_seconds * MICROSECONDS_PER_SECOND);
 *     }
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 */
