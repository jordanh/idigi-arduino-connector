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
#include "mqx.h"
#include "mqx_prv.h"
#include <errno.h>
#include <idigi_types.h>
#include "idigi_api.h"
#include "idigi_def.h"
#include <platform.h>

extern unsigned long cpu_usage_baseline;

unsigned long cpu_usage = 0, elapsed_loop1;

void init_cpu_usage(void)
{
    unsigned long initial_loop1, elapsed_loop1, average_loop1, total_loop1 = 0;
    volatile KERNEL_DATA_STRUCT _PTR_ kernel_data;
    int i;
	//_mqx_get_kernel_data(kernel_data);
	  
	kernel_data = _mqx_kernel_data;
	  
	/*
	 * We first need to establish a baseline for 0 cpu utilization
	 * so we run a test 5 times for 1 second to determine the
	 * average size for the count in kernel_data->IDLE_LOOP1 after
	 * 1 second has elapsed.
	 */

	for (i = 0; i < 5; i ++)
	{
	    kernel_data->IDLE_LOOP1 = 0;
	    kernel_data->IDLE_LOOP2 = 0;
	    kernel_data->IDLE_LOOP3 = 0;
	    kernel_data->IDLE_LOOP4 = 0;
	  
	    initial_loop1 = kernel_data->IDLE_LOOP1;
	  
	    _time_delay(1000);
	  
	    elapsed_loop1 = kernel_data->IDLE_LOOP1 - initial_loop1;
	    total_loop1 += elapsed_loop1;
	}
	
	cpu_usage_baseline = total_loop1 / 5;
	
	APP_DEBUG("cpu_usage_baseline [%x]\n", cpu_usage_baseline);

#ifdef TWR_K53N512
	if (cpu_usage_baseline > 0x900000)
	    cpu_usage_baseline = 0x7d547e;
#else
	cpu_usage_baseline = 0x270664;
#endif
}

void idigi_cpu_usage(unsigned long initial_data)
{
	volatile KERNEL_DATA_STRUCT _PTR_ kernel_data;
	int i;
	
	kernel_data = _mqx_kernel_data;
	  
    while (1)
    {
    	unsigned long initial_loop1, initial_loop2, average_loop1;
    	
    	_time_delay(5000);
    	
	    kernel_data->IDLE_LOOP1 = 0;
  	    kernel_data->IDLE_LOOP2 = 0;
   	    kernel_data->IDLE_LOOP3 = 0;
   	    kernel_data->IDLE_LOOP4 = 0;
    	  
   	    initial_loop1 = kernel_data->IDLE_LOOP1;
    	  
   	    _time_delay(1000);
   	    	    
   	    if (initial_loop1 > kernel_data->IDLE_LOOP1)
   	    {
   	    	elapsed_loop1 = 0xffff - initial_loop1 + kernel_data->IDLE_LOOP1 + 1;
#ifdef DEBUG_CPU_USAGE
   	   	    APP_DEBUG("idigi_cpu_usage: initial_loop1 [%x] kernel_data->IDLE_LOOP1 [%x]elapsed_loop1 [%x]\n", initial_loop1, kernel_data->IDLE_LOOP1, elapsed_loop1);
#endif
   	    }
   	    else
   	    {
   	        elapsed_loop1 = kernel_data->IDLE_LOOP1 - initial_loop1;
   	    }

   	    // Ignore reading if elapsed time > cpu_usage_baseline
   	    if (elapsed_loop1 <= cpu_usage_baseline)
   	    {
   	   	    cpu_usage = 100 - ((elapsed_loop1 * 100)/cpu_usage_baseline);
   	    }
   	    else
   	    {
   	    	if (cpu_usage == 0)
   	    	    cpu_usage = 6;
   	    }

#ifdef DEBUG_CPU_USAGE
   	    APP_DEBUG("idigi_cpu_usage: cpu_usage_baseline [%x] elapsed_loop1 [%x] cpu_usage [%x]\n", cpu_usage_baseline, elapsed_loop1, cpu_usage);
#endif
    }
}
