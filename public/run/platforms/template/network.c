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

/**
 * @file
 *  @brief Rountines which implement the IIK network interface.
 */
#include "idigi_api.h"
#include "platform.h"

/**
 * @brief   Connect to the iDigi server
 *
 * This routine sets up a connection to the iDigi Device Cloud specified in the
 * host_name and fills in a network handle which is then passed to subsequent 
 * networking calls. This routine will take the given hostname and attempt to 
 * resolve the name, connect and wait for the connection to complete. The example 
 * given waits one second for the connection to complete, if the connection does not 
 * complete idigi_callback_busy is returned and the network_connect routine is called again. 
 * If the connection was successful then idigi_callback_continue is returned.
 *
 * @param [in] host_name  FQDN of iDigi host to connect to
 * @param [in] length  Number of bytes in the hostname
 * @param [out] network_handle This is filled in with the value of a handle which 
 * is passed to subsequent networking calls. The idigi_network_handle_t is defined in 
 * public\include\idigi_types.h.
 *
 * @retval idigi_callback_status_t
 *
 * @see @ref connect API Network Callback
 */
static idigi_callback_status_t app_network_connect(char const * const host_name, size_t const length, idigi_network_handle_t ** network_handle)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    UNUSED_ARGUMENT(host_name);
    UNUSED_ARGUMENT(length);
    UNUSED_ARGUMENT(network_handle);

    return rc;
}

/**
 * @brief   Send data to the iDigi server
 *
 * This routine sends data to the iDigi Device Cloud. This function must not block.
 * If it encounters EAGAIN or EWOULDBLOCK error, 0 bytes must be returned and the IIK 
 * will continue calling this function. If successful idigi_callback_continue is returned. 
 * If the data could not be sent idigi_callback_busy is returned, otherwise idigi_callback_abort is returned.
 *
 * @param [in] write_data  Pointer to a idigi_write_request_t structure
 * @param [out] sent_length Filled in with the number of bytes sent.
 *
 * @retval idigi_callback_status_t
 *
 * @see @ref send API Network Callback
 */
static idigi_callback_status_t app_network_send(idigi_write_request_t const * const write_data,
                                            size_t * sent_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    UNUSED_ARGUMENT(write_data);
    UNUSED_ARGUMENT(sent_length);

    return rc;
}

/**
 * @brief   Receive data from the iDigi server
 *
 * This routine reads a specified number of bytes from the iDigi Device Cloud.
 * This function blocks up to the timeout value specified. If no data is received
 * and the timeout has expired 0 bytes must be returned and IIK will continue 
 * calling this function.
 *
 * @param [in] read_data  Pointer to a idigi_read_request_t structure
 * @param [out] read_length Filled in with the number of bytes received.
 *
 * @retval idigi_callback_status_t
 *
 * @note When running in a multithreaded model this is where the IIK will
 * relinquish control, the user should sleep until data is received up to the 
 * timeout given.
 *
 * @see @ref receive API Network Callback
 */
static idigi_callback_status_t app_network_receive(idigi_read_request_t * read_data, size_t * read_length)
{
    idigi_callback_status_t rc = idigi_callback_continue;

    UNUSED_ARGUMENT(read_data);
    UNUSED_ARGUMENT(read_length);
    return rc;
}

/**
 * @brief   Close the connection to the iDigi server
 *
 * This routine closes the connection to the iDigi Device Cloud
 *
 * @param [in] fd  Pointer to a idigi_read_request_t structure
 *
 * @retval idigi_callback_status_t
 *
 * @see @ref close API Network Callback
 */
static idigi_callback_status_t app_network_close(idigi_network_handle_t * const fd)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(fd);

    return status;
}

/**
 * @cond DEV
 */
static int app_server_disconnected(void)
{
    return 0;
}

static int app_server_reboot(void)
{
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
        break;

    }

    return status;
}

/**
 * @endcond
 */
