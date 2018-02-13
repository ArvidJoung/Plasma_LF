#include "DSP280x_Device.h"     // Headerfile Include File
#include "DSP280x_Examples.h"   // Examples Include File
#include "PwmCtrl.h"

void Set_Pwm1_Type_280x (Uint16 type);

void Init_Pwm1_280x (Uint16 prd)
{
	/* ePWM1/ePWM2 PIN */
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;				// Configure GPIO0 as EPWM1A
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;				// Configure GPIO1 as EPWM1B
	EDIS;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	/* ePWM1/ePWM2 Time-Base Submodule */
	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;		// set Immediate load
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)]
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;			// TBCLK = [100MHz / (1*1)] = 100MHz
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
	EPwm1Regs.TBPRD = prd;			  	    // Set period for ePWM1 (PWM 캐리어 주파수 : 100MHz/(1000 * 2) = 50kHz)
	EPwm1Regs.TBCTR = 0;							// Clear counter
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;

	/*
	EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;		// set Immediate load
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)]
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;			// TBCLK = [100MHz / (1*1)] = 100MHz
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm2Regs.TBPRD = gnPwm_TBPRD * 2;		  	    // Set period for ePWM2 (PWM 캐리어 주파수 : 100MHz/(1000 * 2) = 50kHz)
	EPwm2Regs.TBCTR = 0;							// Clear counter
	EPwm2Regs.TBPHS.all = 0;
	EPwm2Regs.TBCTL.bit.PHSEN = 1;
	EPwm2Regs.TBCTL.bit.PHSDIR = 1;
	*/

	/* ePWM1/ePWM2 Counter-Compare Submodule */
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// 비교 레지스터에 Shadow 레지스터 사용
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	// 카운터가 0 일때 Shadow 레지스터에서 비교 레지스터에 비교 값 로드

	/*
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// 비교 레지스터에 Shadow 레지스터 사용
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	// 카운터가 0 일때 Shadow 레지스터에서 비교 레지스터에 비교 값 로드
	*/

	/* ePWM1 Action-qualifier */
	/*
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;			// 카운터가 제로일 때 High
	*/
	Set_Pwm1_Type_280x (0);

	/*
	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;				// 카운터가 제로일 때 High
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	*/

	/* ePWM2 Event-Trigger Submodule */
	/*
	EPwm2Regs.ETSEL.bit.SOCAEN = 0;     			// SOCA 이벤트 트리거 Enable
	EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;  	// SCCA 트리거 조건
	EPwm2Regs.ETPS.bit.SOCACNT = 1;
	EPwm2Regs.ETPS.bit.SOCAPRD = ET_1ST;			// SOCA 이벤트 분주 설정 : 트리거 조건 한번 마다
	*/

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

void Set_Pwm1_Cmp_280x (Uint16 a, Uint16 b, Uint16 prd)
{
	/*
	// check parameter.
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

	// PWM.
	gnPwm_TBPRD = 100000000 / (gnPAR_PwmFreq * 100 * 2);
	gnPwm_Width = gnPwm_TBPRD * gnPAR_PwmWidth / PAR_MAX_PWM_WIDTH;

	if (gnPwm_Width < 0) gnPwm_Width = 0;
	if (gnPwm_Width > gnPwm_TBPRD * PAR_MAX_PWM_WIDTH_LIMIT / PAR_MAX_PWM_WIDTH)
		gnPwm_Width = gnPwm_TBPRD * PAR_MAX_PWM_WIDTH_LIMIT / PAR_MAX_PWM_WIDTH;

	EPwm1Regs.CMPA.half.CMPA = gnPwm_TBPRD - gnPwm_Width;
	EPwm1Regs.CMPB = gnPwm_Width;
	EPwm1Regs.TBPRD = gnPwm_TBPRD;
	*/

	EPwm1Regs.CMPA.half.CMPA = a;
	EPwm1Regs.CMPB = b;
	EPwm1Regs.TBPRD = prd;

	/* >>[dkht] 테스트 코드.
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.CBU = AQ_NO_ACTION;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.CBD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;			// 카운터가 제로일 때 High
	EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.CBU = AQ_NO_ACTION;				// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.CBD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
	EPwm1Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;			// 카운터가 제로일 때 High
	<<[]*/


	/*
	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPB = 0;
	EPwm2Regs.TBPRD = gnPwm_TBPRD * 2;

	// Burst.
	gnPwm_BurstWidthMax = 100000 / gnPAR_PwmBurstFreq;
	gnPwm_BurstWidth = gnPwm_BurstWidthMax * gnPAR_PwmBurstWidth / PAR_MAX_PWM_BURST_WIDTH;
	*/
}

void Set_Pwm1_Type_280x (Uint16 type)
{
	if (type == 0)
	{
		EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;				// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLA.bit.CAD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLA.bit.CBU = AQ_NO_ACTION;				// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLA.bit.CBD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;			// 카운터가 제로일 때 High
		EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR;			// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLB.bit.CAU = AQ_NO_ACTION;				// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLB.bit.CAD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLB.bit.CBU = AQ_NO_ACTION;				// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;			// 카운터가 상승할 때 비교값이 일치하면 Low
		EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;			// 카운터가 제로일 때 High
		EPwm1Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;			// 카운터가 상승할 때 비교값이 일치하면 Low
	}
	else
	{

	}
}

void Check_Pwm1_280x (int32 t10us)
{




}

