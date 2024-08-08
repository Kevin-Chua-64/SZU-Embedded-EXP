#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H

#include <reg52.h>

sbit IN1_A = P3^3;
sbit IN2_B = P2^7;
sbit IN3_C = P2^6;
sbit IN4_D = P2^0;

extern bit flag_step_motor;

void step_motor_round();

#endif
