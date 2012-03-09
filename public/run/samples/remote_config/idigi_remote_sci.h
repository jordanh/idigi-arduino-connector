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

#ifndef IDIGI_REMOTE_SCI_H_
#define IDIGI_REMOTE_SCI_H_

#include "stdio.h"
#include "idigi_api.h"
#include "idigi_remote.h"

typedef struct {
    idigi_remote_config_request_t request_id;

    idigi_remote_group_request_t request_data;
    idigi_remote_group_response_t response_data;

} idigi_remote_data_t;

extern void print_xml_open_index(char const * const tag, unsigned int index);
extern void print_xml_open(char const * const tag);
extern void print_xml_close(char const * const tag);
extern void print_xml_open_close(char const * const tag);
extern void print_xml_error(char const * const * errors, unsigned int count, unsigned int error_id, char const * const hint);
extern void print_xml_value(char const * const name);

extern idigi_callback_status_t remote_sci_request(char * rci_command, size_t length);

extern idigi_callback_status_t idigi_remote_query_handler(idigi_remote_data_t * remote_ptr);
extern idigi_callback_status_t idigi_remote_set_handler(idigi_remote_data_t * remote_ptr);

extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);

#endif /* IDIGI_REMOTE_SCI_H_ */
