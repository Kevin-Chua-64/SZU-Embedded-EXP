#include <reg51.h>

unsigned char code led_num[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40};
unsigned char hour=0, min=0, sec=0;
bit flag1=0, flag2=0;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

void delay(unsigned char z)	 // delay z ms
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
}

void timer1_config()  //11.0592MHz
{
	TMOD &= 0x0f;
	TMOD |= 0x10;  // 计数器1工作在模式1
	TR1 = 1;  // 计数器1工作
	TH1 = 0xdc;   
	TL1 = 0x00;  // 65536-56320=9216, 10ms
}
	
void count()
{
	if (flag2==1)
	{
		flag2 = 0;
		sec++;
		if (sec==60)
		{
			sec = 0;
			min++;
			if (min==60)
			{
				min = 0;
				hour++;
				if (hour==24) hour = 0;	
			}
		}
	}		
}
				 
void display()
{
	static unsigned char i=0;

   	switch(i)  // 位选
	{
		case 0: LSC=1;LSB=1;LSA=1;P0 = led_num[hour/10]; break;
		case 1: LSC=1;LSB=1;LSA=0;P0 = led_num[hour%10]; break;
		case 2: LSC=1;LSB=0;LSA=1;P0 = led_num[10]; break;
		case 3: LSC=1;LSB=0;LSA=0;P0 = led_num[min/10]; break;
		case 4: LSC=0;LSB=1;LSA=1;P0 = led_num[min%10]; break;
		case 5: LSC=0;LSB=1;LSA=0;P0 = led_num[10]; break;
		case 6: LSC=0;LSB=0;LSA=1;P0 = led_num[sec/10]; break;
		case 7: LSC=0;LSB=0;LSA=0;P0 = led_num[sec%10]; break;
	}

	delay(2);
	P0=0x00;  // 消音
	i++;
	if (i==8) i=0;
}

void led_running()
{
	static unsigned char i=0;  // 除掉2345

	P2 = (0xfe<<i)|(0xfe>>(8-i));

	if (flag1==1)
	{
		flag1 = 0;
		i++;
		if (i==2) i=6;
		if (i==8) i=0;
	}
}

void main()
{	
	EA = 1; // 中断总开关
	ET0 = 1;  // 定时器0中断分开关
	ET1 = 1;  // 定时器1中断分开关
	timer0_config();
	timer1_config();
	while(1)
	{
		count();
		display();
		if (min==0) led_running(); else P2 = 0xff;	 // 1分钟时启动流水灯，2分钟关闭
	}		
}

void led_200ms() interrupt 1
{
	static unsigned char cnt=0;
	
	cnt++;
	TH0 = 0xf8;   
	TL0 = 0xcd;
	if (cnt==100)
	{
		cnt = 0;
		flag1 = 1;
	}
}

void count_1s() interrupt 3
{
	static unsigned char cnt=0;
	
	cnt++;
	TH1 = 0xdc;   
	TL1 = 0x00;
	if (cnt==100)
	{
		cnt = 0;
		flag2 = 1;
	}
}
