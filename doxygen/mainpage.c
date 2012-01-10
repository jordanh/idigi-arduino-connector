 /*! @mainpage iDigi Integration Kit
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents Overview
 *
 *          -# @ref intro
 *          -# @ref language
 *          -# @ref requirements
 *              -# @ref CodespaceRequirements
 *              -# @ref RAMRequirements 
 *              -# @ref StackRequirements 
 *          -# @ref features
 *          -# @ref communicating
 *          -# @ref threading
 *          -# @ref code_organization
 *          -# @ref zlib
 *          -# @ref the_getting_started_process
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
 * The IIK software provided is ANSI X3.159-1989 (ANSI C89) and ISO/IEC 9899:1999 (ANSI C99) compliant. The sample platforms 
 * provided use standard ANSI C calls which are available in most operating systems.  If you are running on a Linux i486 based platform 
 * and using the GNU toolchain the Linux platform and samples can be run without any modifications.
 *
 * @section requirements Platform Memory Requirements
 * The IIK requires both Flash and RAM.  Flash is needed to store instructions and variables.  RAM is needed for dynamic allocation
 * and program stack.  
 *     
 * @subsection CodespaceRequirements Code Space Estimates 
 * The following Flash/Code space data was generated using Ubuntu 4.4.3 gcc with the compile_and_link sample application.  The
 * build used optimization for size (-Os) and disabled @ref IDIGI_COMPRESSION in all cases.  The metrics exclude any code space 
 * required for application layer calls.  This information should be treated as a typical use case.  
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">IIK Service Options and Configuration Methodology</td>
 * <th class="title">Text</td>
 * <th class="title">rodata</td>
 * <th class="title">data</td>
 * <th class="title">bss</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DEBUG, @ref IDIGI_FIRMWARE_SERVICE, @ref IDIGI_DATA_SERVICE @htmlonly enabled.</td>
 * <td>20285</td>
 * <td>6840</td>
 * <td>0</td>
 * <td>12</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FIRMWARE_SERVICE, @ref IDIGI_DATA_SERVICE @htmlonly enabled.</td>
 * <td>16400</td>
 * <td>372</td>
 * <td>0</td>
 * <td>0</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FIRMWARE_SERVICE, @ref IDIGI_DATA_SERVICE enabled.  @ref idigi_config_data_options used. @htmlonly </td>
 * <td>15545</td>
 * <td>372</td>
 * <td>0</td>
 * <td>0</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FIRMWARE_SERVICE enabled.  @ref idigi_config_data_options used. @htmlonly </td>
 * <td>9813</td>
 * <td>280</td>
 * <td>0</td>
 * <td>0</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DATA_SERVICE enabled.  @ref idigi_config_data_options used. @htmlonly </td>
 * <td>12413</td>
 * <td>244</td>
 * <td>0</td>
 * <td>0</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * References to Text, rodata, data, and bss refer to program instructions, Constant-Read-only data, Initialized global and 
 * static variables, and Uninitialized zero-ed data, respectively.  References to the @ref idigi_config_data_options refers to 
 * the code reduction defines in idigi_config.h: @ref IDIGI_DEVICE_TYPE, @ref IDIGI_CLOUD_URL, @ref IDIGI_TX_KEEPALIVE_IN_SECONDS, 
 * @ref IDIGI_RX_KEEPALIVE_IN_SECONDS, @ref IDIGI_WAIT_COUNT, @ref IDIGI_VENDOR_ID, @ref IDIGI_MSG_MAX_TRANSACTION, @ref IDIGI_CONNECTION_TYPE, 
 * @ref IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND, @ref IDIGI_WAN_PHONE_NUMBER_DIALED, @ref IDIGI_FIRMWARE_SUPPORT, and @ref IDIGI_DATA_SERVICE_SUPPORT.
 * 
 * @subsection RAMRequirements Dynamic RAM Usage 
 * The following dynamic RAM usage was developed by monitoring the high water mark during the 
 * @ref idigi_os_malloc @ref idigi_callback_t "application-defined callback".  
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Description</td>
 * <th class="title">IDIGI_COMPRESSION enabled</td>
 * <th class="title">IDIGI_COMPRESSION disabled</td>
 * <th class="title">Notes</td>
 * </tr>
 * <tr>
 * <td>IIK Base usage</td>
 * <td>2436</td>
 * <td>2436</td>
 * <td>Memory usage for internal state machines, infrastructure and communication buffers</td>
 * </tr>
 * <tr>
 * <td>With @endhtmlonly @ref IDIGI_FIRMWARE_SERVICE @htmlonly enabled</td>
 * <td>108</td>
 * <td>108</td>
 * <td></td>
 * </tr>
 * <tr>
 * <td>With @endhtmlonly @ref IDIGI_DATA_SERVICE @htmlonly enabled</td>
 * <td>92 + 3320*n</td>
 * <td>92 + 56*n</td>
 * <td>Where n is the maximum number of simultaneous message transactions, defined in the @endhtmlonly @ref IDIGI_MSG_MAX_TRANSACTION @htmlonly </td>
 * </tr>
 * <tr>
 * <td>Total</td>
 * <td>5920</td>
 * <td>2656</td>
 * <td>Assumes both @endhtmlonly @ref IDIGI_FIRMWARE_SERVICE and @ref IDIGI_FIRMWARE_SERVICE @htmlonly enabled, with n=1 (one data service message transaction maximum)</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *  
 * @subsection StackRequirements Program Stack Usage 
 * The following program stack usage was observed using gcc 4.2.0 compiled for a 32-bit ARM9 TDMI architecture.  
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Build Details </td>
 * <th class="title"> Program stack size</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DEBUG @htmlonly disabled</td>
 * <td>1723</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DEBUG @htmlonly enabled</td>
 * <td>2091</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *   
 * This includes all code from within the private IIK library, but none of the code implemented in the @ref idigi_callback_t 
 * "application-defined callback".  These metrics should be treated as typical.  It is recommended that the actual program stack size 
 * used to call @ref idigi_run() or idigi_step() be these metrics plus the amount of memory typical for handling a TCP network client, 
 * or a thread that accesses local configuration, or makes malloc/free OS calls, whichever is the worst case.
 * 
 * @section features IIK Features
 *     @li Send data to and from a device over the iDigi Device Cloud
 *     @li Update firmware on the device
 *     @li View the device configuration
 *     @li Reboot a device
 *
 * @section communicating Communicating with your device
 *
 * To manage your device you can use the iDigi Device Cloud Portal, this contains
 * the graphical user interface to the iDigi Device Cloud, this tool allows for
 * complete management of your device.
 *
 * To communicate with your device remotely without using the graphical interface
 * to the iDigi Device Cloud you can use @b iDigi @b Web @b Services.  Web service 
 * requests are sent from an application to the iDigi Device Cloud, the iDigi
 * Device Cloud then communicates with the device, this allows for bidirectional
 * machine to machine communication.  Python samples are provided to show how to
 * communicate to a device over the iDigi Device Cloud.
 *
 * @section threading Threading Model
 *
 * The IIK can be deployed in a multithreaded (idigi_run()) or round robin control loop (idigi_step()) environment.    
 * In environemnts that include preemptive threading, the IIK can be implemented as a seperate stand-alone thread
 * by calling idigi_run().  This a blocking call that only returns on a major system failure.
 *    
 * Alternatively, when threading is unavailable, in a round robin control loop or fixed state machine, the IIK can 
 * be implemented using the non-blocking idigi_step() call within the round robin control loop.  
 * 
 * Note in a cooperative, non-preemptive multithreaded environment, either idigi_run() or idigi_step() can used, based on 
 * system determinism and the potential for a non-cooperative thread to exceed the IIK's system timing.  
 * 
 * @note You should decide before proceeding how you intend to call the IIK (within a round robin control loop or running 
 * as a separate thread).  In a limited services OS with no real time threading, you should use the idigi_step() routine.  
 * Otherwise you should use the idigi_run() routine.
 *
 * @section code_organization Source Code Organization
 *
 * When uncompressed the directory structure below will be created in the idigi directory.
 * The public directory is divided into step and run, use the appropriate directory
 * based on your threading model.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Directory</td>
 * <th class="title">Description</td>
 * </tr>
 * <tr>
 * <td rowspan="2">private</td>
 * <td style="border-bottom: 0px none;">IIK Library Code  <strong>(do not modify)</strong></td>
 * </tr><tr>
 * <td style="border-top: 0px none;">This directory contains all the private and internal files used to build the IIK library. 
 * <b>You should never modify files in this directory</b>. These files are only provided 
 * so the library can be built using the tool chain for your platform.</td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/include</td>
 * <td style="border-bottom: 0px none;">Public API</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">This directory contains the IIK public header files required for application development.
 * When porting to a new platform, you may need to modify @endhtmlonly @ref idigi_types.h @htmlonly to match your platform's characteristics (i.e., data size and supported compiler data types).
 * The file @endhtmlonly @ref idigi_api.h @htmlonly in this directory is the IIK public API.</td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/run</td>
 * <td style="border-bottom: 0px none;">Platforms and samples for running the IIK as a separate thread</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">This directory contains platforms and samples which run the IIK as a separate 
 * thread in a multitasking environment.</td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/step</td>
 * <td style="border-bottom: 0px none;">Platforms and samples for stepping the IIK in a round robin control loop</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">This directory contains platforms and samples which step the IIK in a round robin state machine or 
 * control loop model.  These samples are for those systems that do not include pre-emptive RTOS.  </td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/run/platforms</td>
 * <td style="border-bottom: 0px none;">Platforms for running the IIK as a separate thread</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">For each supported platform there is a subdirectory along with a set of interface routines.
 * The Getting Started Procedure will walk you through setting up your platform.</td>
 * </tr>
 * <tr>
 * <td>public/run/platforms/linux</td>
 * <td>Code used to interface to a linux system</td>
 * </tr>
 * <tr>
 * <td>public/run/platforms/template</td>
 * <td>Shell routines to implement a new platform from scratch</td>
 * </tr>
 * <tr>
 * <td rowspan="2">public/run/samples</td>
 * <td style="border-bottom: 0px none;">Samples for the IIK</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">Samples on how to use the IIK, the compile_and_link sample is used to verify
 * that your new envirorment is able to build. There is a sample for each major
 * feature in the IIK, there is documentation in this guide for each sample.</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @section zlib Optional Data Compression Support
 * The IIK has an optional Data Compression switch that reduces the amount of network traffic.  This option requires applications 
 * to link with the zlib library and add the zlib header file (zlib.h) to the IIK include path.
 *   
 * @note Enabling this option greatly increases the application code size and memory required to execute.   
 * 
 * If your application requires Data compression, but your development environment does not include the zlib library, 
 * you will need to download and build the library.  The zlib home page is located at: http://zlib.net/.   Instructions 
 * on how to build zlib are provided with the package.
 * 
 * @note The zlib library is required only if your application enables the Data Compression switch.  
 *   
 * @section the_getting_started_process Getting Started 
 * To get started, follow along the steps of the @ref getting_started process.
 *
 */

