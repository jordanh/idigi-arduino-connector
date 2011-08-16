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
 
#include <winsock2.h>
#include <windns.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
 
#include "idigi_struct.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Dnsapi.lib")

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
static int e_dns_resolve(char const * domain_name, IN_ADDR * ipaddr)
{
    DNS_STATUS status;
    PDNS_RECORD record;
    
    DNS_FREE_TYPE freetype;
    freetype = DnsFreeRecordListDeep;
    
    status = DnsQuery(domain_name, // host to query
                        DNS_TYPE_A, // query for A record.
                        DNS_QUERY_BYPASS_CACHE,  // Bypass the resolver cache.
                        NULL, // Don't provide a DNS server ip.
                        &record, 
                        NULL);
                        

    if (status)
    {
        DEBUG_PRINTF("DNS resolution failed for %s\n", domain_name);
    }
    else
    {
        ipaddr->S_un.S_addr = (record->Data.A.IpAddress);
        DEBUG_PRINTF("e_dns_resolve: ip address = %s\n", inet_ntoa(*ipaddr));
        
        // Free memory allocated for DNS records.
        DnsRecordListFree(record, freetype);
    }
    return status;
}
 
static idigi_callback_status_t network_connect(char * host_name, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_abort;
    IN_ADDR ip_addr;
    SOCKET connect_socket;
    WSADATA wsaData;
    char idigi_port[4];
    
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    
    int bOptVal = 120;
    int res;
    
    if (iDigiSetting.socket_fd == -1)
    {
        /*
         * check if it's a dotted-notation IP address.
         * If it's a domain name, attempt to resolve it.
         */
        ip_addr.S_un.S_addr = inet_addr(host_name);
        
        if (ip_addr.S_un.S_addr == INADDR_NONE)
        {
            /* Resolve domain name */
            if (e_dns_resolve(host_name, &ip_addr) < 0)
            {
                DEBUG_PRINTF("network_connect: Can't resolve DNS for %s\n", host_name);
                /* Can't resolve it either */
                goto done;
            }
        }
        
        // Initialize Winsock
        res = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(res != 0){
            DEBUG_PRINTF("WSAStartup failed with error: %d\n", res);
        }
        
        ZeroMemory( &hints, sizeof(hints) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        
        itoa(IDIGI_PORT, idigi_port, 10);
        res = getaddrinfo(inet_ntoa(ip_addr), idigi_port, &hints, &result);
        if(res != 0){
            DEBUG_PRINTF("getaddrinfo failed: %d.\n", result);
            WSACleanup();
            goto done;
        }
        
        ptr = result;
        
        // Create a SOCKET for connecting to server.
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if(connect_socket == INVALID_SOCKET){
            DEBUG_PRINTF("Error at socket(); %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            goto done;
        }
        
        // Connect to Server.
        res = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if(res == SOCKET_ERROR){
            closesocket(connect_socket);
            freeaddrinfo(result);
            connect_socket = INVALID_SOCKET;
            WSACleanup();
            goto done;
        }
        
        if(setsockopt(connect_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&bOptVal, sizeof(bOptVal)) != 0)
        {
            DEBUG_PRINTF("Failed to set socket: [%d]\n", WSAGetLastError());
            closesocket(connect_socket);
            return idigi_callback_abort;
        }

        if(setsockopt(connect_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&bOptVal, sizeof(bOptVal)) != 0)
        {
            DEBUG_PRINTF("Failed to set socket: [%d]\n", WSAGetLastError());
            closesocket(connect_socket);
            return idigi_callback_abort;
        }

                
        iDigiSetting.socket_fd = connect_socket;
        *network_handle = &iDigiSetting.socket_fd;
        rc = idigi_callback_continue;
    }
    
done:
    if (rc == idigi_callback_abort && connect_socket != INVALID_SOCKET)
    {
        close(connect_socket);
        iDigiSetting.socket_fd = -1;
    }
    return rc;
}

static idigi_callback_status_t network_send(idigi_write_request_t * write_data, size_t * sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int         ccode;
    
    SOCKET sock = *write_data->network_handle;
    
    ccode = send(sock, (char *)write_data->buffer, write_data->length, 0);

    if(ccode == SOCKET_ERROR){
        int val = WSAGetLastError();

		if(val == WSAEWOULDBLOCK || val == WSAETIMEDOUT)
		{
			DEBUG_PRINTF("network_send timeout.\n");
			return idigi_callback_busy;
		}
		else if(val == WSAECONNABORTED)
		{
			DEBUG_PRINTF("Software caused connection abort.\n");
			return idigi_callback_abort;
		}
		else
		{
			DEBUG_PRINTF("network_send error: %d\n", val);
			return idigi_callback_abort;
		}
        DEBUG_PRINTF("Network Send Fails: %d.\n", ccode);
        rc = idigi_callback_abort;
    }
    
    *sent_length = ccode;        
    return rc;
}

static idigi_callback_status_t network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;
    int ccode;

    SOCKET sock = *read_data->network_handle;
    
    ccode = recv(sock, (char *)read_data->buffer, (int)read_data->length, 0);
    
    
    
    if (ccode == 0)
    {
        /* EOF on input: the connection was closed. */
        DEBUG_PRINTF("network_receive: EOF on TCP socket read.\n");
        rc = idigi_callback_abort;
    }
    else if(ccode == SOCKET_ERROR)
    {
        int val = WSAGetLastError();

		if(val == WSAEWOULDBLOCK || val == WSAETIMEDOUT)
		{
			// Nothing to read.  Continue.
			rc = idigi_callback_busy;
		}
		else if(val == WSAECONNABORTED)
		{
            DEBUG_PRINTF("network_receive: Software Connection Abort.\n");
            rc = idigi_callback_abort;
		}
		else
		{
            DEBUG_PRINTF("network_receive: EOF on TCP socket read.\n");
            rc = idigi_callback_abort;
		}
    }
    
    *read_length = ccode;
    return rc;
}

static idigi_callback_status_t network_close(idigi_network_handle_t * fd)
{
    idigi_callback_status_t status = idigi_callback_continue;
    
    SOCKET sock = *fd;
    
    closesocket(sock);
    WSACleanup();
    
    iDigiSetting.socket_fd = -1;
    return status;
}

uint8_t network_select(idigi_network_handle_t fd, uint8_t select_set, unsigned wait_time)
{
    return 0;
}

int get_device_address()
{
    char name[255];
    PHOSTENT hostinfo;
    
    if(gethostname( name, sizeof(name)) == 0)
    {
            if((hostinfo = gethostbyname(name)) != NULL)
            {
                IN_ADDR ip_addr = *(struct in_addr *)*hostinfo->h_addr_list;
                int long_ip = (uint32_t)ip_addr.s_addr;
                return long_ip;
            }
    }
    return 0;
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

    case idigi_network_reboot:
    	// Do nothing for now.
    	DEBUG_PRINTF("Rebooting Device\n");
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
