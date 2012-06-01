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
#include <lwevent.h>
#include "tower_demo.h"
#include "main.h"
#include "idigi_api.h"
#include "idigi_def.h"
#include "platform.h"
#include "idigi_connector.h"

#ifndef APP_DEBUG
#define APP_DEBUG   _io_printf
#endif

void disable_flash_cache(void);

/* Global Variables */
SENSOR_DATA Sensor;
DEMO_MODE mode;
LWSEM_STRUCT touch_sem;

static char accel_file_path[] = "accel.dat";
static char ad_file_path[] = "ad.dat";
static char pot_file_path[] = "pot.dat";
static char touch_pad_file_path[] = "leds.dat";
static char sysinfo_file_path[] = "sysinfo.txt";
static char syslog_file_path[] = "syslog.txt";

char idigi_led_override[8] = "False";
char idigi_watchdog_caused_reset[8] = "False";

uint_8 ad_average_dec, ad_average_int; 
int failures_to_detect_GPIO_TO_A2D_connections;
int pulse_generator_rate = 0;

static int led_blinkrate = 1;
static int task_terminations = 0;
static int task_startups = 0;
static int put_led_state = 0;
static int valid_leds_request = 0;
static int invalid_leds_request = 0;
static int valid_gpio_request = 0;
static int invalid_gpio_request = 0;

static char unknown_target_error_response_data[] = "Unknown target_name";
static char const device_request_LEDS_target[] = "LEDS";
static char const device_request_GPIO_target[] = "GPIO";
static char leds_callback_response_data[] = "LEDS successful callback response";
static char gpio_callback_response_data[] = "GPIO successful callback response";
static char touchpad_button_pressed[] = "slow touch pad button pressed\n";
static char leds_callback_error_response_data[64];
static char gpio_callback_error_response_data[64];

#define BUFFER_SIZE 512

char put_temp_buffer[PUT_REQUEST_BUFFER_SIZE];
char syslog_put_request_buffer[BUFFER_SIZE];
int data_size_in_put_request_buffer = 0;

char cur_pot_data_int = -1, cur_pot_data_dec = -1;
char cur_accel_data[3] = {-1, -1, -1};

extern unsigned long cpu_usage, elapsed_loop1;
char device_request_syslog_buffer[DEVICE_REQUEST_BUFFER_SIZE];

unsigned long cpu_usage_baseline = 0;

unsigned char ad_average_int, ad_average_dec, cur_ad_average_int = 99, cur_ad_average_dec = 99;

/*
 *  This function will initiate a put request to the iDigi cloud.
 *
 *  Parameters:
 *      path            -- NUL terminated file path where user wants to store the data on the iDigi cloud.
 *      data            -- Data to write to file on iDigi cloud.
 *      content_type    -- NUL terminated content type (text/plain, text/xml, application/json, etc.
 *      length_in_bytes -- Data length in put_request
 *      flags           -- Indicates whether server should archive and/or append.
 *
 *  Return Value:
 *      idigi_success
 *      idigi_invalid_data      -- Indicates bad parameters
 *      idigi_invalid_response  -- Indicates error response from iDigi cloud
 */
idigi_status_t idigi_initiate_put_request(char const * const path, char const * const data, char const * const content_type,
		                                  size_t const length_in_bytes, unsigned int const flags) 
{
    idigi_connector_error_t ret;
    int status=-1;
    
    do
    {
        static idigi_connector_data_t ic_data = {0};
        //static char buffer[] = "iDigi Device application data!\n";

        #define WAIT_FOR_A_SECOND  1
        app_os_sleep(WAIT_FOR_A_SECOND);

        ic_data.data_ptr = (char *)data;
        ic_data.length_in_bytes = length_in_bytes;
        ic_data.flags = flags;
        ret = idigi_send_data(path, &ic_data, content_type);

    } while (ret == idigi_connector_init_error);

    if (ret != idigi_connector_success)
    {
        APP_DEBUG("\nSend failed [%d]\n", ret);
        goto error;
    }

#ifdef DEBUG_PUT_REQUEST
    APP_DEBUG("\nSend completed\n");
#endif
    
    status = 0;

error:
    
    return status;
}

/*
 * Adds a string to the syslog buffer to be sent to iDigi
 */
idigi_status_t add_to_syslog_buffer(char *syslog_string) 
{
	int size;
	
	size = strlen(syslog_string);
	if (size > BUFFER_SIZE)
		return idigi_invalid_data_size;
	
    snprintf((char *)&syslog_put_request_buffer[data_size_in_put_request_buffer],
    		  BUFFER_SIZE - data_size_in_put_request_buffer,
    		  syslog_string);
    
    data_size_in_put_request_buffer += strlen(syslog_string);
       
    return idigi_success;
}

void InitializeK60TowerDemo(void)
{
    unsigned short reset_count;
    idigi_status_t rc;
    
	disable_flash_cache();

	printf("\n\nKinetis TWR-K60N512 MQX %s Demo\n", _mqx_version);
	
	init_cpu_usage();

	/* Initialize global variables */
	mode=TOUCH; //Default to TSI toggle mode	
    _lwsem_create(&touch_sem, 0); //Create semaphore with initial value of 0
	                              //used for detecting touches
    
    /* Setup GPIO */
    InitializeIO();

	/* Create Acceletometer Task to read accelerometer data */
	_task_create(0,ACCEL_TASK,0);

	/* Create ADC Task to read POT and Temp Sensor */
	_task_create(0,ADC_TASK,0);

	/* Create LED Task to display current state of the LEDs */
	_task_create(0,IDIGI_LED_TASK,0);
	
	/* Create Touch Task to change the state of the LEDs */
	_task_create(0,IDIGI_TOUCH_TASK,0);
	
	/* Create Push Button Task to check the the state of the Push Buttons */
	_task_create(0,IDIGI_BUTTON_TASK,0);

	/* Create Device Request Task */
	_task_create(0,IDIGI_APP_TASK,0);
	
	/* Create GPIO Task */
	_task_create(0,IDIGI_GPIO_TASK,0);
	
	/* Create CPU usage Task */
	_task_create(0,IDIGI_CPU_USAGE_TASK,0);

    // Check if the last reset was a watchdog reset by reading the WDOG_RSTCNT register
    reset_count = WDOG_RSTCNT;
    if (reset_count)
    {
	    APP_DEBUG("InitializeK60TowerDemo: last reset was a watchdog reset\n");

	    snprintf(idigi_watchdog_caused_reset, sizeof(idigi_watchdog_caused_reset), "True");
	    
	    // Add syslog entry for reset
        rc = add_to_syslog_buffer("Reset was a watchdog reset\n");
        if (rc)
		    APP_DEBUG("InitializeK60TowerDemo: error adding to syslog buffer\n");
	    
	    // Clear the watchdog count so a soft reset will be recognized
	    WDOG_RSTCNT = 0xffff;
	    WDOG_RSTCNT = 0x0;
    }
    else
    {
	    APP_DEBUG("InitializeK60TowerDemo: last reset was a normal reset\n");
	    
	    snprintf(idigi_watchdog_caused_reset, sizeof(idigi_watchdog_caused_reset), "False");
	    
	    // Add syslog entry for reset
        rc = add_to_syslog_buffer("Reset was a normal reset\n");
        if (rc)
		    APP_DEBUG("InitializeK60TowerDemo: error adding to syslog buffer\n");
    }

	/* Initialize unexpected ISR */
	_int_install_unexpected_isr();
	  
	/* Setup TSI last for initialization */
	TSI_Start();
}

/*
 * Temporary task to cause CPU usage to go up.
 */
void idigi_utility_task1(unsigned long initial_data)
{
    while (1)
    {
    	APP_DEBUG("idigi_utility_task1: is running\n");
        _time_delay(1);    	
    }
}

/*
 * Temporary task to cause CPU usage to go up.
 */
void idigi_utility_task2(unsigned long initial_data)
{
    while (1)
    {
    	APP_DEBUG("idigi_utility_task2: is running\n");
        _time_delay(1);   	
    }
}

/*
 * Read the push buttons to determine if the utility threads
 * should be started.
 */
void idigi_button_task(unsigned long initial_data)
{
    static unsigned int sw1_state = 1;
    idigi_status_t rc;
    
	while (1)
	{
		if (GetInput(btn1))
        {
    	    _mqx_uint taskid1;
    	    _mqx_uint taskid2;
    	
    	    /* Wait a second to see if SW2 is also pressed */
	        _time_delay(1000);
    	
	        if (GetInput(btn2))
    	    {
    		    /* Debounce */
    	        while (GetInput(btn1) || GetInput(btn2));
    	    
        	    APP_DEBUG("idigi_button_task: sw1 and sw2 are pressed to calibrate the accelerometer\n");
    	    
    	        // Add syslog entry
    	        rc = add_to_syslog_buffer("sw1 and sw2 are pressed, calibrating the accelerometer\n");
    	        if (rc)
    		        APP_DEBUG("idigi_button_task: error adding to syslog buffer\n");
    	    
          	    calibrate();
    	    }
    	    else
      	    {
#if 0
    	        TD_STRUCT_PTR td_ptr;
#endif
    	        _mqx_uint status;
    		
    	        while (GetInput(btn1));
    	
         	    /*
     	         *  Invoke a 3 state machine and send a new syslog each time
    	         * 
   	             *  1.  First state: start an instance of the utility thread 
     	         *  2.  Second state: start another instance of the utility thread
   	             *  3.  Third state: kill the two utility threads
    	         */
    	    
    	        switch (sw1_state)
    	        {        	        
    	            case 1:
    	                APP_DEBUG("idigi_button_task: sw1 is pressed, creating 1st utility thread\n");
                    
                        sw1_state = 2;   	        
                        /* Create the first utility Task */
        	            taskid1 = _task_create(0,IDIGI_UTILITY_1, 0);
        	            task_startups ++;
          	    
            	        // Add syslog entry
            	        rc = add_to_syslog_buffer("sw1 is pressed, creating 1st utility thread\n");
            	        if (rc)
            		        APP_DEBUG("idigi_button_task: error adding to syslog buffer\n");               
                        break;
    	            case 2:
    	                APP_DEBUG("idigi_button_task: sw1 is pressed, creating 2nd utility thread\n");
                    
                        sw1_state = 3;    	            
    	                /* Create the second utility Task */
            	        taskid2 = _task_create(0,IDIGI_UTILITY_2, 0);
        	            task_startups ++;

            	        // Add syslog entry
            	        rc = add_to_syslog_buffer("sw1 is pressed, creating 2nd utility thread\n");
            	        if (rc)
            		        APP_DEBUG("idigi_button_task: error adding to syslog buffer\n");               
                        break;
    	            case 3:
    	                APP_DEBUG("idigi_button_task: sw1 is pressed, kill the two utility threads\n");
    	        	                    
                        status = _task_destroy(taskid1);
           
                        if (status == MQX_OK)
                        {
                    	    task_terminations ++;
                	        APP_DEBUG("idigi_button_task: Utility task 1 is destroyed\n");
                        }
                        else
                        {               	
                	        APP_DEBUG("idigi_button_task: _task_destroy(taskid1) failed [%d]\n", status);
                        }
                    
                        status = _task_destroy(taskid2);
                    
                        if (status == MQX_OK)
                        {
                    	    task_terminations ++;
                	        APP_DEBUG("idigi_button_task: Utility task 2 is destroyed\n");
                        }
                        else
                        {
                	        APP_DEBUG("idigi_button_task: _task_destroy(taskid2) failed [%d]\n", status);
                        }
                    
                        sw1_state = 1;
            	    
            	        // Add syslog entry
            	        rc = add_to_syslog_buffer("sw1 is pressed, kill the two utility threads\n");
            	        if (rc)
            		        APP_DEBUG("idigi_button_taskn: error adding to syslog buffer\n");                
                        break;       	    
    	        }
    	    
#if 0
  	            td_ptr = (TD_STRUCT_PTR)_task_get_td( taskid1 );
  	        
  	            APP_DEBUG("idigi_button_task: taskid1 state {%x]\n", td_ptr->STATE);
#endif
    	    }
        }

	    if (GetInput(btn2))
        {       	
    	    /* Wait a second to see if SW1 is also pressed */
	        _time_delay(1000);
    	
	        if (GetInput(btn1))
    	    {
    		    /* Debounce */
    	        while (GetInput(btn1) || GetInput(btn2));
    	    
        	    APP_DEBUG("idigi_button_task: sw1 and sw2 are pressed to calibrate the accelerometer\n");
    	    
    	        // Add syslog entry
    	        rc = add_to_syslog_buffer("sw1 and sw2 are pressed, calibrating the accelerometer\n");
    	        if (rc)
    		        APP_DEBUG("idigi_button_task: error adding to syslog buffer\n");
    	    
        	    calibrate();
    	    }
    	    else
    	    {
    		    int rc;
    		
    	        while (GetInput(btn2));
    	
        	    /*
	             *     At button press, invoke a reset event
	             */
    	
    	        APP_DEBUG("idigi_button_task: sw2 is pressed to invoke reset\n");
   	    
    	        // Add syslog entry
    	        rc = add_to_syslog_buffer("sw2 was pressed to invoke reset\n");
    	        if (rc)
    		        APP_DEBUG("idigi_button_task: error adding to syslog buffer\n");
    	        
    	        // Wait 5 seconds for syslog to be sent
    	        _time_delay(5000);
    	
    	        ic_watchdog_reset();
            }
        }
    
        //Time Delay task to allow other tasks to run
        _time_delay(10);
	}
}

/*
 * Read the touch pads to determine if the Led blink
 * should be slow, fast or none.
 */
void idigi_touch_pad_task(unsigned long initial_data)
{
    char key;    
    idigi_status_t rc;
    
    while (1)
    {       
        mode=TOUCH;
        
    	//Reset semaphore
    	touch_sem.VALUE=0;
    	  
        //Wait for TSI touch
        if(_lwsem_wait_ticks(&touch_sem,1000)!= MQX_OK)
            continue;
        
        WaitForTouch();

        //Wait for release of touch
        WaitForRelease();

        //Get the key pressed
        key=LastKeyPressed();

#ifndef TWR_K53N512
        if (key == 1)
#else
        if (key == 0)
#endif
        {
    	    led_blinkrate = 1; // slow
    	    put_led_state = 1;    	    
        	APP_DEBUG("idigi_touch_pad_task: slow button pressed\n");
    	    
        	snprintf(idigi_led_override, sizeof(idigi_led_override), "True");
        	
    	    // Add syslog entry
            rc = add_to_syslog_buffer("slow touch pad button pressed\n");
            if (rc)
    		    APP_DEBUG("idigi_touch_pad_task: error adding to syslog buffer\n");
        }
#ifndef TWR_K53N512
        else if (key == 2)
#else
        else if (key == 1)
#endif
        {
    	    led_blinkrate = 2; // Fast
    	    put_led_state = 1;
    	    APP_DEBUG("idigi_touch_pad_task: fast button pressed\n");

    	    snprintf(idigi_led_override, sizeof(idigi_led_override), "True");
    	    
    	    // Add syslog entry
            rc = add_to_syslog_buffer("fast touch pad button pressed\n");
            if (rc)
    		    APP_DEBUG("idigi_touch_pad_task: error adding to syslog buffer\n");
        }
        else if (key == 3)
        {
     	    led_blinkrate = 0; // None
    	    put_led_state = 1;
    	    APP_DEBUG("idigi_touch_pad_task: stop button pressed\n");

    	    snprintf(idigi_led_override, sizeof(idigi_led_override), "True");
    	    
    	    // Add syslog entry
            rc = add_to_syslog_buffer("stop touch pad button pressed\n");
            if (rc)
    		    APP_DEBUG("idigi_touch_pad_task: error adding to syslog buffer\n");
        }
    }
}

/*
 * Blink the Led's fast, slow or not at all
 */
void idigi_led_task(unsigned long initial_data)
{
    while (1)
    {
    	if (led_blinkrate)
    	{
            SetOutput(led1,ON);
            SetOutput(led2,OFF);
            
#ifndef TWR_K53N512
            SetOutput(led3,ON);
            SetOutput(led4,OFF);
#endif
            
	        if (led_blinkrate == 2)	    
		        _time_delay(100);
	        else	    
		        _time_delay(1000);
	    
            SetOutput(led1,OFF);
            SetOutput(led2,ON);
            
#ifndef TWR_K53N512
            SetOutput(led3,OFF);
            SetOutput(led4,ON);
#endif
        
	        if (led_blinkrate == 2)	    
		        _time_delay(100);
	        else	    
		        _time_delay(1000);
    	}
    	else
    	{
	        _time_delay(1000);
    	}	
    }	
}

size_t device_response_callback(char const * const target, idigi_connector_data_t * const response_data)
{
    /* static char rsp_string[] = "iDigi Connector device response!\n"; */
    size_t len;
    size_t bytes_to_copy;

    if (response_data->error != idigi_connector_success)
    {
        APP_DEBUG("device_response_callback: error [%d]\n", response_data->error);
        goto error;
    }
    
    memset(response_data->data_ptr, 0, response_data->length_in_bytes);
    
    if (valid_leds_request)
    {
        len = strlen(leds_callback_response_data);
        bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
        memcpy(response_data->data_ptr, leds_callback_response_data, bytes_to_copy);
    }
    else if (invalid_leds_request)
    {
        len = strlen(leds_callback_error_response_data);
        bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
        memcpy(response_data->data_ptr, leds_callback_error_response_data, bytes_to_copy);
    }
    else if (valid_gpio_request)
    {
        len = strlen(gpio_callback_response_data);
        bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
        memcpy(response_data->data_ptr, gpio_callback_response_data, bytes_to_copy);
    }
    else if (invalid_gpio_request)
    {
    	len = strlen(gpio_callback_error_response_data);
        bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
        memcpy(response_data->data_ptr, gpio_callback_error_response_data, bytes_to_copy);
    }
    else
    {
    	len = strlen(unknown_target_error_response_data);
        bytes_to_copy = (len < response_data->length_in_bytes) ? len : response_data->length_in_bytes;
        memcpy(response_data->data_ptr, unknown_target_error_response_data, bytes_to_copy);
    }

    response_data->flags = IDIGI_FLAG_LAST_DATA;

    APP_DEBUG("device_response_callback: target [%s], data- %s\n", target, response_data->data_ptr);

error:
    return bytes_to_copy;
}

idigi_app_error_t device_request_callback(char const * const target, idigi_connector_data_t * const request_data)
{
	size_t bytes_to_send;
	
    idigi_app_error_t status=idigi_app_success;
    idigi_status_t rc;
    char *data = (char *)request_data->data_ptr;
    data[request_data->length_in_bytes] = 0; // Nul Terminate

	valid_leds_request = 0;
	invalid_leds_request = 0;
	valid_gpio_request = 0;
	invalid_gpio_request = 0;

    snprintf(device_request_syslog_buffer, DEVICE_REQUEST_BUFFER_SIZE,"\"%.*s\" for target = \"%s\"\n",
    		 request_data->length_in_bytes,
             data, target);
    
    // Add syslog entry
    rc = add_to_syslog_buffer(device_request_syslog_buffer);
    
    if (rc)
	    APP_DEBUG("device_request_callback: error adding to syslog buffer\n");
    
	if (strcmp(target, device_request_LEDS_target) == 0)
	{   
	    if (strcmp(data, "fast") == 0)
	    {
	    	valid_leds_request = 1;
		    APP_DEBUG("idigi_handle_device_request: setting LED blink to fast\n");
		    led_blinkrate = 2;
    	    put_led_state = 1;
    	    snprintf(idigi_led_override, sizeof(idigi_led_override), "False");
	    }
	    else if (strcmp(data, "slow") == 0)
	    {
	    	valid_leds_request = 1;
		    APP_DEBUG("idigi_handle_device_request: setting LED blink to slow\n");
		    led_blinkrate = 1;
    	    put_led_state = 1;
    	    snprintf(idigi_led_override, sizeof(idigi_led_override), "False");
	    }
	    else if (strcmp(data, "stop") == 0)
	    {
	    	valid_leds_request = 1;
		    APP_DEBUG("idigi_handle_device_request: setting LED blink to stop\n");
		    led_blinkrate = 0;
    	    put_led_state = 1;
    	    snprintf(idigi_led_override, sizeof(idigi_led_override), "False");
	    }
	    else
	    {
	    	invalid_leds_request = 1;
	    	sprintf(leds_callback_error_response_data, "invalid LEDS setting [%s]", data);
	    	APP_DEBUG("idigi_handle_LEDS_device_request: invalid LEDS setting [%s]\n", data);
	    }
	}
	else if (strcmp(target, device_request_GPIO_target) == 0)
	{
        int cur_pulse_generator_rate = pulse_generator_rate;
        pulse_generator_rate = atoi(data);
        if (pulse_generator_rate < PULSE_GENERATOR_VALUE_MIN || pulse_generator_rate > PULSE_GENERATOR_VALUE_MAX)
        {
        	invalid_gpio_request = 1;
	    	sprintf(gpio_callback_error_response_data, "invalid GPIO setting [%d]", pulse_generator_rate);
	    	APP_DEBUG("idigi_handle_GPIO_device_request: invalid GPIO setting [%d]\n", pulse_generator_rate);
            pulse_generator_rate = cur_pulse_generator_rate;
        }
        else
        {
        	valid_gpio_request = 1;
	    	APP_DEBUG("idigi_handle_GPIO_device_request: valid GPIO setting [%d]\n", pulse_generator_rate);
        }
	}
	else
	{
		APP_DEBUG("idigi_handle_device_request: unknown target [%s]\n", target);
	}

    return status;
}

extern int firmware_download_started;
extern float num_ad_samples, ad_running_total, ad_running_average;
extern int ad_sample_available;

void idigi_app_run_task(unsigned long initial_data)
{
    MQX_TICK_STRUCT tickstart, ticknow;
    static int first_time = 1;
    int a2d_failure_check = 0;
    idigi_connector_error_t ret;

    APP_DEBUG("idigi_app_run_task: calling idigi_register_device_request_callbacks\n");
    ret = idigi_register_device_request_callbacks(device_request_callback, device_response_callback);
    if (ret != idigi_connector_success)
    {
        APP_DEBUG("idigi_register_device_request_callbacks failed [%d]\n", ret);
    }
   
    _time_get_elapsed_ticks(&tickstart);
    
    while (1)
    {
        int_32  elapsed;
        idigi_status_t status;
		unsigned long seconds_since_reset;
   	
    	if (ad_sample_available && firmware_download_started == 0)
    	{        	    
            ad_average_int = (unsigned char)ad_running_average;
        
            if (ad_average_int == 5)
        	    ad_average_dec = 0;
            else
                ad_average_dec=(unsigned char)((int)(ad_running_average*10))%10;
        
    	    /* Only send put request if data has changed from the last reading */
    	    if (cur_ad_average_int != ad_average_int ||
    	        cur_ad_average_dec != ad_average_dec)
    	    {
#ifdef DEBUG_GPIO
    		    APP_DEBUG("GPIO [%d.%d] samples [%d]\n", ad_average_int, ad_average_dec, (int)num_ad_samples);
#endif    		
    	        cur_ad_average_int = ad_average_int;
    	        cur_ad_average_dec = ad_average_dec;
    	    
       	        /* Send the a2d sample */
                snprintf(put_temp_buffer, BUFFER_SIZE, FREESCALE_DEMO_CONTROL_AD_FILE_FORMAT, ad_average_int, ad_average_dec);

            	status = idigi_initiate_put_request(ad_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);
	        	
	        	if (status)
	        	    APP_DEBUG("idigi_app_run_task: error putting to ad_file_path\n");
    	    }
        
		    num_ad_samples = 0;
		    ad_sample_available = 0;
    	}
        
    	if (firmware_download_started == 0)
    	{
        	unsigned long days = 0, hours = 0, minutes = 0, seconds = 0;
            int result;
        	
            _time_get_elapsed_ticks(&ticknow);
            elapsed = _time_diff_ticks_int32 (&ticknow, &tickstart, NULL);
        	
            /* Send a put to the iDigi server every 1 second */
            if (elapsed > 100)
            {  
                _time_get_elapsed_ticks(&tickstart);
                
    	    	/* Sense if ADC is not tied to the GPIO */
    	    	if (pulse_generator_rate == 0)
    	    	{
    	            // Check for open connection every 10 seconds
    	            if (a2d_failure_check == 10)
    	            {
    	            	a2d_failure_check = 0;
    	            	
    	    		    if (Sensor.pulse_gen > .1)
    	    			    failures_to_detect_GPIO_TO_A2D_connections ++;
    	    		    else
    	    			    failures_to_detect_GPIO_TO_A2D_connections = 0;
    	            }
    	            else
    	            {
    	            	a2d_failure_check ++;
    	            }
    	    	}
                
    	    	/* Send the initial led state to synchronize with app */
    	    	if (first_time == 1)
    	    	{
    	    		first_time = 0;               	
                	strncpy(put_temp_buffer, "slow", sizeof(put_temp_buffer));
	        	    status = idigi_initiate_put_request(touch_pad_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);        	        	
	        	    if (status)
	        	        APP_DEBUG("idigi_touch_pad_task: error putting to touch pad path\n");
    	    	}
    	    	
    	    	if (put_led_state)
    	    	{
    	    		put_led_state = 0;
    	    		
    	    		switch (led_blinkrate)
    	    		{
    	    		    case 0:      	        	
         	        	    strncpy(put_temp_buffer, "stop", sizeof(put_temp_buffer));
        	        	    status = idigi_initiate_put_request(touch_pad_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);        	        	
        	        	    if (status)
        	        	        APP_DEBUG("idigi_touch_pad_task: error putting to touch pad path\n");
        	        	    break;
    	    		    case 1:      	        	
         	        	    strncpy(put_temp_buffer, "slow", sizeof(put_temp_buffer));
        	        	    status = idigi_initiate_put_request(touch_pad_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);        	        	
        	        	    if (status)
        	        	        APP_DEBUG("idigi_touch_pad_task: error putting to touch pad path\n");
        	        	    break;
    	    		    case 2:      	        	
         	        	    strncpy(put_temp_buffer, "fast", sizeof(put_temp_buffer));
        	        	    status = idigi_initiate_put_request(touch_pad_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);        	        	
        	        	    if (status)
        	        	        APP_DEBUG("idigi_touch_pad_task: error putting to touch pad path\n");
        	        	    break;
    	    		    default:
        	        	    APP_DEBUG("idigi_touch_pad_task: unrecognized blinkrate\n");
        	        	    break;	
    	    		}
    	    	}
    	    	
            	/* Send the accerometer sample only if data has changed */
    	    	if (abs(cur_accel_data[0] - Sensor.mma7660_x) +
    	    	    abs(cur_accel_data[1] - Sensor.mma7660_y) +
    	    	    abs(cur_accel_data[2] - Sensor.mma7660_z) > 1)
            	{
            		cur_accel_data[0] = Sensor.mma7660_x;
            		cur_accel_data[1] = Sensor.mma7660_y;
            		cur_accel_data[2] = Sensor.mma7660_z;
            		
                    snprintf(put_temp_buffer, BUFFER_SIZE, FREESCALE_DEMO_CONTROL_ACCEL_FILE_FORMAT, 
                    		 cur_accel_data[0], cur_accel_data[1], cur_accel_data[2]);

    	        	status = idigi_initiate_put_request(accel_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);
    	        	
    	        	if (status)
    	        	    APP_DEBUG("idigi_app_run_task: error putting to accel path\n");   	    		
            	}
            	
    	    	/* Send a syslog if data is in syslog buffer */
    	    	if (data_size_in_put_request_buffer)
    	    	{
    	        	status = idigi_initiate_put_request(syslog_file_path, syslog_put_request_buffer, "text/plain", data_size_in_put_request_buffer, IDIGI_DATA_PUT_APPEND);
    	        	
    	        	if (status)
    	        	    APP_DEBUG("idigi_app_run_task: error putting to syslog path\n");
    	        	
    	        	data_size_in_put_request_buffer = 0;
    	    	}
                
    	    	/* Only send put request if data has changed from the last reading */
    	    	if (cur_pot_data_int != Sensor.pot_int ||
    	    	    cur_pot_data_dec != Sensor.pot_dec)
    	    	{
    	    		cur_pot_data_int = Sensor.pot_int;
    	    		cur_pot_data_dec = Sensor.pot_dec;
    	        	
                    /* Send the Pot sample */
                    snprintf(put_temp_buffer, BUFFER_SIZE, FREESCALE_DEMO_CONTROL_POT_FILE_FORMAT, cur_pot_data_int, cur_pot_data_dec);

    	        	status = idigi_initiate_put_request(pot_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);
    	        	
    	        	if (status)
    	        	    APP_DEBUG("idigi_app_run_task: error putting to pot path\n");
    	    	}
        	    
                // Send sysinfo put request            	
                result = app_os_get_system_time((uint32_t *)&seconds_since_reset);
                
                days    = seconds_since_reset / (60 * 60 * 24);                
                hours   = (seconds_since_reset / (60 * 60)) % 24;                
                minutes = (seconds_since_reset / 60) % 60;
                seconds = seconds_since_reset % 60;
                
#ifdef TWR_K53N512
                snprintf(put_request_buffer, BUFFER_SIZE, "Platform: Kinetis TWR-K53N512\n\nOS: MQX %s Build %s\n\n"
#else
                snprintf(put_temp_buffer, BUFFER_SIZE, "Platform: Kinetis TWR-K60N512\n\nOS: MQX %s Build %s\n\n"
#endif
#ifdef SYSINFO_CPU
#ifdef TWR_K53N512
                "CPU: MK53N512VMD144: K53N512 in a 144 MAPBGA with 100MHz operation\n"
#else
                "CPU: MK60N512VMD144: K60N512 in a 144 MAPBGA with 100MHz operation\n"
#endif
                "     512 Kbytes of program flash, 128 Kbytes of static RAM\n\n"
#endif
#ifdef DEBUG_CPU_USAGE
                "Uptime: days [%d] hr [%d] min [%d] sec [%d]\n\nCPU Usage: [%d percent] [%x]\n\n"
#else
                "Uptime: days [%d] hr [%d] min [%d] sec [%d]\n\nCPU Usage: [%d percent]\n\n"
#endif                    		
                "Tasks: startups [%d] terminations [%d]\n\nNetwork Connect Successes: [%d]\n\nLED Override: [%s]\n\nFailures: Detect GPIO to A/D converter [%d] "
                "Malloc [%d] Network Receive [%d] Network Send [%d] Network Connect [%d]\n\nWatchdog caused reset: [%s]\n",
                _mqx_version, _mqx_date, days, hours, minutes, seconds, cpu_usage,
#ifdef DEBUG_CPU_USAGE                    	      
                elapsed_loop1, NUM_RUNNING_TASKS + task_startups, task_terminations,
#else
                NUM_RUNNING_TASKS + task_startups, task_terminations, idigi_connect_to_idigi_successes, idigi_led_override, 
#endif                    	      
                failures_to_detect_GPIO_TO_A2D_connections, idigi_malloc_failures,
                idigi_network_receive_failures, idigi_network_send_failures, idigi_connect_to_idigi_failures, idigi_watchdog_caused_reset);

	        	status = idigi_initiate_put_request(sysinfo_file_path, put_temp_buffer, "text/plain", strlen(put_temp_buffer), 0);
            }    		
    	}
       
        //Time Delay task to allow other tasks to run
        _time_delay(10);
    }
}

/*
 * Disable the Flash Cache to work-around errata e2647 on 0M33Z marked Kinetis
 *  devices
 */
void disable_flash_cache(void)
{
  FMC_PFB0CR&=~(FMC_PFB0CR_B0DCE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0IPE_MASK);
  FMC_PFB1CR&=~(FMC_PFB1CR_B1DCE_MASK | FMC_PFB1CR_B1ICE_MASK | FMC_PFB1CR_B1DPE_MASK | FMC_PFB1CR_B1IPE_MASK);

}
