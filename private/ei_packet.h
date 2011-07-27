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
#ifndef _EI_PACKET_H
#define _EI_PACKET_H

#define field_glue(left, right)                 left ## _ ## right
#define field_name(record, field)               field_glue(record,field)
#define field_data(name, data)                  field_glue(name, data)

#define field_named_data(record, field, data)   field_data(field_name(record, field), data)

#define field_allocate(name, type)              field_data(name, offset), \
                                                field_data(name, size) = (sizeof(type)), \
                                                field_data(name, next) = (field_data(name, offset) + field_data(name, size) - 1)

#define field_define(record, field, type)       field_allocate(field_name(record, field), type)

#define record_end(name)                        field_data(name, bytes)

#define record_bytes(name)                      field_data(name, bytes)

#define message_store_be16(record, field, value) \
    do { \
        ASSERT(field_named_data(record, field, size) == sizeof (uint16_t)); \
        StoreBE16(record + field_named_data(record, field, offset), (value)); \
    } while (0)

#define message_store_u8(record, field, value) \
    do { \
        ASSERT(field_named_data(record, field, size) == sizeof (uint8_t)); \
        *(record + field_named_data(record, field, offset)) = (value); \
    } while (0)

#if defined(DEBUG)
#define message_load_be16(record, field) (ASSERT(field_named_data(record, field, size) == sizeof (uint16_t)), LoadBE16(record + field_named_data(record, field, offset)))
#define message_load_u8(record, field) (ASSERT(field_named_data(record, field, size) == sizeof (uint8_t)), *(record + field_named_data(record, field, offset)))
#else
#define message_load_be16(record, field) LoadBE16(record + field_named_data(record, field, offset))
#define message_load_u8(record, field) *(record + field_named_data(record, field, offset))
#endif

#define PACKET_EDP_PROTOCOL_SIZE        record_bytes(edp_protocol)
#define PACKET_EDP_HEADER_SIZE          record_bytes(edp_header)
#define PACKET_EDP_FACILITY_SIZE        PACKET_EDP_PROTOCOL_SIZE + PACKET_EDP_HEADER_SIZE

/* private definitions */
enum edp_header {
    field_define(edp_header, type, uint16_t),
    field_define(edp_header, length, uint16_t),
    record_end(edp_header),
};

enum edp_protocol {
    field_define(edp_protocol, sec_coding, uint8_t),
    field_define(edp_protocol, payload, uint8_t),
    field_define(edp_protocol, facility, uint16_t),
    record_end(edp_protocol)
};


#endif  /* _EI_PACKET_H */
