/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <ipcfg.h>
#include <errno.h>
#include <idigi_api.h>
#include <idigi_types.h>
#include <platform.h>
#include <idigi_debug.h>

int idigi_network_receive_failures = 0;
int idigi_network_send_failures = 0;
int idigi_connect_to_idigi_successes = 0;
int idigi_connect_to_idigi_failures = 0;

static int socket_fd = RTCS_SOCKET_ERROR;

static boolean dns_resolve_name(char const * const name, _ip_address * const ip_addr)
{
    boolean result = FALSE;
    size_t tries = 3;

    APP_DEBUG("dns_resolve_name: DNS Address : %d.%d.%d.%d\n",
        IPBYTES(ipcfg_get_dns_ip(IPCFG_default_enet_device, 0)));

    #define ONE_SECOND_DELAY    1000
    /* Try three times to get name */
    do
    {
        result = RTCS_resolve_ip_address((char_ptr)name, ip_addr, NULL, 0);

        if (result) break;

        APP_DEBUG("Failed - name not resolved\n");
        _time_delay(ONE_SECOND_DELAY);

    } while (--tries > 0);

    return result;
}

static boolean set_socket_options(int const fd)
{
    #define SOCKET_BUFFER_SIZE 512
    #define SOCKET_TIMEOUT_MSEC 1000
    boolean success = FALSE;
    uint_32 option = TRUE;

    if(setsockopt(fd, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof option) != RTCS_OK)
    {
        APP_DEBUG("set_non_blocking_socket: setsockopt OPT_RECEIVE_NOWAIT failed");
        goto error;
    }

    /* Reduce buffer size of socket to save memory */
    option = SOCKET_BUFFER_SIZE;
    if (setsockopt(socket_fd, SOL_TCP, OPT_TBSIZE, &option, sizeof option) != RTCS_OK)
    {
        APP_DEBUG("network_connect: setsockopt OPT_TBSIZE failed\n");
        goto error;
    }

    if (setsockopt(socket_fd, SOL_TCP, OPT_RBSIZE, &option, sizeof option) != RTCS_OK)
    {
        APP_DEBUG("network_connect: setsockopt OPT_RBSIZE failed\n");
        goto error;
    }

    /* set a socket timeout */
    option = SOCKET_TIMEOUT_MSEC;
    if (setsockopt(socket_fd, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof option) != RTCS_OK)
    {
        APP_DEBUG("network_connect: setsockopt OPT_TIMEWAIT_TIMEOUT failed\n");
        goto error;
    }

    success = TRUE;

error:
    return success;
}

static idigi_callback_status_t app_network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t status = idigi_callback_abort;
    sockaddr_in addr;

    if (socket_fd == RTCS_SOCKET_ERROR)
    {
        _ip_address ip_addr;

        if (!dns_resolve_name(host_name, &ip_addr))
            goto error;

        APP_DEBUG("network_connect: Resolved IP Address: %d.%d.%d.%d\n\n", IPBYTES(ip_addr));

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == RTCS_SOCKET_ERROR)
        {
            APP_DEBUG("Could not open socket\n");
            goto done;
        }

        set_socket_options(socket_fd);

        {
            struct sockaddr_in sin = {0};

            /* Allow binding to any address */
            addr.sin_family      = AF_INET;
            addr.sin_port        = 0;
            addr.sin_addr.s_addr = INADDR_ANY;

            if (bind(socket_fd, &addr, sizeof addr) != RTCS_OK)
            {
                APP_DEBUG("Error in binding socket %08x\n", errno);
                goto error;
            }

            APP_DEBUG("Connecting to %s...\n", host_name);

            /* Connect to device */
            addr.sin_port        = IDIGI_PORT;
            addr.sin_addr.s_addr = ip_addr;
            if (connect(socket_fd, &addr, sizeof addr) != RTCS_OK)
            {
                if (errno != EAGAIN && errno != MQX_EINPROGRESS)
                {
                    idigi_connect_to_idigi_failures ++;
                    APP_DEBUG("network_connect: connect() failed %d\n", errno);
                    goto error;
                }
            }
        }
    }

    idigi_connect_to_idigi_successes ++;
    *network_handle = &socket_fd;
    status = idigi_callback_continue;
    APP_DEBUG("network_connect: connected to [%.*s] server\n", (int)length, host_name);
    goto done;

error:
    shutdown(socket_fd, FLAG_ABORT_CONNECTION);
    socket_fd = RTCS_SOCKET_ERROR;

done:
    return status;
}

/*
 * Send data to the iDigi server, this routine must not block.  If it encounters
 * EAGAIN or EWOULDBLOCK error, 0 bytes must be returned and iDigi Connector will continue
 * calling this function.
 */
static idigi_callback_status_t app_network_send(idigi_write_request_t const * const write_data,
                                                size_t * const sent_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint32_t bytes_sent;

    bytes_sent = send(*write_data->network_handle, (char _PTR_)write_data->buffer, write_data->length, 0);
    if (bytes_sent == RTCS_ERROR)
    {
        status = idigi_callback_abort;
        APP_DEBUG("network_send: send() failed RTCS error [%d]", RTCS_geterror(*write_data->network_handle));
        idigi_network_send_failures ++;
    }
    else
    {
        *sent_length = bytes_sent;
        if (bytes_sent == 0)
            status = idigi_callback_busy;
    }

    return status;
}

/*
 * This routine reads a specified number of bytes from the iDigi server.  This
 * function must not block. If it encounters EAGAIN or EWOULDBLOCK error, 0
 * bytes must be returned and iDigi Connector will continue calling this function.
 */
static idigi_callback_status_t app_network_receive(idigi_read_request_t const * const read_data, size_t * const read_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    uint_32 bytes_read;

    *read_length = 0;
    bytes_read = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);
    if (bytes_read == RTCS_ERROR)
    {
        if (errno == EAGAIN)
            status = idigi_callback_busy;
        else
        {
            APP_DEBUG("network_receive: Error, recv() failed RTCS error [%d]", RTCS_geterror(*read_data->network_handle));
            status = idigi_callback_abort;
            idigi_network_receive_failures ++;
        }
    }
    else
    {
        *read_length = bytes_read;
        if (bytes_read == 0)
            status = idigi_callback_busy;
    }

    if (*read_length == 0)
        _time_delay(1);

    return status;
}

static idigi_callback_status_t app_network_close(idigi_network_handle_t * const fd)
{
    ASSERT(*fd == socket_fd);

    // Note: this does a graceful close - like linger
    if (shutdown(*fd, FLAG_CLOSE_TX) != RTCS_OK)
    {
        APP_DEBUG("network_close: failed, code = %d", errno);
    }

    socket_fd = RTCS_SOCKET_ERROR;

error:
    return idigi_callback_continue;
}

static idigi_callback_status_t app_server_disconnected(void)
{
    APP_DEBUG("Disconnected from server\n");
    /* if idigi_run or idigi_step is called again,
    * it will reconnect to iDigi Cloud.
    */
    return idigi_callback_continue;
}

static idigi_callback_status_t app_server_reboot(void)
{
    APP_DEBUG("Reboot from server\n");
    /* should not return from rebooting the system */
    return idigi_callback_continue;
}

/*
 *  Callback routine to handle all networking related calls.
 */
idigi_callback_status_t app_network_handler(idigi_network_request_t const request,
                                            void const * const request_data, size_t const request_length,
                                            void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
        case idigi_network_connect:
            status = app_network_connect(request_data, request_length, response_data);
            *response_length = sizeof(idigi_network_handle_t);
            break;

        case idigi_network_send:
            status = app_network_send(request_data, response_data);
            break;

        case idigi_network_receive:
            status = app_network_receive(request_data, response_data);
            break;

        case idigi_network_close:
            status = app_network_close((idigi_network_handle_t * const)request_data);
            break;

        case idigi_network_disconnected:
            status = app_server_disconnected();
            break;

        case idigi_network_reboot:
            status = app_server_reboot();
            break;

        default:
            APP_DEBUG("app_network_handler: unrecognized callback request [%d]\n", request);
            status = idigi_callback_unrecognized;
            break;
    }

    return status;
}
