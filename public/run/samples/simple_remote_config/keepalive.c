/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
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
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"
#include "remote_config.h"
#include "remote_config_cb.h"

typedef struct {
    unsigned int rx_keepalive;
    unsigned int tx_keepalive;
} keepalive_data_t;

keepalive_data_t nvRamKeepalive = {
        60, 90
};

idigi_callback_status_t app_keepalive_session_start(idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(response);
    printf("app_keepalive_session_start\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_keepalive_session_end(idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(response);
    printf("app_keepalive_session_end\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_keepalive_action_start(idigi_remote_group_request_t * const request,
                                                        idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    printf("app_keepalive_action_start\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_keepalive_action_end(idigi_remote_group_request_t * const request,
                                                      idigi_remote_group_response_t * const response)
{
    /* nothing to do here since we only have one group */
    UNUSED_ARGUMENT(request);
    UNUSED_ARGUMENT(response);
    printf("app_keepalive_action_end\n");
    return idigi_callback_continue;
}

idigi_callback_status_t app_keepalive_group_init(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    void * ptr;
    keepalive_data_t * keepalive;

    UNUSED_ARGUMENT(request);

    printf("app_keepalive_group_init\n");

    if (app_os_malloc(sizeof *keepalive, &ptr) != 0)
    {
        response->error_id = idigi_setting_keepalive_error_no_memory;
    }
    else
    {
        keepalive = ptr;
        *keepalive = nvRamKeepalive;
        response->user_context = keepalive;
    }

    return status;
}

idigi_callback_status_t app_keepalive_group_set(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    keepalive_data_t * const keepalive = response->user_context;

    ASSERT(keepalive != NULL);

    printf("app_keepalive_group_set\n");

    switch (request->element.id)
    {
    case idigi_setting_keepalive_rx:
        keepalive->rx_keepalive = request->element.value->integer_unsigned_value;
        break;
    case idigi_setting_keepalive_tx:
        keepalive->tx_keepalive = request->element.value->integer_unsigned_value;
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;

}

idigi_callback_status_t app_keepalive_group_get(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    keepalive_data_t * const keepalive = response->user_context;

    UNUSED_ARGUMENT(request);
    ASSERT(keepalive != NULL);

    printf("app_keepalive_group_get\n");

    switch (request->element.id)
    {
    case idigi_setting_keepalive_rx:
        response->element_data.element_value->integer_unsigned_value = keepalive->rx_keepalive;
        break;
    case idigi_setting_keepalive_tx:
        response->element_data.element_value->integer_unsigned_value = keepalive->tx_keepalive;
        break;
    default:
        ASSERT(0);
        break;
    }

    return status;

}

idigi_callback_status_t app_keepalive_group_end(idigi_remote_group_request_t * const request,
                                                 idigi_remote_group_response_t * const response)
{
   keepalive_data_t * const keepalive = response->user_context;

    UNUSED_ARGUMENT(request);

    ASSERT(keepalive != NULL);

    printf("app_keepalive_group_end\n");

    /* should start writing onto NvRam */
    nvRamKeepalive = *keepalive;

    printf("Receive (Rx) keepalive interval: %d", nvRamKeepalive.rx_keepalive);
    printf("Transmit (Tx) keepalive interval: %d", nvRamKeepalive.tx_keepalive);
    app_os_free(keepalive);

    return idigi_callback_continue;

}

void app_keepalive_session_cancel(void * context)
{
    ASSERT(context != NULL);

    printf("app_keepalive_group_cancel\n");

    app_os_free(context);

}

