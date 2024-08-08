#ifndef _MATRIX_KEY_H
#define _MATRIX_KEY_H

#include <reg51.h>
#include "delay.h"

sbit matrix_k1 = P1^7;
sbit matrix_k2 = P1^6;
sbit matrix_k3 = P1^5;
sbit matrix_k4 = P1^4;

unsigned char matrix_key_scan();

#endif