/*
 * This is an auto-generated file - DO NOT EDIT! 
 * This is generated by ConfigGenerator tool 
 * This file was generated on: 2012/06/05 11:47:45 
 * The command line arguments were: "-server=test.idigi.com mlchan:***** "Linux Application" 1.0.0.0 config.rci"
 * The version of ConfigGenerator tool was: 1.0.0.0 */

#ifndef remote_config_h
#define remote_config_h

#define RCI_PARSER_USES_ERROR_DESCRIPTIONS
#define RCI_PARSER_USES_STRINGS
#define RCI_PARSER_USES_SIGNED_INTEGER
#define RCI_PARSER_USES_UNSIGNED_INTEGER
#define RCI_PARSER_USES_FLOATING_POINT
#define RCI_PARSER_USES_ENUMERATIONS
#define RCI_PARSER_USES_ON_OFF
#define RCI_PARSER_USES_BOOLEAN

#include "float.h"

#include "idigi_remote.h"

extern char const idigi_remote_all_strings[];

#define RCI_ENTITY_QUOTE (idigi_remote_all_strings+0)
#define RCI_ENTITY_AMPERSAND (idigi_remote_all_strings+5)
#define RCI_ENTITY_APOSTROPHE (idigi_remote_all_strings+9)
#define RCI_ENTITY_LESS_THAN (idigi_remote_all_strings+14)
#define RCI_ENTITY_GREATER_THAN (idigi_remote_all_strings+17)
#define RCI_SET_SETTING (idigi_remote_all_strings+20)
#define RCI_QUERY_SETTING (idigi_remote_all_strings+32)
#define RCI_SET_STATE (idigi_remote_all_strings+46)
#define RCI_QUERY_STATE (idigi_remote_all_strings+56)
#define RCI_VERSION (idigi_remote_all_strings+68)
#define RCI_VERSION_SUPPORTED (idigi_remote_all_strings+76)
#define RCI_REPLY (idigi_remote_all_strings+80)
#define RCI_REQUEST (idigi_remote_all_strings+90)
#define RCI_INDEX (idigi_remote_all_strings+102)
#define RCI_ID (idigi_remote_all_strings+108)
#define RCI_HINT (idigi_remote_all_strings+111)
#define RCI_ERROR (idigi_remote_all_strings+116)
#define RCI_RCI_COMMENT (idigi_remote_all_strings+122)
#define RCI_DESC (idigi_remote_all_strings+126)
#define RCI_ON (idigi_remote_all_strings+131)
#define RCI_OFF (idigi_remote_all_strings+134)
#define RCI_TRUE (idigi_remote_all_strings+138)
#define RCI_FALSE (idigi_remote_all_strings+143)

typedef enum {
 idigi_rci_error_OFFSET = 1,
 idigi_rci_error_parser_error =  idigi_rci_error_OFFSET,
 idigi_rci_error_bad_xml,
 idigi_rci_error_bad_command,
 idigi_rci_error_invalid_version,
 idigi_rci_error_bad_group,
 idigi_rci_error_bad_index,
 idigi_rci_error_bad_element,
 idigi_rci_error_bad_value,
 idigi_rci_error_COUNT
} idigi_rci_error_id_t;


typedef enum {
 idigi_global_error_OFFSET = idigi_rci_error_COUNT,
 idigi_global_error_load_fail =  idigi_global_error_OFFSET,
 idigi_global_error_save_fail,
 idigi_global_error_memory_fail,
 idigi_global_error_COUNT
} idigi_global_error_id_t;

typedef enum {
 idigi_setting_serial_baud_2400,
 idigi_setting_serial_baud_4800,
 idigi_setting_serial_baud_9600,
 idigi_setting_serial_baud_19200,
 idigi_setting_serial_baud_38400,
 idigi_setting_serial_baud_57600,
 idigi_setting_serial_baud_115200,
 idigi_setting_serial_baud_230400,
 idigi_setting_serial_baud_COUNT
} idigi_setting_serial_baud_id_t;

typedef enum {
 idigi_setting_serial_parity_none,
 idigi_setting_serial_parity_odd,
 idigi_setting_serial_parity_even,
 idigi_setting_serial_parity_COUNT
} idigi_setting_serial_parity_id_t;

typedef enum {
 idigi_setting_serial_baud,
 idigi_setting_serial_parity,
 idigi_setting_serial_databits,
 idigi_setting_serial_xbreak,
 idigi_setting_serial_txbytes,
 idigi_setting_serial_COUNT
} idigi_setting_serial_id_t;

typedef enum {
 idigi_setting_serial_error_invalid_baud = idigi_global_error_COUNT,
 idigi_setting_serial_error_invalid_databits,
 idigi_setting_serial_error_invalid_parity,
 idigi_setting_serial_error_invalid_xbreak,
 idigi_setting_serial_error_invalid_databits_parity,
 idigi_setting_serial_error_COUNT
} idigi_setting_serial_error_id_t;

typedef enum {
 idigi_setting_ethernet_duplex_auto,
 idigi_setting_ethernet_duplex_half,
 idigi_setting_ethernet_duplex_full,
 idigi_setting_ethernet_duplex_COUNT
} idigi_setting_ethernet_duplex_id_t;

typedef enum {
 idigi_setting_ethernet_ip,
 idigi_setting_ethernet_subnet,
 idigi_setting_ethernet_gateway,
 idigi_setting_ethernet_dhcp,
 idigi_setting_ethernet_dns,
 idigi_setting_ethernet_duplex,
 idigi_setting_ethernet_COUNT
} idigi_setting_ethernet_id_t;

typedef enum {
 idigi_setting_ethernet_error_invalid_duplex = idigi_global_error_COUNT,
 idigi_setting_ethernet_error_invalid_ip,
 idigi_setting_ethernet_error_invalid_subnet,
 idigi_setting_ethernet_error_invalid_gateway,
 idigi_setting_ethernet_error_invalid_dns,
 idigi_setting_ethernet_error_COUNT
} idigi_setting_ethernet_error_id_t;

typedef enum {
 idigi_setting_device_stats_curtime,
 idigi_setting_device_stats_signed_integer,
 idigi_setting_device_stats_COUNT
} idigi_setting_device_stats_id_t;

typedef enum {
 idigi_setting_device_info_product,
 idigi_setting_device_info_model,
 idigi_setting_device_info_company,
 idigi_setting_device_info_desc,
 idigi_setting_device_info_syspwd,
 idigi_setting_device_info_COUNT
} idigi_setting_device_info_id_t;

typedef enum {
 idigi_setting_device_info_error_invalid_length = idigi_global_error_COUNT,
 idigi_setting_device_info_error_COUNT
} idigi_setting_device_info_error_id_t;

typedef enum {
 idigi_setting_system_contact,
 idigi_setting_system_location,
 idigi_setting_system_description,
 idigi_setting_system_COUNT
} idigi_setting_system_id_t;

typedef enum {
 idigi_setting_system_error_invalid_length = idigi_global_error_COUNT,
 idigi_setting_system_error_COUNT
} idigi_setting_system_error_id_t;

typedef enum {
 idigi_setting_devicesecurity_identityVerificationForm_simple,
 idigi_setting_devicesecurity_identityVerificationForm_password,
 idigi_setting_devicesecurity_identityVerificationForm_COUNT
} idigi_setting_devicesecurity_identityVerificationForm_id_t;

typedef enum {
 idigi_setting_devicesecurity_identityVerificationForm,
 idigi_setting_devicesecurity_password,
 idigi_setting_devicesecurity_COUNT
} idigi_setting_devicesecurity_id_t;

typedef enum {
 idigi_setting_serial,
 idigi_setting_ethernet,
 idigi_setting_device_stats,
 idigi_setting_device_info,
 idigi_setting_system,
 idigi_setting_devicesecurity,
 idigi_setting_COUNT
} idigi_setting_id_t;

typedef enum {
 idigi_state_debug_info_version,
 idigi_state_debug_info_stacksize,
 idigi_state_debug_info_COUNT
} idigi_state_debug_info_id_t;

typedef enum {
 idigi_state_gps_stats_latitude,
 idigi_state_gps_stats_longitude,
 idigi_state_gps_stats_COUNT
} idigi_state_gps_stats_id_t;

typedef enum {
 idigi_state_gps_stats_error_invalid_lat = idigi_global_error_COUNT,
 idigi_state_gps_stats_error_invalid_long,
 idigi_state_gps_stats_error_COUNT
} idigi_state_gps_stats_error_id_t;

typedef enum {
 idigi_state_debug_info,
 idigi_state_gps_stats,
 idigi_state_COUNT
} idigi_state_id_t;



#if defined IDIGI_RCI_PARSER_INTERNAL_DATA

#define SETTING_SERIAL (idigi_remote_all_strings+149)
#define SETTING_SERIAL_BAUD (idigi_remote_all_strings+156)
#define SETTING_SERIAL_BAUD_2400 (idigi_remote_all_strings+161)
#define SETTING_SERIAL_BAUD_4800 (idigi_remote_all_strings+166)
#define SETTING_SERIAL_BAUD_9600 (idigi_remote_all_strings+171)
#define SETTING_SERIAL_BAUD_19200 (idigi_remote_all_strings+176)
#define SETTING_SERIAL_BAUD_38400 (idigi_remote_all_strings+182)
#define SETTING_SERIAL_BAUD_57600 (idigi_remote_all_strings+188)
#define SETTING_SERIAL_BAUD_115200 (idigi_remote_all_strings+194)
#define SETTING_SERIAL_BAUD_230400 (idigi_remote_all_strings+201)
#define SETTING_SERIAL_PARITY (idigi_remote_all_strings+208)
#define SETTING_SERIAL_PARITY_NONE (idigi_remote_all_strings+215)
#define SETTING_SERIAL_PARITY_ODD (idigi_remote_all_strings+220)
#define SETTING_SERIAL_PARITY_EVEN (idigi_remote_all_strings+224)
#define SETTING_SERIAL_DATABITS (idigi_remote_all_strings+229)
#define SETTING_SERIAL_XBREAK (idigi_remote_all_strings+238)
#define SETTING_SERIAL_TXBYTES (idigi_remote_all_strings+245)
#define SETTING_SERIAL_ERROR_INVALID_BAUD (idigi_remote_all_strings+253)
#define SETTING_SERIAL_ERROR_INVALID_DATABITS (idigi_remote_all_strings+266)
#define SETTING_SERIAL_ERROR_INVALID_PARITY (idigi_remote_all_strings+283)
#define SETTING_SERIAL_ERROR_INVALID_XBREAK (idigi_remote_all_strings+298)
#define SETTING_SERIAL_ERROR_INVALID_DATABITS_PARITY (idigi_remote_all_strings+313)
#define SETTING_ETHERNET (idigi_remote_all_strings+337)
#define SETTING_ETHERNET_IP (idigi_remote_all_strings+346)
#define SETTING_ETHERNET_SUBNET (idigi_remote_all_strings+349)
#define SETTING_ETHERNET_GATEWAY (idigi_remote_all_strings+356)
#define SETTING_ETHERNET_DHCP (idigi_remote_all_strings+364)
#define SETTING_ETHERNET_DNS (idigi_remote_all_strings+369)
#define SETTING_ETHERNET_DUPLEX (idigi_remote_all_strings+373)
#define SETTING_ETHERNET_DUPLEX_AUTO (idigi_remote_all_strings+380)
#define SETTING_ETHERNET_DUPLEX_HALF (idigi_remote_all_strings+385)
#define SETTING_ETHERNET_DUPLEX_FULL (idigi_remote_all_strings+390)
#define SETTING_ETHERNET_ERROR_INVALID_DUPLEX (idigi_remote_all_strings+395)
#define SETTING_ETHERNET_ERROR_INVALID_IP (idigi_remote_all_strings+410)
#define SETTING_ETHERNET_ERROR_INVALID_SUBNET (idigi_remote_all_strings+421)
#define SETTING_ETHERNET_ERROR_INVALID_GATEWAY (idigi_remote_all_strings+436)
#define SETTING_ETHERNET_ERROR_INVALID_DNS (idigi_remote_all_strings+452)
#define SETTING_DEVICE_STATS (idigi_remote_all_strings+464)
#define SETTING_DEVICE_STATS_CURTIME (idigi_remote_all_strings+477)
#define SETTING_DEVICE_STATS_SIGNED_INTEGER (idigi_remote_all_strings+485)
#define SETTING_DEVICE_INFO (idigi_remote_all_strings+500)
#define SETTING_DEVICE_INFO_PRODUCT (idigi_remote_all_strings+512)
#define SETTING_DEVICE_INFO_MODEL (idigi_remote_all_strings+520)
#define SETTING_DEVICE_INFO_COMPANY (idigi_remote_all_strings+526)
#define SETTING_DEVICE_INFO_DESC (idigi_remote_all_strings+534)
#define SETTING_DEVICE_INFO_SYSPWD (idigi_remote_all_strings+539)
#define SETTING_DEVICE_INFO_ERROR_INVALID_LENGTH (idigi_remote_all_strings+546)
#define SETTING_SYSTEM (idigi_remote_all_strings+561)
#define SETTING_SYSTEM_CONTACT (idigi_remote_all_strings+568)
#define SETTING_SYSTEM_LOCATION (idigi_remote_all_strings+576)
#define SETTING_SYSTEM_DESCRIPTION (idigi_remote_all_strings+585)
#define SETTING_SYSTEM_ERROR_INVALID_LENGTH (idigi_remote_all_strings+597)
#define SETTING_DEVICESECURITY (idigi_remote_all_strings+612)
#define SETTING_DEVICESECURITY_IDENTITYVERIFICATIONFORM (idigi_remote_all_strings+627)
#define SETTING_DEVICESECURITY_IDENTITYVERIFICATIONFORM_SIMPLE (idigi_remote_all_strings+652)
#define SETTING_DEVICESECURITY_IDENTITYVERIFICATIONFORM_PASSWORD (idigi_remote_all_strings+659)
#define SETTING_DEVICESECURITY_PASSWORD (idigi_remote_all_strings+668)
#define STATE_DEBUG_INFO (idigi_remote_all_strings+677)
#define STATE_DEBUG_INFO_VERSION (idigi_remote_all_strings+688)
#define STATE_DEBUG_INFO_STACKSIZE (idigi_remote_all_strings+696)
#define STATE_GPS_STATS (idigi_remote_all_strings+706)
#define STATE_GPS_STATS_LATITUDE (idigi_remote_all_strings+716)
#define STATE_GPS_STATS_LONGITUDE (idigi_remote_all_strings+725)
#define STATE_GPS_STATS_ERROR_INVALID_LAT (idigi_remote_all_strings+735)
#define STATE_GPS_STATS_ERROR_INVALID_LONG (idigi_remote_all_strings+747)
#define IDIGI_GLOBAL_ERROR_LOAD_FAIL (idigi_remote_all_strings+760)
#define IDIGI_GLOBAL_ERROR_SAVE_FAIL (idigi_remote_all_strings+770)
#define IDIGI_GLOBAL_ERROR_MEMORY_FAIL (idigi_remote_all_strings+780)
#define IDIGI_RCI_ERROR_PARSER_ERROR (idigi_remote_all_strings+800)
#define IDIGI_RCI_ERROR_BAD_XML (idigi_remote_all_strings+813)
#define IDIGI_RCI_ERROR_BAD_COMMAND (idigi_remote_all_strings+821)
#define IDIGI_RCI_ERROR_INVALID_VERSION (idigi_remote_all_strings+833)
#define IDIGI_RCI_ERROR_BAD_GROUP (idigi_remote_all_strings+849)
#define IDIGI_RCI_ERROR_BAD_INDEX (idigi_remote_all_strings+859)
#define IDIGI_RCI_ERROR_BAD_ELEMENT (idigi_remote_all_strings+869)
#define IDIGI_RCI_ERROR_BAD_VALUE (idigi_remote_all_strings+881)

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
 7,'v','e','r','s','i','o','n',
 3,'1','.','1',
 9,'r','c','i','_','r','e','p','l','y',
 11,'r','c','i','_','r','e','q','u','e','s','t',
 5,'i','n','d','e','x',
 2,'i','d',
 4,'h','i','n','t',
 5,'e','r','r','o','r',
 3,'!','-','-',
 4,'d','e','s','c',
 2,'o','n',
 3,'o','f','f',
 4,'t','r','u','e',
 5,'f','a','l','s','e',
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
 14,'s','i','g','n','e','d','_','i','n','t','e','g','e','r',
 11,'d','e','v','i','c','e','_','i','n','f','o',
 7,'p','r','o','d','u','c','t',
 5,'m','o','d','e','l',
 7,'c','o','m','p','a','n','y',
 4,'d','e','s','c',
 6,'s','y','s','p','w','d',
 14,'i','n','v','a','l','i','d','_','l','e','n','g','t','h',
 6,'s','y','s','t','e','m',
 7,'c','o','n','t','a','c','t',
 8,'l','o','c','a','t','i','o','n',
 11,'d','e','s','c','r','i','p','t','i','o','n',
 14,'i','n','v','a','l','i','d','_','l','e','n','g','t','h',
 14,'d','e','v','i','c','e','s','e','c','u','r','i','t','y',
 24,'i','d','e','n','t','i','t','y','V','e','r','i','f','i','c','a','t','i','o','n','F','o','r','m',
 6,'s','i','m','p','l','e',
 8,'p','a','s','s','w','o','r','d',
 8,'p','a','s','s','w','o','r','d',
 10,'d','e','b','u','g','_','i','n','f','o',
 7,'v','e','r','s','i','o','n',
 9,'s','t','a','c','k','s','i','z','e',
 9,'g','p','s','_','s','t','a','t','s',
 8,'l','a','t','i','t','u','d','e',
 9,'l','o','n','g','i','t','u','d','e',
 11,'i','n','v','a','l','i','d','_','l','a','t',
 12,'i','n','v','a','l','i','d','_','l','o','n','g',
 9,'l','o','a','d','_','f','a','i','l',
 9,'s','a','v','e','_','f','a','i','l',
 11,'m','e','m','o','r','y','_','f','a','i','l',
 12,'p','a','r','s','e','r','_','e','r','r','o','r',
 7,'b','a','d','_','x','m','l',
 11,'b','a','d','_','c','o','m','m','a','n','d',
 15,'i','n','v','a','l','i','d','_','v','e','r','s','i','o','n',
 9,'b','a','d','_','g','r','o','u','p',
 9,'b','a','d','_','i','n','d','e','x',
 11,'b','a','d','_','e','l','e','m','e','n','t',
 9,'b','a','d','_','v','a','l','u','e'
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
 { SETTING_SERIAL_BAUD,  /*baud*/
   idigi_element_access_read_write,
   idigi_element_type_enum,
  (idigi_element_value_limit_t *)&setting_serial_baud_limit
 },
 { SETTING_SERIAL_PARITY,  /*parity*/
   idigi_element_access_read_write,
   idigi_element_type_enum,
  (idigi_element_value_limit_t *)&setting_serial_parity_limit
 },
 { SETTING_SERIAL_DATABITS,  /*databits*/
   idigi_element_access_read_write,
   idigi_element_type_uint32,
  (idigi_element_value_limit_t *)&setting_serial_databits_limit
 },
 { SETTING_SERIAL_XBREAK,  /*xbreak*/
   idigi_element_access_read_write,
   idigi_element_type_on_off,
  NULL
 },
 { SETTING_SERIAL_TXBYTES,  /*txbytes*/
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
 { SETTING_ETHERNET_IP,  /*ip*/
   idigi_element_access_read_write,
   idigi_element_type_ipv4,
  NULL
 },
 { SETTING_ETHERNET_SUBNET,  /*subnet*/
   idigi_element_access_read_write,
   idigi_element_type_ipv4,
  NULL
 },
 { SETTING_ETHERNET_GATEWAY,  /*gateway*/
   idigi_element_access_read_write,
   idigi_element_type_ipv4,
  NULL
 },
 { SETTING_ETHERNET_DHCP,  /*dhcp*/
   idigi_element_access_read_write,
   idigi_element_type_boolean,
  NULL
 },
 { SETTING_ETHERNET_DNS,  /*dns*/
   idigi_element_access_read_write,
   idigi_element_type_fqdnv4,
  NULL
 },
 { SETTING_ETHERNET_DUPLEX,  /*duplex*/
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

static idigi_element_value_signed_integer_t const setting_device_stats_signed_integer_limit = {
 -100,
 100
};

static idigi_group_element_t const setting_device_stats_elements[] = {
 { SETTING_DEVICE_STATS_CURTIME,  /*curtime*/
   idigi_element_access_read_write,
   idigi_element_type_datetime,
  NULL
 },
 { SETTING_DEVICE_STATS_SIGNED_INTEGER,  /*signed_integer*/
   idigi_element_access_read_write,
   idigi_element_type_int32,
  (idigi_element_value_limit_t *)&setting_device_stats_signed_integer_limit
 }
};

static idigi_element_value_string_t const setting_device_info_product_limit = {
 1,
 64
};

static idigi_element_value_string_t const setting_device_info_model_limit = {
 0,
 32
};

static idigi_element_value_string_t const setting_device_info_syspwd_limit = {
 0,
 64
};

static idigi_group_element_t const setting_device_info_elements[] = {
 { SETTING_DEVICE_INFO_PRODUCT,  /*product*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_device_info_product_limit
 },
 { SETTING_DEVICE_INFO_MODEL,  /*model*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_device_info_model_limit
 },
 { SETTING_DEVICE_INFO_COMPANY,  /*company*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  NULL
 },
 { SETTING_DEVICE_INFO_DESC,  /*desc*/
   idigi_element_access_read_write,
   idigi_element_type_multiline_string,
  NULL
 },
 { SETTING_DEVICE_INFO_SYSPWD,  /*syspwd*/
   idigi_element_access_read_write,
   idigi_element_type_password,
  (idigi_element_value_limit_t *)&setting_device_info_syspwd_limit
 }
};

static char const * const setting_device_info_errors[] = {
 SETTING_DEVICE_INFO_ERROR_INVALID_LENGTH /*invalid_length*/
};

static idigi_element_value_string_t const setting_system_contact_limit = {
 0,
 63
};

static idigi_element_value_string_t const setting_system_location_limit = {
 0,
 63
};

static idigi_element_value_string_t const setting_system_description_limit = {
 0,
 63
};

static idigi_group_element_t const setting_system_elements[] = {
 { SETTING_SYSTEM_CONTACT,  /*contact*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_system_contact_limit
 },
 { SETTING_SYSTEM_LOCATION,  /*location*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_system_location_limit
 },
 { SETTING_SYSTEM_DESCRIPTION,  /*description*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_system_description_limit
 }
};

static char const * const setting_system_errors[] = {
 SETTING_SYSTEM_ERROR_INVALID_LENGTH /*invalid_length*/
};

static char const * const setting_devicesecurity_identityverificationform_enum[] = {
 SETTING_DEVICESECURITY_IDENTITYVERIFICATIONFORM_SIMPLE, /*simple*/
 SETTING_DEVICESECURITY_IDENTITYVERIFICATIONFORM_PASSWORD /*password*/
};

static idigi_element_value_enum_t const setting_devicesecurity_identityverificationform_limit = {
 asizeof(setting_devicesecurity_identityverificationform_enum),
 setting_devicesecurity_identityverificationform_enum
};

static idigi_element_value_string_t const setting_devicesecurity_password_limit = {
 0,
 134
};

static idigi_group_element_t const setting_devicesecurity_elements[] = {
 { SETTING_DEVICESECURITY_IDENTITYVERIFICATIONFORM,  /*identityVerificationForm*/
   idigi_element_access_read_write,
   idigi_element_type_enum,
  (idigi_element_value_limit_t *)&setting_devicesecurity_identityverificationform_limit
 },
 { SETTING_DEVICESECURITY_PASSWORD,  /*password*/
   idigi_element_access_read_write,
   idigi_element_type_password,
  (idigi_element_value_limit_t *)&setting_devicesecurity_password_limit
 }
};

static idigi_group_t const idigi_setting_groups[] = {
 { SETTING_SERIAL,  /*serial*/
   2,
   { asizeof(setting_serial_elements),
     setting_serial_elements
   },
   { asizeof(setting_serial_errors),
     setting_serial_errors
   }
 }
,
 { SETTING_ETHERNET,  /*ethernet*/
   1,
   { asizeof(setting_ethernet_elements),
     setting_ethernet_elements
   },
   { asizeof(setting_ethernet_errors),
     setting_ethernet_errors
   }
 }
,
 { SETTING_DEVICE_STATS,  /*device_stats*/
   1,
   { asizeof(setting_device_stats_elements),
     setting_device_stats_elements
   },
   { 0,
     NULL
   }
 },
 { SETTING_DEVICE_INFO,  /*device_info*/
   1,
   { asizeof(setting_device_info_elements),
     setting_device_info_elements
   },
   { asizeof(setting_device_info_errors),
     setting_device_info_errors
   }
 }
,
 { SETTING_SYSTEM,  /*system*/
   1,
   { asizeof(setting_system_elements),
     setting_system_elements
   },
   { asizeof(setting_system_errors),
     setting_system_errors
   }
 }
,
 { SETTING_DEVICESECURITY,  /*devicesecurity*/
   1,
   { asizeof(setting_devicesecurity_elements),
     setting_devicesecurity_elements
   },
   { 0,
     NULL
   }
 }
};

static idigi_group_element_t const state_debug_info_elements[] = {
 { STATE_DEBUG_INFO_VERSION,  /*version*/
   idigi_element_access_read_only,
   idigi_element_type_0xhex,
  NULL
 },
 { STATE_DEBUG_INFO_STACKSIZE,  /*stacksize*/
   idigi_element_access_read_only,
   idigi_element_type_hex32,
  NULL
 }
};

static idigi_group_element_t const state_gps_stats_elements[] = {
 { STATE_GPS_STATS_LATITUDE,  /*latitude*/
   idigi_element_access_read_only,
   idigi_element_type_float,
  NULL
 },
 { STATE_GPS_STATS_LONGITUDE,  /*longitude*/
   idigi_element_access_read_only,
   idigi_element_type_float,
  NULL
 }
};

static char const * const state_gps_stats_errors[] = {
 STATE_GPS_STATS_ERROR_INVALID_LAT, /*invalid_lat*/
 STATE_GPS_STATS_ERROR_INVALID_LONG /*invalid_long*/
};

static idigi_group_t const idigi_state_groups[] = {
 { STATE_DEBUG_INFO,  /*debug_info*/
   1,
   { asizeof(state_debug_info_elements),
     state_debug_info_elements
   },
   { 0,
     NULL
   }
 },
 { STATE_GPS_STATS,  /*gps_stats*/
   1,
   { asizeof(state_gps_stats_elements),
     state_gps_stats_elements
   },
   { asizeof(state_gps_stats_errors),
     state_gps_stats_errors
   }
 }

};

static char const * const idigi_rci_errors[] = {
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

static idigi_group_table_t const idigi_group_table[] = {
 {idigi_setting_groups,
 asizeof(idigi_setting_groups)
 },
 {idigi_state_groups,
 asizeof(idigi_state_groups)
 }
};


#endif /* IDIGI_RCI_PARSER_INTERNAL_DATA */


#endif /* remote_config_h */
