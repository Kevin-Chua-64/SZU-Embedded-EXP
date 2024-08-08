#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H

#include <reg52.h>
#include "delay.h"

#define USE_OUTER_STEP_MOTOR 1

#if USE_OUTER_STEP_MOTOR
sbit IN1_A = P3^6;
sbit IN2_B = P2^7;
sbit IN3_C = P2^6;
sbit IN4_D = P2^0;
#else
sbit IN1_A = P1^0;
sbit IN2_B = P1^1;
sbit IN3_C = P1^2;
sbit IN4_D = P1^3;
#endif

void step_motor_send_pulse(bit dir);
void step_motor_round(unsigned char speed);

#endif
