#ifndef _INFRARED_H
#define _INFRARED_H

#include <reg52.h>
#include "delay.h"
#include "global.h"
#include "pwm.h"

sbit IRED=P3^2;

extern bit flag_infrared_1s;

void ired_init();

#endif
