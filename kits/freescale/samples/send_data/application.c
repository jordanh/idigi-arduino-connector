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

#include <stdio.h>
#include "platform.h"
#include "idigi_connector.h"
#include <io_gpio.h>

static LWGPIO_STRUCT push_button;

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    APP_DEBUG("idigi_status: status update %d [%s]\n", status, status_message);
}

int application_start(void)
{
    idigi_connector_error_t ret;
    int status=-1;

    /* Initialize Push Buttons */
    if (!lwgpio_init(&push_button, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button 1 GPIO as input failed.\n");
        goto error;
    }
    /* set pin functionality (MUX) to GPIO*/
    lwgpio_set_functionality(&push_button, BSP_BUTTON1_MUX_GPIO);
    lwgpio_set_attribute(&push_button, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    APP_DEBUG("application_start: calling idigi_connector_start\n");
    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_connector_start failed [%d]\n", ret);
        goto error;
    }

    do
    {
        static idigi_connector_data_t ic_data = {0};
        static char buffer[] = "iDigi Device application data!\n";

        if (lwgpio_get_value(&push_button) == LWGPIO_VALUE_LOW)
        {
            APP_DEBUG("Sending data to cloud using idigi_send_data...\n");
            ic_data.data_ptr = buffer;
            ic_data.length_in_bytes = sizeof buffer - 1;
            ret = idigi_send_data("test/test.txt", &ic_data, NULL);
            _time_delay(100);
        }
        else
        {
            _time_delay(10);
        }

        if (ret != idigi_connector_success)
        {
            APP_DEBUG("\nSend failed [%d]\n", ret);
        }
        else
        {
            APP_DEBUG("\nSend completed\n");
        }
    } while ((ret == idigi_connector_init_error) || (ret == idigi_connector_success));

    status = 0;

error:
    return status;
}
