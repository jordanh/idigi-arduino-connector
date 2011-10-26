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
#include "debug.c"

static void reset_initial_data(idigi_data_t * const idigi_ptr)
{
    idigi_ptr->active_state = idigi_device_started;
    idigi_ptr->edp_state = edp_init_layer;
    idigi_ptr->layer_state = 0;
    idigi_ptr->request_id = 0;
    idigi_ptr->error_code = idigi_success;
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

    idigi_ptr->receive_packet.free_packet_buffer = &idigi_ptr->receive_packet.packet_buffer;

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
       idigi_debug("idigi_callback : callback returns unrecognized request for request=%d class_id = %d which iDigi requires for this version\n",
                        request_id.config_request, class_id);
        break;
    case idigi_callback_abort:
        idigi_debug("idigi_callback: callback for class id = %d request id = %d returned abort\n", class_id, request_id.config_request);
        break;
    default:
    {
        /* callback returns invalid return code */
        idigi_error_status_t err_status ;
        idigi_request_t const err_id = {idigi_config_error_status};

        err_status.class_id = class_id;
        err_status.request_id = request_id;
        err_status.status = idigi_invalid_response;

        idigi_debug("idigi_callback: callback for class id = %d request id = %d returned invalid return code %d\n", class_id, request_id.config_request, status);
        callback(idigi_class_config, err_id, &err_status, sizeof err_status, NULL, NULL);
        status = idigi_callback_abort;
        break;
    }
    }

    return status;
}

static void notify_error_status(idigi_callback_t const callback, idigi_class_t const class_number, idigi_request_t const request_number, idigi_status_t const status)
{
    idigi_error_status_t err_status;
    idigi_request_t const request_id = {idigi_config_error_status};

    err_status.class_id = class_number;
    err_status.request_id = request_number;
    err_status.status = status;

    idigi_callback_no_response(callback, idigi_class_config, request_id, &err_status, sizeof err_status);
    return;
}


static idigi_callback_status_t get_system_time(idigi_data_t * const idigi_ptr, uint32_t * const uptime)
{
    size_t  length;
    idigi_callback_status_t status;
    idigi_request_t request_id = {idigi_os_system_up_time};

    /* Call callback to get system up time in second */
    status = idigi_callback_no_request_data(idigi_ptr->callback, idigi_class_operating_system, request_id, uptime, &length);
    if (status == idigi_callback_abort || status == idigi_callback_unrecognized)
    {
        idigi_ptr->error_code = idigi_configuration_error;
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
    status = idigi_callback(callback, idigi_class_operating_system, request_id, &size, sizeof size, ptr, &len);
    if (status == idigi_callback_continue)
    {
        add_malloc_stats(*ptr, size);
        ASSERT(*ptr != NULL);
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
    idigi_request_t request_id = {idigi_os_free};

    idigi_callback_no_response(idigi_ptr->callback, idigi_class_operating_system, request_id, ptr, sizeof ptr);
    del_malloc_stats(ptr);

    return;
}

static uint32_t get_elapsed_value(uint32_t const max_value, uint32_t const last_value, uint32_t const current_value)
{
    uint32_t elapsed_value;
    uint32_t the_value = max_value;

    ASSERT(current_value >= last_value);

    elapsed_value = (current_value - last_value);
    if (max_value > 0)
    {
        if (elapsed_value > max_value)
        {
            /* already over the value so return 0 */
            elapsed_value = max_value;
        }

        the_value -= elapsed_value;
    }
    return the_value;
}

static idigi_callback_status_t get_keepalive_timeout(idigi_data_t * const idigi_ptr, uint32_t * const rx_timeout, uint32_t * const tx_timeout, uint32_t * const cur_system_time)
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
        *rx_timeout =  get_elapsed_value(idigi_ptr->rx_keepalive_interval, idigi_ptr->last_rx_keepalive_sent_time, *cur_system_time);
    }
    if (tx_timeout != NULL)
    {
        /* Get the elapsed time between the configured tx keepalive interval and the last time we received tx keepalive (last_tx_keepalive_received_time) */
        *tx_timeout =  get_elapsed_value(idigi_ptr->tx_keepalive_interval, idigi_ptr->last_tx_keepalive_received_time, *cur_system_time);
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

static idigi_callback_status_t add_facility_data(idigi_data_t * const idigi_ptr, uint16_t const facility_num, void ** fac_ptr, size_t const size,
                                               idigi_facility_process_cb_t discovery_cb, idigi_facility_process_cb_t process_cb)
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

        ASSERT(ptr != NULL);
        /* add facility to idigi facility list */
        facility = ptr;
        facility->facility_num = facility_num;
        facility->size = size;
        facility->discovery_cb = discovery_cb;
        facility->process_cb = process_cb;

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


