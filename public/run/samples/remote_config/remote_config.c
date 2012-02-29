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
static char const * serial_baud[] = {"2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400"};
static size_t const serial_baud_count = asizeof(serial_baud);
static idigi_element_value_enum_t const serial_baud_limit = {serial_baud_count, serial_baud};

static char const * serial_parity[] = {"none", "odd", "even"};
static size_t const serial_parity_count = asizeof(serial_parity);
static idigi_element_value_enum_t const serial_parity_limit = {serial_parity_count, serial_parity};

static idigi_element_value_unsigned_integer_t const serial_data_bits_limit = {5, 8};

idigi_group_element_t serial_elements[] =
{
    {"baud", idigi_element_access_read_write, idigi_element_type_enum, serial_baud_limit},
	{"parity", idigi_element_access_read_write, idigi_element_type_enum, serial_parity_limit},
	{"databits", idigi_element_access_read_write, idigi_element_type_uint32, serial_data_bits_limit},
	{"xbreak", idigi_element_access_read_write, idigi_element_type_on_off},
	{"txbytes", idigi_element_access_read_only, idigi_element_type_uint32}
};

static size_t const serial_element_count = asizeof(serial_elements);

static idigi_element_value_string_t const dns_server_name_size_limit = {0, 64};

static char const * ethernet_duplex[] = {"auto", "half", "full"};
static size_t const ethernet_duplex_count = asizeof(ethernet_duplex);
static idigi_element_value_enum_t const ethernet_duplex_limit = {ethernet_duplex_count, ethernet_duplex};

idigi_group_element_t ethernet_elements[] =
{
    {"ip", idigi_element_access_read_write, idigi_element_type_ipv4},
    {"gateway", idigi_element_access_read_write, idigi_element_type_ipv4},
	{"dhcp", idigi_element_access_read_write, idigi_element_type_boolean},
	{"dns", idigi_element_access_read_write, idigi_element_type_fqdnv4, dns_server_name_size_limit},
	{"duplex", idigi_element_access_read_write, idigi_element_type_enum, ethernet_duplex_limit},
	{"rxbytes", idigi_element_access_read_only, idigi_element_type_uint32}
};

static size_t const ethernet_element_count = asizeof(ethernet_elements);

idigi_group_t idigi_groups[] = 
{
    {
        "serial",    /* name */
	    0,         /* min_index */
	    1,         /* max_index */
		{
			serial_element_count,
			serial_elements
		},
		{
		    2,   /* count */
			{"com/0 config", "com/1 config"}  /* description? */
		}
	},
	{
	    "Ethernet",
		0,
		0,
		{
		    ethernet_element_count,
			ethernet_elements
		},
		{
		    1,
			{"eth0 config"}
		}
	}
}

/*
RCI samples:

<rci_request version="1.1"><query_descriptor/></rci_request>
<rci_reply version="1.1"><error id=”3” desc=”Unknown command” hint=”query_descriptor”/></rci_reply>

<rci_request version=”1.1”><query_setting><serial/></query_setting></rci_request>
<rci_reply version=”1.1”><query_setting> <serial index="0"><baud>9600</baud> <parity>odd</parity>
    <databits>8</databits><xbreak>off</xbreak></serial></query_setting></rci_reply>

<rci_request version=”1.1”><set_setting><serial><baud>12345</baud></serial></set_setting></rci_request>
<rci_reply version=”1.1”><set_setting><serial><error id=”2”><desc>Invalid baud”</desc><hint>baud</hint>
    </error></serial></set_setting></rci_reply>

<rci_request version=”1.1”><set_setting><serial><baud>19200</baud></serial></set_setting></rci_request>
<rci_reply version=”1.1”><set_setting><serial index="0"><info>success???</info></serial></query_setting></rci_reply>
*/