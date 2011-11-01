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
 * Rountines which implement the IIK network callbacks.
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

static bool netowrk_initialization_complete = false;

static bool dns_resolve_name(char const * const domain_name, in_addr_t * ip_addr)
{
    bool rc=false;
    struct addrinfo *res0, *res, hint;
    int error;

    if ((domain_name == NULL) || (ip_addr == NULL))
    {
        goto done;
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family   = AF_INET;
    error = getaddrinfo(domain_name, NULL, &hint, &res0);
    if (error != 0)
    {
        APP_DEBUG("dns_resolve_name: DNS resolution failed for [%s]\n", domain_name);
        goto done;
    }

    /* loop over all returned results and look for a V4 IP address */
    for (res = res0; res; res = res->ai_next)
    {
        if (res->ai_family == PF_INET)
        {
            *ip_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
            APP_DEBUG("dns_resolve_name: ip address = [%s]\n", inet_ntoa(((struct sockaddr_in*)res->ai_addr)->sin_addr));
            rc = true;
            break;
        }
    }

    freeaddrinfo(res0);
done:
    return rc;
}

static idigi_callback_status_t network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_abort;
    struct timeval timeout = {1, 0};
    int opt=1, result;
    fd_set read_set;
    fd_set write_set;
    static int fd = -1;

    if (fd == -1)
    {
        int ccode;
        char server_name[64];
        in_addr_t ip_addr;
        struct sockaddr_in sin;

        strncpy(server_name, host_name, length);
        server_name[length] = '\0';

        /*
         * Check if it's a dotted-notation IP address, if it's a domain name,
         * attempt to resolve it.
         */
        ip_addr = inet_addr(server_name);
        if (ip_addr == INADDR_NONE)
        {
            if (!dns_resolve_name(server_name, &ip_addr))
            {
                APP_DEBUG("network_connect: Can't resolve DNS for %s\n", server_name);
                goto done;
            }
        }

        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd >= 0)
        {
            int enabled = 1;

            if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&enabled, sizeof(enabled)) < 0)
            {
                perror("open_socket: setsockopt SO_KEEPALIVE failed");
            }

            if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, sizeof(enabled)) < 0)
            {
                perror("open_socket: setsockopt TCP_NODELAY failed");
            }
        }
        else
        {
            perror("Could not open socket");
            goto done;
        }

        if (ioctl(fd, FIONBIO, &opt) < 0)
        {
            perror("ioctl: FIONBIO failed");
            goto done;
        }

        memset((char *)&sin, 0, sizeof(sin));
        memcpy(&sin.sin_addr, &ip_addr, sizeof sin.sin_addr);
        sin.sin_port   = htons(IDIGI_PORT);
        sin.sin_family = AF_INET;
        ccode = connect(fd, (struct sockaddr *)&sin, sizeof(sin));
        if (ccode < 0)
        {
            if (errno != EAGAIN && errno != EINPROGRESS && errno != EWOULDBLOCK)
            {
                perror("network_connect: connect() failed");
                goto done;
            }
        }
    }

    FD_ZERO(&read_set);
    FD_SET(fd, &read_set);
    write_set = read_set;

    result = select(fd+1, &read_set, &write_set, NULL, &timeout);
    if (result < 0)
    {
        goto done;
    }

    if (result == 0)
    {
        rc = idigi_callback_busy;
        APP_DEBUG("network_connect: select timeout\r\n");
        perror("network_connect: select");
        goto done;
    }

    /* Check whether the socket is now writable (connection succeeded). */
    if (FD_ISSET(fd, &write_set))
    {
        /* We expect "socket writable" when the connection succeeds. */
        /* If we also got a "socket readable" we have an error. */
        if (FD_ISSET(fd, &read_set))
        {
            APP_DEBUG("network_connect: error to connect to %.*s server\n", length, host_name);
            goto done;
        }
        *network_handle = &fd;
        rc = idigi_callback_continue;
        APP_DEBUG("network_connect: connected to [%.*s] server\n", length, host_name);
    }

done:
    if ((rc == idigi_callback_abort) && (fd >= 0))
    {
        close(fd);
        fd = -1;
    }

    return rc;
}

/*
 * Send data to the iDigi server, this routine must not block.  If it encounters
 * EAGAIN or EWOULDBLOCK error, 0 bytes must be returned and IIK will continue
 * calling this function.
 */
static idigi_callback_status_t network_send(idigi_write_request_t const * const write_data,
                                            size_t * sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int ccode;

    ccode = send(*write_data->network_handle, (char *)write_data->buffer,
                 write_data->length, 0);
    if (ccode < 0) 
    {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
        {
            rc = idigi_callback_busy;
        }
        else
        {
            rc = idigi_callback_abort;
            perror("network_send: send() failed");
        }
    }
    *sent_length = ccode;

    return rc;
}

/*
 * This routine reads a specified number of bytes from the iDigi server.  This 
 * function must not block. If it encounters EAGAIN or EWOULDBLOCK error, 0 
 * bytes must be returned and IIK will continue calling this function.
 */
static idigi_callback_status_t network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    struct timeval timeout;
    fd_set read_set;
    int ccode, err;

    timeout.tv_sec = read_data->timeout;
    timeout.tv_usec = 0;

    *read_length = 0;

    FD_ZERO(&read_set);
    FD_SET(*read_data->network_handle, &read_set);

    /* Blocking point for IIK */
    ccode = select(*read_data->network_handle+1, &read_set, NULL, NULL, &timeout);
    if (ccode < 0)
    {
        goto done;
    }

    if (!FD_ISSET(*read_data->network_handle, &read_set))
    {
        rc = idigi_callback_busy;
        goto done;
    }
    ccode = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);

    if (ccode == 0)
    {
        /* EOF on input: the connection was closed. */
        APP_DEBUG("network_receive: EOF on socket\r\n");
        rc = idigi_callback_abort;
    }
    else if (ccode < 0)
    {
        /* An error of some sort occurred: handle it appropriately. */
        err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK)
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

static idigi_callback_status_t network_close(idigi_network_handle_t * const fd)
{
    idigi_callback_status_t status = idigi_callback_continue;
    struct linger ling_opt;

    ling_opt.l_linger = 1;
    ling_opt.l_onoff  = 1;

    if (setsockopt(*fd, SOL_SOCKET, SO_LINGER, (char*)&ling_opt, sizeof(ling_opt) ) < 0)
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

    return status;
}

static bool server_disconnected(void)
{

    APP_DEBUG("Disconnected from server\n");
    return true;
}

static bool server_reboot(void)
{

    APP_DEBUG("Reboot from server\n");
    /* should not return from rebooting the system */
    return true;
}

/*
 *  Callback routine to handle all networking related calls.
 */
idigi_callback_status_t idigi_network_callback(idigi_network_request_t const request,
                                            void * const request_data, size_t const request_length,
                                            void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    bool ret;

    UNUSED_PARAMETER(request_length);

    switch (request)
    {
    case idigi_network_connect:
        status = network_connect((char *)request_data, request_length, (idigi_network_handle_t **)response_data);
        *response_length = sizeof(idigi_network_handle_t);
        break;

    case idigi_network_send:
        status = network_send((idigi_write_request_t *)request_data, (size_t *)response_data);
        break;

    case idigi_network_receive:
        status = network_receive((idigi_read_request_t *)request_data, (size_t *)response_data);
        break;

    case idigi_network_close:
        status = network_close((idigi_network_handle_t *)request_data);
        break;

    case idigi_network_disconnected:
       ret = server_disconnected();
       status = (ret == true) ? idigi_callback_continue : idigi_callback_abort;
       break;

    case idigi_network_reboot:
        ret = server_reboot();
        status = (ret == true) ? idigi_callback_continue : idigi_callback_abort;
        break;

    default:
        APP_DEBUG("idigi_network_callback: unrecognized callback request [%d]\n", request);
        break;

    }

    return status;
}

bool is_initialization_complete(void)
{
    return netowrk_initialization_complete;
}
