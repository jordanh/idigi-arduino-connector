/*
 * This is an auto-generated file - DO NOT EDIT! 
 * This is generated by ConfigGenerator tool 
 * This file was generated on: 2012/04/24 09:47:20 
 * The command line arguments were: "-s=test.idigi.com mlchan:***** "Linux Application" 1 config.rci" 
 * The version of ConfigGenerator tool was: 1.0.0.0
*/

#ifndef REMOTE_CONFIG_H
#define REMOTE_CONFIG_H

#include "limits.h"
#include "idigi_remote.h"

#define RCI_PARSER_USES_ERROR_DESCRIPTIONS
#define RCI_PARSER_USES_ENUMERATIONS
#define RCI_PARSER_USES_FLOATING_POINT

extern char const idigi_remote_all_strings[];

#define RCI_ENTITY_QUOTE    (idigi_remote_all_strings + 0)
#define RCI_ENTITY_AMPERSAND    (idigi_remote_all_strings + 5)
#define RCI_ENTITY_APOSTROPHE    (idigi_remote_all_strings + 9)
#define RCI_ENTITY_LESS_THAN    (idigi_remote_all_strings + 14)
#define RCI_ENTITY_GREATER_THAN    (idigi_remote_all_strings + 17)
#define RCI_SET_SETTING    (idigi_remote_all_strings + 20)
#define RCI_QUERY_SETTING    (idigi_remote_all_strings + 32)
#define RCI_SET_STATE    (idigi_remote_all_strings + 46)
#define RCI_QUERY_STATE    (idigi_remote_all_strings + 56)
#define RCI_VERSION    (idigi_remote_all_strings + 68)
#define RCI_VERSION_SUPPORTED    (idigi_remote_all_strings + 76)
#define RCI_REPLY    (idigi_remote_all_strings + 80)
#define RCI_REQUEST    (idigi_remote_all_strings + 90)
#define RCI_INDEX    (idigi_remote_all_strings + 102)
#define RCI_ID    (idigi_remote_all_strings + 108)
#define RCI_HINT    (idigi_remote_all_strings + 111)
#define RCI_ERROR    (idigi_remote_all_strings + 116)
#define RCI_ON    (idigi_remote_all_strings + 122)
#define RCI_OFF    (idigi_remote_all_strings + 125)
#define RCI_TRUE    (idigi_remote_all_strings + 129)
#define RCI_FALSE    (idigi_remote_all_strings + 134)

typedef enum {
    idigi_rci_error_OFFSET = 1,
    idigi_rci_error_parser_error =     idigi_rci_error_OFFSET,
    idigi_rci_error_bad_xml,
    idigi_rci_error_bad_command,
    idigi_rci_error_invalid_version,
    idigi_rci_error_bad_group,
    idigi_rci_error_bad_index,
    idigi_rci_error_bad_element,
    idigi_rci_error_bad_value,
    idigi_global_error_load_fail,
    idigi_global_error_save_fail,
    idigi_global_error_memory_fail,
    idigi_global_error_COUNT,
    idigi_global_error_WIDTH = INT_MAX
}idigi_global_error_id_t;

typedef enum {
    idigi_setting_serial_baud_2400,
    idigi_setting_serial_baud_4800,
    idigi_setting_serial_baud_9600,
    idigi_setting_serial_baud_19200,
    idigi_setting_serial_baud_38400,
    idigi_setting_serial_baud_57600,
    idigi_setting_serial_baud_115200,
    idigi_setting_serial_baud_230400,
    idigi_setting_serial_baud_WIDTH = INT_MAX
}    idigi_setting_serial_baud_id_t;

typedef enum {
    idigi_setting_serial_parity_none,
    idigi_setting_serial_parity_odd,
    idigi_setting_serial_parity_even,
    idigi_setting_serial_parity_WIDTH = INT_MAX
}    idigi_setting_serial_parity_id_t;

typedef enum {
    idigi_setting_serial_baud,
    idigi_setting_serial_parity,
    idigi_setting_serial_databits,
    idigi_setting_serial_xbreak,
    idigi_setting_serial_txbytes,
    idigi_setting_serial_WIDTH = INT_MAX
}    idigi_setting_serial_id_t;

typedef enum {
    idigi_setting_serial_error_invalid_baud = idigi_global_error_COUNT,
    idigi_setting_serial_error_invalid_databits,
    idigi_setting_serial_error_invalid_parity,
    idigi_setting_serial_error_invalid_xbreak,
    idigi_setting_serial_error_invalid_databits_parity,
    idigi_setting_serial_error_WIDTH = INT_MAX
}    idigi_setting_serial_error_id_t;

typedef enum {
    idigi_setting_ethernet_duplex_auto,
    idigi_setting_ethernet_duplex_half,
    idigi_setting_ethernet_duplex_full,
    idigi_setting_ethernet_duplex_WIDTH = INT_MAX
}    idigi_setting_ethernet_duplex_id_t;

typedef enum {
    idigi_setting_ethernet_ip,
    idigi_setting_ethernet_subnet,
    idigi_setting_ethernet_gateway,
    idigi_setting_ethernet_dhcp,
    idigi_setting_ethernet_dns,
    idigi_setting_ethernet_duplex,
    idigi_setting_ethernet_WIDTH = INT_MAX
}    idigi_setting_ethernet_id_t;

typedef enum {
    idigi_setting_ethernet_error_invalid_duplex = idigi_global_error_COUNT,
    idigi_setting_ethernet_error_invalid_ip,
    idigi_setting_ethernet_error_invalid_subnet,
    idigi_setting_ethernet_error_invalid_gateway,
    idigi_setting_ethernet_error_invalid_dns,
    idigi_setting_ethernet_error_WIDTH = INT_MAX
}    idigi_setting_ethernet_error_id_t;

typedef enum {
    idigi_setting_device_stats_curtime,
    idigi_setting_device_stats_ctemp,
    idigi_setting_device_stats_ftemp,
    idigi_setting_device_stats_WIDTH = INT_MAX
}    idigi_setting_device_stats_id_t;

typedef enum {
    idigi_setting_device_info_product,
    idigi_setting_device_info_model,
    idigi_setting_device_info_company,
    idigi_setting_device_info_desc,
    idigi_setting_device_info_syspwd,
    idigi_setting_device_info_WIDTH = INT_MAX
}    idigi_setting_device_info_id_t;

typedef enum {
    idigi_setting_device_info_error_invalid_length = idigi_global_error_COUNT,
    idigi_setting_device_info_error_WIDTH = INT_MAX
}    idigi_setting_device_info_error_id_t;

typedef enum {
    idigi_setting_serial,
    idigi_setting_ethernet,
    idigi_setting_device_stats,
    idigi_setting_device_info,
    idigi_setting_WIDTH = INT_MAX
}    idigi_setting_id_t;

typedef enum {
    idigi_state_debug_info_version,
    idigi_state_debug_info_stacktop,
    idigi_state_debug_info_stacksize,
    idigi_state_debug_info_stackbottom,
    idigi_state_debug_info_usedmem,
    idigi_state_debug_info_WIDTH = INT_MAX
}    idigi_state_debug_info_id_t;

typedef enum {
    idigi_state_debug_info,
    idigi_state_WIDTH = INT_MAX
}    idigi_state_id_t;


#endif /* REMOTE_CONFIG_H */
