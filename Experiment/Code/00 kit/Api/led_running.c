#include "led_running.h"

/*******************************
函数名：led_run
函数功能：流水灯
函数形参：u8 dir，u8 ms
函数返回值：void
备注：dir为1：顺流
		   2：逆流
		   3：左右横跳
	  时间间隔为ms
*******************************/
void led_run(unsigned char dir, unsigned char ms)
{
	static unsigned char i=0;
	if (dir==0){  // up
		P2 = (0xfe<<i)|(0xfe>>(8-i));
		if (i==8) i=0;
	}else if (dir==1){  // down
		P2 = (0x7f>>i)|(0x7f<<(8-i));
		if (i==8) i=0;
	}else if (dir==2){  // up and down
		if (i<7)
			P2 = (0xfe<<i)|(0xfe>>(8-i));
		else
			P2 = (0x7f>>(i-7))|(0x7f<<(8-(i-7)));
		if (i==14) i=0;
	}
	
	delay_ms(ms);
	i++;	
}
