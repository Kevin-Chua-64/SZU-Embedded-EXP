#ifndef _TIMER_H
#define _TIMER_H

#include <reg52.h>
#include "display.h"
#include "matrix_key.h"
#include "pwm.h"
#include "dc_motor.h"
  
#define USE_TIMER0 1
#define USE_TIMER1 0
#define USE_TIMER2 0

sbit buzzer = P2^5;

#if USE_TIMER0
void timer0_config();
#endif

#if USE_TIMER1
void timer1_config();
#endif

#if USE_TIMER2
void timer2_config();
#endif

#endif
