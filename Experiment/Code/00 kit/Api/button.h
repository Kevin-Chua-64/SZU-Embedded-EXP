#ifndef _BUTTON_H
#define _BUTTON_H

#include <reg52.h>
#include "delay.h"

sbit button1 = P3^1;
sbit button2 = P3^0;
sbit button3 = P3^2;
sbit button4 = P3^3;

unsigned char button_scan();

#endif
