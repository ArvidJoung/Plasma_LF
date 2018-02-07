/*
 * main.c
 */
#include "DSP2833x_Device.h"	/* DSP2833x Headerfile Include File */
#include "DSP2833x_Examples.h"	/* DSP2833x Examples Include File */
#include "pwm.h"
#include "global.h"
#include <math.h>

#define CL_NUMBER	2583

#if FLASH_LOAD == 1
#pragma CODE_SECTION (Main_Loop, "ramfuncs");
#pragma CODE_SECTION (Isr_Scia_RxFifo, "ramfuncs");
#pragma CODE_SECTION (Isr_Adc, "ramfuncs");
#pragma CODE_SECTION (Isr_CpuTimer0, "ramfuncs");

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
#endif

/*-----------------------------------------------------
 * RS232 communication. (UNIT : 0.1KHz, 0.1%, 1W)
 */
#define RX_BUF_SIZE				64

Uint16 gRxBuf[RX_BUF_SIZE];
int32 gnRxBufPos = 0;

Uint16 gRxCmdBuf[RX_BUF_SIZE];
Uint32 gisRxCmd = FALSE;
int32 gnRxCmdSize = 0;

int32 gnRX_PWM_Freq = DEF_PWM_FREQ;
int32 gnRX_PWM_Width = DEF_PWM_WIDTH;
int32 gnRX_Boost_Freq = DEF_BST_FREQ;
int32 gnRX_Boost_Width = DEF_BST_WIDTH;

int32 gnRX_SV = 0;	// Unit: 1W

/*-----------------------------------------------------
 * ACD.
 */
#define ADC_VOLTAGE_RES				300	// Voltage resolution.
#define ADC_VOLTAGE_AVR_RES			50	// Voltage average resolution.

int32 gnAdcA0_DataIndex = 0;
int32 gnAdcA0_Avr_DataIndex = 0;
int16 gnAdcA0_Data[ADC_VOLTAGE_RES] = {0, };
int16 gnAdcA0_Avr_Data[ADC_VOLTAGE_AVR_RES] = {0, };

#define ADC_EC_CURRENT_RES			500	// Current resolution.
#define ADC_EC_FREQ_MULTI			5
#define ADC_EC_LPF_FREQ				8	// Unit : sample.

int32 gnAdcA2_DataIndex = 0;
int16 gnAdcA2_Data[ADC_EC_CURRENT_RES] = {0, };
int16 gnAdcA2_Data_Ext[ADC_EC_CURRENT_RES] = {0, };
int16 gisAdcA2_Start = FALSE;

//int32 gnAdcB6_DataIndex = 0;
int16 gnAdcB6_Data[ADC_EC_CURRENT_RES] = {0, };
//int16 gnAdcB6_Data_Ext[ADC_EC_CURRENT_RES] = {0, };
//int16 gisAdcB6_Start = FALSE;


/*-----------------------------------------------------
 * EC.
 */
#define EC_BUF_SIZE			50 //100

int64 gnEC_Check_PreTime = 0;
double gfEC_OneCycle = 0;
double gfEC_OneCycle_HVO = 0;
double gfEC_Peak = 0;
double gfEC_Peak_Max = 0;

int32 gnEC_BufIndex = 0;
int16 gnEC_Buf[EC_BUF_SIZE] = {0, };
int16 gnEC_Buf_HVO[EC_BUF_SIZE] = {0, };

/*-----------------------------------------------------
 * Power.
 */
#define POWER_VOLTAGE				120		// Unit: 1V

#define POWER_MIN_SV				100		// Unit: 1W
#define POWER_MAX_SV_LIMIT			2000	// Unit: 1W
#define POWER_MAX_SV				2000	// Unit: 1W

#define POWER_MIN_MV				8		// Unit: 1% (Width)
#define POWER_MAX_MV				50		// Unit: 1% (Width)

double gfPower_SV = POWER_MIN_SV;	// Unit: 1W
double gfPower_PV = 0;				// Unit: 1W
double gfPower_MV = 0;				// Pulse width (%).

int16 gisPower_On = FALSE;
int16 gisPower_MinLimit_On = TRUE;
int32 gnPower_ChangeTime = 0;

int64 gnPower_Check_PreTime = 0;

/*-----------------------------------------------------
 * LED.
 */
int64 gnLED_Run = 0;
int64 gnLED_TXD = 0;
int64 gnLED_RXD = 0;
int64 gnLED_ALARM = 0;

/*-----------------------------------------------------
 * Mode.
 */
#define OP_MODE_MANUAL				0
#define OP_MODE_AUTO				1
//#define OP_MODE_AUTO_MONITORING		2
//#define OP_MODE_AUTO_ADC			3

int16 gnOpMode = OP_MODE_AUTO;

/*-----------------------------------------------------
 * Monitoring & Inter-lock.
 */
#define MAX_INTERLOCK_CURRENT_VAL	100

#define OVERCURRENT_MAX_COUNT		30	// Max. check count. (300msec)
int16 gnOvercurrent_Val = 25;			// Check point (Amp)
int32 gnOverCurrent_Count = 0;
int16 gisOverCurrent = FALSE;

#define ERSHORT_MAX_COUNT			30	// Max. check count. (300msec)
int16 gnER_Short_CurrentVal = 45;		// Check point (Amp)
int32 gnER_Short_Count = 0;				// Electrode Short.
int16 gisER_Short = FALSE;

#define VDROP_MAX_COUNT				30	// Max. check count.
#define VDROP_VAL					0.7	// Voltage drop check point. (ADC-1V => SMPS-100V)
int32 gnVDrop_Count = 0;
int16 gisVDrop = FALSE;
double gfVSource = 0;

int32 gnSCI_RxErrorCnt = 0;
int64 gnInterlock_Check_PreTime = 0;

/*-----------------------------------------------------
 * Etc.
 */
int32 gnMonitoringTime = 0;
int64 gnTimerUS = 0;

int16 gisDebug_BreakOn = FALSE;


void Init_CpuTimer ()
{
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 150, 10);
	StartCpuTimer0();

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	IER |= M_INT1;
}

int g_nBurstOn = 1;
int g_nBurstTimerCnt = 0;

interrupt void Isr_CpuTimer0 (void)
{
	gnTimerUS += 10;

	/*
	if (++g_nBurstTimerCnt >= 500)
	{
		g_nBurstTimerCnt = 0;
		g_nBurstOn = (g_nBurstOn == 1) ? 0 : 1;

		if (g_nBurstOn == 1)
		{
			SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, gnRX_PWM_Width / 10.0);
			SetPwm_2833x (6, TB_COUNT_UP, gnRX_PWM_Freq * 100 * ADC_EC_FREQ_MULTI, gnRX_PWM_Width / 10.0);
		}
		else
		{
			SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
			SetPwm_2833x (6, TB_COUNT_UP, gnRX_PWM_Freq * 100 * ADC_EC_FREQ_MULTI, 0);
		}
	}
	*/

	/*
	if (!g_isBurstOn && g_nTimerUS - g_nTimerUS_Start >= g_nBurstPeriodUS)
	{
		SetPwm_2833x (5, TB_COUNT_UPDOWN, 50000, 20);
		SetPwm_2833x (1, TB_COUNT_UP, 50000, 20);

		g_isBurstOn = 1;
		g_nTimerUS_Start = g_nTimerUS;
	}
	else if (g_isBurstOn && g_nTimerUS - g_nTimerUS_Start >= g_nBurstWidthUS)
	{
		SetPwm_2833x (5, TB_COUNT_UPDOWN, 50000, 0);
		SetPwm_2833x (1, TB_COUNT_UP, 50000, 0);

		g_isBurstOn = 0;
	}
	*/

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void Init_SciaGpio()
{
   EALLOW;

   /* Enable internal pull-up for the selected pins */
   // Pull-ups can be enabled or disabled disabled by the user.
   // This will enable the pullups for the specified pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;     // Enable pull-up for GPI29  (SCITXDA)
   GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)

   /* Set qualification for selected pins to asynch only */
   // This will select asynch (no qualification) for the selected pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)

   /* Configure SCI-B pins using GPIO regs*/
   // This specifies which of the possible GPIO pins will be SCI functional pins.
   // Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;    // Configure GPI29 for SCITXDA operation
   GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation

   EDIS;
}

void Init_Scia ()
{
	SciaRegs.SCICTL1.bit.SWRESET = 0;		// SCI 소프트웨어 리셋

	SciaRegs.SCICCR.bit.SCICHAR = 7;		// SCI 송수신 Charcter-length 설정 : 8bit
	//SciaRegs.SCICCR.bit.LOOPBKENA = 1;	// SCI 루프백 테스트 모드 Enable
	SciaRegs.SCICTL1.bit.RXENA = 1;			// SCI 송신기능 Enable
	SciaRegs.SCICTL1.bit.TXENA = 1;			// SCI 수신기능 Enable
	SciaRegs.SCIHBAUD = 0x00;      			// SCI Baudrate 설정
	SciaRegs.SCILBAUD = 0x79;				//  - [SCIHBAUD,SCILBAUD] => 0x0079 => 약38400bps

	// SCI의 송신 FIFO 설정
	SciaRegs.SCIFFTX.bit.SCIFFENA = 1;		// SCI FIFO 사용 설정 Enable
	SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;	// SCI 송신 FIFO 인터럽트 플래그 클리어
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;	// SCI 송신 FIFO RE-enable
	SciaRegs.SCIFFTX.bit.SCIRST = 1;		// SCI 리셋 해제

	// SCI의 수신 FIFO 설정
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;	// SCI 수신 FIFO 인터럽트 플래그 클리어
	SciaRegs.SCIFFRX.bit.RXFFIENA = 1;		// SCI 수신 FIFO 인터럽트 Enable
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;	// SCI 수신 FIFO RE-enable
	SciaRegs.SCIFFRX.bit.RXFFIL = 1;		// SCI 수신 FIFO 인터럽트 레벨 설정

	SciaRegs.SCICTL1.bit.SWRESET = 1;		// SCI 소프트웨어 리셋 해제

	PieCtrlRegs.PIEIER9.bit.INTx1=1;     	// PIE Group 9, int1
	IER |= M_INT9;						 	// Enable INT
}

interrupt void Isr_Scia_RxFifo (void)
{
	Uint16 i;
	Uint16 num_byte;
	Uint16 rx_data = 0;

	// 수신된 데이터의 바이트 수 확인
	num_byte = SciaRegs.SCIFFRX.bit.RXFFST;

	// FIFO에 수신된 데이터를 메모리에 저장
	for(i = 0; i < num_byte; i++)
	{
		rx_data = SciaRegs.SCIRXBUF.bit.RXDT;

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

	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // Clear Overflow flag
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // Clear Interrupt flag

	PieCtrlRegs.PIEACK.all |= 0x100;       // Issue PIE ack
}

void Init_ADC ()
{
	InitAdc();

	/*--------------------------
	 * ADC-A2
	 *--------------------------*/

	// ADC 설정
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 2;				// ADCCLKPS = [HSPCLK / ADCCLKPS *2] (5MHz = 100MHz / 20)
	AdcRegs.ADCTRL1.bit.CPS = 0;					// ADCCLK = ADCCLKPS / 1  (ADC의 구동 클럭을 결정 : ADCCLK = 5MHz)
	AdcRegs.ADCTRL1.bit.ACQ_PS = 2;					// 샘플/홀드 시간 설정 : ACQ_PS + 1 = 3 사이클
	//AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;				// 시퀀스 직렬 모드
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; 		// ePWM_SOCA로 ADC 시퀀스 시동
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;			// seq 시퀀스 완료시 인터럽트 발생 설정

	/*
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0;				// 변환 채널수 설정 = MAXCONV + 1
	AdcRegs.ADCMAXCONV.bit.MAX_CONV2 = 0;				// 변환 채널수 설정 = MAXCONV + 1

	//seq 변환 순서
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 2;			// 1번째 변환 : ADC_A0
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0;			// 2번째 변환 : ADC_A2
	*/

	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0x1; 			// Setup simultaneous sampling mode
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0x1; 			// Setup cascaded sequencer mode
	AdcRegs.ADCMAXCONV.all = 0x0002; 				// 8 double conv's (16 total)
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0; 			// Setup conv from ADCINA0 &amp; ADCINB0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x2; 			// Setup conv from ADCINA1 &amp; ADCINB1
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x6; 			// Setup conv from ADCINA1 &amp; ADCINB1

	/*--------------------------
	 * ADC-A0
	 *--------------------------*/

	/*
	// ADC 설정
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 2;				// ADCCLKPS = [HSPCLK / ADCCLKPS *2] (5MHz = 100MHz / 20)
	AdcRegs.ADCTRL1.bit.CPS = 0;					// ADCCLK = ADCCLKPS / 1  (ADC의 구동 클럭을 결정 : ADCCLK = 5MHz)
	AdcRegs.ADCTRL1.bit.ACQ_PS = 2;					// 샘플/홀드 시간 설정 : ACQ_PS + 1 = 3 사이클
	//AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;				// 시퀀스 직렬 모드
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; 		// ePWM_SOCA로 ADC 시퀀스 시동
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;			// seq 시퀀스 완료시 인터럽트 발생 설정

	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0x1; 			// Setup simultaneous sampling mode
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0x1; 			// Setup cascaded sequencer mode
	AdcRegs.ADCMAXCONV.all = 0x0001; 				// 8 double conv's (16 total)
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0; 			// Setup conv from ADCINA0 &amp; ADCINB0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x2; 			// Setup conv from ADCINA1 &amp; ADCINB1
	*/

	// PIE의 ADC 인터럽트 활성화
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;				// PIE 인터럽트(ADCINT) 활성화
	IER |= M_INT1; 									// CPU 인터럽트(INT1)  활성화
}

interrupt void Isr_Adc(void)
{
	/* ADC-A0. */
	gnAdcA0_Data[gnAdcA0_DataIndex] = AdcRegs.ADCRESULT0 >> 4;
	gnAdcA0_DataIndex++;

	if (gnAdcA0_DataIndex >= ADC_VOLTAGE_RES)
	{
		gnAdcA0_DataIndex = 0;

		int i = 0;
		int32 avr = 0;
		for (i = 0; i < ADC_VOLTAGE_RES; i++) avr += gnAdcA0_Data[i];
		avr = avr / ADC_VOLTAGE_RES;

		gnAdcA0_Avr_Data[gnAdcA0_Avr_DataIndex] = avr;
		gnAdcA0_Avr_DataIndex++;

		if (gnAdcA0_Avr_DataIndex >= ADC_VOLTAGE_AVR_RES)
		{
			gnAdcA0_Avr_DataIndex = 0;
		}
	}

	/* ADC-A2, B6 */
	//int32 rest = gnAdcA2_DataIndex % ADC_EC_FREQ_MULTI;
	//int32 index = (ADC_EC_CURRENT_RES / ADC_EC_FREQ_MULTI) * rest + (gnAdcA2_DataIndex / ADC_EC_FREQ_MULTI);

	//gnAdcA2_Data[index] = AdcRegs.ADCRESULT2 >> 4;
	//gnAdcB6_Data[index] = AdcRegs.ADCRESULT5 >> 4;

	gnAdcA2_Data[gnAdcA2_DataIndex] = AdcRegs.ADCRESULT2 >> 4;
	gnAdcB6_Data[gnAdcA2_DataIndex] = AdcRegs.ADCRESULT5 >> 4;

	gnAdcA2_DataIndex++;

	if (gnAdcA2_DataIndex >= ADC_EC_CURRENT_RES)
	{
		gnAdcA2_DataIndex = 0;
		EPwm6Regs.ETSEL.bit.SOCAEN = 0;
	}

	//if (rest == ADC_EC_FREQ_MULTI - 1)
		EPwm6Regs.CMPA.half.CMPA = EPwm6Regs.TBPRD * gnAdcA2_DataIndex / ADC_EC_CURRENT_RES;

	/* ADC-B6. */
	/*
	rest = gnAdcB6_DataIndex % ADC_EC_FREQ_MULTI;
	index = (ADC_EC_CURRENT_RES / ADC_EC_FREQ_MULTI) * rest + (gnAdcB6_DataIndex / ADC_EC_FREQ_MULTI);

	gnAdcB6_Data[index] = AdcRegs.ADCRESULT2 >> 4;
	gnAdcB6_DataIndex++;

	if (gnAdcB6_DataIndex >= ADC_EC_CURRENT_RES)
	{
		gnAdcB6_DataIndex = 0;
		EPwm6Regs.ETSEL.bit.SOCAEN = 0;
	}

	if (rest == ADC_EC_FREQ_MULTI - 1)
		EPwm6Regs.CMPA.half.CMPA = EPwm6Regs.TBPRD * gnAdcB6_DataIndex / ADC_EC_CURRENT_RES;
	*/

	// Reinitialize for next ADC sequence
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}

Uint16 Scia_Tx(Uint16 *Data, Uint16 num_byte)
{
	Uint16 i;

	for(i = 0 ; i < num_byte; i++)
	{
		while (SciaRegs.SCIFFTX.bit.TXFFST > 8);

		SciaRegs.SCITXBUF = *(Data + i) & 0x00FF;
	}

	return 1;
}


Uint16 Scia_Tx_Byte(char *Data, Uint16 num_byte)
{
	Uint16 i;

	for(i = 0 ; i < num_byte; i++)
	{
		while (SciaRegs.SCIFFTX.bit.TXFFST > 8);

		SciaRegs.SCITXBUF = (Uint16)Data[i] & 0x00FF;
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

	return '0' + (char)(ret % 64);
}


void SetGPIO_Out (int16 id, int16 on_off)
{
	Uint32 val = 0x00000040;
	val = val << id;

	if (on_off == TRUE)
	{
		GpioDataRegs.GPCSET.all = val;
	}
	else if (on_off == FALSE)
	{
		GpioDataRegs.GPCCLEAR.all = val;
	}
	else
	{
		GpioDataRegs.GPCTOGGLE.all = val;
	}
}


void Tx_Cmd (char *data, Uint16 len)
{
	char buf[RX_BUF_SIZE];

	memcpy (buf, data, len);

	buf[len] = buf[len - 1];
	buf[len - 1] = CalCheckSum_Base64 (buf, 1, len - 2);
	len++;

	Scia_Tx_Byte (buf, len);

	// TX - LED.
	SetGPIO_Out (3, TRUE);
	gnLED_TXD = gnTimerUS;
}

void SleepUS (int32 time_us)
{
	int64 start = gnTimerUS;

	while (TRUE)
	{
		if (gnTimerUS - start > time_us) break;
	}
}

//#define EC_CAL_WITH_PEAK TRUE

double GetEC_OneCycle ()
{
	int16 i, n;
	double ct_a0 = 2.0;
	double ct_a1 = 0.020;
	int16 noise_ad = 10;
	int16 ad_a0 = (int16)(4096 * ct_a0 / 3.0);
	int32 sum = 0;

	for (i = 0; i < ADC_EC_CURRENT_RES; i++)
	{
		sum = 0;
		for (n = 0; n < 8; n++)
		{
			sum += gnAdcA2_Data[(i + n) % ADC_EC_CURRENT_RES];
		}

		gnAdcA2_Data_Ext[(i + 8) % ADC_EC_CURRENT_RES] = sum / 8;
	}

	int32 ad_current = 0;
	int32 ad_peak = 0;
	sum = 0;

	for (i = 0; i < ADC_EC_CURRENT_RES; i++)
	{
		if (gnAdcA2_Data_Ext[i] > ad_a0 + noise_ad)
		{
			ad_current = gnAdcA2_Data_Ext[i] - ad_a0;
			sum += ad_current;

			if (ad_current > ad_peak) ad_peak = ad_current;
		}
		else if (gnAdcA2_Data_Ext[i] < ad_a0 - noise_ad)
		{
			ad_current = ad_a0 - gnAdcA2_Data_Ext[i];
			sum += ad_current;

			if (ad_current > ad_peak) ad_peak = ad_current;
		}
	}

	double avr = (double)sum / (double)ADC_EC_CURRENT_RES;
	double ec = avr * 3.0 / 4096.0 / ct_a1 * 1.3;
	gfEC_Peak_Max = ad_peak * 3.0 / 4096.0 / ct_a1 * 1.3;

#if EC_CAL_WITH_PEAK
	if (gnOpMode == OP_MODE_MANUAL)
		ec = gfEC_Peak_Max * (double)gnRX_PWM_Width / 1000 / 1.3;
	else
		ec = gfEC_Peak_Max * gfPower_MV / 100 / 1.3;
#endif

	return ec;
}

double GetEC_OneCycle_HVO ()
{
	int16 i, n;
	double ct_a0 = 2.5;
	double ct_a1 = 0.1042;
	int16 noise_ad = 10;
	int16 ad_a0 = (int16)(4096 * ct_a0 / 3.0);
	int32 sum = 0;

	for (i = 0; i < ADC_EC_CURRENT_RES; i++)
	{
		sum = 0;
		for (n = 0; n < 8; n++)
		{
			sum += gnAdcB6_Data[(i + n) % ADC_EC_CURRENT_RES];
		}

		gnAdcA2_Data_Ext[(i + 8) % ADC_EC_CURRENT_RES] = sum / 8;
	}

	sum = 0;
	for (i = 0; i < ADC_EC_CURRENT_RES; i++)
	{
		if (gnAdcA2_Data_Ext[i] > ad_a0 + noise_ad)
		{
			sum += gnAdcA2_Data_Ext[i] - ad_a0;
		}
		else if (gnAdcA2_Data_Ext[i] < ad_a0 - noise_ad)
		{
			sum += ad_a0 - gnAdcA2_Data_Ext[i];
		}
	}

	double avr = (double)sum / (double)ADC_EC_CURRENT_RES;
	double ec = avr * 3.0 / 4096.0 / ct_a1 * 1.0;

	return ec;
}

void TurnSrcPwrOn (int16 on_off)
{
	if (on_off == TRUE)
	{
		//GpioDataRegs.GPCSET.all = 0x00002000;	// GPIO 77
		SetGPIO_Out (7, TRUE);
		SleepUS (500000);
	}
	else
	{
		//GpioDataRegs.GPCCLEAR.all = 0x00002000;	// GPIO 77
		SetGPIO_Out (7, FALSE);
	}
}

void TurnPowerOn (int16 on_off)
{
	if (on_off == TRUE)
	{
		if (!gisPower_On)
		{
			TurnSrcPwrOn (TRUE);

			gisPower_On = TRUE;
			gfPower_SV = gnRX_SV;

			SetGPIO_Out (4, TRUE);
		}
	}
	else
	{
		gisPower_On = FALSE;
		gfPower_SV = 0;
		gfPower_MV = 0;

		SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);

		TurnSrcPwrOn (FALSE);
		SetGPIO_Out (4, FALSE);
	}
}

void Main_Loop (void)
{
	char rx_cmd_buf [RX_BUF_SIZE];
	char tx_cmd_buf [RX_BUF_SIZE];
	int16 rx_cmd_size = 0;
	int16 rx_val_size = 0;

	int32 i = 0;
	int32 num = 0;
	int32 freq = 0;
	int32 width = 0;

	char check_sum = 0;

	/* Power On LED. */
	SetGPIO_Out (0, TRUE);

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

			/* RX - LED.*/
			SetGPIO_Out (2, TRUE);
			gnLED_RXD = gnTimerUS;

			/* CL Number. */
			if (rx_cmd_buf[1] == 'V' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'V';

				Int_To_Code10 (tx_cmd_buf, num, 4, CL_NUMBER);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Auto mode. */
			else if (rx_cmd_buf[1] == 'A' && rx_val_size == 1)
			{
				// ON.
				if (rx_cmd_buf[2] == '1' || rx_cmd_buf[2] == '2')
				{
					Tx_Cmd ("{A}", 3);

					gnOpMode = OP_MODE_AUTO;
					gfPower_SV = 0;
					SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
				}

				// OFF.
				else if (rx_cmd_buf[2] == '0')
				{
					Tx_Cmd ("{A}", 3);

					gnOpMode = OP_MODE_MANUAL;
					SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
				}

				else
				{
					Tx_Cmd ("{E0}", 4);
				}
			}

			/* Power. */
			else if (gnOpMode == OP_MODE_AUTO && rx_cmd_buf[1] == 'P')
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

					if (sv == 0) TurnPowerOn (FALSE);
					else if (sv < POWER_MIN_SV) sv = POWER_MIN_SV;
					else if (sv > POWER_MAX_SV_LIMIT) sv = POWER_MAX_SV_LIMIT;

					gnRX_SV = sv;

					if (gisPower_On == TRUE)
					{
						gfPower_SV = sv;
					}
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

					/*
					if (!gisPower_On)
					{
						gisPower_On = TRUE;
						gfPower_SV = gnRX_SV;
					}
					*/
				}

				// OFF.
				else if (rx_cmd_buf[2] == '0')
				{
					Tx_Cmd ("{T}", 3);
					TurnPowerOn (FALSE);

					/*
					gisPower_On = FALSE;
					gfPower_SV = 0;
					gfPower_MV = 0;

					SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
					*/
				}

				else
				{
					Tx_Cmd ("{E0}", 4);
				}
			}

			/* Over-Current. */
			else if (rx_cmd_buf[1] == 'O')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'O';

					Int_To_Code10 (tx_cmd_buf, num, 4, (int32)gnOvercurrent_Val);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{O}", 3);
					int32 val = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (val < 0) val = 0;
					if (val > MAX_INTERLOCK_CURRENT_VAL) val = MAX_INTERLOCK_CURRENT_VAL;

					gnOvercurrent_Val = val;
				}
			}

			/* ER-Short. */
			else if (rx_cmd_buf[1] == 'E')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'E';

					Int_To_Code10 (tx_cmd_buf, num, 4, (int32)gnER_Short_CurrentVal);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{E}", 3);
					int32 val = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (val < 0) val = 0;
					if (val > MAX_INTERLOCK_CURRENT_VAL) val = MAX_INTERLOCK_CURRENT_VAL;

					gnER_Short_CurrentVal = val;
				}
			}

			/* Interlock-Reset. */
			else if (rx_cmd_buf[1] == 'N')
			{
				Tx_Cmd ("{N}", 3);

				gisOverCurrent = FALSE;
				gnOverCurrent_Count = 0;

				gisER_Short = FALSE;
				gnER_Short_Count = 0;

				gisVDrop = FALSE;
				gnVDrop_Count = 0;
			}

			/* PWM Frequency. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'F')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'F';

					Int_To_Code10 (tx_cmd_buf, num, 4, gnRX_PWM_Freq);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{F}", 3);
					freq = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (freq < MIN_PWM_FREQ) freq = MIN_PWM_FREQ;
					if (freq > MAX_PWM_FREQ) freq = MAX_PWM_FREQ;

					gnRX_PWM_Freq = freq;
					SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, gnRX_PWM_Width / 10.0);
					//SetPwm_2833x (6, TB_COUNT_UP, gnRX_PWM_Freq * 100 * ADC_EC_FREQ_MULTI, gnRX_PWM_Width / 10.0);
					SetPwm_2833x (6, TB_COUNT_UP, gnRX_PWM_Freq * 100, gnRX_PWM_Width / 10.0);
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

					Int_To_Code10 (tx_cmd_buf, num, 4, gnRX_PWM_Width);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{W}", 3);
					width = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (width < MIN_PWM_WIDTH) width = MIN_PWM_WIDTH;
					if (width > MAX_PWM_WIDTH_LIMIT) width = MAX_PWM_WIDTH_LIMIT;

					if (width == 0) TurnSrcPwrOn (FALSE);
					if (width > 0) TurnSrcPwrOn (TRUE);

					gnRX_PWM_Width = width;
					SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, gnRX_PWM_Width / 10.0);
					//SetPwm_2833x (6, TB_COUNT_UP, gnRX_PWM_Freq * 100 * ADC_EC_FREQ_MULTI, gnRX_PWM_Width / 10.0);
					SetPwm_2833x (6, TB_COUNT_UP, gnRX_PWM_Freq * 100, gnRX_PWM_Width / 10.0);
				}
			}

			/* Monitoring. */
			else if (rx_cmd_buf[1] == 'M' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'M';

				// PV.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'W';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)gfPower_PV);
				num += 4;

				// Over current.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'O';
				if (gisOverCurrent)
					tx_cmd_buf[num++] = '1';
				else tx_cmd_buf[num++] = '0';

				// Elect-rode short.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'E';
				if (gisER_Short)
					tx_cmd_buf[num++] = '1';
				else tx_cmd_buf[num++] = '0';

				// SMPS voltage drop.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'V';
				if (gisVDrop)
					tx_cmd_buf[num++] = '1';
				else tx_cmd_buf[num++] = '0';

				// RX error count.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'R';
				Int_To_Code10 (tx_cmd_buf, num, 4, gnSCI_RxErrorCnt);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Monitoring. - Internal */
			else if (rx_cmd_buf[1] == 'I' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'I';

				// PV.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'P';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)gfPower_PV);
				num += 4;

				// MV.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'M';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(gfPower_MV * 10.0));
				num += 4;

				// Interlock.
				int32 il = 0;
				if (gisOverCurrent) il |= 0x01;
				if (gisER_Short) il |= 0x02;
				if (gisVDrop) il |= 0x04;

				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'L';
				Int_To_Code10 (tx_cmd_buf, num, 4, il);
				num += 4;

				// Current.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'C';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(gfEC_OneCycle * 100));
				num += 4;

				// Current HVO.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'H';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(gfEC_OneCycle_HVO * 100));
				num += 4;

				// Peak Current.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'P';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(gfEC_Peak * 100));
				num += 4;

				// Peak Max. Current.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'X';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(gfEC_Peak_Max * 100));
				num += 4;

				// VSource.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'V';
				Int_To_Code10 (tx_cmd_buf, num, 4, (int32)(gfVSource));
				num += 4;

				// RX error count.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'R';
				Int_To_Code10 (tx_cmd_buf, num, 4, gnSCI_RxErrorCnt);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Reset. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'R' && rx_val_size == 0)
			{
				/*
				Tx_Cmd ("{R}", 3);
				ResetPwm ();
				*/
			}

			/* Current. */
			else if (rx_cmd_buf[1] == 'C' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'C';

/*
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
*/
				// EC_SRC.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'S';

				int32 ec_val = (int32)(gfEC_OneCycle * 100);
				if (ec_val < 0) ec_val = 0;

				Int_To_Code10 (tx_cmd_buf, num, 4, ec_val);
				num += 4;

				// EC_HVO.
				tx_cmd_buf[num++] = ',';
				tx_cmd_buf[num++] = 'H';

				ec_val = (int32)(gfEC_OneCycle_HVO * 100);
				if (ec_val < 0) ec_val = 0;

				Int_To_Code10 (tx_cmd_buf, num, 4, ec_val);
				num += 4;

				//SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
				//SleepUS (1000);
				//SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, gnRX_PWM_Width / 10.0);

				//gisDebug_BreakOn = TRUE;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Boost freq. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'K')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'K';

					Int_To_Code10 (tx_cmd_buf, num, 4, gnRX_Boost_Freq);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{K}", 3);
					freq = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (freq < MIN_BST_FREQ) freq = MIN_BST_FREQ;
					if (freq > MAX_BST_FREQ) freq = MAX_BST_FREQ;

					gnRX_Boost_Freq = freq;
					SetPwm_2833x (1, TB_COUNT_UP, gnRX_Boost_Freq * 100, gnRX_Boost_Width / 10.0);
				}
			}

			/* Boost width. */
			else if (gnOpMode == OP_MODE_MANUAL && rx_cmd_buf[1] == 'L')
			{
				if (rx_cmd_buf[2] == '?')
				{
					num = 0;
					tx_cmd_buf[num++] = '{';
					tx_cmd_buf[num++] = 'L';

					Int_To_Code10 (tx_cmd_buf, num, 4, gnRX_Boost_Width);
					num += 4;

					tx_cmd_buf[num++] = '}';
					Tx_Cmd (tx_cmd_buf, num);
				}
				else
				{
					Tx_Cmd ("{L}", 3);
					width = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

					if (width < MIN_BST_WIDTH) width = MIN_BST_WIDTH;
					if (width > MAX_BST_WIDTH) width = MAX_BST_WIDTH;

					gnRX_Boost_Width = width;
					SetPwm_2833x (1, TB_COUNT_UP, gnRX_Boost_Freq * 100, gnRX_Boost_Width / 10.0);
				}
			}

			/* Test - SMPS On/Off */
			else if (rx_cmd_buf[1] == 's')
			{
				int val = Code10_To_Int (rx_cmd_buf, 2, rx_val_size);

				if (val == 0) TurnSrcPwrOn (FALSE);	// GpioDataRegs.GPCCLEAR.all = 0x00002000;
				if (val == 1) TurnSrcPwrOn (TRUE); 	// GpioDataRegs.GPCSET.all = 0x00002000;

				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 's';

				Int_To_Code10 (tx_cmd_buf, num, 4, val);
				num += 4;

				tx_cmd_buf[num++] = '}';
				Tx_Cmd (tx_cmd_buf, num);
			}

			/* Test. */
			else if (rx_cmd_buf[1] == 'z' && rx_val_size == 0)
			{
				num = 0;
				tx_cmd_buf[num++] = '{';
				tx_cmd_buf[num++] = 'z';

				// ADC-2
				AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 1;
				/*
				int i;
				for (i = 0; i < 10000; i++);

				AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 0;
				*/

				int32 val1 = AdcRegs.ADCRESULT0 >> 4;
				int32 val2 = AdcRegs.ADCRESULT8 >> 4;

				Int_To_Code10 (tx_cmd_buf, num, 4, val1);
				num += 4;

				tx_cmd_buf[num++] = ',';

				Int_To_Code10 (tx_cmd_buf, num, 4, val2);
				num += 4;

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

		/* LED. */
		if (gnTimerUS - gnLED_Run > 500000)
		{
			gnLED_Run = gnTimerUS;
			SetGPIO_Out (1, 2);
		}

		if (gnLED_RXD > 0 && gnTimerUS - gnLED_RXD > 100000)
		{
			gnLED_RXD = 0;
			SetGPIO_Out (2, FALSE);
		}

		if (gnLED_TXD > 0 && gnTimerUS - gnLED_TXD > 100000)
		{
			gnLED_TXD = 0;
			SetGPIO_Out (3, FALSE);
		}

		if (gnTimerUS - gnLED_ALARM > 100000)
		{
			gnLED_ALARM = gnTimerUS;

			int64 val = gnTimerUS / 100000;

			if (val % 5 == 0)
			{
				if (gisOverCurrent == TRUE) SetGPIO_Out (5, TRUE);
				else SetGPIO_Out (5, FALSE);

				if (gisER_Short == TRUE && gisVDrop == FALSE) SetGPIO_Out (6, TRUE);
				if (gisER_Short == FALSE && gisVDrop == TRUE) SetGPIO_Out (6, 2);
			}

			if (gisER_Short == TRUE && gisVDrop == TRUE) SetGPIO_Out (6, 2);
			if (gisER_Short == FALSE && gisVDrop == FALSE) SetGPIO_Out (6, FALSE);
		}

		/* Check EC. */
		if (gnTimerUS > gnEC_Check_PreTime + 10000 &&
			gisAdcA2_Start == FALSE && EPwm6Regs.ETSEL.bit.SOCAEN == 0)
		{
			gnEC_Check_PreTime = gnTimerUS;

			gisAdcA2_Start = TRUE;
			EPwm6Regs.ETSEL.bit.SOCAEN = 1;
		}

		if (gisAdcA2_Start && EPwm6Regs.ETSEL.bit.SOCAEN == 0)
		{
			gisAdcA2_Start = FALSE;

			// EC.
			double ec = GetEC_OneCycle ();
			double ec_hvo = GetEC_OneCycle_HVO ();

			/*
			if (gisDebug_BreakOn == TRUE)
			{
				SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
				SleepUS (1000);
				for (i = 0; i < 1000000; i++) ;
				gisDebug_BreakOn = FALSE;
			}
			*/

			gnEC_Buf[gnEC_BufIndex] = (int16)(ec * 100);
			gnEC_Buf_HVO[gnEC_BufIndex] = (int16)(ec_hvo * 100);

			gnEC_BufIndex++;
			if (gnEC_BufIndex >= EC_BUF_SIZE) gnEC_BufIndex = 0;

			int32 ec_sum = 0;
			int32 ec_hvo_sum = 0;
			for (i = 0; i < EC_BUF_SIZE; i++)
			{
				ec_sum += gnEC_Buf[i];
				ec_hvo_sum += gnEC_Buf_HVO[i];
			}

			gfEC_OneCycle = (double)ec_sum / EC_BUF_SIZE / 100.0;
			gfEC_OneCycle_HVO = (double)ec_hvo_sum / EC_BUF_SIZE / 100.0;

			// EC-HVO.
			/*
			ec = GetEC_OneCycle_HVO ();

			gnEC_Buf_HVO[gnEC_BufIndex++] = (int16)(ec * 100);
			if (gnEC_BufIndex >= EC_BUF_SIZE) gnEC_BufIndex = 0;

			ec_sum = 0;
			for (i = 0; i < EC_BUF_SIZE; i++)
			{
				ec_sum += gnEC_Buf_HVO[i];
			}

			gfEC_OneCycle_HVO = (double)ec_sum / EC_BUF_SIZE / 100.0;
			*/
		}

		/* Power Control. */
		if (gnTimerUS > gnPower_Check_PreTime + 50000 &&
			gnOpMode == OP_MODE_AUTO)
		{
			gnPower_Check_PreTime = gnTimerUS;

			if (gisPower_On == TRUE)
			{
				double pvs = 300;

				gfPower_PV = POWER_VOLTAGE * gfEC_OneCycle;
				if (gfPower_SV <= pvs)
					gfPower_PV /= 1.0 + ((pvs - gfPower_SV) / (pvs - (double)POWER_MIN_SV) * 0.5);
				else gfPower_PV *= 1.0 + ((gfPower_SV - pvs) / ((double)POWER_MAX_SV - pvs) * 1.3);

				double mv = gfPower_SV - gfPower_PV;
				double mv_abs = (mv < 0) ? -mv : mv;

				/*
				double mv_ratio = 0.5;
				if (mv_abs < 50) mv_ratio = 0.05;
				else if (mv_abs < 50) mv_ratio = 0.05;
				else if (mv_abs < 100) mv_ratio = 0.08;
				else if (mv_abs < 200) mv_ratio = 0.1;
				else if (mv_abs < 300) mv_ratio = 0.3;
				*/

				double mv_ratio = mv_abs / 1000.0;

				if (gfPower_PV <= 500)
					mv_ratio = (mv_ratio > 0.5) ? 0.5 : mv_ratio;
				else if (gfPower_PV <= 1000)
					mv_ratio = (mv_ratio > 0.25) ? 0.25 : mv_ratio;
				else mv_ratio = (mv_ratio > 0.125) ? 0.125 : mv_ratio;

				/*
				double mv_ratio = mv_abs / 500.0;
				//if (gfPower_SV > 800) mv_ratio = mv_abs / 1200.0;
				mv_ratio = (mv_ratio > 1) ? 1 : mv_ratio;
				*/

				if (mv_abs != 0)
				{
					mv = (mv / mv_abs) * mv_ratio;
				}

				/*
				double mv_ratio = mv / gfPower_SV * 0.001;

				if (mv_ratio < 0) mv_ratio = -mv_ratio;

				mv_ratio += 0.0009;
				if (mv_ratio > 0.002) mv_ratio = 0.002;

				mv *= mv_ratio;
				*/

				/*
				if (gfPower_SV > 500 && mv < 100 && mv > -100) mv *= 0.0008;
				else mv *= 0.0012;
				*/

				gfPower_MV += mv;

				if (gfPower_MV < POWER_MIN_MV) gfPower_MV = POWER_MIN_MV;
				if (gfPower_MV > POWER_MAX_MV) gfPower_MV = POWER_MAX_MV;

				SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, gfPower_MV);
			}
			else
			{
				SetPwm_2833x (5, TB_COUNT_UPDOWN, gnRX_PWM_Freq * 100, 0);
			}
		}

		/* Interlock. */
		if (gnTimerUS > gnInterlock_Check_PreTime + 10000)
		{
			gnInterlock_Check_PreTime = gnTimerUS;

			/* Over Current. (FET broken) */
			if (gfEC_OneCycle > gnOvercurrent_Val)
				gnOverCurrent_Count++;
			else gnOverCurrent_Count--;

			if (gnOverCurrent_Count >= OVERCURRENT_MAX_COUNT)
			{
				gnOverCurrent_Count = OVERCURRENT_MAX_COUNT;
				gisOverCurrent = TRUE;

				TurnPowerOn (FALSE);
			}

			/*
			if (gnOverCurrent_Count <= 0)
			{
				gnOverCurrent_Count = 0;
				gisOverCurrent = FALSE;
			}
			*/

			/* Electrode short. */
			if (gisPower_On == TRUE && gfPower_MV >= POWER_MIN_MV)
			{
				gfEC_Peak = gfEC_OneCycle * 100 / gfPower_MV;

				if (gfEC_Peak > gnER_Short_CurrentVal)
					gnER_Short_Count++;
				else gnER_Short_Count--;

				if (gnER_Short_Count >= ERSHORT_MAX_COUNT)
				{
					gnER_Short_Count = ERSHORT_MAX_COUNT;
					gisER_Short = TRUE;

					TurnPowerOn (FALSE);
				}

				/*
				if (gnER_Short_Count <= 0)
				{
					gnER_Short_Count = 0;
					gisER_Short = FALSE;
				}
				*/
			}
			else if (gnOpMode == OP_MODE_MANUAL && gnRX_PWM_Width > MIN_PWM_WIDTH)
			{
				gfEC_Peak = gfEC_OneCycle * 100 / (gnRX_PWM_Width / 10.0);
			}
			else
			{
				gfEC_Peak = 0;
			}

			/* SMPS Voltage drop. */
			if (gnTimerUS > 3000000)
			{
				double avr = 0;

				for (i = 0; i < ADC_VOLTAGE_AVR_RES; i++)
				{
					avr += gnAdcA0_Avr_Data[i];
				}

				avr /= ADC_VOLTAGE_AVR_RES;
				gfVSource = avr * 3 / 4096 * 100;

				if ((gnOpMode == OP_MODE_AUTO && gisPower_On == TRUE) ||
					(gnOpMode == OP_MODE_MANUAL && gnRX_PWM_Width > 0))
				{
					if (avr * 3 / 4096 < VDROP_VAL)
						gnVDrop_Count++;
					else gnVDrop_Count--;

					if (gnVDrop_Count >= VDROP_MAX_COUNT)
					{
						gnVDrop_Count = VDROP_MAX_COUNT;
						gisVDrop = TRUE;
					}

					/*
					if (gnVDrop_Count <= 0)
					{
						gnVDrop_Count = 0;
						gisVDrop = FALSE;
					}
					*/
				}
			}
		}

		/* RX error check. */
		if (SciaRegs.SCIRXST.bit.RXERROR == 0x01)
		{
			gnSCI_RxErrorCnt++;

			Init_Scia ();

			for (i = 0; i < 1000; i++)
			{
				SleepUS (100);
				if (SciaRegs.SCIRXST.bit.RXERROR == 0x00) break;
			}
		}
	}
}

void main (void)
{
	/* Step 1. Initialize System Control */
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();

#if FLASH_LOAD == 1
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash ();
#endif

	/* Step 2. Initialize GPIO */
	// This example function is found in the DSP2833x_Gpio.c file and
	// illustrates how to set the GPIO to it's default state.

	EALLOW;
	/*
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;
	GpioCtrlRegs.GPADIR.all = 0xFFFFFFFF;
   	GpioDataRegs.GPASET.all = 0x00000000;
   	GpioDataRegs.GPACLEAR.all = 0xFFFFFFFF;

   	GpioCtrlRegs.GPBMUX1.all = 0x00000000;
	GpioCtrlRegs.GPBMUX2.all = 0x00000000;
	GpioCtrlRegs.GPBDIR.all = 0xFFFFFFFF;
   	GpioDataRegs.GPBSET.all = 0x00000000;
   	GpioDataRegs.GPBCLEAR.all = 0xFFFFFFFF;
	*/

   	//GpioCtrlRegs.GPCMUX1.all = 0x00000000;
	//GpioCtrlRegs.GPCMUX2.all = 0x00000000;
	GpioCtrlRegs.GPCDIR.all = 0x00003FC0;
   	//GpioDataRegs.GPCSET.all = 0x0000FF00;
   	GpioDataRegs.GPCCLEAR.all = 0x00003FC0;
   	EDIS;

	InitSciaGpio();
	InitEPwm1Gpio ();
	InitEPwm5Gpio ();
	InitEPwm6Gpio ();

	/* Step 3. Initialize PIE vector table */
	// Disable and clear all CPU interrupts
	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize PIE control registers to their default state:
	// This function is found in the DSP2833x_PieCtrl.c file.
	InitPieCtrl();

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
	// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();

	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;
	PieVectTable.TINT0 = &Isr_CpuTimer0;
	PieVectTable.SCIRXINTA = &Isr_Scia_RxFifo;
	PieVectTable.ADCINT = &Isr_Adc;
	EDIS;

	/* Step 4. Initialize all the Device Peripherals */
	Init_CpuTimer ();
	Init_Scia ();
	Init_ADC ();

	union AQCTL_REG aqctla;
	union AQCTL_REG aqctlb;

	aqctla.all = 0;
	aqctlb.all = 0;
	aqctla.bit.ZRO = AQ_SET;
	aqctla.bit.CAU = AQ_CLEAR;
	InitPwm_2833x (1, TB_COUNT_UP, 0, aqctla, aqctlb);

	aqctla.all = 0;
	aqctlb.all = 0;
	aqctla.bit.CAU = AQ_SET;
	aqctla.bit.PRD = AQ_CLEAR;
	aqctlb.bit.CBD = AQ_SET;
	aqctlb.bit.ZRO = AQ_CLEAR;
	InitPwm_2833x (5, TB_COUNT_UPDOWN, 0, aqctla, aqctlb);

	aqctla.all = 0;
	aqctlb.all = 0;
	aqctla.bit.ZRO = AQ_SET;
	aqctla.bit.CAU = AQ_CLEAR;
	InitPwm_2833x (6, TB_COUNT_UP, 1, aqctla, aqctlb);

	/* Step 5. User specific code, enable interrupts */
	EINT;

	/* Step 6. IDLE loop. Just sit and loop forever (optional) */
	Main_Loop ();
}
