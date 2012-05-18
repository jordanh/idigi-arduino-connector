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
 * Rountines which implement the IIK network interface.
 */

#include "idigi_api.h"
#include "platform.h"

static idigi_callback_status_t app_network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    UNUSED_ARGUMENT(host_name);
    UNUSED_ARGUMENT(length);
    UNUSED_ARGUMENT(network_handle);

    return rc;
}

static idigi_callback_status_t app_network_send(idigi_write_request_t const * const write_data,
                                            size_t * const sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    UNUSED_ARGUMENT(write_data);
    UNUSED_ARGUMENT(sent_length);

    return rc;
}

static idigi_callback_status_t app_network_receive(idigi_read_request_t * read_data, size_t * const read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    UNUSED_ARGUMENT(read_data);
    UNUSED_ARGUMENT(read_length);
    return rc;
}

static idigi_callback_status_t app_network_close(idigi_network_handle_t * const fd)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(fd);

    return status;
}

/**
 * @cond DEV
 */
static idigi_callback_status_t app_server_disconnected(void)
{
    return idigi_callback_continue;
}

static idigi_callback_status_t app_server_reboot(void)
{
    /* should not return from rebooting the system */
    return idigi_callback_continue;
}

/*
 *  Callback routine to handle all networking related calls.
 */
idigi_callback_status_t app_network_handler(idigi_network_request_t const request,
                                            void const * const request_data, size_t const request_length,
                                            void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    switch (request)
    {
    case idigi_network_connect:
        status = app_network_connect((char *)request_data, request_length, response_data);
        *response_length = sizeof(idigi_network_handle_t);
        break;

    case idigi_network_send:
        status = app_network_send((idigi_write_request_t *)request_data, response_data);
        break;

    case idigi_network_receive:
        status = app_network_receive((idigi_read_request_t *)request_data, response_data);
        break;

    case idigi_network_close:
        status = app_network_close((idigi_network_handle_t *)request_data);
        break;

    case idigi_network_disconnected:
       status = app_server_disconnected();
       break;

    case idigi_network_reboot:
        status = app_server_reboot();
        break;

    default:
        status = idigi_callback_unrecognized;
        break;

    }

    return status;
}

/**
 * @endcond
 */
