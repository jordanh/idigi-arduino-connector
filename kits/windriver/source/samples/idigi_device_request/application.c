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
#include <fcntl.h>
#include <stdbool.h>
#include "idigi_connector.h"

typedef idigi_app_error_t (* idigi_cmd_handler_t)(void const * const data, unsigned short length);

static idigi_app_error_t led_on(void const * const data, unsigned short length);
static idigi_app_error_t led_off(void const * const data, unsigned short length);

/* Table containing list of device requests along with request handler routines */
struct command_info_t
{
     char const * const name;
     idigi_cmd_handler_t cmd_handler;

} const command_table[] =
{
     /* Request       Command Handler  */
     {"LED_ON",         led_on},
     {"LED_OFF",        led_off},
     {NULL,             NULL}
};

static bool leds_initialized=false;

static bool ok_response=false;

static void idigi_status(idigi_connector_error_t const status, char const * const status_message)
{
    printf("idigi_status: status update %d [%s]\n", status, status_message);
}

/* We are passed commands via the device request callback */
idigi_app_error_t device_request_callback(char const * const target, idigi_connector_data_t * const request_data)
{
    idigi_app_error_t status=idigi_app_invalid_parameter;
    int i;

    ok_response = false;

    if (request_data->error != idigi_connector_success)
    {
        printf("devcie_request_callback: error [%d]\n", request_data->error);
        goto error;
    }

    printf("device_request_callback: command [%s]\n", target);

    /* 
     * The command is passed as the target, loop through our table and find the routine 
     * to handle the command. 
     */

    for (i=0; command_table[i].name != NULL; i++)
    {
        if (strcmp(target, command_table[i].name) == 0)
        {   
            status = command_table[i].cmd_handler(request_data->data_ptr, request_data->length_in_bytes);
            ok_response = true;
            break;
        }
    }

    status = idigi_app_success;

error:
    return status;
}

size_t device_response_callback(char const * const target, idigi_connector_data_t * const response_data)
{
    static char rsp_string[] = "iDigi Connector device response OK\n";
    static char error_rsp_string[] = "Invalid Target\n";
    size_t const len;
    size_t const bytes_to_copy;

    if (response_data->error != idigi_connector_success)
    {
        printf("devcie_response_callback: error [%d]\n", response_data->error);
        goto error;
    }
    if (ok_response) 
    {
        memcpy(response_data->data_ptr, rsp_string, bytes_to_copy);
        len = sizeof rsp_string - 1;
    }
    else
    {
        memcpy(response_data->data_ptr, error_rsp_string, bytes_to_copy);
        len = sizeof error_rsp_string - 1
    }
    response_data->flags = IDIGI_FLAG_LAST_DATA;

    bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;

    printf("devcie_response_callback: data- %s\n", rsp_string);

error:
    return bytes_to_copy;
}


int application_start(void)
{
    idigi_connector_data_t ic_data;
    idigi_connector_error_t ret;
    int status=-1;

    printf("application_start: calling idigi_connector_start\n");
    ret = idigi_connector_start(idigi_status);
    if (ret != idigi_connector_success)
    {
        printf("idigi_connector_start failed [%d]\n", ret);
        goto error;
    }

    printf("application_start: calling idigi_register_device_request_callbacks\n");
    ret = idigi_register_device_request_callbacks(device_request_callback, device_response_callback);
    if (ret != idigi_connector_success)
    {
        printf("idigi_register_device_request_callbacks failed [%d]\n", ret);
        goto error;
    }
 
    status = 0;

error:
    return status;
    
}

static idigi_app_error_t initialize_leds(void)
{
    idigi_app_error_t status=idigi_app_invalid_parameter;
    FILE *fp;
    char set_value[16];

    if (leds_initialized) 
    {
        goto error;
    }
    
    printf("Initializing LED's\n");

	if ((fp = fopen("/sys/class/gpio/export", "w")) == NULL)
	{
		printf("Cannot open GPIO export file.\n");
        goto error;	
	}

    rewind(fp);
    strcpy(set_value,"0");
    fwrite(&set_value, sizeof(char), 1, fp);
    rewind(fp);
    strcpy(set_value,"1");
    fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);

	if ((fp = fopen("/sys/class/gpio/gpio0/direction", "rb+")) == NULL)
	{
		printf("Cannot open GPIO 0 direction file.\n");
        goto error;	
    }


    rewind(fp);
    strcpy(set_value,"out");
    fwrite(&set_value, 3, 1, fp);
	fclose(fp);

	if ((fp = fopen("/sys/class/gpio/gpio1/direction", "rb+")) == NULL)
	{
		printf("Cannot open GPIO 1 direction file.\n");
        goto error;	
	}

    rewind(fp);
    strcpy(set_value,"out");
    fwrite(&set_value, 3, 1, fp);
	fclose(fp);

    leds_initialized = true;

    status = idigi_app_success;

error:
    return status;
}

static idigi_app_error_t led_on(void const * const data, unsigned short length)
{
    idigi_app_error_t status=idigi_app_invalid_parameter;
    FILE *fp;
    char set_value[16];
    
    printf("led_on\n");

    if (!leds_initialized) {
        initialize_leds();
    }

	if ((fp = fopen("/sys/class/gpio/gpio0/value", "rb+")) == NULL)
	{
		printf("Cannot open GPIO0 value file.\n");
        goto error;	
	}

    /* Set the value of the LED 0= off */
    rewind(fp);
    strcpy(set_value,"0");
    fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);
	
	if ((fp = fopen("/sys/class/gpio/gpio1/value", "rb+")) == NULL)
	{
		printf("Cannot open GPIO0 value file.\n");
        goto error;	
	}

    /* Set the value of the LED 0= off */
    rewind(fp);
    strcpy(set_value,"0");
    fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);


    status = idigi_app_success;

error:
    return status;
}

static idigi_app_error_t led_off(void const * const data, unsigned short length)
{
    idigi_app_error_t status=idigi_app_invalid_parameter;
    FILE *fp;
    char set_value[16];
    
    printf("led_off\n");

    if (!leds_initialized) {
        initialize_leds();
    }

	if ((fp = fopen("/sys/class/gpio/gpio0/value", "rb+")) == NULL)
	{
		printf("Cannot open GPIO direction file.\n");
        goto error;	
	}

    /* Set the value of the LED 0= off */
    rewind(fp);
    strcpy(set_value,"1");
    fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);
	
	if ((fp = fopen("/sys/class/gpio/gpio1/value", "rb+")) == NULL)
	{
		printf("Cannot open GPIO direction file.\n");
        goto error;	
	}

    /* Set the value of the LED 0= off */
    rewind(fp);
    strcpy(set_value,"1");
    fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);

    status = idigi_app_success;

error:
    return status;
}
