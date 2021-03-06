// TI File $Revision: /main/4 $
// Checkin $Date: April 18, 2007   10:28:50 $
//###########################################################################
//
// FILE:	Example_EpwmSetup.c
//
// TITLE:	Frequency measurement using EQEP peripheral
//
// DESCRIPTION:
//
// This file contains source for the ePWM initialization for the
// freq calculation module
//
//###########################################################################
// Original Author: SD
//
// $TI Release: DSP280x Header Files V1.60 $
// $Release Date: December 3, 2007 $
//###########################################################################

#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "DSP280x_Examples.h"   // DSP280x Examples Include File
#include "Example_freqcal.h"    // Example specific include file

#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6
#endif
#if (CPU_FRQ_60MHZ)
  #define CPU_CLK   60e6
#endif
#define PWM_CLK   5e3
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, timebase = SYSCLKOUT


void EPwmSetup()
{
	EPwm1Regs.TBSTS.all=0;
	EPwm1Regs.TBPHS.half.TBPHS=0;
	EPwm1Regs.TBCTR=0;

	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm1Regs.CMPA.half.CMPA =SP/2;
	EPwm1Regs.CMPB=0;

	EPwm1Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm1Regs.AQCTLB.all=0;
	EPwm1Regs.AQSFRC.all=0;
	EPwm1Regs.AQCSFRC.all=0;

	EPwm1Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm1Regs.DBRED=0;
	EPwm1Regs.DBFED=0;

	EPwm1Regs.TZSEL.all=0;
	EPwm1Regs.TZCTL.all=0;
	EPwm1Regs.TZEINT.all=0;
	EPwm1Regs.TZFLG.all=0;
	EPwm1Regs.TZCLR.all=0;
	EPwm1Regs.TZFRC.all=0;

	EPwm1Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm1Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm1Regs.ETFLG.all=0;
	EPwm1Regs.ETCLR.all=0;
	EPwm1Regs.ETFRC.all=0;

	EPwm1Regs.PCCTL.all=0;

	EPwm1Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm1Regs.TBPRD=SP;
}


