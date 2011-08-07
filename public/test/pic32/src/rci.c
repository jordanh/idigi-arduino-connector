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
#include "idigi.h"

static bool get_rci_zlib_compression_support(void)
{
    return false;
}

idigi_callback_status_t idigi_rci_callback(idigi_rci_request_t const request_id,
                                              void const * request_data,
                                              size_t const request_length,
                                              void * response_data,
                                              size_t * response_length)
{
    idigi_callback_status_t status;
    bool ret = false;

    UNUSED_PARAMETER(request_data);
    UNUSED_PARAMETER(request_length);
    UNUSED_PARAMETER(response_data);
    UNUSED_PARAMETER(response_length);


    switch (request_id)
    {
    case idigi_rci_zlib_compression:
        *((bool *)response_data) = get_rci_zlib_compression_support();
        ret = true;
        break;
    case idigi_rci_compress_data:
    case idigi_rci_compress_data_done:
    case idigi_rci_decompress_data:
    case idigi_rci_decompress_data_done:
        /* not support zlib compression. should not call here */
        ASSERT(0);
        break;
    case idigi_rci_query_setting:
    case idigi_rci_query_state:
    case idigi_rci_set_setting:
    case idigi_rci_set_state:
    case idigi_rci_set_default:
    case idigi_rci_do_command:
    case idigi_rci_get_setting_descriptor:
    case idigi_rci_get_state_descriptor:
        break;

    }
    status = (ret == true) ? idigi_callback_continue : idigi_callback_abort;
    return status;
}

