/*! @page getting_started Getting Started
 *
 * @htmlinclude nav.html
 *
 * @section table_of_contents_porting Getting Started
 *
 * The Getting Started process will walk you through the steps necessary to get the IIK integrated into your 
 * development environment and running applications which connect to the iDigi Device Cloud.  
 * 
 * These steps include:
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
 *              -# @ref default_config
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
 * Two sample applications, compile_and_link and connect_to_idigi, will be used to confirm your compilation tool chain 
 * and integration, respectively.  When complete, your device device will be connected to the iDigi Device Cloud and be displayed 
 * in the iDigi Device Cloud Portal graphical user interface.
 *
 * @section step1 Step 1: Determine if your compiler is C89 or C99 compliant
 *
 * The IIK is ANSI X3.159-1989 (ANSI C89) and ISO/IEC 9899:1999 (ANSI C99) compliant.  If your compiler is ANSI C89 or C99 compliant and you are 
 * running on a 32-bit processor you can skip to the next section.  
 *
 * If your compiler is not ANSI C89 or C99 compliant, you will have to review (and edit) public/include/idigi_types.h 
 * to adjust the data types @ref uint8_t, @ref uint16_t, @ref uint32_t.  
 *
 *  @li @ref uint8_t
 *  @li @ref uint16_t
 *  @li @ref uint32_t
 *
 * Note by default these are defined for a 32-bit machine.
 *  
 * @subsection debug_macros Implement the debug macros
 * The file public/include/idigi_types.h implements the two macros listed below, which are used to debug the IIK.
 *
 *  @li @ref USER_DEBUG_VPRINTF
 *  @li @ref ASSERT 
 * 
 * Review these definitions and modify these for your platform:
 * 
 * If you don't have vprintf or assert available redefine these macros to call corresponding 
 * routines for your platform. 
 *
 * @section step2 Step 2: Modify idigi_config.h
 *
 * Open the file public/include/idigi_config.h to configure processor endianess.  
 * 
 * The IIK defaults to little endian.  To reconfigure for big endian, comment out the @ref IDIGI_LITTLE_ENDIAN define.
 *
 * If your application does not want the optional Data Compression (see @ref zlib) then comment out @ref IDIGI_COMPRESSION,
 * if compression is supported leave this define alone.
 *
 * @section step3 Step 3: Build the compile_and_link sample
 *
 * In this step we are going to build the compile_and_link sample,
 * this sample will verify that your build environment is setup correctly.
 * You need to add the source files and include paths to your build system listed
 * below.
 *
 * @subsection idigi_initialization IIK Initialization
 *
 * The routine idigi_init() is called to initialize the IIK, currently this is
 * called from the routine main() in main.c; you will need to call idigi_init()
 * at the initialization point for your application with an application's callback. 
 * For the compile_and_link sample you can call idigi_init as:
 *
 * @code
 * /* 
 *  * Call idigi_init() where you intend to initialize the IIK, a NULL callback 
 *  * is passed into idigi_init() for this sample, we want the IIK to be linked in 
 *  * to verify the build.
 *  */
 * (void)idigi_init((idigi_callback_t)0);
 * @endcode
 *
 * If you are not using main() you will need to add in the call to idigi_init()
 * where appropriate.
 *
 * @subsection add_files Add the source files to your build system
 * The following is a list of files to add to your build system, there is a sample
 * linux Makefile provided (public/run/samples/compile_and_link/Makefile) which you 
 * can use as a reference.  Add the following files to your make/build system.
 * 
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</td>
 * <th class="title">Description</td>
 * <th class="title">Location</td>
 * </tr>
 * <tr>
 * <td>idigi_api.c</td>
 * <td>Contains the code for the IIK</td>
 * <td>private</td>
 * </tr>
 * <tr>
 * <td>main.c</td>
 * <td>Program entry point, calls @endhtmlonly idigi_init() @htmlonly</td>
 * <td>public/run/samples/compile_and_link</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @subsection add_path Add the include paths
 * The following is a list of include paths to add to your system:
 *
 * @li public/include
 *
 * @subsection add_define Add the defines
 *
 * The following define is required, this is used to indicate the version of
 * the IIK is 1.1
 *
 * @li IDIGI_VERSION=0x1010000UL
 *
 * @subsection build_sample Build the sample
 *
 * Now that you have the build environment setup, verify that you can compile and
 * link.  If you are using the Makefile provided, type make in the compile_and_link directory, 
 * otherwise execute the build for your system.
 * If you are experiencing a build problem double check the steps listed above.
 * There is only one include path and two C files to build this sample.  Once the
 * build is successful you can proceed to the next step.
 *
 * @note Do not execute the compile_and_link sample, this only verifies that the
 * build environment is setup correctly, the application will ASSERT when run.
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
 * 1. Navigate to http://www.idigi.com
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
 * Register for new vendor id button.  Record the Vendor ID you will need it later.
 *
 * @image html idigi6.jpg
 *
 * @subsection idigi_device_id Obtain an iDigi Device ID
 *
 * Device IDs are a globally unique identifier for iDigi clients. The Device ID is a 
 * 16-octet value derived from the MAC address of a network interface on the client.
 * The mapping from MAC address to Device ID consist of inserting "FFFF" in the middle 
 * of the MAC and setting all other bytes of the Device ID to 0.
 * For Example:
 * MAC Address 00:04:2D:01:6A:53, would map to a Device ID: 00000000-00042DFF-FF016A53
 * If a client has more than one network interface, it does not matter to iDigi which 
 * network interface MAC is used for the basis of the Device ID. If the MAC is read 
 * directly from the network interface to generate the client's Device ID, care must be 
 * taken to always use the same network interface's MAC since there is a unique mapping 
 * between a device and a Device ID. 
 *  Use the above mapping with your MAC address to create the Device ID and record
 * it for later.
 *
 *
 * @section step5 Step 5: Setup your platform
 * 
 * Go into the platforms directory and select a platform which is the similar to your platform, 
 * if none of the available platforms are similar to yours you can use the 
 * template platform. 
 *
 * @note In this release of the IIK only the Linux platform is provided.
 *
 * In this guide we use the public/run/platforms/linux as an
 * example, but all platforms have the same structure.   
 * So choose a platform (or copy an existing one)
 * and go into that directory.
 *
 * @subsection os_routines OS Routines
 * Open the file os.c and implement the routines listed below, for the linux platform
 * the file is located at public/run/platforms/@a my_platform/os.c.  Click on the routine
 * to see a description, then modify to operate with your platform.  These routines
 * are callbacks which are described in the API section.
 *
 *  @li app_os_malloc()
 *  @li app_os_free()
 *  @li app_os_get_system_time()
 *  @li app_os_sleep()
 *
 * @subsection network_routines Network Routines
 * Open the file network.c and implement the network interface routines. 
 * The network interface routines provided in the Linux platform are implemented using standard 
 * Berkeley socket calls and can be used as a reference for your platform. 
 * You may have to modify the routines in this file based on your 
 * platforms network implementation.
 *
 * The following is a list of network interface routines which must be implemented:
 *  @li app_network_connect()
 *  @li app_network_send()
 *  @li app_network_receive()
 *  @li app_network_close()
 *
 * @subsection configuration_routines Configuration Routines
 *
 * In this step we setup the configuration routines for your platform. Each configuration 
 * item has a corresponding get routine for retrieving its value. Open the file config.c 
 * for your platform and implement the configuration routines.
 * All configuration routines are passed in a pointer to a pointer, along with a pointer to the size. 
 * The address of the configuration item is stored in the pointer passed in, and the size is assigned to the pointer to the size variable.
 * For example app_get_server_url() is the routine used to retrieve the iDigi Developer Cloud URL
 * and is shown below.
 * @code
 *  static int app_get_server_url(char ** url, size_t * size)
 *  {
 *  #error "Specify iDigi Server URL" /* This #error must be removed to compile */
 *      /* Statically allocated string containing the sever URL */
 *      static const char const *idigi_server_url = "developer.idigi.com";
 *  
 *      /* Fill in the pointer with the address of the URL in memory */
 *      *url = (char *)idigi_server_url;
 *      /* Fill in the size */
 *      *size = strlen(idigi_server_url);
 *  
 *      return 0;
 *  }
 * @endcode
 *
 * @note The required routines contain the @htmlonly #error @endhtmlonly preprocessor directive 
 * which must be removed before compiling.
 *
 * @note The memory assigned to configuration items must be @b statically @b allocated and is accessed by the IIK 
 * after the routine returns.
 *
 * The following is a list of configuration routines which needs to be implemented for
 * your platform, please review each routine:
 *  @li app_get_ip_address()
 *  @li app_get_mac_addr()
 *  @li app_get_device_id()
 *  @li app_get_vendor_id()
 *  @li app_get_device_type()
 *  @li app_get_server_url()
 *  @li app_get_connection_type()
 *  @li app_get_link_speed()
 *  @li app_get_phone_number()
 *  @li app_get_tx_keepalive_interval()
 *  @li app_get_rx_keepalive_interval()
 *  @li app_get_wait_count()
 *  @li app_get_firmware_support()
 *  @li app_get_data_service_support()
 *  @li app_get_max_message_transactions()
 *  @li app_config_error()
 *
 * @subsection default_config Hard Code Configuration 
 *
 * If the following configurations in @ref idigi_config.h are defined, the
 * callback is not used or called for these configurations.  This reduces code size
 * and memory usage.  You can define individual configuration.
 *
 * @code
 * #define IDIGI_DEVICE_TYPE               "IIK Linux Sample"
 * #define IDIGI_CLOUD_URL                 "developer.idigi.com" 
 * #define IDIGI_TX_KEEPALIVE_IN_SECONDS   75
 * #define IDIGI_RX_KEEPALIVE_IN_SECONDS   75
 * #define IDIGI_WAIT_COUNT                10
 * #define IDIGI_VENDOR_ID                 0x00000000
 * #define IDIGI_MSG_MAX_TRANSACTION       1
 *
 * #define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type
 * #define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0
 * #define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678"
 * 
 * #define IDIGI_FIRMWARE_SUPPORT
 * #define IDIGI_DATA_SERVICE_SUPPORT
 * @endcode
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Configuration Defines</td>
 * <th class="title">Description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DEVICE_TYPE @htmlonly</td>
 * <td>Device type in iso-8859-1 encoded string to identify the device.
 * See @endhtmlonly @ref device_type @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_CLOUD_URL @htmlonly </td>
 * <td>iDigi Device Cloud FQDN.
 * See @endhtmlonly @ref server_url @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_TX_KEEPALIVE_IN_SECONDS @htmlonly </td>
 * <td>TX Keepalive interval in seconds between 5 and 7200 seconds.
 * See @endhtmlonly @ref tx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_RX_KEEPALIVE_IN_SECONDS @htmlonly </td>
 * <td>RX Keepalive interval in seconds between 5 and 7200 seconds.
 * See @endhtmlonly @ref rx_keepalive @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_WAIT_COUNT @htmlonly </td>
 * <td>Number of time of not receiving a keepalive message which
 * a connection should be considered lost.it must be between 2 and 63 times.
 * See @endhtmlonly @ref wait_count @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_VENDOR_ID @htmlonly </td>
 * <td>Vendor ID from iDigi Account.
 * See @endhtmlonly @ref vendor_id @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_MSG_MAX_TRANSACTION @htmlonly </td>
 * <td>Maximum simultaneous transactions for data service receiving message.
 * See @endhtmlonly @ref max_msg_transactions @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_CONNECTION_TYPE @htmlonly </td>
 * <td> @endhtmlonly @ref idigi_lan_connection_type @htmlonly for LAN connection or
 * @endhtmlonly @ref idigi_wan_connection_type @htmlonly WAN connection.
 * See @endhtmlonly @ref connection_type @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND @htmlonly </td>
 * <td>Link speed for WAN connection.
 * See @endhtmlonly @ref link_speed @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_WAN_PHONE_NUMBER_DIALED @htmlonly </td>
 * <td>Phone number dialed for WAN connection in a string.
 * See @endhtmlonly @ref phone_number @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_FIRMWARE_SUPPORT @htmlonly </td>
 * <td>If defined it enables firmware download capability.
 * See @endhtmlonly @ref firmware_support @htmlonly</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref IDIGI_DATA_SERVICE_SUPPORT @htmlonly </td>
 * <td>If defined it enables data service capability.
 * See @endhtmlonly @ref data_service_support @htmlonly</td>
 * </tr>
 * </table>
 * @endhtmlonly
  *
 * @see @ref idigi_config_data_options
 
 * @subsection application_start iDigi Initialization
 *
 * As in the previous sample you will need to setup the call to idigi_init()
 * where you intend to start the IIK, in this case you will pass in the 
 * application callback which handles all OS, network, and configuration routines.
 *
 * @code
 * /* Initialize the IIK with the application callback */
 * idigi_handle = idigi_init((idigi_callback_t) idigi_callback);
 * @endcode
 *
 * In the Linux platform provided the routine main() creates a thread which then
 * calls idigi_run(), you will need to setup a similar thread in your environment
 * or call idigi_step() periodically.
 * This thread is shown below:
 * @code
 * static void * idigi_run_thread(void * arg)
 * {
 *     idigi_status_t status;
 * 
 *     APP_DEBUG("idigi_run thread starts\n");
 * 
 *     /* Run the IIK, this will only return on an IIK abort */
 *     status = idigi_run((idigi_handle_t)arg);
 * 
 *     APP_DEBUG("idigi_run thread exits %d\n", status);
 * 
 *     pthread_exit(arg);
 * }
 * @endcode
 *
 * @section step6 Step 6: Setup your build environment
 *
 * @subsection add_c_files Add the source files to your build system
 *
 * To build the connect_to_idigi sample you will need to add the files shown below 
 * to your build environment.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</td>
 * <th class="title">Description</td>
 * <th class="title">Location</td>
 * </tr>
 * <tr>
 * <td>idigi_api.c</td>
 * <td>Contains the code for the IIK</td>
 * <td>private</td>
 * </tr>
 * <tr>
 * <td>application.c</td>
 * <td>Contains the code which runs the sample and main callback that calls
 * callbacks in os.c, network.c, & config.c</td>
 * <td>public/run/samples/connect_to_idigi</td>
 * </tr>
 * <tr>
 * <td>os.c</td>
 * <td>Operating System Routines</td>
 * <td>public/run/platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>network.c</td>
 * <td>Network Interface</td>
 * <td>public/run/platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>config.c</td>
 * <td>Configuration Routines</td>
 * <td>public/run/platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>main.c (if applicable)</td>
 * <td>Program entry point, calls @endhtmlonly idigi_init() @htmlonly</td>
 * <td>public/run/platforms/<i>my_platform</i></td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @subsection add_include_paths Add the include paths
 *
 * The following is a list of include paths you will need to add to your build
 * system:
 *
 * @li public/include
 * @li run/platforms/@a my_platform
 * 
 * @section step7 Step 7: Build the connect_to_idigi sample
 * Next build the sample, if you are running on Linux you can simply type make in
 * the directory public/run/samples/connect_to_idigi, if you are not using Linux then
 * you will have to build for your environment.
 *
 * @section step8 Step 8: View results
 *
 * The name of the executable generated by the linux Makefile is called idigi, in
 * linux type ./idigi from a console to execute the program.
 *
 * By default @ref IDIGI_DEBUG is defined in idigi_config.h, this will enable the
 * debug output from the program to be displayed as shown below.
 *
 * Start iDigi
 *
 * initialize_facilities: callback supports  13 facility
 *
 * initialize_facilities: callback supports  14 facility
 *
 * idigi_run thread starts
 *
 * idigi_run thread starts
 *
 * application_run thread exits 0
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
 * communication layer: Rx keepalive parameter = 60
 *
 * communication layer: Tx keepalive parameter = 90
 *
 * communication layer: Wait Count parameter = 5
 *
 * initialization layer: send protocol version
 *
 * initialization layer: receive protocol version
 *
 * Security layer: send security form
 *
 * security layer: send device ID =  00 00 00 00 00 00 00 00 78 E7 D1 FF FF 84 47 82
 *
 * security layer: send server url = developer.idigi.com
 *
 * discovery layer: send vendor id = 0x1000043
 *
 * discovery layer: send device type = Linux Application
 *
 * fw_discovery: No target supported
 *
 * Connection Control: send redirect_report
 *
 * get packet buffer: send pending
 *
 * Connection Control: send redirect_report
 *
 * Connection Control: send connection report
 *
 * get_ip_address: Looking for current device IP address: found [2] entries
 *
 * get_ip_address: 1: Interface name [lo]  IP Address [127.0.0.1]
 *
 * get_ip_address: 2: Interface name [eth1]        IP Address [10.52.18.75]
 *
 * get_ip_addr: Device IP address =  0A 34 12 4B
 *
 * get_connection_type: connection type = 0
 *
 * get_mac_addr: MAC address =  78 E7 D1 84 47 82
 *
 * discovery layer: send complete
 *
 * @subsection add_your_device_to_the_cloud Add your Device to the iDigi Developer Cloud
 * 
 * The next several sections walk through the steps required to add your device to 
 * the iDigi Developer Cloud. 
 *
 *  -# Navigate to http://www.idigi.com and login to the iDigi Developer Cloud
 *  -# Click on Devices from within the left navigation panel.
 *  -# Click the Add Devices button (the + button under Devices) to bring up the Add Devices dialog.
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
 * If successful your device Status will show connected.  You can now communicate 
 * with your device over the iDigi cloud, the next step 
 * is to integrate in one of the samples to add in more functionality to your
 * application.
 *
 * @note If you cannot connect to the iDigi Device cloud, the most likely problem
 * is in the iDigi Configuration, review the configuration routines in config.c;
 * The device ID and vendor ID must be valid to connect.
 *
 * @image html results1.png
 *
 * @section step9 Step 9: Next Steps
 *
 * Now that you have a basic sample up and running with the iDigi Device Cloud, you
 * can port in more functionality to your application.  We first suggest 
 * reading the @ref api1_overview "API" section of the documentation and then 
 * select a sample which has the functionality you need.
 *
 */

