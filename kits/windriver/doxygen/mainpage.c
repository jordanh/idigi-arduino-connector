 /*! @mainpage iDigi Connector Kit
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents Overview
 *          -# @ref intro
 *          -# @ref features
 *          -# @ref idigi_package
 *          -# @ref communicating
 *          -# @ref ssl_support
 *          -# @ref required_libraries
 *          -# @ref config_file
 *          -# @ref debug_library
 *          -# @ref file_system
 *          -# @ref the_getting_started_process
 *
 * @section intro Introduction
 *
 * The iDigi Connector Kit is a software development package used to
 * communicate and exchange information between a device and the iDigi Device Cloud. 
 * iDigi supports application to device data interaction (messaging),
 * application & device data storage, and remote management of devices. Devices
 * are associated with the iDigi Device Cloud through the Internet or other wide
 * area network connections, which allows for communication between the device, the
 * iDigi Device Cloud, and customer applications. An important part of this communication
 * is the transfer of data from a device to the iDigi Device Cloud.
 * iDigi is based upon a cloud computing model that provides on-demand scalability so
 * you can rest assured that when you need additional computing and storage, iDigi will
 * scale to meet your needs. The iDigi Device Cloud is designed using a high-availability
 * architecture, with redundancy and failover characteristics in mind.
 * Using iDigi, customers can now easily develop cloud connected devices and
 * applications that quickly scale from dozens to hundreds, thousands or even
 * millions of endpoints.
 *
 * @image html cloud.jpg 
 *
 * @section features iDigi Connector Features
 * 
 * The following is a list of features available using the iDigi Device Cloud:
 * 
 *     @li Send data to and from a remote device
 *     @li Access the file system on the device
 *     @li Upload and download files to the device
 *     @li View and modify the device configuration
 *     @li Reboot the device
 *     @li SSL support
 *
 * @section communicating Communicating with your device over iDigi
 * 
 * All devices in the iDigi Device Cloud are assigned a device ID, the device ID is derived from the MAC
 * address of the device. The value 0xFFFF is inserted in the MAC address to form the device
 * ID, and example is shown below.
 * 
 * Example MAC: 112233:445566
 * 
 * Device ID mapping: 00000000-00000000-112233FF-FF445566
 *
 * When you walk through the @ref the_getting_started_process "getting started process" you will provision your device, this will
 * register your device with the iDigi Device Cloud and push up information about your device.
 * 
 * @b Before @b you @b can @b communicate @b with @b iDigi @b your @b device @b must @b be @b provisioned.
 * 
 * To manage your device you can use the <a href="http://www.idigi.com/idigimanagerpro">iDigi Manager Pro</a>
 * interface by logging into your @ref step4 "iDigi Device Cloud account".  Alternatively, you can communicate with
 * your device programmatically by using @ref web_services "iDigi Web Services".
 *
 * @ref web_services "iDigi Web Services" requests are sent from a remote application to
 * the iDigi Device Cloud, which then directly communicates to the device.  This
 * allows for bidirectional machine to machine communication.  Each iDigi connector sample includes
 * a Python application demonstrating how to communicate to a device using the
 * @ref web_services "iDigi Web Services".
 *
 * @section idigi_package iDigi in Wind River Linux
 *
 * The iDigi Connector Kit is provided as a source package (SRPM) in the Wind River Linux environment.
 * The library provided contains all the functions necessary to communicate with the iDigi Device
 * Cloud.  The library contains the @ref api_overview "API" described in this document.  Samples are provided in the
 * Wind River Workbench which demonstrate how to use these API's to communicate with the iDigi Device Cloud.  The iDigi package is provided
 * with the Wind River distribution in the user space for the platform project.  The Wind River
 * Workbench is used to build, debug and run the iDigi Samples.  The @ref the_getting_started_process "getting started process"
 * walks you through using the Workbench and communicating with your Kontron device.
 * 
 * The components of the iDigi SRPM are:
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 *   <th class="title">File</th>
 *   <th class="title">Directory</th>
 *   <th class="title">Description</th>
 * </tr>
 * <tr>
 *   <td>libidigi.so</td>
 *   <td>/usr/lib</td>
 *   <td>iDigi connector library (non debug) which implements the iDigi Connector API and contains all the functionality to communicate to the iDigi Device Cloud</td>
 * </tr>
 * <tr>
 *   <td>libidigi_debug.so</td>
 *   <td>/usr/lib</td>
 *   <td>iDigi connector debug library which implements the iDigi Connector API and contains all the functionality to communicate to the iDigi Device Cloud.
 *   This library has ASSERTS turned on and has debug messages sent to stdout </td>
 * </tr>
 * <tr>
 *   <td>idigi_connector.h</td>
 *   <td>/usr/include</td>
 *   <td>Contains the iDigi Connector @endhtmlonly @ref api_overview "API" @htmlonly </td>
 * </tr>
 * <tr>
 *   <td>idigi.conf</td>
 *   <td>/etc</td>
 *   <td>Device configuration information</td>
 * </tr>
 * <tr>
 *   <td>idigi-ca-cert-public.crt</td>
 *   <td>/etc</td>
 *   <td>SSL public CA certificate</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * @section ssl_support SSL Support
 * 
 * The iDigi connector library uses a secure SSL connection, the CA certificate idigi-ca-cert-public.crt is stored
 * in the /etc directory and is included in the iDigi SRPM.  If the date on the device is incorrect an
 * error may be returned when validating the server certificate, to set the time and date on the device use the date command, as
 * shown below:
 * 
 * @htmlonly date +%Y%m%d -s "20120601" (replace the string with the correct date) @endhtmlonly
 * 
 * @section required_libraries Required Libraries
 * 
 * The following is a list of libraries which must be linked into to an application which uses
 * the iDigi library.
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</th>
 * <th  class="title">Description</th>
 * </tr>
 * <tr>
 * <td>pthread</td>
 * <td>POSIX threads</td>
 * </tr>
 * <tr>
 * <td>ssl</td>
 * <td>Open SSL library</td>
 * </tr>
 * <tr>
 * <td>crypto</td>
 * <td>Crytographic Libraries</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * @section config_file Configuration File
 * 
 * The iDigi library requires a configuration file which contains information about the device.
 * The configuration file is located in /etc/idigi.conf.  This file is automatically generated
 * when provisioning the device.  If you modify this file you need to run the @ref rci_tool, see the
 * section @ref rci_config if you modify this file.
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</th>
 * <th  class="title">Description</th>
 * </tr>
 * <tr>
 * <td>vendor_id</td>
 * <td>Vendor ID obtained from www.idigi.com</td>
 * </tr>
 * <tr>
 * <td>mac_addr</td>
 * <td>MAC Address of device used to create the Device ID</td>
 * </tr>
 * <tr>
 * <td>server_url</td>
 * <td>Crytographic Libraries</td>
 * </tr>
 * <tr>
 * <td>device_type</td>
 * <td>Device type in iDigi</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * @section debug_library Debug Library
 * 
 * Two libraries are provided as part of the iDigi SRPM; one is a debug version of the library
 * libidigi_debug.so which has ASSERTS turned on along with debugging output to stdout,
 * the library libidigi.so has the debug code turned off.  While writing and debugging your application
 * link in libidigi_debug.so; when deploying your application link in libidigi.so.
 * 
 * @section file_system File System Support
 * 
 * File system support allows access to files on the device remotely from the iDigi Device Cloud.
 * File system support is built into the iDigi library.  The File system can be accessed through
 * <a href="http://www.idigi.com/idigimanagerpro">iDigi Manager Pro</a> or by using
 * @ref web_services "iDigi Web Services".  The file permissions are dictated by the permissions of the user which starts the
 * iDigi process on the device.
 * 
 * @section the_getting_started_process Getting Started 
 * To get started, follow the steps of the <a href="/mnt/live/idigi/idigi.pdf">Getting Started Procedure</a>.
 * The getting started procedure will walk you through setting up your device and running a sample application.
 * Once you have completed the getting started procedure then continue to run one of the samples.
 *
 * @htmlinclude terminate.html
 */

