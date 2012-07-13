 /*! @page  rci_config Remote Configuration
 * 
 * @htmlinclude nav.html
 *
 * @section remote_config_overview Overview
 *
 * Remote configuration is a service for applications to 
 * exchange device configruation data and information between the device and the iDigi Device Cloud using
 * remote command interface (RCI). 
 * 
 * In this version of the iDigi Connector Kit only minimal RCI support is provided. A default RCI configuration
 * is provided with the elements in the table below.  The user only needs to run the @ref rci_tool if the
 * any of the following are modified:
 *       -# @ref device_type
 *       -# iDigi server
 *       -# iDigi account
 *       -# Version number
 * 
 * The syntax for running the rci tool can be found @ref rci_tool_usage "here".
 * 
 * The following table describes the configuration items supported in this release.
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</th>
 * <th  class="title">Description</th>
 * </tr>
 * <tr>
 * <td>Description</td>
 * <td>Description of the device</td>
 * </tr>
 * <tr>
 * <td>Contact</td>
 * <td>Contact information for the device</td>
 * </tr>
 * <tr>
 * <td>Location</td>
 * <td>Location Information</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section remote_config_file Configuration File
 * 
 * The remote configuration file config.rci is provided in the /home/wruser/idigi directory, this contains the configruation
 * information defined above.
 *
 * # iDigi Manager Pro queries this system setting to display the information in root folder
 * group setting system "System"
 *   element description "Description" type string max 63
 *   element contact "Contact" type string max 63
 *   element location "Location" type string max 63
 *   error invalid_length "Invalid Length"
 *   error no_memory "Insufficient memory"
 *
 * @section device_type Device Type
 *
 *  If you modify the device type you must rerun the @ref rci_tool; this informs the iDigi Device Cloud
 *  that the name of the device has changed.  The default device type is "Digi Intel Linux Device".
 *
 *
 * @htmlinclude terminate.html
 */
