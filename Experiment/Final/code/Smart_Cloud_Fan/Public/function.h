#ifndef _FUNCTION_H
#define _FUNCTION_H

#include <reg52.h>
#include <stdio.h>
#include "delay.h"
#include "timer.h"
#include "global.h"
#include "matrix_key.h"
#include "display.h"
#include "pwm.h"
#include "step_motor.h"
#include "DS18B20.h"
#include "infrared.h"
#include "wifi.h"

extern bit flag_cur_time_1s;
extern bit flag_timer_1s;

unsigned char Init();
void key_state_update();
void aciton();
void nixie();
void temp_update();
void wifi_update();

#endif
