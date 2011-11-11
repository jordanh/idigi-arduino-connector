/*! @page getting_started Getting Started
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents_porting Getting Started
 *
 *          -# @ref step1
 *              -# @ref debug_macros
 *          -# @ref step2
 *          -# @ref step3
 *              -# @ref idigi_initialization
 *              -# @ref add_files
 *              -# @ref add_path
 *              -# @ref build_sample
 *          -# @ref step4
 *              -# @ref idigi_login
 *              -# @ref idigi_vendor_id
 *              -# @ref idigi_device_id
 *          -# @ref step5
 *              -# @ref os_routines
 *              -# @ref network_routines
 *              -# @ref configuration_routines
 *              -# @ref application_start
 *          -# @ref step6
 *              -# @ref add_c_files
 *              -# @ref add_include_paths
 *          -# @ref step7
 *          -# @ref step8
 *              -# @ref add_your_device_to_the_cloud
 *              -# @ref view_result_on_cloud
 *          -# @ref step9
 * 
 * This getting started guide will walk you through the stpes necessary to
 * get an application running with the IIK, this application will connect your device to the
 * iDigi server and the configuration will be displayed in ConnectWare Manager.
 *
 * @section step1 Step 1: Determine if your compiler is C89 or C99 compliant
 *
 * If your compiler is not C89 or C99 compliant you will have to modify idigi_types.h. 
 * If C99 compliant, then the standard C99 header file stdint.h is included. Otherwise, 
 * if not C99 compliant, data types uint8_t, uint16_t, uint32_t are defined. Note these defines assume at 
 * least a 32-bit machine. If your platform is not at least a 32-bit machine, open up the 
 * file public/include/idigi_types.h and modify the types listed below.
 *
 *  @li @ref uint8_t
 *  @li @ref uint16_t
 *  @li @ref uint32_t
 *
 * @subsection debug_macros Implement the debug macros
 * The IIK uses the two debug macros listed below, review these definions and
 * modify them for your system.
 * 
 *  @li @ref USER_DEBUG_VPRINTF
 *  @li @ref ASSERT 
 * 
 * If you don't have printf or assert available you can redefine them to call corresponding 
 * routines in your platform. 
 *
 * These are only used during debug and are turned off if the 
 * debug flag is not specified when building the IIK.  The USER_DEBUG_VPRINTF is used throughout the 
 * IIK and samples to display useful debug output.
 *
 * @section step2 Step 2: Modify idigi_config.h and set the endianess
 *
 * Open the file public/include/idigi_config.h and set the endianess, and define @ref IDIGI_LITTLE_ENDIAN 
 * for little endian or comment out for big endian.
 *
 * @section step3 Step 3: Build the compile_and_link sample
 *
 * The first sample we are going to run is in the public/run/samples/compile_and_link directory,
 * this sample will verify that your build environment is setup correctly.
 * You need to add the source files and include paths to your build system described
 * below.
 *
 * @subsection idigi_initialization IIK Initialization
 *
 * The routine idigi_init() is called to initialize the IIK, currently this is
 * called from the routine main() in main.c, you will need to call idigi_init()
 * at the initialization point for your application. For the compile_and_link
 * sample you can call idigi_init as:
 *
 * (void)idigi_init((idigi_callback_t)0);
 *
 * This will cause the IIK library to be linked in and verify the build.
 *
 * @subsection add_files Add the source files to your build system
 * The following is a list of files to add to your build system, there is a sample
 * linux Makefile provided (public/samples/compile_and_link/Makefile) which you 
 * can use as a reference.
 *
 *  @li private/idigi_api.c
 *  @li public/samples/compile_and_link/main.c (if applicable)
 *
 * The IIK library consists on one C file idigi_api.c, if you use main.c you 
 * will also need to include it.
 *
 * @subsection add_path Add the include paths
 * The following is a list of include paths to add to your system:
 *
 * @li public/include
 *
 * @subsection build_sample Build the sample
 *
 * Now that you have the build environment setup, verify that you can compile and
 * link.  If you are using the Makefile provided, type make in the sample directory.
 * If you are experiencing a build problem double check the steps listed above.
 * There is only one include path and two C files to build this sample.  Once the
 * build is successful you can proceed to the next step.
 *
 * @note Do not execute the compile_and_link sample, this only verifies that the
 * build enviroment is setup correctly, the application will ASSERT when run.
 *
 * @section step4 Step 4: Get a vendor and device ID
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
 *
 * @section step5 Step 5: Setup your platform
 * 
 * Go into the platforms directory and select a platform which is the similar to your platform, 
 * if none of the available platforms are similar to yours you can use the 
 * template platform. These routines are callbacks which are described in detail
 * in the API section.  In this guide we use the public/run/platforms/linux as an
 * example, but all platforms have the same structure.
 *
 * @subsection os_routines OS Routines
 * Open the file os.c and implement the routines listed below, for the linux platform
 * the file is located at public/run/platforms/linux/os.c.
 *
 *  @li os_malloc()
 *  @li os_free()
 *  @li os_get_system_time()
 *  @li os_sleep()
 *
 * @subsection network_routines Network Routines
 * Open the file network.c and implement the network interface routines. 
 * The network interface routines in the sample provided are implemented using the standard 
 * Linux BSD socket calls. You may have to modify the routines in this file based on your 
 * platforms socket implementation.
 *
 * The following is a list of network interface routines which must be implemented:
 *  @li network_connect()
 *  @li network_send()
 *  @li network_receive()
 *  @li network_close()
 *
 * @subsection configuration_routines Configuration Routines
 *
 * In this step we setup the configuration routines for your platform. Each configuration 
 * item has a corresponding get routine for retrieving its value. Open the file config.c 
 * for your platform and implement the configuration routines.
 * All configuration routines are passed in a pointer to a pointer, along with a pointer to the size. 
 * The address of the configuration item is stored in the pointer passed in, and the size is assigned to the pointer to the size variable.
 * For example get_server_url() is the routine used to retrieve the iDigi Developer Cloud URL:
 *
 * @note The required routines contain the @b #error preprocessor directive 
 * which must be removed before compiling.
 *
 * @note The memory assigned to configuration items must be @b statically @b allocated and is accessed by the IIK 
 * after the routine returns.
 *
 * The following is a list of configuration routines which must be implemented for
 * your platform, please review each routine:
 *  @li get_ip_address()
 *  @li get_mac_addr()
 *  @li get_device_id()
 *  @li get_vendor_id()
 *  @li get_device_type()
 *  @li get_server_url()
 *  @li get_connection_type()
 *  @li get_link_speed()
 *  @li get_phone_number()
 *  @li get_tx_keepalive_interval()
 *  @li get_rx_keepalive_interval()
 *  @li get_wait_count()
 *  @li get_firmware_support()
 *  @li get_data_service_support()
 *
 * @subsection application_start iDigi Initialization
 *
 * In the Linux platform provided the routine main() creates a thread which then
 * calls idigi_run(), you will need to setup a similar thread in your environment
 * or call idigi_step().
 *
 * @section step6 Step 6: Setup your build enviroment
 *
 * @subsection add_c_files Add the source files to your build system
 *
 * To build the connect_to_idigi sample you will need to add the following
 * files to your build envirment note that in this procedure we are
 * using the linux platform.
 *
 * @li private/idigi_api.c
 * @li public/run/samples/connect_to_idigi/application.c
 * @li public/run/samples/connect_to_idigi/main.c (if applicable)
 * @li public/run/platforms/linux/os.c
 * @li public/run/platforms/linux/network.c
 * @li public/run/platforms/linux/config.c
 *
 * @subsection add_include_paths Add the include paths
 *
 * The following is a list of include paths you will need to add to your build
 * system:
 *
 * @li public/include
 * @li run/platforms/linux
 * 
 * @section step7 Step 7: Build the connect_to_idigi sample
 * Next build the sample, if you are running on Linux you can simply type make in
 * the directory run/samples/connect_to_idigi, if you are not using Linux then
 * you will have to build for your enviroment.
 *
 * @section step8 Step 8: View results
 *
 * By default the IDIGI_DEBUG is defined in idigi_config.h, the  name of the
 * executable generated by the Makefile is called idigi.
 *
 * You will see the output shown below when running this sample:
 *
 * Start iDigi
 *
 * dns_resolve_name: ip address = [50.56.41.153]
 *
 * network_connect: connected to [developer.idigi.com] server
 *
 * communication layer: Send MT Version
 *
 * communication layer: receive Mt version
 *
 * communication layer: send keepalive params
 *
 * initialization layer: send protocol version
 *
 * initialization layer: receive protocol version
 *
 * Security layer: send security form
 *
 * security layer: send device ID
 *
 * security layer: send server url
 *
 * discovery layer: send vendor id
 *
 * discovery layer: send device type
 *
 * Discovery Facility layer: 0xc0
 *
 * Discovery Facility layer: 0x70
 *
 * get packet buffer: send pending
 *
 * Discovery Facility layer: 0x70
 *
 * Discovery Facility layer: 0xffff
 *
 * Connection Control: send redirect_report
 *
 * get packet buffer: send pending
 *
 * Discovery Facility layer: 0xffff
 *
 * Connection Control: send redirect_report
 *
 * Discovery Facility layer: 0xffff
 *
 * Connection Control: send connection report
 *
 * get_ip_address: Looking for current device IP address: found [2] entries
 *
 * get_ip_address: 1: Interface name [lo] IP Address [127.0.0.1]
 *
 * get_ip_address: 2: Interface name [eth1] IP Address [10.52.18.74]
 *
 * discovery layer: send complete
 *
 * idigi_facility_layer: receive data facility = 0x00c0
 *
 * @subsection add_your_device_to_the_cloud Add your Device to the iDigi Developer Cloud
 * 
 * The next several sections walk through the steps required to add your device to 
 * the iDigi Developer Cloud. 
 *
 *  -# Navigate to http://www.idigi.com and login using your iDigi developer user account credentials
 *  -# Click on Devices from within the left navigation panel.
 *  -# Click the Add Devices button to bring up the Add Devices dialog.
 *
 * @image html add1.png
 *
 * Within the Add Devices dialog click the Add Manually>> button.
 *
 * @image html add2.png
 *
 * Select Device ID: from the drop-down menu.
 *
 * @image html add3.png
 *
 * Enter the Device ID for your device then click the Add button.
 *
 * @image html add4.png
 *
 * Click the OK button to add your device to the iDigi Developer Cloud.
 * 
 * @image html add5.png
 *
 * @subsection view_result_on_cloud Viewing Results on the iDigi Developer Cloud
 *
 * Click the Refresh button. The device will reconnect to the iDigi Developer Cloud.
 * You can now communicate with your device over the iDigi cloud, the next step 
 * is to integrate in one of the samples to add in more functionality to your
 * application.
 *
 *  @image html results1.png
 *
 * @section step9 Step 9: Next Steps
 *
 * Now that you have a basic sample up and running with the iDigi server, you
 * can port in more functionality to your application.  We first suggest 
 * reading the @ref api1_overview "API" section of the documentation and then look at the documentation
 * for one of the samples.
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 */
