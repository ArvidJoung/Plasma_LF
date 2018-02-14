/*
 * spi.c
 *
 *  Created on: 2014. 4. 24.
 *      Author: lastdkht
 */

#include "DSP280x_Device.h"     // Headerfile Include File
#include "DSP280x_Examples.h"   // Examples Include File

void Init_SpibGpio_280x()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;     // Enable pull-up on GPIO12 (SPISIMOB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;     // Enable pull-up on GPIO24 (SPISIMOB)

//    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;     // Enable pull-up on GPIO13 (SPISOMIB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;     // Enable pull-up on GPIO25 (SPISOMIB)

    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;     // Enable pull-up on GPIO14 (SPICLKB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;     // Enable pull-up on GPIO26 (SPICLKB)

//   GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;     // Enable pull-up on GPIO15 (SPISTEB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;     // Enable pull-up on GPIO27 (SPISTEB)


/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;   // Asynch input GPIO12 (SPISIMOB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 3;   // Asynch input GPIO24 (SPISIMOB)

//    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3;   // Asynch input GPIO13 (SPISOMIB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 3;   // Asynch input GPIO25 (SPISOMIB)

    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;   // Asynch input GPIO14 (SPICLKB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;   // Asynch input GPIO26 (SPICLKB)

//    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;   // Asynch input GPIO15 (SPISTEB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 3;   // Asynch input GPIO27 (SPISTEB)

/* Configure SPI-B pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 3;    // Configure GPIO12 as SPISIMOB
//  GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 3;    // Configure GPIO24 as SPISIMOB

//    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 3;    // Configure GPIO13 as SPISOMIB
//  GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 3;    // Configure GPIO25 as SPISOMIB

    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 3;    // Configure GPIO14 as SPICLKB
//  GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 3;    // Configure GPIO26 as SPICLKB

//    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 3;    // Configure GPIO15 as SPISTEB
//  GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 3;    // Configure GPIO27 as SPISTEB

    EDIS;
}

void Init_Spib_280x ()
{
	// Initialize SPI-B PIN
	Init_SpibGpio_280x();

	/*
	EALLOW;
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;	// Configure GPIO19 as EEPROM_CS
	GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;		// Output, EEPROM_CS
	EDIS;
	GpioDataRegs.GPASET.bit.GPIO19 = 1;		// High, EEPROM_CS
	*/

	/*
	// Initialize Low Speed Clock Prescaler
	EALLOW;
	SysCtrlRegs.LOSPCP.all = 0x0000;		// LOSCLK = 100MHZ
	EDIS;
	*/

	// Initialize internal SPI peripheral module
	SpibRegs.SPICCR.bit.SPISWRESET=0;				// Reset SCI
	SpibRegs.SPICCR.bit.SPICHAR = 15;				// 15bit 데이터 크기 사용
	SpibRegs.SPICCR.bit.CLKPOLARITY = 0;			// 클럭의 상승엣지에 데이터 출력
	SpibRegs.SPICTL.bit.CLK_PHASE = 1;				// Half-cycle 만큼 클럭지연
	SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;			// 마스터 모드
	SpibRegs.SPICTL.bit.TALK = 1;					// 전송 기능을 활성화
	SpibRegs.SPIBRR = 9;							// 10Mbps
	SpibRegs.SPICCR.bit.SPISWRESET=1;				// Enable SCI
}

Uint16 Write_Spib_280x (Uint16 Data)
{
	Uint16 Read = 0, Temp;
	Temp = Data;
	//Temp = Data << 8;
	//Temp = Temp & 0xFF00;

	while(SpibRegs.SPISTS.bit.BUFFULL_FLAG);
	SpibRegs.SPITXBUF = Temp;

	//while(!(SpibRegs.SPISTS.bit.INT_FLAG));
	//Read = SpibRegs.SPIRXBUF;

	return Read;
}

