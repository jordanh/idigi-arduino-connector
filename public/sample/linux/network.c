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

#include "idigi_data.h"

static bool dns_resolve_name(char const * const domain_name, in_addr_t * ip_addr)
{
    bool rc=false;
    struct addrinfo *res0;
    struct addrinfo *res;
    struct addrinfo hint;
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
        DEBUG_PRINTF("dns_resolve_name: DNS resolution failed for [%s]\n", domain_name);
        goto done;
    }

    /* loop over all returned results and look for a V4 IP address */
    for (res = res0; res; res = res->ai_next)
    {
        if (res->ai_family == PF_INET)
        {
            *ip_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
            DEBUG_PRINTF("dns_resolve_name: ip address = [%s]\n", inet_ntoa(((struct sockaddr_in*)res->ai_addr)->sin_addr));
            rc = true;
            break;
        }
    }

    freeaddrinfo(res0);
done:
    return rc;
}

static bool open_socket(int *sock_fd)
{
    int s, ret = false, enabled = 1;

    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s >= 0)
    {

        if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&enabled, sizeof(enabled)) < 0)
        {
            perror("open_socket: setsockopt SO_KEEPALIVE failed");
        }

        if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, sizeof(enabled)) < 0)
        {
            perror("open_socket: setsockopt TCP_NODELAY failed");
        }

        *sock_fd = (uint32_t)s;
        ret = true;
    }
    else
    {
        perror("open_socket: could not open socket");
    }
    return ret;
}

/* 
 * 
 */ 
static bool set_blocking_socket(unsigned const sockfd, bool const block)
{
    int opt = (int)(block == false);
    bool ret = true;

    if (ioctl(sockfd, FIONBIO, &opt) < 0)
    {
        perror("ioctl: FIONBIO failed");
        ret = false;
    }

    return ret;
}


static idigi_callback_status_t network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_abort;
    int fd=device_data.socket_fd;
    uint8_t actual_set;
    struct timeval timeout = {1, 0};

    if (fd == -1)
    {
        int ccode;
        char server_name[length+1];
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
                DEBUG_PRINTF("network_connect: Can't resolve DNS for %s\n", server_name);
                goto done;
            }
        }

        if (!open_socket(&fd))
        {
            goto done;
        }

        device_data.socket_fd = fd;

        if (!set_blocking_socket(fd, false))
        {
            goto done;
        }

        memset((char *)&sin, 0, sizeof(sin));
        memcpy(&sin.sin_addr, &ip_addr, sizeof sin.sin_addr);
        sin.sin_port   = htons(IDIGI_PORT);
        sin.sin_family = AF_INET;
        ccode = connect(fd, (struct sockaddr *)&sin, sizeof(sin));
        if (ccode < 0)
        {
            if (errno != EAGAIN && errno != EINPROGRESS)
            {
                perror("network_connect: connect() failed");
                goto done;
            }
        }
    }

    /*
     * Wait for the connection initiated by the non-blocking connect()
     * to complete or time out.
     */
    actual_set = network_select(fd, NETWORK_TIMEOUT_SET | NETWORK_WRITE_SET | NETWORK_READ_SET, &timeout);

    /* Did the select either time out or error out? */
    if (actual_set == 0)
    {
        perror("network_connect: select error");
        goto done;
    }
    else if (actual_set & NETWORK_TIMEOUT_SET)
    {
        rc = idigi_callback_busy;
        DEBUG_PRINTF("network_connect: select timeout\r\n");
        perror("network_connect: select");
        goto done;
    }

    /* Check whether the socket is now writable (connection succeeded). */
    if (actual_set & NETWORK_WRITE_SET)
    {
        /* We expect "socket writable" when the connection succeeds. */
        /* If we also got a "socket readable" we have an error. */
        if (actual_set & NETWORK_READ_SET)
        {
            goto done;
        }

        *network_handle = &device_data.socket_fd;
        rc = idigi_callback_continue;
        DEBUG_PRINTF("network_connect: connected to [%.*s] server\n", length, host_name);
        device_data.connected = true;
    }

done:
    if ((rc == idigi_callback_abort) && (fd >= 0))
    {
        close(fd);
        device_data.socket_fd = -1;
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
            device_data.select_data |= NETWORK_READ_SET;
            rc = idigi_callback_busy;
        }
        else
        {
            rc = idigi_callback_abort;
            perror("network_send: send() failed");
        }
    }
    *sent_length = ccode;
    if (ccode < (int)write_data->length)
    {
        device_data.select_data |= NETWORK_READ_SET;
    }
    else
    {
        device_data.select_data &= ~NETWORK_READ_SET;
    }

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
    int ccode, err;

    ccode = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);

    if (ccode == 0)
    {
        /* EOF on input: the connection was closed. */
        DEBUG_PRINTF("network_receive: EOF on socket\r\n");
        rc = idigi_callback_abort;
    }
    else if (ccode < 0)
    {
        /* An error of some sort occurred: handle it appropriately. */
        err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK)
        {
            device_data.select_data |= NETWORK_WRITE_SET;
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
    if (ccode < (int)read_data->length)
    {
        device_data.select_data |= NETWORK_WRITE_SET;
    }
    else
    {
        device_data.select_data &= ~NETWORK_WRITE_SET;
    }

    return rc;
}


static idigi_callback_status_t network_close(idigi_network_handle_t * const fd)
{
    idigi_callback_status_t status = idigi_callback_continue;
    struct linger ling_opt;

    ling_opt.l_linger = 1;
    ling_opt.l_onoff  = 1;

    if (*fd != device_data.socket_fd)
    {
        DEBUG_PRINTF("network_close: mismatch on network handles: callback [%d] != local [%d]\n", *fd, device_data.socket_fd);
        ASSERT(false);
    }

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
    device_data.socket_fd = -1;

    return status;
}

static bool server_disconnected(void)
{

    DEBUG_PRINTF("Disconnected from server\n");
    /* socket should be already closed. */
    ASSERT(device_data.socket_fd == -1);
    return true;
}

static bool server_reboot(void)
{

    DEBUG_PRINTF("Reboot from server\n");
    /* should not return from rebooting the system */
    return true;
}
uint8_t network_select(idigi_network_handle_t fd, uint8_t select_set, struct timeval *timeout)
{
    uint8_t actual_set = 0;
    int     ccode;
    fd_set  read_set, write_set;
    
    if ((fd < 0) || ((select_set & (NETWORK_READ_SET | NETWORK_WRITE_SET | NETWORK_TIMEOUT_SET)) == 0))
    {
        DEBUG_PRINTF("network_select: Invalid arguments\n");
        goto done;
    }

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    if (select_set & NETWORK_READ_SET)
    {
        FD_SET(fd, &read_set);
    }
    if (select_set & NETWORK_WRITE_SET)
    {
        FD_SET(fd, &write_set);
    }

    ccode = select(fd+1, &read_set, &write_set, NULL, timeout);
    if (ccode == 0)
    {
        actual_set |= NETWORK_TIMEOUT_SET; /* timeout */
    }
    else if (ccode > 0)
    {
        if (FD_ISSET(fd, &read_set))
        {
            actual_set |= NETWORK_READ_SET;
        }
        if (FD_ISSET(fd, &write_set))
        {
            actual_set |= NETWORK_WRITE_SET;
        }
    }
done:
    return actual_set;
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
        DEBUG_PRINTF("idigi_network_callback: unrecognized callback request [%d]\n", request);
        break;

    }

    return status;
}


