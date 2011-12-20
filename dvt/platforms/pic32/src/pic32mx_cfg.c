/*********************************************************************
 *
 *                  Configuration file for the Microchip BSD stack HTTP Server
 *
 *********************************************************************
 * FileName:        pic32mx_cfg.c		
 * Dependencies:
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ìCompanyî) for its PICmicroÆ Microcontroller is intended and
 * supplied to you, the Companyís customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ìAS ISî CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/

#ifndef _PIC32MX_CONFIGURATION_SETTINGS
#define _PIC32MX_CONFIGURATION_SETTINGS

/********************************************************************
 * Oscillator Selection bits [FNOSC]
 *
 * FRC      - Fast RC oscillator  
 * FRCPLL   - Fast RC oscillator w/ PLL  
 * PRI      - Primary oscillator (XT, HS, EC)  
 * PRIPLL   - Primary oscillator (XT, HS, EC) w/ PLL  
 * SOSC     - Secondary oscillator  
 * LPRC     - Low power RC oscillator  
 * FRCDIV16 - Fast RC oscillator with divide by 16  
 * FRCDIV   - Fast RC oscillator with divide  
 ********************************************************************/
#pragma config FNOSC = PRIPLL

/********************************************************************
 * PLL Input Divider [FPLLIDIV]
 * The PLL need to have a value between 4 - 5 MHz.  Use this
 * setting to scale down your input frequency.
 * 
 * DIV_1    - Divide by 1  
 * DIV_2    - Divide by 2  
 * DIV_3    - Divide by 3  
 * DIV_4    - Divide by 4  
 * DIV_5    - Divide by 5  
 * DIV_6    - Divide by 6  
 * DIV_10   - Divide by 10  
 * DIV_12   - Divide by 12  
 ********************************************************************/
#pragma config FPLLIDIV = DIV_2

/********************************************************************
 * PLL Multipler [FPLLMUL]
 * 
 * MUL_15 - Multiply by 15  
 * MUL_16 - Multiply by 16  
 * MUL_17 - Multiply by 17  
 * MUL_18 - Multiply by 18  
 * MUL_19 - Multiply by 19  
 * MUL_20 - Multiply by 20  
 * MUL_21 - Multiply by 21  
 * MUL_24 - Multiply by 24  
 ********************************************************************/
#pragma config FPLLMUL = MUL_20

/********************************************************************
 * PLL Output Divider [FPLLODIV]
 * 
 * DIV_1 - Divide by 1  
 * DIV_2 - Divide by 2  
 * DIV_4 - Divide by 4  
 * DIV_8 - Divide by 8  
 * DIV_16 - Divide by 16  
 * DIV_32 - Divide by 32  
 * DIV_64 - Divide by 64  
 * DIV_256 - Divide by 256  
 ********************************************************************/
#pragma config FPLLODIV = DIV_1

/********************************************************************
 * Bootup Peripheral Bus Clock Divider [FPBDIV]
 * 
 * DIV_1 - Divide by 1  
 * DIV_2 - Divide by 2  
 * DIV_4 - Divide by 4  
 * DIV_8 - Divide by 8  
 ********************************************************************/
#pragma config FPBDIV = DIV_1

/********************************************************************
 * Primary Oscillator [POSCMOD]
 * 
 * EC - EC oscillator  
 * XT - XT oscillator  
 * HS - HS oscillator  
 * OFF - Disabled  
 ********************************************************************/
#pragma config POSCMOD = HS

/********************************************************************
 * Secondary Oscillator Enable [FSOSCEN]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config FSOSCEN = OFF

/********************************************************************
 * Internal/External Switch Over [IESO]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config IESO = OFF

/********************************************************************
 * Clock Switching and Monitor Selection [FCKSM]
 * 
 * CSECME - Clock Switching Enabled, Clock Monitoring Enabled  
 * CSECMD - Clock Switching Enabled, Clock Monitoring Disabled  
 * CSDCMD - Clock Switching Disabled, Clock Monitoring Disabled  
 ********************************************************************/
#pragma config FCKSM = CSDCMD

/********************************************************************
 * Clock Output Enable [OSCIOFNC]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config OSCIOFNC = OFF

/********************************************************************
 * Watchdog Timer Enable [FWDTEN]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config FWDTEN = OFF

/********************************************************************
 * Watchdog Timer Postscale Select [WDTPS]
 * 
 * PS1         - 1:1  
 * PS2         - 1:2  
 * PS4         - 1:4  
 * PS8         - 1:8  
 * PS16        - 1:16  
 * PS32        - 1:32  
 * PS64        - 1:64  
 * PS128       - 1:128  
 * PS256       - 1:256  
 * PS512       - 1:512  
 * PS1024      - 1:1024  
 * PS2048      - 1:2048  
 * PS4096      - 1:4096  
 * PS8192      - 1:8,192  
 * PS16384     - 1:16,384  
 * PS32768     - 1:32,768  
 * PS65536     - 1:65,536  
 * PS131072    - 1:131,072  
 * PS262144    - 1:262,144  
 * PS524288    - 1:524,288  
 * PS1048576   - 1:1,048,576  
 ********************************************************************/
#pragma config WDTPS = PS512

/********************************************************************
 * Boot Flash Protect Enable [BWP]
 *
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config BWP = OFF

/********************************************************************
 * Code Protect Enable [CP]
 *
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config CP = OFF

/********************************************************************
 * ICE/ICD Comm Channel Select [ICESEL]
 * 
 * ICS_PGx1 - ICE pins are shared with PGC1, PGD1  
 * ICS_PGx2 - ICE pins are shared with PGC2, PGD2  
 ********************************************************************/
#pragma config ICESEL = ICS_PGx2

/********************************************************************
 * Background Debugger Enable bit [DEBUG]
 * 
 * ON   - enabled  
 * OFF  - disabled  
 ********************************************************************/
#pragma config DEBUG = OFF


/********************************************************************
 * Ethernet MII Enable bit [FMIIEN]
 * 
 * ON   - MII enabled, RMII disabled  
 * OFF  - MII disabled, RMII enabled  
 ********************************************************************/
#pragma config FMIIEN = OFF


/********************************************************************
 * Ethernet IO Pin Selection bit [FETHIO]
 * 
 * ON   - Default Ethernet IO Pins   
 * OFF  - Alternate Ethernet IO Pins  
 ********************************************************************/
#pragma config FETHIO = OFF


#endif
