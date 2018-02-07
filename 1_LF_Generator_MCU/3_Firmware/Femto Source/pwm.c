/*
 * pwm.c
 *
 *  Created on: 2013. 6. 19.
 *      Author: lastdkht
 */

#include "DSP2833x_Device.h"     // Headerfile Include File
#include "DSP2833x_Examples.h"   // Examples Include File
#include "pwm.h"
#include <math.h>

#define TRUE					1
#define FALSE					0


int InitPwm_2833x (int32 id, Uint16 ctrmode, Uint16 phsen, union AQCTL_REG aqctla, union AQCTL_REG aqctlb)
{
	struct EPWM_REGS PwmRegs;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	if (id == 1) PwmRegs = EPwm1Regs;
	if (id == 2) PwmRegs = EPwm2Regs;
	if (id == 5) PwmRegs = EPwm5Regs;
	if (id == 6) PwmRegs = EPwm6Regs;

	/* ePWM1/ePWM2 Time-Base Submodule */
	PwmRegs.TBCTL.bit.PRDLD = TB_IMMEDIATE;			// set Immediate load
	PwmRegs.TBCTL.bit.HSPCLKDIV = TB_DIV1;			// TBCLK = [SYSCLKOUT / (HSPCLKDIV * CLKDIV)]
	PwmRegs.TBCTL.bit.CLKDIV = TB_DIV1;				// TBCLK = [100MHz / (1*1)] = 100MHz
	PwmRegs.TBCTL.bit.CTRMODE = ctrmode;
	PwmRegs.TBPRD = 1000;
	PwmRegs.TBCTR = 0;								// Clear counter

	//if (id == 1)
	PwmRegs.TBPHS.half.TBPHS = 0;					// Phase is 0 */

	if (phsen == 1)
	{
		PwmRegs.TBCTL.bit.PHSEN = 1;
		PwmRegs.TBCTL.bit.PHSDIR = 1;
	}
	else
	{
		PwmRegs.TBCTL.bit.SYNCOSEL = 1;
		PwmRegs.TBCTL.bit.PHSEN = 0;
	}

	/* ePWM Counter-Compare Submodule */
	PwmRegs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// 비교 레지스터에 Shadow 레지스터 사용
	PwmRegs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;		// 카운터가 0 일때 Shadow 레지스터에서 비교 레지스터에 비교 값 로드

	/* ePWM Action-qualifier */
	PwmRegs.AQCTLA = aqctla;
	PwmRegs.AQCTLB = aqctlb;

	/* ePWM2 Event-Trigger Submodule */
	if (phsen == 1)
	{
		PwmRegs.ETSEL.bit.SOCAEN = 0;     			// SOCA 이벤트 트리거 Enable
		PwmRegs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;  	// SCCA 트리거 조건
		PwmRegs.ETPS.bit.SOCACNT = 1;
		PwmRegs.ETPS.bit.SOCAPRD = ET_1ST;			// SOCA 이벤트 분주 설정 : 트리거 조건 한번 마다
	}

	if (id == 1) EPwm1Regs = PwmRegs;
	if (id == 2) EPwm2Regs = PwmRegs;
	if (id == 5) EPwm5Regs = PwmRegs;
	if (id == 6) EPwm6Regs = PwmRegs;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

	SetPwm_2833x (id, ctrmode, 50000, 0);

	return 1;
}

int InitPwm_ET_2833x (int32 id, union ETSEL_REG etsel, union ETPS_REG etps)
{
	struct EPWM_REGS PwmRegs;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	if (id == 1) PwmRegs = EPwm1Regs;
	if (id == 2) PwmRegs = EPwm2Regs;
	if (id == 5) PwmRegs = EPwm5Regs;
	if (id == 6) PwmRegs = EPwm6Regs;

	PwmRegs.ETSEL = etsel;
	PwmRegs.ETPS = etps;

	if (id == 1) EPwm1Regs = PwmRegs;
	if (id == 2) EPwm2Regs = PwmRegs;
	if (id == 5) EPwm5Regs = PwmRegs;
	if (id == 6) EPwm6Regs = PwmRegs;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

	return 1;
}

void SetPwm_2833x (int32 id, Uint16 ctrmode, int32 pwm_freq, float32 pwm_width)
{
	int32 freq = 0;
	float32 width = 0;

	/* check parameter. */
	if (pwm_freq >= 0)
	{
		freq = pwm_freq;
		if (freq < MIN_PWM_FREQ_2833x) freq = MIN_PWM_FREQ_2833x;
		if (freq > MAX_PWM_FREQ_2833x) freq = MAX_PWM_FREQ_2833x;
	}

	if (pwm_width >= 0)
	{
		width = pwm_width;
		if (width < MIN_PWM_WIDTH_2833x) width = MIN_PWM_WIDTH_2833x;
		if (width > MAX_PWM_WIDTH_LIMIT_2833x) width = MAX_PWM_WIDTH_LIMIT_2833x; // 삭제 필요.
	}

	/* PWM. */
	int32 tbprd = 0;
	if (ctrmode == TB_COUNT_UPDOWN) tbprd = 150000000 / (freq * 2);
	else tbprd = 150000000 / freq;
	int32 cmp_width = (int32)(tbprd * width / MAX_PWM_WIDTH_2833x);

	if (cmp_width < 0) cmp_width = 0;
	if (cmp_width > tbprd * MAX_PWM_WIDTH_LIMIT_2833x / MAX_PWM_WIDTH_2833x) // 삭제 필요.
		cmp_width = tbprd * MAX_PWM_WIDTH_LIMIT_2833x / MAX_PWM_WIDTH_2833x; // 삭제 필요.

	int cmpa = 0;
	int cmpb = 0;
	if (ctrmode == TB_COUNT_UPDOWN)
	{
		cmpa = tbprd - cmp_width;
		cmpb = cmp_width;
	}
	else
	{
		cmpa = cmp_width;
		cmpb = 0;
	}

	if (id == 1)
	{
		EPwm1Regs.CMPA.half.CMPA = cmpa;
		EPwm1Regs.CMPB = cmpb;
		EPwm1Regs.TBPRD = tbprd;
	}
	else if (id == 2)
	{
		EPwm2Regs.CMPA.half.CMPA = cmpa;
		EPwm2Regs.CMPB = cmpb;
		EPwm2Regs.TBPRD = tbprd;
	}
	else if (id == 5)
	{
		EPwm5Regs.CMPA.half.CMPA = cmpa;
		EPwm5Regs.CMPB = cmpb;
		EPwm5Regs.TBPRD = tbprd;
	}
	else if (id == 6)
	{
		EPwm6Regs.CMPA.half.CMPA = cmpa;
		EPwm6Regs.CMPB = cmpb;
		EPwm6Regs.TBPRD = tbprd;
	}
}



