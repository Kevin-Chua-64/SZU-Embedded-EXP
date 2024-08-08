#include "display.h"

#if USE_DIGITAL_DISPLAY_TIMER0
/*******************************
函数名：digital_display_timer0_config
函数功能：定时器0的配置
函数形参：void
函数返回值：void
备注：
*******************************/
void digital_display_timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // 打开中断
}
#endif

/*******************************
函数名：digital_display
函数功能：数码管显示
函数形参：u8 ch[8]
函数返回值：void
备注：显示ch（模值）里的值，从左到右为ch[0]到ch[7]	
      占用定时器0
*******************************/
bit flag_digital_display = 0;					 
void digital_display(unsigned char *ch)
{
	static unsigned char i=0;

	if (flag_digital_display==1)
	{
		flag_digital_display = 0;
		i++;
		if (i==8) i=0;
		SMG_PORT = 0x00;  // 消音
	}

   	switch(i)  // 位选和段选
	{
		case 0: LSC=1;LSB=1;LSA=1; SMG_PORT = *(ch+0); break;
		case 1: LSC=1;LSB=1;LSA=0; SMG_PORT = *(ch+1); break;
		case 2: LSC=1;LSB=0;LSA=1; SMG_PORT = *(ch+2); break;
		case 3: LSC=1;LSB=0;LSA=0; SMG_PORT = *(ch+3); break;
		case 4: LSC=0;LSB=1;LSA=1; SMG_PORT = *(ch+4); break;
		case 5: LSC=0;LSB=1;LSA=0; SMG_PORT = *(ch+5); break;
		case 6: LSC=0;LSB=0;LSA=1; SMG_PORT = *(ch+6); break;
		case 7: LSC=0;LSB=0;LSA=0; SMG_PORT = *(ch+7); break;
	}
}

#if	USE_DIGITAL_DISPLAY_TIMER0_INT
/*******************************
函数名：digital_display_timer0_interrupt
函数功能：定时器0的中断服务函数
函数形参：void
函数返回值：void
备注：
*******************************/
bit flag_digital_display=0;
void digital_display_timer0_interrupt() interrupt 1
{
	TH0 = 0xf8;   
	TL0 = 0xcd;
	flag_digital_display = 1;
}
#endif
