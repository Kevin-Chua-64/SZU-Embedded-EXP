#ifndef _MAIN_H
#define _MAIN_H

#include <reg52.h>
#include <stdio.h>
#include "delay.h"
#include "timer.h"
#include "global.h"
#include "function.h"
#include "led_running.h"
#include "button.h"
#include "matrix_key.h"
#include "display.h"
#include "lcd1602.h"
#include "pwm.h"
#include "dc_motor.h"
#include "step_motor.h"
#include "uart.h"
#include "DS18B20.h"
#include "infrared.h"
#include "wifi.h"

unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

#endif
