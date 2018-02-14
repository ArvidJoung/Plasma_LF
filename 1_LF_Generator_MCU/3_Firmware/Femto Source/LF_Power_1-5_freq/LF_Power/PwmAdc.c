#include "DSP280x_Device.h"     // Headerfile Include File
#include "DSP280x_Examples.h"   // Examples Include File
#include "Global.h"
#include "PwmAdc.h"
#include "Flash280x_API_Library.h"
#include "Example_Flash280x_API.h"
#include <math.h>

/* PWM Parameter. */
static int32 gnPAR_PwmWidth = PAR_DEF_PWM_WIDTH;
static int32 gnPAR_PwmFreq = PAR_DEF_PWM_FREQ;
static int32 gnPAR_PwmBurstWidth = PAR_DEF_PWM_BURST_WIDTH;
static int32 gnPAR_PwmBurstFreq	 = PAR_DEF_PWM_BURST_FREQ;

/* PWM Config. */
static int32 gnPwm_TBPRD = 999;
static int32 gnPwm_Width = 0;

static int32 gisPwm_BurstOn = FALSE;
static int32 gnPwm_BurstWidth = 0;
static int32 gnPwm_BurstWidthMax = 0;

int32 gnPwm_WidthMinLimit = 200;	// Unit: 0.1%
int32 gnPwm_ErrRange = 150;			// Unit: 0.1%

/* ADC. */
#define ADC_CURRENT_RES				200		// current resolution.
#define ADC_CURRENT_ZERO_RANGE		28		// about 0.2A
#define ADC_CURRENT_AVR_NUM			3
#define ADC_CHECK_PWM_WIDTH			(PAR_MAX_PWM_WIDTH / 5)

#define POWER_EC_DATA_LEN 			100

static int32 gnAdcA0_DataIndex = 0;
static int16 gnAdcA0_Data[ADC_CURRENT_RES] = {0, };
//static int16 gnAdcA0_Data_Limit[ADC_CURRENT_RES] = {0, };
//static double gnAdcA0_Data_Avr[ADC_CURRENT_RES] = {0, };

static int32 gnPower_EC_Index = 0;
static Uint16 gnPower_EC_Data[POWER_EC_DATA_LEN] = {0, };
//static double gnPower_EC_Data_Sort[POWER_EC_DATA_LEN] = {0, };

/* Timer. */
int32 gnTimer_10us = 0;


static void Init_ADC ()
{
	InitAdc();

	/* ADC & PWM interrupt. */
	/*
	// ADC 설정
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 10;				// ADCCLKPS = [HSPCLK / ADCCLKPS *2] (5MHz = 100MHz / 20)
	AdcRegs.ADCTRL1.bit.CPS = 0;					// ADCCLK = ADCCLKPS / 1  (ADC의 구동 클럭을 결정 : ADCCLK = 5MHz)
	AdcRegs.ADCTRL1.bit.ACQ_PS = 2;					// 샘플/홀드 시간 설정 : ACQ_PS + 1 = 3 사이클
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;				// 시퀀스 직렬 모드
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; 		// ePWM_SOCA로 ADC 시퀀스 시동
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;			// seq 시퀀스 완료시 인터럽트 발생 설정

	//>>[]
	//AdcRegs.ADCMAXCONV.all = 0;						// 변환 채널수 설정 = MAXCONV + 1
	AdcRegs.ADCMAXCONV.all = 1; 					// 변환 채널수 설정 = MAXCONV + 1

	//seq 변환 순서
	//AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 8;			// 1번째 변환 : ADC_B0
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;			// 2번째 변환 : ADC_A0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 2;			// 2번째 변환 : ADC_A2
	//<<[]

	//seq 트리거 조건 - PWM3의 주파수(8KHz)
	EPwm3Regs.ETSEL.bit.SOCAEN = 1;     			// SOCA 이벤트 트리거 Enable
	EPwm3Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD;  		// SCCA 트리거 조건 : 카운터 주기 일치 시
	EPwm3Regs.ETPS.bit.SOCAPRD = ET_1ST;			// SOCA 이벤트 분주 설정 : 트리거 조건 한번 마다
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		// count up and start
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)]
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;			// TBCLK = [100MHz / (1*1)] = 100MHz
   	EPwm3Regs.TBPRD = 12499;						// Set period for ePWM3 (ADC 샘플링 주파수 : 100MHz/12500 = 8kHz)
	EPwm3Regs.TBCTR = 0;							// Clear counter

	// PIE의 ADC 인터럽트 활성화
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;				// PIE 인터럽트(ADCINT) 활성화
	IER |= M_INT1; 									// CPU 인터럽트(INT1)  활성화
	*/

	/* Continuous ADC. */
	/*
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0x0f;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 1;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        		// 1  Cascaded mode
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;			// 1번째 변환 : ADC_A0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 2;			// 2번째 변환 : ADC_A2
	AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       		// Setup continuous run

	AdcRegs.ADCMAXCONV.all = 1; 					// 변환 채널수 설정 = MAXCONV + 1

	AdcRegs.ADCTRL2.all = 0x2000;
	*/

	/* ADC & PWM interrupt. */
	// ADC 설정
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 2;				// ADCCLKPS = [HSPCLK / ADCCLKPS *2] (5MHz = 100MHz / 20)
	AdcRegs.ADCTRL1.bit.CPS = 0;					// ADCCLK = ADCCLKPS / 1  (ADC의 구동 클럭을 결정 : ADCCLK = 5MHz)
	AdcRegs.ADCTRL1.bit.ACQ_PS = 2;					// 샘플/홀드 시간 설정 : ACQ_PS + 1 = 3 사이클
	//AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;				// 시퀀스 직렬 모드
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;				// 시퀀스 직렬 모드
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; 		// ePWM_SOCA로 ADC 시퀀스 시동
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;			// seq 시퀀스 완료시 인터럽트 발생 설정

	//AdcRegs.ADCMAXCONV.all = 0;						// 변환 채널수 설정 = MAXCONV + 1
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0;				// 변환 채널수 설정 = MAXCONV + 1
	AdcRegs.ADCMAXCONV.bit.MAX_CONV2 = 0;				// 변환 채널수 설정 = MAXCONV + 1

	//seq 변환 순서
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;			// 1번째 변환 : ADC_A0
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 2;			// 2번째 변환 : ADC_A2

	// PIE의 ADC 인터럽트 활성화
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;				// PIE 인터럽트(ADCINT) 활성화
	IER |= M_INT1; 									// CPU 인터럽트(INT1)  활성화
}


static void Init_ePWM ()
{
	/* ePWM1/ePWM2 PIN */
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;				// Configure GPIO0 as EPWM1A
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;				// Configure GPIO1 as EPWM1B

	//GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;				// Configure GPIO2 as EPWM2A
	//GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;				// Configure GPIO3 as EPWM2B
	EDIS;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	/* ePWM1/ePWM2 Time-Base Submodule */
	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;		// set Immediate load
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)]
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;			// TBCLK = [100MHz / (1*1)] = 100MHz
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
	EPwm1Regs.TBPRD = gnPwm_TBPRD;			  	    // Set period for ePWM1 (PWM 캐리어 주파수 : 100MHz/(1000 * 2) = 50kHz)
	EPwm1Regs.TBCTR = 0;							// Clear counter
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;

	EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;		// set Immediate load
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)]
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;			// TBCLK = [100MHz / (1*1)] = 100MHz
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm2Regs.TBPRD = gnPwm_TBPRD * 2;		  	    // Set period for ePWM2 (PWM 캐리어 주파수 : 100MHz/(1000 * 2) = 50kHz)
	EPwm2Regs.TBCTR = 0;							// Clear counter
	EPwm2Regs.TBPHS.all = 0;
	EPwm2Regs.TBCTL.bit.PHSEN = 1;
	EPwm2Regs.TBCTL.bit.PHSDIR = 1;

	/* ePWM1/ePWM2 Counter-Compare Submodule */
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// 비교 레지스터에 Shadow 레지스터 사용
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	// 카운터가 0 일때 Shadow 레지스터에서 비교 레지스터에 비교 값 로드

	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// 비교 레지스터에 Shadow 레지스터 사용
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	// 카운터가 0 일때 Shadow 레지스터에서 비교 레지스터에 비교 값 로드

	/* ePWM1 Action-qualifier */
	//EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;				// 카운터가 제로일 때 High
	//EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	//EPwm1Regs.AQCTLB.bit.PRD = AQ_SET;				// 카운터가 제로일 때 High
	//EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;			// 카운터가 제로일 때 High
	
	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;				// 카운터가 제로일 때 High
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low

	/* ePWM2 Event-Trigger Submodule */
	EPwm2Regs.ETSEL.bit.SOCAEN = 0;     			// SOCA 이벤트 트리거 Enable
	EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;  	// SCCA 트리거 조건
	EPwm2Regs.ETPS.bit.SOCACNT = 1;
	EPwm2Regs.ETPS.bit.SOCAPRD = ET_1ST;			// SOCA 이벤트 분주 설정 : 트리거 조건 한번 마다

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}


static void Init_CpuTimer ()
{
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 100, 10);
	StartCpuTimer0();

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	IER |= M_INT1;
}


interrupt void Isr_Adc(void)
{
	// ADC & PWM.
	if (gisPwm_BurstOn)
	{
		gnAdcA0_Data[gnAdcA0_DataIndex] = AdcRegs.ADCRESULT0 >> 4;
		gnAdcA0_DataIndex++;

#if (PID_CONTROL == TRUE)
		if (gnPAR_PwmWidth < ADC_CHECK_PWM_WIDTH)
		{
			int32 adc_check_point_1 = ADC_CURRENT_RES * 5 / 10;
			int32 adc_check_point_2 = ADC_CURRENT_RES * 8 / 10;

			if (gnAdcA0_DataIndex == adc_check_point_1)
			{
				gnAdcA0_DataIndex = adc_check_point_2;
			}
		}

		if (gnAdcA0_DataIndex >= ADC_CURRENT_RES)
		{
			gnAdcA0_DataIndex = 0;
			EPwm2Regs.ETSEL.bit.SOCAEN = 0;
		}
#else
		if (gnAdcA0_DataIndex >= ADC_CURRENT_RES)
		{
			gnAdcA0_DataIndex = 0;
			EPwm2Regs.ETSEL.bit.SOCAEN = 0;
		}
#endif

		EPwm2Regs.CMPA.half.CMPA = gnPwm_TBPRD * 2 * (gnAdcA0_DataIndex % ADC_CURRENT_RES) / ADC_CURRENT_RES;
	}

	// Reinitialize for next ADC sequence
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}


interrupt void Isr_CpuTimer0 (void)
{
	gnTimer_10us++;
	if (gnTimer_10us >= TIMER_10US_MAX) gnTimer_10us = 0;	// reset at every 5hours.

	int32 count = gnTimer_10us % gnPwm_BurstWidthMax;

	/* PWM On */
	if (count == 0)
	{
		gisPwm_BurstOn = TRUE;
		EPwm1Regs.AQCSFRC.bit.CSFA = 0;
		EPwm1Regs.AQCSFRC.bit.CSFB = 0;
	}
	else if (count >= gnPwm_BurstWidth)
	{
		gisPwm_BurstOn = FALSE;
		EPwm1Regs.AQCSFRC.bit.CSFA = 1;
		EPwm1Regs.AQCSFRC.bit.CSFB = 1;
	}

	/* ADC check on */
	/*
	int32 max_check_dur = (gnPwm_BurstWidthMax * 0.5);

	if (count == 0 && gnPwm_BurstWidth < max_check_dur)
	{
		gisPwm_BurstOn = TRUE;
	}
	else if (count == (gnPwm_BurstWidth - max_check_dur) && gnPwm_BurstWidth >= max_check_dur)
	{
		gisPwm_BurstOn = TRUE;
	}
	else if (count >= gnPwm_BurstWidth)
	{
		gisPwm_BurstOn = FALSE;
	}
	*/

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


void InitPwmAdc ()
{
   	Init_ADC ();
   	Init_ePWM ();
   	Init_CpuTimer ();
}


void SetPwm (int32 pwm_freq, int32 pwm_width, int32 burst_freq, int32 burst_width)
{
	/* check parameter. */
	if (pwm_freq >= 0)
	{
		gnPAR_PwmFreq = pwm_freq;
		if (gnPAR_PwmFreq < PAR_MIN_PWM_FREQ) gnPAR_PwmFreq = PAR_MIN_PWM_FREQ;
		if (gnPAR_PwmFreq > PAR_MAX_PWM_FREQ) gnPAR_PwmFreq = PAR_MAX_PWM_FREQ;
	}

	if (pwm_width >= 0)
	{
		gnPAR_PwmWidth = pwm_width;
		if (gnPAR_PwmWidth < PAR_MIN_PWM_WIDTH) gnPAR_PwmWidth = PAR_MIN_PWM_WIDTH;
		if (gnPAR_PwmWidth > PAR_MAX_PWM_WIDTH_LIMIT) gnPAR_PwmWidth = PAR_MAX_PWM_WIDTH_LIMIT;
	}

	if (burst_freq >= 0)
	{
		gnPAR_PwmBurstFreq = burst_freq;
		if (gnPAR_PwmBurstFreq < PAR_MIN_PWM_BURST_FREQ) gnPAR_PwmBurstFreq = PAR_MIN_PWM_BURST_FREQ;
		if (gnPAR_PwmBurstFreq > PAR_MAX_PWM_BURST_FREQ) gnPAR_PwmBurstFreq = PAR_MAX_PWM_BURST_FREQ;
	}

	if (burst_width >= 0)
	{
		gnPAR_PwmBurstWidth = burst_width;
		if (gnPAR_PwmBurstWidth < PAR_MIN_PWM_BURST_WIDTH) gnPAR_PwmBurstWidth = PAR_MIN_PWM_BURST_WIDTH;
		if (gnPAR_PwmBurstWidth > PAR_MAX_PWM_BURST_WIDTH) gnPAR_PwmBurstWidth = PAR_MAX_PWM_BURST_WIDTH;
	}


	/* PWM. */
	gnPwm_TBPRD = 100000000 / (gnPAR_PwmFreq * 100 * 2);
	gnPwm_Width = gnPwm_TBPRD * gnPAR_PwmWidth / PAR_MAX_PWM_WIDTH;

	if (gnPwm_Width < 0) gnPwm_Width = 0;
	if (gnPwm_Width > gnPwm_TBPRD * PAR_MAX_PWM_WIDTH_LIMIT / PAR_MAX_PWM_WIDTH)
		gnPwm_Width = gnPwm_TBPRD * PAR_MAX_PWM_WIDTH_LIMIT / PAR_MAX_PWM_WIDTH;

	EPwm1Regs.CMPA.half.CMPA = gnPwm_TBPRD - gnPwm_Width;
	EPwm1Regs.CMPB = gnPwm_Width;
	EPwm1Regs.TBPRD = gnPwm_TBPRD;

	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPB = 0;
	EPwm2Regs.TBPRD = gnPwm_TBPRD * 2;

	/* Burst. */
	gnPwm_BurstWidthMax = 100000 / gnPAR_PwmBurstFreq;
	gnPwm_BurstWidth = gnPwm_BurstWidthMax * gnPAR_PwmBurstWidth / PAR_MAX_PWM_BURST_WIDTH;
}


void GetPwm (int32 *pwm_freq, int32 *pwm_width, int32 *burst_freq, int32 *burst_width)
{
	if (pwm_freq != NULL) *pwm_freq = gnPAR_PwmFreq;
	if (pwm_width != NULL) *pwm_width = gnPAR_PwmWidth;
	if (burst_freq != NULL) *burst_freq = gnPAR_PwmBurstFreq;
	if (burst_width != NULL) *burst_width = gnPAR_PwmBurstWidth;
}


void Sort_EC (double src[], double dst[], int32 len)
{
	int32 i;
	int32 n;
	double m;

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


#if (PID_CONTROL == TRUE)

double GetEC ()
{
	int16 i = 0;
	int32 start_time = 0;

	double v25 = 0;
	double sum_plus = 0;
	double sum_minus = 0;

	double ec = 0;
	double ret = 0;

	if (gnPAR_PwmWidth < ADC_CHECK_PWM_WIDTH)
	{
		gnAdcA0_DataIndex = ADC_CURRENT_RES * 3 / 10;
	}
	else
	{
		gnAdcA0_DataIndex = 0;
	}

	for (i = 0; i < ADC_CURRENT_RES; i++) gnAdcA0_Data[i] = 0;

	start_time = gnTimer_10us;
	EPwm2Regs.ETSEL.bit.SOCAEN = 1;

	while (TRUE)
	{
		if (EPwm2Regs.ETSEL.bit.SOCAEN == 0) break;

		if (gnTimer_10us > start_time + 100000 || gnTimer_10us < start_time)
		{
			gfPower_EC_Data[gnPower_EC_Index % POWER_EC_DATA_LEN] = 0;
			gnPower_EC_Index++;
			goto GETEC_TIME_OUT;
		}
	}

#define GEC_LOGIC	1

#if (GEC_LOGIC == 1)	// PWM On인 구간만 검사.
#define ADC_CURRENT_LIMIT			426		// about 3A
#define ADC_DELAY_SAMPLES			3

	int16 start_sample;
	int16 available_samples;

	double avr_plus;
	double avr_minus;

	/* Limit. */
	v25 = 4096.0 * 2.5 / 3.0;

	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		gnAdcA0_Data_Limit[i] = gnAdcA0_Data[i] - v25;

		if (gnAdcA0_Data_Limit[i] > ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data_Limit[i] = ADC_CURRENT_LIMIT;
		}
		if (gnAdcA0_Data_Limit[i] < -ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data_Limit[i] = -ADC_CURRENT_LIMIT;
		}
	}

	/* plus. */
	start_sample = ADC_CURRENT_RES / 2 - ADC_DELAY_SAMPLES;
	available_samples = ADC_CURRENT_RES / 2 * gnPAR_PwmWidth / PAR_MAX_PWM_WIDTH;

	sum_plus = 0;
	for (i = start_sample; i > start_sample - available_samples; i--)
	{
		if (gnAdcA0_Data_Limit[i] > 0)
			sum_plus += gnAdcA0_Data_Limit[i];
	}
	avr_plus = sum_plus / (double)available_samples;

	for (i = start_sample; i > start_sample - available_samples; i--)
	{
		if (gnAdcA0_Data_Limit[i] < avr_plus) gnAdcA0_Data_Limit[i] = avr_plus;
	}

	sum_plus = 0;
	for (i = start_sample; i > start_sample - available_samples; i--)
	{
		sum_plus += gnAdcA0_Data_Limit[i];
	}
	avr_plus = sum_plus / (double)available_samples;

	/* minus. */
	start_sample = ADC_CURRENT_RES - ADC_DELAY_SAMPLES;
	available_samples = ADC_CURRENT_RES / 2 * gnPAR_PwmWidth / PAR_MAX_PWM_WIDTH;

	sum_minus = 0;
	for (i = start_sample; i > start_sample - available_samples; i--)
	{
		if (gnAdcA0_Data_Limit[i] < 0)
			sum_minus += gnAdcA0_Data_Limit[i];
	}
	avr_minus = sum_minus / (double)available_samples;

	for (i = start_sample; i > start_sample - available_samples; i--)
	{
		if (gnAdcA0_Data_Limit[i] > avr_minus) gnAdcA0_Data_Limit[i] = avr_minus;
	}

	sum_minus = 0;
	for (i = start_sample; i > start_sample - available_samples; i--)
	{
		sum_minus += gnAdcA0_Data_Limit[i];
	}
	avr_minus = sum_minus / (double)available_samples;

	avr_plus = avr_plus * (gnPAR_PwmWidth + (PAR_MAX_PWM_WIDTH * 0.1)) / PAR_MAX_PWM_WIDTH;
	avr_minus = avr_minus * (gnPAR_PwmWidth + (PAR_MAX_PWM_WIDTH * 0.1)) / PAR_MAX_PWM_WIDTH;

	ec = (avr_plus - avr_minus) * 2.0 / 2.0 / 4096.0 * 3.0 / 0.104;
	if (ec < 0) ec = -ec;

	gfPower_EC_Data[gnPower_EC_Index % POWER_EC_DATA_LEN] = ec;
	gnPower_EC_Index++;

#elif (GEC_LOGIC == 2)	// 전류값의 변경폭에 제한, burst와 CW사이에서 오차가 많이 발생.
#define ADC_CURRENT_LIMIT			284	// about 2A
#define ADC_SWING_LIMIT				0	// about 0.5A
#define ADC_UP_CURRENT_DELTA		71	// about 0.5A
#define ADC_DOWN_CURRENT_DELTA		43	// about 0.3A
#define ADC_SWING_DUR				5

	int16 n;
	int16 m;
	int32 mid;
	int32 ec_limit;

	double avr;
	double sum_avr;

	/* Limit. */
	v25 = 4096.0 * 2.5 / 3.0;
	mid = ADC_CURRENT_RES / 2;

	for (i = 0; i < mid; i++)
	{
		// upper, lower limit.
		if (i < ADC_SWING_DUR) ec_limit = ADC_SWING_LIMIT;
		else ec_limit = ADC_CURRENT_LIMIT;

		if (gnAdcA0_Data[i] > v25 + ec_limit)
		{
			gnAdcA0_Data_Limit[i] = v25 + ec_limit;
		}
		else if (gnAdcA0_Data[i] < v25)
		{
			gnAdcA0_Data_Limit[i] = v25;
		}
		else gnAdcA0_Data_Limit[i] = gnAdcA0_Data[i];

		// apply down-delta.
		if (i > 1)
		{
			if (gnAdcA0_Data_Limit[i] < gnAdcA0_Data_Limit[i - 1] - ADC_DOWN_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] - ADC_DOWN_CURRENT_DELTA;
			}
			if (gnAdcA0_Data_Limit[i] > gnAdcA0_Data_Limit[i - 1] + ADC_UP_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] + ADC_UP_CURRENT_DELTA;
			}
		}
	}

	for (i = mid; i < ADC_CURRENT_RES; i++)
	{
		// upper, lower limit.
		if (i < mid + ADC_SWING_DUR) ec_limit = ADC_SWING_LIMIT;
		else ec_limit = ADC_CURRENT_LIMIT;

		if (gnAdcA0_Data[i] < v25 - ec_limit)
		{
			gnAdcA0_Data_Limit[i] = v25 - ec_limit;
		}
		else if (gnAdcA0_Data[i] > v25)
		{
			gnAdcA0_Data_Limit[i] = v25;
		}
		else gnAdcA0_Data_Limit[i] = gnAdcA0_Data[i];

		// apply up-delta.
		if (i > mid + 1)
		{
			if (gnAdcA0_Data_Limit[i] > gnAdcA0_Data_Limit[i - 1] + ADC_DOWN_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] + ADC_DOWN_CURRENT_DELTA;
			}
			if (gnAdcA0_Data_Limit[i] < gnAdcA0_Data_Limit[i - 1] - ADC_UP_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] - ADC_UP_CURRENT_DELTA;
			}
		}
	}

	/* Average. */
	/*
	for (i = ADC_CURRENT_RES; i < (ADC_CURRENT_RES * 2); i++)
	{
		sum_avr = 0;
		for (n = 0; n < ADC_CURRENT_AVR_NUM; n++)
		{
			m = (i - n) % ADC_CURRENT_RES;
			sum_avr += gnAdcA0_Data_Limit[m];
		}
		gnAdcA0_Data_Avr[(i % ADC_CURRENT_RES)] = sum_avr / (double)ADC_CURRENT_AVR_NUM;
	}
	*/

	/* Sum */
	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		gnAdcA0_Data_Limit[i] -= v25;
		if (gnAdcA0_Data_Limit[i] > -ADC_CURRENT_ZERO_RANGE && gnAdcA0_Data_Limit[i] < ADC_CURRENT_ZERO_RANGE)
		{
			gnAdcA0_Data_Limit[i] = 0;
		}
	}

	sum_plus = 0;
	for (i = 0; i < mid; i++)
	{
		sum_plus += gnAdcA0_Data_Limit[i];
	}

	sum_minus = 0;
	for (i = mid; i < ADC_CURRENT_RES; i++)
	{
		sum_minus += gnAdcA0_Data_Limit[i];
	}

	/* Electric current. */
	avr = (sum_plus - sum_minus) / (double)ADC_CURRENT_RES;
	ec = avr / 4096.0 * 3.0 / 0.104;

	gfPower_EC_Data[gnPower_EC_Index % POWER_EC_DATA_LEN] = ec;
	gnPower_EC_Index++;


#elif (GEC_LOGIC == 3) // 평균값으로 계산, burst와 CW사이에서 오차가 많이 발생.

#define ADC_MAX_CURRENT_DELTA		47	// about 0.33A

	int16 n;
	int16 m;

	double v_diff;
	double sum_avr;
	double avr;

	/* Limit. */
	v25 = 4096.0 * 2.5 / 3.0;
	gnAdcA0_Data_Limit[0] = v25;

	for (i = 1; i < ADC_CURRENT_RES; i++)
	{
		if (gnAdcA0_Data[i] > gnAdcA0_Data_Limit[i - 1] + ADC_MAX_CURRENT_DELTA)
		{
			gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] + ADC_MAX_CURRENT_DELTA;
		}
		else if (gnAdcA0_Data[i] < gnAdcA0_Data_Limit[i - 1] - ADC_MAX_CURRENT_DELTA)
		{
			gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] - ADC_MAX_CURRENT_DELTA;
		}
		else gnAdcA0_Data_Limit[i] = gnAdcA0_Data[i];
	}

	/*
	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		if (gnAdcA0_Data[i] > v25 + ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data[i] = v25 + ADC_CURRENT_LIMIT;
		}

		if (gnAdcA0_Data[i] < v25 - ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data[i] = v25 - ADC_CURRENT_LIMIT;
		}
	}
	*/

	/* Average. */
	for (i = ADC_CURRENT_RES; i < (ADC_CURRENT_RES * 2); i++)
	{
		sum_avr = 0;
		for (n = 0; n < ADC_CURRENT_AVR_NUM; n++)
		{
			m = (i - n) % ADC_CURRENT_RES;
			sum_avr += gnAdcA0_Data_Limit[m];
		}
		gnAdcA0_Data_Avr[(i % ADC_CURRENT_RES)] = sum_avr / (double)ADC_CURRENT_AVR_NUM;
	}

	/* Sum. */
	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		gnAdcA0_Data_Avr[i] -= v25;
		v_diff = gnAdcA0_Data_Avr[i];

		if (v_diff > -ADC_CURRENT_ZERO_RANGE && v_diff < ADC_CURRENT_ZERO_RANGE) continue;

		if (v_diff > 0) sum_plus += v_diff;
		else sum_minus += -v_diff;
	}

	/* Sort. */
	/*
	for (i = 0; i < ADC_CURRENT_RES - 1; i++)
	{
		for (n = 0; n < ADC_CURRENT_RES - i - 1; n++)
		{
			if (gnAdcA0_Data_Avr[n] < gnAdcA0_Data_Avr[n + 1])
			{
				m = gnAdcA0_Data_Avr[n];
				gnAdcA0_Data_Avr[n] = gnAdcA0_Data_Avr[n + 1];
				gnAdcA0_Data_Avr[n + 1] = m;
			}
		}
	}
	*/

	/* Electric current. */
	avr = (sum_plus + sum_minus) / (double)ADC_CURRENT_RES;
	ec = avr * 3.0 / 4096.0 / 0.104;

	gfPower_EC_Data[gnPower_EC_Index % POWER_EC_DATA_LEN] = ec;
	gnPower_EC_Index++;

#endif

GETEC_TIME_OUT:

	if (gnPower_EC_Index < POWER_EC_DATA_LEN)
	{
		ret = 0;
		for (i = 0; i < gnPower_EC_Index; i++) ret += gfPower_EC_Data[i];

		ret = ret / (double)gnPower_EC_Index;
	}
	else
	{
		int remove_num = 10;

		Sort_EC (gfPower_EC_Data, gfPower_EC_Data_Sort, POWER_EC_DATA_LEN);

		ret = 0;
		for (i = remove_num; i < POWER_EC_DATA_LEN - remove_num; i++) ret += gfPower_EC_Data_Sort[i];

		ret = ret / (double)(POWER_EC_DATA_LEN - (remove_num * 2));
	}

	return ret * ((double)gnPAR_PwmBurstWidth / (double)PAR_MAX_PWM_BURST_WIDTH);

	//int32 sum_len = gnPAR_PwmWidth / 2 / 10;
	//for (i = 0; i < sum_len; i++) sum += gnAdcA0_Data[i];
	//return sum / sum_len;

	//return 1.0 * ((double)gnPAR_PwmWidth / PAR_MAX_PWM_WIDTH) * ((double)gnPAR_PwmBurstWidth / PAR_MAX_PWM_BURST_WIDTH);

	/*
	for (i = 0; i < ADC_CURRENT_RES; i++) sum += gnAdcA0_Data[i];

	// Electric current.
	double avr = (double)sum / ADC_CURRENT_RES;
	double ec = avr - (2.5 / 3.0 * 4096.0);

	if (ec < 0) gfPower_EC_Data[gnPower_EC_Index] = -ec;
	else gfPower_EC_Data[gnPower_EC_Index] = ec;

	if (++gnPower_EC_Index >= POWER_EC_DATA_LEN) gnPower_EC_Index = 0;

	double ec_sum = 0;
	for (i = 0; i < POWER_EC_DATA_LEN; i++) ec_sum += gfPower_EC_Data[i];

	return ec_sum / POWER_EC_DATA_LEN;
	*/
}

#else

#if (CTRL_FREQ == TRUE)
#define ADC_CURRENT_LIMIT			422
#else
#define ADC_CURRENT_LIMIT			426		// 3A
#endif

#define ADC_EC_REAL2INT_RES			10000

void CalEC_OneCycle ()
{
	static int32 pwm_width_old = 0;
	int32 i = 0;
	int32 n = 0;

	if (gnPAR_PwmWidth == 0)
	{
		pwm_width_old = 0;
		EPwm2Regs.ETSEL.bit.SOCAEN = 0;
		return;
	}

	if (pwm_width_old == 0 && gnPAR_PwmWidth > 0)
	{
		gnAdcA0_DataIndex = 0;
		for (i = 0; i < ADC_CURRENT_RES; i++) gnAdcA0_Data[i] = 0;
		EPwm2Regs.ETSEL.bit.SOCAEN = 1;
	}

	pwm_width_old = gnPAR_PwmWidth;

	if (EPwm2Regs.ETSEL.bit.SOCAEN == 1) return;

	/* v25. */
	int16 v25 = (int16)(4096.0 * 2.5 / 3.0);

	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		gnAdcA0_Data[i] = gnAdcA0_Data[i] - v25;
	}

	/* Limit & sum */
	int32 half_res = ADC_CURRENT_RES / 2;

#if (SIG_TRANS_TYPE == SIG_TRANS_TYPE_OLD)
	int32 pwm_width_res = half_res * (gnPAR_PwmWidth - 30) / PAR_MAX_PWM_WIDTH;
	int32 start_pos = half_res * (PAR_MAX_PWM_WIDTH - gnPAR_PwmWidth + 150) / PAR_MAX_PWM_WIDTH;
#elif (SIG_TRANS_TYPE == SIG_TRANS_TYPE_NEW)
	int32 noise_range = gnPwm_ErrRange;
	int32 pwm_width_res = half_res * (gnPAR_PwmWidth - noise_range) / PAR_MAX_PWM_WIDTH;
	int32 start_pos = half_res * (PAR_MAX_PWM_WIDTH - gnPAR_PwmWidth + noise_range) / PAR_MAX_PWM_WIDTH;
#endif

	int32 end_pos = start_pos + pwm_width_res;

	int32 sum_plus = 0;
	for (i = start_pos; i < end_pos; i++)
	{
		if (gnAdcA0_Data[i] > ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data[i] = ADC_CURRENT_LIMIT;
		}
		else if (gnAdcA0_Data[i] < 0)
		{
			gnAdcA0_Data[i] = 0;
		}

		sum_plus += gnAdcA0_Data[i];
	}

	start_pos = half_res + start_pos;
	end_pos = start_pos + pwm_width_res;

	int32 sum_minus = 0;
	for (i = start_pos; i < end_pos; i++)
	{
		n = i % ADC_CURRENT_RES;

		if (gnAdcA0_Data[n] < -ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data[n] = -ADC_CURRENT_LIMIT;
		}
		else if (gnAdcA0_Data[n] > 0)
		{
			gnAdcA0_Data[n] = 0;
		}

		sum_minus += gnAdcA0_Data[n];
	}

	/* Electric current. */
	float64 avr_ec = 0;
	if (sum_plus > -sum_minus)
	{
		avr_ec = (float64)sum_plus / (float64)pwm_width_res;
	}
	else
	{
		avr_ec = (float64)(-sum_minus) / (float64)pwm_width_res;
	}

#if (CTRL_FREQ == TRUE)
	avr_ec = avr_ec / 4096.0 * 3.0 / 0.108;
#else
	avr_ec = avr_ec / 4096.0 * 3.0 / 0.104;
#endif

	gnPower_EC_Data[gnPower_EC_Index % POWER_EC_DATA_LEN] = (Uint16)(avr_ec * ADC_EC_REAL2INT_RES);
	gnPower_EC_Index++;

	EPwm2Regs.ETSEL.bit.SOCAEN = 1;
}

float64 GetEC_OneCycle ()
{
	int i;
	float64 avr_ec = 0;

	if (gnPower_EC_Index < POWER_EC_DATA_LEN)
	{
		avr_ec = 0;
		for (i = 0; i < gnPower_EC_Index; i++) avr_ec += gnPower_EC_Data[i];

		avr_ec = avr_ec / (float64)gnPower_EC_Index / (float64)ADC_EC_REAL2INT_RES;
	}
	else
	{
		/*
		int remove_num = 20;

		Sort_EC (gnPower_EC_Data, gnPower_EC_Data_Sort, POWER_EC_DATA_LEN);

		ret = 0;
		for (i = remove_num; i < POWER_EC_DATA_LEN - remove_num; i++) ret += gnPower_EC_Data_Sort[i];

		ret = ret / (double)(POWER_EC_DATA_LEN - (remove_num * 2));
		*/

		avr_ec = 0;
		for (i = 0; i < POWER_EC_DATA_LEN; i++) avr_ec += gnPower_EC_Data[i];

		avr_ec = avr_ec / (float64)POWER_EC_DATA_LEN / (float64)ADC_EC_REAL2INT_RES;
	}

#if (SIG_TRANS_TYPE == SIG_TRANS_TYPE_OLD)
	float64 ret = avr_ec * (float64)(gnPAR_PwmWidth + 120) / (float64)PAR_MAX_PWM_WIDTH * 1.1 + 0.1;
#elif (SIG_TRANS_TYPE == SIG_TRANS_TYPE_NEW)
#if (CTRL_FREQ == TRUE)
	float64 ret = avr_ec * (float64)gnPAR_PwmWidth / (float64)PAR_MAX_PWM_WIDTH * 1.19 + 0.177;
#else
	float64 ret = avr_ec * (float64)gnPAR_PwmWidth / (float64)PAR_MAX_PWM_WIDTH * 1.2 + 0.18;
#endif
#endif

	return ret;
}


#if FALSE

#define ADC_UP_CURRENT_DELTA		28		// 0.2A
#define ADC_DOWN_CURRENT_DELTA		14		// 0.1A

void CalEC_OneCycle_1 ()
{
	static int32 pwm_width_old = 0;
	int32 i = 0;

	if (gnPAR_PwmWidth == 0)
	{
		pwm_width_old = 0;
		EPwm2Regs.ETSEL.bit.SOCAEN = 0;
		return;
	}

	if (pwm_width_old == 0 && gnPAR_PwmWidth > 0)
	{
		gnAdcA0_DataIndex = 0;
		for (i = 0; i < ADC_CURRENT_RES; i++) gnAdcA0_Data[i] = 0;
		EPwm2Regs.ETSEL.bit.SOCAEN = 1;
	}

	pwm_width_old = gnPAR_PwmWidth;

	if (EPwm2Regs.ETSEL.bit.SOCAEN == 1) return;

	/* Shift. */
#if (SIG_TRANS_TYPE == SIG_TRANS_TYPE_OLD)
	int32 shift_samples = ADC_CURRENT_RES * 8 / 100;	// 8% shift left.
#elif (SIG_TRANS_TYPE == SIG_TRANS_TYPE_NEW)
	int32 shift_samples = ADC_CURRENT_RES * 2 / 100;	// 2% shift left.
#endif

	int32 n = 0;
	double v25 = 4096.0 * 2.5 / 3.0;

	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		n = (ADC_CURRENT_RES + i + shift_samples) % ADC_CURRENT_RES;
		gnAdcA0_Data_Limit[i] = gnAdcA0_Data[n] - v25;
	}

	/* Limit. */
	int32 mid = ADC_CURRENT_RES / 2;
	int32 noise_width = ADC_CURRENT_RES * (PAR_MAX_PWM_WIDTH - PAR_MAX_PWM_WIDTH_LIMIT) / 2 / PAR_MAX_PWM_WIDTH;	// 5% (half cycle)

	int32 margin_samples = ADC_CURRENT_RES * 6 / 100;	// 6% margin. (half cycle)
	int32 pulse_samples = ADC_CURRENT_RES * gnPAR_PwmWidth / 2 / PAR_MAX_PWM_WIDTH + margin_samples;

	if (pulse_samples > ((int32)ADC_CURRENT_RES * PAR_MAX_PWM_WIDTH_LIMIT / 2 / PAR_MAX_PWM_WIDTH))
		pulse_samples = (int32)ADC_CURRENT_RES * PAR_MAX_PWM_WIDTH_LIMIT / 2 / PAR_MAX_PWM_WIDTH;

	for (i = 0; i < mid; i++)
	{
		if (i < noise_width)
		{
			gnAdcA0_Data_Limit[i] = 0;
		}
		else if (gnAdcA0_Data_Limit[i] > ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data_Limit[i] = ADC_CURRENT_LIMIT;
		}
		else if (gnAdcA0_Data_Limit[i] < 0)
		{
			gnAdcA0_Data_Limit[i] = 0;
		}

		if (i < mid - pulse_samples)
		{
			gnAdcA0_Data_Limit[i] /= 2;	// average V = max. V / 2
		}

		// apply up/down delta.
		/*
		if (i > 1)
		{
			if (gnAdcA0_Data_Limit[i] > gnAdcA0_Data_Limit[i - 1] + ADC_UP_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] + ADC_UP_CURRENT_DELTA;
			}
			if (gnAdcA0_Data_Limit[i] < gnAdcA0_Data_Limit[i - 1] - ADC_DOWN_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] - ADC_DOWN_CURRENT_DELTA;
			}
		}
		*/
	}

	for (i = mid; i < ADC_CURRENT_RES; i++)
	{
		if (i < mid + noise_width)
		{
			gnAdcA0_Data_Limit[i] = 0;
		}
		else if (gnAdcA0_Data_Limit[i] < -ADC_CURRENT_LIMIT)
		{
			gnAdcA0_Data_Limit[i] = -ADC_CURRENT_LIMIT;
		}
		else if (gnAdcA0_Data_Limit[i] > 0)
		{
			gnAdcA0_Data_Limit[i] = 0;
		}

		if (i < ADC_CURRENT_RES - pulse_samples)
		{
			gnAdcA0_Data_Limit[i] /= 2;	// average V = max. V / 2
		}

		// apply up/down delta.
		/*
		if (i > mid + 1)
		{
			if (gnAdcA0_Data_Limit[i] < gnAdcA0_Data_Limit[i - 1] - ADC_UP_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] - ADC_UP_CURRENT_DELTA;
			}
			if (gnAdcA0_Data_Limit[i] > gnAdcA0_Data_Limit[i - 1] + ADC_DOWN_CURRENT_DELTA)
			{
				gnAdcA0_Data_Limit[i] = gnAdcA0_Data_Limit[i - 1] + ADC_DOWN_CURRENT_DELTA;
			}
		}
		*/
	}

	/* Average. */
	/*
	int16 m = 0;
	int32 sum_avr = 0;
	for (i = ADC_CURRENT_RES; i < (ADC_CURRENT_RES * 2); i++)
	{
		sum_avr = 0;
		for (n = -ADC_CURRENT_AVR_NUM; n < ADC_CURRENT_AVR_NUM; n++)
		{
			m = (i + n) % ADC_CURRENT_RES;
			sum_avr += gnAdcA0_Data_Limit[m];
		}
		gnAdcA0_Data[(i % ADC_CURRENT_RES)] = sum_avr / (ADC_CURRENT_AVR_NUM * 2);
	}
	*/

	/*
	for (i = 0; i < mid; i++)
	{
		if (i < noise_width)
		{
			gnAdcA0_Data[i] = 0;
		}

		// apply up/down delta.
		if (i > 1)
		{
			if (gnAdcA0_Data[i] > gnAdcA0_Data[i - 1] + ADC_UP_CURRENT_DELTA)
			{
				gnAdcA0_Data[i] = gnAdcA0_Data[i - 1] + ADC_UP_CURRENT_DELTA;
			}
			if (gnAdcA0_Data[i] < gnAdcA0_Data[i - 1] - ADC_DOWN_CURRENT_DELTA)
			{
				gnAdcA0_Data[i] = gnAdcA0_Data[i - 1] - ADC_DOWN_CURRENT_DELTA;
			}
		}
	}

	for (i = mid; i < ADC_CURRENT_RES; i++)
	{
		if (i < mid + noise_width)
		{
			gnAdcA0_Data[i] = 0;
		}

		// apply up/down delta.
		if (i > mid + 1)
		{
			if (gnAdcA0_Data[i] < gnAdcA0_Data[i - 1] - ADC_UP_CURRENT_DELTA)
			{
				gnAdcA0_Data[i] = gnAdcA0_Data[i - 1] - ADC_UP_CURRENT_DELTA;
			}
			if (gnAdcA0_Data[i] > gnAdcA0_Data[i - 1] + ADC_DOWN_CURRENT_DELTA)
			{
				gnAdcA0_Data[i] = gnAdcA0_Data[i - 1] + ADC_DOWN_CURRENT_DELTA;
			}
		}
	}
	*/

	/* Sum. */
	/*
	int32 margin_samples = ADC_CURRENT_RES * 6 / 100;	// 6% margin. (half cycle)
	int32 pulse_samples = ADC_CURRENT_RES * gnPAR_PwmWidth / 2 / PAR_MAX_PWM_WIDTH + margin_samples;
	int32 start_pos = 0;
	int32 sum_plus = 0;
	int32 sum_minus = 0;

	if (pulse_samples > ((int32)ADC_CURRENT_RES * PAR_MAX_PWM_WIDTH_LIMIT / 2 / PAR_MAX_PWM_WIDTH))
		pulse_samples = (int32)ADC_CURRENT_RES * PAR_MAX_PWM_WIDTH_LIMIT / 2 / PAR_MAX_PWM_WIDTH;

	start_pos = mid - pulse_samples;
	for (i = start_pos; i < mid; i++)
	{
		sum_plus += gnAdcA0_Data_Limit[i];
	}

	start_pos = ADC_CURRENT_RES - pulse_samples;
	for (i = start_pos; i < ADC_CURRENT_RES; i++)
	{
		sum_minus += gnAdcA0_Data_Limit[i];
	}
	*/

	/*
	for (i = 0; i < ADC_CURRENT_RES; i++)
	{
		if (gnAdcA0_Data[i] > -ADC_CURRENT_ZERO_RANGE && gnAdcA0_Data[i] < ADC_CURRENT_ZERO_RANGE)
		{
			gnAdcA0_Data[i] = 0;
		}
	}
	*/

	int32 sum_plus = 0;
	for (i = 0; i < mid; i++)
	{
		sum_plus += gnAdcA0_Data_Limit[i];
	}

	int32 sum_minus = 0;
	for (i = mid; i < ADC_CURRENT_RES; i++)
	{
		sum_minus += gnAdcA0_Data_Limit[i];
	}

	/* Electric current. */
	float64 avr = (float64)(sum_plus - sum_minus) / (float64)ADC_CURRENT_RES;
	float64 ec = avr / 4096.0 * 3.0 / 0.104;

	gnPower_EC_Data[gnPower_EC_Index % POWER_EC_DATA_LEN] = (Uint16)(ec * ADC_EC_REAL2INT_RES);
	gnPower_EC_Index++;

	EPwm2Regs.ETSEL.bit.SOCAEN = 1;
}
#endif
#endif


void ResetPwm ()
{
	/*
	gnPAR_PwmWidth = PAR_DEF_PWM_WIDTH;
	gnPAR_PwmFreq = PAR_DEF_PWM_FREQ;
	gnPAR_PwmBurstWidth = PAR_DEF_PWM_BURST_WIDTH;
	gnPAR_PwmBurstFreq = PAR_DEF_PWM_BURST_FREQ;

	SetPwm (-1, -1, -1, -1);
	*/
	ResetPwm_Freq (1);
}

void ResetPwm_Freq (int32 is_freq)
{
	gnPAR_PwmWidth = PAR_DEF_PWM_WIDTH;
	if (is_freq > 0) gnPAR_PwmFreq = PAR_DEF_PWM_FREQ;
	gnPAR_PwmBurstWidth = PAR_DEF_PWM_BURST_WIDTH;
	gnPAR_PwmBurstFreq = PAR_DEF_PWM_BURST_FREQ;

	SetPwm (-1, -1, -1, -1);
}

void ResetECData ()
{
	int i = 0;

	gnPower_EC_Index = 0;
	for (i = 0; i < POWER_EC_DATA_LEN; i++) gnPower_EC_Data[i] = 0;
}

void Sleep10us (int32 time_10us)
{
	int32 start = gnTimer_10us;

	while (TRUE)
	{
		if (gnTimer_10us >= start)
		{
			if (gnTimer_10us - start > time_10us) break;
		}
		else
		{
			if (((TIMER_10US_MAX - start) + gnTimer_10us) > time_10us) break;
		}
	}
}





