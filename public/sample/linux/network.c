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
int e_dns_resolve(char *domain_name, struct in_addr * ip_addr)
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
#if 1
    rc = -1;    /* Assume failure. */
    /* Find the first IPv4 address, if any. */
    for (res = res0; res; res = res->ai_next)
    {
        if (res->ai_family == PF_INET)  /* v4 address? */
        {
        ip_addr->s_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
        rc = 0;     /* Success! */
        break;
        }
    }
#else

    ip_addr->s_addr = ((struct sockaddr_in *)(res0->ai_addr))->sin_addr.s_addr;

#endif

    DEBUG_PRINTF("e_dns_resolve: ip address = %s\n", inet_ntoa(*ip_addr));

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

void set_socket_blockopt(unsigned sockfd, int block)
{
   int opt = (int)(block == 0);
    if (ioctl(sockfd, FIONBIO, &opt) < 0)
    {
        perror("ioctl: ");
        DEBUG_PRINTF("set_socket_blockopt: ioctl errno = %d\n", errno);
    }

}

idk_callback_status_t network_connect(idk_connect_request_t * connect_data)
{
//#error "Add code to make connection to server";
    idk_callback_status_t rc = idk_callback_abort;

    struct in_addr ip_addr;
    struct sockaddr_in sin;
    int s=-1;
    int ccode;

    fd_set set_rd;       /* readable socket descriptor set for selectsocket() */
    fd_set set_wr;       /* writable socket descriptor set for selectsocket() */
    struct timeval conn_to; /* timeout value for selectsocket() */

    if (iDigiSetting.socket_fd == -1)
    {
        /*
         * Do I need to check a dotted-notation IP address or a domain
         * name. If it's a domain name, attempt to resolve it.
         */
        ip_addr.s_addr = inet_addr(connect_data->host_name);
        if (ip_addr.s_addr == -1)
        {
            if (e_dns_resolve(connect_data->host_name, &ip_addr) < 0)
            {
                DEBUG_PRINTF("network_connect: Can't resolve DNS for %s\n", connect_data->host_name);
                /* Can't resolve it either. Too bad. */
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
        sin.sin_port = htons(connect_data->port);
        sin.sin_family = AF_INET;
        ccode = connect(s, (struct sockaddr *)&sin, sizeof(sin));
        if (ccode < 0)
        {
            if (errno != EAGAIN && errno != EINPROGRESS)
            {
                perror("network_connect fails");
                DEBUG_PRINTF("network_connect: errno=%d\n", errno);
                /* We must wait for the connect to complete or time out. */
                goto done;
            }
        }
    }

    s = iDigiSetting.socket_fd;
    /*
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
            rc = idk_callback_busy;
            DEBUG_PRINTF("network_connect: select timeout\r\n");
        }
        /* restore socket block flag */
        goto done;
    }

    /* Check whether the socket is now writable (connection succeeded). */
    if (FD_ISSET(s, &set_wr))
    {
        /* We expect "socket writable" when the connection succeeds. */
        /* If we also got a "socket readable" we have an error. */
        if (FD_ISSET(s, &set_rd))
        {
            /* restore socket block flag */
            goto done;
        }

        /* We're connected! */
        rc = idk_callback_continue;
        DEBUG_PRINTF("network_connect: connected to %s server\n", connect_data->host_name);
    }

done:
    if (rc == idk_callback_abort && s >= 0)
    {
        close(s);
        iDigiSetting.socket_fd = -1;
    }
    return rc;
}

idk_callback_status_t network_send(idk_write_request_t * write_data, size_t * sent_length)
{
//#error "Add code to send data to server";
    idk_callback_status_t rc = idk_callback_continue;
    int         ccode;


    ccode = send(*write_data->network_handle, (char *)write_data->buffer, write_data->length, 0);
    if (ccode < 0) {
        int err = errno;

        perror("hal_send: ");
        if (err == EAGAIN || err == EWOULDBLOCK)
        {
            iDigiSetting.select_data |= NETWORK_READ_SET;
            rc = idk_callback_busy;
        }
        else
        {
            /* if not block (something's wrong),
             * let's abort it.
             */
            rc = idk_callback_abort;
            DEBUG_PRINTF("network_send: send returns errno %d\n", err);
        }
    }
    *sent_length = ccode;
    if (ccode < write_data->length)
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

idk_callback_status_t network_receive(idk_read_request_t * read_data, size_t * read_length)
{
//#error "Add code to receive data from server"
    idk_callback_status_t rc = idk_callback_continue;
    int ccode;
    int err;


    ccode = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);

    if (ccode == 0)
    {
        /* EOF on input: the connection was closed. */
        DEBUG_PRINTF("network_receive: EOF on TCP socket read\r\n");
        rc = idk_callback_abort;
    }
    else if (ccode < 0)
    {
        /* An error of some sort occurred: handle it appropriately. */
        err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK)
        {
            iDigiSetting.select_data |= NETWORK_WRITE_SET;
            rc = idk_callback_busy;
        }
        else
        {
            perror("hal_receive: ");
            /* if not timeout (no data), let's return error */
            rc = idk_callback_abort;
            /* The error was other than a timeout: indicate a socket error. */
            DEBUG_PRINTF("network_receive: Error %d on TCP socket read\r\n", err);
        }
    }

    *read_length = (size_t)ccode;
    if (ccode < read_data->length)
    {
        iDigiSetting.select_data |= NETWORK_WRITE_SET;
    }
    else
    {
        iDigiSetting.select_data &= ~NETWORK_WRITE_SET;
    }

    return rc;
}


idk_callback_status_t network_close(idk_network_handle_t * fd)
{
//#error "Add code to close connection";
    idk_callback_status_t status = idk_callback_continue;
    struct linger ling_opt;

    ling_opt.l_linger = 1;
    ling_opt.l_onoff = 1;

    if (*fd != iDigiSetting.socket_fd)
    {
        DEBUG_PRINTF("network_close: mis-match network handle callback %d != local %d\n", *fd, iDigiSetting.socket_fd);
    }

    if (setsockopt(*fd, SOL_SOCKET, SO_LINGER, (char*)&ling_opt, sizeof(ling_opt) ) < 0)
    {
        perror("network_close: setsockopt fails: ");
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            status = idk_callback_busy;
        }
    }
    if (close(*fd) < 0)
    {
        perror("network_close: close fails: ");
    }
    iDigiSetting.socket_fd = -1;


    return status;
}

uint8_t network_select(idk_network_handle_t fd, uint8_t select_set, unsigned wait_time)
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


