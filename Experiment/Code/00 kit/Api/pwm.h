#ifndef _PWM_H
#define _PWM_H

#include <reg52.h>
#include "delay.h"

#define USE_PWM_TIMER2 0
#define USE_PWM_TIMER2_INT 0

sbit PWM = P2^1;
extern xdata unsigned char g_pwm_tim_scale, g_pwm_duty;

void pwm_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty);
void pwm_duty(unsigned char duty);
void pwm_led2(unsigned char duty_max, unsigned char duty_min);

#endif
