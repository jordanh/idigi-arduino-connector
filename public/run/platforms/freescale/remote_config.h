/*
 * This is an auto-generated file - DO NOT EDIT! 
 * This is generated by ConfigGenerator tool 
 * This file was generated on: 2012/06/13 16:27:10 
 * The command line arguments were: "-server=test.idigi.com mlchan:***** "Linux Application" 1.0.0.0 config.rci"
 * The version of ConfigGenerator tool was: 1.0.0.0 */

#ifndef remote_config_h
#define remote_config_h

#define RCI_PARSER_USES_ERROR_DESCRIPTIONS
#define RCI_PARSER_USES_STRINGS

typedef struct {
    size_t min_length_in_bytes;
    size_t max_length_in_bytes;
} idigi_element_value_string_t;


typedef struct {
    char const * string_value;
} idigi_element_value_t;


 typedef struct {    idigi_element_value_string_t string_value;
} idigi_element_value_limit_t;


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
#define RCI_COMMENT (idigi_remote_all_strings+122)
#define RCI_DESC (idigi_remote_all_strings+126)

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
 idigi_setting_system_description,
 idigi_setting_system_contact,
 idigi_setting_system_location,
 idigi_setting_system_COUNT
} idigi_setting_system_id_t;

typedef enum {
 idigi_setting_system_error_invalid_length = idigi_global_error_COUNT,
 idigi_setting_system_error_no_memory,
 idigi_setting_system_error_COUNT
} idigi_setting_system_error_id_t;

typedef enum {
 idigi_setting_system,
 idigi_setting_COUNT
} idigi_setting_id_t;



#if defined IDIGI_RCI_PARSER_INTERNAL_DATA

#define SETTING_SYSTEM (idigi_remote_all_strings+131)
#define SETTING_SYSTEM_DESCRIPTION (idigi_remote_all_strings+138)
#define SETTING_SYSTEM_CONTACT (idigi_remote_all_strings+150)
#define SETTING_SYSTEM_LOCATION (idigi_remote_all_strings+158)
#define SETTING_SYSTEM_ERROR_INVALID_LENGTH (idigi_remote_all_strings+167)
#define SETTING_SYSTEM_ERROR_NO_MEMORY (idigi_remote_all_strings+182)
#define IDIGI_RCI_ERROR_PARSER_ERROR (idigi_remote_all_strings+202)
#define IDIGI_RCI_ERROR_BAD_XML (idigi_remote_all_strings+215)
#define IDIGI_RCI_ERROR_BAD_COMMAND (idigi_remote_all_strings+223)
#define IDIGI_RCI_ERROR_INVALID_VERSION (idigi_remote_all_strings+235)
#define IDIGI_RCI_ERROR_BAD_GROUP (idigi_remote_all_strings+251)
#define IDIGI_RCI_ERROR_BAD_INDEX (idigi_remote_all_strings+261)
#define IDIGI_RCI_ERROR_BAD_ELEMENT (idigi_remote_all_strings+271)
#define IDIGI_RCI_ERROR_BAD_VALUE (idigi_remote_all_strings+283)

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
 6,'s','y','s','t','e','m',
 11,'d','e','s','c','r','i','p','t','i','o','n',
 7,'c','o','n','t','a','c','t',
 8,'l','o','c','a','t','i','o','n',
 14,'I','n','v','a','l','i','d',' ','L','e','n','g','t','h',
 19,'I','n','s','u','f','f','i','c','i','e','n','t',' ','m','e','m','o','r','y',
 12,'P','a','r','s','e','r',' ','e','r','r','o','r',
 7,'B','a','d',' ','X','M','L',
 11,'B','a','d',' ','c','o','m','m','a','n','d',
 15,'I','n','v','a','l','i','d',' ','v','e','r','s','i','o','n',
 9,'B','a','d',' ','g','r','o','u','p',
 9,'B','a','d',' ','i','n','d','e','x',
 11,'B','a','d',' ','e','l','e','m','e','n','t',
 9,'B','a','d',' ','v','a','l','u','e'
};

static idigi_element_value_string_t const setting_system_description_limit = {
 0,
 63
};

static idigi_element_value_string_t const setting_system_contact_limit = {
 0,
 63
};

static idigi_element_value_string_t const setting_system_location_limit = {
 0,
 63
};

static idigi_group_element_t const setting_system_elements[] = {
 { SETTING_SYSTEM_DESCRIPTION,  /*description*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_system_description_limit
 },
 { SETTING_SYSTEM_CONTACT,  /*contact*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_system_contact_limit
 },
 { SETTING_SYSTEM_LOCATION,  /*location*/
   idigi_element_access_read_write,
   idigi_element_type_string,
  (idigi_element_value_limit_t *)&setting_system_location_limit
 }
};

static char const * const setting_system_errors[] = {
 SETTING_SYSTEM_ERROR_INVALID_LENGTH, /*invalid_length*/
 SETTING_SYSTEM_ERROR_NO_MEMORY /*no_memory*/
};

static idigi_group_t const idigi_setting_groups[] = {
 { SETTING_SYSTEM,  /*system*/
   1,
   { asizeof(setting_system_elements),
     setting_system_elements
   },
   { asizeof(setting_system_errors),
     setting_system_errors
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
 IDIGI_RCI_ERROR_BAD_VALUE /*bad_value*/
};

static idigi_group_table_t const idigi_group_table[] = {
 {idigi_setting_groups,
 asizeof(idigi_setting_groups)
 },
 {NULL,
 0
 }
};


#endif /* IDIGI_RCI_PARSER_INTERNAL_DATA */


#endif /* remote_config_h */
