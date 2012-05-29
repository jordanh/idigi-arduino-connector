/**************************************************************************
*
* FileName: TSI_Task.c
*
* Comments: Uses TSI driver to read touch pad presses
*
*
***************************************************************************/


#include "tower_demo.h"
#include "tsi.h"

uint_16  g16ElectrodeTouch[16] = {0};
uint_16  g16ElectrodeBaseline[16] = {0};
uint_32  g32DebounceCounter[16] = {DBOUNCE_COUNTS};

/*
 * Functions for setting up the touch pads
 *
 */

void TSI_Start()
{
  TSI_Init();
  TSI_SelfCalibration();

  START_SCANNING;
  ENABLE_EOS_INT;
  ENABLE_TSI;
}

/********************************************************************************
 *   TSI_Init: Initializes TSI module
 * Notes:
 *    -
 ********************************************************************************/
void TSI_Init(void)
{
  SIM_SCGC5 |= (SIM_SCGC5_TSI_MASK); //Turn on clock to TSI module
  PORTA_PCR4 = PORT_PCR_MUX(0);      //Enable ALT0 for portA4
  PORTB_PCR2 = PORT_PCR_MUX(0);      //Enable ALT0 for portB2
  PORTB_PCR3 = PORT_PCR_MUX(0);      //Enable ALT0 for portB3
  PORTB_PCR16 = PORT_PCR_MUX(0);      //Enable ALT0 for portB16


  TSI0_GENCS |= ((TSI_GENCS_NSCN(10))|(TSI_GENCS_PS(3)));
  TSI0_SCANC |= ((TSI_SCANC_EXTCHRG(3))|(TSI_SCANC_REFCHRG(31))|(TSI_SCANC_DELVOL(7))|(TSI_SCANC_SMOD(0))|(TSI_SCANC_AMPSC(0)));

  ELECTRODE_ENABLE_REG = ELECTRODE0_EN_MASK|ELECTRODE1_EN_MASK|ELECTRODE2_EN_MASK|ELECTRODE3_EN_MASK;

  TSI0_GENCS |= (TSI_GENCS_TSIEN_MASK);  //Enables TSI

  /* Init TSI interrupts */
  _int_install_isr(INT_TSI0, TSI_isr, NULL);
  _cortex_int_init(INT_TSI0, 4, TRUE);
  /***********************/

}

/********************************************************************************
 *   TSI_SelfCalibration: Simple auto calibration version
 * Notes:
 *    - Very simple, only sums a prefixed amount to the current baseline
 ********************************************************************************/
void TSI_SelfCalibration(void)
{
  TSI0_GENCS |= TSI_GENCS_SWTS_MASK;

  while(!TSI0_GENCS&TSI_GENCS_EOSF_MASK){};

  _time_delay(1000);

  g16ElectrodeBaseline[ELECTRODE0] = ELECTRODE0_COUNT;
  ELECTRODE0_OVERRUN = (uint_32)((g16ElectrodeBaseline[ELECTRODE0]+ELECTRODE0_OVRRUN));
  g16ElectrodeTouch[ELECTRODE0] = g16ElectrodeBaseline[ELECTRODE0] + ELECTRODE0_TOUCH;

  g16ElectrodeBaseline[ELECTRODE1] = ELECTRODE1_COUNT;
  ELECTRODE1_OVERRUN = (uint_32)((g16ElectrodeBaseline[ELECTRODE1]+ELECTRODE1_OVRRUN));
  g16ElectrodeTouch[ELECTRODE1] = g16ElectrodeBaseline[ELECTRODE1] + ELECTRODE1_TOUCH;

  g16ElectrodeBaseline[ELECTRODE2] = ELECTRODE2_COUNT;
  ELECTRODE2_OVERRUN = (uint_32)((g16ElectrodeBaseline[ELECTRODE2]+ELECTRODE2_OVRRUN));
  g16ElectrodeTouch[ELECTRODE2] = g16ElectrodeBaseline[ELECTRODE2] + ELECTRODE2_TOUCH;

  g16ElectrodeBaseline[ELECTRODE3] = ELECTRODE3_COUNT;
  ELECTRODE3_OVERRUN = (uint_32)((g16ElectrodeBaseline[ELECTRODE3]+ELECTRODE3_OVRRUN));
  g16ElectrodeTouch[ELECTRODE3] = g16ElectrodeBaseline[ELECTRODE3] + ELECTRODE3_TOUCH;

  DISABLE_TSI;

}

/********************************************************************************
 *   TSI_isr: TSI interrupt subroutine
 * Notes:
 *    -
 ********************************************************************************/

void TSI_isr(pointer isr)
{
  static uint_16 TouchEvent = 0;
  uint_16 lValidTouch = 0;
  uint_16 l16Counter;

  TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK;
  TSI0_GENCS |= TSI_GENCS_EOSF_MASK;


  /* Process electrode 0 */
  l16Counter = ELECTRODE0_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE0]) //See if detected a touch
  {
    TouchEvent |= (1<<ELECTRODE0); //Set touch event variable
    g32DebounceCounter[ELECTRODE0]--; //Decrement debounce counter
    if(!g32DebounceCounter[ELECTRODE0]) //If debounce counter reaches 0....
    {
      _lwsem_post(&touch_sem);
      keypressed |= ((1<<ELECTRODE0)); //Signal that a valid touch has been detected
      lValidTouch |= ((1<<ELECTRODE0)); //Signal that a valid touch has been detected
      TouchEvent &= ~(1<<ELECTRODE0);  //Clear touch event variable
    }
  }
  else
  {
    keypressed &= ~((1<<ELECTRODE0)); //Clear valid touch
    TouchEvent &= ~(1<<ELECTRODE0); //Clear touch event variable
    g32DebounceCounter[ELECTRODE0] = DBOUNCE_COUNTS; //Reset debounce counter
  }
  /***********************/

  /* Process electrode 1 */
  l16Counter = ELECTRODE1_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE1])
  {
    TouchEvent |= (1<<ELECTRODE1);
    g32DebounceCounter[ELECTRODE1]--;
    if(!g32DebounceCounter[ELECTRODE1])
    {
      _lwsem_post(&touch_sem);
      keypressed |= ((1<<ELECTRODE1));
      lValidTouch |= ((1<<ELECTRODE1));
      TouchEvent &= ~(1<<ELECTRODE1);
    }
  }
  else
  {
    keypressed&= ~((1<<ELECTRODE1)); //Clear valid touch
    TouchEvent &= ~(1<<ELECTRODE1);
    g32DebounceCounter[ELECTRODE1] = DBOUNCE_COUNTS;
  }
  /***********************/

  /* Process electrode 2 */
  l16Counter = ELECTRODE2_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE2])
  {
    TouchEvent |= (1<<ELECTRODE2);
    g32DebounceCounter[ELECTRODE2]--;
    if(!g32DebounceCounter[ELECTRODE2])
    {
      _lwsem_post(&touch_sem);
      keypressed |= ((1<<ELECTRODE2));
      lValidTouch |= ((1<<ELECTRODE2));
      TouchEvent &= ~(1<<ELECTRODE2);
    }
  }
  else
  {
    keypressed &= ~((1<<ELECTRODE2)); //Clear valid touch
    TouchEvent &= ~(1<<ELECTRODE2);
    g32DebounceCounter[ELECTRODE2] = DBOUNCE_COUNTS;
  }
  /***********************/

  /* Process electrode 3 */
  l16Counter = ELECTRODE3_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE3])
  {
    TouchEvent |= (1<<ELECTRODE3);
    g32DebounceCounter[ELECTRODE3]--;
    if(!g32DebounceCounter[ELECTRODE3])
    {
      _lwsem_post(&touch_sem);
      keypressed |= ((1<<ELECTRODE3));
      lValidTouch |= ((1<<ELECTRODE3));
      TouchEvent &= ~(1<<ELECTRODE3);
    }
  }
  else
  {
    keypressed &= ~((1<<ELECTRODE3)); //Clear valid touch
    TouchEvent &= ~(1<<ELECTRODE3);
    g32DebounceCounter[ELECTRODE3] = DBOUNCE_COUNTS;
  }
  /***********************/

  if(lValidTouch&((1<<ELECTRODE0))) //If detected a valid touch...
  {
    if(mode==TOUCH)
    {
//      SetOutput(LED1,!GetOutput(LED1));
        AddTouch(LED1);
    }
    if(mode==GAME)
    {
      AddTouch(LED1);
    }
    lValidTouch &= ~((1<<ELECTRODE0)); //Clear valid touch
  }
  if(lValidTouch&((1<<ELECTRODE1)))
  {
    if(mode==TOUCH)
    {
//      SetOutput(LED2,!GetOutput(LED2));
        AddTouch(LED2);
    }
    if(mode==GAME)
    {
      AddTouch(LED2);
    }
    lValidTouch &= ~((1<<ELECTRODE1));
  }
  if(lValidTouch&((1<<ELECTRODE2)))
  {
    if(mode==TOUCH)
    {
//      SetOutput(LED3,!GetOutput(LED3));
        AddTouch(LED3);
    }
    if(mode==GAME)
    {
      AddTouch(LED3);
    }
    lValidTouch &= ~((1<<ELECTRODE2));
  }
  if(lValidTouch&((1<<ELECTRODE3)))
  {
    if(mode==TOUCH)
    {
//      SetOutput(LED4,!GetOutput(LED4));
        AddTouch(LED4);
    }
    if(mode==GAME)
    {
      AddTouch(LED4);
    }
    lValidTouch &= ~((1<<ELECTRODE3));
  }

  TSI0_STATUS = 0xFFFFFFFF;
}

