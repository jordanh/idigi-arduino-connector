/*
 * This is an auto-generated file - DO NOT EDIT! 
 * This is generated by ConfigGenerator tool 
 * This file was generated on: 2012/05/01 16:36:41 
 * The command line arguments were: "-server=test.idigi.com mlchan:***** "Simple Remote Config Linux Application" 1 config.rci"
 * The version of ConfigGenerator tool was: 1.0.0.0 */

#ifndef remote_config_h
#define remote_config_h

#define RCI_PARSER_USES_ERROR_DESCRIPTIONS
#define RCI_PARSER_USES_UNSIGNED_INTEGER

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
 idigi_global_error_COUNT = idigi_global_error_OFFSET
} idigi_global_error_id_t;

typedef enum {
 idigi_setting_keepalive_rx,
 idigi_setting_keepalive_tx,
 idigi_setting_keepalive_COUNT
} idigi_setting_keepalive_id_t;

typedef enum {
 idigi_setting_keepalive_error_load_fail = idigi_global_error_COUNT,
 idigi_setting_keepalive_error_save_fail,
 idigi_setting_keepalive_error_no_memory,
 idigi_setting_keepalive_error_COUNT
} idigi_setting_keepalive_error_id_t;

typedef enum {
 idigi_setting_keepalive,
 idigi_setting_COUNT
} idigi_setting_id_t;


#endif /* remote_config_h */
