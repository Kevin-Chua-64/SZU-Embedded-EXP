#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <reg52.h>
#include "delay.h"
#include "matrix_key.h"

#define USE_DC_MOTOR_TIMER2 0
#define USE_DC_MOTOR_TIMER2_INT 0

sbit DC_Motor = P1^0;
extern xdata unsigned char g_motor_tim_scale, g_motor_duty;

void DC_Motor_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty);
void DC_Motor_duty(unsigned char duty);
void DC_Motor_Run(unsigned char tim_scale);

#endif
