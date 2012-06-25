/*! @page web_services iDigi Web Services
 *
 * @htmlinclude nav.html
 *
 * @section services iDigi Web Services
 *
 * Applications can communicate to devices over the iDigi Cloud using iDigi Web Services,
 * requests are sent to the iDigi server using Server Command Interface (SCI).
 * The iDigi connector supports a subset of the SCI operations supported by iDigi, the
 * supported SCI operations are listed in the table below:
 *
 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</th>
 * <th  class="title">Description</th>
 * </tr>
 * <tr>
 * <td>update_firmware</td>
 * <td>Updates the firmware of the device</td>
 * </tr>
 * <tr>
 * <td>disconnect</td>
 * <td>Sends a request to the device to disconnect from the server</td>
 * </tr>
 * <tr>
 * <td>query_firmware_targets</td>
 * <td>Gets a list of firmware targets on the device</td>
 * </tr>
 * <tr>
 * <td>data_service</td>
 * <td>Sends messages to devices over the data service</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 *
 * To learn more on how to use iDigi Web Services read this document, refer to
 * Chapter 5. SCI (Server Command Interface).
 *
 * @htmlonly <a href="http://developer.idigi.com/edocs/downloads/90002008_C.pdf">iDigi Web Services Programming Guide</a>@endhtmlonly
 *
 *
 * @note To access this document you will have to login to iDigi
 *
 * @section Sample Web Services Applications
 *
 * Most sample Applications have associated web services client code that demonstrates how
 * a web services client can interact with the iDigi Cloud.  These samples are written using
 * Python.
 *
 * @htmlinclude terminate.html
 *
 */
