#ifndef _PWM_H
#define _PWM_H

#include <reg52.h>
#include "global.h"

extern xdata unsigned char g_pwm_tim_scale, g_pwm_duty;
extern bit flag_pwm_breath_8ms;

void pwm_init(unsigned char tim_scale, unsigned char duty);
void pwm_duty(unsigned char duty);
void pwm_sleep_wind();
void pwm_temp_ctrl();

#endif
