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
 * is provided with the elements in the table below.  The user must run the @ref rci_tool if the
 * any of the following are modified:
 *       -# @ref device_type
 *       -# iDigi server
 *       -# iDigi account
 *       -# @ref version_number
 * 
 * The syntax for running the rci tool can be found @ref rci_tool_usage "here".
 * 
 * An example of changing the device type is shown below, you would
 * need to run the ConfigGenerator.jar tool which is located in /home/wruser/idigi.  In this example
 * we change the name of the device to "My Device Type", we need to modify /etc/idigi.conf to match this
 * and rerun the RCI configuration tool as shown below; a default config.rci is in the
 * /home/wruser/idigi directory.  The tool generates an output header file, for this release
 * the header file is not used.
 * 
 * java -jar ConfigGenerator.jar username:password "My Companies Device Type" 1.0.0.0 config.rci
 * 
 * @note The parameters passed to ConfigGenerator.jar must match the configuration file
 * /etc/idigi.conf.
 *
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
 * @section remote_config_file Remote Configuration File
 * 
 * The remote configuration file config.rci is provided in the /home/wruser/idigi directory, this contains the configruation
 * information defined above.
 *
 * # iDigi Manager Pro queries this system setting to display the information in root folder
 * 
 * group setting system "System"
 * 
 *   element description "Description" type string max 63
 * 
 *   element contact "Contact" type string max 63
 * 
 *   element location "Location" type string max 63
 * 
 *   error invalid_length "Invalid Length"
 * 
 *   error no_memory "Insufficient memory"
 *
 * @section device_type Device Type
 * 
 *  The device type is an iso-8859-1 encoded string which identifies the device. This string should be chosen by the device manufacturer
 *  as a name that uniquely identifies this model of device to the server. When the server finds two devices with
 *  the same device type, it can infer that they are the same product and product-scoped data may be shared among
 *  all devices with this device type. A device's type cannot be an empty string, nor contain only whitespace.
 *  If you modify the device type you must rerun the @ref rci_tool; this informs the iDigi Device Cloud
 *  that the name of the device has changed.  The default device type is "Digi Intel Linux Device".
 * 
 * 
 * @section version_number Version Number
 * 
 * The firmware version of this release is 1.0.0.0, currently the user cannot modify the version number.
 * This must be left at 1.0.0.0.
 * 
 *
 *
 * @htmlinclude terminate.html
 */
