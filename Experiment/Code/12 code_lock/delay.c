#include "delay.h"

/*******************************
函数名：delay_ms
函数功能：毫秒级别延时
函数形参：u8 z
函数返回值：void
备注：z不超过255
*******************************/
void delay_ms(unsigned char z)
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}
