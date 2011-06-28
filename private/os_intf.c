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

static void init_setting(idigi_data_t * idigi_ptr)
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
    idigi_ptr->receive_packet.total_length = 0;
    idigi_ptr->receive_packet.length = 0;
    idigi_ptr->receive_packet.index = 0;
    idigi_ptr->receive_packet.free_packet_buffer = &idigi_ptr->receive_packet.packet_buffer;


}

static idigi_callback_status_t idigi_callback(idigi_callback_t const callback, idigi_class_t class_id, idigi_request_t request_id,
                                   void * const request_data, size_t request_length,
                                   void * response_data, size_t * response_length)
{
    idigi_callback_status_t status;

    status = callback(class_id, request_id, request_data, request_length, response_data, response_length);

    ASSERT((status == idigi_callback_continue) || (status == idigi_callback_abort) || (status == idigi_callback_busy) || (status == idigi_callback_unrecognized));

    if (status == idigi_callback_unrecognized)
    {
        /* Application must support all requests in this iDigi release
         * so if application returns unrecognized status, abort iDigi.
         */
        DEBUG_PRINTF("ERROR: Application returns unrecognized request for request=%d class_id = %d which iDigi requires for this version\n", request_id.config_request, class_id);
        status = idigi_callback_abort;
    }

    return status;
}


static void notify_error_status(idigi_callback_t callback, idigi_class_t class_number, idigi_request_t request_number, idigi_status_t status)
{
    idigi_error_status_t err_status;
    idigi_request_t request_id;

    err_status.class_id = class_number;
    err_status.request_id = request_number;
    err_status.status = status;

    request_id.config_request = idigi_config_error_status;
    idigi_callback(callback, idigi_class_config, request_id, &err_status, sizeof err_status, NULL, NULL);
    return;
}


static idigi_callback_status_t get_system_time(idigi_data_t * idigi_ptr, uint32_t * mstime)
{
    size_t  length;
    idigi_callback_status_t status;
    idigi_request_t request_id;

    request_id.os_request = idigi_os_system_up_time;
    status = idigi_callback(idigi_ptr->callback, idigi_class_operating_system, request_id, NULL, 0, mstime, &length);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
    }

    return status;
}

static idigi_callback_status_t malloc_cb(idigi_callback_t const callback, size_t length, void ** ptr)
{
    idigi_callback_status_t status;
    size_t  size = length, len;
    idigi_request_t request_id;
    void * p;

    if (callback == NULL)
    {
        status = idigi_callback_abort;
    }
    request_id.os_request = idigi_os_malloc;
    status = idigi_callback(callback, idigi_class_operating_system, request_id, &size, sizeof size, &p, &len);
    if (status == idigi_callback_continue)
    {
        *ptr = p;
        add_malloc_stats(p, size);
    }
    return status;

}


static idigi_callback_status_t malloc_data(idigi_data_t * idigi_ptr, size_t length, void ** ptr)
{
    idigi_callback_status_t status;

    status = malloc_cb(idigi_ptr->callback, length, ptr);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
    }
    return status;

}

static idigi_callback_status_t free_data(idigi_data_t * idigi_ptr, void * ptr)
{
    idigi_callback_status_t   status;
    idigi_request_t request_id;

    request_id.os_request = idigi_os_free;
    status = idigi_callback(idigi_ptr->callback, idigi_class_operating_system, request_id, ptr, sizeof(void *), NULL, NULL);
    if (status == idigi_callback_abort)
    {
        idigi_ptr->error_code = idigi_configuration_error;
    }
    else if (status == idigi_callback_continue)
    {
        del_malloc_stats(ptr);
    }

    return status;
}

static uint32_t get_timeout_limit_in_seconds(uint32_t max_timeout_in_second, uint32_t system_up_time_in_millisecond)
{
    uint32_t time_limit;
    uint32_t system_up_time = system_up_time_in_millisecond;

    time_limit = (max_timeout_in_second * MILLISECONDS_PER_SECOND);
    if (time_limit > 0)
    {
        if (system_up_time_in_millisecond > time_limit)
        {
            system_up_time %=time_limit;
            system_up_time *= time_limit;
        }

        time_limit -= system_up_time;
        time_limit /= MILLISECONDS_PER_SECOND;
    }
    return time_limit;
}

static void * get_facility_data(idigi_data_t * idigi_ptr, uint16_t facility_num)
{
    idigi_facility_t * fac_ptr;
    void * ptr = NULL;

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

static idigi_callback_status_t add_facility_data(idigi_data_t * idigi_ptr, uint16_t facility_num, void ** fac_ptr, size_t size,
                                               idigi_facility_process_cb_t discovery_cb, idigi_facility_process_cb_t process_cb)
{
    idigi_callback_status_t status;
    idigi_facility_t * facility;
    void * ptr;
    size_t facility_size = sizeof(idigi_facility_t);

    *fac_ptr = NULL;
    status = malloc_data(idigi_ptr, size + facility_size + sizeof(idigi_buffer_t), &ptr);
    if (status == idigi_callback_continue && ptr != NULL)
    {
        idigi_buffer_t * buffer_ptr;

        facility = (idigi_facility_t *)ptr;
        facility->facility_num = facility_num;
        facility->size = size;
        facility->discovery_cb = discovery_cb;
        facility->process_cb = process_cb;
        facility->packet = NULL;
        facility->next = idigi_ptr->facility_list;
        idigi_ptr->facility_list = ptr;
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

static idigi_callback_status_t del_facility_data(idigi_data_t * idigi_ptr, uint16_t facility_num)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_facility_t * fac_ptr;
    idigi_facility_t * prev_ptr = NULL;
    idigi_facility_t * next_ptr;

    for (fac_ptr = idigi_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {
            next_ptr = fac_ptr->next;

            status = free_data(idigi_ptr, fac_ptr);
            if (status == idigi_callback_continue)
            {
                if (prev_ptr != NULL)
                {
                    prev_ptr->next = next_ptr;
                }
                else
                {
                    idigi_ptr->facility_list = next_ptr;
                }
            }
            break;
        }
        prev_ptr = fac_ptr;
    }
    return status;
}
