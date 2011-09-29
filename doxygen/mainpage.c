 /*! @mainpage IDigi Integration Kit Users Guide
 *
 * 
 * @section contents Contents
 *
 * @ref intro_sec
 *
 * @ref architecture_sec
 *
 * @ref porting_steps
 *
 * @ref api_details
 *
 * @ref troubleshooting
 *
 * @ref sample_code
 *
 * @section intro_sec Introduction
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
 * 
 *  
 * IIK Features
 *     @li Send data from devices to the iDigi Device Cloud: \url http://www.idigi.com
 *     @li Update firmware in the devices
 *     @li View and change the configuration settings of a device
 *     @li Reboot a device
 *
 * @section architecture_sec Architecture 
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
 * The IIK API includes two major software interfaces:
 *      @li IIK function calls
 *      @li IIK application-defined callback
 * 
 *  The functions available in the IIK are listed below:
 *      @li @ref idigi_init()
 *      @li @ref idigi_step()
 *      @li @ref idigi_run()
 *      @li @ref idigi_initiate_action()
 * 
 * @section porting_steps Porting Steps 
 *  
 * In this chapter we illustrate how to port the IIK to your platform. For a detailed explanation 
 * of the IIK public API see the section @ref api_details.
 *
 * The diagram below is a simplified flowchart of the porting process. 
 * 
 * @image html Drawing1.jpg 
 *  
 * @subsection step1 Step 1: Choose a Sample
 * 
 * The first step in porting is choosing which sample you are going to modify, in this guide we choose Linux as an example. 
 * The porting procedure is similar for all platforms, so choose a sample that is closest to your platform. 
 *  
 * @subsection step2 Step 2: Add Required Types and Macros
 * Open the file public/include/idigi_types.h in a text editor and verify that the 
 * required data types are supported by your platform. This file checks the __STD_VERSION__ macro's compliance to ISO/IEC 9899:1999. 
 * If C99 compliant, then standard C99 header files stdint.h and stdbool.h are included. 
 * Otherwise, if not C99 compliant, the folling data types are defined:
 *  @li @ref uint8_t
 *  @li @ref uint16_t
 *  @li @ref uint32_t
 *  @li @ref bool which is @ref true or @ref false
 * 
 * These defines assume at least a 32-bit machine. If your platform is not at 
 * least a 32-bit machine, these definitions should be reviewed.
 *
 * The IIK uses two debug macros; these are listed in the table below:
 * 
 *  @li @ref DEBUG_PRINTF
 *  @li @ref ASSERT 
 * 
 * If you don't have printf or assert available you can redefine them to call corresponding 
 * routines in your platform. 
 *
 * These are only used during debug and are turned off if the 
 * debug flag is not specified when building the IIK.  The DEBUG_PRINTF is used throughout the 
 * IIK and samples to display useful debug output.
 * 
 * @subsection step3 Step 3: Implement the Required OS routines
 * Open the file public/sample/linux/os.c and go through the routines listed below, 
 * and modify them for your platform.
 * 
 * The following is a list of the required OS routines:
 *  @li @ref os_malloc Dynamically allocate memory
 *  @li @ref os_free Free dynamically allocate memory
 *  @li @ref os_get_system_time Get the current time
 *
 *
 * @section api_details IIK Programming Interface
 * @subsection overview Overview
 * The IIK uses a single application-defined @ref idigi_callback_t "callback" function to process socket events, 
 * handle operating system (OS) calls, return device configuration data, and process firmware updates. 
 * The callback is also used to report error status and event notification. The samples provided 
 * within the IIK show examples of how to use the API.  The file public/include/idigi_api.h contains the 
 * API defined in this chapter.
 *
 *The IIK will invoke the application defined callback in used to perform the following actions:
 *   @li Obtain configuration information
 *   @li Notify the application of critical events and status
 *   @li Firmware download
 *   @li Network calls
 *
 * 
 * @subsection firmware_access Firmware Access Facility
 *
 * The firmware access facility is an optional facility for applications to update their firmware. 
 * The IIK invokes the application defined callback to query whether this firmware access facility is supported or not.
 * The firmware access facility uses a target number to distinguish application dependant firmware images. 
 * For example, if a standalone bootloader is separate from an application image, or a default file system image.
 * Applications define an image to each target except target 0 (target 0 must be the firmware image that is running the IIK). 
 * Only one firmware upgrade can be in progress at any given time. The IIK will send a firmware target list to the iDigi 
 * Device Cloud to identify the number of target applications and the version number of each target.
 *
 * The firmware portion of the application defined callback is used for the following:
 *      @li To obtain the number of firmware downloadable images.
 *      @li To obtain firmware image information and descriptions.
 *      @li To process firmware image upgrades.
 *      @li To reset firmware images.
 *
 * A typical application defined callback sequence for downloading a firmware image would include:
 *      -# IIK calls application defined callback to return firmware information which includes the firmware version number, maximum size of an image, firmware description, and file name spec.
 *      -# IIK calls application defined callback to initiate firmware download.
 *      -# IIK calls application defined callback to receive firmware binary data blocks.
 *      -# Repeat step 3 until the entire firmware image data is completed.
 *      -# IIK calls application defined callback to complete firmware download.
 *      -# IIK calls application defined callback to reset and reboot the target to begin executing the new firmware.
 *      -# IIK calls application defined callback to disconnect the current connection.
 *
 * Applications may choose to begin writing their downloaded image to Flash either 
 * during the Firmware Binary or Firmware Download Complete message. This is an application level 
 * decision with clear tradeoffs (memory needs versus power loss protection). If you write during 
 * the Firmware Binary data, you might lose communication or power at some point during the data transfer. 
 * If protection is built in (i.e., back up images) this would not be problem. If protections are not built in, 
 * applications might choose to place the entire image contents into RAM before writing to Flash. 
 * Back up image protection would still protect against power loss after your write occurred, but the communication 
 * loss or corruption could be avoided by having a making a checksum test before starting your Flash write.
 * Lastly, no application defined callback can block IIK execution. This application layer design requirement 
 * is most sensitive when considering writing firmware.
 *
 *
 * @subsection IIK API Details
 * 
 * This section describes the functions and prototypes used in the IIK API, these are defined in idigi_api.h.  Click on 
 * the function name for details.
 * 
 * @b Initialization
 * 
 * The API idigi_init() is called once upon startup to allocate and initialize the IIK. 
 * It takes the application defined callback function as an argument; this @ref idigi_callback_t "callback" is used by the IIK to communicate with the application. 
 * This function must be called before all other IIK APIs.
 *
 * @b Running @b the @b IIK @b in @b a @b single-threaded @b model
 * 
 * The function idigi_step() is called to start and run the IIK. This function performs a sequence of operations or events and returns control back to the caller. 
 * This allows a caller to perform other tasks, especially in single-threaded system. A caller must call this API again to continue IIK operations. 
 * The connection is already terminated when idigi_step returns an error, idigi_step will try reconnecting to the iDigi Device Cloud if it's called again.
 * The idigi_step() performs the following operations:
 *  -# Establish a connection with the iDigi Device Cloud.
 *  -# Wait for incoming messages from the iDigi Device Cloud.
 *  -# Invoke and pass message to the appropriate process (such as firmware access facility).
 *
 * 
 * @b Running @b the @b IIK @b in @b a @b multi-threaded @b model
 *
 * The function idigi_run() is similar to idigi_step() except it doesn't return control back to 
 * caller unless IIK encounters an error. This function should be executed as a separated thread.
 *
 * 
 * @b Sending @b data @b to @b the @b idigi @b Device @b Cloud
 * 
 * The function idigi_initiate_action() is used to request IIK to perform an action. 
 * This is used to send data from the device to the iDigi Device Cloud and to 
 * terminate the IIK library.
 *
 * 
 * @b IIK @b Callback
 * 
 * A single callback @ref idigi_callback_t "idigi_callback_t" is used to interface between an application and the IIK. Class and request 
 * enumeration values are used to identify the callback action. The application must perform 
 * the appropriate action and return the corresponding data according to the class and request 
 * enumeration values. The callback actions are listed below:
 *
 *  @li @ref idigi_os_malloc "malloc"
 *  @li @ref idigi_os_free "free"
 *  @li @ref idigi_os_system_up_time "System Uptime"
 *  @li Firmware Access Support
 *  @li Data Service Support
 *  @li Connect
 *  @li Send
 *
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
