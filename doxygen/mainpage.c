 /*! @mainpage iDigi Intergration Kit
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents Overview
 *
 *          -# @ref intro
 *              -# @ref features
 *          -# @ref requirements
 *          -# @ref language
 *          -# @ref code_organization
 *              -# @ref private
 *              -# @ref public
 *              -# @ref samples
 *          -# @ref architecture
 *          -# @ref zlib
 *          -# @ref idigi
 *              -# @ref idigi_account
 *              -# @ref idigi_login
 *              -# @ref idigi_vendor_id
 *              -# @ref idigi_device_id
 *
 * @section intro Introduction
 *
 * The iDigi Integration Kit (IIK) is a software development package used to
 * communicate and exchange information between a device and the iDigi Device
 * Cloud. iDigi supports application to device data interaction (messaging),
 * application & device data storage, and remote management of devices. Devices
 * are associated with the iDigi Device Cloud through the Internet or other wide
 * area network connection, which allows for communication between the device, the
 * iDigi Device Cloud, and customer applications. An important part of this communication
 * is the transfer of data from a device to the iDigi Device Cloud.
 * iDigi is based upon a cloud computing model that provides on-demand scalability so
 * you can rest assured that when you need additional computing and storage iDigi will
 * scale to meet your needs. The iDigi Device Cloud is designed using a high-availability
 * architecture, with redundancy and failover characteristics in mind.
 * Using iDigi, customers can now easily develop cloud connected devices and
 * applications that quickly scale from dozens to hundreds, thousands or even
 * millions of endpoints.
 *
 * @image html cloud.jpg 
 *
 * @section language Language Support
 *
 * The software provided is C99 and ANSI compliant. The sample provided uses standard C 
 * calls which are available in most operating systems; the networking portion of the 
 * sample uses Berkley sockets calls.  If you are running on a Linux i486 based platform 
 * and using the GNU toolchain the linux sample can be run without any modifications.
 * 
 * @section requirements Platform Requirements
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Description</td>
 * <th>Typical Size</td>
 * <th>Notes</td>
 * </tr>
 * <tr>
 * <td>Heap Space: IIK Base</td>
 * <td>5516 bytes</td>
 * <td>IIK base usage</td>
 * </tr>
 * <tr>
 * <td>Heap Space: With firmware facility</td>
 * <td>+1684 bytes</td>
 * <td>With firmware facility included</td>
 * <tr>
 * <td>Heap Space: With data service</td>
 * <td>+1676 + 2 * n * 1696 bytes</td>
 * <td>With data service included and n simultaneous data service sessions</td>
 * </tr>
 * <tr>
 * <td>Code Space</td>
 * <td>17393 bytes</td>
 * <td>On a 64-bit i686 Linux machine, using GCC v4.4.4, optimized for size (-Os)</td>
 * </tr>
 * <td>Disk Space</td>
 * <td>2.1 Mb</td>
 * <td>Tarball plus unzipped tree size</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *  
 * @subsection features IIK Features
 *     @li Send data from devices to the iDigi Device Cloud
 *     @li Update firmware in the devices
 *     @li View and change the configuration settings of a device
 *     @li Reboot a device
 *
 * @section code_organization Source Code Organization
 * The IIK source code ships in a compressed image with the format idigi_iik_x.x.xx.tar.gz. 
 * To unzip the file use the following commands: tar -xvzf idigi_iik_x.x.xx.tar.gz
 * When uncompressed the directory structure below will be created in the idigi subdirectory. 
 * The source code for the IIK is organized into several top-level directories described below.
 * @note The code in the private directory should never be modified.
 *
 * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Directory</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>private</td>
 * <td>IIK Library Code</td>
 * </tr>
 * <tr>
 * <td>public/include</td>
 * <td>Public API</td>
 * </tr>
 * <tr>
 * <td>public/samples</td>
 * <td>Examples of using the IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @subsection private private Directory
 * The private directory contains all the files which are used to build the IIK library. 
 * The user doesn't need to modify any files in this directory. These files are only provided 
 * so the library can be built using the tool chain for your platform.
 *
 * @subsection public include Directory
 * The public/include directory contains header files used globally by the IIK. 
 * When porting to a new platform the user may need to modify the file idigi_types.h. 
 * The file idigi_api.h in this directory is the IIK public API.
 *
 * @subsection samples samples Directory
 * For each supported platform there is a subdirectory along with a sample, for Linux 
 * based platforms there is sample/linux. When porting to a new platform you will copy 
 * and modify an existing sample.
 *
 * @section zlib zlib Support
 * The zlib software library is used for data compression by the IIK, the zlib library 
 * is required if compression is to be supported by your device. Data compression is 
 * used to reduce the network traffic load. If your application does not require compression, 
 * skip this section. The zlib source code is not under the GNU license, the license is described in zlib.h.
 * The zlib home page is located at: http://zlib.net/. If your platform does not already have the zlib library 
 * you will need to download and build the zlib library. The header file zlib.h is included by the IIK and 
 * must be in the include path (described in the @ref building section) for the IIK library. 
 *
 * @section idigi iDigi
 * The iDigi Device Cloud provides secure application messaging, data storage and device management 
 * for networks comprised of wired, cellular and satellite-connected devices.
 *
 * @subsection idigi_account Create an iDigi Account
 * Before getting started you will need to create a new iDigi account, to create an account navigate to
 * https://developer.idigi.com/user_registration.do and fill out the iDigi registration form. 
 * If you are a current iDigi developer account user, login with your existing user name and password and proceed to:
 *
 * @image html idigi1.jpg 
 *
 * @subsection idigi_login Login to the iDigi Server
 * 1. Navigate to http://www.idigi.com.
 *
 * 2. Click the iDigi Login button in the upper right corner of the page.
 *
 * @image html idigi2.jpg 
 *
 * 3. Click the iDigi Developer Cloud Login button.
 *
 * @image html idigi3.jpg
 * 
 * You will be redirected to the iDigi Developer Cloud login page.
 *
 * 4. Login with the user credentials you created in section @ref idigi_account
 * 
 * @image html idigi4.jpg
 *
 * @subsection idigi_vendor_id Obtain an iDigi Vendor ID
 *
 * @note You will need to contact Digi to request a vendor ID. You will need the vendor ID 
 * before your device can connect to iDigi.
 *
 * To request a vendor ID within iDigi:
 *
 * 1. Click on My Account within the left navigation panel.
 *
 * 2. Click the Register for new vendor id button.
 *
 * @image html idigi5.jpg
 *
 * The page will refresh and your unique vendor ID number will be displayed in place of the
 * Register for new vendor id button.
 *
 * @image html idigi6.jpg
 *
 * @note When you are ready to deploy in production, contact Digi customer support in order to move your 
 * vendor ID to my.idigi.com
 *
 * @subsection idigi_device_id Obtain an iDigi Device ID
 *
 * The customer is responsible for creating their own unique device ID. Device IDs are used 
 * to identify devices in iDigi. A device ID is a 16-octet number that is unique to the device 
 * and does not change over its lifetime. A device ID is derived from globally unique values 
 * already assigned to a device (such as a MAC address, IMEI, etc).
 * The canonical method for writing device IDs is as four groups of eight hexadecimal digits 
 * separated by a dash. An example device ID is: @b 01234567-89ABCDEF-01234567-89ABCDEF
 *
 * @section architecture Architecture 
 *  
 * The iDigi platform is an on-demand hosted service platform with no infrastructure 
 * requirements for the user. iDigi provides device management, real-time device messaging, 
 * and data storage services for a network comprised of both wired and wireless devices. 
 * iDigi provides easy integration with M2M and mesh networking devices.
 * The IIK consists of the IIK library and sample user applications. The IIK library provides  
 * all the functionality required to communicate with the iDigi Device Cloud. The user sample 
 * applications provide easy to use and understand demonstration use cases for configuration, 
 * OS interfaces and communication with the IIK library.
 *
 *   
 * The IIK API includes two major software interfaces:
 *      @li IIK function calls
 *      @li IIK application-defined callback
 * 
 *  The functions available in the IIK are listed below:
  * @htmlonly
 * <table border="8">
 * <tr>
 * <th>Routine</td>
 * <th>Description</td>
 * </tr>
 * <tr>
 * <td>idigi_init</td>
 * <td>Start the IIK</td>
 * </tr>
 * <tr>
 * <td>idigi_step</td>
 * <td>Execute the IIK and return</td>
 * </tr>
 * <tr>
 * <td>idigi_run</td>
 * <td>Start the IIK and do not return</td>
 * </tr>
 * <tr>
 * <td>idigi_initiate_action</td>
 * <td>Tell the IIK to perform some action</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @image html arch1.jpg
 *
 * The block diagram above shows the software API components. The application 
 * defined callback is a generic mechanism to execute methods inside the users' application.
 * The details of the API and callback method are described in detail in the @ref api section.
 * The user links there application to the IIK library and then uses the API
 * to communicate with the iDigi server.  The user can then login to the iDigi
 * server and communicate with their device.
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 */

/*! @page troubleshooting Troubleshooting
  Read this page if you are having trouble with your device.
  @section sec An example section
  This page contains the subsections @ref subsection1 and @ref subsection2.
  For more info see page @ref page2.
  @subsection subsection1 The first subsection
  Text.
  @subsection subsection2 The second subsection
  More text.
*/

/*! @page sample_code Examples
  Read this page if you are having trouble with your device.
  @section sec An example section
  This page contains the subsections @ref subsection1 and @ref subsection2.
  For more info see page @ref page2.
  @subsection subsection1 The first subsection
  Text.
  @subsection subsection2 The second subsection
  More text.
*/


