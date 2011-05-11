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

static void init_setting(idk_data_t * idk_ptr)
{
    idk_ptr->edp_state = edp_init_layer;
    idk_ptr->layer_state = 0;
    idk_ptr->request_id = 0;
    idk_ptr->error_code = idk_success;
    idk_ptr->active_facility = NULL;
    //idk_ptr->network_busy = false

}


static idk_callback_status_t notify_error_status(idk_callback_t callback, idk_class_t class, idk_request_t request, idk_status_t status)
{
    idk_error_status_t err_status;
    idk_request_t request_id;

    err_status.class = class;
    err_status.request = request;
    err_status.status = status;

    request_id.base_request = idk_base_error_status;
    return callback(idk_class_base, request_id, &err_status, sizeof err_status, NULL, NULL);
}


static idk_callback_status_t get_system_time(idk_data_t * idk_ptr, uint32_t * mstime)
{
    size_t  length;
    idk_callback_status_t status;
    idk_request_t request_id;

    request_id.base_request = idk_base_system_time;
    status = idk_ptr->callback(idk_class_base, request_id, NULL, 0, mstime, &length);
    if (status == idk_callback_abort)
    {
        idk_ptr->error_code = idk_configuration_error;
    }
    /* do I need to check the length */

    return status;
}



static idk_callback_status_t malloc_data(idk_data_t * idk_ptr, size_t length, void ** ptr)
{
    idk_callback_status_t status;
    size_t  size = length, len;
    idk_request_t request_id;
    void * p;

    request_id.base_request = idk_base_malloc;
    status = idk_ptr->callback(idk_class_base, request_id, &size, sizeof size, &p, &len);
    if (status == idk_callback_abort)
    {
        idk_ptr->error_code = idk_configuration_error;
    }
    else if (status == idk_callback_continue)
    {
        *ptr = p;
        add_malloc_stats(p, size);
    }
    return status;

}

static idk_callback_status_t free_data(idk_data_t * idk_ptr, void * ptr)
{
    idk_callback_status_t       status;
    idk_request_t request_id;

    request_id.base_request = idk_base_free;
    status = idk_ptr->callback(idk_class_base, request_id, ptr, sizeof(void *), NULL, NULL);
    if (status == idk_callback_abort)
    {
        idk_ptr->error_code = idk_configuration_error;
    }
    else if (status == idk_callback_continue)
    {
        del_malloc_stats(ptr);
    }

    return status;
}

static idk_facility_t * get_facility_data(idk_data_t * idk_ptr, uint16_t facility_num)
{
    idk_facility_t * fac_ptr;

    for (fac_ptr = idk_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {
            break;
        }
    }

    return fac_ptr;
}

static idk_callback_status_t add_facility_data(idk_data_t * idk_ptr, uint16_t facility_num, idk_facility_t ** fac_ptr, size_t size,
                                                                               idk_facility_process_cb_t discovery_cb, idk_facility_process_cb_t process_cb)
{
    idk_callback_status_t status;
    idk_facility_t * ptr = NULL;

    status = malloc_data(idk_ptr, size, (void **)&ptr);
    if (status == idk_callback_continue && ptr != NULL)
    {
        ptr->facility_num = facility_num;
        ptr->size = size;
        ptr->discovery_cb = discovery_cb;
        ptr->process_cb = process_cb;
        ptr->packet = NULL;
        ptr->next = idk_ptr->facility_list;
        idk_ptr->facility_list = ptr;
    }

    *fac_ptr = ptr;
    return status;
}

static idk_callback_status_t del_facility_data(idk_data_t * idk_ptr, uint16_t facility_num)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_facility_t * fac_ptr, * prev_ptr = NULL, * next_ptr;

    for (fac_ptr = idk_ptr->facility_list; fac_ptr != NULL; fac_ptr = fac_ptr->next)
    {
        if (fac_ptr->facility_num == facility_num)
        {
            next_ptr = fac_ptr->next;

            status = free_data(idk_ptr, fac_ptr);
            if (status == idk_callback_continue)
            {
                if (prev_ptr != NULL)
                {
                    prev_ptr->next = next_ptr;
                }
                else
                {
                    idk_ptr->facility_list = next_ptr;
                }
            }
            break;
        }
        prev_ptr = fac_ptr;
    }
    return status;
}
