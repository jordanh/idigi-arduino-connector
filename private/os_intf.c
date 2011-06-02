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

static void init_setting(iik_data_t * iik_ptr)
{
    iik_ptr->edp_state = edp_init_layer;
    iik_ptr->layer_state = 0;
    iik_ptr->request_id = 0;
    iik_ptr->error_code = iik_success;
    iik_ptr->active_facility = NULL;
    iik_ptr->rx_ka_time = 0;
    iik_ptr->tx_ka_time = 0;

    iik_ptr->send_packet.total_length = 0;
    iik_ptr->send_packet.length = 0;
    iik_ptr->send_packet.ptr = NULL;
    iik_ptr->receive_packet.total_length = 0;
    iik_ptr->receive_packet.length = 0;
    iik_ptr->receive_packet.index = 0;


}

static iik_callback_status_t iik_callback(iik_callback_t const callback, iik_class_t class, iik_request_t request,
                                   void const * request_data, size_t request_length,
                                   void * response_data, size_t * response_length)
{
    iik_callback_status_t status;

    status = callback(class, request, request_data, request_length, response_data, response_length);

    ASSERT((status == iik_callback_continue) || (status == iik_callback_abort) || (status == iik_callback_busy) || (status == iik_callback_unrecognized));

    if (status == iik_callback_unrecognized)
    {
        /* Application must support all requests in this IIK release
         * so if application returns unrecognized status, abort IIK.
         */
        DEBUG_PRINTF("ERROR: Application returns unrecognized request for request=%d class = %d which IIK requires for this version\n", request.config_request, class);
        status = iik_callback_abort;
    }

    return status;
}


static iik_callback_status_t notify_error_status(iik_callback_t callback, iik_class_t class, iik_request_t request, iik_status_t status)
{
    iik_error_status_t err_status;
    iik_request_t request_id;

    err_status.class = class;
    err_status.request = request;
    err_status.status = status;

    request_id.config_request = iik_config_error_status;
    return iik_callback(callback, iik_class_config, request_id, &err_status, sizeof err_status, NULL, NULL);
}


static iik_callback_status_t get_system_time(iik_data_t * iik_ptr, uint32_t * mstime)
{
    size_t  length;
    iik_callback_status_t status;
    iik_request_t request_id;

    request_id.os_request = iik_os_system_up_time;
    status = iik_callback(iik_ptr->callback, iik_class_operating_system, request_id, NULL, 0, mstime, &length);
    if (status == iik_callback_abort)
    {
        iik_ptr->error_code = iik_configuration_error;
    }

    return status;
}

static iik_callback_status_t malloc_cb(iik_callback_t const callback, size_t length, void ** ptr)
{
    iik_callback_status_t status;
    size_t  size = length, len;
    iik_request_t request_id;
    void * p;

    if (callback == NULL)
    {
        status = iik_callback_abort;
    }
    request_id.os_request = iik_os_malloc;
    status = iik_callback(callback, iik_class_operating_system, request_id, &size, sizeof size, &p, &len);
    if (status == iik_callback_continue)
    {
        *ptr = p;
        add_malloc_stats(p, size);
    }
    return status;

}


static iik_callback_status_t malloc_data(iik_data_t * iik_ptr, size_t length, void ** ptr)
{
    iik_callback_status_t status;

    status = malloc_cb(iik_ptr->callback, length, ptr);
    if (status == iik_callback_abort)
    {
        iik_ptr->error_code = iik_configuration_error;
    }
    return status;

}

static iik_callback_status_t free_data(iik_data_t * iik_ptr, void * ptr)
{
    iik_callback_status_t   status;
    iik_request_t request_id;

    request_id.os_request = iik_os_free;
    status = iik_callback(iik_ptr->callback, iik_class_operating_system, request_id, ptr, sizeof(void *), NULL, NULL);
    if (status == iik_callback_abort)
    {
        iik_ptr->error_code = iik_configuration_error;
    }
    else if (status == iik_callback_continue)
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

static void * get_facility_data(iik_data_t * iik_ptr, uint16_t facility_num)
{
    iik_facility_t * fac_ptr;
    void * ptr = NULL;

    for (fac_ptr = iik_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {
            ptr = fac_ptr->facility_data;
            break;
        }
    }

    return ptr;
}

static iik_callback_status_t add_facility_data(iik_data_t * iik_ptr, uint16_t facility_num, void ** fac_ptr, size_t size,
                                               iik_facility_process_cb_t discovery_cb, iik_facility_process_cb_t process_cb)
{
    iik_callback_status_t status;
    iik_facility_t * facility;
    void * ptr;
    size_t facility_size = sizeof(iik_facility_t);

    *fac_ptr = NULL;
    status = malloc_data(iik_ptr, size + facility_size, &ptr);
    if (status == iik_callback_continue && ptr != NULL)
    {
        facility = (iik_facility_t *)ptr;
        facility->facility_num = facility_num;
        facility->size = size;
        facility->discovery_cb = discovery_cb;
        facility->process_cb = process_cb;
        facility->packet = NULL;
        facility->next = iik_ptr->facility_list;
        iik_ptr->facility_list = ptr;
        facility->facility_data = ptr + facility_size;
        *fac_ptr = facility->facility_data;
    }

    return status;
}

static iik_callback_status_t del_facility_data(iik_data_t * iik_ptr, uint16_t facility_num)
{
    iik_callback_status_t status = iik_callback_continue;
    iik_facility_t * fac_ptr;
    iik_facility_t * prev_ptr = NULL;
    iik_facility_t * next_ptr;

    for (fac_ptr = iik_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {
            next_ptr = fac_ptr->next;

            status = free_data(iik_ptr, fac_ptr);
            if (status == iik_callback_continue)
            {
                if (prev_ptr != NULL)
                {
                    prev_ptr->next = next_ptr;
                }
                else
                {
                    iik_ptr->facility_list = next_ptr;
                }
            }
            break;
        }
        prev_ptr = fac_ptr;
    }
    return status;
}
