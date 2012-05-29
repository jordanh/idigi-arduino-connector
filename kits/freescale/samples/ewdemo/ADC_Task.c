/**************************************************************************
*
* FileName: ADC_Task.c
*
* Comments:
* Task initializes ADC1 module, and then samples Channel 20 (temp sensor)
*  and Channel 26 (connected to on-board pot).
*
* This code does not use the MQX ADC driver since it has not been ported yet to
*  Kinetis. All the code in this file is bare-metal
*
*
***************************************************************************/

#include <string.h>
#include "tower_demo.h"
#include "adc16.h"

extern int pulse_generator_rate;

LWGPIO_STRUCT pulse_gen_gpio;

tADC_Config Master_Adc_Config;
tADC_Cal_Blk CalibrationStore[2];

float num_ad_samples = 0, ad_running_total = 0, ad_running_average;
int ad_sample_available = 0;

void InitializeADC();

void ADC_sample_pulse_gen(void)
{
	float pulse_gen;
	unsigned int value;
	
    //Read pulse generator current voltage (0 - 5)
    ADC1_SC1A = AIEN_OFF | ADC_SC1_ADCH(19);  //ADC1_DM0
    while ( (ADC1_SC1A & ADC_SC1_COCO_MASK) == COCO_NOT );  // Wait conversion end
    value=ADC1_RA;
    
    //Convert to a voltage
    if(value>65450)
    {
        pulse_gen=5;
    }
    else
    {
    	pulse_gen=((float)value/65535)*5;
    }

    Sensor.pulse_gen=(float)pulse_gen;
    //printf("pulse gen int = %d\n",Sensor.pulse_gen_int);

    /* Take AD average every 100HZ */
    if (num_ad_samples == 0) /* means 1st time */
    {
    	ad_running_total = Sensor.pulse_gen;
        num_ad_samples = 1;
    }
    else
    {
        num_ad_samples ++;
        ad_running_total = ad_running_total + Sensor.pulse_gen;
    }
    
    ad_running_average = ad_running_total / num_ad_samples;
}

/*
 * Pulse the GPIO line at the "pulse_generator_rate"
 * to create a pulse generator signal.
 */
void idigi_gpio_pulse_task(unsigned long initial_data)
{    
    /* Initialize LED's */
    if (TRUE == lwgpio_init(&pulse_gen_gpio, PULSE_GEN_GPIO, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))    {
        /* swich pin functionality (MUX) to GPIO mode */
        lwgpio_set_functionality(&pulse_gen_gpio, BSP_PULSE_GEN_MUX_GPIO);
    }

    while (1)
    {
    	int i, pulse_width;
    	
        if (pulse_generator_rate)
        {
            lwgpio_set_value(&pulse_gen_gpio, LWGPIO_VALUE_LOW); /* set pin to 0 */
            
            /* Hold for pulse width, rate=50 sleep 500 rate=25 sleep 750 rate 75 sleep 250 */
            pulse_width = ((1000 - (pulse_generator_rate * 10)) / 5);
            for (i = 0; i < pulse_width; i ++)
            {
            	_time_delay(5);
            	ADC_sample_pulse_gen();
            }

            lwgpio_set_value(&pulse_gen_gpio, LWGPIO_VALUE_HIGH); /* set pin to 1 */
            
            /* Hold for pulse width */
            pulse_width = pulse_generator_rate * 2;
            for (i = 0; i < pulse_width; i ++)
            {
            	_time_delay(5);
            	ADC_sample_pulse_gen();
            }
            ad_sample_available = 1;
            while (ad_sample_available)
            {
            	_time_delay(100);
            }
        }
        else
        {       	
            /* Set output low */
            if (output_port)
            {
                ioctl(output_port, GPIO_IOCTL_WRITE_LOG0, NULL);
            }
 
        	ADC_sample_pulse_gen();
            ad_sample_available = 1;
            while (ad_sample_available)
            {
            	_time_delay(100);
            }
        	_time_delay(1000);
        }
    }
}

  /*
   * This task configures the ADC module
   *
   * It then samples Channel 26 which has the ADC pulse generator sensor, and calculates
   *   the temperature
   *
   * Then it samples Channel 20 to read the value on the potentiometer
   *
   * Task then waits 500ms before sampling values again
   */
void ADC_Task()
{
  float vpulse_gen;
  float pot;
  unsigned int value;

  printf("Starting ADC Task\n");

  InitializeADC();

  while(1)
  {
    //Read Pot value
    ADC1_SC1A = AIEN_OFF | ADC_SC1_ADCH(20);
    while ( (ADC1_SC1A & ADC_SC1_COCO_MASK) == COCO_NOT );  // Wait conversion end
    value=ADC1_RA;
    //printf("pot value: %d\n",value);

    //Convert to percentage
    if(value>65450)
    {
    	Sensor.pot_int =(uint_8)5;
        Sensor.pot_dec=(uint_8)0;
    }
    else
    {
      pot=((float)value/65535)*5;
      Sensor.pot_int = (uint_8)pot;
      Sensor.pot_dec=(uint_8)((int)(pot*100))%10;
    }
   
    //Time Delay task to allow other tasks to run
    _time_delay(500);
  }
}

void InitializeADC()
{
   /* Turn on the ADCo and ADC1 clocks */
  SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );
  SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK );

  /* setup the initial configuration */
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_4) | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16)| ADC_CFG1_ADICLK(ADICLK_BUS);
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA | ADACKEN_DISABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
  Master_Adc_Config.COMPARE1 = 0x1234u ;
  Master_Adc_Config.COMPARE2 = 0x5678u ;
  Master_Adc_Config.STATUS2  = ADTRG_SW | ACFE_DISABLED | ACFGT_GREATER | ACREN_ENABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_DISABLED | ADC_SC3_AVGS(AVGS_32);
  Master_Adc_Config.PGA      = PGAEN_DISABLED | PGACHP_NOCHOP | PGALP_NORMAL | ADC_PGA_PGAG(PGAG_64);
  Master_Adc_Config.STATUS1A = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);
  Master_Adc_Config.STATUS1B = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);

  ADC_Config_Alt(ADC1_BASE_PTR, &Master_Adc_Config);  // config ADC
  ADC_Cal(ADC1_BASE_PTR);                    // do the calibration
  ADC_Read_Cal(ADC1_BASE_PTR,&CalibrationStore[1]);   // store the cal
}

