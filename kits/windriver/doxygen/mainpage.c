 /*! @mainpage iDigi Connector Kit
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents Overview
 *
 *          -# @ref intro
 *          -# @ref features 
 *          -# @ref idigi_package
 *          -# @ref communicating
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
 * @section features iDigi connector Features
 *     @li Send data to and from a device using the iDigi Device Cloud
 *     @li Access the file system on the device
 *     @li Upload and download files to the device
 *     @li View and configure the device configurations
 *     @li Reboot the device
 *
 * @section communicating Communicating with your device
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
 * @subsection idigi_package iDigi in Wind River Linux
 *
 * The iDigi connector kit is provided as a source package (SRPM) in the Wind River Linux envirorment.
 * The library provided contains all the functions necessary to communicate with the iDigi device
 * cloud.  The library contains the API described in this document.  Samples are provided in the
 * Wind River Workbench which demonstrate how to use these API's.  The idigi package is provided
 * with the Wind River distrubution in the User space for the platform project.  The Wind River
 * Workbench is used to build, debug and run the iDigi Samples.
 *
 * 
 * The components of the idigi SRPM are
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
 *   <td>iDigi connector Library (non debug) Library which implements the iDigi Connector API and contains all the functionality to communicate to the iDigi Cloud</td>
 * </tr>
 * <tr>
 *   <td>libidigi_debug.so</td>
 *   <td>/usr/lib</td>
 *   <td>iDigi connector Library (non debug) Debug version of the library which implements the iDigi Connector API and contains all the functionality to communicate to the iDigi Cloud.
 *   This library has ASSERTS turned on and has debug messages sent to stdout </td>
 * </tr>
 * <tr>
 *   <td>idigi_connector.h</td>
 *   <td>/usr/include</td>
 *   <td>iDigi connector Library (non debug) Debug version of the library which implements the iDigi Connector API and contains all the functionality to communicate to the iDigi Cloud.
 *   This library has ASSERTS turned on and has debug messages sent to stdout </td>
 * </tr>
 * <tr>
 *   <td>idigi.conf</td>
 *   <td>/etc</td>
 *   <td>Default configuration file</td>
 * </tr>
 * <tr>
 *   <td>idigi-ca-cert-public.crt</td>
 *   <td>/etc</td>
 *   <td>SSL certificate</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section the_getting_started_process Getting Started 
 * To get started, follow along the steps of the <a href="/home/wruser/idigi/idigi.pdf">Getting Started Procedure</a>
 *
 * @htmlinclude terminate.html
 */

