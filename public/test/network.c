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

#include "idigi_struct.h"
#include "idigi_data.h"


/*
 * e_dns_resolve
 *
 * Resolve a domain name to an IP address.
 *
 * The domain name is assumed to be a null-terminated ASCII string, a
 * pointer to which is passed as parameter domain_name. The IP address
 * for that domain name is returned in the data item pointed to by the
 * parameter ip_addr. The returned IP address is in network byte order
 * (big endian).
 *
 * The return value upon success is zero, with the IP address pointed to
 * by the parameter ip_addr. If the domain name cannot be resolved, the
 * return value is -1, and the value of the data item pointed to by the
 * parameter ip_addr is undefined.
 */
static int e_dns_resolve(char const *domain_name, in_addr_t * ip_addr)
{
    int rc;
    struct addrinfo *res0;
    struct addrinfo *res;
    struct addrinfo hint;

    /* Validate the caller's parameters. */
    if (domain_name == NULL || ip_addr == NULL) {
        return -1;
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_INET;
    rc = getaddrinfo(domain_name, NULL, &hint, &res0);
    if (rc != 0)
    {
        DEBUG_PRINTF("DNS resolution failed for %s\n", domain_name);
        return -1;  /* Resolv failed. */
    }
    rc = -1;    /* Assume failure. */
    /* Find the first IPv4 address, if any. */
    for (res = res0; res; res = res->ai_next)
    {
        if (res->ai_family == PF_INET)  /* v4 address? */
        {
        *ip_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
        rc = 0;     /* Success! */
        break;
        }
    }

    DEBUG_PRINTF("e_dns_resolve: ip address = %s\n", inet_ntoa(((struct sockaddr_in*)res->ai_addr)->sin_addr));

    freeaddrinfo(res0);
    if (rc < 0)
    {
        DEBUG_PRINTF("DNS resolution failed for %s\n", domain_name);
    }
    return rc;
}

static int open_socket(int *sock_fd, int type)
{
    int s;
    int enabled = 1;
    int ret = -1;

    /* Open a socket. */
    s = socket(AF_INET, type, 0);

    if (s >= 0)
    {

        if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&enabled, sizeof(enabled)) < 0)
        {
            DEBUG_PRINTF("open_socket: setsockopt (SO_KEEPALIVE) errno = %d\n", errno);
        }

        if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, sizeof(enabled)) < 0)
        {
            DEBUG_PRINTF("open_socket: setsockopt (TCP_NODELAY) errno = %d\n", errno);
        }

        *sock_fd = (uint32_t)s;
        ret = 0;
    }

    return ret;
}

static void set_socket_blockopt(unsigned sockfd, int block)
{
   int opt = (int)(block == 0);
    if (ioctl(sockfd, FIONBIO, &opt) < 0)
    {
        perror("ioctl: ");
        DEBUG_PRINTF("set_socket_blockopt: ioctl errno = %d\n", errno);
    }

}

static idigi_callback_status_t network_connect(char * host_name, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_abort;

    in_addr_t ip_addr;
    struct sockaddr_in sin;
    int s=-1;
    int ccode;

    fd_set set_rd;       /* readable socket descriptor set for selectsocket() */
    fd_set set_wr;       /* writable socket descriptor set for selectsocket() */
    struct timeval conn_to; /* timeout value for selectsocket() */

    if (iDigiSetting.socket_fd == -1)
    {
        /*
         * check if it's a dotted-notation IP address.
         * If it's a domain name, attempt to resolve it.
         */
        ip_addr = inet_addr(host_name);
        if (ip_addr == INADDR_NONE)
        {
            /* Resolve domain name */
            if (e_dns_resolve(host_name, &ip_addr) < 0)
            {
                DEBUG_PRINTF("network_connect: Can't resolve DNS for %s\n", host_name);
                /* Can't resolve it either */
                goto done;
            }
        }

        if (open_socket(&s, SOCK_STREAM) < 0)
        {
            goto done;
        }

        iDigiSetting.socket_fd = s;

        /* Make socket non-blocking */
        set_socket_blockopt(s, 0);

        /* try to connect */
        memset((char *)&sin, 0, sizeof(sin));
        memcpy(&sin.sin_addr, &ip_addr, sizeof sin.sin_addr);
        sin.sin_port = htons(IDIGI_PORT);
        sin.sin_family = AF_INET;
        ccode = connect(s, (struct sockaddr *)&sin, sizeof(sin));
        if (ccode < 0)
        {
            if (errno != EAGAIN && errno != EINPROGRESS)
            {
                perror("network_connect fails");
                goto done;
            }
        }
    }

    s = iDigiSetting.socket_fd;
    /*
     *
     * Wait for the connection initiated by the non-blocking connect()
     * to complete or time out.
     */

    conn_to.tv_sec = 1;
    conn_to.tv_usec = 0;
    FD_ZERO(&set_rd);
    FD_ZERO(&set_wr);
    FD_SET(s, &set_rd);
    FD_SET(s, &set_wr);
    ccode = select(s+1, &set_rd, &set_wr, 0, &conn_to);

    /* Did the select either time out or error out? */
    if (ccode <= 0)
    {
        /* An error or timeout occurred. */
        if (ccode < 0)
        {
            DEBUG_PRINTF("network_connect: select error\r\n");
        }
        else
        {
            rc = idigi_callback_busy;
            DEBUG_PRINTF("network_connect: select timeout\r\n");
        }
        goto done;
    }

    /* Check whether the socket is now writable (connection succeeded). */
    if (FD_ISSET(s, &set_wr))
    {
        /* We expect "socket writable" when the connection succeeds. */
        /* If we also got a "socket readable" we have an error. */
        if (FD_ISSET(s, &set_rd))
        {
            goto done;
        }

        /* We're connected! */
        *network_handle = &iDigiSetting.socket_fd;
        rc = idigi_callback_continue;
        DEBUG_PRINTF("network_connect: connected to %s server\n", host_name);
    }

done:
    if (rc == idigi_callback_abort && s >= 0)
    {
        close(s);
        iDigiSetting.socket_fd = -1;
    }
    return rc;
}

static idigi_callback_status_t network_send(idigi_write_request_t * write_data, size_t * sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int         ccode;


    ccode = send(*write_data->network_handle, (char *)write_data->buffer, write_data->length, 0);
    if (ccode < 0) {
        int err = errno;

        if (err == EAGAIN || err == EWOULDBLOCK)
        {
            iDigiSetting.select_data |= NETWORK_READ_SET;
            rc = idigi_callback_busy;
        }
        else
        {
            /* if not block (something's wrong),
             * let's abort it.
             */
            rc = idigi_callback_abort;
            perror("network send fails: ");
        }
    }
    *sent_length = ccode;
    if (ccode < (int)write_data->length)
    {
        iDigiSetting.select_data |= NETWORK_READ_SET;
    }
    else
    {
        iDigiSetting.select_data &= ~NETWORK_READ_SET;
    }
    /* Return success (the number of bytes sent). */
    return rc;
}

static idigi_callback_status_t network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int ccode;
    int err;


    ccode = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);

    if (ccode == 0)
    {
        /* EOF on input: the connection was closed. */
        DEBUG_PRINTF("network_receive: EOF on TCP socket read\r\n");
        rc = idigi_callback_abort;
    }
    else if (ccode < 0)
    {
        /* An error of some sort occurred: handle it appropriately. */
        err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK)
        {
            iDigiSetting.select_data |= NETWORK_WRITE_SET;
            rc = idigi_callback_busy;
        }
        else
        {
            perror("network receive fails: ");
            /* if not timeout (no data), let's return error */
            rc = idigi_callback_abort;
        }
    }

    *read_length = (size_t)ccode;
    if (ccode < (int)read_data->length)
    {
        iDigiSetting.select_data |= NETWORK_WRITE_SET;
    }
    else
    {
        iDigiSetting.select_data &= ~NETWORK_WRITE_SET;
    }

    return rc;
}


static idigi_callback_status_t network_close(idigi_network_handle_t * fd)
{
    idigi_callback_status_t status = idigi_callback_continue;
    struct linger ling_opt;

    ling_opt.l_linger = 1;
    ling_opt.l_onoff = 1;

    if (*fd != iDigiSetting.socket_fd)
    {
        DEBUG_PRINTF("network_close: mis-match network handle callback %d != local %d\n", *fd, iDigiSetting.socket_fd);
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
        perror("network close: close fails: ");
    }
    iDigiSetting.socket_fd = -1;


    return status;
}

uint8_t network_select(idigi_network_handle_t fd, uint8_t select_set, unsigned wait_time)
{
    uint8_t actual_set = 0;
    int             ccode;
    fd_set          read_set, write_set;
    struct timeval  * timeout_ptr = NULL;
    struct timeval  timeout;

    if (fd < 0||
        (select_set  & (NETWORK_READ_SET | NETWORK_WRITE_SET | NETWORK_TIMEOUT_SET)) == 0)
    {
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
    if (select_set & NETWORK_TIMEOUT_SET)
    {
        timeout.tv_sec = wait_time;
        timeout.tv_usec = 0;
        timeout_ptr = &timeout;
    }

    ccode = select(fd+1, &read_set, &write_set, NULL, timeout_ptr);
    if (ccode == 0)
    {
        /* timeout */
        actual_set |= NETWORK_TIMEOUT_SET;
    }
    else if (ccode > 0)
    {
        if (FD_ISSET(fd, &read_set))
        {
            DEBUG_PRINTF("network_select: Read ready\n");
            actual_set |= NETWORK_READ_SET;
        }
        if (FD_ISSET(fd, &write_set))
        {
            DEBUG_PRINTF("network_select: write ready\n");
            actual_set |= NETWORK_WRITE_SET;
        }
    }
done:
    return actual_set;
}


idigi_callback_status_t idigi_network_callback(idigi_network_request_t request,
                                            void * const request_data, size_t request_length,
                                            void * response_data, size_t * response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(request_length);

    switch (request)
    {
    case idigi_network_connect:
        status = network_connect((char *)request_data, (idigi_network_handle_t **)response_data);
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
    	DEBUG_PRINTF("Disconnected from server\n");
        /* set status to idk_callback_abort to abort iDigi or
         * idk_callback_continue to reconnect to server.
         */
        break;


    }

    return status;
}


