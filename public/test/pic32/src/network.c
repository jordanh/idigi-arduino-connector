
#include "idigi.h"
#include <plib.h>
#include <tcpip_bsd/tcpip_bsd.h>

static idigi_callback_status_t network_connect(char * host_name, 
                                            idigi_network_handle_t ** network_handle){
    SOCKET sock;
    struct sockaddr_in sin;
    
    // If socket descriptor does not exist, create and connect.
    if(device_data.socket_fd == -1){
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
        sin.sin_port = 3197;
        sin.sin_family = AF_INET;
        sin.sin_addr.S_un.S_addr = TCPIPAddrStrToDword(host_name);
        
        // The connect process requires multiple messages to be
        // sent across the link, keep trying until successful.
        // TODO: quit after a number of tries.
        int status;
        int tries = 0;
        while((status = connect(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0){
            if(status != SOCKET_CNXN_IN_PROGRESS){ // Not a socket Error.
                closesocket(sock);
                return idigi_callback_abort;
            }
            if(++tries >= 4096){
                closesocket(sock);
                return idigi_callback_abort;
            }
        }
        
        // Successfully connected.
        device_data.socket_fd = sock;
        /* We're connected! */
        *network_handle = &device_data.socket_fd;
        return idigi_callback_continue;
    }
}

static idigi_callback_status_t network_send(idigi_write_request_t * write_data, size_t * sent_length){
    int rc;

    rc = send(*write_data->network_handle, (char *)write_data->buffer, write_data->length, 0);
    
    // Error encountered sending data on socket.
    if(rc < 0){
        if(rc == SOCKET_NOT_READY){
            // Previous Packet not Yet ACKed.
            return idigi_callback_busy;
        }
        else{ // Assume some kind of Socket Error, No Data to Send, or Cannot allocate for sending.
            return idigi_callback_abort;
        }
    }
    
    // If result from send > 0, rc is number of bytes sent.
    *sent_length = (size_t)rc;
    
    return idigi_callback_continue;
}

static idigi_callback_status_t network_receive(idigi_read_request_t * read_data, size_t * read_length){
    int rc;
    int sock = *read_data->network_handle;
    
    while((rc = recv(sock, (char *)read_data->buffer, (int)read_data->length, 0)) < 0){
        // Error encountered receiving data on socket.
        return idigi_callback_abort;
    }
    // If result from recv > 0, rc is number of bytes received.
    *read_length = (size_t)rc;
    return idigi_callback_continue;
}

static idigi_callback_status_t network_close(idigi_network_handle_t * fd){
    // Close socket and Set device_data socket descriptor to -1 (uninitialized).
    closesocket(*fd);
    device_data.socket_fd = -1;
    
    return idigi_callback_continue;
}

idigi_callback_status_t idigi_network_callback(idigi_network_request_t request, 
                                            void * const request_data, size_t request_length, 
                                            void * response_data, size_t * response_length){
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_PARAMETER(request_length);

    switch (request){
        
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
    	// Do nothing for now.  Eventually perform a Device Reset.
    	break;
    case idigi_network_disconnected:
        /* set status to idk_callback_abort to abort iDigi or
         * idk_callback_continue to reconnect to server.
         */
        break;
    }
    
    return status;
}