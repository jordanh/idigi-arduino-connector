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
 *          -# @ref step4
 *          -# @ref step5
 *          -# @ref step6
 *          -# @ref step7
 *          -# @ref step8
 * 
 * This getting started guide will walk you through the stpes necessary to
 * get an application running with the IIK, this application will connect to the
 * iDigi server and the configuration will be displayed in ConnectWare Manager.
 *
 * @section step1 Step 1: Determine if your compiler is C89 or C99 compliant
 *
 * If your compiler is not C89 or C99 compliant you will have to modify idigi_types.h
 * this file tests the __STD_VERSION__ macro's compliance to ISO/IEC 9899:1999. 
 * If C99 compliant, then standard C99 header files stdint.h and stdbool.h are included. Otherwise, 
 * if not C99 compliant, data types uint8_t, uint16_t, uint32_t, and bool are defined, along with the 
 * values for true and false. Note these defines assume at least a 32-bit machine. If your platform 
 * is not at least a 32-bit machine, these definitions should be reviewed.
 *
 *  @li @ref uint8_t
 *  @li @ref uint16_t
 *  @li @ref uint32_t
 *  @li @ref bool which is @ref true or @ref false
 * 
 * In addition to the types listed above, the IIK also uses bool for a Boolean value, this is defined in stdbool.h as:
 * @ref true which expands to 1
 * @ref false which expands to 0
 *
 * @subsection debug_macros Debug Macros
 * The IIK uses the two debug macros listed below:
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
 * @section step2 Step 2: Modify config.h and set the endianess
 *
 * Open the file config.h and set the endianess, set the macro @ref LITTLE_ENDIAN 
 * to TRUE or FALSE for your platform.
 *
 * @section step3 Step 3: Build the compile_and_link sample
 *
 * The first sample we are going to run is in the samples/compile_and_link directory,
 * this sample will verify that your build environment is setup correctly.
 *
 * @subsection add_files Add the files to your build system
 * The following is a list of files to add to your build system, there is a sample
 * linux Makefile provided.
 *
 *  @li idigi/private/idigi_api.c
 *  @li idigi/public/samples/compile_and_link/main.c
 *
 * @subsection add_path Add the include paths
 * The following is a list of include paths to add to your system:
 *
 * @li ../../include
 *
 * @subsection build_sample Build the sample
 *
 * Now that you have the build environment setup, verify that you can compile and
 * link.  If you are using the Makefile provided, type make in the sample directory.
 * If you are experiencing a build problem double check the steps listed above.
 *
 * @section step4 Step 4: Get a vendor and device ID
 *
 *
 * @section step5 Step 5: Setup your platform
 * 
 * Go into the platforms and select a platform which is the similar to your platform, 
 * if none of the available platforms are similar to yours you can use the 
 * template platform.
 *
 * @subsection OS routines
 * Open the file os.c and implement the routines listed below:
 *  @li os_malloc()
 *  @li os_free()
 *  @li os_get_system_time()
 *
 * @subsection Network routines
 * Open the file network.c and implement the network interface routines. 
 * The network interface routines in the sample provided are implemented using the standard 
 * Linux BSD socket calls. You may have to modify the routines in this file based on your 
 * platforms socket implementation.
 *
 * The following is a list of network interface routines which must be implemented:
 *  @li open_socket()
 *  @li network_connect()
 *  @li network_send()
 *  @li network_receive()
 *  @li network_close()
 *
 * @subsection Configuration routines
 *
 * In this step we setup the configuration routines for your platform. Each configuration 
 * item has a corresponding get routine for retrieving its value. Open the file config.c 
 * and implement the configuration routines.
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
 * @section step6 Step 6: Setup your build enviroment
 *
 *
 * 
 * @subsection step7 Step 7: Build the connect_to_idigi sample
 *
 * @section step8 Step 8: View results   
 *
 * </td></tr>
 * </table>
 * @endhtmlonly
 */
