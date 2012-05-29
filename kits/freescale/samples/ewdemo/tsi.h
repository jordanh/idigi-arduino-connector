#ifndef __tsi_h__
#define __tsi_h__

#define LOG_LENGTH  30
char LastKeyPressed();
char KeyPressed();
void AddTouch(char key);
extern volatile unsigned char keypressed;

#define ELECTRODE0  0
#define ELECTRODE1  1
#define ELECTRODE2  2
#define ELECTRODE3  3

#define ELECTRODE0_TOUCH  0x200
#define ELECTRODE1_TOUCH  0x200
#define ELECTRODE2_TOUCH  0x200
#define ELECTRODE3_TOUCH  0x200

#define ELECTRODE0_OVRRUN  0xf000
#define ELECTRODE1_OVRRUN  0xf000
#define ELECTRODE2_OVRRUN  0xf000
#define ELECTRODE3_OVRRUN  0xf000

/* Number of scans needed for a touch to remain high to be considered valid */
#define DBOUNCE_COUNTS  0x00000010

#define ELECTRODE_ENABLE_REG    TSI0_PEN

  #define ELECTRODE0_COUNT  (uint_16)((TSI0_CNTR5>>16)&0x0000FFFF)
  #define ELECTRODE1_COUNT  (uint_16)((TSI0_CNTR9)&0x0000FFFF)
  #define ELECTRODE2_COUNT  (uint_16)((TSI0_CNTR7>>16)&0x0000FFFF)
  #define ELECTRODE3_COUNT  (uint_16)((TSI0_CNTR9>>16)&0x0000FFFF)

  #define ELECTRODE0_OVERRUN  TSI0_THRESHLD5
  #define ELECTRODE1_OVERRUN  TSI0_THRESHLD8
  #define ELECTRODE2_OVERRUN  TSI0_THRESHLD7
  #define ELECTRODE3_OVERRUN  TSI0_THRESHLD9

  #define ELECTRODE0_EN_MASK  TSI_PEN_PEN5_MASK
  #define ELECTRODE1_EN_MASK  TSI_PEN_PEN8_MASK
  #define ELECTRODE2_EN_MASK  TSI_PEN_PEN7_MASK
  #define ELECTRODE3_EN_MASK  TSI_PEN_PEN9_MASK

#define START_SCANNING  TSI0_GENCS |= TSI_GENCS_STM_MASK
#define ENABLE_EOS_INT  TSI0_GENCS |= (TSI_GENCS_TSIIE_MASK|TSI_GENCS_ESOR_MASK)
#define ENABLE_TSI      TSI0_GENCS |= TSI_GENCS_TSIEN_MASK
#define DISABLE_TSI     TSI0_GENCS &= ~TSI_GENCS_TSIEN_MASK


void TSI_isr(pointer isr);
void TSI_Init(void);
void TSI_SelfCalibration(void);
void TSI_Start();


#endif