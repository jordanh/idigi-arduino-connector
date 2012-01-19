#include "idigi_app.h"

static BOOL dns_resolve_name(char const * const domain_name,
        IP_ADDR * ip_addr){

    DNSResolve((BYTE *)domain_name, DNS_TYPE_A);
    int i = 50; // number of tries.
    BOOL resolved = false;
    // Try for 10 seconds to resolve DNS.
    while(i < 200 && !resolved){
        resolved = DNSIsResolved(ip_addr);
        i++;
        sleep(50);
        StackTask();
    }
    return resolved;
}

static idigi_callback_status_t network_connect(char const * const host_name,
        idigi_network_handle_t ** network_handle){
    
    SOCKET sock;
    IP_ADDR * host_addr = malloc(sizeof(IP_ADDR));
    struct sockaddr_in sin;

    // If socket descriptor does not exist, create and connect.
    if(idigi_config.socket_fd == -1){
        // Create the Socket
        if((sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) == SOCKET_ERROR ){
            return idigi_callback_abort;
        }

        // bind to a unique local port
        sin.sin_port = 0; // let stack pick port.
        sin.sin_addr.S_un.S_addr = IP_ADDR_ANY;
        if(bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR){
            closesocket(sock);
            return idigi_callback_abort;
        }

        // Try to connect to server on iDigi port.
        sin.sin_port = IDIGI_PORT;
        sin.sin_family = AF_INET;

        // Attempt to resolve DNS, if host name is an IP address
        // host_addr will be set ot IP.
        if(!dns_resolve_name(host_name, host_addr)){
            return idigi_callback_abort; // Could not resolve DNS.
        }

        sin.sin_addr.S_un.S_addr = (*host_addr).Val;

        // The connect process requires multiple messages to be
        // sent across the link, keep trying until successful.
        // Try for 10 seconds to establish connection.
        int status;
        int i = 0;
        while(i < 200 & (status = connect(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0){
            if(status != SOCKET_CNXN_IN_PROGRESS){ // Not a socket Error.
                closesocket(sock);
                return idigi_callback_abort;
            }
            StackTask();
            sleep(50);
            i++;
        }

        if(status == SOCKET_CNXN_IN_PROGRESS || status == SOCKET_ERROR){
            // Connection never established, abort.
            closesocket(sock);
            return idigi_callback_abort;
        }

        // Successfully connected.
        idigi_config.socket_fd = sock;
        /* We're connected! */
        *network_handle = &idigi_config.socket_fd;
        return idigi_callback_continue;
    }
}

static idigi_callback_status_t network_send(idigi_write_request_t * write_data, size_t * sent_length){
    int rc;

    StackTask();
    rc = send(idigi_config.socket_fd,
        (char *)write_data->buffer, write_data->length, 0);
    StackTask();

    // Error encountered sending data on socket.
    if(rc < 0){
        // Assume some kind of Socket Error, No Data to Send, or Cannot allocate for sending.
        return idigi_callback_abort;
    }

    // If result from send > 0, rc is number of bytes sent.
    *sent_length = (size_t)rc;

    return idigi_callback_continue;
}

static idigi_callback_status_t network_receive(idigi_read_request_t * read_data, size_t * read_length){
    int rc;
    int sock = idigi_config.socket_fd;

    if((rc = recv(sock, read_data->buffer, read_data->length, 0)) < 0){
        return idigi_callback_abort;
    }
    
    // If result from recv >= 0, rc is number of bytes received.
    *read_length = (size_t)rc;
    return idigi_callback_continue;
}

static idigi_callback_status_t network_close(idigi_network_handle_t * fd){
    // Close socket and Set device_data socket descriptor to -1 (uninitialized).
    closesocket(idigi_config.socket_fd);
    idigi_config.socket_fd = -1;
    idigi_config.connected = false;

    return idigi_callback_continue;
}

idigi_callback_status_t idigi_network_callback(idigi_network_request_t request,
                                            const void * const request_data,
                                            void * response_data,
                                            size_t * response_length){
    
    idigi_callback_status_t status = idigi_callback_continue;
    switch(request){
        case idigi_network_connect:
            status = network_connect((char *)request_data,
                    (idigi_network_handle_t **)response_data);
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
        case idigi_network_reboot:
        default:
            DEBUG_PRINTF("idigi_network_callback: unrecognized callback request [%d]\n", request);
            break;
    }

    return status;
}