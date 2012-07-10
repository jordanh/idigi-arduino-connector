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

    #define  WAIT_FOR_2_SEC  2000
    _time_delay(WAIT_FOR_2_SEC);

    do
    {
        #define WAIT_FOR_10_MSEC    10
        static idigi_connector_data_t ic_data = {0};
        static boolean display_push_msg = TRUE;
        static unsigned char count = 0;
        static char buffer[] = "iDigi Device application data. Count xxxx.\n";
        size_t const buf_size = (sizeof buffer) - 1;

        if (lwgpio_get_value(&push_button) == LWGPIO_VALUE_LOW)
        {
            while (lwgpio_get_value(&push_button) == LWGPIO_VALUE_LOW)
                _time_delay(WAIT_FOR_10_MSEC);

            APP_DEBUG("Sending data to the iDigi Device Cloud using idigi_send_data...\n");
            {
                size_t const bytes_copied = snprintf(buffer, buf_size, "iDigi Device application data. Count %d.\n", count);

                APP_DEBUG("%s", buffer);
                ic_data.data_ptr = buffer;
                ic_data.length_in_bytes = bytes_copied;
                ret = idigi_send_data("test/test.txt", &ic_data, NULL);
                display_push_msg = TRUE;
            }
        }
        else
        {
            if (display_push_msg)
            {
                APP_DEBUG("\nPush SW1 to send data to the iDigi Device Cloud.\n");
                display_push_msg = FALSE;
            }

            _time_delay(WAIT_FOR_10_MSEC);
            continue;
        }

        if (ret != idigi_connector_success)
        {
            APP_DEBUG("Send failed [%d]\n", ret);
        }
        else
        {
            APP_DEBUG("Send completed\n");
            ic_data.flags = IDIGI_FLAG_APPEND_DATA;
            count++;
        }
    } while ((ret == idigi_connector_init_error) || (ret == idigi_connector_success));

    status = 0;

error:
    return status;
}
