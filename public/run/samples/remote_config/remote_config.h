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

#ifndef _REMOTE_CONFIG_H_
#define _REMOTE_CONFIG_H_

#include "idigi_remote.h"

#define asizeof(array) (sizeof(array)/sizeof(array[0]))


typedef enum {
    idigi_group_serial,
    idigi_group_ethernet,
    idigi_group_device_stats,
    idigi_group_device_info,
    idigi_group_ic_thread,
    idigi_group_width = INT_MAX
} idigi_group_id_t;

/* serial group */
typedef enum {
    idigi_group_serial_baud_2400,
    idigi_group_serial_baud_4800,
    idigi_group_serial_baud_9600,
    idigi_group_serial_baud_19200,
    idigi_group_serial_baud_38400,
    idigi_group_serial_baud_57600,
    idigi_group_serial_baud_115200,
    idigi_group_serial_baud_230400,
    idigi_group_serial_baud_width = INT_MAX
} idigi_group_serial_bauld_id_t;

typedef enum {
    idigi_group_serial_parity_none,
    idigi_group_serial_parity_odd,
    idigi_group_serial_parity_even,
    idigi_group_serial_parity_width = INT_MAX
} idigi_group_serial_parity_id_t;

typedef enum {
    idigi_group_serial_error_invalid_baud = idigi_group_error_count,
    idigi_group_serial_error_invalid_databits,
    idigi_group_serial_error_invalid_parity,
    idigi_group_serial_error_invalid_xbreak,
    idigi_group_serial_error_invalid_databits_and_parity,
    idigi_group_serial_error_width = INT_MAX
} idigi_group_serial_error_id;

typedef enum {
    idigi_group_serial_baud,
    idigi_group_serial_parity,
    idigi_group_serial_databits,
    idigi_group_serial_xbreak,
    idigi_group_serial_txbytes,
    idigi_group_serial_width = INT_MAX
} idigi_group_serial_id;

/* ethernet group */
typedef enum {
    idigi_group_ethernet_duplex_auto,
    idigi_group_ethernet_duplex_half,
    idigi_group_ethernet_duplex_full,
    idigi_group_ethernet_duplex_width = INT_MAX
} idigi_group_ethernet_duplex_id_t;

typedef enum {
    idigi_group_ethernet_ip,
    idigi_group_ethernet_subnet,
    idigi_group_ethernet_gateway,
    idigi_group_ethernet_dhcp,
    idigi_group_ethernet_dns,
    idigi_group_ethernet_duplex,
    idigi_group_ethernet_width = INT_MAX
} idigi_group_ethernet_id_t;

typedef enum {
    idigi_group_ethernet_error_invalid_ethernet_duplex = idigi_group_error_count,
    idigi_group_ethernet_error_invalid_ip_address,
    idigi_group_ethernet_error_invalid_subnet,
    idigi_group_ethernet_error_invalid_gateway,
    idigi_group_ethernet_error_invalid_dns,
    idigi_group_ethernet_error_width = INT_MAX
} idigi_group_ethernet_error_id_t;

typedef enum {
    idigi_group_device_stats_curtime,
    idigi_group_device_stats_ctemp,
    idigi_group_device_stats_ftemp,
    idigi_group_device_stats_width = INT_MAX
} idigi_group_device_stats_id_t;

typedef enum {
    idigi_group_device_info_product,
    idigi_group_device_info_model,
    idigi_group_device_info_company,
    idigi_group_device_info_desc,
    idigi_group_device_info_syspwd,
    idigi_group_device_info_width = INT_MAX
} idigi_group_device_info_id_t;

typedef enum {
    idigi_group_device_info_error_invalid_length = idigi_group_error_count,
    idigi_group_device_info_error_width = INT_MAX
} idigi_group_device_info_error_id_t;

typedef enum {
    idigi_group_debug_info_version,
    idigi_group_debug_info_stacktop,
    idigi_group_debug_info_stacksize,
    idigi_group_debug_info_stackbottom,
    idigi_group_debug_info_usedmem,
    idigi_group_debug_info_width = INT_MAX
} idigi_group_debug_info_id_t;

extern idigi_group_t const idigi_config_groups[];
extern size_t const idigi_config_group_count;

extern idigi_group_t const * idigi_sysinfo_groups;
extern size_t const idigi_sysinfo_group_count;

extern char const idigi_remote_all_strings[];

#endif /* _REMOTE_CONFIG_H_ */
