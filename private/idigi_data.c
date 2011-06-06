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

/* TODO: need to pass this handle to app */
static uint16_t data_service_handle = EMPTY_SERVICE_HANDLE;

static idigi_callback_status_t data_service_callback(uint16_t handle, uint8_t * buffer, uint16_t length)
{
    UNUSED_PARAMETER(handle);
    UNUSED_PARAMETER(buffer);
    UNUSED_PARAMETER(length);

    /* need to call user callback here */

    return idigi_callback_continue;
}

static idigi_callback_status_t data_service_delete(idigi_data_t * data_ptr)
{
    return msg_delete_facility(data_ptr, data_service_handle);
}

static idigi_callback_status_t data_service_init(idigi_data_t * data_ptr)
{
    data_service_handle = msg_init_facility(data_ptr, DATA_SERVICE_ID, data_service_callback);

    return (data_service_handle != EMPTY_SERVICE_HANDLE) ? idigi_callback_continue : idigi_callback_busy;
}

