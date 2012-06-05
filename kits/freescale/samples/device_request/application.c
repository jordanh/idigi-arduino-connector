/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <io_gpio.h>
#include "platform.h"
#include "idigi_connector.h"

static LWGPIO_STRUCT led1;
static LWGPIO_STRUCT led2;

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    APP_DEBUG("idigi_status: status update %d [%s]\n", status, status_message);
}

/*
 * Turn on and off LED outputs
 */
static void SetOutput(LWGPIO_STRUCT * led, boolean state)
{
    if (state)
        lwgpio_set_value(led, LWGPIO_VALUE_LOW); /* set pin to 0 */
    else
        lwgpio_set_value(led, LWGPIO_VALUE_HIGH); /* set pin to 1 */
}

static LWGPIO_STRUCT * GetLEDPin(char const * const ledString)
{
    LWGPIO_STRUCT * led = NULL;

    if (!strcmp(ledString, "LED1"))
        led = &led1;
    else if (!strcmp(ledString, "LED2"))
        led = &led2;
    else
    {
        APP_DEBUG("Unknown target [%s]\n", ledString);
    }

    return led;
}

static idigi_app_error_t device_request_callback(char const * const target, idigi_connector_data_t * const request_data)
{
    idigi_app_error_t status = idigi_app_invalid_parameter;
    LWGPIO_STRUCT * const led = GetLEDPin(target);

    if (request_data->error != idigi_connector_success)
    {
        APP_DEBUG("devcie_request_callback: target [%s], error [%d]\n", target, request_data->error);
        goto error;
    }

    if (led != NULL)
    {
        char const * const stateStr = request_data->data_ptr;
        boolean const state = (strstr(stateStr, "ON") == NULL) ? TRUE : FALSE;

        APP_DEBUG("Turning %s %s\n", state ? "ON" : "OFF", target);
        SetOutput(led, state);
        status = idigi_app_success;
    }

error:
    return status;
}

static size_t device_response_callback(char const * const target, idigi_connector_data_t * const response_data)
{
    LWGPIO_STRUCT * const led = GetLEDPin(target);
    char const * const status = (led != NULL) ? "Success" : "Failed";
    size_t bytes_to_copy = strlen(status);

    memcpy(response_data->data_ptr, status, bytes_to_copy);
    response_data->flags = IDIGI_FLAG_LAST_DATA;

    APP_DEBUG("%s action is %s\n", target, status);

error:
    return bytes_to_copy;
}


int application_start(void)
{
    idigi_connector_data_t ic_data;
    idigi_connector_error_t ret;
    int status=-1;

    /* Initialize LED's */
    if (lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE) == TRUE)
        lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);

    if (lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE) == TRUE)
        lwgpio_set_functionality(&led2, BSP_LED2_MUX_GPIO);

    APP_DEBUG("application_start: calling idigi_connector_start\n");
    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_connector_start failed [%d]\n", ret);
        goto error;
    }

    APP_DEBUG("application_start: calling idigi_register_device_request_callbacks\n");
    ret = idigi_register_device_request_callbacks(device_request_callback, device_response_callback);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_register_device_request_callbacks failed [%d]\n", ret);
        goto error;
    }

    status = 0;

error:
    return status;

}
