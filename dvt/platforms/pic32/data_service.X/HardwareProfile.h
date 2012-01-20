/*********************************************************************
 *
 *	Hardware specific definitions
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.10 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.34 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/06	Original, copied from Compiler.h
 * Ken Hesky            01/xx/10    Added MRF24WB0M-specific features
 ********************************************************************/
#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

// Choose which hardware profile to compile for here.  See
// the hardware profiles below for meaning of various options.
//#define PICDEMNET2
//#define PIC18_EXPLORER
//#define HPC_EXPLORER
//#define PIC24FJ64GA004_PIM		// Explorer 16, but with the PIC24FJ64GA004 PIM module, which has significantly differnt pin mappings
//#define EXPLORER_16				// PIC24FJ128GA010, PIC24HJ256GP610, dsPIC33FJ256GP710, PIC32MX360F512L, PIC32MX460F512L, PIC32MX795F512L, etc. PIMs
//#define DSPICDEM11
//#define PIC32_GP_SK_DM320001			// PIC32MX360F512L General Purpose Starter Kit (for purposes of TCP/IP, defining this macro is the same as defining PIC32_USB_DM320003_1 or PIC32_USB_SK_DM320003_2)
//#define PIC32_USB_DM320003_1			// PIC32MX460F512L USB Starter Board (for purposes of TCP/IP, defining this macro is the same as defining PIC32_GP_SK_DM320001 or PIC32_USB_SK_DM320003_2)
//#define PIC32_USB_SK_DM320003_2			// PIC32MX795F512L USB Starter Kit II (for purposes of TCP/IP, defining this macro is the same as defining PIC32_GP_SK_DM320001 or PIC32_USB_DM320003_1)
#define PIC32_ENET_SK_DM320004		// PIC32MX795F512L Ethernet Starter Kit board with embedded Ethernet controller
//#define PIC24FJ256DA210_DEV_BOARD	// PIC24FJ256DA210 Development Board (Graphics)

// If no hardware profiles are defined, assume that we are using
// a Microchip demo board and try to auto-select the correct profile
// based on processor selected in MPLAB
#if !defined(PICDEMNET2) && !defined(PIC18_EXPLORER) && !defined(HPC_EXPLORER) && !defined(EXPLORER_16) && !defined(PIC24FJ64GA004_PIM) && !defined(DSPICDEM11) && !defined(PICDEMNET2) && !defined(INTERNET_RADIO) && !defined(YOUR_BOARD) && !defined(__PIC24FJ128GA006__) && !defined(PIC32_GP_SK_DM320001) && !defined(PIC32_USB_DM320003_1) && !defined(PIC32_USB_SK_DM320003_2) && !defined(PIC32_ENET_SK_DM320004) && !defined(PIC24FJ256DA210_DEV_BOARD)
	#if defined(__18F97J60) || defined(_18F97J60)
		#define PICDEMNET2
	#elif defined(__18F67J60) || defined(_18F67J60)
		#define INTERNET_RADIO
	#elif defined(__18F8722) || defined(__18F87J10) || defined(_18F8722) || defined(_18F87J10) || defined(__18F87J11) || defined(_18F87J11)|| defined(__18F87J50) || defined(_18F87J50)
		#define PIC18_EXPLORER
		//#define HPC_EXPLORER
	#elif defined(__PIC24FJ64GA004__)
		#define PIC24FJ64GA004_PIM
	#elif defined(__PIC24FJ256DA210__)
		#define PIC24FJ256DA210_DEV_BOARD
	#elif defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__) || defined(__PIC32MX__)
		#define EXPLORER_16
	#elif defined(__dsPIC30F__)
		#define DSPICDEM11
	#endif
#endif

// Set configuration fuses (but only once)
#if defined(THIS_IS_STACK_APPLICATION)
	#if defined(__18CXX)
		#if defined(__EXTENDED18__)
			#pragma config XINST=ON
		#elif !defined(HI_TECH_C)
			#pragma config XINST=OFF
		#endif

		#if defined(__18F8722) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF, PWRT=OFF, WDT=OFF, LVP=OFF
		#elif defined(_18F8722)	// HI-TECH PICC-18 compiler
			// PICDEM HPC Explorer or PIC18 Explorer board with HI-TECH PICC-18 compiler
			__CONFIG(1, HSPLL);
			__CONFIG(2, WDTDIS);
			__CONFIG(3, MCLREN);
			__CONFIG(4, XINSTDIS & LVPDIS);
		#elif defined(__18F87J10) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config WDTEN=OFF, FOSC2=ON, FOSC=HSPLL
		#elif defined(__18F87J11) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config WDTEN=OFF, FOSC=HSPLL
		#elif defined(__18F87J50) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config WDTEN=OFF, FOSC=HSPLL, PLLDIV=3, CPUDIV=OSC1
		#elif (defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60)) && !defined(HI_TECH_C)
			// PICDEM.net 2 or any other PIC18F97J60 family device
			#pragma config WDT=OFF, FOSC2=ON, FOSC=HSPLL, ETHLED=ON
		#elif defined(_18F97J60) || defined(_18F96J65) || defined(_18F96J60) || defined(_18F87J60) || defined(_18F86J65) || defined(_18F86J60) || defined(_18F67J60) || defined(_18F66J65) || defined(_18F66J60)
			// PICDEM.net 2 board with HI-TECH PICC-18 compiler
			__CONFIG(1, WDTDIS & XINSTDIS);
			__CONFIG(2, HSPLL);
			__CONFIG(3, ETHLEDEN);
		#elif defined(__18F4620) && !defined(HI_TECH_C)
			#pragma config OSC=HSPLL, WDT=OFF, MCLRE=ON, PBADEN=OFF, LVP=OFF
		#endif
	#elif defined(__PIC24FJ256DA210__) || defined(__PIC24FJ256GB210__)
		// PIC24FJ256DA210 Development Board (Graphics) or PIC24FJ256GB210 PIM on Explorer 16
		_CONFIG3(ALTPMP_ALPMPDIS & SOSCSEL_EC); 										// PMP in default location, disable Timer1 oscillator so that RC13 can be used as a GPIO
		_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT & IOL1WAY_OFF & PLL96MHZ_ON & PLLDIV_DIV2);	// Primary XT OSC with 96MHz PLL (8MHz crystal input), IOLOCK can be set and cleared
		_CONFIG1(FWDTEN_OFF & ICS_PGx2 & JTAGEN_OFF & ALTVREF_ALTVREDIS);				// Watchdog timer off, ICD debugging on PGEC2/PGED2 pins, JTAG off, AVREF and CVREF in default locations
	#elif defined(__PIC24FJ256GB110__)
		// PIC24FJ256GB110 PIM on Explorer 16
		_CONFIG2(PLLDIV_DIV2 & PLL_96MHZ_ON & FNOSC_PRIPLL & IOL1WAY_OFF & POSCMOD_XT); // Primary XT OSC with 96MHz PLL (8MHz crystal input), IOLOCK can be set and cleared
		_CONFIG1(JTAGEN_OFF & ICS_PGx2 & FWDTEN_OFF);									// Watchdog timer off, ICD debugging on PGEC2/PGED2 pins, JTAG off
	#elif defined(__PIC24FJ256GA110__)
		// PIC24FJ256GA110 PIM on Explorer 16
		_CONFIG2(FNOSC_PRIPLL & IOL1WAY_OFF & POSCMOD_XT);	// Primary XT OSC with PLL, IOLOCK can be set and cleared
		_CONFIG1(JTAGEN_OFF & ICS_PGx2 & FWDTEN_OFF);		// Watchdog timer off, ICD debugging on PGEC2/PGED2 pins, JTAG off
	#elif defined(__PIC24F__)
		// Explorer 16 board
		_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT)		// Primary XT OSC with 4x PLL
		_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)		// JTAG off, watchdog timer off
	#elif defined(__dsPIC33F__) || defined(__PIC24H__)
		// Explorer 16 board
		_FOSCSEL(FNOSC_PRIPLL)			// PLL enabled
		_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
		_FWDT(FWDTEN_OFF)				// Disable Watchdog timer
		// JTAG should be disabled as well
	#elif defined(__dsPIC30F__)
		// dsPICDEM 1.1 board
		_FOSC(XT_PLL16)					// XT Osc + 16X PLL
		_FWDT(WDT_OFF)					// Disable Watchdog timer
		_FBORPOR(MCLR_EN & PBOR_OFF & PWRT_OFF)
	#elif defined(__PIC32MX__)
		#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FWDTEN = OFF, FPBDIV = DIV_1, POSCMOD = XT, FNOSC = PRIPLL, CP = OFF
		#if defined(PIC32_ENET_SK_DM320004)
			#pragma config FMIIEN = OFF, FETHIO = OFF	// external PHY in RMII/alternate configuration
		#endif
	#endif
#endif // Prevent more than one set of config fuse definitions

// Clock frequency value.
// This value is used to calculate Tick Counter value
#if defined(__18CXX)
	// All PIC18 processors
	#if defined(PICDEMNET2) || defined(INTERNET_RADIO) || defined(YOUR_BOARD)
		#define CLOCK_FREQ		(41666667ul)      // Hz
		#define GetSystemClock()		(41666667ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#elif defined(__18F87J50) || defined(_18F87J50)
		#define GetSystemClock()		(48000000ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#else
		#define GetSystemClock()		(40000000ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#endif
#elif defined(__PIC24F__) || defined(__PIC24FK__)
	// PIC24F processor
	#define GetSystemClock()		(32000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__PIC24H__)
	// PIC24H processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__dsPIC33F__)
	// dsPIC33F processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__dsPIC30F__)
	// dsPIC30F processor
	#define GetSystemClock()		(117920000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/4)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__PIC32MX__)
	// PIC32MX processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/1)
	#define GetPeripheralClock()	(GetInstructionClock()/1)	// Set your divider according to your Peripheral Bus Frequency configuration fuse setting
#endif

// Hardware mappings
#if defined(PIC18_EXPLORER) && !defined(HI_TECH_C)
// PIC18 Explorer + Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail
	// I/O pins
	#define LED0_TRIS			(TRISDbits.TRISD0)
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(TRISDbits.TRISD3)
	#define LED3_IO				(LATDbits.LATD3)
	#define LED4_TRIS			(TRISDbits.TRISD4)
	#define LED4_IO				(LATDbits.LATD4)
	#define LED5_TRIS			(TRISDbits.TRISD5)
	#define LED5_IO				(LATDbits.LATD5)
	#define LED6_TRIS			(TRISDbits.TRISD6)
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(TRISDbits.TRISD7)
	#define LED7_IO				(LATDbits.LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))

	#define BUTTON0_TRIS		(TRISAbits.TRISA5)
	#define	BUTTON0_IO			(PORTAbits.RA5)
	#define BUTTON1_TRIS		(TRISBbits.TRISB0)
	#define	BUTTON1_IO			(PORTBbits.RB0)
	#define BUTTON2_TRIS		(PRODL)				// No Button2 on this board
	#define	BUTTON2_IO			(1u)
	#define BUTTON3_TRIS		(PRODL)				// No Button3 on this board
	#define	BUTTON3_IO			(1u)


//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
//	#define ENC100_INTERFACE_MODE			0		// Uncomment this line to use the ENC424J600/624J600 Ethernet controller (SPI mode) or comment it out to use some other network controller
//
//	// ENC100_MDIX, ENC100_POR, and ENC100_INT are all optional.  Simply leave
//	// them commented out if you don't have such a hardware feature on your
//	// board.
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
//	#define ENC100_MDIX_IO					(LATBbits.LATB4)
////	#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
////	#define ENC100_POR_IO					(LATBbits.LATB5)
////	#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
////	#define ENC100_INT_IO					(PORTBbits.RB2)
//
//	// ENC424J600/624J600 SPI pinout
//	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
//	#define ENC100_CS_IO					(LATBbits.LATB3)
//	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
//	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
//	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
//	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
//
//	// ENC424J600/624J600 SPI SFR register selection (controls which SPI
//	// peripheral to use on PICs with multiple SPI peripherals).
////	#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
////	#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
////	#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
////	#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(PIR1bits.SSPIF)
//	#define ENC100_SSPBUF			(SSP1BUF)
//	#define ENC100_SPISTAT			(SSP1STAT)
//	#define ENC100_SPISTATbits		(SSP1STATbits)
//	#define ENC100_SPICON1			(SSP1CON1)
//	#define ENC100_SPICON1bits		(SSP1CON1bits)
//	#define ENC100_SPICON2			(SSP1CON2)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISBbits.TRISB5)
	#define ENC_RST_IO			(LATBbits.LATB5)
	#define ENC_CS_TRIS			(TRISBbits.TRISB3)	// Uncomment this line to use the ENC28J60 Ethernet controller or comment it out to use some other network controller
	#define ENC_CS_IO			(LATBbits.LATB3)
	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
	#define ENC_SPI_IF			(PIR1bits.SSPIF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

//    //----------------
//    // MRF24WB0M I/O pins
//    //----------------
//	#define WF_CS_TRIS			(TRISCbits.TRISC2)	// Uncomment this line if you wish to use the MRF24WB0M on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
//	#define WF_SDI_TRIS			(TRISCbits.TRISC4)
//	#define WF_SCK_TRIS			(TRISCbits.TRISC3)
//	#define WF_SDO_TRIS			(TRISCbits.TRISC5)
//	#define WF_RESET_TRIS		(TRISBbits.TRISB1)
//	#define WF_RESET_IO			(LATBbits.LATB1)
//	#define WF_INT_TRIS	    	(TRISBbits.TRISB0)
//	#define WF_INT_IO			(PORTBbits.RB0)
//	#define WF_CS_IO			(LATCbits.LATC2)
//	#define WF_HIBERNATE_TRIS   (TRISBbits.TRISB2)
//	#define	WF_HIBERNATE_IO 	(PORTBbits.RB2)
//	#define WF_INT_EDGE		    (INTCON2bits.INTEDG0)
//	#define WF_INT_IE			(INTCONbits.INT0IE)
//	#define WF_INT_IF			(INTCONbits.INT0IF)
//	#define WF_SPI_IF			(PIR1bits.SSPIF)
//	#define WF_SSPBUF			(SSP1BUF)
//	#define WF_SPISTAT			(SSP1STAT)
//	#define WF_SPISTATbits		(SSP1STATbits)
//	#define WF_SPICON1			(SSP1CON1)
//	#define WF_SPICON1bits		(SSP1CON1bits)
//	#define WF_SPICON2			(SSP1CON2)
//	#define WF_SPI_IE			(PIE1bits.SSPIE)
//	#define WF_SPI_IP			(IPR1bits.SSPIP)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISAbits.TRISA3)
	#define EEPROM_CS_IO		(LATAbits.LATA3)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
	#define EEPROM_SSPBUF		(SSP1BUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	// TODO: Need to add support for LCD behind MCP23S17 I/O expander.  This
	// requires code that isn't in the TCP/IP stack, not just a hardware
	// profile change.

	// Serial Flash/SRAM/UART PICtail
//	#define SPIRAM_CS_TRIS			(TRISBbits.TRISB5)
//	#define SPIRAM_CS_IO			(LATBbits.LATB5)
//	#define SPIRAM_SCK_TRIS			(TRISCbits.TRISC3)
//	#define SPIRAM_SDI_TRIS			(TRISCbits.TRISC4)
//	#define SPIRAM_SDO_TRIS			(TRISCbits.TRISC5)
//	#define SPIRAM_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIRAM_SSPBUF			(SSP1BUF)
//	#define SPIRAM_SPICON1			(SSP1CON1)
//	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
//	#define SPIRAM_SPICON2			(SSP1CON2)
//	#define SPIRAM_SPISTAT			(SSP1STAT)
//	#define SPIRAM_SPISTATbits		(SSP1STATbits)
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION,
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB4)
//	#define SPIFLASH_CS_IO			(LATBbits.LATB4)
//	#define SPIFLASH_SCK_TRIS		(TRISCbits.TRISC3)
//	#define SPIFLASH_SDI_TRIS		(TRISCbits.TRISC4)
//	#define SPIFLASH_SDI_IO			(PORTCbits.RC4)
//	#define SPIFLASH_SDO_TRIS		(TRISCbits.TRISC5)
//	#define SPIFLASH_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIFLASH_SSPBUF			(SSP1BUF)
//	#define SPIFLASH_SPICON1		(SSP1CON1)
//	#define SPIFLASH_SPICON1bits	(SSP1CON1bits)
//	#define SPIFLASH_SPICON2		(SSP1CON2)
//	#define SPIFLASH_SPISTAT		(SSP1STAT)
//	#define SPIFLASH_SPISTATbits	(SSP1STATbits)

	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
		#define ADCON2		ADCON1
	#endif

#elif defined(PIC18_EXPLORER) && defined(HI_TECH_C)
// PIC18 Explorer + Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail + HI-TECH PICC-18 compiler
	// I/O pins
	#define LED0_TRIS			(TRISD0)
	#define LED0_IO				(LATD0)
	#define LED1_TRIS			(TRISD1)
	#define LED1_IO				(LATD1)
	#define LED2_TRIS			(TRISD2)
	#define LED2_IO				(LATD2)
	#define LED3_TRIS			(TRISD3)
	#define LED3_IO				(LATD3)
	#define LED4_TRIS			(TRISD4)
	#define LED4_IO				(LATD4)
	#define LED5_TRIS			(TRISD5)
	#define LED5_IO				(LATD5)
	#define LED6_TRIS			(TRISD6)
	#define LED6_IO				(LATD6)
	#define LED7_TRIS			(TRISD7)
	#define LED7_IO				(LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))

	#define BUTTON0_TRIS		(TRISA5)
	#define	BUTTON0_IO			(RA5)
	#define BUTTON1_TRIS		(TRISB0)
	#define	BUTTON1_IO			(RB0)
	#define BUTTON2_TRIS		(PRODL)				// No Button2 on this board
	#define	BUTTON2_IO			(1u)
	#define BUTTON3_TRIS		(PRODL)				// No Button3 on this board
	#define	BUTTON3_IO			(1u)

//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
//	#define ENC100_INTERFACE_MODE			0		// Uncomment this line to use the ENC424J600/624J600 Ethernet controller (SPI mode) or comment it out to use some other network controller
//
//	// ENC100_MDIX, ENC100_POR, and ENC100_INT are all optional.  Simply leave
//	// them commented out if you don't have such a hardware feature on your
//	// hardware.
//	#define ENC100_MDIX_TRIS				(TRISB4)
//	#define ENC100_MDIX_IO					(LATB4)
////	#define ENC100_POR_TRIS					(TRISB5)
////	#define ENC100_POR_IO					(LATB5)
////	#define ENC100_INT_TRIS					(TRISB2)
////	#define ENC100_INT_IO					(RB2)
//
//	// ENC424J600/624J600 SPI pinout
//	#define ENC100_CS_TRIS					(TRISB3)
//	#define ENC100_CS_IO					(LATB3)
//	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//	#define ENC100_SO_WR_B0SEL_EN_IO		(RC4)
//	#define ENC100_SI_RD_RW_TRIS			(TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//	#define ENC100_SI_RD_RW_IO				(LATC5)
//	#define ENC100_SCK_AL_TRIS				(TRISC3)
//	#define ENC100_SCK_AL_IO				(RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
//
//	// ENC424J600/624J600 SPI SFR register selection (controls which SPI
//	// peripheral to use on PICs with multiple SPI peripherals).
////	#define ENC100_ISR_ENABLE		(INT2IE)
////	#define ENC100_ISR_FLAG			(INT2IF)
////	#define ENC100_ISR_POLARITY		(INTEDG2)
////	#define ENC100_ISR_PRIORITY		(INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(SSP1IF)
//	#define ENC100_SSPBUF			(SSP1BUF)
//	#define ENC100_SPISTAT			(SSP1STAT)
//	#define ENC100_SPISTATbits		(SSP1STATbits)
//	#define ENC100_SPICON1			(SSP1CON1)
//	#define ENC100_SPICON1bits		(SSP1CON1bits)
//	#define ENC100_SPICON2			(SSP1CON2)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISB5)
	#define ENC_RST_IO			(LATB5)
	#define ENC_CS_TRIS			(TRISB3)	// Uncomment this line to use the ENC28J60 Ethernet controller or comment it out to use some other network controller
	#define ENC_CS_IO			(LATB3)
	#define ENC_SCK_TRIS		(TRISC3)
	#define ENC_SDI_TRIS		(TRISC4)
	#define ENC_SDO_TRIS		(TRISC5)
	#define ENC_SPI_IF			(SSP1IF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

//    //----------------
//    // MRF24WB0M I/O pins
//    //----------------
//	#define WF_CS_TRIS			(TRISCbits.TRISC2)	// Uncomment this line if you wish to use the MRF24WB0M on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
//	#define WF_SDI_TRIS			(TRISCbits.TRISC4)
//	#define WF_SCK_TRIS			(TRISCbits.TRISC3)
//	#define WF_SDO_TRIS			(TRISCbits.TRISC5)
//	#define WF_RESET_TRIS		(TRISBbits.TRISB1)
//	#define WF_RESET_IO			(LATBbits.LATB1)
//	#define WF_INT_TRIS		    (TRISBbits.TRISB0)
//	#define WF_INT_IO			(PORTBbits.RB0)
//	#define WF_CS_IO			(LATCbits.LATC2)
//	#define WF_HIBERNATE_TRIS   (TRISBbits.TRISB2)
//	#define	WF_HIBERNATE_IO		(PORTBbits.RB2)
//	#define WF_INT_EDGE		    (INTCON2bits.INTEDG0)
//	#define WF_INT_IE			(INTCONbits.INT0IE)
//	#define WF_INT_IF			(INTCONbits.INT0IF)
//	#define WF_SPI_IF			(PIR1bits.SSPIF)
//	#define WF_SSPBUF			(SSP1BUF)
//	#define WF_SPISTAT			(SSP1STAT)
//	#define WF_SPISTATbits		(SSP1STATbits)
//	#define WF_SPICON1			(SSP1CON1)
//	#define WF_SPICON1bits		(SSP1CON1bits)
//	#define WF_SPICON2			(SSP1CON2)
//	#define WF_SPI_IE			(PIE1bits.SSPIE)
//	#define WF_SPI_IP			(IPR1bits.SSPIP)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISA3)
	#define EEPROM_CS_IO		(LATA3)
	#define EEPROM_SCK_TRIS		(TRISC3)
	#define EEPROM_SDI_TRIS		(TRISC4)
	#define EEPROM_SDO_TRIS		(TRISC5)
	#define EEPROM_SPI_IF		(SSP1IF)
	#define EEPROM_SSPBUF		(SSP1BUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	// TODO: Need to add support for LCD behind MCP23S17 I/O expander.  This
	// requires code that isn't in the TCP/IP stack, not just a hardware
	// profile change.

	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#define TXSTAbits			TXSTA1bits
	#define RCSTAbits			RCSTA1bits
	#define BAUDCONbits			BAUDCON1bits
	#define GO					GODONE
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
		#define ADCON2		ADCON1
	#endif

#elif defined(HPC_EXPLORER) && !defined(HI_TECH_C)
// PICDEM HPC Explorer + Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail
	// I/O pins
	#define LED0_TRIS			(TRISDbits.TRISD0)
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(TRISDbits.TRISD3)
	#define LED3_IO				(LATDbits.LATD3)
	#define LED4_TRIS			(TRISDbits.TRISD4)
	#define LED4_IO				(LATDbits.LATD4)
	#define LED5_TRIS			(TRISDbits.TRISD5)
	#define LED5_IO				(LATDbits.LATD5)
	#define LED6_TRIS			(TRISDbits.TRISD6)
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(TRISDbits.TRISD7)
	#define LED7_IO				(LATDbits.LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))


	#define BUTTON0_TRIS		(TRISBbits.TRISB0)
	#define	BUTTON0_IO			(PORTBbits.RB0)
	#define BUTTON1_TRIS		(TRISBbits.TRISB0)	// No Button1 on this board, remap to Button0
	#define	BUTTON1_IO			(PORTBbits.RB0)
	#define BUTTON2_TRIS		(TRISBbits.TRISB0)	// No Button2 on this board, remap to Button0
	#define	BUTTON2_IO			(PORTBbits.RB0)
	#define BUTTON3_TRIS		(TRISBbits.TRISB0)	// No Button3 on this board, remap to Button0
	#define	BUTTON3_IO			(PORTBbits.RB0)

//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
//	#define ENC100_INTERFACE_MODE			0		// Uncomment this line to use the ENC424J600/624J600 Ethernet controller (SPI mode) or comment it out to use some other network controller
//
//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus I/O pins
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
//	#define ENC100_MDIX_IO					(LATBbits.LATB4)
//	#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
//	#define ENC100_POR_IO					(LATBbits.LATB5)
////	#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
////	#define ENC100_INT_IO					(PORTBbits.RB2)
//
//	// ENC424J600/624J600 SPI pinout
//	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
//	#define ENC100_CS_IO					(LATBbits.LATB3)
//	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
//	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
//	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
//	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
//
//	// ENC424J600/624J600 SPI SFR register selection (controls which SPI
//	// peripheral to use on PICs with multiple SPI peripherals).
////	#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
////	#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
////	#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
////	#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(PIR1bits.SSPIF)
//	#define ENC100_SSPBUF			(SSP1BUF)
//	#define ENC100_SPISTAT			(SSP1STAT)
//	#define ENC100_SPISTATbits		(SSP1STATbits)
//	#define ENC100_SPICON1			(SSP1CON1)
//	#define ENC100_SPICON1bits		(SSP1CON1bits)
//	#define ENC100_SPICON2			(SSP1CON2)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISBbits.TRISB5)
	#define ENC_RST_IO			(LATBbits.LATB5)
	#define ENC_CS_TRIS			(TRISBbits.TRISB3)	// Uncomment this line to use the ENC28J60 Ethernet controller or comment it out to use some other network controller
	#define ENC_CS_IO			(LATBbits.LATB3)
	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
	#define ENC_SPI_IF			(PIR1bits.SSPIF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISBbits.TRISB4)
	#define EEPROM_CS_IO		(LATBbits.LATB4)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
	#define EEPROM_SSPBUF		(SSP1BUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
		#define ADCON2		ADCON1
	#endif

#elif defined(HPC_EXPLORER) && defined(HI_TECH_C)
// PICDEM HPC Explorer + Ethernet PICtail
	#define TXSTA				TXSTA1
	#define RCSTA				RCSTA1
	#define SPBRG				SPBRG1
	#define SPBRGH				SPBRGH1
	#define RCREG				RCREG1
	#define TXREG				TXREG1

	// I/O pins
	#define LED0_TRIS			(TRISD0)
	#define LED0_IO				(LATD0)
	#define LED1_TRIS			(TRISD1)
	#define LED1_IO				(LATD1)
	#define LED2_TRIS			(TRISD2)
	#define LED2_IO				(LATD2)
	#define LED3_TRIS			(TRISD3)
	#define LED3_IO				(LATD3)
	#define LED4_TRIS			(TRISD4)
	#define LED4_IO				(LATD4)
	#define LED5_TRIS			(TRISD5)
	#define LED5_IO				(LATD5)
	#define LED6_TRIS			(TRISD6)
	#define LED6_IO				(LATD6)
	#define LED7_TRIS			(TRISD7)
	#define LED7_IO				(LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))

	#define BUTTON0_TRIS		(TRISB0)
	#define	BUTTON0_IO			(RB0)
	#define BUTTON1_TRIS		(TRISB0)	// No Button1 on this board, remap to Button0
	#define	BUTTON1_IO			(RB0)
	#define BUTTON2_TRIS		(TRISB0)	// No Button2 on this board, remap to Button0
	#define	BUTTON2_IO			(RB0)
	#define BUTTON3_TRIS		(TRISB0)	// No Button3 on this board, remap to Button0
	#define	BUTTON3_IO			(RB0)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISB5)
	#define ENC_RST_IO			(LATB5)
	#define ENC_CS_TRIS			(TRISB3)
	#define ENC_CS_IO			(LATB3)
	#define ENC_SCK_TRIS		(TRISC3)
	#define ENC_SDI_TRIS		(TRISC4)
	#define ENC_SDO_TRIS		(TRISC5)
	#define ENC_SPI_IF			(SSP1IF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISB4)
	#define EEPROM_CS_IO		(LATB4)
	#define EEPROM_SCK_TRIS		(TRISC3)
	#define EEPROM_SDI_TRIS		(TRISC4)
	#define EEPROM_SDO_TRIS		(TRISC5)
	#define EEPROM_SPI_IF		(SSP1IF)
	#define EEPROM_SSPBUF		(SSP1BUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

#elif defined(PIC24FJ64GA004_PIM)
// Explorer 16 + PIC24FJ64GA004 PIM + Ethernet PICtail Plus

	// Push Button I/O pins
	#define BUTTON3_TRIS		TRISAbits.TRISA10		// Mutliplexed with LED0
	#define BUTTON3_IO			PORTAbits.RA10
	#define BUTTON2_TRIS		TRISAbits.TRISA9		// Multiplexed with LED4
	#define BUTTON2_IO			PORTAbits.RA9
	#define BUTTON1_TRIS		TRISCbits.TRISC6		// Multiplexed with LED7
	#define BUTTON1_IO			PORTCbits.RC6
	#define BUTTON0_TRIS		TRISAbits.TRISA7		// Multiplexed with LED1
	#define BUTTON0_IO			PORTAbits.RA7

	// LED I/O pins
	#define LED0_TRIS			TRISAbits.TRISA10		// Multiplexed with BUTTON3
	#define LED0_IO				LATAbits.LATA10
	#define LED1_TRIS			TRISAbits.TRISA7		// Multiplexed with BUTTON0
	#define LED1_IO				LATAbits.LATA7
	#define LED2_TRIS			TRISBbits.TRISB8		// Multiplexed with LCD_DATA4
	#define LED2_IO				LATBbits.LATB8
	#define LED3_TRIS			TRISBbits.TRISB9		// Multiplexed with LCD_DATA3
	#define LED3_IO				LATBbits.LATB9
	#define LED4_TRIS			TRISAbits.TRISA9		// Multiplexed with BUTTON2
	#define LED4_IO				LATAbits.LATA9
	#define LED5_TRIS			TRISAbits.TRISA8		// Multiplexed with EEPROM_CS
	#define LED5_IO				LATAbits.LATA8
	#define LED6_TRIS			TRISBbits.TRISB12		// Multiplexed with LCD_DATA0
	#define LED6_IO				LATBbits.LATB12
	#define LED7_TRIS			TRISCbits.TRISC6		// Multiplexed with BUTTON1
	#define LED7_IO				LATCbits.LATC6
	#define LED_GET()			(0u)
	#define LED_PUT(a)

	// UART I/O Mapping
	#define UARTTX_TRIS			(TRISCbits.TRISC9)
	#define UARTTX_IO			(PORTCbits.RC9)
	#define UARTRX_TRIS			(TRISCbits.TRISC3)
	#define UARTRX_IO			(PORTCbits.RC3)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISCbits.TRISC8)	// Not connected by default
	#define ENC_RST_IO			(PORTCbits.RC8)
	#define ENC_CS_TRIS			(TRISBbits.TRISB3)
	#define ENC_CS_IO			(PORTBbits.RB3)
	// SPI SCK, SDI, SDO pins are automatically controlled by the
	// PIC24/dsPIC/PIC32 SPI module
	#define ENC_SPI_IF			(IFS0bits.SPI1IF)
	#define ENC_SSPBUF			(SPI1BUF)
	#define ENC_SPISTAT			(SPI1STAT)
	#define ENC_SPISTATbits		(SPI1STATbits)
	#define ENC_SPICON1			(SPI1CON1)
	#define ENC_SPICON1bits		(SPI1CON1bits)
	#define ENC_SPICON2			(SPI1CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISAbits.TRISA8)
	#define EEPROM_CS_IO		(PORTAbits.RA8)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC8)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(IFS2bits.SPI2IF)
	#define EEPROM_SSPBUF		(SPI2BUF)
	#define EEPROM_SPICON1		(SPI2CON1)
	#define EEPROM_SPICON1bits	(SPI2CON1bits)
	#define EEPROM_SPICON2		(SPI2CON2)
	#define EEPROM_SPISTAT		(SPI2STAT)
	#define EEPROM_SPISTATbits	(SPI2STATbits)

	// LCD Module I/O pins
	#define LCD_DATA0_TRIS		(TRISBbits.TRISB12)		// Multiplexed with LED6
	#define LCD_DATA0_IO		(LATBbits.LATB12)
	#define LCD_DATA1_TRIS		(TRISBbits.TRISB11)
	#define LCD_DATA1_IO		(LATBbits.LATB11)
	#define LCD_DATA2_TRIS		(TRISBbits.TRISB10)
	#define LCD_DATA2_IO		(LATBbits.LATB10)
	#define LCD_DATA3_TRIS		(TRISBbits.TRISB9)		// Multiplexed with LED3
	#define LCD_DATA3_IO		(LATBbits.LATB9)
	#define LCD_DATA4_TRIS		(TRISBbits.TRISB8)		// Multiplexed with LED2
	#define LCD_DATA4_IO		(LATBbits.LATB8)
	#define LCD_DATA5_TRIS		(TRISBbits.TRISB7)
	#define LCD_DATA5_IO		(LATBbits.LATB7)
	#define LCD_DATA6_TRIS		(TRISBbits.TRISB6)
	#define LCD_DATA6_IO		(LATBbits.LATB6)
	#define LCD_DATA7_TRIS		(TRISBbits.TRISB5)
	#define LCD_DATA7_IO		(LATBbits.LATB5)
	#define LCD_RD_WR_TRIS		(TRISBbits.TRISB13)
	#define LCD_RD_WR_IO		(LATBbits.LATB13)
	#define LCD_RS_TRIS			(TRISCbits.TRISC7)
	#define LCD_RS_IO			(LATCbits.LATC7)
	#define LCD_E_TRIS			(TRISBbits.TRISB14)
	#define LCD_E_IO			(LATBbits.LATB14)

	// Peripheral Pin Select Outputs
	#define NULL_IO		0
	#define C1OUT_IO	1
	#define C2OUT_IO	2
	#define U1TX_IO		3
	#define U1RTS_IO	4
	#define U2TX_IO		5
	#define U2RTS_IO	6
	#define SDO1_IO		7
	#define SCK1OUT_IO	8
	#define SS1OUT_IO	9
	#define SDO2_IO		10
	#define SCK2OUT_IO	11
	#define SS2OUT_IO	12
	#define OC1_IO		18
	#define OC2_IO		19
	#define OC3_IO		20
	#define OC4_IO		21
	#define OC5_IO		22

#elif defined(EXPLORER_16)
// Explorer 16 + PIC24FJ128GA010/PIC24HJ256GP610/dsPIC33FJ256GP710/
//				 PIC32MX460F512L/PIC32MX360F512L/PIC32MX795F512L PIM +
//               Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail Plus or MRF24WB0M WiFi PICtail Plus

	#define LED0_TRIS			(TRISAbits.TRISA0)	// Ref D3
	#define LED0_IO				(LATAbits.LATA0)
	#define LED1_TRIS			(TRISAbits.TRISA1)	// Ref D4
	#define LED1_IO				(LATAbits.LATA1)
	#define LED2_TRIS			(TRISAbits.TRISA2)	// Ref D5
	#define LED2_IO				(LATAbits.LATA2)
	#define LED3_TRIS			(TRISAbits.TRISA3)	// Ref D6
	#define LED3_IO				(LATAbits.LATA3)
	#define LED4_TRIS			(TRISAbits.TRISA4)	// Ref D7
	#define LED4_IO				(LATAbits.LATA4)
	#define LED5_TRIS			(TRISAbits.TRISA5)	// Ref D8
	#define LED5_IO				(LATAbits.LATA5)
	#define LED6_TRIS			(TRISAbits.TRISA6)	// Ref D9
	#define LED6_IO				(LATAbits.LATA6)
	#define LED7_TRIS			(TRISAbits.TRISA7)	// Ref D10	// Note: This is multiplexed with BUTTON1
	#define LED7_IO				(LATAbits.LATA7)
	#define LED_GET()			(*((volatile unsigned char*)(&LATA)))
	#define LED_PUT(a)			(*((volatile unsigned char*)(&LATA)) = (a))


	#define BUTTON0_TRIS		(TRISDbits.TRISD13)	// Ref S4
	#define	BUTTON0_IO			(PORTDbits.RD13)
	#define BUTTON1_TRIS		(TRISAbits.TRISA7)	// Ref S5	// Note: This is multiplexed with LED7
	#define	BUTTON1_IO			(PORTAbits.RA7)
	#define BUTTON2_TRIS		(TRISDbits.TRISD7)	// Ref S6
	#define	BUTTON2_IO			(PORTDbits.RD7)
	#define BUTTON3_TRIS		(TRISDbits.TRISD6)	// Ref S3
	#define	BUTTON3_IO			(PORTDbits.RD6)

	#define UARTTX_TRIS			(TRISFbits.TRISF5)
	#define UARTTX_IO			(PORTFbits.RF5)
	#define UARTRX_TRIS			(TRISFbits.TRISF4)
	#define UARTRX_IO			(PORTFbits.RF4)

	// ENC28J60 I/O pins
	#if defined(__PIC24FJ256GA110__)	// PIC24FJ256GA110 must place the ENC28J60 on SPI2 because PIC rev A3 SCK1 output pin is a PPS input only (fixed on A5, but demos use SPI2 for simplicity)
		#define ENC_CS_TRIS			(TRISFbits.TRISF12)	// Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
		#define ENC_CS_IO			(LATFbits.LATF12)
		// SPI SCK, SDI, SDO pins are automatically controlled by the
		// PIC24/dsPIC/PIC32 SPI module
		#define ENC_SPI_IF			(IFS2bits.SPI2IF)
		#define ENC_SSPBUF			(SPI2BUF)
		#define ENC_SPISTAT			(SPI2STAT)
		#define ENC_SPISTATbits		(SPI2STATbits)
		#define ENC_SPICON1			(SPI2CON1)
		#define ENC_SPICON1bits		(SPI2CON1bits)
		#define ENC_SPICON2			(SPI2CON2)
	#else	// SPI1 for all other processors
		#define ENC_CS_TRIS			(TRISDbits.TRISD14)	// Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
		#define ENC_CS_IO			(LATDbits.LATD14)
		// SPI SCK, SDI, SDO pins are automatically controlled by the
		// PIC24/dsPIC/PIC32 SPI module
		#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
			#define ENC_SPI_IF			(IFS0bits.SPI1IF)
			#define ENC_SSPBUF			(SPI1BUF)
			#define ENC_SPISTAT			(SPI1STAT)
			#define ENC_SPISTATbits		(SPI1STATbits)
			#define ENC_SPICON1			(SPI1CON1)
			#define ENC_SPICON1bits		(SPI1CON1bits)
			#define ENC_SPICON2			(SPI1CON2)
		#else					// PIC32
			#define ENC_SPI_IF			(IFS0bits.SPI1RXIF)
			#define ENC_SSPBUF			(SPI1BUF)
			#define ENC_SPISTATbits		(SPI1STATbits)
			#define ENC_SPICON1			(SPI1CON)
			#define ENC_SPICON1bits		(SPI1CONbits)
			#define ENC_SPIBRG			(SPI1BRG)
		#endif
	#endif


//	// ENC624J600 Interface Configuration
//	// Comment out ENC100_INTERFACE_MODE if you don't have an ENC624J600 or
//	// ENC424J600.  Otherwise, choose the correct setting for the interface you
//	// are using.  Legal values are:
//	//  - Commented out: No ENC424J600/624J600 present or used.  All other
//	//                   ENC100_* macros are ignored.
//	//	- 0: SPI mode using CS, SCK, SI, and SO pins
//	//  - 1: 8-bit demultiplexed PSP Mode 1 with RD and WR pins
//	//  - 2: *8-bit demultiplexed PSP Mode 2 with R/Wbar and EN pins
//	//  - 3: *16-bit demultiplexed PSP Mode 3 with RD, WRL, and WRH pins
//	//  - 4: *16-bit demultiplexed PSP Mode 4 with R/Wbar, B0SEL, and B1SEL pins
//	//  - 5: 8-bit multiplexed PSP Mode 5 with RD and WR pins
//	//  - 6: *8-bit multiplexed PSP Mode 6 with R/Wbar and EN pins
//	//  - 9: 16-bit multiplexed PSP Mode 9 with AL, RD, WRL, and WRH pins
//	//  - 10: *16-bit multiplexed PSP Mode 10 with AL, R/Wbar, B0SEL, and B1SEL
//	//        pins
//	// *IMPORTANT NOTE: DO NOT USE PSP MODE 2, 4, 6, OR 10 ON EXPLORER 16!
//	// Attempting to do so will cause bus contention with the LCD module which
//	// shares the PMP.  Also, PSP Mode 3 is risky on the Explorer 16 since it
//	// can randomly cause bus contention with the 25LC256 EEPROM.
//	#define ENC100_INTERFACE_MODE			0
//
//	// If using a parallel interface, direct RAM addressing can be used (if all
//	// addresses wires are connected), or a reduced number of pins can be used
//	// for indirect addressing.  If using an SPI interface or PSP Mode 9 or 10
//	// (multiplexed 16-bit modes), which require all address lines to always be
//	// connected, then this option is ignored. Comment out or uncomment this
//	// macro to match your hardware connections.
//	#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
//
//	// ENC424J600/624J600 parallel indirect address remapping macro function.
//	// This section translates SFR and RAM addresses presented to the
//	// ReadMemory() and WriteMemory() APIs in ENCX24J600.c to the actual
//	// addresses that must be presented on the parallel interface.  This macro
//	// must be modified to match your hardware if you are using an indirect PSP
//	// addressing mode (ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING is defined) and
//	// have some of your address lines tied off to Vdd.  If you are using the
//	// SPI interface, then this section can be ignored or deleted.
//	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) // 8-bit PSP
//		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
//	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) // 16-bit PSP
//		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		(a)
//	#endif
//
//	// Auto-crossover pins on Fast 100Mbps Ethernet PICtail/PICtail Plus.  If
//	// your circuit doesn't have such a feature, delete these two defines.
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB3)
//	#define ENC100_MDIX_IO					(LATBbits.LATB3)
//
//	// ENC624J600 I/O control and status pins
//	// If a pin is not required for your selected ENC100_INTERFACE_MODE
//	// interface selection (ex: WRH/B1SEL for PSP modes 1, 2, 5, and 6), then
//	// you can ignore, delete, or put anything for the pin definition.  Also,
//	// the INT and POR pins are entirely optional.  If not connected, comment
//	// them out.
//	#if defined(__dsPIC33FJ256GP710__) || defined(__PIC24HJ256GP610__)
//		#define ENC100_INT_TRIS				(TRISAbits.TRISA13)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
//		#define ENC100_INT_IO				(PORTAbits.RA13)
//	#else
//		#define ENC100_INT_TRIS				(TRISEbits.TRISE9)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
//		#define ENC100_INT_IO				(PORTEbits.RE9)
//	#endif
//	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
//		// PSP control signal pinout
//		#define ENC100_CS_TRIS					(TRISAbits.TRISA5)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
//		#define ENC100_CS_IO					(LATAbits.LATA5)
//		#define ENC100_POR_TRIS					(TRISCbits.TRISC1)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//		#define ENC100_POR_IO					(LATCbits.LATC1)
//		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISDbits.TRISD4)
//		#define ENC100_SO_WR_B0SEL_EN_IO		(LATDbits.LATD4)
//		#define ENC100_SI_RD_RW_TRIS			(TRISDbits.TRISD5)
//		#define ENC100_SI_RD_RW_IO				(LATDbits.LATD5)
//		#define ENC100_SCK_AL_TRIS				(TRISBbits.TRISB15)
//		#define ENC100_SCK_AL_IO				(LATBbits.LATB15)
//	#else
//		// SPI pinout
//		#if defined(__PIC24FJ256GA110__)	// The PIC24FJ256GA110 must use SPI2 slot on Explorer 16.  If you don't have a PIC24FJ256GA110 but want to use SPI2 for some reason, you can use these definitions.
//			#define ENC100_CS_TRIS					(TRISFbits.TRISF12)	// CS is mandatory when using the SPI interface
//			#define ENC100_CS_IO					(LATFbits.LATF12)
//			#define ENC100_POR_TRIS					(TRISFbits.TRISF13)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//			#define ENC100_POR_IO					(LATFbits.LATF13)
//		#else	// All other PIC24s, dsPICs, and PIC32s use SPI1 slot (top most closest to LCD)
//			#define ENC100_CS_TRIS					(TRISDbits.TRISD14)	// CS is mandatory when using the SPI interface
//			#define ENC100_CS_IO					(LATDbits.LATD14)
//			#define ENC100_POR_TRIS					(TRISDbits.TRISD15)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//			#define ENC100_POR_IO					(LATDbits.LATD15)
//		#endif
//	#endif
//
//	// ENC624J600 Bit Bang PSP I/O macros and pin configuration for address and
//	// data.  If using the SPI interface (ENC100_INTERFACE_MODE is 0) then this
//	// section is not used and can be ignored or deleted.  If using the PIC PMP
//	// hardware module (if present), then ENC100_BIT_BANG_PMP must be commented
//	// out and the remaining definitions will be ignored/can be deleted.
//	// Otherwise, if you are using a parallel interface mode, but do not have a
//	// PMP (or want to interface using different pins), define
//	// ENC100_BIT_BANG_PMP and properly configure the applicable macros.
//	//#define ENC100_BIT_BANG_PMP
//	#if defined(ENC100_BIT_BANG_PMP)
//		#if ENC100_INTERFACE_MODE == 1 || ENC100_INTERFACE_MODE == 2	// Dumultiplexed 8-bit address/data modes
//			// SPI2 CANNOT BE ENABLED WHEN ACCESSING THE ENC624J600 FOR THESE TWO MODES AS THE PINS OVERLAP WITH ADDRESS LINES.
//			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A8 connected (A9-A14 tied to Vdd)
//				#if defined(__PIC24FJ256GB210__)
//					#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0xF9E7; ANSB &= 0x3FFF; ANSG &= 0xFCFF;} while(0)		// RE0-RE7, RF12, RD11, RD4, RD5 (AD0-AD7, A5, A8, WR, RD) pins are already digital only pins.
//				#else
//					#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF; ((volatile BYTE*)&AD1PCFGL)[1] |= 0xC0;}while(0)	// Disable AN24-AN31 and AN14-AN15 analog inputs on RE0-RE7 and RB14-RB15 pins (ENCX24J600 AD0-AD7, A1-A0)
//				#endif
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x3FFF; TRISFbits.TRISF12 = 0; TRISGbits.TRISG9 = 0; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATGbits.LATG9 = 0; LATA &= 0xF9E7; LATFbits.LATF12 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATAbits.LATA4 = 1; if(_SetMacro & 0x0010) LATAbits.LATA3 = 1; if(_SetMacro & 0x0020) LATFbits.LATF12 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_IN()		(((volatile BYTE*)&TRISE)[0] = 0xFF)
//				#define ENC100_SET_AD_TRIS_OUT()	(((volatile BYTE*)&TRISE)[0] = 0x00)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#else 	// All ENC624J600 address pins A0-A14 connected
//				#if defined(__PIC24FJ256GB210__)
//					#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0xF9E7; ANSB &= 0x03FF; ANSG &= 0xFCFF;} while(0)		// RE0-RE7, RF12, RD11, RD4, RD5 (AD0-AD7, A5, A14, WR, RD) pins are already digital only pins.
//				#else
//					#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF; ((volatile BYTE*)&AD1PCFGL)[1] |= 0xFC;}while(0)	// Disable AN24-AN31 and AN10-AN15 analog inputs on RE0-RE7 and RB10-RB15 pins (ENCX24J600 AD0-AD7, A1-A0, A13-A10)
//				#endif
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x03FF; TRISF &= 0xEFCF; TRISGbits.TRISG9 = 0; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATA &= 0xF9E7; LATB &= 0x03FF; LATF &= 0xEFCF; LATGbits.LATG9 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATAbits.LATA4 = 1; if(_SetMacro & 0x0010) LATAbits.LATA3 = 1; if(_SetMacro & 0x0020) LATFbits.LATF12 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x0100) LATFbits.LATF5 = 1; if(_SetMacro & 0x0200) LATFbits.LATF4 = 1; if(_SetMacro & 0x0400) LATBbits.LATB13 = 1; if(_SetMacro & 0x0800) LATBbits.LATB12 = 1; if(_SetMacro & 0x1000) LATBbits.LATB11 = 1; if(_SetMacro & 0x2000) LATBbits.LATB10 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_IN()		(((volatile BYTE*)&TRISE)[0] = 0xFF)
//				#define ENC100_SET_AD_TRIS_OUT()	(((volatile BYTE*)&TRISE)[0] = 0x00)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#endif
//		#elif ENC100_INTERFACE_MODE == 3 || ENC100_INTERFACE_MODE == 4	// Dumultiplexed 16-bit address/data modes
//			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A7 connected (A8-A13 tied to Vdd)
//				#if defined(__PIC24FJ256GB210__)
//					#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0x79E7; ANSB &= 0x3FFF; ANSD &= 0xCF0F; ANSG &= 0xFCFC;}while(0)
//				#else
//					#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGH = 0xFFFF; AD1PCFGL = 0xFFFF; AD2PCFGL = 0xFFFF;}while(0)
//				#endif
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISBbits.TRISB15 = 0; TRISBbits.TRISB14 = 0; TRISFbits.TRISF12 = 0; TRISGbits.TRISG9 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _wSetMacro = (a); LATA &= 0xF9E7; LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATFbits.LATF12 = 0; LATGbits.LATG9 = 0; if(_wSetMacro & 0x0001) LATBbits.LATB15 = 1; if(_wSetMacro & 0x0002) LATBbits.LATB14 = 1; if(_wSetMacro & 0x0004) LATGbits.LATG9 = 1; if(_wSetMacro & 0x0008) LATAbits.LATA4 = 1; if(_wSetMacro & 0x0010) LATAbits.LATA3 = 1; if(_wSetMacro & 0x0020) LATFbits.LATF12 = 1; if(_wSetMacro & 0x0040) LATAbits.LATA10 = 1; if(_wSetMacro & 0x0080) LATAbits.LATA9 = 1;}while(0)
//				#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//				#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//				#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//				#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#else 	// All ENC624J600 address pins A0-A13 connected
//				#if defined(__PIC24FJ256GB210__)
//					#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0x79E7; ANSB &= 0x03FF; ANSD &= 0xCF0F; ANSG &= 0xFCFC;}while(0)
//				#else
//					#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGH = 0xFFFF; AD1PCFGL = 0xFFFF; AD2PCFGL = 0xFFFF;}while(0)
//				#endif
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x03FF; TRISF &= 0xEFCF; TRISGbits.TRISG9 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _wSetMacro = (a); LATA &= 0xF9E7; LATB &= 0x03FF; LATF &= 0xEFCF; LATGbits.LATG9 = 0; if(_wSetMacro & 0x0001) LATBbits.LATB15 = 1; if(_wSetMacro & 0x0002) LATBbits.LATB14 = 1; if(_wSetMacro & 0x0004) LATGbits.LATG9 = 1; if(_wSetMacro & 0x0008) LATAbits.LATA4 = 1; if(_wSetMacro & 0x0010) LATAbits.LATA3 = 1; if(_wSetMacro & 0x0020) LATFbits.LATF12 = 1; if(_wSetMacro & 0x0040) LATAbits.LATA10 = 1; if(_wSetMacro & 0x0080) LATAbits.LATA9 = 1; if(_wSetMacro & 0x0100) LATFbits.LATF5 = 1; if(_wSetMacro & 0x0200) LATFbits.LATF4 = 1; if(_wSetMacro & 0x0400) LATBbits.LATB13 = 1; if(_wSetMacro & 0x0800) LATBbits.LATB12 = 1; if(_wSetMacro & 0x1000) LATBbits.LATB11 = 1; if(_wSetMacro & 0x2000) LATBbits.LATB10 = 1;}while(0)
//				#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//				#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//				#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//				#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#endif
//		#elif ENC100_INTERFACE_MODE == 5 || ENC100_INTERFACE_MODE == 6	// Mutliplexed 8-bit address/data modes
//			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENCX24J600 address pins AD0-AD8 connected (AD9-AD14 tied to Vdd)
//				#if defined(__PIC24FJ256GB210__)
//					#define ENC100_INIT_PSP_BIT_BANG()	do{ANSB &= 0x7FFF; ANSG &= 0xFEFF;} while(0)		// RE0-RE7, RD11, RD4, RD5 (AD0-AD7, AD8, WR, RD) pins are already digital only pins.  RB15, RG8 (AL, CS) needs to be made digital only.
//				#else
//					#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN24-AN31 analog inputs on RE0-RE7 pins (ENCX24J600 AD0-AD7)
//				#endif
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = (BYTE)_wSetMacro; LATDbits.LATD11 = 0; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#else 	// All ENCX24J600 address pins AD0-AD14 connected
//				// This pinout is bad for doing 8-bit bit-bang operations with all address lines.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 15 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
//				// UART2 CANNOT BE USED OR ENABLED FOR THESE TWO MODES AS THE PINS OVERLAP WITH ADDRESS LINES.
//				#if defined(__PIC24FJ256GB210__)
//					#define ENC100_INIT_PSP_BIT_BANG()	do{ANSB &= 0x43FF; ANSG &= 0xFEFF;} while(0) // Set pins as digital I/Os (not analog).  RD11, RD5, RD4, RE0-RE7, RF4, RF5 are all digital-only pins and therefore no writes to ANSD, ANSE, or ANSF are needed.
//				#else
//					#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGL |= 0x3C00; ((volatile BYTE*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN10-AN13 and AN24-AN31 analog inputs on RB10-RB13 and RE0-RE7 pins (ENCX24J600 AD13-AD10 and AD0-AD7)
//				#endif
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISFbits.TRISF5 = 0; TRISFbits.TRISF4 = 0; TRISB &= 0x43FF; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = (BYTE)_wSetMacro; LATFbits.LATF5 = 0; LATFbits.LATF4 = 0; LATB &= 0x43FF; LATDbits.LATD11 = 0; if(_wSetMacro & 0x0100) LATFbits.LATF5 = 1; if(_wSetMacro & 0x0200) LATFbits.LATF4 = 1; if(_wSetMacro & 0x0400) LATBbits.LATB13 = 1; if(_wSetMacro & 0x0800) LATBbits.LATB12 = 1; if(_wSetMacro & 0x1000) LATBbits.LATB11 = 1;  if(_wSetMacro & 0x2000) LATBbits.LATB10 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#endif
//		#elif ENC100_INTERFACE_MODE == 9 || ENC100_INTERFACE_MODE == 10	// Mutliplexed 16-bit address/data modes
//			// All ENC624J600 adddress/data pins AD0-AD15 connected (required for 16-bit data, so there is no differentiation for indirect versus direct addressing mode)
//			// This pinout is awful for doing 16-bit bit-bang operations.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 16 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSBbits.ANSB15 = 0; ANSCbits.ANSC13 = 0; ANSD &= 0xCF0F; ANSGbits.ANSG8 = 0;}while(0)	// Set pins as digital I/Os (not analog).  RA15 and RE0-RE7 are all digital-only pins and therefore no writes to ANSA or ANSE are needed.
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN24-AN31 analog inputs on RE0-RE7 pins (ENCX24J600 AD0-AD7)
//			#endif
//			#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//			#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//			#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//			#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//			#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//			#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//			#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//		#endif
//	#endif
//
//	// ENC624J600 SPI SFR register selection (controls which SPI peripheral to
//	// use on PICs with multiple SPI peripherals).  If a parallel interface is
//	// used (ENC100_INTERFACE_MODE is >= 1), then the SPI is not used and this
//	// section can be ignored or deleted.
//	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
//		#if defined(__PIC24FJ256GA110__)	// The PIC24FJ256GA110 must use SPI2 slot on Explorer 16.  If you don't have a PIC24FJ256GA110 but want to use SPI2 for some reason, you can use these definitions.
//			#define ENC100_ISR_ENABLE		(IEC3bits.INT4IE)
//			#define ENC100_ISR_FLAG			(IFS3bits.INT4IF)
//			#define ENC100_ISR_POLARITY		(INTCON2bits.INT4EP)
//			#define ENC100_ISR_PRIORITY		(IPC13bits.INT4IP)
//			#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//			#define ENC100_SPI_IF			(IFS1bits.SPI2IF)
//			#define ENC100_SSPBUF			(SPI2BUF)
//			#define ENC100_SPISTAT			(SPI2STAT)
//			#define ENC100_SPISTATbits		(SPI2STATbits)
//			#define ENC100_SPICON1			(SPI2CON1)
//			#define ENC100_SPICON1bits		(SPI2CON1bits)
//			#define ENC100_SPICON2			(SPI2CON2)
//		#else	// All other PIC24s and dsPICs use SPI1 slot (top most closest to LCD)
//			#define ENC100_ISR_ENABLE		(IEC1bits.INT2IE)
//			#define ENC100_ISR_FLAG			(IFS1bits.INT2IF)
//			#define ENC100_ISR_POLARITY		(INTCON2bits.INT2EP)
//			#define ENC100_ISR_PRIORITY		(IPC7bits.INT2IP)
//			#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//			#define ENC100_SPI_IF			(IFS0bits.SPI1IF)
//			#define ENC100_SSPBUF			(SPI1BUF)
//			#define ENC100_SPISTAT			(SPI1STAT)
//			#define ENC100_SPISTATbits		(SPI1STATbits)
//			#define ENC100_SPICON1			(SPI1CON1)
//			#define ENC100_SPICON1bits		(SPI1CON1bits)
//			#define ENC100_SPICON2			(SPI1CON2)
//		#endif
//	#else					// PIC32MX
//		#define ENC100_ISR_ENABLE		(IEC0bits.INT2IE)
//		#define ENC100_ISR_FLAG			(IFS0bits.INT2IF)
//		#define ENC100_ISR_POLARITY		(INTCONbits.INT2EP)
//		#define ENC100_ISR_PRIORITY		(IPC2bits.INT2IP)
//		#define ENC100_SPI_ENABLE		(ENC100_SPICON1bits.ON)
//		#define ENC100_SPI_IF			(IFS0bits.SPI1RXIF)
//		#define ENC100_SSPBUF			(SPI1BUF)
//		#define ENC100_SPICON1			(SPI1CON)
//		#define ENC100_SPISTATbits		(SPI1STATbits)
//		#define ENC100_SPICON1bits		(SPI1CONbits)
//		#define ENC100_SPIBRG			(SPI1BRG)
//	#endif


	// 25LC256 I/O pins
	#if defined(__PIC24FJ256GB110__)
		// PIC24FJ256GB110 USB PIM has RD12 pin on Explorer 16 schematic
		// remapped and actually connected to PIC24FJ256GB110 pin 90 (RG0).
		#define EEPROM_CS_TRIS		(TRISGbits.TRISG0)
		#define EEPROM_CS_IO		(LATGbits.LATG0)
	#elif defined(__PIC24FJ256GB210__)
		// PIC24FJ256GB210 USB PIM has RD12 pin on Explorer 16 schematic
		// remapped and actually connected to PIC24FJ256GB210 pin 90 (RG0) when
		// JP1 on PIM has pins 1-2 shorted (USB).  When JP1 pins 2-3 are shorted
		// (PMP), PIC pin 90 does connect to RD12.  To make the PIM work with
		// either jumper setting, we will drive both RG0 and RD12 simultaneously
		// as chip select to the same states.  For an actual application, you'd
		// want to specify only the single necessary pin as this double
		// assignment operation generates inefficient code by the C compiler.
		#define EEPROM_CS_TRIS		TRISGbits.TRISG0 = TRISDbits.TRISD12
		#define EEPROM_CS_IO		LATGbits.LATG0 = LATDbits.LATD12
	#else
		#define EEPROM_CS_TRIS		(TRISDbits.TRISD12)
		#define EEPROM_CS_IO		(LATDbits.LATD12)
	#endif
	#define EEPROM_SCK_TRIS		(TRISGbits.TRISG6)
	#define EEPROM_SDI_TRIS		(TRISGbits.TRISG7)
	#define EEPROM_SDO_TRIS		(TRISGbits.TRISG8)
	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
		#define EEPROM_SPI_IF		(IFS2bits.SPI2IF)
		#define EEPROM_SSPBUF		(SPI2BUF)
		#define EEPROM_SPICON1		(SPI2CON1)
		#define EEPROM_SPICON1bits	(SPI2CON1bits)
		#define EEPROM_SPICON2		(SPI2CON2)
		#define EEPROM_SPISTAT		(SPI2STAT)
		#define EEPROM_SPISTATbits	(SPI2STATbits)
	#else					// PIC32
		#define EEPROM_SPI_IF		(IFS1bits.SPI2RXIF)
		#define EEPROM_SSPBUF		(SPI2BUF)
		#define EEPROM_SPICON1		(SPI2CON)
		#define EEPROM_SPICON1bits	(SPI2CONbits)
		#define EEPROM_SPIBRG		(SPI2BRG)
		#define EEPROM_SPISTAT		(SPI2STAT)
		#define EEPROM_SPISTATbits	(SPI2STATbits)
	#endif


	// LCD Module I/O pins.  NOTE: On the Explorer 16, the LCD is wired to the
	// same PMP lines required to communicate with an ENCX24J600 in parallel
	// mode.  Since the LCD does not have a chip select wire, if you are using
	// the ENC424J600/624J600 in parallel mode, the LCD cannot be used.
	#if !defined(ENC100_INTERFACE_MODE) || (ENC100_INTERFACE_MODE == 0)	// SPI only
		#define LCD_DATA_TRIS		(*((volatile BYTE*)&TRISE))
		#define LCD_DATA_IO			(*((volatile BYTE*)&LATE))
		#define LCD_RD_WR_TRIS		(TRISDbits.TRISD5)
		#define LCD_RD_WR_IO		(LATDbits.LATD5)
		#define LCD_RS_TRIS			(TRISBbits.TRISB15)
		#define LCD_RS_IO			(LATBbits.LATB15)
		#define LCD_E_TRIS			(TRISDbits.TRISD4)
		#define LCD_E_IO			(LATDbits.LATD4)
	#endif


//	// Serial Flash/SRAM/UART PICtail Plus attached to SPI2 (middle pin group)
//  // This daughter card is not in production, but if you custom attach an SPI
//  // RAM or SPI Flash chip to your board, then use these definitions as a
//  // starting point.
//	#define SPIRAM_CS_TRIS			(TRISGbits.TRISG9)
//	#define SPIRAM_CS_IO			(LATGbits.LATG9)
//	#define SPIRAM_SCK_TRIS			(TRISGbits.TRISG6)
//	#define SPIRAM_SDI_TRIS			(TRISGbits.TRISG7)
//	#define SPIRAM_SDO_TRIS			(TRISGbits.TRISG8)
//	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
//		#define SPIRAM_SPI_IF			(IFS2bits.SPI2IF)
//		#define SPIRAM_SSPBUF			(SPI2BUF)
//		#define SPIRAM_SPICON1			(SPI2CON1)
//		#define SPIRAM_SPICON1bits		(SPI2CON1bits)
//		#define SPIRAM_SPICON2			(SPI2CON2)
//		#define SPIRAM_SPISTAT			(SPI2STAT)
//		#define SPIRAM_SPISTATbits		(SPI2STATbits)
//	#else					// PIC32
//		#define SPIRAM_SPI_IF			(IFS1bits.SPI2RXIF)
//		#define SPIRAM_SSPBUF			(SPI2BUF)
//		#define SPIRAM_SPICON1			(SPI2CON)
//		#define SPIRAM_SPICON1bits		(SPI2CONbits)
//		#define SPIRAM_SPIBRG			(SPI2BRG)
//	#endif
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION,
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB8)
//	#define SPIFLASH_CS_IO			(LATBbits.LATB8)
//	#define SPIFLASH_SCK_TRIS		(TRISGbits.TRISG6)
//	#define SPIFLASH_SDI_TRIS		(TRISGbits.TRISG7)
//	#define SPIFLASH_SDI_IO			(PORTGbits.RG7)
//	#define SPIFLASH_SDO_TRIS		(TRISGbits.TRISG8)
//	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
//		#define SPIFLASH_SPI_IF			(IFS2bits.SPI2IF)
//		#define SPIFLASH_SSPBUF			(SPI2BUF)
//		#define SPIFLASH_SPICON1		(SPI2CON1)
//		#define SPIFLASH_SPICON1bits	(SPI2CON1bits)
//		#define SPIFLASH_SPICON2		(SPI2CON2)
//		#define SPIFLASH_SPISTAT		(SPI2STAT)
//		#define SPIFLASH_SPISTATbits	(SPI2STATbits)
//	#else					// PIC32
//		#define SPIFLASH_SPI_IF			(IFS1bits.SPI2RXIF)
//		#define SPIFLASH_SSPBUF			(SPI2BUF)
//		#define SPIFLASH_SPICON1		(SPI2CON)
//		#define SPIFLASH_SPICON1bits	(SPI2CONbits)
//		#define SPIFLASH_SPISTATbits	(SPI2STATbits)
//		#define SPIFLASH_SPIBRG			(SPI2BRG)
//	#endif

//    //----------------------------
//    // MRF24WB0M WiFi I/O pins
//    //----------------------------
//	// If you have a MRF24WB0M WiFi PICtail, you must uncomment one of
//	// these two lines to use it.  SPI1 is the top-most slot in the Explorer 16
//	// (closer to the LCD and prototyping area) while SPI2 corresponds to
//	// insertion of the PICtail into the middle of the side edge connector slot.
//	#define MRF24WB0M_IN_SPI1
//	//#define MRF24WB0M_IN_SPI2
//
//	// PIC24FJ256GA110 PIM on Explorer 16 must use SPI2, not SPI1
//	#if defined(MRF24WB0M_IN_SPI1) && defined(__PIC24FJ256GA110__)
//		#undef MRF24WB0M_IN_SPI1
//		#define MRF24WB0M_IN_SPI2
//	#endif
//
//    #if defined( MRF24WB0M_IN_SPI1 ) && !defined(__32MX460F512L__) && !defined(__32MX795F512L__) && !defined(__PIC24FJ256GA110__)
//        // MRF24WB0M in SPI1 slot
//        #define WF_CS_TRIS			(TRISBbits.TRISB2)
//    	#define WF_CS_IO			(LATBbits.LATB2)
//    	#define WF_SDI_TRIS			(TRISFbits.TRISF7)
//    	#define WF_SCK_TRIS			(TRISFbits.TRISF6)
//    	#define WF_SDO_TRIS			(TRISFbits.TRISF8)
//      	#define WF_RESET_TRIS		(TRISFbits.TRISF0)
//    	#define WF_RESET_IO			(LATFbits.LATF0)
//        #if defined(__dsPIC33FJ256GP710__) || defined(__PIC24HJ256GP610__)
//			#define WF_INT_TRIS	    (TRISAbits.TRISA12)
//			#define WF_INT_IO		(PORTAbits.RA12)
//        #else
//			#define WF_INT_TRIS	    (TRISEbits.TRISE8)  // INT1
//			#define WF_INT_IO		(PORTEbits.RE8)
//        #endif
//    	#define WF_HIBERNATE_TRIS	(TRISFbits.TRISF1)
//    	#define	WF_HIBERNATE_IO		(PORTFbits.RF1)
//    	#if defined( __C30__ )
//            #define WF_INT_EDGE		    (INTCON2bits.INT1EP)
//        	#define WF_INT_IE			(IEC1bits.INT1IE)
//        	#define WF_INT_IF			(IFS1bits.INT1IF)
//    	#elif defined( __PIC32MX__ )
//            #define WF_INT_EDGE		    (INTCONbits.INT1EP)
//        	#define WF_INT_IE			(IEC0bits.INT1IE)
//        	#define WF_INT_IF			(IFS0bits.INT1IF)
//        	#define WF_INT_IE_CLEAR     IEC0CLR
//        	#define WF_INT_IF_CLEAR     IFS0CLR
//        	#define WF_INT_IE_SET       IEC0SET
//        	#define WF_INT_IF_SET       IFS0SET
//        	#define WF_INT_BIT          0x00000080
//        	#define WF_INT_IPCSET       IPC1SET
//        	#define WF_INT_IPCCLR       IPC1CLR
//        	#define WF_INT_IPC_MASK     0xFF000000
//        	#define WF_INT_IPC_VALUE    0x0C000000
//        #else
//            #error Determine MRF24WB0M external interrupt
//        #endif
//
//    	#define WF_SSPBUF			(SPI1BUF)
//    	#define WF_SPISTAT			(SPI1STAT)
//    	#define WF_SPISTATbits		(SPI1STATbits)
//    	#if defined( __C30__ )
//        	#define WF_SPICON1			(SPI1CON1)
//        	#define WF_SPICON1bits		(SPI1CON1bits)
//        	#define WF_SPICON2			(SPI1CON2)
//        	#define WF_SPI_IE			(IEC0bits.SPI1IE)
//    //    	#define WF_SPI_IP			(IPC2bits.SPI1IP)
//        	#define WF_SPI_IF			(IFS0bits.SPI1IF)
//        #elif defined( __PIC32MX__ )
//        	#define WF_SPICON1			(SPI1CON)
//        	#define WF_SPICON1bits		(SPI1CONbits)
//            #define WF_SPI_IE_CLEAR     IEC0CLR
//            #define WF_SPI_IF_CLEAR     IFS0CLR
//            #define WF_SPI_INT_BITS     0x03800000
//    		#define WF_SPI_BRG		    (SPI1BRG)
//            #define WF_MAX_SPI_FREQ     (10000000ul)	// Hz
//        #else
//            #error Determine MRF24WB0M SPI information
//        #endif
//
//    #elif defined( MRF24WB0M_IN_SPI2 ) && !defined(__32MX460F512L__) && !defined(__32MX795F512L__)
//        // MRF24WB0M in SPI2 slot
//        #define WF_CS_TRIS			(TRISGbits.TRISG9)
//    	#define WF_CS_IO			(LATGbits.LATG9)
//    	#define WF_SDI_TRIS			(TRISGbits.TRISG7)
//    	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
//    	#define WF_SDO_TRIS			(TRISGbits.TRISG8)
//      	#define WF_RESET_TRIS		(TRISGbits.TRISG0)
//    	#define WF_RESET_IO			(LATGbits.LATG0)
//		#if defined(__PIC24FJ256GB110__) || defined(__PIC24FJ256GB210__)
//			#define WF_INT_TRIS		    (TRISCbits.TRISC3)	// INT3
//			#define WF_INT_IO			(PORTCbits.RC3)
//		#else
//			#define WF_INT_TRIS		    (TRISAbits.TRISA14)	// INT3
//			#define WF_INT_IO			(PORTAbits.RA14)
//		#endif
//    	#define WF_HIBERNATE_TRIS	    (TRISGbits.TRISG1)
//    	#define	WF_HIBERNATE_IO			(PORTGbits.RG1)
//    	#if defined( __C30__ )
//            #define WF_INT_EDGE		    (INTCON2bits.INT3EP)
//        	#define WF_INT_IE			(IEC3bits.INT3IE)
//        	#define WF_INT_IF			(IFS3bits.INT3IF)
//    	#elif defined( __PIC32MX__ )
//            #define WF_INT_EDGE		    (INTCONbits.INT3EP)
//        	#define WF_INT_IE			(IEC0bits.INT3IE)
//        	#define WF_INT_IF			(IFS0bits.INT3IF)
//        	#define WF_INT_IE_CLEAR     IEC0CLR
//        	#define WF_INT_IF_CLEAR     IFS0CLR
//        	#define WF_INT_IE_SET       IEC0SET
//        	#define WF_INT_IF_SET       IFS0SET
//        	#define WF_INT_BIT          0x00008000
//        	#define WF_INT_IPCSET       IPC3SET
//        	#define WF_INT_IPCCLR       IPC3CLR
//        	#define WF_INT_IPC_MASK     0xFF000000
//        	#define WF_INT_IPC_VALUE    0x0C000000
//        #else
//            #error Determine MRF24WB0M external interrupt
//        #endif
//
//    	#define WF_SSPBUF			(SPI2BUF)
//    	#define WF_SPISTAT			(SPI2STAT)
//    	#define WF_SPISTATbits		(SPI2STATbits)
//    	#if defined( __C30__ )
//        	#define WF_SPICON1			(SPI2CON1)
//        	#define WF_SPICON1bits		(SPI2CON1bits)
//        	#define WF_SPICON2			(SPI2CON2)
//        	#define WF_SPI_IE			(IEC2bits.SPI2IE)
//    //    	#define WF_SPI_IP			(IPC8bits.SPI2IP)
//        	#define WF_SPI_IF			(IFS2bits.SPI2IF)
//        #elif defined( __PIC32MX__ )
//        	#define WF_SPICON1			(SPI2CON)
//        	#define WF_SPICON1bits		(SPI2CONbits)
//            #define WF_SPI_IE_CLEAR     IEC1CLR
//            #define WF_SPI_IF_CLEAR     IFS1CLR
//            #define WF_SPI_INT_BITS     0x000000e0
//    		#define WF_SPI_BRG		    (SPI2BRG)
//            #define WF_MAX_SPI_FREQ     (10000000ul)	// Hz
//        #else
//            #error Determine MRF24WB0M SPI information
//        #endif
//
//	#elif defined( MRF24WB0M_IN_SPI1 ) && (defined(__32MX460F512L__) || defined(__32MX795F512L__))
//        // MRF24WB0M in SPI1 slot
//        #define WF_CS_TRIS			(TRISDbits.TRISD9)
//    	#define WF_CS_IO			(LATDbits.LATD9)
//    	#define WF_SDI_TRIS			(TRISCbits.TRISC4)
//    	#define WF_SCK_TRIS			(TRISDbits.TRISD10)
//    	#define WF_SDO_TRIS			(TRISDbits.TRISD0)
//      	#define WF_RESET_TRIS		(TRISFbits.TRISF0)
//    	#define WF_RESET_IO			(LATFbits.LATF0)
//    	#define WF_INT_TRIS		    (TRISEbits.TRISE8)  // INT1
//    	#define WF_INT_IO			(PORTEbits.RE8)
//    	#define WF_HIBERNATE_TRIS	(TRISFbits.TRISF1)
//    	#define WF_HIBERNATE_IO		(PORTFbits.RF1)
//    	#if defined( __C30__ )
//            #define WF_INT_EDGE		    (INTCON2bits.INT1EP)
//        	#define WF_INT_IE			(IEC1bits.INT1IE)
//        	#define WF_INT_IF			(IFS1bits.INT1IF)
//    	#elif defined( __PIC32MX__ )
//            #define WF_INT_EDGE		    (INTCONbits.INT1EP)
//        	#define WF_INT_IE			(IEC0bits.INT1IE)
//        	#define WF_INT_IF			(IFS0bits.INT1IF)
//        	#define WF_INT_IE_CLEAR     IEC0CLR
//        	#define WF_INT_IF_CLEAR     IFS0CLR
//        	#define WF_INT_IE_SET       IEC0SET
//        	#define WF_INT_IF_SET       IFS0SET
//        	#define WF_INT_BIT          0x00000080
//        	#define WF_INT_IPCSET       IPC1SET
//        	#define WF_INT_IPCCLR       IPC1CLR
//        	#define WF_INT_IPC_MASK     0xFF000000
//        	#define WF_INT_IPC_VALUE    0x0C000000
//        #else
//            #error Determine MRF24WB0M external interrupt
//        #endif
//
//    	#define WF_SSPBUF			(SPI1BUF)
//    	#define WF_SPISTAT			(SPI1STAT)
//    	#define WF_SPISTATbits		(SPI1STATbits)
//    	#if defined( __C30__ )
//        	#define WF_SPICON1			(SPI1CON1)
//        	#define WF_SPICON1bits		(SPI1CON1bits)
//        	#define WF_SPICON2			(SPI1CON2)
//        	#define WF_SPI_IE			(IEC0bits.SPI1IE)
//    //    	#define WF_SPI_IP			(IPC2bits.SPI1IP)
//        	#define WF_SPI_IF			(IFS0bits.SPI1IF)
//        #elif defined( __PIC32MX__ )
//        	#define WF_SPICON1			(SPI1CON)
//        	#define WF_SPICON1bits		(SPI1CONbits)
//            #define WF_SPI_IE_CLEAR     IEC0CLR
//            #define WF_SPI_IF_CLEAR     IFS0CLR
//            #define WF_SPI_INT_BITS     0x03800000
//    		#define WF_SPI_BRG		    (SPI1BRG)
//            #define WF_MAX_SPI_FREQ     (10000000ul)	// Hz
//        #else
//            #error Determine MRF24WB0M SPI information
//        #endif
//    #elif defined( MRF24WB0M_IN_SPI2 ) && (defined(__32MX460F512L__) || defined(__32MX795F512L__))
//    	#error "/RST and /CE are on RG2 and RG3 which are multiplexed with USB D+ and D-."
//    #endif

#elif defined(DSPICDEM11)
// dsPICDEM 1.1 Development Board + Ethernet PICtail airwired. There
// is no PICtail header on this development board.  The following
// airwires must be made:
// 1. dsPICDEM GND <-> PICtail GND (PICtail pin 27)
// 2. dsPICDEM Vdd <- PICtail VPIC (PICtail pin 25)
// 3. dsPICDEM RG2 -> PICtail ENC28J60 CS (PICtail pin 22)
// 4. dsPICDEM RF6 -> PICtail SCK (PICtail pin 11)
// 5. dsPICDEM RF7 <- PICtail SDI (PICtail pin 9)
// 6. dsPICDEM RF8 -> PICtail SDO (PICtail pin 7)
// 7. dsPICDEM RG3 -> PICtail 25LC256 CS (PICtail pin 20)

	#define LED0_TRIS			(TRISDbits.TRISD3)	// Ref LED4
	#define LED0_IO				(PORTDbits.RD3)
	#define LED1_TRIS			(TRISDbits.TRISD2)	// Ref LED3
	#define LED1_IO				(PORTDbits.RD2)
	#define LED2_TRIS			(TRISDbits.TRISD1)	// Ref LED2
	#define LED2_IO				(PORTDbits.RD1)
	#define LED3_TRIS			(TRISDbits.TRISD0)	// Ref LED1
	#define LED3_IO				(PORTDbits.RD0)
	#define LED4_TRIS			(TRISDbits.TRISD3)	// No LED, Remapped to Ref LED4
	#define LED4_IO				(PORTDbits.RD3)
	#define LED5_TRIS			(TRISDbits.TRISD2)	// No LED, Remapped to Ref LED3
	#define LED5_IO				(PORTDbits.RD2)
	#define LED6_TRIS			(TRISDbits.TRISD1)	// No LED, Remapped to Ref LED2
	#define LED6_IO				(PORTDbits.RD1)
	#define LED7_TRIS			(TRISDbits.TRISD0)	// No LED, Remapped to Ref LED1
	#define LED7_IO				(PORTDbits.RD0)
	#define LED_GET()			((LED3_IO<<3) | (LED2_IO<<2) | (LED1_IO<<1) | LED0_IO)
	#define LED_PUT(a)			do{BYTE vTemp = (a); LED0_IO = vTemp&0x1; LED1_IO = vTemp&0x2; LED2_IO = vTemp&0x4; LED3_IO = vTemp&0x8;} while(0)

	#define BUTTON0_TRIS		(TRISAbits.TRISA15)	// Ref SW4
	#define	BUTTON0_IO			(PORTAbits.RA15)
	#define BUTTON1_TRIS		(TRISAbits.TRISA14)	// Ref SW3
	#define	BUTTON1_IO			(PORTAbits.RA14)
	#define BUTTON2_TRIS		(TRISAbits.TRISA13)	// Ref SW2
	#define	BUTTON2_IO			(PORTAbits.RA13)
	#define BUTTON3_TRIS		(TRISAbits.TRISA12)	// Ref SW1
	#define	BUTTON3_IO			(PORTAbits.RA12)

	#define UARTTX_TRIS			(TRISFbits.TRISF3)
	#define UARTTX_IO			(PORTFbits.RF3)
	#define UARTRX_TRIS			(TRISFbits.TRISF2)
	#define UARTRX_IO			(PORTFbits.RF2)

	// ENC28J60 I/O pins
	#define ENC_CS_TRIS			(TRISGbits.TRISG2)		// User must airwire this
	#define ENC_CS_IO			(PORTGbits.RG2)
	// SPI SCK, SDI, SDO pins are automatically controlled by the
	// PIC24/dsPIC/PIC32 SPI module
	#define ENC_SPI_IF			(IFS0bits.SPI1IF)
	#define ENC_SSPBUF			(SPI1BUF)
	#define ENC_SPICON1			(SPI1CON)
	#define ENC_SPICON1bits		(SPI1CONbits)
	#define ENC_SPICON2			(SPI1BUF)				// SPI1CON2 doesn't exist, remap to unimportant register
	#define ENC_SPISTAT			(SPI1STAT)
	#define ENC_SPISTATbits		(SPI1STATbits)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISGbits.TRISG3)	// User must airwire this
	#define EEPROM_CS_IO		(PORTGbits.RG3)
	#define EEPROM_SCK_TRIS		(TRISGbits.TRISG6)
	#define EEPROM_SDI_TRIS		(TRISGbits.TRISG7)
	#define EEPROM_SDO_TRIS		(TRISGbits.TRISG8)
	#define EEPROM_SPI_IF		(IFS0bits.SPI1IF)
	#define EEPROM_SSPBUF		(SPI1BUF)
	#define EEPROM_SPICON1		(SPI1CON)
	#define EEPROM_SPICON1bits	(SPI1CONbits)
	#define EEPROM_SPICON2		(SPI1BUF)			// SPI1CON2 doesn't exist, remap to unimportant register
	#define EEPROM_SPISTAT		(SPI1STAT)
	#define EEPROM_SPISTATbits	(SPI1STATbits)

	// SI3000 codec pins
	#define CODEC_RST_TRIS		(TRISFbits.TRISF6)
	#define CODEC_RST_IO		(PORTFbits.RF6)

	// PIC18F252 LCD Controller
	#define LCDCTRL_CS_TRIS		(TRISGbits.TRISG9)
	#define LCDCTRL_CS_IO		(PORTGbits.RG9)

#elif defined(PICDEMNET2) && !defined(HI_TECH_C)
// PICDEM.net 2 (PIC18F97J60 + ENC28J60 or ENC624J600)

	// I/O pins
	#define LED0_TRIS			(TRISJbits.TRISJ0)
	#define LED0_IO				(LATJbits.LATJ0)
	#define LED1_TRIS			(TRISJbits.TRISJ1)
	#define LED1_IO				(LATJbits.LATJ1)
	#define LED2_TRIS			(TRISJbits.TRISJ2)
	#define LED2_IO				(LATJbits.LATJ2)
	#define LED3_TRIS			(TRISJbits.TRISJ3)
	#define LED3_IO				(LATJbits.LATJ3)
	#define LED4_TRIS			(TRISJbits.TRISJ4)
	#define LED4_IO				(LATJbits.LATJ4)
	#define LED5_TRIS			(TRISJbits.TRISJ5)
	#define LED5_IO				(LATJbits.LATJ5)
	#define LED6_TRIS			(TRISJbits.TRISJ6)
	#define LED6_IO				(LATJbits.LATJ6)
	#define LED7_TRIS			(TRISJbits.TRISJ7)
	#define LED7_IO				(LATJbits.LATJ7)
	#define LED_GET()			(LATJ)
	#define LED_PUT(a)			(LATJ = (a))

	#define BUTTON0_TRIS		(TRISBbits.TRISB3)
	#define	BUTTON0_IO			(PORTBbits.RB3)
	#define BUTTON1_TRIS		(TRISBbits.TRISB2)
	#define	BUTTON1_IO			(PORTBbits.RB2)
	#define BUTTON2_TRIS		(TRISBbits.TRISB1)
	#define	BUTTON2_IO			(PORTBbits.RB1)
	#define BUTTON3_TRIS		(TRISBbits.TRISB0)
	#define	BUTTON3_IO			(PORTBbits.RB0)

//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
//	#define ENC100_INTERFACE_MODE			0	// Uncomment this to use the ENC424J600/624J600 Ethernet controller
//
//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus I/O pins
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
//	#define ENC100_MDIX_IO					(LATBbits.LATB4)
//	#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
//	#define ENC100_POR_IO					(LATBbits.LATB5)
//	#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
//	#define ENC100_INT_IO					(PORTBbits.RB2)
//
//	// ENC424J600/624J600 SPI pinout
//	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
//	#define ENC100_CS_IO					(LATBbits.LATB3)
//	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
//	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
//	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
//	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
//
//	// ENC424J600/624J600 SPI SFR register selection (controls which SPI
//	// peripheral to use on PICs with multiple SPI peripherals).
////	#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
////	#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
////	#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
////	#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(PIR1bits.SSP1IF)
//	#define ENC100_SSPBUF			(SSP1BUF)
//	#define ENC100_SPISTAT			(SSP1STAT)
//	#define ENC100_SPISTATbits		(SSP1STATbits)
//	#define ENC100_SPICON1			(SSP1CON1)
//	#define ENC100_SPICON1bits		(SSP1CON1bits)
//	#define ENC100_SPICON2			(SSP1CON2)


	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISDbits.TRISD2)	// Not connected by default
	#define ENC_RST_IO			(LATDbits.LATD2)
	//#define ENC_CS_TRIS			(TRISDbits.TRISD3)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
	#define ENC_CS_IO			(LATDbits.LATD3)
	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
	#define ENC_SPI_IF			(PIR1bits.SSP1IF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

//    //----------------
//    // MRF24WB0M I/O pins
//    //----------------
//	#define WF_CS_TRIS			(TRISCbits.TRISC2)	// Uncomment this line if you wish to use the MRF24WB0M on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
//	#define WF_SDI_TRIS			(TRISCbits.TRISC4)
//	#define WF_SCK_TRIS			(TRISCbits.TRISC3)
//	#define WF_SDO_TRIS			(TRISCbits.TRISC5)
//	#define WF_RESET_TRIS		(TRISBbits.TRISB1)
//	#define WF_RESET_IO			(LATBbits.LATB1)
//	#define WF_INT_TRIS		    (TRISBbits.TRISB0)
//	#define WF_INT_IO			(PORTBbits.RB0)
//	#define WF_CS_IO			(LATCbits.LATC2)
//	#define WF_HIBERNATE_TRIS   (TRISBbits.TRISB2)
//	#define	WF_HIBERNATE_IO	    (PORTBbits.RB2)
//	#define WF_INT_EDGE		    (INTCON2bits.INTEDG0)
//	#define WF_INT_IE			(INTCONbits.INT0IE)
//	#define WF_INT_IF			(INTCONbits.INT0IF)
//	#define WF_SPI_IF			(PIR1bits.SSPIF)
//	#define WF_SSPBUF			(SSP1BUF)
//	#define WF_SPISTAT			(SSP1STAT)
//	#define WF_SPISTATbits		(SSP1STATbits)
//	#define WF_SPICON1			(SSP1CON1)
//	#define WF_SPICON1bits		(SSP1CON1bits)
//	#define WF_SPICON2			(SSP1CON2)
//	#define WF_SPI_IE			(PIE1bits.SSPIE)
//	#define WF_SPI_IP			(IPR1bits.SSPIP)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISDbits.TRISD7)
	#define EEPROM_CS_IO		(LATDbits.LATD7)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSP1IF)
	#define EEPROM_SSPBUF		(SSP1BUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
	#define LCD_RD_WR_TRIS		(TRISHbits.TRISH1)
	#define LCD_RD_WR_IO		(LATHbitsTemp.LATH1)
	#define LCD_RS_TRIS			(TRISHbits.TRISH2)
	#define LCD_RS_IO			(LATHbits.LATH2)
	#define LCD_E_TRIS			(TRISHbits.TRISH0)
	#define LCD_E_IO			(LATHbits.LATH0)

	// Serial Flash/SRAM/UART PICtail
//	#define SPIRAM_CS_TRIS			(TRISBbits.TRISB5)
//	#define SPIRAM_CS_IO			(LATBbits.LATB5)
//	#define SPIRAM_SCK_TRIS			(TRISCbits.TRISC3)
//	#define SPIRAM_SDI_TRIS			(TRISCbits.TRISC4)
//	#define SPIRAM_SDO_TRIS			(TRISCbits.TRISC5)
//	#define SPIRAM_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIRAM_SSPBUF			(SSP1BUF)
//	#define SPIRAM_SPICON1			(SSP1CON1)
//	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
//	#define SPIRAM_SPICON2			(SSP1CON2)
//	#define SPIRAM_SPISTAT			(SSP1STAT)
//	#define SPIRAM_SPISTATbits		(SSP1STATbits)
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION,
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB4)
//	#define SPIFLASH_CS_IO			(LATBbits.LATB4)
//	#define SPIFLASH_SCK_TRIS		(TRISCbits.TRISC3)
//	#define SPIFLASH_SDI_TRIS		(TRISCbits.TRISC4)
//	#define SPIFLASH_SDI_IO			(PORTCbits.RC4)
//	#define SPIFLASH_SDO_TRIS		(TRISCbits.TRISC5)
//	#define SPIFLASH_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIFLASH_SSPBUF			(SSP1BUF)
//	#define SPIFLASH_SPICON1		(SSP1CON1)
//	#define SPIFLASH_SPICON1bits	(SSP1CON1bits)
//	#define SPIFLASH_SPICON2		(SSP1CON2)
//	#define SPIFLASH_SPISTAT		(SSP1STAT)
//	#define SPIFLASH_SPISTATbits	(SSP1STATbits)

#elif defined(PICDEMNET2) && defined(HI_TECH_C)
// PICDEM.net 2 (PIC18F97J60 + ENC28J60) + HI-TECH PICC-18 compiler
// Optionally may use Fast 100Mbps Ethernet PICtail Plus (ENC624J600)
	// I/O pins
	#define LED0_TRIS			(TRISJ0)
	#define LED0_IO				(LATJ0)
	#define LED1_TRIS			(TRISJ1)
	#define LED1_IO				(LATJ1)
	#define LED2_TRIS			(TRISJ2)
	#define LED2_IO				(LATJ2)
	#define LED3_TRIS			(TRISJ3)
	#define LED3_IO				(LATJ3)
	#define LED4_TRIS			(TRISJ4)
	#define LED4_IO				(LATJ4)
	#define LED5_TRIS			(TRISJ5)
	#define LED5_IO				(LATJ5)
	#define LED6_TRIS			(TRISJ6)
	#define LED6_IO				(LATJ6)
	#define LED7_TRIS			(TRISJ7)
	#define LED7_IO				(LATJ7)
	#define LED_GET()			(LATJ)
	#define LED_PUT(a)			(LATJ = (a))

	#define BUTTON0_TRIS		(TRISB3)
	#define	BUTTON0_IO			(RB3)
	#define BUTTON1_TRIS		(TRISB2)
	#define	BUTTON1_IO			(RB2)
	#define BUTTON2_TRIS		(TRISB1)
	#define	BUTTON2_IO			(RB1)
	#define BUTTON3_TRIS		(TRISB0)
	#define	BUTTON3_IO			(RB0)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISD2)	// Not connected by default
	#define ENC_RST_IO			(LATD2)
	//#define ENC_CS_TRIS			(TRISD3)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
	#define ENC_CS_IO			(LATD3)
	#define ENC_SCK_TRIS		(TRISC3)
	#define ENC_SDI_TRIS		(TRISC4)
	#define ENC_SDO_TRIS		(TRISC5)
	#define ENC_SPI_IF			(SSPIF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
//	#define ENC100_INTERFACE_MODE			0	// Uncomment this to use the ENC424J600/624J600 Ethernet controller
//
//	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus I/O pins
//	#define ENC100_MDIX_TRIS				(TRISB4)
//	#define ENC100_MDIX_IO					(LATB4)
//	#define ENC100_POR_TRIS					(TRISB5)
//	#define ENC100_POR_IO					(LATB5)
//	#define ENC100_INT_TRIS					(TRISB2)
//	#define ENC100_INT_IO					(RB2)
//
//	// ENC424J600/624J600 SPI pinout
//	#define ENC100_CS_TRIS					(TRISB3)
//	#define ENC100_CS_IO					(LATB3)
//	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//	#define ENC100_SO_WR_B0SEL_EN_IO		(RC4)
//	#define ENC100_SI_RD_RW_TRIS			(TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//	#define ENC100_SI_RD_RW_IO				(LATC5)
//	#define ENC100_SCK_AL_TRIS				(TRISC3)
//	#define ENC100_SCK_AL_IO				(RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
//
//	// ENC424J600/624J600 SPI SFR register selection (controls which SPI
//	// peripheral to use on PICs with multiple SPI peripherals).
////	#define ENC100_ISR_ENABLE		(INT2IE)
////	#define ENC100_ISR_FLAG			(INT2IF)
////	#define ENC100_ISR_POLARITY		(INTEDG2)
////	#define ENC100_ISR_PRIORITY		(INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(PIR1bits.SSP1IF)
//	#define ENC100_SSPBUF			(SSP1BUF)
//	#define ENC100_SPISTAT			(SSP1STAT)
//	#define ENC100_SPISTATbits		(SSP1STATbits)
//	#define ENC100_SPICON1			(SSP1CON1)
//	#define ENC100_SPICON1bits		(SSP1CON1bits)
//	#define ENC100_SPICON2			(SSP1CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISD7)
	#define EEPROM_CS_IO		(LATD7)
	#define EEPROM_SCK_TRIS		(TRISC3)
	#define EEPROM_SDI_TRIS		(TRISC4)
	#define EEPROM_SDO_TRIS		(TRISC5)
	#define EEPROM_SPI_IF		(SSP1IF)
	#define EEPROM_SSPBUF		(SSP1BUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
	#define LCD_RD_WR_TRIS		(TRISH1)
	#define LCD_RD_WR_IO		(LATH1)
	#define LCD_RS_TRIS			(TRISH2)
	#define LCD_RS_IO			(LATH2)
	#define LCD_E_TRIS			(TRISH0)
	#define LCD_E_IO			(LATH0)

	// Serial Flash/SRAM/UART PICtail
//	#define SPIRAM_CS_TRIS			(TRISB5)
//	#define SPIRAM_CS_IO			(LATB5)
//	#define SPIRAM_SCK_TRIS			(TRISC3)
//	#define SPIRAM_SDI_TRIS			(TRISC4)
//	#define SPIRAM_SDO_TRIS			(TRISC5)
//	#define SPIRAM_SPI_IF			(SSPIF)
//	#define SPIRAM_SSPBUF			(SSP1BUF)
//	#define SPIRAM_SPICON1			(SSP1CON1)
//	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
//	#define SPIRAM_SPICON2			(SSP1CON2)
//	#define SPIRAM_SPISTAT			(SSP1STAT)
//	#define SPIRAM_SPISTATbits		(SSP1STATbits)
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION,
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISB4)
//	#define SPIFLASH_CS_IO			(LATB4)
//	#define SPIFLASH_SCK_TRIS		(TRISC3)
//	#define SPIFLASH_SDI_TRIS		(TRISC4)
//	#define SPIFLASH_SDI_IO			(RC4)
//	#define SPIFLASH_SDO_TRIS		(TRISC5)
//	#define SPIFLASH_SPI_IF			(SSPIF)
//	#define SPIFLASH_SSPBUF			(SSP1BUF)
//	#define SPIFLASH_SPICON1		(SSP1CON1)
//	#define SPIFLASH_SPICON1bits	(SSP1CON1bits)
//	#define SPIFLASH_SPICON2		(SSP1CON2)
//	#define SPIFLASH_SPISTAT		(SSP1STAT)
//	#define SPIFLASH_SPISTATbits	(SSP1STATbits)

	// Register name fixup for certain processors
	#define SPBRGH				SPBRGH1
	#define TXSTAbits			TXSTA1bits
	#define RCSTAbits			RCSTA1bits
	#define BAUDCONbits			BAUDCON1bits
	#define GO					GODONE

#elif defined(INTERNET_RADIO) // Internet Radio board
// Internet Radio board (05-60142)
	// I/O pins
	#define LED0_TRIS			(TRISCbits.TRISC2)
	#define LED0_IO				(LATCbits.LATC2)
	#define LED1_TRIS			(PRODL)				// No LED1 on this board
	#define LED1_IO				(PRODL)
	#define LED2_TRIS			(PRODL)				// No LED2 on this board
	#define LED2_IO				(PRODL)
	#define LED3_TRIS			(PRODL)				// No LED3 on this board
	#define LED3_IO				(PRODL)
	#define LED4_TRIS			(PRODL)				// No LED4 on this board
	#define LED4_IO				(PRODL)
	#define LED5_TRIS			(PRODL)				// No LED5 on this board
	#define LED5_IO				(PRODL)
	#define LED6_TRIS			(PRODL)				// No LED6 on this board
	#define LED6_IO				(PRODL)
	#define LED7_TRIS			(PRODL)				// No LED7 on this board
	#define LED7_IO				(PRODL)
	#define LED_GET()			(LED0_IO)
	#define LED_PUT(a)			(LED0_IO = (a))

	#define BUTTON0_TRIS		(TRISBbits.TRISB5)
	#define	BUTTON0_IO			(PORTBbits.RB5)
	#define BUTTON1_TRIS		(TRISFbits.TRISF1)
	#define	BUTTON1_IO			(PORTFbits.RF1)
	#define BUTTON2_TRIS		(TRISBbits.TRISB4)
	#define	BUTTON2_IO			(PORTBbits.RB4)
	#define BUTTON3_TRIS		(PRODL)				// No BUTTON3 on this board
	#define	BUTTON3_IO			(PRODL)

	// Serial SRAM
	#define SPIRAM_CS_TRIS			(TRISEbits.TRISE4)
	#define SPIRAM_CS_IO			(LATEbits.LATE4)
	#define SPIRAM_SCK_TRIS			(TRISCbits.TRISC3)
	#define SPIRAM_SDI_TRIS			(TRISCbits.TRISC4)
	#define SPIRAM_SDO_TRIS			(TRISCbits.TRISC5)
	#define SPIRAM_SPI_IF			(PIR1bits.SSPIF)
	#define SPIRAM_SSPBUF			(SSP1BUF)
	#define SPIRAM_SPICON1			(SSP1CON1)
	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
	#define SPIRAM_SPICON2			(SSP1CON2)
	#define SPIRAM_SPISTAT			(SSP1STAT)
	#define SPIRAM_SPISTATbits		(SSP1STATbits)
	#define SPIRAM2_CS_TRIS			(TRISEbits.TRISE5)
	#define SPIRAM2_CS_IO			(LATEbits.LATE5)
	#define SPIRAM2_SCK_TRIS		(TRISCbits.TRISC3)
	#define SPIRAM2_SDI_TRIS		(TRISCbits.TRISC4)
	#define SPIRAM2_SDO_TRIS		(TRISCbits.TRISC5)
	#define SPIRAM2_SPI_IF			(PIR1bits.SSPIF)
	#define SPIRAM2_SSPBUF			(SSP1BUF)
	#define SPIRAM2_SPICON1			(SSP1CON1)
	#define SPIRAM2_SPICON1bits		(SSP1CON1bits)
	#define SPIRAM2_SPICON2			(SSP1CON2)
	#define SPIRAM2_SPISTAT			(SSP1STAT)
	#define SPIRAM2_SPISTATbits		(SSP1STATbits)

    // VLSI VS1011/VS1053 audio encoder/decoder and DAC
	#define MP3_DREQ_TRIS			(TRISBbits.TRISB0)	// Data Request
	#define MP3_DREQ_IO 			(PORTBbits.RB0)
	#define MP3_XRESET_TRIS			(TRISDbits.TRISD0)	// Reset, active low
	#define MP3_XRESET_IO			(LATDbits.LATD0)
	#define MP3_XDCS_TRIS			(TRISBbits.TRISB1)	// Data Chip Select
	#define MP3_XDCS_IO				(LATBbits.LATB1)
	#define MP3_XCS_TRIS			(TRISBbits.TRISB2)	// Control Chip Select
	#define MP3_XCS_IO				(LATBbits.LATB2)
	#define MP3_SCK_TRIS			(TRISCbits.TRISC3)
	#define MP3_SDI_TRIS			(TRISCbits.TRISC4)
	#define MP3_SDO_TRIS			(TRISCbits.TRISC5)
	#define MP3_SPI_IF				(PIR1bits.SSP1IF)
	#define MP3_SSPBUF				(SSP1BUF)
	#define MP3_SPICON1				(SSP1CON1)
	#define MP3_SPICON1bits			(SSP1CON1bits)
	#define MP3_SPICON2				(SSP1CON2)
	#define MP3_SPISTAT				(SSP1STAT)
	#define MP3_SPISTATbits			(SSP1STATbits)

    // OLED Display
    #define oledWR                  (PORTAbits.RA3)
    #define oledWR_TRIS             (TRISAbits.TRISA3)
    #define oledRD                  (PORTAbits.RA4)
    #define oledRD_TRIS             (TRISAbits.TRISA4)
    #define oledCS                  (PORTAbits.RA5)
    #define oledCS_TRIS             (TRISAbits.TRISA5)
    #define oledRESET               (PORTDbits.RD1)
    #define oledRESET_TRIS          (TRISDbits.TRISD1)
    #define oledD_C                 (PORTGbits.RG4)
    #define oledD_C_TRIS            (TRISGbits.TRISG4)


#elif defined(PIC32_GP_SK_DM320001) || defined(PIC32_USB_DM320003_1) || defined(PIC32_USB_SK_DM320003_2)
// PIC32 (General Purpose) Starter Kit (02-02002) with PIC32MX360F512L processor,
// PIC32 USB Starter Board (02-02030) with PIC32MX460F512L processor, or
// PIC32 USB Starter Kit II (02-02148) with PIC32MX795F512L processor +
// PIC32 I/O Expansion Board (05-02029) +
// Ethernet PICtail Plus, Fast 100Mbps Ethernet PICtail Plus, or
// MRF24WB0M 802.11 WiFi PICtail Plus

	// Specify which SPI to use for the ENC28J60 or ENC624J600.  SPI1 is
	// the topmost slot with pin 1 on it.  SPI2 is the middle slot
	// starting on pin 33.
	#define ENC_IN_SPI1
	//#define ENC_IN_SPI2

	// Note that SPI1 cannot be used when using the PIC32 USB Starter
	// Board or PIC32 USB Starter Kit II due to the USB peripheral pins
	// mapping on top of the ordinary SPI1 pinout.
	#if defined(ENC_IN_SPI1) && (defined(__32MX460F512L__) || defined(__32MX795F512L__))
		#undef ENC_IN_SPI1
		#define ENC_IN_SPI2
	#endif


    // Hardware mappings
    #define LED0_TRIS			(TRISDbits.TRISD0)	// Ref LED1
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)	// Ref LED2
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)	// Ref LED3
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(LED2_TRIS)			// No such LED
	#define LED3_IO				(LATDbits.LATD6)
	#define LED4_TRIS			(LED2_TRIS)			// No such LED
	#define LED4_IO				(LATDbits.LATD6)
	#define LED5_TRIS			(LED2_TRIS)			// No such LED
	#define LED5_IO				(LATDbits.LATD6)
	#define LED6_TRIS			(LED2_TRIS)			// No such LED
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(LED2_TRIS)			// No such LED
	#define LED7_IO				(LATDbits.LATD6)

	#define LED_GET()			((BYTE)LATD & 0x07)
	#define LED_PUT(a)			do{LATD = (LATD & 0xFFF8) | ((a)&0x07);}while(0)

    #define BUTTON0_TRIS		(TRISDbits.TRISD6)	// Ref SW1
	#define	BUTTON0_IO			(PORTDbits.RD6)
	#define BUTTON1_TRIS		(TRISDbits.TRISD7)	// Ref SW2
	#define	BUTTON1_IO			(PORTDbits.RD7)
	#define BUTTON2_TRIS		(TRISDbits.TRISD13)	// Ref SW3
	#define	BUTTON2_IO			(PORTDbits.RD13)
	#define BUTTON3_TRIS		(TRISDbits.TRISD13)	// No BUTTON3 on this board
	#define	BUTTON3_IO			(1)

	// UART configuration (not too important since we don't have a UART
	// connector attached normally, but needed to compile if the STACK_USE_UART
	// or STACK_USE_UART2TCP_BRIDGE features are enabled.
	#define UARTTX_TRIS			(TRISFbits.TRISF3)
	#define UARTRX_TRIS			(TRISFbits.TRISF2)


	// ENC28J60 I/O pins
	#if defined ENC_IN_SPI1
		#define ENC_CS_TRIS			(TRISDbits.TRISD14)	// Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
		#define ENC_CS_IO			(PORTDbits.RD14)
		//#define ENC_RST_TRIS		(TRISDbits.TRISD15)	// Not connected by default.  It is okay to leave this pin completely unconnected, in which case this macro should simply be left undefined.
		//#define ENC_RST_IO		(PORTDbits.RD15)

		// SPI SCK, SDI, SDO pins are automatically controlled by the
		#define ENC_SPI_IF			(IFS0bits.SPI1RXIF)
		#define ENC_SSPBUF			(SPI1BUF)
		#define ENC_SPICON1			(SPI1CON)
		#define ENC_SPICON1bits		(SPI1CONbits)
		#define ENC_SPIBRG			(SPI1BRG)
		#define ENC_SPISTATbits		(SPI1STATbits)
	#elif defined ENC_IN_SPI2
		#define ENC_CS_TRIS			(TRISFbits.TRISF12) // Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
		#define ENC_CS_IO			(PORTFbits.RF12)
		//#define ENC_RST_TRIS		(TRISFbits.TRISF13)	// Not connected by default
		//#define ENC_RST_IO		(PORTFbits.RF13)

		// SPI SCK, SDI, SDO pins are automatically controlled by the
		// PIC32 SPI module
		#define ENC_SPI_IF			(IFS1bits.SPI2RXIF)
		#define ENC_SSPBUF			(SPI2BUF)
		#define ENC_SPICON1			(SPI2CON)
		#define ENC_SPISTATbits		(SPI2STATbits)
		#define ENC_SPICON1bits		(SPI2CONbits)
		#define ENC_SPIBRG			(SPI2BRG)
	#endif



//	// ENC624J600 Interface Configuration
//	// Comment out ENC100_INTERFACE_MODE if you don't have an ENC624J600 or
//	// ENC424J600.  Otherwise, choose the correct setting for the interface you
//	// are using.  Legal values are:
//	//  - Commented out: No ENC424J600/624J600 present or used.  All other
//	//                   ENC100_* macros are ignored.
//	//	- 0: SPI mode using CS, SCK, SI, and SO pins
//	//  - 1: 8-bit demultiplexed PSP Mode 1 with RD and WR pins
//	//  - 2: 8-bit demultiplexed PSP Mode 2 with R/Wbar and EN pins
//	//  - 3: 16-bit demultiplexed PSP Mode 3 with RD, WRL, and WRH pins
//	//  - 4: 16-bit demultiplexed PSP Mode 4 with R/Wbar, B0SEL, and B1SEL pins
//	//  - 5: 8-bit multiplexed PSP Mode 5 with RD and WR pins
//	//  - 6: 8-bit multiplexed PSP Mode 6 with R/Wbar and EN pins
//	//  - 9: 16-bit multiplexed PSP Mode 9 with AL, RD, WRL, and WRH pins
//	//  - 10: 16-bit multiplexed PSP Mode 10 with AL, R/Wbar, B0SEL, and B1SEL
//	//        pins
//	#define ENC100_INTERFACE_MODE			0
//
//	// If using a parallel interface, direct RAM addressing can be used (if all
//	// addresses wires are connected), or a reduced number of pins can be used
//	// for indirect addressing.  If using an SPI interface or PSP Mode 9 or 10
//	// (multiplexed 16-bit modes), which require all address lines to always be
//	// connected, then this option is ignored. Comment out or uncomment this
//	// macro to match your hardware connections.
//	#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
//
//	// ENC424J600/624J600 parallel indirect address remapping macro function.
//	// This section translates SFR and RAM addresses presented to the
//	// ReadMemory() and WriteMemory() APIs in ENCX24J600.c to the actual
//	// addresses that must be presented on the parallel interface.  This macro
//	// must be modified to match your hardware if you are using an indirect PSP
//	// addressing mode (ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING is defined) and
//	// have some of your address lines tied off to Vdd.  If you are using the
//	// SPI interface, then this section can be ignored or deleted.
//	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) // 8-bit PSP
//		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
//	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) // 16-bit PSP
//		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		(a)
//	#endif
//
//	// Auto-crossover pins on Fast 100Mbps Ethernet PICtail/PICtail Plus.  If
//	// your circuit doesn't have such a feature, delete these two defines.
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB3)
//	#define ENC100_MDIX_IO					(LATBbits.LATB3)
//
//	// ENC624J600 I/O control and status pins
//	// If a pin is not required for your selected ENC100_INTERFACE_MODE
//	// interface selection (ex: WRH/B1SEL for PSP modes 1, 2, 5, and 6), then
//	// you can ignore, delete, or put anything for the pin definition.  Also,
//	// the INT and POR pins are entirely optional.  If not connected, comment
//	// them out.
//	#define ENC100_INT_TRIS					(TRISEbits.TRISE9)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
//	#define ENC100_INT_IO					(PORTEbits.RE9)
//	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
//		// PSP control signal pinout
//		#define ENC100_CS_TRIS					(TRISAbits.TRISA5)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
//		#define ENC100_CS_IO					(LATAbits.LATA5)
//		#define ENC100_POR_TRIS					(TRISCbits.TRISC1)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//		#define ENC100_POR_IO					(LATCbits.LATC1)
//		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISDbits.TRISD4)
//		#define ENC100_SO_WR_B0SEL_EN_IO		(LATDbits.LATD4)
//		#define ENC100_SI_RD_RW_TRIS			(TRISDbits.TRISD5)
//		#define ENC100_SI_RD_RW_IO				(LATDbits.LATD5)
//		#define ENC100_SCK_AL_TRIS				(TRISBbits.TRISB15)
//		#define ENC100_SCK_AL_IO				(LATBbits.LATB15)
//	#else
//		// SPI pinout
//    	#if defined ENC_IN_SPI1
//        	#define ENC100_CS_TRIS					(TRISDbits.TRISD14)	// CS is mandatory when using the SPI interface
//        	#define ENC100_CS_IO					(LATDbits.LATD14)
//        	#define ENC100_POR_TRIS					(TRISDbits.TRISD15)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//        	#define ENC100_POR_IO					(LATDbits.LATD15)
//        	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISFbits.TRISF7)	// SO is ENCX24J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//        	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTFbits.RF7)
//        	#define ENC100_SI_RD_RW_TRIS			(TRISFbits.TRISF8)	// SI is ENCX24J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//        	#define ENC100_SI_RD_RW_IO				(LATFbits.LATF8)
//        	#define ENC100_SCK_AL_TRIS				(TRISFbits.TRISF6)
//        #elif defined ENC_IN_SPI2
//            #define ENC100_CS_TRIS					(TRISFbits.TRISF12)
//    		#define ENC100_CS_IO					(LATFbits.LATF12)
//    		#define ENC100_POR_TRIS					(TRISFbits.TRISF13)
//    		#define ENC100_POR_IO					(LATFbits.LATF13)
//    		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISGbits.TRISG7)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//    		#define ENC100_SO_WR_B0SEL_EN_IO		(PORTGbits.RG7)
//    		#define ENC100_SI_RD_RW_TRIS			(TRISGbits.TRISG8)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//    		#define ENC100_SI_RD_RW_IO				(LATGbits.LATG8)
//    		#define ENC100_SCK_AL_TRIS				(TRISGbits.TRISG6)
//    		#define ENC100_SCK_AL_IO				(PORTGbits.RG6)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
//        #endif
//    #endif
//
//
//	// ENC624J600 SPI SFR register selection (controls which SPI peripheral to
//	// use on PICs with multiple SPI peripherals).  If a parallel interface is
//	// used (ENC100_INTERFACE_MODE is >= 1), then the SPI is not used and this
//	// section can be ignored or deleted.
//	#if defined ENC_IN_SPI1
//		#define ENC100_ISR_ENABLE		(IEC0bits.INT2IE)
//		#define ENC100_ISR_FLAG			(IFS0bits.INT2IF)
//		#define ENC100_ISR_POLARITY		(INTCONbits.INT2EP)
//		#define ENC100_ISR_PRIORITY		(IPC2bits.INT2IP)
//		#define ENC100_SPI_ENABLE		(ENC100_SPICON1bits.ON)
//		#define ENC100_SPI_IF			(IFS0bits.SPI1RXIF)
//		#define ENC100_SSPBUF			(SPI1BUF)
//		#define ENC100_SPICON1			(SPI1CON)
//		#define ENC100_SPISTATbits		(SPI1STATbits)
//		#define ENC100_SPICON1bits		(SPI1CONbits)
//		#define ENC100_SPIBRG			(SPI1BRG)
//	#elif defined ENC_IN_SPI2
//		#define ENC100_ISR_ENABLE		(IEC0bits.INT4IE)
//		#define ENC100_ISR_FLAG			(IFS0bits.INT4IF)
//		#define ENC100_ISR_POLARITY		(INTCONbits.INT4EP)
//		#define ENC100_ISR_PRIORITY		(IPC2bits.INT4IP)
//		#define ENC100_SPI_ENABLE		(ENC100_SPICON1bits.ON)
//		#define ENC100_SPI_IF			(IFS1bits.SPI2RXIF)
//		#define ENC100_SSPBUF			(SPI2BUF)
//		#define ENC100_SPICON1			(SPI2CON)
//		#define ENC100_SPISTATbits		(SPI2STATbits)
//		#define ENC100_SPICON1bits		(SPI2CONbits)
//		#define ENC100_SPIBRG			(SPI2BRG)
//	#endif


//	//----------------------------
//	// MRF24WB0M WiFi I/O pins
//	//----------------------------
//	// If you have a MRF24WB0M WiFi PICtail, you must uncomment one of
//	// these two lines to use it.  SPI1 is the top-most slot while SPI2 corresponds to
//	// insertion of the PICtail into the middle of the side edge connector slot.
//
//	#define MRF24WB0M_IN_SPI1
//	//#define MRF24WB0M_IN_SPI2
//	#if defined( MRF24WB0M_IN_SPI1 )
//		// MRF24WB0M in SPI1 slot
//		#define WF_CS_TRIS			(TRISBbits.TRISB2)
//    	#define WF_CS_IO			(LATBbits.LATB2)
//    	#define WF_SDI_TRIS			(TRISFbits.TRISF7)
//    	#define WF_SCK_TRIS			(TRISFbits.TRISF6)
//    	#define WF_SDO_TRIS			(TRISFbits.TRISF8)
//      	#define WF_RESET_TRIS		(TRISFbits.TRISF0)
//    	#define WF_RESET_IO			(LATFbits.LATF0)
//    	#define WF_INT_TRIS		    (TRISEbits.TRISE8)  // INT1
//    	#define WF_INT_IO			(PORTEbits.RE8)
//    	#define WF_HIBERNATE_TRIS   (TRISFbits.TRISF1)
//    	#define	WF_HIBERNATE_IO		(PORTFbits.RF1)
//       #define WF_INT_EDGE		    (INTCONbits.INT1EP)
//       #define WF_INT_IE			(IEC0bits.INT1IE)
//       #define WF_INT_IF			(IFS0bits.INT1IF)
//       #define WF_INT_IE_CLEAR      IEC0CLR
//       #define WF_INT_IF_CLEAR      IFS0CLR
//       #define WF_INT_IE_SET        IEC0SET
//       #define WF_INT_IF_SET        IFS0SET
//       #define WF_INT_BIT           0x00000080
//       #define WF_INT_IPCSET        IPC1SET
//       #define WF_INT_IPCCLR        IPC1CLR
//       #define WF_INT_IPC_MASK      0xFF000000
//       #define WF_INT_IPC_VALUE     0x0C000000
//
//    	#define WF_SSPBUF			(SPI1BUF)
//    	#define WF_SPISTAT			(SPI1STAT)
//    	#define WF_SPISTATbits		(SPI1STATbits)
//       #define WF_SPICON1			(SPI1CON)
//       #define WF_SPICON1bits		(SPI1CONbits)
//       #define WF_SPI_IE_CLEAR     IEC0CLR
//       #define WF_SPI_IF_CLEAR     IFS0CLR
//       #define WF_SPI_INT_BITS     0x03800000
//    	#define WF_SPI_BRG		    (SPI1BRG)
//       #define WF_MAX_SPI_FREQ     (10000000ul)	// Hz
//
//	#elif defined( MRF24WB0M_IN_SPI2 )
//       // MRF24WB0M in SPI2 slot
//       #define WF_CS_TRIS			(TRISGbits.TRISG9)
//    	#define WF_CS_IO			(LATGbits.LATG9)
//    	#define WF_SDI_TRIS			(TRISGbits.TRISG7)
//    	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
//    	#define WF_SDO_TRIS			(TRISGbits.TRISG8)
//      	#define WF_RESET_TRIS		(TRISGbits.TRISG0)
//    	#define WF_RESET_IO			(LATGbits.LATG0)
//    	#define WF_INT_TRIS		    (TRISAbits.TRISA14) // INT3
//    	#define WF_INT_IO			(PORTAbits.RA14)
//    	#define WF_HIBERNATE_TRIS   (TRISGbits.TRISG1)
//    	#define	WF_HIBERNATE_IO	    (PORTGbits.RG1)
//       #define WF_INT_EDGE		    (INTCONbits.INT3EP)
//       #define WF_INT_IE			(IEC0bits.INT3IE)
//       #define WF_INT_IF			(IFS0bits.INT3IF)
//       #define WF_INT_IE_CLEAR      IEC0CLR
//       #define WF_INT_IF_CLEAR      IFS0CLR
//       #define WF_INT_IE_SET        IEC0SET
//       #define WF_INT_IF_SET        IFS0SET
//       #define WF_INT_BIT           0x00008000
//       #define WF_INT_IPCSET        IPC3SET
//       #define WF_INT_IPCCLR        IPC3CLR
//       #define WF_INT_IPC_MASK      0xFF000000
//       #define WF_INT_IPC_VALUE     0x0C000000
//
//    	#define WF_SSPBUF			(SPI2BUF)
//    	#define WF_SPISTAT			(SPI2STAT)
//    	#define WF_SPISTATbits		(SPI2STATbits)
//       #define WF_SPICON1			(SPI2CON)
//       #define WF_SPICON1bits		(SPI2CONbits)
//       #define WF_SPI_IE_CLEAR     IEC1CLR
//       #define WF_SPI_IF_CLEAR     IFS1CLR
//       #define WF_SPI_INT_BITS     0x000000e0
//    	#define WF_SPI_BRG		    (SPI2BRG)
//       #define WF_MAX_SPI_FREQ     (10000000ul)	// Hz
//	#endif


#elif defined(PIC32_ENET_SK_DM320004)
// PIC32 Ethernet Starter Kit (04-02146) with PIC32MX795F512L processor and National DP83848 10/100 PHY
	// External SMSC PHY configuration
	#define	PHY_RMII				// external PHY runs in RMII mode
	#define	PHY_CONFIG_ALTERNATE	// alternate configuration used
	#define	PHY_ADDRESS			0x1	// the address of the National DP83848 PHY

    // Hardware mappings
    #define LED0_TRIS			(TRISDbits.TRISD0)	// Ref LED1
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)	// Ref LED2
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)	// Ref LED3
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(LED2_TRIS)			// No such LED
	#define LED3_IO				(LATDbits.LATD6)
	#define LED4_TRIS			(LED2_TRIS)			// No such LED
	#define LED4_IO				(LATDbits.LATD6)
	#define LED5_TRIS			(LED2_TRIS)			// No such LED
	#define LED5_IO				(LATDbits.LATD6)
	#define LED6_TRIS			(LED2_TRIS)			// No such LED
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(LED2_TRIS)			// No such LED
	#define LED7_IO				(LATDbits.LATD6)

	#define LED_GET()			((BYTE)LATD & 0x07)
	#define LED_PUT(a)			do{LATD = (LATD & 0xFFF8) | ((a)&0x07);}while(0)

    #define BUTTON0_TRIS		(TRISDbits.TRISD6)	// Ref SW1
	#define	BUTTON0_IO			(PORTDbits.RD6)
	#define BUTTON1_TRIS		(TRISDbits.TRISD7)	// Ref SW2
	#define	BUTTON1_IO			(PORTDbits.RD7)
	#define BUTTON2_TRIS		(TRISDbits.TRISD13)	// Ref SW3
	#define	BUTTON2_IO			(PORTDbits.RD13)
	#define BUTTON3_TRIS		(TRISDbits.TRISD13)	// No BUTTON3 on this board
	#define	BUTTON3_IO			(1)


	// Note, it is not possible to use a MRF24WB0M 802.11 WiFi PICtail
	// Plus card with this starter kit.  The required interrupt signal, among
	// possibly other I/O pins aren't available on the Starter Kit board.

#elif defined(PIC24FJ256DA210_DEV_BOARD)
// PIC24FJ256DA210 Development Board (Graphics) +
//   Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail Plus or MRF24WB0M WiFi PICtail Plus

	#define LED0_TRIS			(TRISAbits.TRISA7)		// Ref D4: Jumper JP11 must have a shunt shorting pins 1 and 2 together
	#define LED0_IO				(LATAbits.LATA7)
	#define LED1_TRIS			(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register.  D3 is the natural choice for LED0, but the D3 pin (RB5) is multiplexed with R3 potentiometer and MDIX signal on Fast 100Mbps Ethernet PICtail Plus, so it cannot be used
	#define LED1_IO				(((BYTE*)&NVMKEY)[1])
	#define LED2_TRIS			(TRISEbits.TRISE9)		// Ref D2.  NOTE: When using the PSP interface, this RE9 signal also controls the POR (SHDN) signal on the Fast 100Mbps Ethernet PICtail Plus.
	#define LED2_IO				(LATEbits.LATE9)
	#define LED3_TRIS			(TRISGbits.TRISG8)		// Ref D1.  NOTE: When using the PSP interface, this RG8 signal also controls the CS signal on the Fast 100Mbps Ethernet PICtail Plus.
	#define LED3_IO				(LATGbits.LATG8)
	#define LED4_TRIS			(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register
	#define LED4_IO				(((BYTE*)&NVMKEY)[1])
	#define LED5_TRIS			(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register
	#define LED5_IO				(((BYTE*)&NVMKEY)[1])
	#define LED6_TRIS			(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register
	#define LED6_IO				(((BYTE*)&NVMKEY)[1])
	#define LED7_TRIS			(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register
	#define LED7_IO				(((BYTE*)&NVMKEY)[1])
	#define LED_GET()			((LATGbits.LATG8<<3) | (LATEbits.LATE9<<2) | LATAbits.LATA7)
	#define LED_PUT(a)			do{BYTE vTemp = (a); LED0_IO = vTemp&0x1; LED2_IO = vTemp&0x4; LED3_IO = vTemp&0x8;} while(0)


	#define BUTTON0_TRIS		(((BYTE*)&NVMKEY)[1])	// Ref S3: NOTE: This pin is multiplexed with D3 and cannot be used simulatneously.  Therefore, we will pretend there is no such button.
	#define	BUTTON0_IO			(1)
	#define BUTTON1_TRIS		(((BYTE*)&NVMKEY)[1])	// Ref S2: NOTE: This pin is multiplexed with D2 and cannot be used simulatneously.  Therefore, we will pretend there is no such button.
	#define	BUTTON1_IO			(1)
	#define BUTTON2_TRIS		(((BYTE*)&NVMKEY)[1])	// Ref S1: NOTE: This pin is multiplexed with D1 and cannot be used simulatneously.  Therefore, we will pretend there is no such button.
	#define	BUTTON2_IO			(1)
	#define BUTTON3_TRIS		(((BYTE*)&NVMKEY)[1])	// No such button
	#define	BUTTON3_IO			(1)


	#define UARTTX_TRIS			(TRISFbits.TRISF3)
	#define UARTTX_IO			(PORTFbits.RF3)
	#define UARTRX_TRIS			(TRISDbits.TRISD0)
	#define UARTRX_IO			(PORTDbits.RD0)


	// NOTE: You must also set SPIFlash.h file to define SPI_FLASH_SST, define
	//       SPI_FLASH_SECTOR_SIZE as 4096, and define SPI_FLASH_PAGE_SIZE as 0.
	//       Jumper JP23 must have a shunt shorting pins 2-3 (not the default).
	// SST SST25VF016B (16Mbit/2Mbyte)
	#define SPIFLASH_CS_TRIS		(TRISAbits.TRISA14)
	#define SPIFLASH_CS_IO			(LATAbits.LATA14)
	#define SPIFLASH_SCK_TRIS		(TRISDbits.TRISD8)
	#define SPIFLASH_SDI_TRIS		(TRISBbits.TRISB0)
	#define SPIFLASH_SDI_IO			(PORTBbits.RB0)
	#define SPIFLASH_SDO_TRIS		(TRISBbits.TRISB1)
	#define SPIFLASH_SPI_IF			(IFS0bits.SPI1IF)
	#define SPIFLASH_SSPBUF			(SPI1BUF)
	#define SPIFLASH_SPICON1		(SPI1CON1)
	#define SPIFLASH_SPICON1bits	(SPI1CON1bits)
	#define SPIFLASH_SPICON2		(SPI1CON2)
	#define SPIFLASH_SPISTAT		(SPI1STAT)
	#define SPIFLASH_SPISTATbits	(SPI1STATbits)


	// ENC28J60 I/O pins
	#define ENC_CS_TRIS			(TRISGbits.TRISG6)	// Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
	#define ENC_CS_IO			(LATGbits.LATG6)
	//#define ENC_RST_TRIS		(TRISAbits.TRISA7)	// Not connected by default.  It is okay to leave this pin completely unconnected, in which case this macro should simply be left undefined.
	//#define ENC_RST_IO			(LATAbits.LATA7)
	// SPI SCK, SDI, SDO pins are automatically controlled by the
	// PIC24 SPI module, but Peripheral Pin Select must be configured correctly.
	// MISO = RB0 (RP0); MOSI = RB1 (RP1); SCK = RD8 (RP2)
	#define ENC_SPI_IF			(IFS0bits.SPI1IF)
	#define ENC_SSPBUF			(SPI1BUF)
	#define ENC_SPISTAT			(SPI1STAT)
	#define ENC_SPISTATbits		(SPI1STATbits)
	#define ENC_SPICON1			(SPI1CON1)
	#define ENC_SPICON1bits		(SPI1CON1bits)
	#define ENC_SPICON2			(SPI1CON2)


//	// ENC624J600 Interface Configuration
//	// Comment out ENC100_INTERFACE_MODE if you don't have an ENC624J600 or
//	// ENC424J600.  Otherwise, choose the correct setting for the interface you
//	// are using.  Legal values are:
//	//  - Commented out: No ENC424J600/624J600 present or used.  All other
//	//                   ENC100_* macros are ignored.
//	//	- 0: SPI mode using CS, SCK, SI, and SO pins
//	//  - 1: 8-bit demultiplexed PSP Mode 1 with RD and WR pins
//	//  - 2: 8-bit demultiplexed PSP Mode 2 with R/Wbar and EN pins
//	//  - 3: 16-bit demultiplexed PSP Mode 3 with RD, WRL, and WRH pins
//	//  - 4: 16-bit demultiplexed PSP Mode 4 with R/Wbar, B0SEL, and B1SEL pins
//	//  - 5: 8-bit multiplexed PSP Mode 5 with RD and WR pins
//	//  - 6: 8-bit multiplexed PSP Mode 6 with R/Wbar and EN pins
//	//  - 9: 16-bit multiplexed PSP Mode 9 with AL, RD, WRL, and WRH pins
//	//  - 10: 16-bit multiplexed PSP Mode 10 with AL, R/Wbar, B0SEL, and B1SEL
//	//        pins
//	#define ENC100_INTERFACE_MODE			0
//
//	// If using a parallel interface, direct RAM addressing can be used (if all
//	// addresses wires are connected), or a reduced number of pins can be used
//	// for indirect addressing.  If using an SPI interface or PSP Mode 9 or 10
//	// (multiplexed 16-bit modes), which require all address lines to always be
//	// connected, then this option is ignored. Comment out or uncomment this
//	// macro to match your hardware connections.
//	#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
//
//	// ENC424J600/624J600 parallel indirect address remapping macro function.
//	// This section translates SFR and RAM addresses presented to the
//	// ReadMemory() and WriteMemory() APIs in ENCX24J600.c to the actual
//	// addresses that must be presented on the parallel interface.  This macro
//	// must be modified to match your hardware if you are using an indirect PSP
//	// addressing mode (ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING is defined) and
//	// have some of your address lines tied off to Vdd.  If you are using the
//	// SPI interface, then this section can be ignored or deleted.
//	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) // 8-bit PSP
//		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
//	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) // 16-bit PSP
//		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		(a)
//	#endif
//
//	// Auto-crossover pins on Fast 100Mbps Ethernet PICtail/PICtail Plus.  If
//	// your circuit doesn't have such a feature, delete these two defines.
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB5)
//	#define ENC100_MDIX_IO					(LATBbits.LATB5)
//
//	// ENC624J600 I/O control and status pins
//	// If a pin is not required for your selected ENC100_INTERFACE_MODE
//	// interface selection (ex: WRH/B1SEL for PSP modes 1, 2, 5, and 6), then
//	// you can ignore, delete, or put anything for the pin definition.  Also,
//	// the INT and POR pins are entirely optional.  If not connected, comment
//	// them out.
//	#define ENC100_INT_TRIS					(TRISAbits.TRISA15)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
//	#define ENC100_INT_IO					(PORTAbits.RA15)
//	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
//		// PSP control signal pinout
//		#define ENC100_CS_TRIS				(TRISGbits.TRISG8)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
//		#define ENC100_CS_IO				(LATGbits.LATG8)
//		#define ENC100_POR_TRIS				(TRISEbits.TRISE9)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//		#define ENC100_POR_IO				(LATEbits.LATE9)
//		#define ENC100_SO_WR_B0SEL_EN_TRIS	(TRISDbits.TRISD4)
//		#define ENC100_SO_WR_B0SEL_EN_IO	(LATDbits.LATD4)
//		#define ENC100_SI_RD_RW_TRIS		(TRISDbits.TRISD5)
//		#define ENC100_SI_RD_RW_IO			(LATDbits.LATD5)
//		#define ENC100_SCK_AL_TRIS			(TRISBbits.TRISB15)
//		#define ENC100_SCK_AL_IO			(LATBbits.LATB15)
//		#undef LED1_TRIS
//		#undef LED1_IO
//		#undef LED2_TRIS
//		#undef LED2_IO
//		#undef LED_GET
//		#undef LED_PUT
//		#define LED1_TRIS					(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register.  This is required with the Fast 100Mbps Ethernet PICtail Plus in parallel mode because this RE9 signal also controls the POR (SHDN) signal.
//		#define LED1_IO						(((BYTE*)&NVMKEY)[1])
//		#define LED2_TRIS					(((BYTE*)&NVMKEY)[1])	// No such LED, map to dummy register.  This is required with the Fast 100Mbps Ethernet PICtail Plus in parallel mode because this RG8 signal also controls the CS signal.
//		#define LED2_IO						(((BYTE*)&NVMKEY)[1])
//		#define LED_GET()					LED0_IO
//		#define LED_PUT(a)					(LED0_IO = (a) & 0x1)
//	#else
//		// SPI pinout
//		#define ENC100_CS_TRIS				(TRISGbits.TRISG6)	// CS is mandatory when using the SPI interface
//		#define ENC100_CS_IO				(LATGbits.LATG6)
//		#define ENC100_POR_TRIS				(TRISCbits.TRISC13)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//		#define ENC100_POR_IO				(LATCbits.LATC13)
//		#define ENC100_SO_WR_B0SEL_EN_TRIS	(TRISBbits.TRISB0)	// SO is ENCX24J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
//		#define ENC100_SO_WR_B0SEL_EN_IO	(PORTBbits.RB0)
//		#define ENC100_SI_RD_RW_TRIS		(TRISBbits.TRISB1)	// SI is ENCX24J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
//		#define ENC100_SI_RD_RW_IO			(LATBbits.LATB1)
//		#define ENC100_SCK_AL_TRIS			(TRISDbits.TRISD8)
//	#endif
//
//
//	// ENC624J600 SPI SFR register selection (controls which SPI peripheral to
//	// use on PICs with multiple SPI peripherals).  If a parallel interface is
//	// used (ENC100_INTERFACE_MODE is >= 1), then the SPI is not used and this
//	// section can be ignored or deleted.
//	#define ENC100_ISR_ENABLE		(IEC1bits.INT2IE)
//	#define ENC100_ISR_FLAG			(IFS1bits.INT2IF)
//	#define ENC100_ISR_POLARITY		(INTCON2bits.INT2EP)
//	#define ENC100_ISR_PRIORITY		(IPC7bits.INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(IFS0bits.SPI1IF)
//	#define ENC100_SSPBUF			(SPI1BUF)
//	#define ENC100_SPISTAT			(SPI1STAT)
//	#define ENC100_SPISTATbits		(SPI1STATbits)
//	#define ENC100_SPICON1			(SPI1CON1)
//	#define ENC100_SPICON1bits		(SPI1CON1bits)
//	#define ENC100_SPICON2			(SPI1CON2)
//
//	// ENC624J600 Bit Bang PSP I/O macros and pin configuration for address and
//	// data.  If using the SPI interface (ENC100_INTERFACE_MODE is 0) then this
//	// section is not used and can be ignored or deleted.  The Enhanced PMP
//	// module available on the PIC24FJ256DA210 family will not work with the
//	// ENC424J600/624J600, so bit bang mode must be used if parallel access is
//	// desired.
//	#define ENC100_BIT_BANG_PMP
//	#if defined(ENC100_BIT_BANG_PMP)
//		#if ENC100_INTERFACE_MODE == 1 || ENC100_INTERFACE_MODE == 2	// Dumultiplexed 8-bit address/data modes
//			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A8 connected (A9-A14 tied to Vdd)
//				// AD0-AD7: "PMD0-PMD7" -> RE0-RE7
//				// A0: "PMA0" -> RB15
//				// A1: "PMA1" -> RB14
//				// A2: "PMA2" -> RG9
//				// A3: "PMA3" -> RA4
//				// A4: "PMA4" -> RA3
//				// A5: "PMA5" -> RF12
//				// A6: "PMA6" -> RA10
//				// A7: "PMA7" -> RA9
//				// A8: "PMA14_TO_P104" "PMA14" -> RD11
//				// RD: "PMRD/RD5" -> RD5
//				// WR: "PMWR/RD4 -> RD4
//				// CS: "AN19/RG8_TO_P72" "AN19/RG8" -> RG8
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0xF9E7; ANSB &= 0x3FFF; ANSG &= 0xFCFF;} while(0)		// RE0-RE7, RF12, RD11, RD4, RD5 (AD0-AD7, A5, A8, WR, RD) pins are already digital only pins.
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x3FFF; TRISFbits.TRISF12 = 0; TRISGbits.TRISG9 = 0; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATGbits.LATG9 = 0; LATA &= 0xF9E7; LATFbits.LATF12 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATAbits.LATA4 = 1; if(_SetMacro & 0x0010) LATAbits.LATA3 = 1; if(_SetMacro & 0x0020) LATFbits.LATF12 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_IN()		(((volatile BYTE*)&TRISE)[0] = 0xFF)
//				#define ENC100_SET_AD_TRIS_OUT()	(((volatile BYTE*)&TRISE)[0] = 0x00)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#else 	// All ENC624J600 address pins A0-A14 connected
//				// AD0-AD7: "PMD0-PMD7" -> RE0-RE7
//				// A0: "PMA0" -> RB15
//				// A1: "PMA1" -> RB14
//				// A2: "PMA2" -> RG9
//				// A3: "PMA3" -> RA4
//				// A4: "PMA4" -> RA3
//				// A5: "PMA5" -> RF12
//				// A6: "PMA6" -> RA10
//				// A7: "PMA7" -> RA9
//				// A8: "PMA8" -> RF5
//				// A9: "PMA9" -> RF4
//				// A10: "PMA10" -> RB13
//				// A11: "PMA11" -> RB12
//				// A12: "PMA12" -> RB11
//				// A13: "PMA13" -> RB10
//				// A14: "PMA14_TO_P104" "PMA14" -> RD11
//				// RD: "PMRD/RD5" -> RD5
//				// WR: "PMWR/RD4 -> RD4
//				// CS: "AN19/RG8_TO_P72" "AN19/RG8" -> RG8
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0xF9E7; ANSB &= 0x03FF; ANSG &= 0xFCFF;} while(0)		// RE0-RE7, RF12, RD11, RD4, RD5 (AD0-AD7, A5, A14, WR, RD) pins are already digital only pins.
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x03FF; TRISF &= 0xEFCF; TRISGbits.TRISG9 = 0; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATA &= 0xF9E7; LATB &= 0x03FF; LATF &= 0xEFCF; LATGbits.LATG9 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATAbits.LATA4 = 1; if(_SetMacro & 0x0010) LATAbits.LATA3 = 1; if(_SetMacro & 0x0020) LATFbits.LATF12 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x0100) LATFbits.LATF5 = 1; if(_SetMacro & 0x0200) LATFbits.LATF4 = 1; if(_SetMacro & 0x0400) LATBbits.LATB13 = 1; if(_SetMacro & 0x0800) LATBbits.LATB12 = 1; if(_SetMacro & 0x1000) LATBbits.LATB11 = 1; if(_SetMacro & 0x2000) LATBbits.LATB10 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_IN()		(((volatile BYTE*)&TRISE)[0] = 0xFF)
//				#define ENC100_SET_AD_TRIS_OUT()	(((volatile BYTE*)&TRISE)[0] = 0x00)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#endif
//		#elif ENC100_INTERFACE_MODE == 3 || ENC100_INTERFACE_MODE == 4	// Dumultiplexed 16-bit address/data modes
//			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A7 connected (A8-A13 tied to Vdd)
//				// A0: "PMA0" -> RB15
//				// A1: "PMA1" -> RB14
//				// A2: "PMA2" -> RG9
//				// A3: "PMA3" -> RA4
//				// A4: "PMA4" -> RA3
//				// A5: "PMA5" -> RF12
//				// A6: "PMA6" -> RA10
//				// A7: "PMA7" -> RA9
//				// AD0-AD7: "PMD0-PMD7" -> RE0-RE7
//				// AD8: "PMD8" -> RG0
//				// AD9: "PMD9" -> RG1
//				// AD10: "RC13_PMD10_TO_P30" "RC13" -> RC13
//				// AD11: "PMBE1_PMD11_TO_P28" "PMBE1" -> RA15
//				// AD12: "PMD12" -> RD12
//				// AD13: "PMD13" -> RD13
//				// AD14: "PMD14" -> RD6
//				// AD15: "PMD15" -> RD7
//				// RD: "PMRD/RD5" -> RD5
//				// WRL & WRH: "PMWR/RD4 -> RD4
//				// CS: "AN19/RG8_TO_P72" "AN19/RG8" -> RG8
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0x79E7; ANSB &= 0x3FFF; ANSD &= 0xCF0F; ANSG &= 0xFCFC;}while(0)
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISBbits.TRISB15 = 0; TRISBbits.TRISB14 = 0; TRISFbits.TRISF12 = 0; TRISGbits.TRISG9 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _wSetMacro = (a); LATA &= 0xF9E7; LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATFbits.LATF12 = 0; LATGbits.LATG9 = 0; if(_wSetMacro & 0x0001) LATBbits.LATB15 = 1; if(_wSetMacro & 0x0002) LATBbits.LATB14 = 1; if(_wSetMacro & 0x0004) LATGbits.LATG9 = 1; if(_wSetMacro & 0x0008) LATAbits.LATA4 = 1; if(_wSetMacro & 0x0010) LATAbits.LATA3 = 1; if(_wSetMacro & 0x0020) LATFbits.LATF12 = 1; if(_wSetMacro & 0x0040) LATAbits.LATA10 = 1; if(_wSetMacro & 0x0080) LATAbits.LATA9 = 1;}while(0)
//				#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//				#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//				#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//				#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#else 	// All ENC624J600 address pins A0-A13 connected
//				// A0: "PMA0" -> RB15
//				// A1: "PMA1" -> RB14
//				// A2: "PMA2" -> RG9
//				// A3: "PMA3" -> RA4
//				// A4: "PMA4" -> RA3
//				// A5: "PMA5" -> RF12
//				// A6: "PMA6" -> RA10
//				// A7: "PMA7" -> RA9
//				// A8: "PMA8" -> RF5
//				// A9: "PMA9" -> RF4
//				// A10: "PMA10" -> RB13
//				// A11: "PMA11" -> RB12
//				// A12: "PMA12" -> RB11
//				// A13: "PMA13" -> RB10
//				// AD0-AD7: "PMD0-PMD7" -> RE0-RE7
//				// AD8: "PMD8" -> RG0
//				// AD9: "PMD9" -> RG1
//				// AD10: "RC13_PMD10_TO_P30" "RC13" -> RC13
//				// AD11: "PMBE1_PMD11_TO_P28" "PMBE1" -> RA15
//				// AD12: "PMD12" -> RD12
//				// AD13: "PMD13" -> RD13
//				// AD14: "PMD14" -> RD6
//				// AD15: "PMD15" -> RD7
//				// RD: "PMRD/RD5" -> RD5
//				// WRL & WRH: "PMWR/RD4 -> RD4
//				// CS: "AN19/RG8_TO_P72" "AN19/RG8" -> RG8
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0x79E7; ANSB &= 0x03FF; ANSD &= 0xCF0F; ANSG &= 0xFCFC;}while(0)
//				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x03FF; TRISF &= 0xEFCF; TRISGbits.TRISG9 = 0;}while(0)
//				#define ENC100_SET_ADDR_IO(a)		do{WORD _wSetMacro = (a); LATA &= 0xF9E7; LATB &= 0x03FF; LATF &= 0xEFCF; LATGbits.LATG9 = 0; if(_wSetMacro & 0x0001) LATBbits.LATB15 = 1; if(_wSetMacro & 0x0002) LATBbits.LATB14 = 1; if(_wSetMacro & 0x0004) LATGbits.LATG9 = 1; if(_wSetMacro & 0x0008) LATAbits.LATA4 = 1; if(_wSetMacro & 0x0010) LATAbits.LATA3 = 1; if(_wSetMacro & 0x0020) LATFbits.LATF12 = 1; if(_wSetMacro & 0x0040) LATAbits.LATA10 = 1; if(_wSetMacro & 0x0080) LATAbits.LATA9 = 1; if(_wSetMacro & 0x0100) LATFbits.LATF5 = 1; if(_wSetMacro & 0x0200) LATFbits.LATF4 = 1; if(_wSetMacro & 0x0400) LATBbits.LATB13 = 1; if(_wSetMacro & 0x0800) LATBbits.LATB12 = 1; if(_wSetMacro & 0x1000) LATBbits.LATB11 = 1; if(_wSetMacro & 0x2000) LATBbits.LATB10 = 1;}while(0)
//				#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//				#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//				#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//				#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#endif
//		#elif ENC100_INTERFACE_MODE == 5 || ENC100_INTERFACE_MODE == 6	// Mutliplexed 8-bit address/data modes
//			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENCX24J600 address pins AD0-AD8 connected (AD9-AD14 tied to Vdd)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSB &= 0x7FFF; ANSG &= 0xFEFF;} while(0)		// RE0-RE7, RD11, RD4, RD5 (AD0-AD7, AD8, WR, RD) pins are already digital only pins.  RB15, RG8 (AL, CS) needs to be made digital only.
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = (BYTE)_wSetMacro; LATDbits.LATD11 = 0; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#else 	// All ENCX24J600 address pins AD0-AD14 connected
//				// AD0-AD7: "PMD0-PMD7" -> RE0-RE7
//				// AD8: "PMA8" -> RF5
//				// AD9: "PMA9" -> RF4
//				// AD10: "PMA10" -> RB13
//				// AD11: "PMA11" -> RB12
//				// AD12: "PMA12" -> RB11
//				// AD13: "PMA13" -> RB10
//				// AD14: "PMA14_TO_P104" "PMA14" -> RD11
//				// RD: "PMRD/RD5" -> RD5
//				// WR: "PMWR/RD4 -> RD4
//				// AL: "PMA0" -> RB15
//				// CS: "AN19/RG8_TO_P72" "AN19/RG8" -> RG8
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSB &= 0x43FF; ANSG &= 0xFEFF;} while(0) // Set pins as digital I/Os (not analog).  RD11, RD5, RD4, RE0-RE7, RF4, RF5 are all digital-only pins and therefore no writes to ANSD, ANSE, or ANSF are needed.
//				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF;}while(0)
//				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISFbits.TRISF5 = 0; TRISFbits.TRISF4 = 0; TRISB &= 0x43FF; TRISDbits.TRISD11 = 0;}while(0)
//				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
//				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = (BYTE)_wSetMacro; LATFbits.LATF5 = 0; LATFbits.LATF4 = 0; LATB &= 0x43FF; LATDbits.LATD11 = 0; if(_wSetMacro & 0x0100) LATFbits.LATF5 = 1; if(_wSetMacro & 0x0200) LATFbits.LATF4 = 1; if(_wSetMacro & 0x0400) LATBbits.LATB13 = 1; if(_wSetMacro & 0x0800) LATBbits.LATB12 = 1; if(_wSetMacro & 0x1000) LATBbits.LATB11 = 1;  if(_wSetMacro & 0x2000) LATBbits.LATB10 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//			#endif
//		#elif ENC100_INTERFACE_MODE == 9 || ENC100_INTERFACE_MODE == 10	// Mutliplexed 16-bit address/data modes
//			// All ENC624J600 adddress/data pins AD0-AD15 connected (required for 16-bit data, so there is no differentiation for indirect versus direct addressing mode)
//			// This pinout is awful for doing 16-bit bit-bang operations.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 16 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
//			// AD0-AD7: "PMD0-PMD7" -> RE0-RE7
//			// AD8: "PMD8" -> RG0
//			// AD9: "PMD9" -> RG1
//			// AD10: "RC13_PMD10_TO_P30" "RC13" -> RC13
//			// AD11: "PMBE1_PMD11_TO_P28" "PMBE1" -> RA15
//			// AD12: "PMD12" -> RD12
//			// AD13: "PMD13" -> RD13
//			// AD14: "PMD14" -> RD6
//			// AD15: "PMD15" -> RD7
//			// RD: "PMRD/RD5" -> RD5
//			// WRL & WRH: "PMWR/RD4 -> RD4
//			// AL: "PMA0" -> RB15
//			// CS: "AN19/RG8_TO_P72" "AN19/RG8" -> RG8
//			#define ENC100_INIT_PSP_BIT_BANG()	do{ANSBbits.ANSB15 = 0; ANSCbits.ANSC13 = 0; ANSD &= 0xCF0F; ANSGbits.ANSG8 = 0;}while(0)	// Set pins as digital I/Os (not analog).  RA15 and RE0-RE7 are all digital-only pins and therefore no writes to ANSA or ANSE are needed.
//			#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//			#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//			#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//			#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//			#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//			#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//			#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
//		#endif
//	#endif


//	//----------------------------
//	// MRF24WB0M WiFi I/O pins
//	//----------------------------
//	#define WF_CS_TRIS			(TRISGbits.TRISG8)		// Comment this line out if you are using the ENC28J60, ENC424J600/624J600, or other network controller.
//	#define WF_CS_IO			(LATGbits.LATG8)
//	#define WF_SDI_TRIS			(TRISBbits.TRISB1)
//	#define WF_SCK_TRIS			(TRISDbits.TRISD8)
//	#define WF_SDO_TRIS			(TRISBbits.TRISB0)
//	#define WF_RESET_TRIS		(TRISAbits.TRISA15)
//	#define WF_RESET_IO			(LATAbits.LATA15)
//	#define WF_INT_TRIS		    (TRISEbits.TRISE9)  // INT1
//	#define WF_INT_IO			(PORTEbits.RE9)
//	#define WF_HIBERNATE_TRIS   (TRISAbits.TRISA7)
//	#define	WF_HIBERNATE_IO		(LATAbits.LATA7)
//	#define WF_INT_EDGE		    (INTCON2bits.INT1EP)
//	#define WF_INT_IE			(IEC1bits.INT1IE)
//	#define WF_INT_IF			(IFS1bits.INT1IF)
//	#define WF_SSPBUF			(SPI1BUF)
//	#define WF_SPISTAT			(SPI1STAT)
//	#define WF_SPISTATbits		(SPI1STATbits)
//	#define WF_SPICON1			(SPI1CON1)
//	#define WF_SPICON1bits		(SPI1CON1bits)
//	#define WF_SPICON2			(SPI1CON2)
//	#define WF_SPI_IE			(IEC0bits.SPI1IE)
//	//#define WF_SPI_IP			(IPC2bits.SPI1IP)
//	#define WF_SPI_IF			(IFS0bits.SPI1IF)


#elif defined(YOUR_BOARD)
// Define your own board hardware profile here
// Define your own board hardware profile here
	// I/O pins
	#define 	LED0_TRIS		(TRISEbits.TRISE2)
	#define 	LED0_IO			(PORTEbits.RE2)
	#define 	LED1_TRIS		(TRISEbits.TRISE3)
	#define 	LED1_IO			(PORTEbits.RE3)
	#define 	LED2_TRIS		(TRISEbits.TRISE4)
	#define 	LED2_IO			(PORTEbits.RE4)
	#define 	LED3_TRIS		(TRISEbits.TRISE5)
	#define 	LED3_IO			(PORTEbits.RE5)
	#define 	LED4_TRIS		(TRISDbits.TRISD0)
	#define 	LED4_IO			(PORTDbits.RD0)
	#define 	LED5_TRIS		(TRISDbits.TRISD1)
	#define 	LED5_IO			(PORTDbits.RD1)
	#define 	LED6_TRIS		(TRISDbits.TRISD2)
	#define 	LED6_IO			(PORTDbits.RD2)
	#define 	LED7_TRIS		(TRISEbits.TRISE0)
	#define 	LED7_IO			(PORTEbits.RE0)
	#define 	LED_IO			(*((volatile unsigned char*)(&PORTE)))

	#define 	BUTTON0_TRIS	(TRISBbits.TRISB3)
	#define		BUTTON0_IO		(PORTBbits.RB3)
	#define 	BUTTON1_TRIS	(TRISBbits.TRISB2)
	#define		BUTTON1_IO		(PORTBbits.RB2)
	#define 	BUTTON2_TRIS	(TRISBbits.TRISB1)
	#define		BUTTON2_IO		(PORTBbits.RB1)
	#define 	BUTTON3_TRIS	(TRISBbits.TRISB0)
	#define		BUTTON3_IO		(PORTBbits.RB0)

	// LCD I/O pins
	#define 	LCD_DATA_TRIS	(TRISF)
	#define 	LCD_DATA_IO		(LATF)
	#define 	LCD_RD_WR_TRIS	(TRISFbits.TRISF1)
	#define 	LCD_RD_WR_IO	(LATFbits.LATF1)
	#define 	LCD_RS_TRIS		(TRISFbits.TRISF2)
	#define 	LCD_RS_IO		(LATFbits.LATF2)
	#define 	LCD_E_TRIS		(TRISFbits.TRISF3)
	#define 	LCD_E_IO		(LATFbits.LATF3)

	// Servo Output
	#define		RCS1_TRIS		(TRISCbits.TRISC2)
	#define 	RCS1_IO			(LATCbits.LATC2)
	#define		RCS2_TRIS		(TRISCbits.TRISC1)
	#define 	RCS2_IO			(LATCbits.LATC1)
	#define		UIO1_TRIS		(TRISCbits.TRISC6)
	#define		UIO1_IO			(LATCbits.LATC6)
	#define		UIO2_TRIS		(TRISCbits.TRISC7)
	#define		UIO2_IO			(LATCbits.LATC7)

	// 24LC256 I/O pins
	#define EEPROM_SCL_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDA_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SCL_IO		(PORTCbits.RC3)
	#define EEPROM_SDA_IO		(PORTCbits.RC4)
	#define EEPROM_SSPBUF		(SSPBUF)
	#define EEPROM_SSPCON1		(SSPCON1)
	#define EEPROM_SSPCON1bits	(SSPCON1bits)
	#define EEPROM_SSPCON2		(SSPCON2)
	#define EEPROM_SSPCON2bits	(SSPCON2bits)
	#define EEPROM_SSPSTATbits	(SSPSTATbits)


// I2C bits are in different registers for 8-bit and 16-bit
// microcontrollers
#if defined(__18CXX)
	#define I2CBIT_ACKDT		(EEPROM_SSPCON2bits.ACKDT)
	#define I2CBIT_ACKEN		(EEPROM_SSPCON2bits.ACKEN)
	#define I2CBIT_RCEN			(EEPROM_SSPCON2bits.RCEN)
	#define I2CBIT_PEN			(EEPROM_SSPCON2bits.PEN)
	#define I2CBIT_SEN			(EEPROM_SSPCON2bits.SEN)
	#define I2CBIT_ACKSTAT		(EEPROM_SSPCON2bits.ACKSTAT)
	#define I2CBIT_TRSTAT		(EEPROM_SSPSTATbits.R_W)
//	#define I2CBIT_TRSTAT		(EEPROM_SSPSTATbits.BF)
	#define I2CBIT_IWCOL		(EEPROM_SSPCON1bits.WCOL)
	#define I2C_DATA_IN			(EEPROM_SSPBUF)
	#define I2C_DATA_OUT		(EEPROM_SSPBUF)
#elif defined(__C30__)
	#define I2CBIT_ACKDT		(EEPROM_I2CCONbits.ACKDT)
	#define I2CBIT_ACKEN		(EEPROM_I2CCONbits.ACKEN)
	#define I2CBIT_RCEN			(EEPROM_I2CCONbits.RCEN)
	#define I2CBIT_PEN			(EEPROM_I2CCONbits.PEN)
	#define I2CBIT_SEN			(EEPROM_I2CCONbits.SEN)
	#define I2CBIT_ACKSTAT		(EEPROM_I2CSTATbits.ACKSTAT)
	#define I2CBIT_TRSTAT		(EEPROM_I2CSTATbits.TRSTAT)
	#define I2CBIT_IWCOL		(EEPROM_I2CSTATbits.IWCOL)
	#define I2C_DATA_IN			(EEPROM_I2CRCV)
	#define I2C_DATA_OUT		(EEPROM_I2CTRN)
#endif



#else
	#error "Hardware profile not defined.  See available profiles in HardwareProfile.h and modify or create one."
#endif


#if defined(__18CXX)	// PIC18
	// UART mapping functions for consistent API names across 8-bit and 16 or
	// 32 bit compilers.  For simplicity, everything will use "UART" instead
	// of USART/EUSART/etc.
	#define BusyUART()				BusyUSART()
	#define CloseUART()				CloseUSART()
	#define ConfigIntUART(a)		ConfigIntUSART(a)
	#define DataRdyUART()			DataRdyUSART()
	#define OpenUART(a,b,c)			OpenUSART(a,b,c)
	#define ReadUART()				ReadUSART()
	#define WriteUART(a)			WriteUSART(a)
	#define getsUART(a,b,c)			getsUSART(b,a)
	#define putsUART(a)				putsUSART(a)
	#define getcUART()				ReadUSART()
	#define putcUART(a)				WriteUSART(a)
	#define putrsUART(a)			putrsUSART((far rom char*)a)

#else	 // PIC24F, PIC24H, dsPIC30, dsPIC33, PIC32
	// Some A/D converter registers on dsPIC30s are named slightly differently
	// on other procesors, so we need to rename them.
	#if defined(__dsPIC30F__)
		#define ADC1BUF0			ADCBUF0
		#define AD1CHS				ADCHS
		#define	AD1CON1				ADCON1
		#define AD1CON2				ADCON2
		#define AD1CON3				ADCON3
		#define AD1PCFGbits			ADPCFGbits
		#define AD1CSSL				ADCSSL
		#define AD1IF				ADIF
		#define AD1IE				ADIE
		#define _ADC1Interrupt		_ADCInterrupt
	#endif

	// Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE
	// options will use.  You can change these to U1BRG, U1MODE, etc. if you
	// want to use the UART1 module instead of UART2.
	#define UBRG					U2BRG
	#define UMODE					U2MODE
	#define USTA					U2STA
	#define BusyUART()				BusyUART2()
	#define CloseUART()				CloseUART2()
	#define ConfigIntUART(a)		ConfigIntUART2(a)
	#define DataRdyUART()			DataRdyUART2()
	#define OpenUART(a,b,c)			OpenUART2(a,b,c)
	#define ReadUART()				ReadUART2()
	#define WriteUART(a)			WriteUART2(a)
	#define getsUART(a,b,c)			getsUART2(a,b,c)
	#if defined(__C32__)
		#define putsUART(a)			putsUART2(a)
	#else
		#define putsUART(a)			putsUART2((unsigned int*)a)
	#endif
	#define getcUART()				getcUART2()
	#define putcUART(a)				do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
	#define putrsUART(a)			putrsUART2(a)
#endif


#endif
