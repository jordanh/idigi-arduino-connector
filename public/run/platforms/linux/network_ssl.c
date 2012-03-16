/*
 *  Copyright (c) 2011-2012 Digi International Inc., All Rights Reserved
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
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "idigi_api.h"
#include "platform.h"

typedef struct
{
    int sfd;
    SSL_CTX * ctx;
    SSL * ssl;
} app_ssl_t;

static int app_dns_resolve_name(char const * const domain_name, in_addr_t * ip_addr)
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

    /* loop over all returned results and look for a V4 IP address */
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

static int app_setup_server_socket(void)
{
    int const protocol = 0;
    int sd = socket(AF_INET, SOCK_STREAM, protocol);

    if (sd < 0)
        goto done;

    {
        int enabled = 1;

        if (setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (char *)&enabled, sizeof enabled) < 0)
        {
            perror("open_socket: setsockopt SO_KEEPALIVE failed");
            goto error;
        }

        enabled = 1;
        if (setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (char *)&enabled, sizeof enabled) < 0)
        {
            perror("open_socket: setsockopt TCP_NODELAY failed");
            goto error;
        }
    }

    goto done;

error:
    close(sd);
    sd = -1;

done:
    return sd;
}

static int app_connect_to_server(int fd, char const * const host_name, size_t length)
{
    int ret = -1;
    in_addr_t ip_addr;

    {
        char server_name[64];

        if (length >= asizeof(server_name))
        {
            APP_DEBUG("app_connect_to_server: server name length [%d]\n", length);
            goto error;
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
                goto error;
            }
        }
    }

    {
        struct sockaddr_in sin = {0};

        memcpy(&sin.sin_addr, &ip_addr, sizeof sin.sin_addr);
        sin.sin_port   = htons(IDIGI_SSL_PORT);
        sin.sin_family = AF_INET;
        ret = connect(fd, (struct sockaddr *)&sin, sizeof sin);
        if (ret < 0)
        {
            switch (errno) 
            {
            case EAGAIN:
            case EINPROGRESS:
                ret = 0;
                break;

            default:
                perror("network_connect: connect() failed");
                break;
            }
        }
    }

error:
    return ret;
}

static int app_is_connect_complete(int fd)
{
    int ret = -1;
    struct timeval timeout = {2, 0};
    fd_set read_set;
    fd_set write_set;

    FD_ZERO(&read_set);
    FD_SET(fd, &read_set);
    write_set = read_set;

    /* wait for 2 seconds to connect */
    if (select(fd+1, &read_set, &write_set, NULL, &timeout) <= 0)
        goto error;

    /* Check whether the socket is now writable (connection succeeded). */
    if (FD_ISSET(fd, &write_set))
    {
        /* We expect "socket writable" when the connection succeeds. */
        /* If we also got a "socket readable" we have an error. */
        if (FD_ISSET(fd, &read_set))
            goto error;

        ret = 0;
    }

error:
    return ret;
}

#if (defined APP_SSL_CLNT_CERT)
static int get_user_passwd(char * buf, int size, int rwflag, void * password)
{
  char const passwd[] = APP_SSL_CLNT_CERT_PASSWORD;
  int const pwd_bytes = asizeof(passwd) - 1;
  int const copy_bytes = (pwd_bytes < size) ? pwd_bytes : size-1;

  UNUSED_ARGUMENT(rwflag);
  UNUSED_ARGUMENT(password);

  ASSERT_GOTO(copy_bytes >= 0, error);
  memcpy(buf, passwd, copy_bytes);
  buf[copy_bytes] = '\0';

error:
  return copy_bytes;
}
#endif

static int app_load_certificate_and_key(SSL_CTX * const ctx)
{
    int ret = -1;

    {
        ret = SSL_CTX_load_verify_locations(ctx, APP_SSL_CA_CERT, NULL);
        if (ret != 1) 
        {
            APP_DEBUG("Failed to load CA cert %d\n", ret);
            ERR_print_errors_fp(stderr);
            goto error;
        }
    }

    #if (defined APP_SSL_CLNT_CERT)
    SSL_CTX_set_default_passwd_cb(ctx, get_user_passwd);
    ret = SSL_CTX_use_certificate_file(ctx, APP_SSL_CLNT_KEY, SSL_FILETYPE_PEM);
    if (ret != 1)
    {
        APP_DEBUG("SSL_use_certificate_file() Error [%d]\n", ret);
        goto error;
    }

    ret = SSL_CTX_use_RSAPrivateKey_file(ctx, APP_SSL_CLNT_CERT, SSL_FILETYPE_PEM);
    if (ret != 1)
    {
        APP_DEBUG("SSL_use_RSAPrivateKey_file() Error [%d]\n", ret);
        goto error;
    }
    #endif

error:
    return ret;
}

static void app_free_ssl_info(app_ssl_t * const ssl_ptr)
{
    if (ssl_ptr->ssl != NULL) 
    {
        SSL_free(ssl_ptr->ssl);
        ssl_ptr->ssl = NULL;
    }

    if (ssl_ptr->ctx != NULL) 
    {
        SSL_CTX_free(ssl_ptr->ctx);
        ssl_ptr->ctx = NULL;
    }

    if (ssl_ptr->sfd != -1) 
    {
        close(ssl_ptr->sfd);
        ssl_ptr->sfd = -1;
    }
}

static int app_verify_server_certificate(SSL * const ssl)
{
    int ret = -1;
    X509 * const server_cert = SSL_get_peer_certificate(ssl);

    if (server_cert == NULL)
    {
        APP_DEBUG("app_verify_server_certificate: No server certificate is provided\n");
        goto done;
    }

    ret = SSL_get_verify_result(ssl);
    if (ret !=  X509_V_OK)
    {
        APP_DEBUG("Server certificate is invalid %d\n", ret);
        goto done;
    }

done:
    return ret;
}

static int app_ssl_connect(app_ssl_t * const ssl_ptr)
{
    int ret = -1;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ssl_ptr->ctx = SSL_CTX_new(TLSv1_client_method());    
    if (ssl_ptr->ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        goto error;
    } 

    ssl_ptr->ssl = SSL_new(ssl_ptr->ctx);
    if (ssl_ptr->ssl == NULL)
    {
        ERR_print_errors_fp(stderr);
        goto error;
    }

    SSL_set_fd(ssl_ptr->ssl, ssl_ptr->sfd);
    if (app_load_certificate_and_key(ssl_ptr->ctx) != 1)
        goto error;

    SSL_set_options(ssl_ptr->ssl, SSL_OP_ALL);
    if (SSL_connect(ssl_ptr->ssl) <= 0)
    {
        ERR_print_errors_fp(stderr);
        goto error;
    }

    if (app_verify_server_certificate(ssl_ptr->ssl) != X509_V_OK) 
        goto error;

    ret = 0;

error:
    return ret;
}

static idigi_callback_status_t app_network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t status = idigi_callback_abort;
    static app_ssl_t ssl_info = {-1, NULL, NULL};

    ssl_info.sfd = app_setup_server_socket();
    if (ssl_info.sfd < 0)
    {
        perror("Could not open socket");
        goto done;
    }

    if (app_connect_to_server(ssl_info.sfd, host_name, length) < 0)
    {
        APP_DEBUG("Failed to connect to %s\n", host_name);
        goto error;
    }
    
    if (app_is_connect_complete(ssl_info.sfd) < 0)
        goto error;

    if (app_ssl_connect(&ssl_info) < 0)
        goto error;

    /* make it non-blocking now */
    {
        int enabled = 1;

        if (ioctl(ssl_info.sfd, FIONBIO, &enabled) < 0)
        {
            perror("ioctl: FIONBIO failed");
            goto error;
        }
    }

    APP_DEBUG("network_connect: connected to [%.*s] server\n", (int)length, host_name);
    *network_handle = (idigi_network_handle_t *)&ssl_info;
    status = idigi_callback_continue;
    goto done;

error:
    APP_DEBUG("network_connect: error to connect to %.*s server\n", (int)length, host_name);
    app_free_ssl_info(&ssl_info);

done:
    return status;
}

/*
 * Send data to the iDigi server, this routine must not block.
 */
static idigi_callback_status_t app_network_send(idigi_write_request_t const * const write_data,
                                            size_t * sent_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_ssl_t * const ssl_ptr = (app_ssl_t *)write_data->network_handle;
    int bytes_sent = 0;

    bytes_sent = SSL_write(ssl_ptr->ssl, write_data->buffer, write_data->length);
    if (bytes_sent <= 0)
    {
        APP_DEBUG("SSL_write failed %zu\n", bytes_sent);
        status = idigi_callback_abort;
    }

    *sent_length = bytes_sent;
    return status;
}

/*
 * This routine reads a specified number of bytes from the iDigi server.
 */
static idigi_callback_status_t app_network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_ssl_t * const ssl_ptr = (app_ssl_t *)read_data->network_handle;
    int bytes_read = 0;

    *read_length = 0;
    if (SSL_pending(ssl_ptr->ssl) == 0)
    {
        int ready;
        struct timeval timeout;
        fd_set read_set;

        timeout.tv_sec = read_data->timeout;
        timeout.tv_usec = 0;

        FD_ZERO(&read_set);
        FD_SET(ssl_ptr->sfd, &read_set);

        /* Blocking point for IIK */
        ready = select(ssl_ptr->sfd + 1, &read_set, NULL, NULL, &timeout);
        if (ready == 0)
        {
            status = idigi_callback_busy;
            goto done;
        }

        if (ready < 0)
        {
           APP_DEBUG("app_network_receive: select failed\n");
           goto done;
        }
    }

    bytes_read = SSL_read(ssl_ptr->ssl, read_data->buffer, (int)read_data->length);
    if (bytes_read <= 0)
    {
        /* EOF on input: the connection was closed. */
        APP_DEBUG("SSL_read failed %d\n", bytes_read);
        status = idigi_callback_abort;
    }

    *read_length = (size_t)bytes_read;

done:
    return status;
}

static idigi_callback_status_t app_network_close(idigi_network_handle_t * const handle)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_ssl_t * const ssl_ptr = (app_ssl_t *)handle;

    /* send close notify to peer */
    if (SSL_shutdown(ssl_ptr->ssl) == 0) 
        SSL_shutdown(ssl_ptr->ssl);  /* wait for peer's close notify */

    app_free_ssl_info(ssl_ptr);
    return status;
}

static int app_server_disconnected(void)
{

    APP_DEBUG("Disconnected from server\n");
    return 0;
}

static int app_server_reboot(void)
{

    APP_DEBUG("Reboot from server\n");
    /* should not return from rebooting the system */
    return 0;
}

/*
 *  Callback routine to handle all networking related calls.
 */
idigi_callback_status_t app_network_handler(idigi_network_request_t const request,
                                            void * const request_data, size_t const request_length,
                                            void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int ret;

    UNUSED_ARGUMENT(request_length);

    switch (request)
    {
    case idigi_network_connect:
        status = app_network_connect((char *)request_data, request_length, (idigi_network_handle_t **)response_data);
        *response_length = sizeof(idigi_network_handle_t);
        break;

    case idigi_network_send:
        status = app_network_send((idigi_write_request_t *)request_data, (size_t *)response_data);
        break;

    case idigi_network_receive:
        status = app_network_receive((idigi_read_request_t *)request_data, (size_t *)response_data);
        break;

    case idigi_network_close:
        status = app_network_close((idigi_network_handle_t *)request_data);
        break;

    case idigi_network_disconnected:
       ret = app_server_disconnected();
       status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
       break;

    case idigi_network_reboot:
        ret = app_server_reboot();
        status = (ret == 0) ? idigi_callback_continue : idigi_callback_abort;
        break;

    default:
        APP_DEBUG("idigi_network_callback: unrecognized callback request [%d]\n", request);
        break;
    }

    return status;
}

