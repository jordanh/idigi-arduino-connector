#include "tower_demo.h"
#include <io_gpio.h>
#include "DemoIO.h"

boolean OutputState[MAX_OUTPUTS];

LWGPIO_STRUCT led1, led2, led3, led4, btn1, btn2;

void irq_callback(pointer);

/* IRQ ISR called when push button is pressed */
static void irq_callback(pointer data)
{
  //Stop accelerometer output as read in Accel_Task
  print_accel=0;
#if 0
  //If SW1 is pressed, go into touch mode
  if(GetInput(SW1_INPUT))
  {
    mode=TOUCH;
    OutputAllOn();
    _lwsem_post(&touch_sem);  //Post touch semaphore to end input waiting in Game_Task
  }

  //If SW2 is pressed, go into game mode
  if(GetInput(SW2_INPUT))
  {
    mode=GAME;
    OutputAllOff();
#endif
}

/*
 * Initialize GPIO for LED's and push buttons
 * This is also where you can set up more external GPIO
 */
boolean InitializeIO(void)
{    
    /* Initialize LED's */
    if (TRUE == lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))    {
        /* swich pin functionality (MUX) to GPIO mode */
        lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);
    }
    
    if (TRUE == lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))    {
        /* swich pin functionality (MUX) to GPIO mode */
        lwgpio_set_functionality(&led2, BSP_LED2_MUX_GPIO);
    }

#ifndef TWR_K53N512
    if (TRUE == lwgpio_init(&led3, BSP_LED3, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))    {
        /* swich pin functionality (MUX) to GPIO mode */
        lwgpio_set_functionality(&led3, BSP_LED3_MUX_GPIO);
    }
    
    if (TRUE == lwgpio_init(&led4, BSP_LED4, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))    {
        /* swich pin functionality (MUX) to GPIO mode */
        lwgpio_set_functionality(&led4, BSP_LED4_MUX_GPIO);
    }
#endif 
    
    /* Initialize Push Buttons */
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
    	printf("Initializing button 1 GPIO as input failed.\n");
    }

    /* set pin functionality (MUX) to GPIO*/
    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_GPIO);
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    /* opening pins for input */
    if (!lwgpio_init(&btn2, BSP_BUTTON2, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
    	printf("Initializing button 2 GPIO as input failed.\n");
    }

    /* set pin functionality (MUX) to GPIO*/
    lwgpio_set_functionality(&btn2, BSP_BUTTON2_MUX_GPIO);
    lwgpio_set_attribute(&btn2, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
}

/*
 * Turn on and off LED outputs
 */
void SetOutput(LWGPIO_STRUCT led, boolean state)
{
	if (state == ON)
        lwgpio_set_value(&led, LWGPIO_VALUE_LOW); /* set pin to 0 */
	else
        lwgpio_set_value(&led, LWGPIO_VALUE_HIGH); /* set pin to 1 */
}

/*
 * Get current value of LED by looking at OutputState variable
 */
boolean GetOutput(Output_t signal)
{
  if(signal<MAX_OUTPUTS)
  {
    return OutputState[signal];
  }
  else
  {
    return FALSE;
  }
}

/*
 * Used by Accel_Task, this function turns on
 *  the LED's specified
 *
 * Each bit represents an LED to turn on
 *
 */
void TiltOutput(char signal)
{
  signal&=0xF;

  if(signal & 0x8)
    SetOutput(led1,ON);
  else
    SetOutput(led1,OFF);

  if(signal & 0x4)
    SetOutput(led2,ON);
  else
    SetOutput(led2,OFF);

  if(signal & 0x2)
    SetOutput(led3,ON);
  else
    SetOutput(led3,OFF);

  if(signal & 0x1)
    SetOutput(led4,ON);
  else
    SetOutput(led4,OFF);
}

/*
 * Get Input from Push Buttons
 */
boolean GetInput(LWGPIO_STRUCT btn)
{  
   if (LWGPIO_VALUE_LOW == lwgpio_get_value(&btn))
       return TRUE;
   else
	   return FALSE;
}


/*
 * Toggle LED's on and off the specified number of times
 */
void ToggleAllOutput(int toggles)
{
  int i;
  for(i=0;i<toggles;i++)
  {
    OutputAllOn();
    _time_delay(250);
    OutputAllOff();
    _time_delay(250);
  }
}

/*
 * Turn on all LED's
 */
void OutputAllOn()
{
  SetOutput(led1,ON);
  SetOutput(led2,ON);
  SetOutput(led3,ON);
  SetOutput(led4,ON);
}

/*
 * Turn off all LED's
 */
void OutputAllOff()
{
  SetOutput(led1,OFF);
  SetOutput(led2,OFF);
  SetOutput(led3,OFF);
  SetOutput(led4,OFF);	
}
