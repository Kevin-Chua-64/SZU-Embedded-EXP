#include "public.h"

void delay_ms(unsigned char z)
{
	unsigned char i,j;
	for (i=0;i<z;i++)
		for(j=0;j<127;j++);
}
