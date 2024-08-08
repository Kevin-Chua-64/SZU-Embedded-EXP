#ifndef _INFRARED_H
#define _INFRARED_H

#include <reg52.h>
#include "delay.h"
#include "global.h"

sbit IRED=P3^2;

void ired_init();

#endif
