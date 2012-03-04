/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 *
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 * =======================================================================
 *
 */
#include "idigi_remote.h"
#include "remote_config.h"

#define asizeof(array) (sizeof(array)/sizeof(array[0]))

#define SERIAL_LENGTH           6
#define SERIAL_BAUD_LENGTH      4
#define SERIAL_PARITY_LENGTH    6
#define SERIAL_DATABITS_LENGTH  8
#define SERIAL_XBREAK_LENGTH    6
#define SERIAL_TXBYTES_LENGTH   7

#define SERIAL_BAUD_2400_LENGTH 4
#define SERIAL_BAUD_4800_LENGTH 4
#define SERIAL_BAUD_9600_LENGTH 4
#define SERIAL_BAUD_19200_LENGTH 5
#define SERIAL_BAUD_38400_LENGTH 5
#define SERIAL_BAUD_57600_LENGTH 5
#define SERIAL_BAUD_115200_LENGTH 6
#define SERIAL_BAUD_230400_LENGTH 6

#define SERIAL_PARITY_NONE_LENGTH   4
#define SERIAL_PARITY_ODD_LENGTH    3
#define SERIAL_PARITY_EVEN_LENGTH   4

#define ERROR_FIELD_NOT_EXIT_LENGTH 30
#define ERROR_LOAD_FAILED_LENGTH    11
#define ERROR_SAVE_FAILED_LENGTH    11

#define SERIAL_ERROR_BAUD_LENGTH        17
#define SERIAL_ERROR_DATABITS_LENGTH    17
#define SERIAL_ERROR_PARITY_LENGTH      14
#define SERIAL_ERROR_XBREAK_LENGTH      22
#define SERIAL_ERROR_DATABITS_PARITY_LENGTH    43

#define SERIAL_BAUD_STRING_INDEX        SERIAL_LENGTH + 1
#define SERIAL_PARITY_STRING_INDEX      SERIAL_BAUD_STRING_INDEX + SERIAL_BAUD_LENGTH + 1
#define SERIAL_DATABITS_STRING_INDEX    SERIAL_PARITY_STRING_INDEX + SERIAL_PARITY_LENGTH + 1
#define SERIAL_XBREAK_STRING_INDEX      SERIAL_DATABITS_STRING_INDEX + SERIAL_DATABITS_LENGTH + 1
#define SERIAL_TXBYTES_STRING_INDEX     SERIAL_XBREAK_STRING_INDEX + SERIAL_XBREAK_LENGTH + 1

#define SERIAL_BAUD_2400_STRING_INDEX   SERIAL_TXBYTES_STRING_INDEX + SERIAL_TXBYTES_LENGTH + 1
#define SERIAL_BAUD_4800_STRING_INDEX   SERIAL_BAUD_2400_STRING_INDEX + SERIAL_BAUD_2400_LENGTH + 1
#define SERIAL_BAUD_9600_STRING_INDEX   SERIAL_BAUD_4800_STRING_INDEX + SERIAL_BAUD_4800_LENGTH + 1
#define SERIAL_BAUD_19200_STRING_INDEX  SERIAL_BAUD_9600_STRING_INDEX + SERIAL_BAUD_9600_LENGTH + 1
#define SERIAL_BAUD_38400_STRING_INDEX  SERIAL_BAUD_19200_STRING_INDEX + SERIAL_BAUD_19200_LENGTH + 1
#define SERIAL_BAUD_57600_STRING_INDEX  SERIAL_BAUD_38400_STRING_INDEX + SERIAL_BAUD_38400_LENGTH + 1
#define SERIAL_BAUD_115200_STRING_INDEX SERIAL_BAUD_57600_STRING_INDEX + SERIAL_BAUD_57600_LENGTH + 1
#define SERIAL_BAUD_230400_STRING_INDEX SERIAL_BAUD_115200_STRING_INDEX + SERIAL_BAUD_115200_LENGTH + 1

#define SERIAL_PARITY_NONE_STRING_INDEX SERIAL_BAUD_230400_STRING_INDEX + SERIAL_BAUD_230400_LENGTH + 1
#define SERIAL_PARITY_ODD_STRING_INDEX  SERIAL_PARITY_NONE_STRING_INDEX + SERIAL_PARITY_NONE_LENGTH + 1
#define SERIAL_PARITY_EVEN_STRING_INDEX SERIAL_PARITY_ODD_STRING_INDEX + SERIAL_PARITY_ODD_LENGTH + 1

#define ERROR_FIELD_NOT_EXIT_STRING_INDEX   SERIAL_PARITY_EVEN_STRING_INDEX + SERIAL_PARITY_EVEN_LENGTH + 1
#define ERROR_LOAD_FAILED_STRING_INDEX      ERROR_FIELD_NOT_EXIT_STRING_INDEX + ERROR_FIELD_NOT_EXIT_LENGTH + 1
#define ERROR_SAVE_FAILED_STRING_INDEX      ERROR_LOAD_FAILED_STRING_INDEX + ERROR_LOAD_FAILED_LENGTH + 1

#define SERIAL_ERROR_BAUD_STRING_INDEX              ERROR_SAVE_FAILED_STRING_INDEX + ERROR_SAVE_FAILED_LENGTH + 1
#define SERIAL_ERROR_DATABITS_STRING_INDEX          SERIAL_ERROR_BAUD_STRING_INDEX + SERIAL_ERROR_BAUD_LENGTH + 1
#define SERIAL_ERROR_PARITY_STRING_INDEX            SERIAL_ERROR_DATABITS_STRING_INDEX + SERIAL_ERROR_DATABITS_LENGTH + 1
#define SERIAL_ERROR_XBREAK_STRING_INDEX            SERIAL_ERROR_PARITY_STRING_INDEX + SERIAL_ERROR_PARITY_LENGTH + 1
#define SERIAL_ERROR_DATABITS_PARITY_STRING_INDEX   SERIAL_ERROR_XBREAK_STRING_INDEX + SERIAL_ERROR_XBREAK_LENGTH + 1

#define ETHERNET_LENGTH         8
#define ETHERNET_IP_LENGTH      2
#define ETHERNET_SUBNET_LENGTH  6
#define ETHERNET_GATEWAY_LENGTH 7
#define ETHERNET_DHCP_LENGTH    4
#define ETHERNET_DNS_LENGTH     3
#define ETHERNET_DUPLEX_LENGTH  6

#define ETHERNET_DUPLEX_AUTO_LENGTH  4
#define ETHERNET_DUPLEX_HALF_LENGTH  4
#define ETHERNET_DUPLEX_FULL_LENGTH  4

#define ETHERNET_ERROR_DUPLEX_LENGTH    31
#define ETHERNET_ERROR_IP_LENGTH        18
#define ETHERNET_ERROR_SUBNET_LENGTH    19
#define ETHERNET_ERROR_GATEWAY_LENGTH   23
#define ETHERNET_ERROR_DNS_LENGTH       19


#define ETHERNET_STRING_INDEX         SERIAL_ERROR_DATABITS_PARITY_STRING_INDEX + SERIAL_ERROR_DATABITS_PARITY_LENGTH + 1
#define ETHERNET_IP_STRING_INDEX      ETHERNET_STRING_INDEX + ETHERNET_LENGTH + 1
#define ETHERNET_SUBNET_STRING_INDEX  ETHERNET_IP_STRING_INDEX + ETHERNET_IP_LENGTH + 1
#define ETHERNET_GATEWAY_STRING_INDEX ETHERNET_SUBNET_STRING_INDEX + ETHERNET_SUBNET_LENGTH + 1
#define ETHERNET_DHCP_STRING_INDEX    ETHERNET_GATEWAY_STRING_INDEX + ETHERNET_GATEWAY_LENGTH + 1
#define ETHERNET_DNS_STRING_INDEX     ETHERNET_DHCP_STRING_INDEX + ETHERNET_DHCP_LENGTH + 1
#define ETHERNET_DUPLEX_STRING_INDEX  ETHERNET_DNS_STRING_INDEX + ETHERNET_DNS_LENGTH + 1

#define ETHERNET_DUPLEX_AUTO_STRING_INDEX  ETHERNET_DUPLEX_STRING_INDEX + ETHERNET_DUPLEX_LENGTH + 1
#define ETHERNET_DUPLEX_HALF_STRING_INDEX  ETHERNET_DUPLEX_AUTO_STRING_INDEX + ETHERNET_DUPLEX_AUTO_LENGTH + 1
#define ETHERNET_DUPLEX_FULL_STRING_INDEX  ETHERNET_DUPLEX_HALF_STRING_INDEX + ETHERNET_DUPLEX_HALF_LENGTH + 1

#define ETHERNET_ERROR_DUPLEX_STRING_INDEX    ETHERNET_DUPLEX_FULL_STRING_INDEX + ETHERNET_DUPLEX_FULL_LENGTH + 1
#define ETHERNET_ERROR_IP_STRING_INDEX        ETHERNET_ERROR_DUPLEX_STRING_INDEX + ETHERNET_ERROR_DUPLEX_LENGTH + 1
#define ETHERNET_ERROR_SUBNET_STRING_INDEX    ETHERNET_ERROR_IP_STRING_INDEX + ETHERNET_ERROR_IP_LENGTH + 1
#define ETHERNET_ERROR_GATEWAY_STRING_INDEX   ETHERNET_ERROR_SUBNET_STRING_INDEX + ETHERNET_ERROR_SUBNET_LENGTH + 1
#define ETHERNET_ERROR_DNS_STRING_INDEX       ETHERNET_ERROR_GATEWAY_STRING_INDEX + ETHERNET_ERROR_GATEWAY_LENGTH + 1

#define DEVICE_STATS_LENGTH 12
#define DEVICE_STATS_CURTIME_LENGTH 7
#define DEVICE_STATS_USEDMEM_LENGTH 7
#define DEVICE_STATS_HEAPSTART_LENGTH 9
#define DEVICE_STATS_HEAPEND_LENGTH 7

#define DEVICE_STATS_STRING_INDEX       ETHERNET_ERROR_DNS_STRING_INDEX + ETHERNET_ERROR_DNS_LENGTH + 1
#define DEVICE_STATS_CURTIME_STRING_INDEX     DEVICE_STATS_STRING_INDEX + DEVICE_STATS_LENGTH + 1
#define DEVICE_STATS_USEDMEM_STRING_INDEX     DEVICE_STATS_CURTIME_STRING_INDEX + DEVICE_STATS_CURTIME_LENGTH + 1
#define DEVICE_STATS_HEAPSTART_STRING_INDEX   DEVICE_STATS_USEDMEM_STRING_INDEX + DEVICE_STATS_USEDMEM_LENGTH + 1
#define DEVICE_STATS_HEAPEND_STRING_INDEX     DEVICE_STATS_HEAPSTART_STRING_INDEX + DEVICE_STATS_HEAPSTART_LENGTH + 1

#define DEVICE_INFO_LENGTH          11
#define DEVICE_INFO_PRODUCT_LENGTH  7
#define DEVICE_INFO_MODEL_LENGTH    5
#define DEVICE_INFO_COMPANY_LENGTH  7
#define DEVICE_INFO_DESC_LENGTH     4

#define DEVICE_INFO_STRING_INDEX            DEVICE_STATS_HEAPEND_STRING_INDEX + DEVICE_STATS_HEAPEND_LENGTH + 1
#define DEVICE_INFO_PRODUCT_STRING_INDEX    DEVICE_INFO_STRING_INDEX + DEVICE_INFO_LENGTH + 1
#define DEVICE_INFO_MODEL_STRING_INDEX      DEVICE_INFO_PRODUCT_STRING_INDEX + DEVICE_INFO_PRODUCT_LENGTH + 1
#define DEVICE_INFO_COMPANY_STRING_INDEX    DEVICE_INFO_MODEL_STRING_INDEX + DEVICE_INFO_MODEL_LENGTH + 1
#define DEVICE_INFO_DESC_STRING_INDEX       DEVICE_INFO_COMPANY_STRING_INDEX + DEVICE_INFO_COMPANY_LENGTH + 1


static char const idigi_remote_all_strings[] = {
    SERIAL_LENGTH, 's', 'e', 'r', 'i', 'a', 'l',
    SERIAL_BAUD_LENGTH, 'b', 'a', 'u', 'd',
    SERIAL_PARITY_LENGTH, 'p', 'a', 'r', 'i', 't', 'y',
    SERIAL_DATABITS_LENGTH, 'd', 'a', 't', 'a','b', 'i', 't', 's',
    SERIAL_XBREAK_LENGTH, 'x', 'b','r','e','a','k',
    SERIAL_TXBYTES_LENGTH, 't','x','b','y','t', 'e', 's',

    SERIAL_BAUD_2400_LENGTH, '2', '4', '0', '0',

    SERIAL_BAUD_4800_LENGTH, '4', '8', '0', '0',
    SERIAL_BAUD_9600_LENGTH, '9', '6', '0', '0',
    SERIAL_BAUD_19200_LENGTH, '1', '9', '2', '0', '0',
    SERIAL_BAUD_38400_LENGTH, '3', '8', '4', '0', '0',
    SERIAL_BAUD_57600_LENGTH, '5', '7', '6', '0', '0',

    SERIAL_BAUD_115200_LENGTH, '1', '1', '5', '2', '0', '0',
    SERIAL_BAUD_230400_LENGTH, '2', '3', '0', '4', '0', '0',
    SERIAL_PARITY_NONE_LENGTH, 'n', 'o', 'n', 'e',
    SERIAL_PARITY_ODD_LENGTH, 'o', 'd', 'd',
    SERIAL_PARITY_EVEN_LENGTH, 'e', 'v', 'e', 'n',

    ERROR_FIELD_NOT_EXIT_LENGTH, 'F', 'i', 'e', 'l', 'd', ' ', 's', 'p', 'e', 'c', 'i', 'f', 'i', 'e', 'd', ' ', 'd', 'o', 'e', 's', ' ', 'n', 'o', 't', ' ', 'e', 'x', 'i', 's', 't',
    ERROR_LOAD_FAILED_LENGTH, 'L', 'o', 'a', 'd', ' ', 'f', 'a', 'i', 'l', 'e', 'd',
    ERROR_SAVE_FAILED_LENGTH, 'S', 'a', 'v', 'e', ' ', 'f', 'a', 'i', 'l', 'e', 'd',
    SERIAL_ERROR_BAUD_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'b', 'a', 'u', 'd', ' ', 'r', 'a', 't', 'e',
    SERIAL_ERROR_DATABITS_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'd', 'a', 't', 'a', ' ', 'b', 'i', 't', 's',
    SERIAL_ERROR_PARITY_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'p', 'a', 'r', 'i', 't', 'y',
    SERIAL_ERROR_XBREAK_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'x', 'b', 'r', 'e', 'a', 'k', ' ', 's', 'e', 't', 't', 'i', 'n', 'g',
    SERIAL_ERROR_DATABITS_PARITY_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'c', 'o', 'm', 'b', 'i', 'n', 'a', 't', 'i', 'o', 'n', ' ', 'o', 'f', ' ', 'd', 'a', 't', 'a', ' ', 'b', 'i', 't', 's', ' ', 'a', 'n', 'd', ' ', 'p', 'a', 'r', 'i', 't', 'y',

    ETHERNET_LENGTH, 'e', 't', 'h', 'e', 'r', 'n', 'e', 't',
    ETHERNET_IP_LENGTH, 'i', 'p',
    ETHERNET_SUBNET_LENGTH, 's', 'u', 'b', 'n', 'e', 't',
    ETHERNET_GATEWAY_LENGTH, 'g', 'a', 't', 'e', 'w', 'a', 'y',
    ETHERNET_DHCP_LENGTH, 'd', 'h', 'c', 'p',
    ETHERNET_DNS_LENGTH, 'd', 'n', 's',
    ETHERNET_DUPLEX_LENGTH, 'd', 'u', 'p', 'l', 'e', 'x',

    ETHERNET_DUPLEX_AUTO_LENGTH, 'a', 'u', 't', 'o',
    ETHERNET_DUPLEX_HALF_LENGTH, 'h', 'a', 'l', 'f',
    ETHERNET_DUPLEX_FULL_LENGTH, 'f', 'u', 'l', 'l',

    ETHERNET_ERROR_DUPLEX_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'e', 't', 'h', 'e', 'r', 'n', 'e', 't', ' ', 'd', 'u', 'p', 'l', 'e', 'x', ' ', 's', 'e', 't', 't', 'i', 'n', 'g',
    ETHERNET_ERROR_IP_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'I', 'P', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's',
    ETHERNET_ERROR_SUBNET_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 's', 'u', 'b', 'n', 'e', 't', ' ', 'm', 'a', 's', 'k',
    ETHERNET_ERROR_GATEWAY_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'g', 'a', 't', 'e', 'w', 'a', 'y', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's',
    ETHERNET_ERROR_DNS_LENGTH, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'D', 'N', 'S', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's',

    DEVICE_STATS_LENGTH, 'd', 'e', 'v', 'i', 'c', 'e', '_', 's', 't', 'a', 't', 's',
    DEVICE_STATS_CURTIME_LENGTH, 'c', 'u', 'r', 't', 'i', 'm', 'e',
    DEVICE_STATS_USEDMEM_LENGTH, 'u', 's', 'e', 'd', 'm', 'e', 'm',
    DEVICE_STATS_HEAPSTART_LENGTH, 'h', 'e', 'a', 'p', 's', 't', 'a', 'r', 't',
    DEVICE_STATS_HEAPEND_LENGTH, 'h', 'e', 'a', 'p', 'e', 'n', 'd',

    DEVICE_INFO_LENGTH, 'd', 'e', 'v', 'i', 'c', 'e', '_', 'i', 'n', 'f', 'o',
    DEVICE_INFO_PRODUCT_LENGTH, 'p', 'r', 'o', 'd', 'u', 'c', 't',
    DEVICE_INFO_MODEL_LENGTH, 'm', 'o', 'd', 'e', 'l',
    DEVICE_INFO_COMPANY_LENGTH, 'c', 'o', 'm', 'p', 'a', 'n', 'y',
    DEVICE_INFO_DESC_LENGTH, 'd', 'e', 's', 'c',

};
static char const * const serial_baud_enum[] = {
    &idigi_remote_all_strings[SERIAL_BAUD_2400_STRING_INDEX],  /* 2400 */
    &idigi_remote_all_strings[SERIAL_BAUD_4800_STRING_INDEX],  /* 4800 */
    &idigi_remote_all_strings[SERIAL_BAUD_9600_STRING_INDEX],  /* 9600 */
    &idigi_remote_all_strings[SERIAL_BAUD_19200_STRING_INDEX],  /* 19200 */
    &idigi_remote_all_strings[SERIAL_BAUD_38400_STRING_INDEX],  /* 38400 */
    &idigi_remote_all_strings[SERIAL_BAUD_57600_STRING_INDEX],  /* 57600 */
    &idigi_remote_all_strings[SERIAL_BAUD_115200_STRING_INDEX],  /* 115200 */
    &idigi_remote_all_strings[SERIAL_BAUD_230400_STRING_INDEX],  /* 230400 */
};

static idigi_element_value_enum_t const serial_baud_limit = {
    asizeof(serial_baud_enum),
    serial_baud_enum
};


static char const * const serial_parity_enum[] = {
    &idigi_remote_all_strings[SERIAL_PARITY_NONE_STRING_INDEX], /* none */
    &idigi_remote_all_strings[SERIAL_PARITY_ODD_STRING_INDEX], /* odd */
    &idigi_remote_all_strings[SERIAL_PARITY_EVEN_STRING_INDEX], /* even */
};

static idigi_element_value_enum_t const serial_parity_limit = {
    asizeof(serial_parity_enum),
    serial_parity_enum
};

static idigi_element_value_unsigned_integer_t const serial_databits_limit = {
    5,
    8
};

static idigi_group_element_t const serial_elements[] =
{
    { &idigi_remote_all_strings[SERIAL_BAUD_STRING_INDEX], /* "baud" */
    idigi_element_access_read_write,
    idigi_element_type_enum,
    (idigi_element_value_limit_t *)&serial_baud_limit
    },

    { &idigi_remote_all_strings[SERIAL_PARITY_STRING_INDEX], /* "parity" */
    idigi_element_access_read_write,
    idigi_element_type_enum,
    (idigi_element_value_limit_t *)&serial_parity_limit},

    { &idigi_remote_all_strings[SERIAL_DATABITS_STRING_INDEX], /* "databits" */
    idigi_element_access_read_write,
    idigi_element_type_uint32,
    (idigi_element_value_limit_t *)&serial_databits_limit},

    { &idigi_remote_all_strings[SERIAL_XBREAK_STRING_INDEX], /* "xbreak" */
    idigi_element_access_read_write,
    idigi_element_type_on_off,
    NULL},

    { &idigi_remote_all_strings[SERIAL_TXBYTES_STRING_INDEX], /* "txbytes" */
    idigi_element_access_read_only,
    idigi_element_type_uint32,
    NULL}
};

static char const * const serial_errors[] = {
    &idigi_remote_all_strings[ERROR_FIELD_NOT_EXIT_STRING_INDEX], /* Field specified does not exist */
    &idigi_remote_all_strings[ERROR_LOAD_FAILED_STRING_INDEX], /* Load failed */
    &idigi_remote_all_strings[ERROR_SAVE_FAILED_STRING_INDEX], /* Save failed */
    &idigi_remote_all_strings[SERIAL_ERROR_BAUD_STRING_INDEX], /* Invalid baud rate */
    &idigi_remote_all_strings[SERIAL_ERROR_DATABITS_STRING_INDEX], /* Invalid data bits */
    &idigi_remote_all_strings[SERIAL_ERROR_PARITY_STRING_INDEX], /* Invalid parity */
    &idigi_remote_all_strings[SERIAL_ERROR_XBREAK_STRING_INDEX], /* Invalid xbreak setting */
    &idigi_remote_all_strings[SERIAL_ERROR_DATABITS_PARITY_STRING_INDEX], /* Invalid combination of data bits and parity */
};


static idigi_element_value_string_t const ethernet_dns_limit = {
    0,
    64
};


static char const * const ethernet_duplex_enum[] = {
    &idigi_remote_all_strings[ETHERNET_DUPLEX_AUTO_STRING_INDEX], /* "auto" */
    &idigi_remote_all_strings[ETHERNET_DUPLEX_HALF_STRING_INDEX], /* "half" */
    &idigi_remote_all_strings[ETHERNET_DUPLEX_FULL_STRING_INDEX]  /* "full" */
};

static idigi_element_value_enum_t const ethernet_duplex_limit = {
        asizeof(ethernet_duplex_enum),
        ethernet_duplex_enum
};

static idigi_group_element_t const ethernet_elements[] = {
    { &idigi_remote_all_strings[ETHERNET_IP_STRING_INDEX], /* ip */
    idigi_element_access_read_write,
    idigi_element_type_ipv4,
    NULL},

    { &idigi_remote_all_strings[ETHERNET_SUBNET_STRING_INDEX], /* subnet */
    idigi_element_access_read_write,
    idigi_element_type_ipv4,
    NULL},

    { &idigi_remote_all_strings[ETHERNET_GATEWAY_STRING_INDEX], /* gateway */
    idigi_element_access_read_write,
    idigi_element_type_ipv4,
    NULL},

    { &idigi_remote_all_strings[ETHERNET_DHCP_STRING_INDEX], /* dhcp */
    idigi_element_access_read_write,
    idigi_element_type_boolean,
    NULL},

    { &idigi_remote_all_strings[ETHERNET_DNS_STRING_INDEX], /* dns */
    idigi_element_access_read_write,
    idigi_element_type_fqdnv4,
    NULL},

    { &idigi_remote_all_strings[ETHERNET_DUPLEX_STRING_INDEX], /* duplex */
    idigi_element_access_read_write,
    idigi_element_type_enum,
    (idigi_element_value_limit_t *)&ethernet_duplex_limit},

};

static char const * const ethernet_errors[] = {
        &idigi_remote_all_strings[ERROR_FIELD_NOT_EXIT_STRING_INDEX], /* Field specified does not exist */
        &idigi_remote_all_strings[ERROR_LOAD_FAILED_STRING_INDEX], /* Load failed */
        &idigi_remote_all_strings[ERROR_SAVE_FAILED_STRING_INDEX], /* Save failed */
        &idigi_remote_all_strings[ETHERNET_ERROR_DUPLEX_STRING_INDEX], /* Invalid ethernet duplex setting */
        &idigi_remote_all_strings[ETHERNET_ERROR_IP_STRING_INDEX], /* Invalid ip address */
        &idigi_remote_all_strings[ETHERNET_ERROR_SUBNET_STRING_INDEX], /* Invalid subnet mask */
        &idigi_remote_all_strings[ETHERNET_ERROR_GATEWAY_STRING_INDEX], /* Invalid gateway address */
        &idigi_remote_all_strings[ETHERNET_ERROR_DNS_STRING_INDEX], /* Invalid DNS address */
};

static idigi_group_element_t const device_stats_elements[] = {
    { &idigi_remote_all_strings[DEVICE_STATS_CURTIME_STRING_INDEX], /* curtime */
    idigi_element_access_read_write,
    idigi_element_type_datetime,
    NULL},

    { &idigi_remote_all_strings[DEVICE_STATS_USEDMEM_STRING_INDEX], /* usedmem */
    idigi_element_access_read_only,
    idigi_element_type_uint32,
    NULL},

    { &idigi_remote_all_strings[DEVICE_STATS_HEAPSTART_STRING_INDEX], /* heapstart */
    idigi_element_access_read_write,
    idigi_element_type_hex32,
    NULL},

    { &idigi_remote_all_strings[DEVICE_STATS_HEAPEND_STRING_INDEX], /* heapend */
    idigi_element_access_read_write,
    idigi_element_type_0xhex,
    NULL},
};

static char const * const device_stats_errors[] = {
        &idigi_remote_all_strings[ERROR_FIELD_NOT_EXIT_STRING_INDEX], /* Field specified does not exist */
        &idigi_remote_all_strings[ERROR_LOAD_FAILED_STRING_INDEX], /* Load failed */
        &idigi_remote_all_strings[ERROR_SAVE_FAILED_STRING_INDEX], /* Save failed */
};


static idigi_element_value_string_t const device_info_product_limit = {
        1,
        64
};

static idigi_element_value_string_t const device_info_model_limit = {
        0,
        32
};

static idigi_group_element_t const device_info_elements[] = {
    { &idigi_remote_all_strings[DEVICE_INFO_PRODUCT_STRING_INDEX], /* product */
    idigi_element_access_read_write,
    idigi_element_type_string,
    (idigi_element_value_limit_t *)&device_info_product_limit},

    { &idigi_remote_all_strings[DEVICE_INFO_MODEL_STRING_INDEX], /* model */
    idigi_element_access_read_write,
    idigi_element_type_string,
    (idigi_element_value_limit_t *)&device_info_model_limit},

    { &idigi_remote_all_strings[DEVICE_INFO_COMPANY_STRING_INDEX], /* company */
    idigi_element_access_read_write,
    idigi_element_type_string,
    NULL},

    { &idigi_remote_all_strings[DEVICE_INFO_DESC_STRING_INDEX], /* desc */
    idigi_element_access_read_write,
    idigi_element_type_multiline_string,
    NULL},
};

static char const * const device_info_errors[] = {
        &idigi_remote_all_strings[ERROR_FIELD_NOT_EXIT_STRING_INDEX], /* Field specified does not exist */
        &idigi_remote_all_strings[ERROR_LOAD_FAILED_STRING_INDEX], /* Load failed */
        &idigi_remote_all_strings[ERROR_SAVE_FAILED_STRING_INDEX], /* Save failed */
};

idigi_group_t const idigi_groups[] = {
    {
        &idigi_remote_all_strings[0], /* "serial" */
        1,
        2,
        {
            asizeof(serial_elements),
            &serial_elements[0]
        },
        {
            asizeof(serial_errors),
            serial_errors
        }
    },

    {
        &idigi_remote_all_strings[ETHERNET_STRING_INDEX], /* "Ethernet" */
        1,
        1,
        {
            asizeof(ethernet_elements),
            ethernet_elements
        },
        {
            asizeof(ethernet_errors),
            ethernet_errors
        }
    },

    {
        &idigi_remote_all_strings[DEVICE_STATS_STRING_INDEX], /* "device_stats" */
        1,
        1,
        {
            asizeof(device_stats_elements),
            device_stats_elements
        },
        {
            asizeof(device_stats_errors),
            device_stats_errors
        }
    },

    {
        &idigi_remote_all_strings[DEVICE_INFO_STRING_INDEX], /* "device_info" */
        1,
        1,
        {
            asizeof(device_info_elements),
            device_info_elements
        },
        {
            asizeof(device_info_errors),
            device_info_errors
        }
    }
};

size_t const idigi_group_count = asizeof(idigi_groups);

