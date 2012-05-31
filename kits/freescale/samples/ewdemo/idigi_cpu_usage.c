/*
 *  Copyright (c) 1996-2012 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees+
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
	  
	// cpu_usage_baseline = total_loop1 / 5;
	  
	cpu_usage_baseline = 0x270664;  // For consistency, set to known good value       
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

#ifdef DEBUG_CPU_USAGE
   	    APP_DEBUG("idigi_cpu_usage: cpu_usage_baseline [%x] elapsed_loop1 [%x] cpu_usage [%x]\n", cpu_usage_baseline, elapsed_loop1, cpu_usage);
#endif
    }
}
