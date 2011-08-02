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

static void init_setting(idigi_data_t * const idigi_ptr)
{
    idigi_ptr->edp_state = edp_init_layer;
    idigi_ptr->layer_state = 0;
    idigi_ptr->request_id = 0;
    idigi_ptr->error_code = idigi_success;
    idigi_ptr->active_facility = NULL;
    idigi_ptr->rx_ka_time = 0;
    idigi_ptr->tx_ka_time = 0;

    idigi_ptr->send_packet.total_length = 0;
    idigi_ptr->send_packet.length = 0;
    idigi_ptr->send_packet.ptr = NULL;
    idigi_ptr->send_packet.packet_buffer.in_used = false;

    idigi_ptr->receive_packet.total_length = 0;
    idigi_ptr->receive_packet.length = 0;
    idigi_ptr->receive_packet.index = 0;
    idigi_ptr->receive_packet.packet_buffer.in_used = false;

    idigi_ptr->receive_packet.free_packet_buffer = &idigi_ptr->receive_packet.packet_buffer;

}

static idigi_callback_status_t idigi_callback(idigi_callback_t const callback, idigi_class_t const class_id,  idigi_request_t const request_id,
                                                                       void * const request_data, size_t const request_length,
                                                                       void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status;

    /* call callback */
    status = callback(class_id, request_id, request_data, request_length, response_data, response_length);

    ASSERT((status == idigi_callback_continue) || (status == idigi_callback_abort) ||
                 (status == idigi_callback_busy) || (status == idigi_callback_unrecognized));

    if (status == idigi_callback_unrecognized)
    {
        /* Application must support all requests in this iDigi release.
         * If application returns unrecognized status, abort iDigi for now.
         * TODO: future release.
         */
        DEBUG_PRINTF("ERROR: Application returns unrecognized request for request=%d class_id = %d which iDigi requires for this version\n",
                        request_id.config_request, class_id);
        status = idigi_callback_abort;
    }
#if defined(DEBUG)
    else if (status == idigi_callback_abort)
    {
        DEBUG_PRINTF("idigi_callback: callback for class id = %d request id = %d returned abort\n", class_id, request_id.config_request);
    }
#endif
    return status;
}


static void notify_error_status(idigi_callback_t const callback, idigi_class_t const class_number, idigi_request_t const request_number, idigi_status_t const status)
{
    idigi_error_status_t err_status = {class_number, request_number, status};
    idigi_request_t request_id = {idigi_config_error_status};

    idigi_callback(callback, idigi_class_config, request_id, &err_status, sizeof err_status, NULL, NULL);
    return;
}


static idigi_callback_status_t get_system_time(idigi_data_t * const idigi_ptr, uint32_t * const uptime)
{
    size_t  length;
    idigi_callback_status_t status;
    idigi_request_t request_id = {idigi_os_system_up_time};

    /* Call callback to get system up time in second */
    status = idigi_callback(idigi_ptr->callback, idigi_class_operating_system, request_id, NULL, 0, uptime, &length);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
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
    }
    return status;

}


static idigi_callback_status_t malloc_data(idigi_data_t * const idigi_ptr, size_t const length, void ** ptr)
{
    idigi_callback_status_t status;

    status = malloc_cb(idigi_ptr->callback, length, ptr);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
    }
    return status;

}

static void free_data(idigi_data_t * const idigi_ptr, void * const ptr)
{
    idigi_request_t request_id = {idigi_os_free};

    idigi_callback(idigi_ptr->callback, idigi_class_operating_system, request_id, ptr, sizeof ptr, NULL, NULL);
    del_malloc_stats(ptr);

    return;
}

static uint16_t get_elapsed_value(uint16_t const max_value, uint32_t const last_value, uint32_t const current_value)
{
    uint16_t elapsed_value;
    uint16_t the_value = max_value;

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

static idigi_callback_status_t get_keepalive_timeout(idigi_data_t * const idigi_ptr, uint16_t * const rx_timeout, uint16_t * const tx_timeout, uint32_t * const cur_system_time)
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
        *rx_timeout =  get_elapsed_value(*idigi_ptr->rx_keepalive, idigi_ptr->rx_ka_time, *cur_system_time);
    }
    if (tx_timeout != NULL)
    {
        *tx_timeout =  get_elapsed_value(*idigi_ptr->tx_keepalive, idigi_ptr->tx_ka_time, *cur_system_time);
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
    const size_t facility_size = sizeof *facility;

    /* allocate facility data and buffer*/
    *fac_ptr = NULL;
    status = malloc_data(idigi_ptr, size + facility_size + sizeof(idigi_buffer_t), &ptr);
    if (status == idigi_callback_continue && ptr != NULL)
    {
        idigi_buffer_t * buffer_ptr;

        /* add facility to idigi facility list */
        facility = ptr;
        facility->facility_num = facility_num;
        facility->size = size;
        facility->discovery_cb = discovery_cb;
        facility->process_cb = process_cb;
//        facility->packet = NULL;
        if (idigi_ptr->facility_list != NULL)
        {
            idigi_ptr->facility_list->prev = ptr;
        }
        facility->next = idigi_ptr->facility_list;
        facility->prev = NULL;

        idigi_ptr->facility_list = ptr;

        /* setup facility data */
        facility->facility_data = ptr + facility_size;
        *fac_ptr = facility->facility_data;

        buffer_ptr = ptr + size + facility_size;
        buffer_ptr->in_used = false;

        /* set up an additional receive buffer to the idigi data */
        buffer_ptr->next = idigi_ptr->receive_packet.packet_buffer.next;
        idigi_ptr->receive_packet.packet_buffer.next = buffer_ptr;
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

            if (fac_ptr->next != NULL)
            {
                fac_ptr->next->prev = fac_ptr->prev;
            }
            if (fac_ptr->prev != NULL)
            {
                fac_ptr->prev->next = fac_ptr->next;
            }


            if (fac_ptr == idigi_ptr->facility_list)
            {
                idigi_ptr->facility_list = fac_ptr->next;
            }
            free_data(idigi_ptr, fac_ptr);
            break;
        }
    }
    return status;
}
