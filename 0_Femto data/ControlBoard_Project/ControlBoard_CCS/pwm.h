/*
 * pwm.h
 *
 *  Created on: 2013. 6. 19.
 *      Author: lastdkht
 */

#ifndef PWM_H_
#define PWM_H_

#define MIN_PWM_WIDTH_280x			0 		// Unit: %
#define MAX_PWM_WIDTH_280x			100 	// Unit: %
#define MAX_PWM_WIDTH_LIMIT_280x	100		// Unit: %

#define MIN_PWM_FREQ_280x			1000	// Unit: Hz
#define MAX_PWM_FREQ_280x			500000	// Unit: Hz


int InitPwm_280x (int32 id, Uint16 ctrmode, Uint16 phsen, union AQCTL_REG aqctla, union AQCTL_REG aqctlb);
int InitPwm_ET_280x (int32 id, union ETSEL_REG etsel, union ETPS_REG etps);
void SetPwm_280x (int32 id, Uint16 ctrmode, int32 pwm_freq, float32 pwm_width);

#endif /* PWM_H_ */
