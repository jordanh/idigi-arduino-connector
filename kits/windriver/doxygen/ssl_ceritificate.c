/*! @page  ssl_certificate SSL Ceritifcate
 * 
 * @htmlinclude nav.html
 *
 * @section remote_config_overview Overview
 *
 * Remote configuration is an optional service for applications to 
 * exchange device configruation data and information between the device and the iDigi Device Cloud using
 * remote command interface (RCI). 
 * User must define all device configurations that are accessed by the iDigi Device Cloud and run the @ref rci_tool
 * to generate a source and header files for remote configuration support.
 * 
 * In this version of the iDigi Connector Kit only minimal RCI support is provided.  Whenever a configueation
 * item is changed the user must run the @ref rci_tool, this communicates the change with the iDigi Device Cloud.
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
 * @section modifying_device_name Device Name
 *
 *  @b If @b you @b modify @b the @b name @b of @b the @b device @b you @b must @b rerun @b the @ref rci_tool; this informs the iDigi Device Cloud
 *  that the name of the device has changed.
 *
 *
 * @htmlinclude terminate.html
 */

