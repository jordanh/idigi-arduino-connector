/*
 * This is an auto-generated file - DO NOT EDIT! 
 * This is generated by com.digi.ic.config.ConfigGenerator tool 
 * This file was generated on: 2012/04/12 16:17:54 
 * The command line arguments were: "mlchan:***** "Linux Application" 1 config.rci" 
 * The version of com.digi.ic.config.ConfigGenerator tool was: 1.0.0.0
*/

#include "idigi_remote.h"

#define SETTING_SERIAL    (idigi_remote_all_strings + 86)
#define SETTING_SERIAL_BAUD    (idigi_remote_all_strings + 93)
#define SETTING_SERIAL_BAUD_2400    (idigi_remote_all_strings + 98)
#define SETTING_SERIAL_BAUD_4800    (idigi_remote_all_strings + 103)
#define SETTING_SERIAL_BAUD_9600    (idigi_remote_all_strings + 108)
#define SETTING_SERIAL_BAUD_19200    (idigi_remote_all_strings + 113)
#define SETTING_SERIAL_BAUD_38400    (idigi_remote_all_strings + 119)
#define SETTING_SERIAL_BAUD_57600    (idigi_remote_all_strings + 125)
#define SETTING_SERIAL_BAUD_115200    (idigi_remote_all_strings + 131)
#define SETTING_SERIAL_BAUD_230400    (idigi_remote_all_strings + 138)
#define SETTING_SERIAL_PARITY    (idigi_remote_all_strings + 145)
#define SETTING_SERIAL_PARITY_NONE    (idigi_remote_all_strings + 152)
#define SETTING_SERIAL_PARITY_ODD    (idigi_remote_all_strings + 157)
#define SETTING_SERIAL_PARITY_EVEN    (idigi_remote_all_strings + 161)
#define SETTING_SERIAL_DATABITS    (idigi_remote_all_strings + 166)
#define SETTING_SERIAL_XBREAK    (idigi_remote_all_strings + 175)
#define SETTING_SERIAL_TXBYTES    (idigi_remote_all_strings + 182)
#define SETTING_SERIAL_ERROR_INVALID_BAUD    (idigi_remote_all_strings + 190)
#define SETTING_SERIAL_ERROR_INVALID_DATABITS    (idigi_remote_all_strings + 203)
#define SETTING_SERIAL_ERROR_INVALID_PARITY    (idigi_remote_all_strings + 220)
#define SETTING_SERIAL_ERROR_INVALID_XBREAK    (idigi_remote_all_strings + 235)
#define SETTING_SERIAL_ERROR_INVALID_DATABITS_PARITY    (idigi_remote_all_strings + 250)
#define SETTING_ETHERNET    (idigi_remote_all_strings + 274)
#define SETTING_ETHERNET_IP    (idigi_remote_all_strings + 283)
#define SETTING_ETHERNET_SUBNET    (idigi_remote_all_strings + 286)
#define SETTING_ETHERNET_GATEWAY    (idigi_remote_all_strings + 293)
#define SETTING_ETHERNET_DHCP    (idigi_remote_all_strings + 301)
#define SETTING_ETHERNET_DNS    (idigi_remote_all_strings + 306)
#define SETTING_ETHERNET_DUPLEX    (idigi_remote_all_strings + 310)
#define SETTING_ETHERNET_DUPLEX_AUTO    (idigi_remote_all_strings + 317)
#define SETTING_ETHERNET_DUPLEX_HALF    (idigi_remote_all_strings + 322)
#define SETTING_ETHERNET_DUPLEX_FULL    (idigi_remote_all_strings + 327)
#define SETTING_ETHERNET_ERROR_INVALID_DUPLEX    (idigi_remote_all_strings + 332)
#define SETTING_ETHERNET_ERROR_INVALID_IP    (idigi_remote_all_strings + 347)
#define SETTING_ETHERNET_ERROR_INVALID_SUBNET    (idigi_remote_all_strings + 358)
#define SETTING_ETHERNET_ERROR_INVALID_GATEWAY    (idigi_remote_all_strings + 373)
#define SETTING_ETHERNET_ERROR_INVALID_DNS    (idigi_remote_all_strings + 389)
#define SETTING_DEVICE_STATS    (idigi_remote_all_strings + 401)
#define SETTING_DEVICE_STATS_CURTIME    (idigi_remote_all_strings + 414)
#define SETTING_DEVICE_STATS_CTEMP    (idigi_remote_all_strings + 422)
#define SETTING_DEVICE_STATS_FTEMP    (idigi_remote_all_strings + 428)
#define SETTING_DEVICE_INFO    (idigi_remote_all_strings + 434)
#define SETTING_DEVICE_INFO_PRODUCT    (idigi_remote_all_strings + 446)
#define SETTING_DEVICE_INFO_MODEL    (idigi_remote_all_strings + 454)
#define SETTING_DEVICE_INFO_COMPANY    (idigi_remote_all_strings + 460)
#define SETTING_DEVICE_INFO_DESC    (idigi_remote_all_strings + 468)
#define SETTING_DEVICE_INFO_SYSPWD    (idigi_remote_all_strings + 473)
#define SETTING_DEVICE_INFO_ERROR_INVALID_LENGTH    (idigi_remote_all_strings + 480)
#define STATE_DEBUG_INFO    (idigi_remote_all_strings + 495)
#define STATE_DEBUG_INFO_VERSION    (idigi_remote_all_strings + 506)
#define STATE_DEBUG_INFO_STACKTOP    (idigi_remote_all_strings + 514)
#define STATE_DEBUG_INFO_STACKSIZE    (idigi_remote_all_strings + 523)
#define STATE_DEBUG_INFO_STACKBOTTOM    (idigi_remote_all_strings + 533)
#define STATE_DEBUG_INFO_USEDMEM    (idigi_remote_all_strings + 545)
#define IDIGI_RCI_ERROR_PARSER_ERROR    (idigi_remote_all_strings + 553)
#define IDIGI_RCI_ERROR_BAD_XML    (idigi_remote_all_strings + 566)
#define IDIGI_RCI_ERROR_BAD_COMMAND    (idigi_remote_all_strings + 574)
#define IDIGI_RCI_ERROR_INVALID_VERSION    (idigi_remote_all_strings + 586)
#define IDIGI_RCI_ERROR_BAD_GROUP    (idigi_remote_all_strings + 602)
#define IDIGI_RCI_ERROR_BAD_INDEX    (idigi_remote_all_strings + 612)
#define IDIGI_RCI_ERROR_BAD_ELEMENT    (idigi_remote_all_strings + 622)
#define IDIGI_RCI_ERROR_BAD_VALUE    (idigi_remote_all_strings + 634)
#define IDIGI_GLOBAL_ERROR_LOAD_FAIL    (idigi_remote_all_strings + 644)
#define IDIGI_GLOBAL_ERROR_SAVE_FAIL    (idigi_remote_all_strings + 654)
#define IDIGI_GLOBAL_ERROR_MEMORY_FAIL    (idigi_remote_all_strings + 664)

char const idigi_remote_all_strings[] = {
    4,'q','u','o','t',
    3,'a','m','p',
    4,'a','p','o','s',
    2,'l','t',
    2,'g','t',
    11,'s','e','t','_','s','e','t','t','i','n','g',
    13,'q','u','e','r','y','_','s','e','t','t','i','n','g',
    9,'s','e','t','_','s','t','a','t','e',
    11,'q','u','e','r','y','_','s','t','a','t','e',
    11,'v','e','r','s','i','o','n',' ','1','.','1',
    5,'i','n','d','e','x',
    6,'s','e','r','i','a','l',
    4,'b','a','u','d',
    4,'2','4','0','0',
    4,'4','8','0','0',
    4,'9','6','0','0',
    5,'1','9','2','0','0',
    5,'3','8','4','0','0',
    5,'5','7','6','0','0',
    6,'1','1','5','2','0','0',
    6,'2','3','0','4','0','0',
    6,'p','a','r','i','t','y',
    4,'n','o','n','e',
    3,'o','d','d',
    4,'e','v','e','n',
    8,'d','a','t','a','b','i','t','s',
    6,'x','b','r','e','a','k',
    7,'t','x','b','y','t','e','s',
    12,'i','n','v','a','l','i','d','_','b','a','u','d',
    16,'i','n','v','a','l','i','d','_','d','a','t','a','b','i','t','s',
    14,'i','n','v','a','l','i','d','_','p','a','r','i','t','y',
    14,'i','n','v','a','l','i','d','_','x','b','r','e','a','k',
    23,'i','n','v','a','l','i','d','_','d','a','t','a','b','i','t','s','_','p','a','r','i','t','y',
    8,'e','t','h','e','r','n','e','t',
    2,'i','p',
    6,'s','u','b','n','e','t',
    7,'g','a','t','e','w','a','y',
    4,'d','h','c','p',
    3,'d','n','s',
    6,'d','u','p','l','e','x',
    4,'a','u','t','o',
    4,'h','a','l','f',
    4,'f','u','l','l',
    14,'i','n','v','a','l','i','d','_','d','u','p','l','e','x',
    10,'i','n','v','a','l','i','d','_','i','p',
    14,'i','n','v','a','l','i','d','_','s','u','b','n','e','t',
    15,'i','n','v','a','l','i','d','_','g','a','t','e','w','a','y',
    11,'i','n','v','a','l','i','d','_','d','n','s',
    12,'d','e','v','i','c','e','_','s','t','a','t','s',
    7,'c','u','r','t','i','m','e',
    5,'c','t','e','m','p',
    5,'f','t','e','m','p',
    11,'d','e','v','i','c','e','_','i','n','f','o',
    7,'p','r','o','d','u','c','t',
    5,'m','o','d','e','l',
    7,'c','o','m','p','a','n','y',
    4,'d','e','s','c',
    6,'s','y','s','p','w','d',
    14,'i','n','v','a','l','i','d','_','l','e','n','g','t','h',
    10,'d','e','b','u','g','_','i','n','f','o',
    7,'v','e','r','s','i','o','n',
    8,'s','t','a','c','k','t','o','p',
    9,'s','t','a','c','k','s','i','z','e',
    11,'s','t','a','c','k','b','o','t','t','o','m',
    7,'u','s','e','d','m','e','m',
    12,'p','a','r','s','e','r','_','e','r','r','o','r',
    7,'b','a','d','_','x','m','l',
    11,'b','a','d','_','c','o','m','m','a','n','d',
    15,'i','n','v','a','l','i','d','_','v','e','r','s','i','o','n',
    9,'b','a','d','_','g','r','o','u','p',
    9,'b','a','d','_','i','n','d','e','x',
    11,'b','a','d','_','e','l','e','m','e','n','t',
    9,'b','a','d','_','v','a','l','u','e',
    9,'l','o','a','d','_','f','a','i','l',
    9,'s','a','v','e','_','f','a','i','l',
    11,'m','e','m','o','r','y','_','f','a','i','l',
    '\0'
};
static char const * const setting_serial_baud_enum[] = {
    SETTING_SERIAL_BAUD_2400, /*2400*/
    SETTING_SERIAL_BAUD_4800, /*4800*/
    SETTING_SERIAL_BAUD_9600, /*9600*/
    SETTING_SERIAL_BAUD_19200, /*19200*/
    SETTING_SERIAL_BAUD_38400, /*38400*/
    SETTING_SERIAL_BAUD_57600, /*57600*/
    SETTING_SERIAL_BAUD_115200, /*115200*/
    SETTING_SERIAL_BAUD_230400 /*230400*/
};

static idigi_element_value_enum_t const setting_serial_baud_limit = {
    asizeof(setting_serial_baud_enum), 
    setting_serial_baud_enum
};

static char const * const setting_serial_parity_enum[] = {
    SETTING_SERIAL_PARITY_NONE, /*none*/
    SETTING_SERIAL_PARITY_ODD, /*odd*/
    SETTING_SERIAL_PARITY_EVEN /*even*/
};

static idigi_element_value_enum_t const setting_serial_parity_limit = {
    asizeof(setting_serial_parity_enum), 
    setting_serial_parity_enum
};

static idigi_element_value_unsigned_integer_t const setting_serial_databits_limit = {
    5,
    8
};

static idigi_group_element_t const setting_serial_elements[] = {
    {    SETTING_SERIAL_BAUD,  /*baud*/

        idigi_element_access_read_write,
        idigi_element_type_enum,
        (idigi_element_value_limit_t *)&setting_serial_baud_limit
    },
    {    SETTING_SERIAL_PARITY,  /*parity*/

        idigi_element_access_read_write,
        idigi_element_type_enum,
        (idigi_element_value_limit_t *)&setting_serial_parity_limit
    },
    {    SETTING_SERIAL_DATABITS,  /*databits*/

        idigi_element_access_read_write,
        idigi_element_type_uint32,
        (idigi_element_value_limit_t *)&setting_serial_databits_limit
    },
    {    SETTING_SERIAL_XBREAK,  /*xbreak*/

        idigi_element_access_read_write,
        idigi_element_type_on_off,
        NULL
    },
    {    SETTING_SERIAL_TXBYTES,  /*txbytes*/

        idigi_element_access_read_only,
        idigi_element_type_uint32,
        NULL
    }
};

static char const * const setting_serial_errors[] = {
    SETTING_SERIAL_ERROR_INVALID_BAUD, /*invalid_baud*/
    SETTING_SERIAL_ERROR_INVALID_DATABITS, /*invalid_databits*/
    SETTING_SERIAL_ERROR_INVALID_PARITY, /*invalid_parity*/
    SETTING_SERIAL_ERROR_INVALID_XBREAK, /*invalid_xbreak*/
    SETTING_SERIAL_ERROR_INVALID_DATABITS_PARITY /*invalid_databits_parity*/
};

static char const * const setting_ethernet_duplex_enum[] = {
    SETTING_ETHERNET_DUPLEX_AUTO, /*auto*/
    SETTING_ETHERNET_DUPLEX_HALF, /*half*/
    SETTING_ETHERNET_DUPLEX_FULL /*full*/
};

static idigi_element_value_enum_t const setting_ethernet_duplex_limit = {
    asizeof(setting_ethernet_duplex_enum), 
    setting_ethernet_duplex_enum
};

static idigi_group_element_t const setting_ethernet_elements[] = {
    {    SETTING_ETHERNET_IP,  /*ip*/

        idigi_element_access_read_write,
        idigi_element_type_ipv4,
        NULL
    },
    {    SETTING_ETHERNET_SUBNET,  /*subnet*/

        idigi_element_access_read_write,
        idigi_element_type_ipv4,
        NULL
    },
    {    SETTING_ETHERNET_GATEWAY,  /*gateway*/

        idigi_element_access_read_write,
        idigi_element_type_ipv4,
        NULL
    },
    {    SETTING_ETHERNET_DHCP,  /*dhcp*/

        idigi_element_access_read_write,
        idigi_element_type_boolean,
        NULL
    },
    {    SETTING_ETHERNET_DNS,  /*dns*/

        idigi_element_access_read_write,
        idigi_element_type_fqdnv4,
        NULL
    },
    {    SETTING_ETHERNET_DUPLEX,  /*duplex*/

        idigi_element_access_read_write,
        idigi_element_type_enum,
        (idigi_element_value_limit_t *)&setting_ethernet_duplex_limit
    }
};

static char const * const setting_ethernet_errors[] = {
    SETTING_ETHERNET_ERROR_INVALID_DUPLEX, /*invalid_duplex*/
    SETTING_ETHERNET_ERROR_INVALID_IP, /*invalid_ip*/
    SETTING_ETHERNET_ERROR_INVALID_SUBNET, /*invalid_subnet*/
    SETTING_ETHERNET_ERROR_INVALID_GATEWAY, /*invalid_gateway*/
    SETTING_ETHERNET_ERROR_INVALID_DNS /*invalid_dns*/
};

static idigi_group_element_t const setting_device_stats_elements[] = {
    {    SETTING_DEVICE_STATS_CURTIME,  /*curtime*/

        idigi_element_access_read_write,
        idigi_element_type_datetime,
        NULL
    },
    {    SETTING_DEVICE_STATS_CTEMP,  /*ctemp*/

        idigi_element_access_read_write,
        idigi_element_type_float,
        NULL
    },
    {    SETTING_DEVICE_STATS_FTEMP,  /*ftemp*/

        idigi_element_access_read_only,
        idigi_element_type_float,
        NULL
    }
};

static idigi_element_value_unsigned_integer_t const setting_device_info_product_limit = {
    1,
    64
};

static idigi_element_value_unsigned_integer_t const setting_device_info_model_limit = {
    0,
    32
};

static idigi_element_value_unsigned_integer_t const setting_device_info_syspwd_limit = {
    0,
    64
};

static idigi_group_element_t const setting_device_info_elements[] = {
    {    SETTING_DEVICE_INFO_PRODUCT,  /*product*/

        idigi_element_access_read_write,
        idigi_element_type_string,
        (idigi_element_value_limit_t *)&setting_device_info_product_limit
    },
    {    SETTING_DEVICE_INFO_MODEL,  /*model*/

        idigi_element_access_read_write,
        idigi_element_type_string,
        (idigi_element_value_limit_t *)&setting_device_info_model_limit
    },
    {    SETTING_DEVICE_INFO_COMPANY,  /*company*/

        idigi_element_access_read_write,
        idigi_element_type_string,
        NULL
    },
    {    SETTING_DEVICE_INFO_DESC,  /*desc*/

        idigi_element_access_read_write,
        idigi_element_type_multiline_string,
        NULL
    },
    {    SETTING_DEVICE_INFO_SYSPWD,  /*syspwd*/

        idigi_element_access_read_write,
        idigi_element_type_password,
        (idigi_element_value_limit_t *)&setting_device_info_syspwd_limit
    }
};

static char const * const setting_device_info_errors[] = {
    SETTING_DEVICE_INFO_ERROR_INVALID_LENGTH /*invalid_length*/
};

static idigi_group_t const idigi_setting_groups[] = {
    {    SETTING_SERIAL,  /*serial*/

        2,
        {    asizeof(setting_serial_elements),
            setting_serial_elements
        },
        {    asizeof(setting_serial_errors),
            setting_serial_errors
        }
    },
    {    SETTING_ETHERNET,  /*ethernet*/

        1,
        {    asizeof(setting_ethernet_elements),
            setting_ethernet_elements
        },
        {    asizeof(setting_ethernet_errors),
            setting_ethernet_errors
        }
    },
    {    SETTING_DEVICE_STATS,  /*device_stats*/

        1,
        {    asizeof(setting_device_stats_elements),
            setting_device_stats_elements
        },
        {    0,
            NULL
        }
    },
    {    SETTING_DEVICE_INFO,  /*device_info*/

        1,
        {    asizeof(setting_device_info_elements),
            setting_device_info_elements
        },
        {    asizeof(setting_device_info_errors),
            setting_device_info_errors
        }
    }
};

static idigi_group_element_t const state_debug_info_elements[] = {
    {    STATE_DEBUG_INFO_VERSION,  /*version*/

        idigi_element_access_read_only,
        idigi_element_type_string,
        NULL
    },
    {    STATE_DEBUG_INFO_STACKTOP,  /*stacktop*/

        idigi_element_access_read_only,
        idigi_element_type_0xhex,
        NULL
    },
    {    STATE_DEBUG_INFO_STACKSIZE,  /*stacksize*/

        idigi_element_access_read_only,
        idigi_element_type_hex32,
        NULL
    },
    {    STATE_DEBUG_INFO_STACKBOTTOM,  /*stackbottom*/

        idigi_element_access_read_only,
        idigi_element_type_0xhex,
        NULL
    },
    {    STATE_DEBUG_INFO_USEDMEM,  /*usedmem*/

        idigi_element_access_read_only,
        idigi_element_type_uint32,
        NULL
    }
};

static idigi_group_t const idigi_state_groups[] = {
    {    STATE_DEBUG_INFO,  /*debug_info*/

        1,
        {    asizeof(state_debug_info_elements),
            state_debug_info_elements
        },
        {    0,
            NULL
        }
    }
};

char const * const idigi_rci_errors[11] = {
    IDIGI_RCI_ERROR_PARSER_ERROR, /*parser_error*/
    IDIGI_RCI_ERROR_BAD_XML, /*bad_xml*/
    IDIGI_RCI_ERROR_BAD_COMMAND, /*bad_command*/
    IDIGI_RCI_ERROR_INVALID_VERSION, /*invalid_version*/
    IDIGI_RCI_ERROR_BAD_GROUP, /*bad_group*/
    IDIGI_RCI_ERROR_BAD_INDEX, /*bad_index*/
    IDIGI_RCI_ERROR_BAD_ELEMENT, /*bad_element*/
    IDIGI_RCI_ERROR_BAD_VALUE, /*bad_value*/
    IDIGI_GLOBAL_ERROR_LOAD_FAIL, /*load_fail*/
    IDIGI_GLOBAL_ERROR_SAVE_FAIL, /*save_fail*/
    IDIGI_GLOBAL_ERROR_MEMORY_FAIL /*memory_fail*/
};

idigi_group_table_t const idigi_group_table[2] = {
    {idigi_setting_groups,
     asizeof(idigi_setting_groups)
    },
    {idigi_state_groups,
     asizeof(idigi_state_groups)
    }
};

