/*
 * File:		adc16.h
 * Purpose:		ADC bare-metal Code
 *
 */
#ifndef __ADC16_H__
#define __ADC16_H__ 1

#define ADC0_irq_no 57
#define ADC1_irq_no 58


// teken from Nucleus validation. bit shifting of bitfiled is already taken into account so bitfiled values are always represented as relative to their position

/************************* #Defines ******************************************/

// ADCSC1

#define A                 0x0
#define B                 0x1

#define COCO_COMPLETE     ADC_SC1_COCO_MASK
#define COCO_NOT          0x00

#define AIEN_ON           ADC_SC1_AIEN_MASK
#define AIEN_OFF          0x00

#define DIFF_SINGLE       0x00
#define DIFF_DIFFERENTIAL ADC_SC1_DIFF_MASK

// ADCCFG1
#define ADLPC_LOW         ADC_CFG1_ADLPC_MASK
#define ADLPC_NORMAL      0x00

#define ADIV_1            0x00
#define ADIV_2            0x01
#define ADIV_4            0x02
#define ADIV_8            0x03

#define ADLSMP_LONG       ADC_CFG1_ADLSMP_MASK
#define ADLSMP_SHORT      0x00

#define MODE_8            0x00
#define MODE_12           0x01
#define MODE_10           0x02
#define MODE_16           0x03

#define ADICLK_BUS        0x00
#define ADICLK_BUS_2      0x01
#define ADICLK_ALTCLK     0x02
#define ADICLK_ADACK      0x03

// ADCCFG2

#define MUXSEL_ADCB       ADC_CFG2_MUXSEL_MASK
#define MUXSEL_ADCA       0x00

#define ADACKEN_ENABLED   ADC_CFG2_ADACKEN_MASK
#define ADACKEN_DISABLED  0x00

#define ADHSC_HISPEED     ADC_CFG2_ADHSC_MASK
#define ADHSC_NORMAL      0x00

#define ADLSTS_20          0x00
#define ADLSTS_12          0x01
#define ADLSTS_6           0x02
#define ADLSTS_2           0x03

//ADCSC2
#define ADACT_ACTIVE       ADC_SC2_ADACT_MASK
#define ADACT_INACTIVE     0x00

#define ADTRG_HW           ADC_SC2_ADTRG_MASK
#define ADTRG_SW           0x00

#define ACFE_DISABLED      0x00
#define ACFE_ENABLED       ADC_SC2_ACFE_MASK

#define ACFGT_GREATER      ADC_SC2_ACFGT_MASK
#define ACFGT_LESS         0x00

#define ACREN_ENABLED      ADC_SC2_ACREN_MASK
#define ACREN_DISABLED     0x00

#define DMAEN_ENABLED      ADC_SC2_DMAEN_MASK
#define DMAEN_DISABLED     0x00

#define REFSEL_EXT         0x00
#define REFSEL_ALT         0x01
#define REFSEL_RES         0x02
#define REFSEL_RES_EXT     0x03     /* reserved but defaults to Vref */

//ADCSC3
#define CAL_BEGIN          ADC_SC3_CAL_MASK
#define CAL_OFF            0x00

#define CALF_FAIL          ADC_SC3_CALF_MASK
#define CALF_NORMAL        0x00

#define ADCO_CONTINUOUS    ADC_SC3_ADCO_MASK
#define ADCO_SINGLE        0x00

#define AVGE_ENABLED       ADC_SC3_AVGE_MASK
#define AVGE_DISABLED      0x00

#define AVGS_4             0x00
#define AVGS_8             0x01
#define AVGS_16            0x02
#define AVGS_32            0x03

//PGA

#define PGAEN_ENABLED      ADC_PGA_PGAEN_MASK
#define PGAEN_DISABLED     0x00

#define PGACHP_CHOP        ADC_PGA_PGACHP_MASK
#define PGACHP_NOCHOP      0x00

#define PGALP_LOW          ADC_PGA_PGALP_MASK
#define PGALP_NORMAL       0x00

#define PGAG_1             0x00
#define PGAG_2             0x01
#define PGAG_4             0x02
#define PGAG_8             0x03
#define PGAG_16            0x04
#define PGAG_32            0x05
#define PGAG_64            0x06



typedef struct adc_cfg {
  uint8_t  CONFIG1;
  uint8_t  CONFIG2;
  uint16_t COMPARE1;
  uint16_t COMPARE2;
  uint8_t  STATUS2;
  uint8_t  STATUS3;
  uint8_t  STATUS1A;
  uint8_t  STATUS1B;
  uint32_t PGA;
  } *tADC_ConfigPtr, tADC_Config ;


#define CAL_BLK_NUMREC 18

typedef struct adc_cal {

uint16_t  OFS;
uint16_t  PG;
uint16_t  MG;
uint8_t   CLPD;
uint8_t   CLPS;
uint16_t  CLP4;
uint16_t  CLP3;
uint8_t   CLP2;
uint8_t   CLP1;
uint8_t   CLP0;
uint8_t   dummy;
uint8_t   CLMD;
uint8_t   CLMS;
uint16_t  CLM4;
uint16_t  CLM3;
uint8_t   CLM2;
uint8_t   CLM1;
uint8_t   CLM0;
} tADC_Cal_Blk ;




// prototypes

uint_8 ADC_Cal(ADC_MemMapPtr);
void ADC_Config(ADC_MemMapPtr, uint8_t , uint8_t , uint16_t , uint16_t, uint8_t , uint8_t , uint8_t , uint8_t , uint32_t );

void ADC_Config_Alt(ADC_MemMapPtr, tADC_ConfigPtr);

void ADC_Read_Cal(ADC_MemMapPtr, tADC_Cal_Blk *);
void ADC_Write_Cal(ADC_MemMapPtr, tADC_Cal_Blk *);

#endif /* __ADC16_H__ */
