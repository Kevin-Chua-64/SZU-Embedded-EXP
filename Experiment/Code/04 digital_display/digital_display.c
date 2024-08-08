#include <reg52.h>
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
bit flag = 0;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 2ms
}
					 
void display()
{
	static unsigned char i=0;

   	switch(i)  // 位选和段选
	{
		case 0: LSC=1;LSB=1;LSA=1;P0 = led_num[0]; break;
		case 1: LSC=1;LSB=1;LSA=0;P0 = led_num[1]; break;
		case 2: LSC=1;LSB=0;LSA=1;P0 = led_num[2]; break;
		case 3: LSC=1;LSB=0;LSA=0;P0 = led_num[3]; break;
		case 4: LSC=0;LSB=1;LSA=1;P0 = led_num[4]; break;
		case 5: LSC=0;LSB=1;LSA=0;P0 = led_num[5]; break;
		case 6: LSC=0;LSB=0;LSA=1;P0 = led_num[6]; break;
		case 7: LSC=0;LSB=0;LSA=0;P0 = led_num[7]; break;
	}

	if (flag==1)
	{
		P0=0x00;  // 消音
		i++;
		if (i==8) i=0;
		flag = 0;
	}
}

void main()
{	
	EA = 1; // 中断总开关
	ET0 = 1;  // 定时器0中断分开关
	timer0_config();
	while(1)
	{
		display();
	}		
}

void timer0_2ms() interrupt 1
{
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 2ms
	flag = 1;
}