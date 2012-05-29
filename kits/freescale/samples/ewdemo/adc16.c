/*
 * File:		adc16.c
 * Purpose:		ADC bare-metal Code
 *
 */

#include "tower_demo.h"
#include "adc16.h"




/********************************************************************/



/******************************************************************************
Function Name	:	AUTO CAL ROUTINE    (from Inga Harris' Nucleus ADC0_ Validation)
Engineer      :	r54940
Date          :	04/08/08

B16087 : modified for Kinetis, matches header files

Parameters		:	adc module pointer
Returns			  :	NONE
Notes         :	Calibrates the ADC0_. Required after reset and before a conversion is initiated
******************************************************************************/
uint_8 ADC_Cal(ADC_MemMapPtr adcmap)
{

  unsigned int cal_var;

  ADC_SC2_REG(adcmap) &=  ~ADC_SC2_ADTRG_MASK ; // Enable Software Conversion Trigger for Calibration Process    - ADC0_SC2 = ADC0_SC2 | ADC_SC2_ADTRGW(0);
  ADC_SC3_REG(adcmap) &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); // set single conversion, clear avgs bitfield for next writing
  ADC_SC3_REG(adcmap) |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(AVGS_32) );  // Turn averaging ON and set at max value ( 32 )


  ADC_SC3_REG(adcmap) |= ADC_SC3_CAL_MASK ;      // Start CAL
  while ( (ADC_SC1_REG(adcmap,A) & ADC_SC1_COCO_MASK ) == COCO_NOT ); // Wait calibration end
  	
  if ((ADC_SC3_REG(adcmap)& ADC_SC3_CALF_MASK) == CALF_FAIL ) return(1);    // Check for Calibration fail error and return

  // Calculate plus-side calibration as per 21.4.7
  cal_var = 0x00;

  cal_var =  ADC_CLP0_REG(adcmap);
  cal_var += ADC_CLP1_REG(adcmap);
  cal_var += ADC_CLP2_REG(adcmap);
  cal_var += ADC_CLP3_REG(adcmap);
  cal_var += ADC_CLP4_REG(adcmap);
  cal_var += ADC_CLPS_REG(adcmap);

  cal_var = cal_var/2;
  cal_var |= 0x8000; // Set MSB

  ADC_PG_REG(adcmap) = ADC_PG_PG(cal_var);


  // Calculate minus-side calibration as per 21.4.7
  cal_var = 0x00;

  cal_var =  ADC_CLM0_REG(adcmap);
  cal_var += ADC_CLM1_REG(adcmap);
  cal_var += ADC_CLM2_REG(adcmap);
  cal_var += ADC_CLM3_REG(adcmap);
  cal_var += ADC_CLM4_REG(adcmap);
  cal_var += ADC_CLMS_REG(adcmap);

  cal_var = cal_var/2;

  cal_var |= 0x8000; // Set MSB

  ADC_MG_REG(adcmap) = ADC_MG_MG(cal_var);

  ADC_SC3_REG(adcmap) &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */

  return(0);
}




/******************************************************************************
Function Name	:	ADC0__Config     (from Inga Harris' Nucleus ADC0_ Validation)
Engineer      :	r54940
Date          :	04/08/08

Parameters		:	
Returns			  :	NONE
Notes         :	Configures ADC0_
******************************************************************************/
void ADC_Config(ADC_MemMapPtr adcmap, uint8_t CONFIG1, uint8_t CONFIG2, uint16_t COMPARE1, uint16_t COMPARE2, uint8_t STATUS2, uint8_t STATUS3, uint8_t STATUS1A, uint8_t STATUS1B, uint32_t PGA )
{
  ADC_CFG1_REG(adcmap) = CONFIG1;
  ADC_CFG2_REG(adcmap) = CONFIG2;
  ADC_CV1_REG(adcmap) = COMPARE1;
  ADC_CV2_REG(adcmap) = COMPARE2;
  ADC_SC2_REG(adcmap) = STATUS2;
  ADC_SC3_REG(adcmap) = STATUS3;
  ADC_SC1_REG(adcmap,A) = STATUS1A;
  ADC_SC1_REG(adcmap,B) = STATUS1B;
  ADC_PGA_REG(adcmap) = PGA;
}


void ADC_Config_Alt(ADC_MemMapPtr adcmap, tADC_ConfigPtr ADC_CfgPtr)
{
 ADC_CFG1_REG(adcmap) = ADC_CfgPtr->CONFIG1;
 ADC_CFG2_REG(adcmap) = ADC_CfgPtr->CONFIG2;
 ADC_CV1_REG(adcmap)  = ADC_CfgPtr->COMPARE1;
 ADC_CV2_REG(adcmap)  = ADC_CfgPtr->COMPARE2;
 ADC_SC2_REG(adcmap)  = ADC_CfgPtr->STATUS2;
 ADC_SC3_REG(adcmap)  = ADC_CfgPtr->STATUS3;
 ADC_PGA_REG(adcmap)  = ADC_CfgPtr->PGA;
 ADC_SC1_REG(adcmap,A)= ADC_CfgPtr->STATUS1A;
 ADC_SC1_REG(adcmap,B)= ADC_CfgPtr->STATUS1B;
}


void ADC_Read_Cal(ADC_MemMapPtr adcmap, tADC_Cal_Blk *blk)
{
  blk->OFS  = ADC_OFS_REG(adcmap);
  blk->PG   = ADC_PG_REG(adcmap);
  blk->MG   = ADC_MG_REG(adcmap);
  blk->CLPD = ADC_CLPD_REG(adcmap);
  blk->CLPS = ADC_CLPS_REG(adcmap);
  blk->CLP4 = ADC_CLP4_REG(adcmap);
  blk->CLP3 = ADC_CLP3_REG(adcmap);
  blk->CLP2 = ADC_CLP2_REG(adcmap);
  blk->CLP1 = ADC_CLP1_REG(adcmap);
  blk->CLP0 = ADC_CLP0_REG(adcmap);
  blk->CLMD = ADC_CLMD_REG(adcmap);
  blk->CLMS = ADC_CLMS_REG(adcmap);
  blk->CLM4 = ADC_CLM4_REG(adcmap);
  blk->CLM3 = ADC_CLM3_REG(adcmap);
  blk->CLM2 = ADC_CLM2_REG(adcmap);
  blk->CLM1 = ADC_CLM1_REG(adcmap);
  blk->CLM0 = ADC_CLM0_REG(adcmap);

}


