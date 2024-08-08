#include <reg51.h>

sbit PWM = P2^1;
unsigned char g_tim_H, g_tim_L, g_tim_scale, g_duty;

void delay_ms(unsigned char z)
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

void pwm_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_tim_H	= tim_H;
	g_tim_L	= tim_L;
	g_tim_scale = tim_scale;
	g_duty = duty;

	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = tim_H;   
	TL0 = tim_L;
	EA = 1;
	ET0 = 1;  // 打开中断
}

void pwm_duty(unsigned duty)
{
	g_duty = duty;
}

void main()
{
	bit dic=0;
	unsigned char duty=0;
	unsigned char period = 100;

	pwm_init(0xff, 0xf0, period, 0);

	while (1)
	{
		if (dic==0)
		{
			duty++;
			if (duty>=period) dic = 1;
		} else if (dic==1)
		{
			duty--;
			if (duty<=0) dic = 0;
		}	
		pwm_duty(duty);
		delay_ms(1);
	}
}

void pwm_interrupt() interrupt 1
{
	static unsigned int cnt=0;
	
	cnt++;
	TH0 = g_tim_H;   
	TL0 = g_tim_L;
	if (cnt >= g_tim_scale) cnt = 0;  // 周期到了
	if (cnt <= g_duty) PWM = 1;
	else PWM = 0;
}
