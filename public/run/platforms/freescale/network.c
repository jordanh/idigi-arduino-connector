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
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <ipcfg.h>
#include <errno.h>
#include <idigi_api.h>
#include <idigi_types.h>
#include <platform.h>

#define SOCKET_BUFFER_SIZE              512
#define SOCKET_TIMEOUT_MSEC             1000

static int socket_fd = RTCS_SOCKET_ERROR;
    
static int dns_resolve_name(char const * const name, _ip_address * ip_addr)
{
    char tries = 0;
    boolean result;
    
    APP_DEBUG("dns_resolve_name: DNS Address : %d.%d.%d.%d\n",
    		   IPBYTES(ipcfg_get_dns_ip(IPCFG_default_enet_device,0)));

    /* Try three times to get name */
    while(tries < 3)
    {
        APP_DEBUG("dns_resolve_name: %s\n", name);

        result = RTCS_resolve_ip_address( (char_ptr)name, ip_addr, NULL, 0);
        if (result == TRUE)
        {
            return 1;
        }
        else
        {
            APP_DEBUG("Failed - name not resolved\n");
        }
        tries++;
        _time_delay(1000);
    }

    return 0;
}

static boolean set_blocking_socket(int fd, boolean const block)
{
    uint_32  error;
    uint_32  option;

    option = (block == FALSE);
    error = setsockopt(fd, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
    if(error != RTCS_OK)
    {
        APP_DEBUG("open_socket: setsockopt OPT_RECEIVE_NOWAIT failed");
        return FALSE;
    }
    return TRUE;
}

static idigi_callback_status_t app_network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_abort;
    sockaddr_in addr;
    int opt=1, result;
    unsigned int option;
    unsigned long error;

    if (socket_fd == RTCS_SOCKET_ERROR)
    {
        int ccode;
        _ip_address ip_addr;
        struct sockaddr_in sin;
        char *ip_address = (char *)&ip_addr;
        
        if (!dns_resolve_name(host_name, &ip_addr))
        {
            APP_DEBUG("network_connect: Error resolving IP address for %s using DNS Server at %d.%d.%d.%d\n\n",
            		   host_name, IPBYTES(ipcfg_get_dns_ip(IPCFG_default_enet_device,0)));
            goto done;
        }
        
        APP_DEBUG("network_connect: Resolved IP Address: %d.%d.%d.%d\n\n", ip_address[3], ip_address[2], ip_address[1], ip_address[0]);

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd >= 0)
        {
            int enabled = 1;

            if (!set_blocking_socket(socket_fd, FALSE))
            {
                APP_DEBUG("network_connect: set non-blocking failed\n");
                goto done;
            }

            /* Reduce buffer size of socket to save memory */
            option = SOCKET_BUFFER_SIZE;
            if (setsockopt(socket_fd, SOL_TCP, OPT_TBSIZE, &option, sizeof(option)) != RTCS_OK)
            {
                APP_DEBUG("network_connect: setsockopt OPT_TBSIZE failed\n");
                goto done;
            }
            
            if (setsockopt(socket_fd, SOL_TCP, OPT_RBSIZE, &option, sizeof(option)) != RTCS_OK)
            {
                APP_DEBUG("network_connect: setsockopt OPT_RBSIZE failed\n");
                goto done;
            }

            /* set a socket timeout */
            option = SOCKET_TIMEOUT_MSEC;
            if (setsockopt(socket_fd, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option)) != RTCS_OK)
            {
                APP_DEBUG("network_connect: setsockopt OPT_TIMEWAIT_TIMEOUT failed\n");
                goto done;
            }

            if (setsockopt(socket_fd, SOL_SOCKET, OPT_KEEPALIVE, (char*)&enabled, sizeof(enabled)) < 0)
            {
            	APP_DEBUG("network_connect: setsockopt OPT_KEEPALIVE failed\n");
                goto done;
            }
        }
        else
        {
        	APP_DEBUG("Could not open socket\n");
            goto done;
        }

        memset((char *)&addr, 0, sizeof(addr));

        /* Allow binding to any address */
        addr.sin_family      = AF_INET;
        addr.sin_port        = 0;
        addr.sin_addr.s_addr = INADDR_ANY;
        error = bind(socket_fd, &addr, sizeof(addr));
        if (error != RTCS_OK)
        {
        	APP_DEBUG("Error in binding socket %08x\n",error);
            goto done;
        }
      
        APP_DEBUG("Connecting to %s...\n", host_name); 

        /* Connect to device */
        addr.sin_port        = IDIGI_PORT;
        addr.sin_addr.s_addr = ip_addr;
        error = connect(socket_fd, &addr, sizeof(addr));
        if (error != RTCS_OK)
        {
            if (errno != EAGAIN && errno != MQX_EINPROGRESS)
            {
                APP_DEBUG("network_connect: connect() failed %08x\n",error);
                goto done;
            }
        }
    }

    *network_handle = &socket_fd;
    rc = idigi_callback_continue;
    APP_DEBUG("network_connect: connected to [%.*s] server\n", length, host_name);

done:
    if ((rc == idigi_callback_abort) && (socket_fd >= 0))
    {
    	/* Close socket */
        shutdown(socket_fd, FLAG_ABORT_CONNECTION);
        socket_fd = -1;
    }

    return rc;
}

/*
 * Send data to the iDigi server, this routine must not block.  If it encounters
 * EAGAIN or EWOULDBLOCK error, 0 bytes must be returned and IIK will continue
 * calling this function.
 */
static idigi_callback_status_t app_network_send(idigi_write_request_t const * const write_data,
                                                size_t * sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    uint32_t ccode;

    ccode = send(*write_data->network_handle, (char _PTR_)write_data->buffer,
                 write_data->length, 0);

    if (ccode == RTCS_ERROR)
    {
        ccode = RTCS_geterror(*write_data->network_handle);
        rc = idigi_callback_abort;
        APP_DEBUG("network_send: send() failed RTCS error [%d]", ccode);
    }
    else
    {
        *sent_length = ccode;
        if (*sent_length == 0)
        {
    	    rc = idigi_callback_busy;
        }
    }

    return rc;
}

/*
 * This routine reads a specified number of bytes from the iDigi server.  This 
 * function must not block. If it encounters EAGAIN or EWOULDBLOCK error, 0 
 * bytes must be returned and IIK will continue calling this function.
 */
static idigi_callback_status_t app_network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int_32 ccode;
    
    ccode = recv(*read_data->network_handle, (char *)read_data->buffer, (int)read_data->length, 0);
    if (ccode == RTCS_ERROR)
    {
        if (errno == EAGAIN)
        {
            rc = idigi_callback_busy;
        }
        else
        {
            ccode = RTCS_geterror(*read_data->network_handle);
        	APP_DEBUG("network_receive: Error, recv() failed RTCS error [%d]", ccode);
            *read_length = (size_t)0;
            rc = idigi_callback_abort;
            
            //Time Delay task to allow to see printf
            _time_delay(5000);
        }
    }
    else
    {
#if 0
    	APP_DEBUG("network_receive: Received %ld bytes of data.\r\n", ccode);
#endif
        *read_length = (size_t)ccode;
        if (*read_length == 0)
        {
        	rc = idigi_callback_busy;
        }
    }

    return rc;
}

static idigi_callback_status_t app_network_close(idigi_network_handle_t * const fd)
{
    uint32_t rtn;

    if (*fd != socket_fd)
    {
    	APP_DEBUG("network_close: mismatch on network handles: callback [%d] != local [%d]\n",
              *fd, socket_fd);
        ASSERT(0);
    }

    // Note: this does a graceful close - like linger
    rtn = shutdown(*fd, FLAG_CLOSE_TX);
    if (rtn != RTCS_OK)
    {
    	APP_DEBUG("network_close: failed, code = %d", rtn);
    }
    
    socket_fd = RTCS_SOCKET_ERROR;

    return idigi_callback_continue;
}

static int app_server_disconnected(void)
{

    APP_DEBUG("Disconnected from server\n");
    return 0;
}

static int app_server_reboot(void)
{

    APP_DEBUG("Reboot from server\n");
    
    return 0;
}

/*
 *  Callback routine to handle all networking related calls.
 */
idigi_callback_status_t app_network_handler(idigi_network_request_t const request,
                                            void const * const request_data, size_t const request_length,
                                            void * const response_data, size_t * const response_length)
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

