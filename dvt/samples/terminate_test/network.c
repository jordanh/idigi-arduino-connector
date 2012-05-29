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
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "idigi_api.h"
#include "platform.h"
#include "application.h"

enum {
    no_delay_receive,
    start_delay_receive,
    delay_receive_started
};
int delay_receive_flag = no_delay_receive;
unsigned long delay_receive_time = 0;

static int app_dns_resolve_name(char const * const domain_name, in_addr_t * const ip_addr)
{
    int ret = -1;
    struct addrinfo *res_list;
    struct addrinfo *res;

    if ((domain_name == NULL) || (ip_addr == NULL))
        goto done;

    {
        struct addrinfo hint = {0};
        int error;

        hint.ai_socktype = SOCK_STREAM;
        hint.ai_family   = AF_INET;
        error = getaddrinfo(domain_name, NULL, &hint, &res_list);
        if (error != 0)
        {
            APP_DEBUG("dns_resolve_name: DNS resolution failed for [%s]\n", domain_name);
            goto done;
        }
    }

    /* loop over all returned results and look for a IPv4 address */
    for (res = res_list; res; res = res->ai_next)
    {
        if (res->ai_family == PF_INET)
        {
            struct in_addr const ipv4_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;

            *ip_addr = ipv4_addr.s_addr;
            APP_DEBUG("dns_resolve_name: ip address = [%s]\n", inet_ntoa(ipv4_addr));
            ret = 0;
            break;
        }
    }

    freeaddrinfo(res_list);

done:
    return ret;
}

static idigi_callback_status_t app_network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_abort;
    static int fd = -1;

    if (fd == -1)
    {
        in_addr_t ip_addr;

        {
            char server_name[64];

            if (length >= asizeof(server_name))
            {
                APP_DEBUG("app_connect_to_server: server name length [%zu]\n", length);
                goto done;
            }

            memcpy(server_name, host_name, length);
            server_name[length] = '\0';

            /*
             * Check if it's a dotted-notation IP address, if it's a domain name,
             * attempt to resolve it.
             */
            ip_addr = inet_addr(server_name);
            if (ip_addr == INADDR_NONE)
            {
                if (app_dns_resolve_name(server_name, &ip_addr) != 0)
                {
                    APP_DEBUG("network_connect: Can't resolve DNS for %s\n", server_name);
                    goto done;
                }
            }
        }

        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd >= 0)
        {
            int enabled = 1;

            if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enabled, sizeof(enabled)) < 0)
            {
                perror("open_socket: setsockopt SO_KEEPALIVE failed");
            }

            if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enabled, sizeof(enabled)) < 0)
            {
                perror("open_socket: setsockopt TCP_NODELAY failed");
            }
        }
        else
        {
            perror("Could not open socket");
            goto done;
        }

        {
            int opt=1;

            if (ioctl(fd, FIONBIO, &opt) < 0)
            {
                perror("ioctl: FIONBIO failed");
                goto done;
            }
        }

        {
            struct sockaddr_in sin = {0};

            memcpy(&sin.sin_addr, &ip_addr, sizeof sin.sin_addr);
            sin.sin_port   = htons(IDIGI_PORT);
            sin.sin_family = AF_INET;

            if (connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
            {
                switch (errno)
                {
                case EAGAIN:
                case EINPROGRESS:
                    break;
                default:
                    perror("network_connect: connect() failed");
                    goto done;
                }
            }
        }
    }

    {
        struct timeval timeout = {30, 0};
        fd_set read_set;
        fd_set write_set;

        FD_ZERO(&read_set);
        FD_SET(fd, &read_set);
        write_set = read_set;

        if (select(fd+1, &read_set, &write_set, NULL, &timeout) <= 0)
        {
            goto done;
        }

        /* Check whether the socket is now writable (connection succeeded). */
        if (FD_ISSET(fd, &write_set))
        {
            /* We expect "socket writable" when the connection succeeds. */
            /* If we also got a "socket readable" we have an error. */
            if (FD_ISSET(fd, &read_set))
            {
                APP_DEBUG("network_connect: error to connect to %.*s server\n", (int)length, host_name);
                goto done;
            }
            *network_handle = &fd;
            rc = idigi_callback_continue;
            APP_DEBUG("network_connect: connected to [%.*s] server\n", (int)length, host_name);
        }
    }

done:
    if ((rc == idigi_callback_abort) && (fd >= 0))
    {
        close(fd);
        fd = -1;
    }

    return rc;
}
static idigi_callback_status_t app_network_send(idigi_write_request_t const * const write_data,
                                            size_t * const sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int ccode = write(*write_data->network_handle, (char *)write_data->buffer, write_data->length);

    if (ccode < 0) 
    {
        if (errno == EAGAIN)
        {
            rc = idigi_callback_busy;
        }
        else
        {
            char buffer[64];

            perror("network_send: send() failed");

            do {
                ccode = read(*write_data->network_handle, buffer, sizeof buffer);

                if (ccode == 0)
                {
                    /* EOF on input: the connection was closed. */
                    APP_DEBUG("network_send: receive EOF on socket\r\n");
                }
                else if (ccode < 0)
                {
                    /* An error of some sort occurred: handle it appropriately. */
                    if (errno != EAGAIN)
                    {
                       perror("network_send: receive: recv() failed");
                        /* if not timeout (no data) return an error */
                    }
                }
                else
                {
                    APP_DEBUG("network_send: receive bytes = %d\n", ccode);
                }
            } while (ccode > 0);

            rc = idigi_callback_abort;
        }
    }
    *sent_length = ccode;

    return rc;
}

static idigi_callback_status_t app_network_receive(idigi_read_request_t const * const read_data, size_t * const read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    struct timeval timeout = {read_data->timeout, 0};
    int ccode;

    timeout.tv_sec = read_data->timeout;
    timeout.tv_usec = 0;

    *read_length = 0;

    switch (delay_receive_flag)
    {
    case start_delay_receive:
        APP_DEBUG("start delay...\n");
        delay_receive_flag = delay_receive_started;
        app_os_get_system_time(&delay_receive_time);
        break;
    case delay_receive_started:
    {
        unsigned long elapsed_time;

        app_os_get_system_time(&elapsed_time);
        elapsed_time -= delay_receive_time;

        if (elapsed_time > 5 && elapsed_time < 60)
        {
            /* hold off receive for 60 seconds */
            rc = idigi_callback_busy;
            goto done;
        }
        break;
    }
    default:
        break;
    }

    {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(*read_data->network_handle, &read_set);

        /* Blocking point for iDigi Connector */
        ccode = select(*read_data->network_handle+1, &read_set, NULL, NULL, &timeout);
        if (ccode < 0)
        {
            goto done;
        }

        if (ccode == 0)
        {
            rc = idigi_callback_busy;
            goto done;
        }
    }

    ccode = read(*read_data->network_handle, read_data->buffer, read_data->length);

    if (ccode == 0)
    {
        /* EOF on input: the connection was closed. */
        APP_DEBUG("network_receive: EOF on socket\r\n");
        rc = idigi_callback_abort;
    }
    else if (ccode < 0)
    {
        /* An error of some sort occurred: handle it appropriately. */
        int const err = errno;
        if (err == EAGAIN)
        {
            rc = idigi_callback_busy;
        }
        else
        {
            perror("network_receive: recv() failed");
            /* if not timeout (no data) return an error */
            rc = idigi_callback_abort;
        }
    }

    *read_length = (size_t)ccode;
done:

    return rc;
}

static idigi_callback_status_t app_network_close(idigi_network_handle_t * const fd)
{
    idigi_callback_status_t status = idigi_callback_continue;
    struct linger ling_opt;

    ling_opt.l_linger = 1;
    ling_opt.l_onoff  = 1;

    if (setsockopt(*fd, SOL_SOCKET, SO_LINGER, &ling_opt, sizeof(ling_opt) ) < 0)
    {
        perror("network close: setsockopt fails: ");
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            status = idigi_callback_busy;
        }
    }
    if (close(*fd) < 0)
    {
        perror("network_close: close() failed");
    }

    *fd = -1;
    firmware_download_started = 0;

    return status;
}

static idigi_callback_status_t app_server_disconnected(void)
{

    APP_DEBUG("Disconnected from server\n");
//    delay_receive_flag = (delay_receive_flag == no_delay_receive) ? start_delay_receive : no_delay_receive;
    return idigi_callback_continue;
}

static idigi_callback_status_t app_server_reboot(void)
{

    APP_DEBUG("Reboot from server\n");
    /* should not return from rebooting the system */
//    delay_receive_flag = (delay_receive_flag == no_delay_receive) ? start_delay_receive : no_delay_receive;
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

