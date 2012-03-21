/*! @page connect_on_ssl Connect over SSL
 *
 * @htmlinclude nav.html
 *
 * @section ssl_overview Overview
 *
 * This sample demonstrates how one can connect to iDigi Cloud securely. The sample includes
 * network_ssl.c, instead of network.c, to provide network specific user callbacks. That means
 * any application can run on ssl by including network_ssl.c.
 * 
 * <b> Makefile changes: </b>
 * 
 * There are two changes user will need to enable SSL.
 * -# Replace network.c with network_ssl.c
 * -# Include libssl.a
 * 
 * <b> How to get CA certificate: </b>
 * 
 * -# Login to developer.idigi.com
 * -# Click Resources link
 * -# Click iDigi Device Cloud CA Certificate under Downloads, General Downloads
 * -# Extract the zip file to public/include (or wherever is convenient)
 * -# Modify the APP_SSL_CA_CERT in platforms/<i>my_platform</i>/network_ssl.c file to match the correct path
 * -# Rebuild the application
 *
 * @section ssl_connect_description Code Flow
 *
 * -# The main(), defined in main.c, calls application_start()
 * -# The application_start(), defined in application.c, registers app_idigi_callbacks()
 * -# The app_idigi_callbacks(), defined in application.c, calls app_network_callbacks() for all network related ICC requests
 * -# The app_network_callbacks(), defined in network_ssl.c, calls subsequent openssl functions to communicate with iDigi cloud over SSL
 *
 * @section connect_build Building
 *
 * To build this example for a Linux-based platform you can go into the directory
 * public/run/samples/connect_on_ssl and type make.  If you are not running Linux you
 * will need to setup your build system with the information described below
 * and then build the image for your platform.
 *
 * @subsection source_files Source Files
 *
 * The following is a list of source files required to build this sample:
 *
 * @htmlonly
 * <table class="apitable">
 * <tr>
 * <th class="title">Name</td>
 * <th class="title">Description</td>
 * <th class="title">Location</td>
 * </tr>
 * <tr>
 * <td>application.c</td>
 * <td>Contains application_run() and the application callback</td>
 * <td>samples/connect_on_ssl</td>
 * </tr>
 * <tr>
 * <td>idigi_api.c</td>
 * <td>Code for the ICC </td>
 * <td>private</td>
 * </tr>
 * <tr>
 * <td>os.c</td>
 * <td>Operating system calls</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>network_ssl.c</td>
 * <td> Network interface </td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>config.c</td>
 * <td>Configuration routines</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * <tr>
 * <td>main.c</td>
 * <td>Starting point of program, dependent on build environment</td>
 * <td>platforms/<i>my_platform</i></td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @subsection include_paths Include Paths
 *
 * The following include paths are required:
 *
 * @li public/include
 * @li run/platforms/linux (substitute with your platform)
 *
 *
 */
