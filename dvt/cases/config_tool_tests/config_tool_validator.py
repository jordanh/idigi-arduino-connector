from __future__ import with_statement
from string import Template

#These are the regex strings of the errors
#expected to be thrown by the config parser.

NO_ERROR=None
UNRECOGNIZED_KEYWORD='.*Unrecognized keyword:'
INVALID_CHARACTER='.*Invalid character in the name:'

#GLOBALERROR
GLOBAL_NO_ERROR=NO_ERROR
GLOBAL_DESCRIPTION_ERROR='.*Missing globalerror description'
GLOBAL_NO_DESCRIPTION=GLOBAL_DESCRIPTION_ERROR
GLOBAL_UNRECOGNIZED_KEYWORD=UNRECOGNIZED_KEYWORD
GLOBAL_BAD_DESCRIPTION=GLOBAL_DESCRIPTION_ERROR
GLOBAL_BAD_NAME=UNRECOGNIZED_KEYWORD
GLOBAL_NO_NAME=INVALID_CHARACTER

#GROUP
GROUP_NO_ERROR=NO_ERROR
GROUP_BAD_HELP='.*No element specified'
GROUP_BAD_TYPE='.*Invalid setting or state keyword:'
GROUP_BAD_COUNT='.*Invalid instance count for the group:'
GROUP_DESCRIPTION_ERROR='.*Missing group description'
GROUP_NO_DESCRIPTION=GROUP_DESCRIPTION_ERROR
GROUP_BAD_DESCRIPTION=GROUP_DESCRIPTION_ERROR
GROUP_BAD_NAME=GROUP_DESCRIPTION_ERROR
GROUP_NO_NAME=INVALID_CHARACTER

#ERROR
ERROR_NO_ERROR=NO_ERROR
ERROR_NO_DESCRIPTION='.*Missing error description'
ERROR_BAD_NAME=UNRECOGNIZED_KEYWORD
ERROR_BAD_DESCRIPTION=ERROR_NO_DESCRIPTION
ERROR_NO_NAME=INVALID_CHARACTER

#ELEMENT
ELEMENT_NO_ERROR=NO_ERROR
ELEMENT_INVALID_TYPE_VALUE='.*Invalid.*value!'
ELEMENT_INVALID_ACCESS_TYPE='.*Invalid access Type:'
ELEMENT_INVALID__ELEMENT_TYPE='.*Invalid element Type:'
ELEMENT_MIN_GREATER_MAX='.*Error min value > max value!'
ELEMENT_NO_DESCRIPTION='.*Missing element description'
ELEMENT_MISSING_MAX='.*Bad or missing max value!'
ELEMENT_MISSING_MIN='.*Bad or missing min value!'
ELEMENT_NEGATIVE_MIN='.*Invalid min or max value for type:'
ELEMENT_MIN_MAX_NOT_SUPPORTED='.*type should not have <min/max>!'
ELEMENT_ENUM_MISSING_VALUE='.*Missing <value>!'
ELEMENT_ENUM_MISSING_TYPE='.*Missing type enum on element:'
ELEMENT_NO_UNIT='.*No Unit'
ELEMENT_MISSING_DESCRIPTION='.*Missing element description'
ELEMENT_INVALID_ACCESS_READ=ELEMENT_INVALID_ACCESS_TYPE
ELEMENT_MISSING_ACCESS=ELEMENT_INVALID_ACCESS_TYPE
ELEMENT_BAD_DESCRIPTION=ELEMENT_MISSING_DESCRIPTION
ELEMENT_BAD_HELP_DESCRIPTION=ELEMENT_INVALID__ELEMENT_TYPE
ELEMENT_NO_TYPE=ELEMENT_INVALID__ELEMENT_TYPE
ELEMENT_BAD_TYPE=ELEMENT_INVALID__ELEMENT_TYPE
ELEMENT_MISSING_TYPE=ELEMENT_INVALID__ELEMENT_TYPE
ELEMENT_BAD_NAME=ELEMENT_MISSING_DESCRIPTION
ELEMENT_WITH_MIN_ONLY=ELEMENT_INVALID_TYPE_VALUE
ELEMENT_MAX_GREATER_MAX_CONTENT=ELEMENT_INVALID_TYPE_VALUE
ELEMENT_NO_NAME=INVALID_CHARACTER
ELEMENT_BAD_UNIT=UNRECOGNIZED_KEYWORD

#List of dictionaries for globalerror test cases.
#'text' is what to substitute in the Template.
#'error' is the expected error string returned from the 
#    exception thrown by ConfigGenerator.jar
#'description' is used to uniquely identify the test case
#    which is used to run the unit test as well as help to
#    identify the test case (for debug and other things).
globalerrorTests=[{'text':'globalerror load_fail "Unable to load configuration"',
                   'error':GLOBAL_NO_ERROR,
                   'description':'globalerror_no_error'},
                  {'text':'globalerror load fail "Unable to load configuration"',
                   'error':GLOBAL_BAD_NAME,
                   'description':'globalerror_bad_name'},
                  {'text':'globalerror "Unable to load configuration"',
                   'error':GLOBAL_NO_NAME,
                   'description':'globalerror_no_name'},
                  {'text':'globalerror load_fail',
                   'error':GLOBAL_NO_DESCRIPTION,
                   'description':'globalerror_no_description'},
                  {'text':'globalerror load_fail 1234',
                   'error':GLOBAL_BAD_DESCRIPTION,
                   'description':'globalerror_bad_description_int'},
                  {'text':'globalerror load_fail DoesThisWork',
                   'error':GLOBAL_BAD_DESCRIPTION,
                   'description':'globalerror_bad_description_no_quote'},
                  ]

#List of dictionaries for group test cases.
#'text' is what to substitute in the Template.
#'error' is the expected error string returned from the 
#    exception thrown by ConfigGenerator.jar
#'description' is used to uniquely identify the test case
#    which is used to run the unit test as well as help to
#    identify the test case (for debug and other things).
groupTests=[{'text':'group setting serial 2 "Serial Port"',
            'error':GROUP_NO_ERROR,
            'description':'group_no_error_setting'},
           {'text':'group setting serial 2 "Serial Port" "help_description"',
            'error':GROUP_NO_ERROR,
            'description':'group_no_error_setting_with_help_description'},
           {'text':'group setting serial 2 "Serial Port" 12345',
            'error':GROUP_BAD_HELP,
            'description':'group_bad_help_description_setting'},
           {'text':'group setting "Serial Port"',
            'error':GROUP_NO_NAME,
            'description':'group_no_name_setting'},
           {'text':'group saywhat serial 2 "Serial Port"',
            'error':GROUP_BAD_TYPE,
            'description':'group_bad_type'},
           {'text':'group setting se rial 2 "Serial Port"',
            'error':GROUP_BAD_NAME,
            'description':'group_bad_name_setting'},
           {'text':'group setting serial 0 "Serial Port"',
            'error':GROUP_BAD_COUNT,
            'description':'group_bad_count_setting'},
           {'text':'group setting serial 2',
            'error':GROUP_NO_DESCRIPTION,
            'description':'group_no_description_setting'},
           {'text':'group setting serial "Serial Port"',
            'error':GROUP_NO_ERROR,
            'description':'group_no_count_setting'},
           {'text':'group setting serial 2 12345',
            'error':GROUP_BAD_DESCRIPTION,
            'description':'group_bad_description_setting'},
           {'text':'group state system_info "System Information"',
            'error':GROUP_NO_ERROR,
            'description':'group_no_error_state'},
           {'text':'group state system info "System Information"',
            'error':GROUP_BAD_NAME,
            'description':'group_bad_name_state'},
           {'text':'group state "System Information"',
            'error':GROUP_NO_NAME,
            'description':'group_no_name_state'},
           {'text':'group state system_info 3 "System Information"',
            'error':GROUP_NO_ERROR,
            'description':'group_no_error_with_count_state'},
           {'text':'group state system_info 0 "System Information"',
            'error':GROUP_BAD_COUNT,
            'description':'group_bad_count_state'},
           {'text':'group state system_info',
            'error':GROUP_NO_DESCRIPTION,
            'description':'group_no_description_state'},
           {'text':'group state system_info 12345',
            'error':GROUP_BAD_DESCRIPTION,
            'description':'group_bad_description_state'},
           {'text':'group state system_info "System Information" "help_description"',
            'error':GROUP_NO_ERROR,
            'description':'group_no_error_state_with_help_description'},
           {'text':'group state system_info "System Information" 12345',
            'error':GROUP_BAD_HELP,
            'description':'group_bad_help_description_state'},
           ]

#List of dictionaries for error test cases.
#'text' is what to substitute in the Template.
#'error' is the expected error string returned from the 
#    exception thrown by ConfigGenerator.jar
#'description' is used to uniquely identify the test case
#    which is used to run the unit test as well as help to
#    identify the test case (for debug and other things).
errorTests=[{'text':'error invalid_bits "invalid data bits rate"',
            'error':ERROR_NO_ERROR,
            'description':'error_no_error'},
           {'text':'error invalid bits "invalid data bits rate"',
            'error':ERROR_BAD_NAME,
            'description':'error_bad_Name'},
           {'text':'error "invalid data bits rate"',
            'error':ERROR_NO_NAME,
            'description':'error_no_name'},
           {'text':'error invalid_bits',
            'error':ERROR_NO_DESCRIPTION,
            'description':'error_no_description'},
           {'text':'error invalid_bits 12345',
            'error':ERROR_BAD_DESCRIPTION,
            'description':'error_bad_description_int'},
           {'text':'error invalid_bits DoesThisWork',
            'error':ERROR_BAD_DESCRIPTION,
            'description':'error_bad_description_no_quotes'},
           ]

#List of dictionaries of all the different element types
#'type' is the element type
#'min' is the minimum value possible. 
#'max' is the maximum value possible
#    SHOULD BE LESS THAN RCI_MAX_CONTENT_LENGTH.
#NOTE: Some types throw errors if min/max is used.
elementTypes=[{'type':'string',
        'min':0,'max':50},
       {'type':'multiline_string',
        'min':0,'max':50},
       {'type':'password',
        'min':0,'max':50},
       {'type':'int32',
        'min':-10,'max':50},
       {'type':'uint32',
        'min':0,'max':50},
       {'type':'hex32',
        'min':0,'max':'A'},
       {'type':'0xhex',
        'min':'0x0','max':'0xFF'},
       {'type':'float',
        'min':-3.2,'max':50.1},
       {'type':'enum',
        'min':0,'max':50},
       {'type':'on_off',
        'min':0,'max':50},
       {'type':'boolean',
        'min':0,'max':50},
       {'type':'ipv4',
        'min':0,'max':50},
       {'type':'fqdnv4',
        'min':0,'max':50},
       {'type':'fqdnv6',
        'min':0,'max':50},
       {'type':'datetime',
        'min':0,'max':50},]

#Returns a list of dictionaries for element test cases.
#'text' is what to substitute in the Template.
#'error' is the expected error string returned from the 
#    exception thrown by ConfigGenerator.jar
#'description' is used to uniquely identify the test case
#    which is used to run the unit test as well as help to
#    identify the test case (for debug and other things).
def getElementTests():
    #List of {TEST, ERROR, ERROR ON MIN/MAX, USING MM_PASS}
    #NOTE: The descriptions have to be unique so unittest will work
    #        this includes unique for each type, hence the Template.
    temps=[{'test':Template('element aname "A Description" type $type access read_write'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':False,
            'description':Template('element_${type}_no_error_access_read_write')},
           {'test':Template('element aname "A Description" type $type access read_only'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':False,
            'description':Template('element_${type}_no_error_access_read_only')},
           {'test':Template('element aname "A Description" type $type access write_only'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':False,
            'description':Template('element_${type}_no_error_access_write_only')},
           {'test':Template('element aname "A Description" type $type access read_write unit "aunit"'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':False,
            'description':Template('element_${type}_no_error_with_unit')},
           {'test':Template('element aname "A Description" type $type access read_write unit'),
            'error':ELEMENT_NO_UNIT,
            'minmax_test':False,
            'description':Template('element_${type}_no_unit')},
           {'test':Template('element aname "A Description" type $type access read_write unit 12345 "Help Description"'),
            'error':ELEMENT_BAD_UNIT,
            'minmax_test':False,
            'description':Template('element_${type}_bad_unit')},
           {'test':Template('element aname "A Description" type $type access read write'),
            'error':ELEMENT_INVALID_ACCESS_READ,
            'minmax_test':False,
            'description':Template('element_${type}_bad_access')},
           {'test':Template('element aname "A Description" type $type access'),
            'error':ELEMENT_MISSING_ACCESS,
            'minmax_test':False,
            'description':Template('element_${type}_missing_access')},
           {'test':Template('element aname "A Description" type $type'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':False,
            'description':Template('element_${type}_no_error_no_access')},
           {'test':Template('element aname "A Description" type'),
            'error':ELEMENT_MISSING_TYPE,
            'minmax_test':False,
            'description':Template('element_${type}_missing_type')},
           {'test':Template('element aname "A Description" type 1234 access read_write'),
            'error':ELEMENT_BAD_TYPE,
            'minmax_test':False,
            'description':Template('element_${type}_bad_type')},
           {'test':Template('element aname "A Description" access read_write'),
            'error':ELEMENT_NO_TYPE,
            'minmax_test':False,
            'description':Template('element_${type}_no_type')},
           {'test':Template('element aname 12345 type $type access read_write'),
            'error':ELEMENT_BAD_DESCRIPTION,
            'minmax_test':False,
            'description':Template('element_${type}_bad_description')},
           {'test':Template('element aname type $type access read_write'),
            'error':ELEMENT_NO_DESCRIPTION,
            'minmax_test':False,
            'description':Template('element_${type}_no_description')},
           {'test':Template('element aname "A Description" type $type min $min max $max access read_write'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':True,
            'description':Template('element_${type}_with_min_max')},
           {'test':Template('element aname "A Description" type $type min $min access read_write'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':True,
            'description':Template('element_${type}_with_min_only')},
           {'test':Template('element aname "A Description" type $type max $max access read_write'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':True,
            'description':Template('element_${type}_with_max_only')},
           {'test':Template('element aname "A Description" type $type min $max max $min access read_write'),
            'error':ELEMENT_MIN_GREATER_MAX,
            'minmax_test':True,
            'description':Template('element_${type}_min_greater_max')},
           {'test':Template('element aname "A Description" type $type min $min max $max_max access read_write'),
            'error':ELEMENT_MAX_GREATER_MAX_CONTENT,
            'minmax_test':True,
            'description':Template('element_${type}_max_greater_max_content')},
           {'test':Template('element aname "A Description" type $type min $min_minus max $max access read_write'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':True,
            'description':Template('element_${type}_negative_min')},
           {'test':Template('element aname "A Description" type $type min "Help Description"'),
            'error':ELEMENT_MISSING_MIN,
            'minmax_test':True,
            'description':Template('element_${type}_missing_min')},
           {'test':Template('element aname "A Description" type $type max "Help Description"'),
            'error':ELEMENT_MISSING_MAX,
            'minmax_test':True,
            'description':Template('element_${type}_missing_max')},
           {'test':Template('element aname 12345 type $type access read_write'),
            'error':ELEMENT_BAD_DESCRIPTION,
            'minmax_test':False,
            'description':Template('element_${type}_bad_description')},
           {'test':Template('element aname "A Description" "help_description" type $type access read_write'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':False,
            'description':Template('element_${type}_no_error_with_help_description')},
           {'test':Template('element aname "A Description" 12345 type $type access read_write'),
            'error':ELEMENT_BAD_HELP_DESCRIPTION,
            'minmax_test':False,
            'description':Template('element_${type}_bad_help_description')},
           {'test':Template('element aname "A Description" "help_description" type $type $mm_pass access read_write unit "aunit"'),
            'error':ELEMENT_NO_ERROR,
            'minmax_test':True,
            'description':Template('element_${type}_no_error_all_options_used')},
           {'test':Template('element a name "A Description" "help_description" type $type'),
            'error':ELEMENT_BAD_NAME,
            'minmax_test':False,
            'description':Template('element_${type}_bad_name')},
           {'test':Template('element "A Description" "help_description" type $type'),
            'error':ELEMENT_NO_NAME,
            'minmax_test':False,
            'description':Template('element_${type}_no_name')},
           ]
    #Append some values if the type is enum!
    enum_append="""\tvalue none\n\tvalue odd\n\tvalue even"""
    #List of types that should error if min/max options used
    minmax_error=['enum','on_off','boolean','ipv4','datetime']
    #List of types that are hex, not decimal
    hex_types=['0xhex','hex32']
    #List of types the should error if given a negative min
    neg_error=['fqdnv4','fqdnv6','multiline_string','password','string','uint32']
    #For every type, substitute it into the test case
    #and doing some other logic to determine proper
    #handling of the error string.
    types=elementTypes
    testList=[]
    for type in types:
        #yay for nested loops!
        for test in temps:
            testDict={}
            t = type['type']
            min=type['min']
            max=type['max']
            desc=test['description'].substitute(type=t)
            error=test['error']
            #Check to see if we need hex values instead
            #of decimal for min/max
            if t in hex_types:
                min_minus='0'
                max_max='FFFFFF'
                #Append 0x if needed
                if t in '0xhex':
                    min_minus='0x%s'%min_minus
                    max_max='0x%s'%max_max
            else:
                min_minus=-1000
                max_max=1000
            mm_pass = "min %s max %s" % (min,max)
            #############################################
            ##These are special case scenarios that we need
            ##to intervene to make is run nicely!
            ###############################################
            #Check to see if will throw error if min/max included
            if ((test['minmax_test']) and (t in minmax_error)):
            #Check to see if we need to set the error
            #to does not support min/max!
                if 'no_error_all_options_used' in desc:
                    #This case is to test all valid options.
                    #Since min/max isn't supported we'll 
                    #remove it so the test passes
                    mm_pass = ''
                else:
                    #We should get this error before all others!
                    error = ELEMENT_MIN_MAX_NOT_SUPPORTED
                    
            #If the type should error on negative value, change the error!
            if t in neg_error and 'negative_min' in desc:
                error = ELEMENT_NEGATIVE_MIN
#            #0xhex,hex32, and float are stupid and give a different error then others.
#            if ('element_0xhex_missing_max') in desc or ('element_0xhex_missing_min' in desc) or (
#                'element_hex32_missing_max') in desc or ('element_hex32_missing_min' in desc) or (
#                'element_float_missing_max') in desc or ('element_float_missing_min' in desc):
#                    error = ELEMENT_INVALID_TYPE_VALUE
            #Because there are values appending after, it gives a different error
            if 'element_enum_bad_unit' in desc:
                error = ELEMENT_ENUM_MISSING_VALUE
            #It knows it should be type of enum becasue we append the values after it.
            #However, it still errors telling you to specify enum
            if 'element_enum_no_type' in desc:
                error = ELEMENT_ENUM_MISSING_TYPE
            ###############################################
            ###############################################
            #Set the test case to the correct string for the type and options
            tst=test['test'].substitute(
                                        type=t,
                                        min=min,
                                        max=max,
                                        max_max=max_max,
                                        mm_pass=mm_pass,
                                        min_minus=min_minus)
            #Append the values if type is enum
            if 'enum' in t:
                tst = """%s\n%s""" % (tst, enum_append)
            #Create the dictionary and add it to the list.
            testDict['text']=tst
            testDict['error']=error
            testDict['description']=desc
            testList.append(testDict)
    return testList

#ELEMENT
elementTests=getElementTests()

#The following are the templates used for the different keywords.
globalerrorTemplate=Template("""
${test}
globalerror save_fail "Save fail"
globalerror memory_fail "Insufficient memory"

group setting serial 2 "Serial Port" "Port 1 is used for printf"
    element baud "Baud rate" type enum access read_write
        value 2400
        value 4800
        value 9600
        value 19200
        value 38400
        value 57600
        value 115200
        value 230400  
    element parity "Parity" type enum access  read_write 
        value none
        value odd
        value even
    element databits "Data bits" type uint32 access  read_write  min  5 max  8 
    element xbreak "Tx Break" type on_off access  read_write 
    element txbytes "Tx bytes" type uint32 access  read_only 
    error invalid_baud "Invalid baud rate "
    error invalid_databits "Invalid data bits"
    error invalid_parity  " Invalid parity"
    error invalid_xbreak "Invalid xbreak setting"
    error invalid_databits_parity "Invalid combination of data bits and parity"

group setting device_info  "Device info"
    element product "Product" type string access  read_write  min  1 max  64 
    element model "Model" type string access  read_write  min  0 max  32 
    element company "Company" type string access  read_write 
    element desc "Description" type multiline_string access  read_write 
    element syspwd "System password" type password access  read_write  max  64 
    error invalid_length "invalid length" 

group state debug_info "Debug info on iDigi connector thread"
    element version "Version" type string access  read_only 
    element stacktop "Stack begin" type 0xhex access  read_only 
    element stacksize "Stack size" type hex32 access  read_only 
    element stackbottom "Stack end" type 0xhex access  read_only 
    element usedmem "Allocated memory used" type uint32 access  read_only 

# State configuration for GPS
# Must setup the following group in order for GPS shown in iDigi Cloud.
group state gps_stats "GPS"
    element latitude "Latitude" type float access read_only
    element longitude "Longitude" type float access read_only""")

groupTemplate=Template("""
globalerror load_fail "Load fail"
globalerror save_fail "Save fail"
globalerror memory_fail "Insufficient memory"

${test}
    element baud "Baud rate" type enum access read_write
        value 2400
        value 4800
        value 9600
        value 19200
        value 38400
        value 57600
        value 115200
        value 230400  
    element parity "Parity" type enum access  read_write 
        value none
        value odd
        value even
    element databits "Data bits" type uint32 access  read_write  min  5 max  8 
    element xbreak "Tx Break" type on_off access  read_write 
    element txbytes "Tx bytes" type uint32 access  read_only 
    error invalid_baud "Invalid baud rate "
    error invalid_databits "Invalid data bits"
    error invalid_parity  " Invalid parity"
    error invalid_xbreak "Invalid xbreak setting"
    error invalid_databits_parity "Invalid combination of data bits and parity"

group setting device_info  "Device info"
    element product "Product" type string access  read_write  min  1 max  64 
    element model "Model" type string access  read_write  min  0 max  32 
    element company "Company" type string access  read_write 
    element desc "Description" type multiline_string access  read_write 
    element syspwd "System password" type password access  read_write  max  64 
    error invalid_length "invalid length" 

group state debug_info "Debug info on iDigi connector thread"
    element version "Version" type string access  read_only 
    element stacktop "Stack begin" type 0xhex access  read_only 
    element stacksize "Stack size" type hex32 access  read_only 
    element stackbottom "Stack end" type 0xhex access  read_only 
    element usedmem "Allocated memory used" type uint32 access  read_only 

# State configuration for GPS
# Must setup the following group in order for GPS shown in iDigi Cloud.
group state gps_stats "GPS"
    element latitude "Latitude" type float access read_only
    element longitude "Longitude" type float access read_only""")

errorTemplate=Template("""
globalerror load_fail "Load fail"
globalerror save_fail "Save fail"
globalerror memory_fail "Insufficient memory"

group setting serial 2 "Serial Port" "Port 1 is used for printf"
    element baud "Baud rate" type enum access read_write
        value 2400
        value 4800
        value 9600
        value 19200
        value 38400
        value 57600
        value 115200
        value 230400  
    element parity "Parity" type enum access  read_write 
        value none
        value odd
        value even
    element databits "Data bits" type uint32 access  read_write  min  5 max  8 
    element xbreak "Tx Break" type on_off access  read_write 
    element txbytes "Tx bytes" type uint32 access  read_only 
    ${test}
    error invalid_databits "Invalid data bits"
    error invalid_parity  " Invalid parity"
    error invalid_xbreak "Invalid xbreak setting"
    error invalid_databits_parity "Invalid combination of data bits and parity"

group setting device_info  "Device info"
    element product "Product" type string access  read_write  min  1 max  64 
    element model "Model" type string access  read_write  min  0 max  32 
    element company "Company" type string access  read_write 
    element desc "Description" type multiline_string access  read_write 
    element syspwd "System password" type password access  read_write  max  64 
    error invalid_length "invalid length" 

group state debug_info "Debug info on iDigi connector thread"
    element version "Version" type string access  read_only 
    element stacktop "Stack begin" type 0xhex access  read_only 
    element stacksize "Stack size" type hex32 access  read_only 
    element stackbottom "Stack end" type 0xhex access  read_only 
    element usedmem "Allocated memory used" type uint32 access  read_only 

# State configuration for GPS
# Must setup the following group in order for GPS shown in iDigi Cloud.
group state gps_stats "GPS"
    element latitude "Latitude" type float access read_only
    element longitude "Longitude" type float access read_only""")

elementTemplate=Template("""
globalerror load_fail "Load fail"
globalerror save_fail "Save fail"
globalerror memory_fail "Insufficient memory"

group setting serial 2 "Serial Port" "Port 1 is used for printf"
    element baud "Baud rate" type enum access read_write
        value 2400
        value 4800
        value 9600
        value 19200
        value 38400
        value 57600
        value 115200
        value 230400  
    ${test}
    element databits "Data bits" type uint32 access  read_write  min  5 max  8 
    element xbreak "Tx Break" type on_off access  read_write 
    element txbytes "Tx bytes" type uint32 access  read_only 
    error invalid_baud "Invalid baud rate "
    error invalid_databits "Invalid data bits"
    error invalid_parity  " Invalid parity"
    error invalid_xbreak "Invalid xbreak setting"
    error invalid_databits_parity "Invalid combination of data bits and parity"

group setting device_info  "Device info"
    element product "Product" type string access  read_write  min  1 max  64 
    element model "Model" type string access  read_write  min  0 max  32 
    element company "Company" type string access  read_write 
    element desc "Description" type multiline_string access  read_write 
    element syspwd "System password" type password access  read_write  max  64 
    error invalid_length "invalid length" 

group state debug_info "Debug info on iDigi connector thread"
    element version "Version" type string access  read_only 
    element stacktop "Stack begin" type 0xhex access  read_only 
    element stacksize "Stack size" type hex32 access  read_only 
    element stackbottom "Stack end" type 0xhex access  read_only 
    element usedmem "Allocated memory used" type uint32 access  read_only 

# State configuration for GPS
# Must setup the following group in order for GPS shown in iDigi Cloud.
group state gps_stats "GPS"
    element latitude "Latitude" type float access read_only
    element longitude "Longitude" type float access read_only""")


class TestCase(object):
    def __init__(self, text, error, description):
        self.text = text
        self.error = error
        self.description = description

    def __repr__(self):
        return self.description


TEST_LIST=[{'tests':globalerrorTests,'template':globalerrorTemplate},
          {'tests':groupTests, 'template':groupTemplate},
          {'tests':errorTests,'template':errorTemplate},
          {'tests':elementTests,'template':elementTemplate}
          ]

#Take the list variable above and create a dictionary of:
# {'text':'Substituted string in template',
#  'error': 'Expected error string returned (or None)',
#  'description': 'description used to uniquely id test cases'}
def get_list(list=TEST_LIST):
    "Returns list of dictionaries to be tested"
    retrnList=[]
    for keyword in list:
         templt=keyword['template']
         for tc in keyword['tests']:
            case = TestCase('%s'%templt.substitute(test='%s'%tc['text']), 
                tc['error'], tc['description'])
            testD={'text':'%s'%templt.substitute(test='%s'%tc['text']),
                    'error':tc['error'],
                    'description':tc['description']}
            retrnList.append(case)
    return retrnList

def printTests(testList=get_list(),filename='output.txt'):
    "Print test list (same format as to be passed to runTests)"
    testNum = 1
    with open(filename,'w') as f:
        for tc in testList:
            f.write('\nTEST CASE %d:\n\n' % testNum)
            f.write('\tDESCRIPTION:\n')
            f.write('\t\t%s\n'%tc['description'])
            f.write('\n\tEXPECTED ERROR:\n')
            f.write('\t\t%s\n'%tc['error'])
            f.write('\n\tTEST CONTENT:\n')
            f.write('%s\n'%tc['text'])
            f.write('******************************************\n')
            testNum += 1
