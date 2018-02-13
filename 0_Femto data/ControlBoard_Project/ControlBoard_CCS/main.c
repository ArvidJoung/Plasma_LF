#include "DSP280x_Device.h"     // Headerfile Include File
#include "DSP280x_Examples.h"   // Examples Include File
#include <math.h>
#include <string.h>
#include "global.h"
#include "pwm.h"
#include "spi.h"
#include "adc.h"

#define B_NUMBER	2371

#if FLASH_LOAD == 1
#pragma CODE_SECTION (Main_Loop, "ramfuncs");
#pragma CODE_SECTION (Isr_CpuTimer0, "ramfuncs");
#pragma CODE_SECTION (Isr_Scib_RxFifo, "ramfuncs");
//#pragma CODE_SECTION (Isr_EQep_UnitTimeOut, "ramfuncs");

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
#endif


/* RS232 communication. */
#define SCI_GPIO_9_11			TRUE
#define RX_BUF_SIZE				64

Uint16 gRxBuf[RX_BUF_SIZE];
int32 gnRxBufPos = 0;

Uint16 gRxCmdBuf[RX_BUF_SIZE];
Uint32 gisRxCmd = FALSE;
int32 gnRxCmdSize = 0;

int32 gnRX_SV = 0;

int32 gnSCI_RxErrorCnt = 0;

/* Freq. */
//#define FREQ_BUF_SIZE	60
//
//Uint32 gnFreqBufTmp[FREQ_BUF_SIZE];

/* EQep */
//#define EQEP_1			FALSE
//#define	ENCODER_REV		100000000		/* Pulse/Revolution */
//#define SYSTEM_FREQ		100000000	/* Hz */
//#define UTIME_FREQ		1			/* Hz */
//#define FREQ_BUF_ENABLE FALSE
//
//Uint32	gnIsrQepCount = 0;
//Uint32 gnEQepBuf[FREQ_BUF_SIZE];
//Uint32 gnLatestFreq = 0;
//Uint32	PositionCounter;
//Uint32	RotateDirection;

/* CPLD */
//Uint32 gnCpldBufCount = 0;
//Uint32 gnCpldBuf[FREQ_BUF_SIZE];

/* Timer. */
long long gnTimerMS = 0;

void Init_CpuTimer ()
{
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 100, 1000);
	StartCpuTimer0();

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	IER |= M_INT1;
}

interrupt void Isr_CpuTimer0 (void)
{
	gnTimerMS += 1;

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void Init_Gpio()
{
   EALLOW;

   // Input port. (0 ~ 7)
   GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO17 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO19 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO28 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO30 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO30 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPBPUD.bit.GPIO33 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPBDIR.bit.GPIO33 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO29 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO31 = 1;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO31 = 0;  // GPIO27 = input

   // Output port. (0 ~ 7)
   GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO20 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;  // GPIO27 = input

   // Output port. (8 ~ 15)
   GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO24 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;  // GPIO27 = input

   // SPI CS0 ~ 3
   GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // GPIO27 = input

   GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // GPIO27 = input

   EDIS;
}

void Init_ScibGpio()
{
   EALLOW;

#ifdef SCI_GPIO_9_11
   /* Enable internal pull-up for the selected pins */
   // Pull-ups can be enabled or disabled disabled by the user.
   // This will enable the pullups for the specified pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;     // Enable pull-up for GPIO9  (SCITXDB)
   GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;    // Enable pull-up for GPIO11 (SCIRXDB)
   /* Set qualification for selected pins to asynch only */
   // This will select asynch (no qualification) for the selected pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
   /* Configure SCI-B pins using GPIO regs*/
   // This specifies which of the possible GPIO pins will be SCI functional pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;    // Configure GPIO9 for SCITXDB operation
   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;   // Configure GPIO11 for SCIRXDB operation
#else
   GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;     // Enable pull-up(SCITXDB)
   GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up(SCIRXDB)
   GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input (SCIRXDB)
   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;    // Configure for SCITXDB operation
   GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure for SCIRXDB operation
#endif

    EDIS;
}

void Init_Scib ()
{
	/*
	InitSciaGpio();

	// SCI 주요설정
	SciaRegs.SCICTL1.bit.SWRESET = 0;		// SCI 소프트웨어 리셋

	SciaRegs.SCICCR.bit.SCICHAR = 7;		// SCI 송수신 Charcter-length 설정 : 8bit
	SciaRegs.SCICCR.bit.LOOPBKENA = 1;		// SCI 루프백 테스트 모드 Enable
	SciaRegs.SCICTL1.bit.RXENA = 1;			// SCI 송신기능 Enable
	SciaRegs.SCICTL1.bit.TXENA = 1;			// SCI 수신기능 Enable
	SciaRegs.SCIHBAUD = 0x00;      			// SCI Baudrate 설정
	SciaRegs.SCILBAUD = 0x50;				//  - [SCIHBAUD,SCILBAUD] => 0x0050 => 38580bps(약38400bps)

	// SCI의 송신 FIFO 설정
	SciaRegs.SCIFFTX.bit.SCIFFENA = 1;		// SCI FIFO 사용 설정 Enable
	SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;	// SCI 송신 FIFO 인터럽트 플래그 클리어
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;	// SCI 송신 FIFO RE-enable
	SciaRegs.SCIFFTX.bit.SCIRST = 1;		// SCI 리셋 해제

	// SCI의 수신 FIFO 설정
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;	// SCI 수신 FIFO 인터럽트 플래그 클리어
	SciaRegs.SCIFFRX.bit.RXFFIENA = 1;		// SCI 수신 FIFO 인터럽트 Enable
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;	// SCI 수신 FIFO RE-enable
	SciaRegs.SCIFFRX.bit.RXFFIL = 16;		// SCI 수신 FIFO 인터럽트 레벨 설정

	SciaRegs.SCICTL1.bit.SWRESET = 1;		// SCI 소프트웨어 리셋 해제
	*/

	Init_ScibGpio ();

	// SCI 주요설정
	ScibRegs.SCICTL1.bit.SWRESET = 0;		// SCI 소프트웨어 리셋

	ScibRegs.SCICCR.bit.SCICHAR = 7;		// SCI 송수신 Charcter-length 설정 : 8bit
	//ScibRegs.SCICCR.bit.LOOPBKENA = 1;		// SCI 루프백 테스트 모드 Enable
	ScibRegs.SCICTL1.bit.RXENA = 1;			// SCI 송신기능 Enable
	ScibRegs.SCICTL1.bit.TXENA = 1;			// SCI 수신기능 Enable
	ScibRegs.SCIHBAUD = 0x00;      			// SCI Baudrate 설정
	ScibRegs.SCILBAUD = 0x50;				//  - [SCIHBAUD,SCILBAUD] => 0x0050 => 38580bps(약38400bps)

	// SCI의 송신 FIFO 설정
	ScibRegs.SCIFFTX.bit.SCIFFENA = 1;		// SCI FIFO 사용 설정 Enable
	ScibRegs.SCIFFTX.bit.TXFFINTCLR = 1;	// SCI 송신 FIFO 인터럽트 플래그 클리어
	ScibRegs.SCIFFTX.bit.TXFIFOXRESET = 1;	// SCI 송신 FIFO RE-enable
	ScibRegs.SCIFFTX.bit.SCIRST = 1;		// SCI 리셋 해제

	// SCI의 수신 FIFO 설정
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;	// SCI 수신 FIFO 인터럽트 플래그 클리어
	ScibRegs.SCIFFRX.bit.RXFFIENA = 1;		// SCI 수신 FIFO 인터럽트 Enable
	ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;	// SCI 수신 FIFO RE-enable
	ScibRegs.SCIFFRX.bit.RXFFIL = 1;		// SCI 수신 FIFO 인터럽트 레벨 설정

	ScibRegs.SCICTL1.bit.SWRESET = 1;		// SCI 소프트웨어 리셋 해제

	PieCtrlRegs.PIEIER9.bit.INTx3 = 1;		// PIE 인터럽트(SCIRXINTA) : Enable
	IER |= M_INT9;							// CPU 인터럽트(INT9) : Enable
}

//void Init_ePWM_1 ()
//{
//	/* ePWM1/ePWM2 PIN */
//	EALLOW;
//	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;				// Configure GPIO8 as EPWM5A
//	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;				// Configure GPIO9 as EPWM5B
//
//	GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;  // output
//	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;  // output
//	EDIS;
//
//	/*
//	EALLOW;
//	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
//	EDIS;
//
//	// ePWM5 Time-Base Submodule
//	EPwm5Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;		// set Immediate load (Time-Base Period Register)
//	EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)] (High Speed Time-base Clock Prescale Bits)
//	EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;			// TBCLK = [100MHz / (1*1)] = 100MHz (Time-base Clock Prescale Bits)
//	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		// (Counter Mode)
//	EPwm5Regs.TBPRD = 2000;			  	    		// Set period for ePWM5 (PWM 캐리어 주파수 : 100MHz/(1000 * 2) = 50kHz) (period of the time-base counter)
//	EPwm5Regs.TBCTR = 0;							// Clear counter (Time-Base Counter Register)
//	EPwm5Regs.CMPA.half.CMPA = 500;				// (Counter-Compare A Register)
//	EPwm5Regs.CMPB = 1500;							// (Counter-Compare B Register)
//	//EPwm5Regs.TBCTL.bit.SYNCOSEL = 1;				// (Synchronization Output Select.)
//
//	// ePWM5 Counter-Compare Submodule
//	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// 비교 레지스터에 Shadow 레지스터 사용 (Counter-compare A (CMPA) Register Operating Mode)
//	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	// 카운터가 0 일때 Shadow 레지스터에서 비교 레지스터에 비교 값 로드 (Active Counter-Compare A (CMPA) Load From Shadow Select Mode)
//
//	// ePWM1 Action-qualifier
//	EPwm5Regs.AQCTLA.bit.ZRO = AQ_SET;				// 카운터가 제로일 때 High (Action-Qualifier Output A Control Register)
//	EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low (Action-Qualifier Output A Control Register)
//
//	EALLOW;
//	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
//	EDIS;
//
//	EPwm5Regs.AQCSFRC.bit.CSFA = 0;
//	EPwm5Regs.AQCSFRC.bit.CSFB = 0;
//	*/
//
//	EPwm5Regs.TBPRD = 1200; // Period = 1201 TBCLK counts
//	EPwm5Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
//	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Asymmetrical mode
//	EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Phase loading disabled
//	EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//	EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
//	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
//	EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
//	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
//	EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
//	EPwm5Regs.AQCTLA.bit.PRD = AQ_CLEAR;
//	EPwm5Regs.AQCTLA.bit.CAU = AQ_SET;
//}

//void Init_EQepGpio ()
//{
//	EALLOW;
//
//#if EQEP_1
//	GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;		/* Enable pull-up on GPIO20 (EQEP1A) */
//	GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;		/* Enable pull-up on GPIO21 (EQEP1B) */
//	GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;		/* Enable pull-up on GPIO22 (EQEP1S) */
//	GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;		/* Enable pull-up on GPIO23 (EQEP1I) */
//
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 0;	/* Sync to SYSCLKOUT GPIO20 (EQEP1A) */
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 0;	/* Sync to SYSCLKOUT GPIO21 (EQEP1B) */
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 0;	/* Sync to SYSCLKOUT GPIO22 (EQEP1S) */
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 0;	/* Sync to SYSCLKOUT GPIO23 (EQEP1I) */
//
//	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;	/* Configure GPIO20 as EQEP1A */
//	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;	/* Configure GPIO21 as EQEP1B */
//	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 1;	/* Configure GPIO22 as EQEP1S */
//	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 1;	/* Configure GPIO23 as EQEP1I */
//#else
//	GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;		/* Enable pull-up on GPIO20 (EQEP1A) */
//	GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;		/* Enable pull-up on GPIO21 (EQEP1B) */
//	GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;		/* Enable pull-up on GPIO22 (EQEP1S) */
//	GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;		/* Enable pull-up on GPIO23 (EQEP1I) */
//
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 0;	/* Sync to SYSCLKOUT GPIO20 (EQEP1A) */
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 0;	/* Sync to SYSCLKOUT GPIO21 (EQEP1B) */
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 0;	/* Sync to SYSCLKOUT GPIO22 (EQEP1S) */
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 0;	/* Sync to SYSCLKOUT GPIO23 (EQEP1I) */
//
//	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 2;	/* Configure GPIO20 as EQEP1A */
//	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 2;	/* Configure GPIO21 as EQEP1B */
//	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 2;	/* Configure GPIO22 as EQEP1S */
//	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 2;	/* Configure GPIO23 as EQEP1I */
//#endif
//
//	EDIS;
//}

//void Init_EQep (Uint32 LineEncoder)
//{
//	Init_EQepGpio ();
//
//#if EQEP_1
//	EQep1Regs.QUPRD = (SYSTEM_FREQ/UTIME_FREQ);	/* Unit Timer for 2Hz at 100 MHz SYSCLKOUT */
//
//	EQep1Regs.QDECCTL.bit.QSRC = 0;			/* Quadrature count mode */
//	EQep1Regs.QEPCTL.bit.FREE_SOFT = 2;
//	EQep1Regs.QEPCTL.bit.UTE = 1; 			/* Unit Timer Enable */
//	EQep1Regs.QEPCTL.bit.QCLM = 1; 			/* Position counter latch on unit time out */
//	EQep1Regs.QEPCTL.bit.PCRM = 1;			/* Position Counter Reset on Maximum Position */
//	EQep1Regs.QPOSMAX = 4*LineEncoder;		/* 24 pulse @ 1 revolution */
//	EQep1Regs.QEPCTL.bit.QPEN = 1; 			/* QEP enable */
//	EQep1Regs.QEINT.bit.UTO = 1;			/* Unit Time Out Interrupt Enable */
//#else
//
//#if	FREQ_BUF_ENABLE
//	EQep2Regs.QUPRD = (SYSTEM_FREQ/(UTIME_FREQ * 2));	/* Unit Timer for 2Hz at 100 MHz SYSCLKOUT */
//#else
//	EQep2Regs.QUPRD = (SYSTEM_FREQ/UTIME_FREQ);	/* Unit Timer for 2Hz at 100 MHz SYSCLKOUT */
//#endif
//
//	//EQep2Regs.QDECCTL.bit.QSRC = 0;			/* Quadrature count mode */
//	//EQep2Regs.QDECCTL.bit.QSRC = 1;			/* Direction-count mode */ // => QEPB should be high.
//	EQep2Regs.QDECCTL.bit.QSRC = 2;			/* UP count mode for frequency measurement */
//
//	EQep2Regs.QEPCTL.bit.FREE_SOFT = 2;
//	EQep2Regs.QEPCTL.bit.UTE = 1; 			/* Unit Timer Enable */
//	EQep2Regs.QEPCTL.bit.QCLM = 1; 			/* Position counter latch on unit time out */
//	EQep2Regs.QEPCTL.bit.PCRM = 1;			/* Position Counter Reset on Maximum Position */
//	EQep2Regs.QPOSMAX = 4*LineEncoder;		/* 24 pulse @ 1 revolution */
//	EQep2Regs.QEPCTL.bit.QPEN = 1; 			/* QEP enable */
//	EQep2Regs.QEINT.bit.UTO = 1;			/* Unit Time Out Interrupt Enable */
//#endif
//}

interrupt void Isr_Scib_RxFifo (void)
{
	Uint16 i;
	Uint16 num_byte;
	Uint16 rx_data = 0;

	// 수신된 데이터의 바이트 수 확인
	num_byte = ScibRegs.SCIFFRX.bit.RXFFST;

	// FIFO에 수신된 데이터를 메모리에 저장
	for(i = 0; i < num_byte; i++)
	{
		rx_data = ScibRegs.SCIRXBUF.bit.RXDT;

		if (rx_data == (Uint16)'{')
		{
			gRxBuf[0] = rx_data;
			gnRxBufPos = 1;
		}
		else
		{
			gRxBuf[gnRxBufPos++] = rx_data;
			if (gnRxBufPos >= RX_BUF_SIZE)
			{
				gnRxBufPos = 0;
			}
		}

		if (gRxBuf[0] == (Uint16)'{' && rx_data == (Uint16)'}' && gRxBuf[1] == gnRxBufPos - 4)
		{
			memcpy (gRxCmdBuf, gRxBuf, gnRxBufPos);
			memset (gRxBuf, 0x00, RX_BUF_SIZE);

			gisRxCmd = TRUE;
			gnRxCmdSize = gnRxBufPos;
			gnRxBufPos = 0;
		}
	}

	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;			// Clear Overflow flag
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;			// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;			// Acknowledge interrupt to PIE
}

//interrupt void Isr_EQep_UnitTimeOut (void)
//{
//	gnIsrQepCount++;
//
//	/* Backup Past Position Latch Value */
//	//PastUTimeOutPositionLatch = UTimeOutPositionLatch;
//
//	/* Check Current Position Latch on Unit Time Out Event */
//	//UTimeOutPositionLatch = EQep1Regs.QPOSLAT;
//
//#if EQEP_1
//	gnQepBuf[gnIsrQepCount % FREQ_BUF_SIZE] = EQep1Regs.QPOSLAT;
//
//	/* Calculate Position Displacement & Speed in RPM */
//	if((EQep1Regs.QFLG.bit.PCO || EQep1Regs.QFLG.bit.PCU))
//	{
//		/* Position Counter overflowed or underflowed, hence do no compute speed */
//		EQep1Regs.QCLR.bit.PCO = 1;
//		EQep1Regs.QCLR.bit.PCU = 1;
//	}
//	else
//	{
//		//Displacement = UTimeOutPositionLatch - PastUTimeOutPositionLatch;
//		//SpeedHz = Displacement * SpeedPrescaler;
//		//SpeedRPM = SpeedHz * 60;
//	}
//
//	EQep1Regs.QCLR.bit.UTO = 1;
//	EQep1Regs.QCLR.bit.INT = 1;
//
//	EQep1Regs.QEPCTL.bit.SWI = 1;
//#else
//
//#if FREQ_BUF_ENABLE
//	gnEQepBuf[gnIsrQepCount % FREQ_BUF_SIZE] = EQep2Regs.QPOSLAT;
//#else
//	gnLatestFreq = EQep2Regs.QPOSLAT;
//	//gnEQepBuf[gnIsrQepCount % FREQ_BUF_SIZE] = gnLatestFreq;
//#endif
//
//	/* Calculate Position Displacement & Speed in RPM */
//	if((EQep2Regs.QFLG.bit.PCO || EQep2Regs.QFLG.bit.PCU))
//	{
//		/* Position Counter overflowed or underflowed, hence do no compute speed */
//		EQep2Regs.QCLR.bit.PCO = 1;
//		EQep2Regs.QCLR.bit.PCU = 1;
//	}
//	else
//	{
//		//Displacement = UTimeOutPositionLatch - PastUTimeOutPositionLatch;
//		//SpeedHz = Displacement * SpeedPrescaler;
//		//SpeedRPM = SpeedHz * 60;
//	}
//
//	EQep2Regs.QCLR.bit.UTO = 1;
//	EQep2Regs.QCLR.bit.INT = 1;
//
//	EQep2Regs.QEPCTL.bit.SWI = 1;
//#endif
//
//	PieCtrlRegs.PIEACK.bit.ACK5 = 1;
//}

Uint32 Code10_To_Int (char *data, Uint16 start, Uint16 len)
{
	Uint32 ret = 0;
	Uint32 mul = 0;
	Uint16 idx = 0;
	Uint16 i = 0;

	if (len <= 0 || len > 4) return 0;

	for (i = 0; i < len; i++)
	{
		mul = (Uint32) pow (10.0, (double)i);
		idx = start + len - i - 1;
		ret += (data[idx] - '0') * mul;
	}

	return ret;
}

void Int_To_Code10 (char *data, Uint16 start, Uint16 len, Uint32 val)
{
	Uint16 code10[4] = {0, };
	Uint16 i = 0;

	if (len <= 0 || len > 4) return;

	code10[0] = val % 10;
	code10[1] = (val / 10) % 10;
	code10[2] = (val / 100) % 10;
	code10[3] = (val / 1000) % 10;

	for (i = 0; i < len; i++)
	{
		data[start + len - 1 - i] = '0' + (char)code10[i];
	}
}

void Uint16_To_Code256 (char *data, Uint16 start, Uint16 val)
{
	data[start] = val % 256;
	data[start + 1] = val / 256;
}

void Uint32_To_Code10 (char *data, Uint16 start, Uint16 len, Uint32 val)
{
	Uint16 code10[10] = {0, };
	Uint16 i = 0;

	if (len < 10) return;

	code10[0] = val % 10;
	code10[1] = (val / 10) % 10;
	code10[2] = (val / 100) % 10;
	code10[3] = (val / 1000) % 10;
	code10[4] = (val / 10000) % 10;
	code10[5] = (val / 100000) % 10;
	code10[6] = (val / 1000000) % 10;
	code10[7] = (val / 10000000) % 10;
	code10[8] = (val / 100000000) % 10;
	code10[9] = (val / 1000000000) % 10;

	for (i = 0; i < 10; i++)
	{
		data[start + 9 - i] = '0' + (char)code10[i];
	}
}

void Uint32_To_Code64 (Uint32 val, char *buf, int len)
{
	if (len == 1)
	{
		buf[0] = (char)('0' + (val % 64));
	}
	else if (len == 2)
	{
		buf[0] = (char)('0' + (val % 64));
		buf[1] = (char)('0' + (val / 64));
	}
	else if (len == 3)
	{
		buf[0] = (char)('0' + (val % 64));
		buf[1] = (char)('0' + (val / 64 % 64));
		buf[2] = (char)('0' + (val / 4096));
	}
	else if (len == 4)
	{
		buf[0] = (char)('0' + (val % 64));
		buf[1] = (char)('0' + (val / 64 % 64));
		buf[2] = (char)('0' + (val / 4096 % 64));
		buf[3] = (char)('0' + (val / 262144));
	}
}

Uint32 Code64_To_Uint32 (char *buf, int len)
{
	int i = 0;
	int ret = 0;

	for (i = 0; i < len; i++)
	{
		ret += (buf[i] - '0') * (int)pow (64, i);
	}

	return ret;
}

char CalCheckSum_Code64 (char *data, Uint16 start, Uint16 len)
{
	int16 i = 0;
	int32 ret = 0;

	for (i = 0; i < len; i++)
	{
		ret += data[start + i];
	}
	ret += 17;

	return '0' + (char)(ret % 64);
}

char CalCheckSum (char *data, Uint16 start, Uint16 len)
{
	int16 i = 0;
	int32 ret = 0;

	for (i = 0; i < len; i++)
	{
		ret += data[start + i];
	}

	return (char)(ret % 256);
}

Uint16 Scib_Tx_Byte(char *Data, Uint16 num_byte)
{
	Uint16 i;

	for(i = 0 ; i < num_byte; i++)
	{
		while (ScibRegs.SCIFFTX.bit.TXFFST > 8);

		ScibRegs.SCITXBUF = (Uint16)Data[i] & 0x00FF;
	}

	return 1;
}

void Tx_Cmd (char *data, Uint16 len)
{
	char buf[RX_BUF_SIZE];

	memcpy (buf + 2, data, len);

	buf[0] = '{';
	buf[1] = (char)len;
	buf[len + 2] = CalCheckSum_Code64 (buf, 2, len);
	buf[len + 3] = '}';

	Scib_Tx_Byte (buf, len + 4);
}

void Sort_Freq (Uint32 src[], Uint32 dst[], int32 len)
{
	int32 i;
	int32 n;
	Uint32 m;

	for (i = 0; i < len; i++)
	{
		dst[i] = src[i];
	}

	for (i = 0; i < len; i++)
	{
		for (n = 0; n < len - i - 1; n++)
		{
			if (dst[n] > dst[n + 1])
			{
				m = dst[n];
				dst[n] = dst[n + 1];
				dst[n + 1] = m;
			}
		}
	}
}

#define RULE_OUT_NUM	10
Uint32 Get_Freq (Uint32 src[], Uint32 dst[], int32 len)
{
	int i = 0;
	Uint32 sum = 0;

	Sort_Freq (src, dst, len);

	for (i = RULE_OUT_NUM; i < len - RULE_OUT_NUM; i++)
	{
		sum += dst[i];
	}

	return sum / (len - (RULE_OUT_NUM * 2));
}

/*
Uint16 Spi8Driver(Uint16 Data)
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
*/

void Set_DAC (Uint16 id, Uint16 value)
{
	Uint16 da_value = 0x1000;

	if (id % 2 == 1) da_value |= 0x8000;

	if (value > 0x0FFF) da_value |= 0x0FFF;
	//else if (value < 0) da_value |= 0x0000;
	else
	{
		//da_value |= (Uint16)(value / 10.0 * 0x0fff);
		da_value |= value;
	}

	if (id <= 1)
		GpioDataRegs.GPACLEAR.bit.GPIO0=1;
	else if (id <= 3)
		GpioDataRegs.GPACLEAR.bit.GPIO2=1;
	else if (id <= 5)
		GpioDataRegs.GPACLEAR.bit.GPIO4=1;
	else if (id <= 7)
		GpioDataRegs.GPACLEAR.bit.GPIO5=1;

	DELAY_US (5);

	Write_Spib_280x (da_value);

	DELAY_US (20);

	if (id <= 1)
		GpioDataRegs.GPASET.bit.GPIO0=1;
	else if (id <= 3)
		GpioDataRegs.GPASET.bit.GPIO2=1;
	else if (id <= 5)
		GpioDataRegs.GPASET.bit.GPIO4=1;
	else if (id <= 7)
		GpioDataRegs.GPASET.bit.GPIO5=1;
}

void Set_OutputPort (Uint16 port)
{
	// Set.
	GpioDataRegs.GPASET.bit.GPIO20 	= (Uint16)((port >> 0) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO22 	= (Uint16)((port >> 1) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO1 	= (Uint16)((port >> 2) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO13 	= (Uint16)((port >> 3) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO3 	= (Uint16)((port >> 4) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO6 	= (Uint16)((port >> 5) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO8 	= (Uint16)((port >> 6) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO10 	= (Uint16)((port >> 7) & 0x01);

	GpioDataRegs.GPASET.bit.GPIO24 	= (Uint16)((port >> 8) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO26 	= (Uint16)((port >> 9) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO7 	= (Uint16)((port >> 10) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO16 	= (Uint16)((port >> 11) & 0x01);
	GpioDataRegs.GPBSET.bit.GPIO32 	= (Uint16)((port >> 12) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO25 	= (Uint16)((port >> 13) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO27 	= (Uint16)((port >> 14) & 0x01);
	GpioDataRegs.GPASET.bit.GPIO15 	= (Uint16)((port >> 15) & 0x01);

	// Clear.
	GpioDataRegs.GPACLEAR.bit.GPIO20 	= ((Uint16)((port >> 0) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO22 	= ((Uint16)((port >> 1) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO1 	= ((Uint16)((port >> 2) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO13 	= ((Uint16)((port >> 3) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO3 	= ((Uint16)((port >> 4) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO6 	= ((Uint16)((port >> 5) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO8 	= ((Uint16)((port >> 6) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO10 	= ((Uint16)((port >> 7) & 0x01) == 0) ? 1 : 0;

	GpioDataRegs.GPACLEAR.bit.GPIO24 	= ((Uint16)((port >> 8) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO26 	= ((Uint16)((port >> 9) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO7 	= ((Uint16)((port >> 10) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO16 	= ((Uint16)((port >> 11) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPBCLEAR.bit.GPIO32 	= ((Uint16)((port >> 12) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO25 	= ((Uint16)((port >> 13) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO27 	= ((Uint16)((port >> 14) & 0x01) == 0) ? 1 : 0;
	GpioDataRegs.GPACLEAR.bit.GPIO15 	= ((Uint16)((port >> 15) & 0x01) == 0) ? 1 : 0;
}

#define INPUT_PORT_NUM		8
#define OUTPUT_PORT_NUM		16
#define ADC_NUM				8
#define DAC_NUM				8

void Main_Loop ()
{
	char rx_cmd_buf [RX_BUF_SIZE];
	char tx_cmd_buf [RX_BUF_SIZE];
	char check_sum = 0;
	int16 rx_cmd_size = 0;
	int16 rx_val_size = 0;

	Uint16 i = 0;
	Uint16 num = 0;
	Uint32 loop_count = 0;

	/*
	Uint32 freq_result = 0;
	Uint32 freq_eqep_result = 0;
	Uint32 freq_cpld_result = 0;
	int32 byte_order = 0;
	int16 is_bit_reset_prior = FALSE;
	int16 is_bit_order_prior = FALSE;
	int16 is_cal_freq = FALSE;
	int32 byte_value[8] = {0,};
	*/

	float32 width = 20;
	Uint16 da_value = 0;

	Uint16 adc_id = 0;
	Uint16 adc_val = 0;

	Uint16 comm_input_val = 0;
	Uint16 comm_input_val_old = 0;
	Uint16 comm_input_edge_val = 0;
	Uint16 comm_output_val = 0;
	Uint16 comm_adc_val[ADC_NUM] = {0, };
	Uint16 comm_dac_val[DAC_NUM] = {0, };


	Uint16 da0 = 100;
	Uint16 da1 = 100;

	char cmd_code = 0;

	long long last_rx_ms = 0;

	/* Init. */
	//ResetPwm ();

//	for (i = 0; i < FREQ_BUF_SIZE; i++)
//	{
//		gnEQepBuf[i] = 0;
//		gnCpldBuf[i] = 0;
//	}

	/* Loop. */
	while (TRUE)
	{
		/* SCI command. */
		if (gisRxCmd)
		{
			gisRxCmd = FALSE;

			rx_cmd_size = gnRxCmdSize;
			rx_val_size = rx_cmd_size - 5;
			for (i = 0; i < rx_cmd_size; i++)
			{
				rx_cmd_buf[i] = (char)gRxCmdBuf[i];
			}

			/* CheckSum. */
			check_sum = CalCheckSum_Code64 (rx_cmd_buf, 2, rx_cmd_size - 4);
			if (check_sum != rx_cmd_buf[rx_cmd_size - 2])
			{
				Tx_Cmd ("E1", 2);
				continue;
			}

			cmd_code = rx_cmd_buf[2];

			/* B-Number. */
			if (cmd_code == 'V' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = 'V';

				Uint32_To_Code64 (B_NUMBER, tx_cmd_buf + num, 4);
				num += 4;

				Tx_Cmd (tx_cmd_buf, num);
				last_rx_ms = gnTimerMS;
			}

			/* Read. */
			else if (cmd_code == 'R' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = 'R';

				Uint32_To_Code64 (comm_input_val, tx_cmd_buf + num, 2);
				num += 2;

				Uint32_To_Code64 (comm_input_edge_val, tx_cmd_buf + num, 2);
				num += 2;

				for (i = 0; i < ADC_NUM; i++)
				{
					Uint32_To_Code64 (comm_adc_val[i], tx_cmd_buf + num, 2);
					num += 2;
				}

				Tx_Cmd (tx_cmd_buf, num);
				last_rx_ms = gnTimerMS;
			}

			/* Write. */
			else if (cmd_code == 'W' && rx_val_size == 20)
			{
				num = 3;
				comm_output_val = Code64_To_Uint32 (rx_cmd_buf + num, 4);
				num += 4;

				for (i = 0; i < DAC_NUM; i++)
				{
					comm_dac_val[i] = Code64_To_Uint32 (rx_cmd_buf + num, 2);
					num += 2;
				}

				Tx_Cmd ("W", 1);
				last_rx_ms = gnTimerMS;
			}

			/* Clear edge. */
			else if (cmd_code == 'C' && rx_val_size == 0)
			{
				comm_input_edge_val = 0;
				Tx_Cmd ("C", 1);
				last_rx_ms = gnTimerMS;
			}

			/* Error. */
			else
			{
				Tx_Cmd ("E0", 2);
			}
		}

		/* RX error check. */
		if (ScibRegs.SCIRXST.bit.RXERROR == 0x01)
		{
			gnSCI_RxErrorCnt++;

			Init_Scib ();

			for (i = 0; i < 1000; i++)
			{
				//Sleep10us (100);
				DELAY_US (1000);
				if (ScibRegs.SCIRXST.bit.RXERROR == 0x00) break;
			}
		}

		/* Get input port (edge). */
		if (loop_count % 1000 == 0)
		{
			Uint16 input_val = 0;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO21;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO17 << 1;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO19 << 2;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO28 << 3;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO30 << 4;
			input_val |= GpioDataRegs.GPBDAT.bit.GPIO33 << 5;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO29 << 6;
			input_val |= GpioDataRegs.GPADAT.bit.GPIO31 << 7;

			comm_input_val = input_val;

			// Edge
			if (comm_input_val != comm_input_val_old)
			{
				Uint16 input_edge_val = comm_input_edge_val;

				for (i = 0; i < INPUT_PORT_NUM; i++)
				{
					Uint16 new_val = (comm_input_val >> i) & 0x01;
					Uint16 old_val = (comm_input_val_old >> i) & 0x01;

					if (new_val != old_val && new_val == 1)
					{
						input_edge_val |= (0x01 << i);
					}
				}

				comm_input_edge_val = input_edge_val;
			}

			comm_input_val_old = comm_input_val;

			// Edge - Start, Stop button.
			if ((comm_input_val & 0x0002) > 0) comm_input_edge_val &= ~0x0001;
			else if ((comm_input_val & 0x0001) > 0) comm_input_edge_val &= ~0x0002;
			else if ((comm_input_edge_val & 0x0001) > 0 && (comm_input_edge_val & 0x0002) > 0)
			{
				comm_input_edge_val &= ~0x0001;
			}
		}

		/* Set output port. */
		if (loop_count % 1000 == 0)
		{
			Set_OutputPort (comm_output_val);
		}

		/* ADC. */
		if (loop_count % 5000 == 0)
		{
			Run_Adc_280x ();

			for (i = 0; i < ADC_NUM; i++)
			{
				comm_adc_val[i] = Get_Adc_Val_280x (i);
			}
		}

		/* DAC. */
		if (loop_count % 5000 == 0)
		{
			for (i = 0; i < DAC_NUM; i++)
			{
				Set_DAC (i, comm_dac_val[i]);
			}
		}

		/* Check COMM error. */
		if (gnTimerMS - last_rx_ms > 5000)
		{
			comm_output_val = 0;
			for (i = 0; i < DAC_NUM; i++)
				comm_dac_val[i] = 0;
		}

		loop_count++;
        DELAY_US (1);

		/* Test. */
//		if (loop_count % 1000000 == 0)
//		{
//			Set_OutputPort (0xFF);
//		}
//		else if (loop_count % 500000 == 0)
//		{
//			Set_OutputPort (0x00);
//		}
//
//		if (loop_count % 70000 == 0)
//		{
//			//GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
//			//GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;
//
//			SetPwm_280x (1, TB_COUNT_UP, 10000, width);
//		}
//
//		// DA Test.
//		if (loop_count % 100000 == 0)
//		{
//			Set_DAC (0, da0);
//			Set_DAC (1, da1);
//		}
//
//		// AD Test.
//		if (loop_count % 1000 == 0)
//		{
//			Run_Adc_280x ();
//		}
//
//		if (loop_count % 100000 == 0)
//		{
//			adc_val = Get_Adc_Val_280x (adc_id);
//		}

//		  // Flash read/write test...
//        if (loop_count % 10000 == 0)
//        {
//
//        	char bb[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
//        	char cc[16] = { 0, };
//
//			MemCopy ((Uint16*)bb, (Uint16*)(bb + 10), (Uint16*)0x3f1000);
//
//			DELAY_US (100);
//
//			MemCopy ((Uint16*)0x3f1000, (Uint16*)(0x3f1000 + 10), (Uint16*)cc);
//
//			char *v;
//			//v = (char*)0x3f1000;
//			v = (char*)0x3f7FF8;
//			//(*v) = 0x12;
//
//			char v1 = 0;
//			v1 = *v;
//
//			v1++;
//        }
	}
}


void main(void)
{
	//============================================================================================
	// Step 1. Disable Global Interrupt
	//--------------------------------------------------------------------------------------------
	DINT;

	//============================================================================================
	// Step 2. 시스템 컨트롤 초기화:
	//--------------------------------------------------------------------------------------------
	// 초기화 시작
	// PLL, WatchDog, enable Peripheral 클럭의 초기화
	// DSP280x_SysCtrl.c 에 정의된 함수.
	//		2.1 Disables the watchdog
	//		2.2 Set the PLLCR for proper SYSCLKOUT frequency
	//		2.3 Set the pre-scaler for the high and low frequency peripheral clocks
	//		2.4 Enable the clocks to the peripherals
	//--------------------------------------------------------------------------------------------
	InitSysCtrl();

#if FLASH_LOAD == 1
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash ();
#endif

   EALLOW;
   SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 1;  // eQEP2
   EDIS;

	//============================================================================================
	// Step 3. 인터럽트 초기화:
	//--------------------------------------------------------------------------------------------
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	// Vector Re-mapping
	EALLOW;
	PieVectTable.TINT0 = &Isr_CpuTimer0;
	PieVectTable.SCIRXINTB = &Isr_Scib_RxFifo;
	//PieVectTable.ADCINT = &Isr_Adc;
	//PieVectTable.TINT0 = &Isr_CpuTimer0;
	/*
#if EQEP_1
	PieVectTable.EQEP1_INT = &Isr_EQep_UnitTimeOut;
#else
	PieVectTable.EQEP2_INT = &Isr_EQep_UnitTimeOut;
#endif
	*/
	EDIS;

	/* Enable PIE group 5 interrupt 1 for EQEP1_INT */
    //PieCtrlRegs.PIEIER5.bit.INTx1 = 1;
    //PieCtrlRegs.PIEIER5.bit.INTx2 = 1;

	/* Enable CPU INT5 for EQEP1_INT */
	//IER |= M_INT5;

	//============================================================================================
	// Step 4. LED setting
	//--------------------------------------------------------------------------------------------
	/*
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;  //  0~15 port use to GPIO
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;  // 16~31 port use to GPIO
	GpioCtrlRegs.GPADIR.all = 0x000085CA;   // GPIO 1,3,6,7,8,10,15 outputs
   	GpioDataRegs.GPASET.all = 0x000005CA;	// GPIO 1,3,6,7,8,10 high (LED OFF)
	GpioDataRegs.GPACLEAR.all = 0x00008000;	// GPIO 15 low (U3100-74LVC4245A Enable)
   	EDIS;
   	*/

	//============================================================================================
	// Step 5. SCI / EQep setting
	//--------------------------------------------------------------------------------------------
	Init_CpuTimer ();
   	Init_Scib ();
   	Init_Gpio ();
   	//Init_ePWM ();
   	//InitPwmAdc ();
   	Init_Spib_280x ();
   	Init_Adc_280x ();

	/* Initialize Enhanced QEP Module 1 */
	//Init_EQep (ENCODER_REV);

   	/*
   	//============================================================================================
   	// Step 3. SPI 초기화
   	//--------------------------------------------------------------------------------------------
	// Initialize SPI-A PIN
	InitSpibGpio();
	EALLOW;
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;	// Configure GPIO19 as EEPROM_CS
	GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;		// Output, EEPROM_CS
	EDIS;
	GpioDataRegs.GPASET.bit.GPIO19 = 1;		// High, EEPROM_CS

	// Initialize Low Speed Clock Prescaler
//	EALLOW;
//	SysCtrlRegs.LOSPCP.all = 0x0000;		// LOSCLK = 100MHZ
//	EDIS;

	// Initialize internal SPI peripheral module
	SpibRegs.SPICCR.bit.SPISWRESET=0;				// Reset SCI
	SpibRegs.SPICCR.bit.SPICHAR = 15;				// 15bit 데이터 크기 사용
	SpibRegs.SPICCR.bit.CLKPOLARITY = 0;			// 클럭의 상승엣지에 데이터 출력
	SpibRegs.SPICTL.bit.CLK_PHASE = 1;				// Half-cycle 만큼 클럭지연
	SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;			// 마스터 모드
	SpibRegs.SPICTL.bit.TALK = 1;					// 전송 기능을 활성화
	SpibRegs.SPIBRR = 9;							// 10Mbps
	SpibRegs.SPICCR.bit.SPISWRESET=1;				// Enable SCI
   	//============================================================================================
	*/


   	/*
   	//============================================================================================
    InitAdc();  // For this example, init the ADC

#define ADC_CKPS   0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 12.5MHz/(1*2)   = 6.25MHz
                         //    for 60 MHz devices: ADC module clk = 7.5MHz/(1*2)     = 3.75MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                          = 16 ADC clocks

    // Specific ADC setup for this example:
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x7;  // convert and store in 8 results registers
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;
    AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x7;
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run

    DELAY_US (100);

    // Start SEQ1
    AdcRegs.ADCTRL2.all = 0x2000;
    //============================================================================================
	*/


	//============================================================================================
	// Enable global Interrupts and higher priority real-time debug events:
	//--------------------------------------------------------------------------------------------
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM

	//============================================================================================
	// IDLE loop. Just sit and loop forever :
	//--------------------------------------------------------------------------------------------
	Main_Loop ();
}
