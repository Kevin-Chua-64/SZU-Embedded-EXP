#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <reg52.h>	 

#define USE_DIGITAL_DISPLAY_TIMER0 0
#define USE_DIGITAL_DISPLAY_TIMER0_INT 0

#define SMG_PORT P0

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

extern bit flag_digital_display;

void digital_display(unsigned char *ch);

#if USE_DIGITAL_DISPLAY_TIMER0

void digital_display_timer0_config();

#endif

#endif
