/*
 * remote_config.h
 *
 *  Created on: Mar 2, 2012
 *      Author: mlchan
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

extern idigi_group_t idigi_groups[];
extern size_t const idigi_group_count;

#endif /* _REMOTE_CONFIG_H_ */
