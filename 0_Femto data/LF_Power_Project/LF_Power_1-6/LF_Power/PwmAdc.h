#ifndef PWMADC_H_
#define PWMADC_H_

#include "Global.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_TRANS_TYPE_OLD		1
#define SIG_TRANS_TYPE_NEW		2
#define SIG_TRANS_TYPE			SIG_TRANS_TYPE_NEW

/* PWM */
#define PAR_MIN_PWM_WIDTH			0 		// Unit: 0.1%

#if (SIG_TRANS_TYPE == SIG_TRANS_TYPE_OLD)
#define PAR_MIN_PWM_WIDTH_LIMIT		80 		// Unit: 0.1%
#define PAR_MIN_PWM_WIDTH_MIDDLE	80 		// Unit: 0.1%
#elif (SIG_TRANS_TYPE == SIG_TRANS_TYPE_NEW)
//#define PAR_MIN_PWM_WIDTH_LIMIT		200 	// Unit: 0.1%
//#define PAR_MIN_PWM_WIDTH_MIDDLE	200 	// Unit: 0.1%
#endif

#define PAR_MAX_PWM_WIDTH			1000 	// Unit: 0.1%
#define PAR_MAX_PWM_WIDTH_LIMIT		900		// Unit: 0.1%
#define PAR_DEF_PWM_WIDTH			PAR_MIN_PWM_WIDTH // => 0%

#define PAR_MIN_PWM_FREQ			100		// Unit: 0.1kHz
#define PAR_MAX_PWM_FREQ			1000	// Unit: 0.1kHz
#define PAR_DEF_PWM_FREQ			500		// Unit: 0.1kHz => 50KHz
//#define PAR_DEF_PWM_FREQ			200

#define PAR_MIN_PWM_BURST_WIDTH		1 		// Unit: 0.1%
#define PAR_MAX_PWM_BURST_WIDTH		1000 	// Unit: 0.1%
#define PAR_DEF_PWM_BURST_WIDTH		30		// Unit: 0.1% => 3%

#define PAR_MIN_PWM_BURST_FREQ		100		// Unit: Hz
#define PAR_MAX_PWM_BURST_FREQ		1000 	// Unit: Hz
#define PAR_DEF_PWM_BURST_FREQ		200		// Unit: Hz => 200Hz

/* Timer. */
#define TIMER_10US_MAX				1800000000
extern int32 gnTimer_10us;

extern int32 gnPwm_WidthMinLimit;
extern int32 gnPwm_ErrRange;

void Isr_Adc ();
void Isr_CpuTimer0 ();
void InitPwmAdc ();
void SetPwm (int32 pwm_freq, int32 pwm_width, int32 burst_freq, int32 burst_width);
void GetPwm (int32 *pwm_freq, int32 *pwm_width, int32 *burst_freq, int32 *burst_width);
void ResetPwm ();
void ResetPwm_Freq (int32 is_freq);
void ResetECData ();
void Sleep10us (int32 time_10us);

#if (PID_CONTROL == TRUE)
double GetEC ();
#else
void CalEC_OneCycle ();
float64 GetEC_OneCycle ();
#endif

#ifdef __cplusplus
}
#endif

#endif


