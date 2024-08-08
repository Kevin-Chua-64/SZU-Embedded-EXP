#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <reg52.h>	
#include "global.h" 

#define SMG_PORT P0

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

extern bit flag_digital_display;
extern bit flag_blink_8ms;	

void digital_display(unsigned char *ch);

#endif
