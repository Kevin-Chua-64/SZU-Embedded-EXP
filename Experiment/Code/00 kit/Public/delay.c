#include "delay.h"

/*******************************
函数名：delay_10us
函数功能：10微秒级别延时
函数形参：u16 ten_us
函数返回值：void
备注：ten_us不超过65535
*******************************/
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

/*******************************
函数名：delay_ms
函数功能：毫秒级别延时
函数形参：u16 ms
函数返回值：void
备注：ms不超过65535
*******************************/
void delay_ms(unsigned int ms)
{
	unsigned int i, j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}
