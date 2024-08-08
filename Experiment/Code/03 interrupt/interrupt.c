#include <reg51.h>
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
bit flag = 0;

void timer0_config()  // 11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x02;  // 计数器0工作在模式2
	TR0 = 1;  // 计数器0工作
	TH0 = 0x1a;  // 每次溢出后向低八位送，无需每次重新赋值TH0,TL0，适用微秒级别计数，节省重新赋值TH0,TL0的软件级别时间
	TL0 = 0x1a;  // 初始计数 250us
}

void main()
{
	unsigned char i = 0;

	EA = 1; // 中断总开关
	ET0 = 1;  // 定时器0中断分开关
	timer0_config();
	while(1)
	{
		P0 = led_num[i];
		if (flag == 1)
		{
			i++;
			if (i == 16) i = 0;
			flag = 0;
		}
	}
}

void timer0_250us() interrupt 1
{

	static unsigned int count = 0;

	count++;	
	if (count == 4000)  // 1s
	{
		flag = 1;
		count = 0;
	}

}