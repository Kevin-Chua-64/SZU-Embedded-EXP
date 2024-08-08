#ifndef _TIMER_H
#define _TIMER_H

#include <reg52.h>
#include "function.h"
#include "global.h"
#include "display.h"
#include "matrix_key.h"
#include "pwm.h"
#include "step_motor.h"
#include "infrared.h"

sbit BUZZER = P2^5;
sbit PWM = P2^1;

void timer0_config();

#endif
