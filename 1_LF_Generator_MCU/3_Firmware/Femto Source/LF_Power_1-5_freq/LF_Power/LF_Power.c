#include "LF_Power.h"
#include "DSP280x_Device.h"     // Headerfile Include File
#include "DSP280x_Examples.h"   // Examples Include File
#include "PwmAdc.h"
#include "Global.h"
#include "Flash280x_API_Library.h"
#include "Example_Flash280x_API.h"
#include <math.h>

#define B_NUMBER	2560

#if FLASH_LOAD == 1
#pragma CODE_SECTION (Main_Loop, "ramfuncs");
#pragma CODE_SECTION (Isr_Scib_RxFifo, "ramfuncs");
#pragma CODE_SECTION (Isr_Adc, "ramfuncs");
#pragma CODE_SECTION (Isr_CpuTimer0, "ramfuncs");

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
#endif

/* RS232 communication. */
#define RX_BUF_SIZE				64

Uint16 gRxBuf[RX_BUF_SIZE];
int32 gnRxBufPos = 0;

Uint16 gRxCmdBuf[RX_BUF_SIZE];
Uint32 gisRxCmd = FALSE;
int32 gnRxCmdSize = 0;

int32 gnRX_SV = 0;

/* Power. */
#define POWER_VOLTAGE				710		// Unit: 1V

#define POWER_MIN_SV				0		// Unit: 0.1W
#define POWER_MIN_SV_LIMIT			100		// Unit: 0.1W
#define POWER_MAX_SV				3000	// Unit: 0.1W

#define POWER_DEF_MV				10		// Burst 1%

int32 gnPower_SV = POWER_MIN_SV;	// Unit: 0.1W
int32 gnPower_PV = POWER_MIN_SV;	// Unit: 0.1W
int32 gnPower_MV = POWER_DEF_MV;	// Pulse & Burst width.

int16 gisPower_On = FALSE;
int16 gisPower_MinLimit_On = TRUE;
int32 gnPower_ChangeTime = 0;

//int32 gnPowerOn_TimeSec = 0;

/* Mode. */
#define OP_MODE_MANUAL				0
#define OP_MODE_AUTO				1
#define OP_MODE_AUTO_MONITORING		2
#define OP_MODE_AUTO_ADC			3

int16 gnOpMode = OP_MODE_AUTO_ADC;

/* Monitoring & Inter-lock. */
int32 gnInterlock_OverCurrent = 0;
int32 gnSCI_RxErrorCnt = 0;

/* Power control from adc. */
#define POWER_ADC_SV_DATA_LEN 			50
static int32 gnPower_AdcSVDataIndex = 0;
static int16 gnPower_AdcSVData[POWER_ADC_SV_DATA_LEN] = {0, };


/* Etc. */
int32 gnMonitoringTime = 0;
int32 gnStartTimeVer = 0;
int32 gnStartTimeLC = 0;

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

		if (gRxBuf[0] == (Uint16)'{' && rx_data == (Uint16)'}')
		{
			memcpy (gRxCmdBuf, gRxBuf, gnRxBufPos);
			gisRxCmd = TRUE;
			gnRxCmdSize = gnRxBufPos;
			gnRxBufPos = 0;
		}
	}

	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;			// Clear Overflow flag
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;			// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;			// Acknowledge interrupt to PIE
}


void Init_ScibGpio()
{
   EALLOW;
	
   /* Enable internal pull-up for the selected pins */
   // Pull-ups can be enabled or disabled disabled by the user.
   // This will enable the pullups for the specified pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;     // Enable pull-up for GPIO9  (SCITXDB)
   //GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
   //GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	   // Enable pull-up for GPIO18 (SCITXDB)
   //GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;    // Enable pull-up for GPIO22 (SCITXDB)

	
   GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;    // Enable pull-up for GPIO11 (SCIRXDB)
   //GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)
   //GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	   // Enable pull-up for GPIO19 (SCIRXDB)
   //GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;    // Enable pull-up for GPIO23 (SCIRXDB)

   /* Set qualification for selected pins to asynch only */
   // This will select asynch (no qualification) for the selected pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
   //GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)
   //GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)
   //GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)

   /* Configure SCI-B pins using GPIO regs*/
   // This specifies which of the possible GPIO pins will be SCI functional pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;    // Configure GPIO9 for SCITXDB operation
   //GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation
   //GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;   // Configure GPIO18 for SCITXDB operation
   //GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;   // Configure GPIO22 for SCITXDB operation
	
   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;   // Configure GPIO11 for SCIRXDB operation
   //GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation
   //GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   // Configure GPIO19 for SCIRXDB operation
   //GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;   // Configure GPIO23 for SCIRXDB operation

    EDIS;
}


void Init_Gpio()
{
   EALLOW;

   GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;  // Enable pullup on GPIO27
   GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;  // GPIO27 = input

   //GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;  // Enable pullup on GPIO15
   //GpioCtrlRegs.GPAMUX1.bit.GPIO15= 0; // GPIO15 = GPIO15
   //GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;  // GPIO15 = input

    EDIS;
}


Uint16 Scib_Tx(Uint16 *Data, Uint16 num_byte)
{
	Uint16 i;

	for(i = 0 ; i < num_byte; i++)
	{
		while (ScibRegs.SCIFFTX.bit.TXFFST > 8);

		ScibRegs.SCITXBUF = *(Data + i) & 0x00FF;
	}

	return 1;
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


Uint32 Code64_To_Int (char *data, Uint16 start, Uint16 len)
{
	Uint32 ret = 0;
	Uint32 mul = 0;
	Uint16 idx = 0;
	Uint16 i = 0;
	
	if (len <= 0 || len > 4) return 0;

	for (i = 0; i < len; i++)
	{
		mul = (Uint32) pow (64.0, (double)i);
		idx = start + len - i - 1;
		ret += (data[idx] - '0') * mul;
	}

	return ret;
}

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

void Int_To_Code64 (char *data, Uint16 start, Uint16 len, Uint32 val)
{
	Uint16 code64[4] = {0, };
	Uint16 i = 0;

	if (len <= 0 || len > 4) return;

	code64[0] = val % 64;
	code64[1] = (val / 64) % 64;
	code64[2] = (val / 4096) % 64;
	code64[3] = (val / 262144) % 64;

	for (i = 0; i < len; i++)
	{
		data[start + len - 1 - i] = '0' + (char)code64[i];
	}
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


char CalCheckSum_Base64 (char *data, Uint16 start, Uint16 len)
{
	int16 i = 0;
	int32 ret = 0;

	for (i = 0; i < len; i++)
	{
		ret += data[start + i];
	}

#if (CTRL_FREQ == TRUE)
	return '0' + (char)(ret % 63);
#else
	return '0' + (char)(ret % 64);
#endif
}


void Tx_Cmd (char *data, Uint16 len)
{
	char buf[RX_BUF_SIZE];

	memcpy (buf, data, len);

	buf[len] = buf[len - 1];
	buf[len - 1] = CalCheckSum_Base64 (buf, 1, len - 2);
	len++;

	Scib_Tx_Byte (buf, len);
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

#if (PID_CONTROL == TRUE)

#define POWER_PID_Kp				0.1

#define POWER_MIN_MV				0
#define POWER_MAX_MV_BURST			2000
#define POWER_MAX_MV				3000

#define POWER_MIN_PULSE_NUM			2


void RunPowerPID ()
{
	if (gnPower_SV <= POWER_MIN_SV)
	{
		gnPower_MV = POWER_DEF_MV;
		ResetPwm ();
		return;
	}

	int32 diff = gnPower_SV - gnPower_PV;
	double mv = (double)diff * POWER_PID_Kp;

	gnPower_MV = (int32)(gnPower_MV + mv);

	if (gnPower_MV < POWER_MIN_MV) gnPower_MV = POWER_MIN_MV;
	if (gnPower_MV > POWER_MAX_MV) gnPower_MV = POWER_MAX_MV;

	double mv_ratio = 0;
	int32 pwm_freq = 0;
	int32 pwm_width = 0;
	int32 burst_width = 0;
	int32 burst_freq = 0;

	GetPwm (&pwm_freq, NULL, &burst_freq, NULL);

#define PPID_LOGIC	1
#if (PPID_LOGIC == 1)

	if (gnPower_MV < POWER_MAX_MV_BURST)
	{
		mv_ratio = (double)(gnPower_MV - POWER_MIN_MV) / (POWER_MAX_MV_BURST - POWER_MIN_MV);
		pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;

		burst_width = PAR_MIN_PWM_BURST_WIDTH + (int32)((PAR_MAX_PWM_BURST_WIDTH - PAR_MIN_PWM_BURST_WIDTH) * mv_ratio);
		if (burst_width < PAR_MIN_PWM_BURST_WIDTH) burst_width = PAR_MIN_PWM_BURST_WIDTH;

		int burst_width_min = (int32)PAR_MAX_PWM_BURST_WIDTH * burst_freq * POWER_MIN_PULSE_NUM / (pwm_freq * 100);
		if (burst_width < burst_width_min) burst_width = burst_width_min;
	}
	else
	{
		mv_ratio = (double)(gnPower_MV - POWER_MAX_MV_BURST) / (POWER_MAX_MV - POWER_MAX_MV_BURST);
		pwm_width = PAR_MIN_PWM_WIDTH_MIDDLE + (int32)((PAR_MAX_PWM_WIDTH_LIMIT - PAR_MIN_PWM_WIDTH_MIDDLE) * mv_ratio);

		if (pwm_width < PAR_MIN_PWM_WIDTH_LIMIT) pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
		if (pwm_width > PAR_MAX_PWM_WIDTH_LIMIT) pwm_width = PAR_MAX_PWM_WIDTH_LIMIT;

		burst_width = PAR_MAX_PWM_BURST_WIDTH;
	}

#elif (PPID_LOGIC == 2)

#define POWER_MAX_MV_PULSE_RATIO	0.2
#define POWER_MAX_BURST_PULSE_WIDTH	500		// Unit: 0.1%

	if (gnPower_MV < POWER_MAX_MV_BURST)
	{
		mv_ratio = (double)(gnPower_MV - POWER_MIN_MV) / (double)(POWER_MAX_MV_BURST - POWER_MIN_MV);

		if (mv_ratio < POWER_MAX_MV_PULSE_RATIO)
		{
			pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
		}
		else
		{
			pwm_width = PAR_MIN_PWM_WIDTH_LIMIT +
						(int32)((POWER_MAX_BURST_PULSE_WIDTH - PAR_MIN_PWM_WIDTH_LIMIT) * (mv_ratio - POWER_MAX_MV_PULSE_RATIO) / (1 - POWER_MAX_MV_PULSE_RATIO));

			if (pwm_width < PAR_MIN_PWM_WIDTH_LIMIT) pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
			if (pwm_width > PAR_MAX_PWM_WIDTH_LIMIT) pwm_width = PAR_MAX_PWM_WIDTH_LIMIT;
		}

		burst_width = PAR_MIN_PWM_BURST_WIDTH + (int32)((PAR_MAX_PWM_BURST_WIDTH - PAR_MIN_PWM_BURST_WIDTH) * mv_ratio);
		if (burst_width < PAR_MIN_PWM_BURST_WIDTH) burst_width = PAR_MIN_PWM_BURST_WIDTH;

		int burst_width_min = (int32)PAR_MAX_PWM_BURST_WIDTH * burst_freq * POWER_MIN_PULSE_NUM / (pwm_freq * 100);
		if (burst_width < burst_width_min) burst_width = burst_width_min;
	}
	else
	{
		mv_ratio = (double)(gnPower_MV - POWER_MAX_MV_BURST) / (double)(POWER_MAX_MV - POWER_MAX_MV_BURST);
		pwm_width = POWER_MAX_BURST_PULSE_WIDTH + (int32)((PAR_MAX_PWM_WIDTH_LIMIT - POWER_MAX_BURST_PULSE_WIDTH) * mv_ratio);

		if (pwm_width < PAR_MIN_PWM_WIDTH_LIMIT) pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
		if (pwm_width > PAR_MAX_PWM_WIDTH_LIMIT) pwm_width = PAR_MAX_PWM_WIDTH_LIMIT;

		burst_width = PAR_MAX_PWM_BURST_WIDTH;
	}
#endif

	SetPwm (-1, pwm_width, burst_freq, burst_width);
}

#else

#define BURST_CONTROL_RATIO			10	// 1%
#define PWM_CONTROL_RATIO			10	// 1%
#define POWER_MAX_PWM_WIDTH_LIMIT	500 // 0.1%

void RunPowerControl ()
{
	static int32 control_count = 0;
	static int32 pwm_width_extra = 0;

	if (gnPower_SV <= POWER_MIN_SV)
	{
		ResetPwm_Freq (0);
		gnPower_PV = 0;
		return;
	}

	// Get PV & PWM.
	float64 ec = GetEC_OneCycle ();
	if (ec < 0) return;

	control_count++;

	int32 pwm_width;
	int32 burst_width;
	GetPwm (NULL, &pwm_width, NULL, &burst_width);

	// Control PWM.
	int32 control_ratio;
	int32 fine_tuning_range;

	ec = ec * (float64)burst_width / (float64)PAR_MAX_PWM_BURST_WIDTH;
	gnPower_PV = (int32)(POWER_VOLTAGE * ec * 10.0);

	if (burst_width < PAR_MAX_PWM_BURST_WIDTH)
	{
		control_ratio = BURST_CONTROL_RATIO;
		fine_tuning_range = 50;

		//pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
		pwm_width = gnPwm_WidthMinLimit;

		if (gnPower_PV < gnPower_SV)
		{
			if ((gnPower_SV - gnPower_PV) < fine_tuning_range / 2) control_ratio = 1;
			else if ((gnPower_SV - gnPower_PV) < fine_tuning_range) control_ratio = 2;

			burst_width += control_ratio;
			if (burst_width > PAR_MAX_PWM_BURST_WIDTH) burst_width = PAR_MAX_PWM_BURST_WIDTH;
		}
		else if (gnPower_PV > gnPower_SV)
		{
			if ((gnPower_PV - gnPower_SV) < fine_tuning_range / 2) control_ratio = 1;
			else if ((gnPower_PV - gnPower_SV) < fine_tuning_range) control_ratio = 2;

			burst_width -= control_ratio;
			if (burst_width < PAR_MIN_PWM_BURST_WIDTH) burst_width = PAR_MIN_PWM_BURST_WIDTH;
		}

		SetPwm (-1, pwm_width, -1, burst_width);
	}
	else
	{
		if (control_count % 5 == 0)
		{
#if (CTRL_FREQ == TRUE)
			control_ratio = PWM_CONTROL_RATIO + 1;
			fine_tuning_range = 48;
#else
			control_ratio = PWM_CONTROL_RATIO;
			fine_tuning_range = 50;
#endif
			if (gnPower_PV < gnPower_SV)
			{
				if ((gnPower_SV - gnPower_PV) < fine_tuning_range / 2) control_ratio = 2;
				else if ((gnPower_SV - gnPower_PV) < fine_tuning_range) control_ratio = 5;

				//pwm_width += control_ratio;
				pwm_width_extra += control_ratio;
				if (pwm_width_extra > 10)
				{
					pwm_width += 1;
					pwm_width_extra = 0;
				}

				if (pwm_width > POWER_MAX_PWM_WIDTH_LIMIT) pwm_width = POWER_MAX_PWM_WIDTH_LIMIT;
			}
			else if (gnPower_PV > gnPower_SV)
			{
				if ((gnPower_PV - gnPower_SV) < fine_tuning_range / 2) control_ratio = 2;
				else if ((gnPower_PV - gnPower_SV) < fine_tuning_range) control_ratio = 5;

				//pwm_width -= control_ratio;
				pwm_width_extra -= control_ratio;
				if (pwm_width_extra < 10)
				{
					pwm_width -= 1;
					pwm_width_extra = 0;
				}

				//if (pwm_width < PAR_MIN_PWM_WIDTH_LIMIT)
				if (pwm_width < gnPwm_WidthMinLimit)
				{
					//pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
					pwm_width = gnPwm_WidthMinLimit;
					burst_width = PAR_MAX_PWM_BURST_WIDTH - BURST_CONTROL_RATIO;
				}
			}

			SetPwm (-1, pwm_width, -1, burst_width);
		}

		/*
		static float64 pwm_width_mv = 0;

		if (control_count % 5 == 0)
		{
			if (pwm_width_mv - pwm_width < -10 || pwm_width_mv -pwm_width > 10)
			{
				pwm_width_mv = pwm_width;
			}

			int32 div_val = 10 + (gnTimer_10us - gnPower_ChangeTime) / 100000;
			if (div_val > 20) div_val = 20;

			float64 change_value = (float64)(gnPower_SV - gnPower_PV) / (float64)div_val;
			pwm_width_mv += change_value;

			if (pwm_width < PAR_MIN_PWM_WIDTH_LIMIT)
			{
				pwm_width = PAR_MIN_PWM_WIDTH_LIMIT;
				burst_width = PAR_MAX_PWM_BURST_WIDTH - 1;
			}

			SetPwm (-1, (int32)pwm_width_mv, -1, burst_width);
		}
		*/
	}
}

#endif


void TurnPowerOn (int16 on_off)
{
	if (on_off == TRUE)
	{
		if (!gisPower_On)
		{
			gisPower_On = TRUE;
			ResetECData ();
			gnPower_SV = gnRX_SV;
			gnPower_ChangeTime = gnTimer_10us;

			//gnPowerOn_TimeSec = gnTimer_10us / 100000L;
		}
	}
	else
	{
		gisPower_On = FALSE;
		gnPower_SV = 0;

		/*
		if (gnPowerOn_TimeSec > 0)
		{
			g_nGenOnCount++;
			g_nGenTimeSec += ((gnTimer_10us / 100000L) - gnPowerOn_TimeSec);

			gnPowerOn_TimeSec = -1;

			DINT;
			Ext_Write_Flash (1);
			EINT;
		}
		*/
	}
}


void SetPower (int32 sv)
{
	if (sv < POWER_MIN_SV) sv = POWER_MIN_SV;
	if (sv > POWER_MAX_SV) sv = POWER_MAX_SV;

	if (sv > POWER_MIN_SV && sv <= 100)	// set power-on minimum value.
	{
		sv = 100 * (sv - 5) / 95;
		if (sv <= 0) sv = 1;
	}

	if (gisPower_MinLimit_On && sv < POWER_MIN_SV_LIMIT) sv = 0;

	gnRX_SV = sv;

	if (gisPower_On == TRUE)
	{
#if (PID_CONTROL == TRUE)
		if (sv <= POWER_MIN_SV || sv != gnPower_SV)
		{
			ResetECData ();
		}
#endif
		gnPower_SV = sv;
		gnPower_ChangeTime = gnTimer_10us;
	}
}

void Main_Loop ()
{
	char rx_cmd_buf [RX_BUF_SIZE];
	char tx_cmd_buf [RX_BUF_SIZE];
	int16 rx_cmd_size = 0;
	int16 rx_val_size = 0;

	int32 freq = 0;
	int32 width = 0;

	int32 i = 0;
	int32 num = 0;
	int32 loop_count = 0;
	//int32 ec_count = 0;
	int32 test_count = 0;

	char check_sum = 0;
	int32 run_pid_time = 0;
	int32 adc_check_time = 0;

	int32 test_cnt_10us = 0;


	/* Init. */
	ResetPwm ();

	while (TRUE)
	{
		/* SCI command. */
		if (gisRxCmd)
		{
			gisRxCmd = FALSE;

			rx_cmd_size = gnRxCmdSize;
			rx_val_size = rx_cmd_size - 4;
			for (i = 0; i < rx_cmd_size; i++)
			{
				rx_cmd_buf[i] = (char)gRxCmdBuf[i];
			}

			/* CheckSum. */
			check_sum = CalCheckSum_Base64 (rx_cmd_buf, 1, rx_cmd_size - 3);
			if (check_sum != rx_cmd_buf[rx_cmd_size - 2])
			{
				Tx_Cmd ("{E1}", 4);
				continue;
			}

			/* B-Number. */
			if (rx_cmd_buf[1] == 'V' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'V';

				Int_To_Code10 (tx_cmd_buf, num, 4, B_NUMBER);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);

				DINT;
				g_nGenOnCount++;
				g_nRndKey = (Uint16)(gnStartTimeVer % 30000);
				Ext_Write_Flash (1);
				EINT;

				/*
				DINT;
				FLASH_ST FlashStatus;
				Flash_Erase ((SECTORB | SECTORC), &FlashStatus);
				EINT;
				*/
			}

			/* Auto mode. */
			else if (rx_cmd_buf[1] == 'A' && rx_val_size == 1)
			{
				// ON.
				if (rx_cmd_buf[2] == '1' || rx_cmd_buf[2] == '2')
				{
					Tx_Cmd ("{A}", 3);

					if (rx_cmd_buf[2] == '2') gnOpMode = OP_MODE_AUTO_MONITORING;
					else if (rx_cmd_buf[2] == '1') gnOpMode = OP_MODE_AUTO;

					gnPower_SV = 0;
					ResetPwm ();
				}

				// OFF.
				else if (rx_cmd_buf[2] == '0')
				{
					Tx_Cmd ("{A}", 3);
					gnOpMode = OP_MODE_MANUAL;
					ResetPwm ();
				}

				else
				{
					Tx_Cmd ("{E0}", 4);
				}
			}

			/* Power. */
			else if ((gnOpMode == OP_MODE_AUTO || gnOpMode == OP_MODE_AUTO_MONITORING) && rx_cmd_buf[1] == 'P')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'P';

					Int_To_Code10 (tx_cmd_buf, num, 4, (int32)gnRX_SV);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{P}", 3);
					int32 sv = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					SetPower (sv);
				}
			}

			/* Turn power on. */
			else if (rx_cmd_buf[1] == 'T' && rx_val_size == 1)
			{
				// ON.
				if (rx_cmd_buf[2] == '1')
				{
					Tx_Cmd ("{T}", 3);
					TurnPowerOn (TRUE);
				}

				// OFF.
				else if (rx_cmd_buf[2] == '0')
				{
					Tx_Cmd ("{T}", 3);
					TurnPowerOn (FALSE);
				}

				else
				{
					Tx_Cmd ("{E0}", 4);
				}
			}

			/* PWM Frequency. */
#if (CTRL_FREQ == TRUE)
			else if (rx_cmd_buf[1] == 'F')
#else
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'F')
#endif
			//else if ((gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'F') ||
			//		 (rx_cmd_buf[1] == 'F' &&  CTRL_FREQ == TRUE))
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'F';

					GetPwm (&freq, NULL, NULL, NULL);
					Int_To_Code10 (tx_cmd_buf, num, 4, freq);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					freq = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (gnOpMode == OP_MODE_MANUAL)
					{
						SetPwm (freq, -1, -1, -1);
					}
					else
					{
						if (freq == 200 || freq == 400 || freq == 600 || freq == 800 || freq == 1000 ||
							freq == 300 || freq == 500 || freq == 700 || freq == 900)
						{
							SetPwm (freq, -1, -1, -1);

							if (freq == 200)
							{
								gnPwm_WidthMinLimit = 180;
								gnPwm_ErrRange = 135;
							}
							else if (freq == 300)
							{
								gnPwm_WidthMinLimit = 180;
								gnPwm_ErrRange = 135;
							}
							else if (freq == 400)
							{
								gnPwm_WidthMinLimit = 200;
								gnPwm_ErrRange = 150;
							}
							else if (freq == 500)
							{
								gnPwm_WidthMinLimit = 200;
								gnPwm_ErrRange = 150;
							}
							else if (freq == 600)
							{
								gnPwm_WidthMinLimit = 240;
								gnPwm_ErrRange = 180;
							}
							else if (freq == 700)
							{
								gnPwm_WidthMinLimit = 280;
								gnPwm_ErrRange = 210;
							}
							else if (freq == 800)
							{
								gnPwm_WidthMinLimit = 320;
								gnPwm_ErrRange = 240;
							}
							else if (freq == 900)
							{
								gnPwm_WidthMinLimit = 360;
								gnPwm_ErrRange = 270;
							}
							else if (freq == 1000)
							{
								gnPwm_WidthMinLimit = 400;
								gnPwm_ErrRange = 300;
							}
						}
					}

					Tx_Cmd ("{F}", 3);
				}
			}

			/* PWM Width. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'W')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'W';

					GetPwm (NULL, &width, NULL, NULL);
					Int_To_Code10 (tx_cmd_buf, num, 4, width);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{W}", 3);
					width = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);
					SetPwm (-1, width, -1, -1);
				}
			}

			/* PWM Burst Freq. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'U')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'U';

					GetPwm (NULL, NULL, &freq, NULL);
					Int_To_Code10 (tx_cmd_buf, num, 4, freq);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{U}", 3);
					freq = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);
					SetPwm (-1, -1, freq, -1);
				}
			}

			/* PWM Burst Width. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'B')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'B';

					GetPwm (NULL, NULL, NULL, &width);
					Int_To_Code10 (tx_cmd_buf, num, 4, width);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{B}", 3);
					width = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);
					SetPwm (-1, -1, -1, width);
				}
			}

			/* Monitoring. */
			else if (rx_cmd_buf[1] == 'M' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'M';

				// Over current.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'O';
				if (gnInterlock_OverCurrent > 0)
				{
					tx_cmd_buf[num++] = '1';
				}
				else
				{
					tx_cmd_buf[num++] = '0';
				}

				// RX error count.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'R';
				Int_To_Code10 (tx_cmd_buf, num, 4, gnSCI_RxErrorCnt);
				num += 4;

				// PV.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'W';
				Int_To_Code10 (tx_cmd_buf, num, 4, gnPower_PV);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);

				// check time.
				gnMonitoringTime = gnTimer_10us;
			}

			/* Reset. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'R' && rx_val_size == 0)
			{
				Tx_Cmd ("{R}", 3);
				ResetPwm ();
			}

			/* Current. */
			else if (rx_cmd_buf[1] == 'C' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'C';

#if (PID_CONTROL == TRUE)
				double ec = GetEC ();
#else
				int32 burst_width;
				GetPwm (NULL, NULL, NULL, &burst_width);

				float64 ec = GetEC_OneCycle ();
				ec = ec * (float64)burst_width / (float64)PAR_MAX_PWM_BURST_WIDTH;
#endif
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(ec * 1000));
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Test. */
			else if (rx_cmd_buf[1] == 'Z' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'Z';

				// ADC-2
				AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 1;
				/*
				int i;
				for (i = 0; i < 10000; i++);

				AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 0;
				*/

				/*
				int32 val1 = AdcRegs.ADCRESULT0 >> 4;
				int32 val2 = AdcRegs.ADCRESULT8 >> 4;

				Int_To_Code10 (tx_cmd_buf, num, 4, val1);
				num += 4;

				tx_cmd_buf[num++] = ',';

				Int_To_Code10 (tx_cmd_buf, num, 4, val2);
				num += 4;
				*/

				// GPIO
				int16 g27 = GpioDataRegs.GPADAT.bit.GPIO27;
				int16 g15 = GpioDataRegs.GPADAT.bit.GPIO15;

				tx_cmd_buf[num++] = ',';

				Int_To_Code10 (tx_cmd_buf, num, 4, g27);
				num += 4;

				tx_cmd_buf[num++] = ',';

				Int_To_Code10 (tx_cmd_buf, num, 4, g15);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Error. */
			else
			{
				Tx_Cmd ("{E0}", 4);
			}
		}

#if (PID_CONTROL == TRUE)
		/* PID. */
		if ((gnTimer_10us > run_pid_time + 1000 || gnTimer_10us < run_pid_time) &&
			(gnOpMode == OP_MODE_AUTO || gnOpMode == OP_MODE_AUTO_MONITORING))
		{
			run_pid_time = gnTimer_10us;

			double ec = GetEC ();
			ec_count++;

			if (ec_count >= 5)
			{
				gnPower_PV = (int32)(POWER_VOLTAGE * ec * 10);
				RunPowerPID ();
				ec_count = 0;
			}
		}
#else
		/* Power control. */
		CalEC_OneCycle ();

		if ((gnTimer_10us > run_pid_time + 2000 || gnTimer_10us < run_pid_time) &&
			(gnOpMode == OP_MODE_AUTO || gnOpMode == OP_MODE_AUTO_MONITORING))
		{
			run_pid_time = gnTimer_10us;
			RunPowerControl ();
		}
#endif

		/* RX error check. */
		if (ScibRegs.SCIRXST.bit.RXERROR == 0x01)
		{
			gnSCI_RxErrorCnt++;

			Init_Scib ();

			for (i = 0; i < 1000; i++)
			{
				Sleep10us (100);
				if (ScibRegs.SCIRXST.bit.RXERROR == 0x00) break;
			}
		}

		/* Monitoring check. */
		if (gnOpMode == OP_MODE_AUTO_MONITORING)
		{
			if (gisPower_On)
			{
				int32 delay_time = 0;
				if (gnTimer_10us >= gnMonitoringTime)
				{
					delay_time = gnTimer_10us - gnMonitoringTime;
				}
				else
				{
					delay_time = TIMER_10US_MAX - gnMonitoringTime + gnTimer_10us;
				}

				if (delay_time > 500000)	// 5sec.
				{
					TurnPowerOn (FALSE);
				}
			}
			else
			{
				gnMonitoringTime = gnTimer_10us;
			}
		}

		/* ADC to Power. */
		#define ADC_SV_CHANGE_LIMIT		70

		if (gnOpMode == OP_MODE_AUTO_ADC &&
			((gnTimer_10us - adc_check_time) > 1000 || gnTimer_10us < adc_check_time))
		{
			static int16 off_count = 0;
			int16 g27 = GpioDataRegs.GPADAT.bit.GPIO27;

			if (g27 == 0)
			{
				TurnPowerOn (TRUE);
				off_count = 0;
			}
			else
			{
				off_count++;
				if (off_count > 10)
				{
					TurnPowerOn (FALSE);
					off_count = 10;
				}
			}

			if (gisPower_On)
			{
				AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 1;

				int32 val = AdcRegs.ADCRESULT8 >> 4;
				int32 sv = POWER_MAX_SV * val / 4096;

				if (sv > 2000) sv = 2000;

				// SV avr.
				int32 sv_count = gnPower_AdcSVDataIndex;
				int32 sv_sum = 0;
				int16 sv_avr = 0;

				if (sv_count > POWER_ADC_SV_DATA_LEN) sv_count = POWER_ADC_SV_DATA_LEN;

				for (i = 0; i < sv_count; i++)
				{
					sv_sum += gnPower_AdcSVData [i];
				}

				sv_avr = (int16)(sv_sum / sv_count);
				if (sv > sv_avr + ADC_SV_CHANGE_LIMIT) sv = sv_avr + ADC_SV_CHANGE_LIMIT;
				else if (sv < sv_avr - ADC_SV_CHANGE_LIMIT) sv = sv_avr - ADC_SV_CHANGE_LIMIT;

				gnPower_AdcSVData [gnPower_AdcSVDataIndex % POWER_ADC_SV_DATA_LEN] = sv;
				gnPower_AdcSVDataIndex++;

				if (sv_avr >= POWER_MIN_SV_LIMIT)
					//SetPwm (-1, PAR_MIN_PWM_WIDTH_LIMIT, -1, sv_avr / 2);
					SetPwm (-1, gnPwm_WidthMinLimit, -1, sv_avr / 2);
				else SetPwm (-1, 0, -1, 0);
			}
			else
			{
				gnPower_AdcSVData [gnPower_AdcSVDataIndex % POWER_ADC_SV_DATA_LEN] = 0;
				gnPower_AdcSVDataIndex++;

				SetPwm (-1, 0, -1, 0);
			}

			adc_check_time = gnTimer_10us;
		}

		/* ADC. */
		/*
		{
			adc_ch0_val = AdcRegs.ADCRESULT0 >> 4;
			adc_ch2_val = AdcRegs.ADCRESULT1 >> 4;

			// check over current.
			if (adc_ch0_val > MAX_ADC_CURRENT_LIMIT)
			{
				gnInterlock_OverCurrent++;
				gnRX_PwmWidth = 0;
				UpdatePWM ();
			}

			// check average current.
			adc_ch0_sum += adc_ch0_val;
			if (++adc_loop_count > 1000)
			{
				adc_ch0_average = adc_ch0_sum / 1000;
				adc_ch0_sum = 0;
				adc_loop_count = 0;
			}
		}
		*/

		/* PWM. */
		/*
		gnAdc_SetVal = AdcRegs.ADCRESULT0 >> 4;
		gnAdc_FBVal = AdcRegs.ADCRESULT1 >> 4;

		gnAdcGap = gnAdcFBVal - gnAdcSetVal;
		if (gnAdcGap < -200) // < -5%.
		{
			gnPwmWidthAdjust = gnPwmWidthAdjust + 10;
			if (gnPwmWidthAdjust > MAX_gnPwmWidth / 2) gnPwmWidthAdjust = MAX_gnPwmWidth / 2;
		}
		else if (gnAdcGap > 200) // > 5%.
		{
			gnPwmWidthAdjust = gnPwmWidthAdjust - 10;
			if (gnPwmWidthAdjust < -MAX_gnPwmWidth / 2) gnPwmWidthAdjust = -MAX_gnPwmWidth / 2;
		}
		else gnPwmWidthAdjust = 0;
		*/

		/*
		gnPwm_TBPRD = 100000000 / (gnRX_PwmFreq * 100 * 2);
		gnPwm_Width = gnPwm_TBPRD * gnRX_PwmWidth / RX_MAX_PWM_WIDTH;

		gnPwm_WidthAdjust = 0;
		gnPwm_Width = gnPwm_Width + gnPwm_WidthAdjust;

		if (gnPwm_Width < 0) gnPwm_Width = 0;
		if (gnPwm_Width > gnPwm_TBPRD * PWM_WIDTH_LIMIT) gnPwm_Width = gnPwm_TBPRD * PWM_WIDTH_LIMIT;

		EPwm1Regs.CMPA.half.CMPA = gnPwm_Width;
		EPwm1Regs.CMPB = gnPwm_TBPRD - gnPwm_Width;
		EPwm1Regs.TBPRD = gnPwm_TBPRD;			  	    // Set period for ePWM1 (PWM 캐리어 주파수 : 100MHz/(1000 * 2) = 50kHz)
		*/

		loop_count++;
	}
}

void main(void)
{
	Uint32 n;

	//============================================================================================
	// Step 1. Disable Global Interrupt
	//--------------------------------------------------------------------------------------------
	Ext_Init_Flash ();
	//for (n = 0; n < 1000000L; n++);
	//Ext_Write_Flash (1);

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

	//============================================================================================
	// Step 3. 인터럽트 초기화:
	//--------------------------------------------------------------------------------------------
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	// Vector Re-mapping
	EALLOW;
	PieVectTable.SCIRXINTB = &Isr_Scib_RxFifo;
	PieVectTable.ADCINT = &Isr_Adc;
	PieVectTable.TINT0 = &Isr_CpuTimer0;
	EDIS;

	//============================================================================================
	// Step 4. LED setting
	//--------------------------------------------------------------------------------------------
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;  //  0~15 port use to GPIO
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;  // 16~31 port use to GPIO
	GpioCtrlRegs.GPADIR.all = 0x000085CA;   // GPIO 1,3,6,7,8,10,15 outputs
   	GpioDataRegs.GPASET.all = 0x000005CA;	// GPIO 1,3,6,7,8,10 high (LED OFF)
	GpioDataRegs.GPACLEAR.all = 0x00008000;	// GPIO 15 low (U3100-74LVC4245A Enable)
   	EDIS;

	//============================================================================================
	// Step 5. SCI setting
	//--------------------------------------------------------------------------------------------
   	Init_Scib ();
   	Init_Gpio ();
   	InitPwmAdc ();

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

