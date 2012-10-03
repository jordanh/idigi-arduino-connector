/*
 * Copyright (c) 1996-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include "iDigiConnector.h"

extern "C" {
#include "idigi_config.h"
#include "idigi_api.h"
}


idigi_callback_status_t iDigiConnectorClass::app_network_connect(char * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    UNUSED_ARGUMENT(length);
    
    idigi_callback_status_t rc = idigi_callback_continue;

    APP_DEBUG("app_network_connect: %s\r\n", host_name);
    if (!client.connect(host_name, IDIGI_PORT))
    {
        rc = idigi_callback_busy;
    } else {
        *network_handle = (idigi_network_handle_t *) &client;
    }
    
    return rc;
}

bool iDigiConnectorClass::app_network_connected(idigi_network_handle_t * network_handle)
{
    EthernetClient *client = (EthernetClient *) network_handle;

    bool socket_connected = (bool) client->connected();
    APP_DEBUG("app_network_connected: %d\r\n", (int) socket_connected);
    if (connected && socket_connected == 0)
    {
      connected = false;
    } 

    return socket_connected;
}

idigi_callback_status_t iDigiConnectorClass::app_network_send(idigi_write_request_t * const write_data, size_t * sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    EthernetClient *client = (EthernetClient *) write_data->network_handle;
    APP_DEBUG("network_send: len = %d\r\n", write_data->length);
    *sent_length = client->write((uint8_t * const) write_data->buffer, write_data->length);
    if (*sent_length == 0)
    {
        return idigi_callback_abort;
    }

    return rc;
}


idigi_callback_status_t iDigiConnectorClass::app_network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    if (!app_network_connected(read_data->network_handle))
      return idigi_callback_abort;

    EthernetClient *client = (EthernetClient *) read_data->network_handle;
    APP_DEBUG("app_network_recv: timeout_sec = %d\r\n", read_data->timeout);
    if (!client->available())
        return idigi_callback_abort;
  
    *read_length = client->read((uint8_t *) read_data->buffer, read_data->length);

    if (*read_length == 0)
      rc = idigi_callback_busy;
  
    if (!app_network_connected(read_data->network_handle))
      rc = idigi_callback_abort;

    return rc;
}


idigi_callback_status_t iDigiConnectorClass::app_network_close(idigi_network_handle_t * const fd)
{
    idigi_callback_status_t status = idigi_callback_continue;

    ((EthernetClient *) fd)->stop();

    return status;
}

int iDigiConnectorClass::app_server_disconnected(void)
{
    return 0;
}

int iDigiConnectorClass::app_server_reboot(void)
{
    /* should not return from rebooting the system */
    return 0;
}


idigi_callback_status_t iDigiConnectorClass::app_network_handler(idigi_network_request_t const request,
                                            void * const request_data, size_t const request_length,
                                            void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int ret;

    switch (request)
    {
    case idigi_network_connect:
        status = app_network_connect((char *)request_data, (size_t) request_length, (idigi_network_handle_t **)response_data);
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
        break;

    }

    return status;
}

