/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
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

static char idigi_remote_all_strings[] = {
    6, 's', 'e', 'r', 'i', 'a', 'l',
    4, 'b', 'a', 'u', 'd',
    6, 'p', 'a', 'r', 'i', 't', 'y',
    8, 'd', 'a', 't', 'a','b', 'i', 't', 's',
    6, 'x', 'b','r','e','a','k',
    7, 't','x','b','y','t', 'e', 's',

    4, '2', '4', '0', '0',

    4, '4', '8', '0', '0',
    4, '9', '6', '0', '0',
    5, '1', '9', '2', '0', '0',
    5, '3', '8', '4', '0', '0',
    5, '5', '7', '6', '0', '0',

    6, '1', '1', '5', '2', '0', '0',
    6, '2', '3', '0', '4', '0', '0',
    4, 'n', 'o', 'n', 'e',
    3, 'o', 'd', 'd',
    4, 'e', 'v', 'e', 'n',

    11, 'L', 'o', 'a', 'd', ' ', 'f', 'a', 'i', 'l', 'e', 'd',
    11, 'S', 'a', 'v', 'e', ' ', 'f', 'a', 'i', 'l', 'e', 'd',
    30, 'F', 'i', 'e', 'l', 'd', ' ', 's', 'p', 'e', 'c', 'i', 'f', 'i', 'e', 'd', ' ', 'd', 'o', 'e', 's', ' ', 'n', 'o', 't', ' ', 'e', 'x', 'i', 's', 't',
    17, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'b', 'a', 'u', 'd', ' ', 'r', 'a', 't', 'e',
    17, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'd', 'a', 't', 'a', ' ', 'b', 'i', 't', 's',
    14, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'p', 'a', 'r', 'i', 't', 'y',
    22, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'x', 'b', 'r', 'e', 'a', 'k', ' ', 's', 'e', 't', 't', 'i', 'n', 'g',
    43, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'c', 'o', 'm', 'b', 'i', 'n', 'a', 't', 'i', 'o', 'n', ' ', 'o', 'f', ' ', 'd', 'a', 't', 'a', ' ', 'b', 'i', 't', 's', ' ', 'a', 'n', 'd', ' ', 'p', 'a', 'r', 'i', 't', 'y',

    8, 'e', 't', 'h', 'e', 'r', 'n', 'e', 't',
    2, 'i', 'p',
    6, 's', 'u', 'b', 'n', 'e', 't',
    7, 'g', 'a', 't', 'e', 'w', 'a', 'y',
    4, 'd', 'h', 'c', 'p',
    3, 'd', 'n', 's',
    6, 'd', 'u', 'p', 'l', 'e', 'x',

    4, 'a', 'u', 't', 'o',
    4, 'h', 'a', 'l', 'f',
    4, 'f', 'u', 'l', 'l',

    31, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'e', 't', 'h', 'e', 'r', 'n', 'e', 't', ' ', 'd', 'u', 'p', 'l', 'e', 'x', ' ', 's', 'e', 't', 't', 'i', 'n', 'g',
    18, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'I', 'P', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's',
    19, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 's', 'u', 'b', 'n', 'e', 't', ' ', 'm', 'a', 's', 'k',
    23, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'g', 'a', 't', 'e', 'w', 'a', 'y', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's',
    19, 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'D', 'N', 'S', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's',

    12, 'd', 'e', 'v', 'i', 'c', 'e', '_', 's', 't', 'a', 't', 's',
    7, 'c', 'u', 'r', 't', 'i', 'm', 'e',
    7, 'u', 's', 'e', 'd', 'm', 'e', 'm',
    8, 'h', 'e', 'x', 'v', 'a', 'l', 'u', 'e',
    10, '0', 'x', 'h', 'e', 'x', 'v', 'a', 'l', 'u', 'e',

    11, 'd', 'e', 'v', 'i', 'c', 'e', '_', 'i', 'n', 'f', 'o',
    7, 'p', 'r', 'o', 'd', 'u', 'c', 't',
    5, 'm', 'o', 'd', 'e', 'l',
    7, 'c', 'o', 'm', 'p', 'a', 'n', 'y',
    4, 'd', 'e', 's', 'c',

};
static char const * const serial_baud_enum[] = {
    &idigi_remote_all_strings[43],  /* 2400 */
    &idigi_remote_all_strings[43 + 4 + 1],  /* 4800 */
    &idigi_remote_all_strings[48 + 4 + 1],  /* 9600 */
    &idigi_remote_all_strings[53 + 4 + 1],  /* 19200 */
    &idigi_remote_all_strings[58 + 5 + 1],  /* 38400 */
    &idigi_remote_all_strings[64 + 5 + 1],  /* 57600 */
    &idigi_remote_all_strings[70 + 5 + 1],  /* 115200 */
    &idigi_remote_all_strings[76 + 6 + 1],  /* 230400 */
};

static idigi_element_value_enum_t const serial_baud_limit = {
    asizeof(serial_baud_enum),
    serial_baud_enum
};


static char const * const serial_parity_enum[] = {
    &idigi_remote_all_strings[90], /* none */
    &idigi_remote_all_strings[90 + 4 + 1], /* odd */
    &idigi_remote_all_strings[95 + 3 + 1], /* even */
};

static idigi_element_value_enum_t const serial_parity_limit = {
    asizeof(serial_parity_enum),
    serial_parity_enum
};

static idigi_element_value_unsigned_integer_t const serial_databits_limit = {
    5,
    8
};

static idigi_group_element_t serial_elements[] =
{
    { &idigi_remote_all_strings[6 + 1], /* "baud" */
    idigi_element_access_read_write,
    idigi_element_type_enum,
    (idigi_element_value_limit_t *)&serial_baud_limit
    },

    { &idigi_remote_all_strings[7 + 4 + 1], /* "parity" */
    idigi_element_access_read_write,
    idigi_element_type_enum,
    (idigi_element_value_limit_t *)&serial_parity_limit},

    { &idigi_remote_all_strings[12 + 6 + 1], /* "databits" */
    idigi_element_access_read_write,
    idigi_element_type_uint32,
    (idigi_element_value_limit_t *)&serial_databits_limit},

    { &idigi_remote_all_strings[19 + 8 + 1], /* "xbreak" */
    idigi_element_access_read_write,
    idigi_element_type_on_off,
    NULL},

    { &idigi_remote_all_strings[28 + 6 + 1], /* "txbytes" */
    idigi_element_access_read_only,
    idigi_element_type_uint32,
    NULL}
};

static char const * const serial_errors[] = {
    &idigi_remote_all_strings[104], /* Load failed */
    &idigi_remote_all_strings[104 + 11 + 1], /* Save failed */
    &idigi_remote_all_strings[116 + 11 + 1], /* Field specified does not exist */
    &idigi_remote_all_strings[128 + 30 + 1], /* Invalid baud rate */
    &idigi_remote_all_strings[159 + 17 + 1], /* Invalid data bits */
    &idigi_remote_all_strings[177 + 17 + 1], /* Invalid parity */
    &idigi_remote_all_strings[195 + 14 + 1], /* Invalid xbreak setting */
    &idigi_remote_all_strings[210 + 22 + 1], /* Invalid combination of data bits and parity */
};


static idigi_element_value_string_t const ethernet_dns_limit = {
    0,
    64
};


static char const * const ethernet_duplex_enum[] = {
    &idigi_remote_all_strings[313 + 6 + 1], /* "auto" */
    &idigi_remote_all_strings[320 + 4 + 1], /* "half" */
    &idigi_remote_all_strings[325 + 4 + 1]  /* "full" */
};

static idigi_element_value_enum_t const ethernet_duplex_limit = {
        asizeof(ethernet_duplex_enum),
        ethernet_duplex_enum
};

static idigi_group_element_t ethernet_elements[] = {
    { &idigi_remote_all_strings[277 + 8 + 1], /* ip */
    idigi_element_access_read_write,
    idigi_element_type_ipv4,
    NULL},

    { &idigi_remote_all_strings[286 + 2 + 1 ], /* subnet */
    idigi_element_access_read_write,
    idigi_element_type_ipv4,
    NULL},

    { &idigi_remote_all_strings[289 + 6 + 1], /* gateway */
    idigi_element_access_read_write,
    idigi_element_type_ipv4,
    NULL},

    { &idigi_remote_all_strings[296 + 7 + 1], /* dhcp */
    idigi_element_access_read_write,
    idigi_element_type_boolean,
    NULL},

    { &idigi_remote_all_strings[304 + 4 + 1], /* dns */
    idigi_element_access_read_write,
    idigi_element_type_fqdnv4,
    NULL},

    { &idigi_remote_all_strings[309 + 3 + 1], /* duplex */
    idigi_element_access_read_write,
    idigi_element_type_enum,
    (idigi_element_value_limit_t *)&ethernet_duplex_limit},

};

static char const * const ethernet_errors[] = {
        &idigi_remote_all_strings[104], /* Load failed */
        &idigi_remote_all_strings[104 + 11 + 1], /* Save failed */
        &idigi_remote_all_strings[116 + 11 + 1], /* Field specified does not exist */
        &idigi_remote_all_strings[330 + 4 + 1], /* Invalid ethernet duplex setting */
        &idigi_remote_all_strings[335 + 31 + 1], /* Invalid ip address */
        &idigi_remote_all_strings[367 + 18 + 1], /* Invalid subnet mask */
        &idigi_remote_all_strings[386 + 19 + 1], /* Invalid gateway address */
        &idigi_remote_all_strings[406 + 23 + 1], /* Invalid DNS address */
};

static idigi_group_element_t device_stats_elements[] = {
    { &idigi_remote_all_strings[450 + 12 + 1], /* curtime */
    idigi_element_access_read_write,
    idigi_element_type_datetime,
    NULL},

    { &idigi_remote_all_strings[463+ 7 + 1], /* usedmem */
    idigi_element_access_read_only,
    idigi_element_type_uint32,
    NULL},

    { &idigi_remote_all_strings[471 + 7 + 1], /* hexvalue */
    idigi_element_access_read_write,
    idigi_element_type_hex32,
    NULL},

    { &idigi_remote_all_strings[479 + 8 + 1], /* 0xhexvalue */
    idigi_element_access_read_write,
    idigi_element_type_0xhex,
    NULL},
};

static char const * const device_stats_errors[] = {
        &idigi_remote_all_strings[104], /* Load failed */
        &idigi_remote_all_strings[104 + 11 + 1], /* Save failed */
        &idigi_remote_all_strings[116 + 11 + 1], /* Field specified does not exist */
};


static idigi_element_value_string_t const device_info_product_limit = {
        1,
        64
};

static idigi_element_value_string_t const device_info_model_limit = {
        0,
        32
};

static idigi_group_element_t device_info_elements[] = {
    { &idigi_remote_all_strings[498 + 12 + 1], /* product */
    idigi_element_access_read_write,
    idigi_element_type_string,
    (idigi_element_value_limit_t *)&device_info_product_limit},

    { &idigi_remote_all_strings[511 + 7 + 1], /* model */
    idigi_element_access_read_write,
    idigi_element_type_string,
    (idigi_element_value_limit_t *)&device_info_model_limit},

    { &idigi_remote_all_strings[519 + 5 + 1], /* company */
    idigi_element_access_read_write,
    idigi_element_type_string,
    NULL},

    { &idigi_remote_all_strings[525 + 7 + 1], /* desc */
    idigi_element_access_read_write,
    idigi_element_type_multiline_string,
    NULL},
};

static char const * const device_info_errors[] = {
        &idigi_remote_all_strings[104], /* Load failed */
        &idigi_remote_all_strings[104 + 11 + 1], /* Save failed */
        &idigi_remote_all_strings[116 + 11 + 1], /* Field specified does not exist */
};

idigi_group_t idigi_groups[] = {
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
        &idigi_remote_all_strings[233 + 43 + 1], /* "Ethernet" */
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
        &idigi_remote_all_strings[430 + 19 + 1], /* "device_stats" */
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
        &idigi_remote_all_strings[488 + 10 + 1], /* "device_info" */
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

