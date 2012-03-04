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

enum {
    idigi_group_serial,
    idigi_group_ethernet
};

/* serial group */
enum {
    idigi_group_serial_baud_2400,
    idigi_group_serial_baud_4800,
    idigi_group_serial_baud_9600,
    idigi_group_serial_baud_19200,
    idigi_group_serial_baud_38400,
    idigi_group_serial_baud_57600,
    idigi_group_serial_baud_115200,
    idigi_group_serial_baud_230400
};

enum {
    idigi_group_serial_parity_none,
    idigi_group_serial_parity_odd,
    idigi_group_serial_parity_even
};

enum {
    idigi_group_serial_error_load_failed = 1,
    idigi_group_serial_error_save_failed,
    idigi_group_serial_error_unsupport_field,
    idigi_group_serial_error_invalid_baud,
    idigi_group_serial_error_invalid_databits,
    idigi_group_serial_error_invalid_parity,
    idigi_group_serial_error_invalid_xbreak,
    idigi_group_serial_error_invalid_databits_and_parity
};

enum {
    idigi_group_serial_baud,
    idigi_group_serial_parity,
    idigi_group_serial_databits,
    idigi_group_serial_xbreak,
    idigi_group_serial_txbytes
};

/* ethernet group */
enum {
    idigi_group_ethernet_duplex_auto,
    idigi_group_ethernet_duplex_half,
    idigi_group_ethernet_duplex_full
};

enum {
    idigi_group_ethernet_ip,
    idigi_group_ethernet_gateway,
    idigi_group_ethernet_dhcp,
    idigi_group_ethernet_duplex,
    idigi_group_rxbytes
};

enum {
    idigi_group_ethernet_error_load_failed = 1,
    idigi_group_ethernet_error_save_failed,
    idigi_group_ethernet_error_unsupport_field,
    idigi_group_ethernet_error_invalid_ethernet_duplex,
    idigi_group_ethernet_error_invalid_ip_address,
    idigi_group_ethernet_error_invalid_subnet,
    idigi_group_ethernet_error_invalid_gateway,
    idigi_group_ethernet_error_invalid_dns

};

extern idigi_group_t const idigi_groups[];
extern size_t const idigi_group_count;

#endif /* _REMOTE_CONFIG_H_ */
