 /*! @mainpage iDigi Integration Kit
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents Overview
 *
 *          -# @ref intro
 *          -# @ref language
 *          -# @ref requirements
 *          -# @ref features
 *          -# @ref communicating
 *          -# @ref threading
 *          -# @ref code_organization
 *          -# @ref zlib
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
 * The software provided is C89, C99 and ANSI compliant. The sample platforms provided use standard C 
 * calls which are available in most operating systems.  If you are running on a Linux i486 based platform 
 * and using the GNU toolchain the Linux platform and samples can be run without any modifications.
 *
 * @section requirements Platform Requirements
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Description</td>
 * <th class="title">Size With Compression On</td>
 * <th class="title">Size With Compression Off</td>
 * <th class="title">Notes</td>
 * </tr>
 * <tr>
 * <td>Heap</td>
 * <td>5920 bytes</td>
 * <td>2656 bytes</td>
 * <td>IIK memory usage</td>
 * </tr>
 * <tr>
 * <td>Stack</td>
 * <td>TBD bytes</td>
 * <td>TBD bytes</td>
 * <td>Maximum Stack Usage</td>
 * </tr>
 * <tr>
 * <td>Code</td>
 * <td>17434 bytes</td>
 * <td>16319 bytes</td>
 * <td>On a 32-bit i486 Linux machine, using GCC v4.4.4, optimized for size (-Os)</td>
 * </tr>
 * </table>
 * @endhtmlonly
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
 * The IIK can be run in a multithreaded or single threaded environment, it is
 * suggested in a multithreaded OS that the IIK be run as a separate thread, in a
 * single threaded system a step routine is provided which runs a portion of the
 * IIK and must be called periodically.  In the multithreaded model the IIK provides
 * a routine which executes the IIK and does not return, this routine is intended
 * to be run as separate thread.  The IIK is designed to run in a preemptive or
 * cooperative multithreaded system, the run routine will call sleep to relinquish
 * control of the CPU.
 *
 * @note You must decide before proceeding if you want to run as a single threaded
 * model (step) or run the IIK as a separate thread (run).  In a small OS with no
 * RTOS you should use the step routine, otherwise you should use the run routine.
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
 * <td style="border-top: 0px none;">It contains all the files which are used to build the IIK library. 
 * The user <b> should not modify any files in this directory</b>. These files are only provided 
 * so the library can be built using the tool chain for your platform.</td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/include</td>
 * <td style="border-bottom: 0px none;">Public API</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">It contains header files used globally by the IIK.
 * When porting to a new platform the user may need to modify the file @endhtmlonly @ref idigi_types.h. @htmlonly
 * The file @endhtmlonly @ref idigi_api.h @htmlonly in this directory is the IIK public API.</td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/run</td>
 * <td style="border-bottom: 0px none;">Platforms and samples for running the IIK as a separate thread</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">It contains platforms and samples which run the IIK as a separate 
 * thread in a multitasking environment.</td>
 * </tr>
 * <tr>
 * <td rowspan=2>public/step</td>
 * <td style="border-bottom: 0px none;">Platforms and samples for running the IIK in a single threaded model</td>
 * </tr><tr>
 * <td style="border-top: 0px none;">It contains platforms and samples which run the IIK as a single thread
 * in a non-multhreaded model.</td>
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
 * The IIK has a Data Compression option used to reduce the amount of network traffic.  The option 
 * requires applications to link with the zlib library and add the zlib header file (zlib.h) to the IIK include path.   
 * 
 * @note The zlib library is only required if your application enables Data Compression.  
 * Enabling this option greatly increases the application code size and memory required to execute.
 *   
 * If your application requires Data compression, but your development environment does not include the zlib library, 
 * you will need to download and build the library.  The zlib home page is located at: http://zlib.net/.   Instructions 
 * on how to build zlib are provided with the package.
 * 
 * @note The next step is to read the @ref getting_started page.
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
/** 
* \def KIT_NAME 
* IIK 
*/ 


