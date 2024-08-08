#include <reg51.h>

// code 把该变量存在FLASH中
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
// 0~F: 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71

void timer0_config()
{
	TMOD = 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xd8;  // 初始计数高8位
	TL0 = 0xf0;  // 初始计数低8位，共10ms
}

void seg_led_display()
{
	static unsigned char i=0;
	static unsigned char count=0;


	P0 = led_num[i];
	if (TF0 == 1)
	{
		count++;
		TF0 = 0;
		TH0 = 0xd8;  // 55536
		TL0 = 0xf0;	
	}
	if (count == 100)  // 1s
	{
		i++;
		if (i == 16) i = 0;
		count = 0;
	}
}

void main()
{
	timer0_config();
	while(1)
	{
		seg_led_display();
	}
}