/*
 * Copyright (c) 2011 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
#include "debug.h"

static void reset_initial_data(idigi_data_t * const idigi_ptr)
{
    idigi_ptr->edp_state = edp_init_layer;
    idigi_ptr->layer_state = 0;
    idigi_ptr->request_id = 0;
    idigi_ptr->active_facility = NULL;
    idigi_ptr->last_rx_keepalive_sent_time = 0;
    idigi_ptr->last_tx_keepalive_received_time = 0;

    idigi_ptr->send_packet.total_length = 0;
    idigi_ptr->send_packet.bytes_sent = 0;
    idigi_ptr->send_packet.ptr = NULL;
    idigi_ptr->send_packet.complete_cb = NULL;

    idigi_ptr->receive_packet.total_length = 0;
    idigi_ptr->receive_packet.bytes_received = 0;
    idigi_ptr->receive_packet.index = 0;
    idigi_ptr->receive_packet.ptr = NULL;
    idigi_ptr->receive_packet.data_packet = NULL;
    idigi_ptr->receive_packet.timeout = MAX_RECEIVE_TIMEOUT_IN_SECONDS;

}

static idigi_callback_status_t idigi_callback(idigi_callback_t const callback, idigi_class_t const class_id,  idigi_request_t const request_id,
                                                                       void const * const request_data, size_t const request_length,
                                                                       void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    status = callback(class_id, request_id, request_data, request_length, response_data, response_length);

    switch (status)
    {
    case idigi_callback_continue:
    case idigi_callback_busy:
        break;

    case idigi_callback_unrecognized:
       idigi_debug_printf("idigi_callback : callback returns unrecognized request for request=%d class_id = %d\n",
                        request_id.config_request, class_id);
        break;
    case idigi_callback_abort:
        idigi_debug_printf("idigi_callback: callback for class id = %d request id = %d returned abort\n", class_id, request_id.config_request);
        break;
    default:
    {
        /* callback returns invalid return code */
        idigi_error_status_t err_status ;
        idigi_request_t err_id;

        err_id.config_request = idigi_config_error_status;

        err_status.class_id = class_id;
        err_status.request_id = request_id;
        err_status.status = idigi_invalid_response;

        idigi_debug_printf("idigi_callback: callback for class id = %d request id = %d returned invalid return code %d\n", class_id, request_id.config_request, status);
        callback(idigi_class_config, err_id, &err_status, sizeof err_status, NULL, NULL);
        status = idigi_callback_abort;
        break;
    }
    }

    return status;
}

static void notify_error_status(idigi_callback_t const callback, idigi_class_t const class_number, idigi_request_t const request_number, idigi_status_t const status)
{
#if defined(IDIGI_DEBUG)
    idigi_error_status_t err_status;
    idigi_request_t request_id;

    request_id.config_request = idigi_config_error_status;
    err_status.class_id = class_number;
    err_status.request_id = request_number;
    err_status.status = status;

    idigi_callback_no_response(callback, idigi_class_config, request_id, &err_status, sizeof err_status);
#else
    UNUSED_PARAMETER(callback);
    UNUSED_PARAMETER(class_number);
    UNUSED_PARAMETER(request_number);
    UNUSED_PARAMETER(status);
#endif
    return;
}


static idigi_callback_status_t get_system_time(idigi_data_t * const idigi_ptr, unsigned long * const uptime)
{
    size_t  length;
    idigi_callback_status_t status;
    idigi_request_t request_id;

    /* Call callback to get system up time in second */
    request_id.os_request = idigi_os_system_up_time;
    status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_operating_system, request_id, uptime, &length);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
    }
    else if (status != idigi_callback_continue)
    {
        ASSERT(idigi_false);
        status = idigi_callback_abort;
    }


    return status;
}

static idigi_callback_status_t malloc_cb(idigi_callback_t const callback, size_t const length, void ** ptr)
{
    idigi_callback_status_t status;
    size_t  size = length, len;
    idigi_request_t request_id;

    request_id.os_request = idigi_os_malloc;
    *ptr = NULL;
    status = idigi_callback(callback, idigi_class_operating_system, request_id, &size, sizeof size, ptr, &len);
    if (status == idigi_callback_continue)
    {
        if (*ptr != NULL)
        { 
            add_malloc_stats(*ptr, size);
        }
        else
        {
            ASSERT(idigi_false);
            status = idigi_callback_abort;
        }
    }
    return status;
}


static idigi_callback_status_t malloc_data(idigi_data_t * const idigi_ptr, size_t const length, void ** ptr)
{
    idigi_callback_status_t status;

    status = malloc_cb(idigi_ptr->callback, length, ptr);
    if (status == idigi_callback_abort || status == idigi_callback_unrecognized)
    {
        status = idigi_callback_abort;
        idigi_ptr->error_code = idigi_configuration_error;
    }
    return status;

}

static void free_data(idigi_data_t * const idigi_ptr, void * const ptr)
{
    idigi_request_t request_id;

    request_id.os_request = idigi_os_free;
    idigi_callback_no_response(idigi_ptr->callback, idigi_class_operating_system, request_id, ptr, 0);
    del_malloc_stats(ptr);

    return;
}

static void sleep_timeout(idigi_data_t * const idigi_ptr)
{
    /* no receive buffer and nothing to be read */
    if (idigi_ptr->receive_packet.free_packet_buffer == NULL &&
        idigi_ptr->receive_packet.total_length == 0)
    {
        idigi_request_t request_id;
        unsigned int const timeout = idigi_ptr->receive_packet.timeout;

        request_id.os_request = idigi_os_sleep;
        idigi_callback_no_response(idigi_ptr->callback, idigi_class_operating_system, request_id, &timeout, sizeof timeout);
    }

    return;
}

static unsigned long get_elapsed_value(unsigned long const max_value, unsigned long const last_value, unsigned long const current_value)
{
    unsigned long elapsed_value;
    unsigned long the_value = max_value;

    ASSERT(current_value >= last_value);

    elapsed_value = (current_value - last_value);
    if (max_value > 0)
    {
        if (elapsed_value > max_value)
        {
            /* already over the value so return max_value */
            elapsed_value = max_value;
        }

        the_value -= elapsed_value; /* remaining value */
    }
    return the_value;
}

static idigi_callback_status_t get_keepalive_timeout(idigi_data_t * const idigi_ptr, unsigned long * const rx_timeout, unsigned long * const tx_timeout, unsigned long * const cur_system_time)
{
    idigi_callback_status_t status = idigi_callback_continue;

    /* Calculate the timeout value (when to send rx keepalive or
     * receive tx keepalive) from last rx keepalive or tx keepalive.
     *
     * If callback exceeds the timeout value, error status callback
     * will be called.
     */

    ASSERT(cur_system_time != NULL);

    status = get_system_time(idigi_ptr, cur_system_time);
    if (status != idigi_callback_continue)
    {
        goto done;
    }

    if (rx_timeout != NULL)
    {
       /* Get the elapsed time between the configured rx keepalive interval and the last time we sent rx keepalive (last_rx_keepalive_sent_time) */
        *rx_timeout =  get_elapsed_value(GET_RX_KEEPALIVE_INTERVAL(idigi_ptr), idigi_ptr->last_rx_keepalive_sent_time, *cur_system_time);
    }
    if (tx_timeout != NULL)
    {
        /* Get the elapsed time between the configured tx keepalive interval and the last time we received tx keepalive (last_tx_keepalive_received_time) */
        *tx_timeout =  get_elapsed_value(GET_TX_KEEPALIVE_INTERVAL(idigi_ptr), idigi_ptr->last_tx_keepalive_received_time, *cur_system_time);
    }
done:
    return status;
}

static void * get_facility_data(idigi_data_t * const idigi_ptr, uint16_t const facility_num)
{
    idigi_facility_t * fac_ptr;
    void * ptr = NULL;

    /* search and return the facility data which allocated from add_facility_data() */
    for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {
            ptr = fac_ptr->facility_data;
            break;
        }
    }

    return ptr;
}

static idigi_callback_status_t add_facility_data(idigi_data_t * const idigi_ptr, unsigned int const facility_index,
                                                 uint16_t const facility_num, void ** fac_ptr, size_t const size)
{
    idigi_callback_status_t status;
    idigi_facility_t * facility;
    void * ptr;
    size_t const facility_size = sizeof *facility;

    /* allocate facility data and buffer*/

    *fac_ptr = NULL;
    status = malloc_data(idigi_ptr, size + facility_size, &ptr);
    if (status == idigi_callback_continue)
    {
        /* add facility to idigi facility list */
        facility = ptr;
        facility->facility_num = facility_num;
        facility->size = size;
        facility->facility_index = facility_index;

        add_node(&idigi_ptr->facility_list, facility);

        /* setup facility data which is at the end of idigi_facility_t */
        facility->facility_data = facility + 1;
         *fac_ptr = facility->facility_data;
    }

    return status;
}

static idigi_callback_status_t del_facility_data(idigi_data_t * const idigi_ptr, uint16_t const facility_num)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_facility_t * fac_ptr;

    /* find and free the facility */
    for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {

            remove_node(&idigi_ptr->facility_list, fac_ptr);

            free_data(idigi_ptr, fac_ptr);
            break;
        }
    }
    return status;
}


