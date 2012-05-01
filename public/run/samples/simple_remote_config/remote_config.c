/*
 * This is an auto-generated file - DO NOT EDIT! 
 * This is generated by ConfigGenerator tool 
 * This file was generated on: 2012/04/30 17:11:00 
 * The command line arguments were: "-server=test.idigi.com mlchan:***** "Simple Remote Config Linux Application" 1 config.rci"
 * The version of ConfigGenerator tool was: 1.0.0.0 */

#include "remote_config.h"

#define SETTING_KEEPALIVE (idigi_remote_all_strings+140)
#define SETTING_KEEPALIVE_RX (idigi_remote_all_strings+150)
#define SETTING_KEEPALIVE_TX (idigi_remote_all_strings+153)
#define SETTING_KEEPALIVE_ERROR_LOAD_FAIL (idigi_remote_all_strings+156)
#define SETTING_KEEPALIVE_ERROR_SAVE_FAIL (idigi_remote_all_strings+166)
#define SETTING_KEEPALIVE_ERROR_NO_MEMORY (idigi_remote_all_strings+176)
#define IDIGI_RCI_ERROR_PARSER_ERROR (idigi_remote_all_strings+186)
#define IDIGI_RCI_ERROR_BAD_XML (idigi_remote_all_strings+199)
#define IDIGI_RCI_ERROR_BAD_COMMAND (idigi_remote_all_strings+207)
#define IDIGI_RCI_ERROR_INVALID_VERSION (idigi_remote_all_strings+219)
#define IDIGI_RCI_ERROR_BAD_GROUP (idigi_remote_all_strings+235)
#define IDIGI_RCI_ERROR_BAD_INDEX (idigi_remote_all_strings+245)
#define IDIGI_RCI_ERROR_BAD_ELEMENT (idigi_remote_all_strings+255)
#define IDIGI_RCI_ERROR_BAD_VALUE (idigi_remote_all_strings+267)

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
 2,'o','n',
 3,'o','f','f',
 4,'t','r','u','e',
 5,'f','a','l','s','e',
 9,'k','e','e','p','a','l','i','v','e',
 2,'r','x',
 2,'t','x',
 9,'l','o','a','d','_','f','a','i','l',
 9,'s','a','v','e','_','f','a','i','l',
 9,'n','o','_','m','e','m','o','r','y',
 12,'p','a','r','s','e','r','_','e','r','r','o','r',
 7,'b','a','d','_','x','m','l',
 11,'b','a','d','_','c','o','m','m','a','n','d',
 15,'i','n','v','a','l','i','d','_','v','e','r','s','i','o','n',
 9,'b','a','d','_','g','r','o','u','p',
 9,'b','a','d','_','i','n','d','e','x',
 11,'b','a','d','_','e','l','e','m','e','n','t',
 9,'b','a','d','_','v','a','l','u','e',
 '\0'
};

static idigi_element_value_unsigned_integer_t const setting_keepalive_rx_limit = {
 5,
 7200
};

static idigi_element_value_unsigned_integer_t const setting_keepalive_tx_limit = {
 5,
 7200
};

static idigi_group_element_t const setting_keepalive_elements[] = {
 { SETTING_KEEPALIVE_RX,  /*rx*/
   idigi_element_access_read_write,
   idigi_element_type_uint32,
  (idigi_element_value_limit_t *)&setting_keepalive_rx_limit
 },
 { SETTING_KEEPALIVE_TX,  /*tx*/
   idigi_element_access_read_write,
   idigi_element_type_uint32,
  (idigi_element_value_limit_t *)&setting_keepalive_tx_limit
 }
};

static char const * const setting_keepalive_errors[] = {
 SETTING_KEEPALIVE_ERROR_LOAD_FAIL, /*load_fail*/
 SETTING_KEEPALIVE_ERROR_SAVE_FAIL, /*save_fail*/
 SETTING_KEEPALIVE_ERROR_NO_MEMORY /*no_memory*/
};

static idigi_group_t const idigi_setting_groups[] = {
 { SETTING_KEEPALIVE,  /*keepalive*/
   1,
   { asizeof(setting_keepalive_elements),
     setting_keepalive_elements
   },
   { asizeof(setting_keepalive_errors),
     setting_keepalive_errors
   }
 }

};

char const * const idigi_rci_errors[8] = {
 IDIGI_RCI_ERROR_PARSER_ERROR, /*parser_error*/
 IDIGI_RCI_ERROR_BAD_XML, /*bad_xml*/
 IDIGI_RCI_ERROR_BAD_COMMAND, /*bad_command*/
 IDIGI_RCI_ERROR_INVALID_VERSION, /*invalid_version*/
 IDIGI_RCI_ERROR_BAD_GROUP, /*bad_group*/
 IDIGI_RCI_ERROR_BAD_INDEX, /*bad_index*/
 IDIGI_RCI_ERROR_BAD_ELEMENT, /*bad_element*/
 IDIGI_RCI_ERROR_BAD_VALUE /*bad_value*/
};

idigi_group_table_t const idigi_group_table[2] = {
 {idigi_setting_groups,
 asizeof(idigi_setting_groups)
 },
 {NULL,
 0
 }
};

