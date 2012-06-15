/*! @page rci_tool Remote Configuration Tool
 *
 * @htmlinclude nav.html
 *
 * @section rci_tool_overview Overview
 *
 * The remote configuration tool is used to generate device configuration header file and upload
 * device configuration information to the iDigi Cloud. It allows user to define all device configuration
 * data and information that can be accessed throught iDigi Cloud using iDigi connector. 
 * User must include the generated header file and provide callback functions to handle all 
 * device configuration in an application.
 *
 * -# @ref rci_tool_usage
 * -# @ref rci_tool_file
 * -# @ref rci_tool_example
 *
 * @section rci_tool_usage Usage
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Usage</th> </tr> 
 * <tr><td colspan="2"> @endhtmlonly java -jar ConfigGenerator.jar [-help] [-verbose] [-nodesc] [-vendor] [-path] [-server]
 *                     \<username[:password]\> \<device_type\> \<firmware_version\> \<input_config_file\> @htmlonly </td>
 * </tr>
 * <tr> <th colspan="2" class="title">Description</th> </tr> 
 * <tr><td colspan="2">It generates and uploads configuration information (descriptors) to the iDigi
 *                     Cloud server and it also generates ANSI C header 
 *                     files (remote_config.h) from the input iDigi connector configuration file.  
 *                     This header file must be included  in the application. </td>
 * </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr><th> -help </th>
 * <td> Optional option to show this menu </td>
 * </tr> <tr>
 * <th> -verbos</th>
 * <td> Optional option to output message about what the tool is doing </td>
 * </tr> <tr>
 * <th> -nodesc</th>
 * <td> Optional option to exclude error description in the generated file. Error description will
 *      not be included and sent to the iDigi Cloud when error is encountered.
 *      This is used to reduce the code size.</td>
 * </tr> <tr>
 * <th> -vendor</th>
 * <td> Optional option for vendor ID obtained from iDigi Cloud registration. 
 *      If not given, tool tries to retrieve it from the iDigi Cloud. </td>
 * </tr> <tr>
 * <th> -path</th>
 * <td> Optional option for directory path where the generated file will be created. </td>
 * </tr>
 * </tr> <tr>
 * <th> -server</th>
 * <td> Optional options for iDigi Cloud server. Default is developer.idigi.com </td>
 * </tr>
 * </tr> <tr>
 * <th> username </th>
 * <td> Username to log in iDigi Cloud.  </td>
 * </tr> <tr>
 * <th> password </th>
 * <td> Optional for password to log in iDigi Cloud. 
 *      If it's not specified in the command line, you will be prompted for password. </td>
 * </tr> <tr>
 * <th> device_type </th>
 * <td> Device type string with quotes. </td>
 * </tr> <tr>
 * <th> firmware_version </th>
 * <td> Firmware version number (i.e. 1.0.0.0) </td>
 * </tr> <tr>
 * <th> input_config_file </th>
 * <td> Input configuration file. See @endhtmlonly @ref rci_tool_file @htmlonly </td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * @a @b username, @a @b password,@a @b vendor_id, @a @b device_type, and @a @b firmware_version are used for accessing your iDigi 
 * Cloud account and uploading the device configuration information (descriptors) for your device.
 *
 * @section rci_tool_file   Input Configuration File
 *
 * User must define all device configuration data and system information that is accessed throught the iDigi Cloud and run the 
 * tool to generate remote_config.h file. User must include this remote_config.h header file and
 * provide callback to handle all device configuration data and system information.
 *
 * User uses the following keywords to define device configuration data and system information:
 * -# @ref globalerror
 * -# @ref group
 * -# @ref error
 * -# @ref element
 * -# @ref type
 * -# @ref value
 * -# @ref min
 * -# @ref max
 * -# @ref access
 * -# @ref unit
 * -# @ref comment
 *
 * @subsection globalerror  globalerror keyword
 * The @a @b globalerror is used to define the text in the iDigi Cloud when 
 * an error condition occurs. This @a @b globalerror defines errors common to all configurations.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b globalerror \<name\> \<description\> @htmlonly </td>
 * </tr> <tr>
 * <th colspan="2" class="title">Options</th> 
 * </tr> <tr>
 * <th>name</th>
 * <td> Label (no spaces) of the error. 
 *     <p> @endhtmlonly The tool creates  @a @b \<name\> with @a @b idigi_global_error_ prefix enumeration value for each error. 
 *         This error enumeration value will be used by the application-defined callback to return an error which is sent along to the server.
 *         @htmlonly
 *     </p> </td>
 * </tr> <tr>
 * <th>description</th>
 * <td> Error string message with quotes (e.g. "Unable to write to NVRam"). 
 *     This description is sent to the iDigi Cloud along with the enumeration value. </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *      globalerror load_fail "Unable to load configuration"
 *      globalerror save_fail "Unable to save configuration"
 *      globalerror memory_fail "Insufficient memory"
 *
 *  The enumeration values:
 *      idigi_global_error_load_fail
 *      idigi_global_error_save_fail
 *      idigi_global_error_memory_fail
 * @endcode
 *
 * @subsection group  group keyword
 * The @a @b group is used to define a device configuration data and information about the 
 * current state of the device.
 * It contains metadata about the element named in the @ref element.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b group \<type\> \<name\> [count] 
 *      \<description\> [help_description] @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>type</th>
 * <td> <ul> <li> <b> setting </b>= device configuration data</li>
 *           <li> <b> state </b>= information about the current state of the device </li> </ul></td>
 * </tr><tr>
 * <th>name</th>
 * <td> Label (no spaces) of the group. 
 *     <p> @endhtmlonly The tool creates  @a @b \<name\> with @a @b idigi_\<type\>_ prefix enumeration value to identify
 *          the group. The iDigi connector passes this enumeration value to the application-defined callback for accessing the group.  
 *         @htmlonly
 *     </p> </td>
 * </tr><tr>
 * <th>count</th>
 * <td> Optional for number of array instances of the group. If count is not given, the group has only 1 instance. </td>
 * </tr> <tr>
 * <th>description</th>
 * <td>Description for this group with quotes, suitable for displaying in a generic GUI to represent 
 *     this group. This is only used in the descriptor. It must be 40 characters or less. </td>
 * </tr><tr>
 * <th>help_description</th>
 * <td> Optional description with quotes for extra information shown as a tool-tip and help text. </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *      group setting serial 2 "Serial Port"
 *      group state system_info "System Information"
 *
 *  The enumeration values:
 *      idigi_setting_serial
 *      idigi_state_system_info
 * @endcode
 *
 * @subsection error  error keyword
 * The @a @b error is used to define the text in the iDigi Device Cloud when an error 
 * condition occurs. This is used to specify an error for individual group.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b error \<name\> \<description\> @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>name</th>
 * <td> Label (no spaces) of the error. 
 *     <p> @endhtmlonly The tool creates  @a @b \<name\> with @a @b idigi_\<group_type\>_\<group_name\>_error_ prefix enumeration value for each error. 
 *         This error enumeration value will be used by the application-defined callback to return an error which is sent along to the server.
 *         @htmlonly
 *     </p> </td>
 * </tr> <tr>
 * <th>description</th>
 * <td> Error string message with quotes (e.g. "Invalid data bits rate"). 
 *     This description is sent to the iDigi Cloud along with the enumeration value. </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *      error invalid_databits "invalid data bits rate"
 *
 *  The enumeration values:
 *      idigi_setting_serial_error_invalid_data
 * @endcode
 *
 * @subsection element element keyword
 * The @a @b element is used to describe valid fields in a group. It describes an element 
 * under the group that has a type and contains a value.
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b element \<name\> \<description\> [help_description] 
 *       type \<type_value\> [min \<min_value\>] [max \<max_value\>] [access \<access_value\>]
 *       [unit \<unit_description\>]
 * @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>name</th>
 * <td> Label (no spaces) of the element. 
 *     <p> @endhtmlonly The tool creates  @a @b \<name\> with @a @b idigi_\<group_type\>_ prefix enumeration value 
 *         to identify the element. The iDigi connector passes this enumeration value to the application-defined callback for accessing the element.
 *         @htmlonly
 *     </p> </td>
 * </tr> <tr>
 * <th>description</th>
 * <td>Description for this element with quotes, suitable for displaying in a generic GUI to represent 
 *     this element. This is only used in the descriptor. It must be 40 characters or less. </td>
 * </tr><tr>
 * <th>help_description</th>
 * <td> Optional description with quotes for extra information shown as a tool-tip and help text. </td>
 * </tr> <tr>
 * <th>type</th>
 * <td>See @endhtmlonly @ref type. @htmlonly </td>
 * </tr><tr>
 * <th>min</th>
 * <td> see @endhtmlonly @ref min. @htmlonly </td>
 * </tr><tr>
 * <th>max</th>
 * <td> see @endhtmlonly @ref max. @htmlonly </td>
 * </tr><tr>
 * <th>access</th>
 * <td> see @endhtmlonly @ref access. @htmlonly </td>
 * </tr><tr>
 * <th>unit</th>
 * <td> see @endhtmlonly @ref unit. @htmlonly </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *       element ip_addr "Static IP Address" type ipv4 access read_write
 *
 *  The enumeration values:
 *      idigi_setting_ethernet_ip_addr
 * @endcode
 *
 * @subsection type type keyword
 * The @a @b type is used to indicate the value type of an element's value named in the 
 * @ref element. The type indicates to the descriptor user what restrictions to 
 * place on a value and how to interpret a value received from iDigi connector. 
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b type \<value\> @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">value</th> </tr> 
 * <tr>
 * <th>string</th>
 * <td> Single line string with nul-terminated.</td>
 * </tr> <tr>
 * <th>multiline_string</th>
 * <td>This type is used for a nul-terminated string that may be more than one line. A "text area" is shown 
 *     in the iDigi Device Cloud that allows a user to form multiple lines by pressing enter. </td>
 * </tr><tr>
 * <th>password</th>
 * <td>This Password type is same as string type except it's shown "*"when a user types in 
 *     characters in the iDigi Cloud. </td>
 * </tr> <tr>
 * <th>int32</th>
 * <td>32-bit signed integer value. </td>
 * </tr><tr>
 * <th>uint32</th>
 * <td> 32-bit unsigned integer value.</td>
 * </tr><tr>
 * <th>hex32</th>
 * <td> 32-bit unsigned hexadecimal </td>
 * </tr><tr>
 * <th>0xhex</th>
 * <td>32-bit unsigned hexadecimal with 0x prefix shown in iDigi Device Cloud. </td>
 * </tr> <tr>
 * <th>float</th>
 * <td>Floating value. float.h is included in the generated header file. </td>
 * </tr><tr>
 * <th>enum</th>
 * <td> enum is used to define a set of allowed values for an element. This is a 
 * pull-down menu shown in iDigi Cloud. See @endhtmlonly @ref value @htmlonly to define set 
 * of enum values for the element. <p> Note: min and max keywords will throw an error.</p></td>
 * </tr><tr>
 * <th>on_off</th>
 * <td> "on" or "off " value. Application-defined callback should use <i> <b> idigi_on </b> </i> value
 *       for "on"  or <i> <b> idigi_off </b> </i> value for "off". This type is shown as 
 *       radio buttons in the iDigi Device Cloud. 
 *       <p>Note: min and max keywords will throw an error.</p> </td>
 * </tr><tr>
 * <th>boolean</th>
 * <td>true or false. Application-defined callback should use <i> <b> idigi_boolean_true </b> </i> value for 
 *     true or <i> <b> idigi_boolean_false </b> </i> value for false.
 *     <p>Note: min and max keywords will throw an error.</p> </td>
 * </tr> <tr>
 * <th>ipv4</th>
 * <td>Valid IPv4 address (32-bit value) which is shown aaa.bbb.ccc.ddd in iDigi Device Cloud
 *     <p>Note: min and max keywords will throw an error.</p></td>
 * </tr><tr>
 * <th>fqdnv4</th>
 * <td> This type accepts either ipv4 or DNS name.</td>
 * </tr> <tr>
 * <th>fqdnv6</th>
 * <td>This type accepts either IPv6 address, IPv4 address, or DNS name. </td>
 * </tr><tr>
 * <th>datetime</th>
 * <td> Date and time type. It's a nul-terminated string which contains the ISO 8601 standard for date and 
 *      time representation.
 *      <p>The format is: YYYY-MM-DDTHH:MM:SStz where 
 *      <ul><li>YYYY:Year</li>
 *          <li>MM: Month</li>
 *          <li>DD: Day </li>
 *          <li>T: The separator between date and time </li>
 *          <li>HH: Hours in 24-hour clock </li>
 *          <li>MM: Minutes </li>
 *          <li>SS: Seconds </li>
 *          <li>tz: Time zone, specified either in the form [+,-]HHMM or 
 *                              Z for Coordinated Universal Time (UTC) </li></ul></p>
 *      <p>Examples: @endhtmlonly
 *      @code
 *      	   "2002-05-030T09:30:10-0600" 
 *      	   "2002-05-030T15:30:10Z"
 *      @endcode @htmlonly
 *      <p> Note: min and max keywords will throw an error.</p></td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *       element ip_addr "Static IP Address" type ipv4 access read_write
 *
 * @endcode
 *
 * @subsection value value keyword
 * The @a @b value is used to define a set of values for an element named in the @ref element
 * with enum type (@ref type).  
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b value \<name\> [description] [help_description] @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>name</th>
 * <td> Label (no spaces) of the enum value. 
 *     <p> @endhtmlonly The tool creates  @a @b \<name\> with @a @b idigi_\<group_type\>_\<group_name\>_\<element_name\>_ 
 *         prefix enumeration value for each values.
 *         The iDigi connector and the application-defined callback use this enumeration value to set and retrieve the value of an element.
 *         @htmlonly
 *     </p> </td>
 * </tr> <tr>
 * <th>description</th>
 * <td>Optional description for this value with quotes, suitable for displaying in a generic GUI to represent 
 *     this element. This is only used in the descriptor. It must be 40 characters or less. </td>
 * </tr><tr>
 * <th>help_description</th>
 * <td> Optional description with quotes for extra information shown as a tool-tip and help text. </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *       value auto "Automatic"
 *       value 1200
 *       value 2400
 *       value 9600
 *       value 19200
 *
 *  The enumeration values:
 *      idigi_setting_serial_baud_auto
 *      idigi_setting_serial_baud_300
 *      idigi_setting_serial_baud_2400
 *      idigi_setting_serial_baud_9600
 *      idigi_setting_serial_baud_19200
 * @endcode
 *
 * @subsection min min keyword
 * The @a @b min is used to specify minimum value or length of an element
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b min \<value\>  @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>value</th>
 * <td> Minimum value or length in bytes.
 *     <p> The iDigi connector will have this range check included. 
 *         The user can do additional checks, as well. Need to determine a list of 
 *         built-in errors that the iDigi connector or user can use.</p> </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *  element setting sys_password type password min 8 max 31
 *
 * @endcode
 *
 * @subsection max max keyword
 * The @a @b max is used to specify maximum value or length of an element
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b max \<value\>  @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>value</th>
 * <td> Maximum value or length in bytes.
 *     <p> The iDigi connector will have this range check included. 
 *         The user can do additional checks, as well. Need to determine a list of 
 *         built-in errors that the iDigi connector or user can use.</p> </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *  element setting sys_password type password min 8 max 31
 *
 * @endcode
 *
 * @subsection access access keyword
 * The @a @b access is used to define accessibility of an element named in the element 
 * keyword. Default is read_write. This tells iDigi connector and the iDigi Cloud that an element 
 * is settable or unsettable. It's grayed out if read_only is specified. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b access \<value\>  @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">value</th> </tr> 
 * <tr>
 * <th>read_only</th>
 * <td> Read only element</td>
 * </tr> <tr>
 * <th>write_only</th>
 * <td> Write only element</td>
 * </tr> <tr>
 * <th>read_write</th>
 * <td> Read and write element</td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *  element setting sys_password type password access read_only min 8 max 31
 *
 * @endcode
 *
 * @subsection unit  unit keyword
 * The @a @b unit is used to define the displayable text in the iDigi Device Cloud. It's used as
 * the units for the element.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Syntax</th> </tr> 
 * <tr> <td colspan="2">@endhtmlonly @a @b unit \<description\> @htmlonly </td> </tr>
 * <tr> <th colspan="2" class="title">Options</th> </tr> 
 * <tr>
 * <th>description</th>
 * <td> Unit string with quotes (e.g. "seconds"). 
 *     This description is shown in the iDigi Cloud. </td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 * @code
 *
 *      element sys_up_time "System up time" type uint32 access read_only unit "seconds"
 *
 * @endcode
 *
 *
 * @subsection comment  Comment Tag
 * The @a @b # is used for comment. Any sequence of characters after # will be treated 
 * as comment and will not be processed.
 *
 *
 * Example:
 * @code
 *
 *      # State configuration for GPS
 *      # Must setup the following group in order for GPS shown in iDigi Cloud.
 *
 * @endcode
 *
 * @section rci_tool_example Input Configuration File Sample
 * @code
 *
 * globalerror load_fail "Load fail"
 * globalerror save_fail "Save fail"
 * globalerror memory_fail "Insufficient memory"
 * 
 * group setting serial 2 "Serial Port" "Port 1 is used for printf"
 *     element baud "Baud rate" type enum access read_write
 *         value 2400
 *         value 4800
 *         value 9600
 *         value 19200
 *         value 38400
 *         value 57600
 *         value 115200
 *         value 230400  
 *     element parity "Parity" type enum access  read_write 
 *         value none
 *         value odd
 *         value even
 *     element databits "Data bits" type uint32 access  read_write  min  5 max  8 
 *     element xbreak "Tx Break" type on_off access  read_write 
 *     element txbytes "Tx bytes" type uint32 access  read_only 
 *     error invalid_baud "Invalid baud rate "
 *     error invalid_databits "Invalid data bits"
 *     error invalid_parity  " Invalid parity"
 *     error invalid_xbreak "Invalid xbreak setting"
 *     error invalid_databits_parity "Invalid combination of data bits and parity"
 *
 * group setting device_info  "Device info"
 *     element product "Product" type string access  read_write  min  1 max  64 
 *     element model "Model" type string access  read_write  min  0 max  32 
 *     element company "Company" type string access  read_write 
 *     element desc "Description" type multiline_string access  read_write 
 *     element syspwd "System password" type password access  read_write  max  64 
 *     error invalid_length "invalid length" 
 * 
 * group state debug_info "Debug info on iDigi connector thread"
 *     element version "Version" type string access  read_only 
 *     element stacktop "Stack begin" type 0xhex access  read_only 
 *     element stacksize "Stack size" type hex32 access  read_only 
 *     element stackbottom "Stack end" type 0xhex access  read_only 
 *     element usedmem "Allocated memory used" type uint32 access  read_only 
 * 
 * # State configuration for GPS
 * # Must setup the following group in order for GPS shown in iDigi Cloud.
 * group state gps_stats "GPS"
 *     element latitude "Latitude" type float access read_only
 *     element longitude "Longitude" type float access read_only
 *
 * @endcode
 *
 */
