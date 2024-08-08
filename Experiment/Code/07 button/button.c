// 没有按键按下：key=0xff，第i个按键按下，key=i			  
#include <reg51.h>

sbit k1 = P3^1;
sbit k2 = P3^0;
sbit k3 = P3^2;
sbit k4 = P3^3;

sbit led1 = P2^0;
sbit led2 = P2^1;
sbit led3 = P2^6;
sbit led4 = P2^7;

bit flag = 0;

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // 打开中断
}

unsigned char key_scan()
{
	static unsigned char key_flag=0x00;  // 倒数第几位为0表示第几个按键没按下
	static unsigned char buff[4]={0xff,0xff,0xff,0xff};  // 消抖，连续采样8次，8次采样值均相同才视为稳定
	unsigned char i;

	if (flag == 1)  // 每2ms检测一次
	{
		flag = 0;
		buff[0] = (buff[0]<<1) | k1;
		buff[1] = (buff[1]<<1) | k2;
		buff[2] = (buff[2]<<1) | k3;
		buff[3] = (buff[3]<<1) | k4;		

		for (i=0;i<4;i++)  //循环检测四个按键
		{
			if ( ((key_flag>>i)&0x01)==0x00 && buff[i]==0x00 )
			{
				key_flag |= 0x01<<i;  // 置1，表示按键按下
				return (i+1);
			}
			else if (buff[i]==0xff) key_flag &= ~(0x01<<i);  // 置0，表示按键松开
		}
	}
	return 0xff;  // 表示按键松开或者处于不稳定状态
} 

void key_action(unsigned char key)
{
	if (key == 1) led1 = ~led1;
	else if (key == 2) led2 = ~led2;
	else if (key == 3) led3 = ~led3;
	else if (key == 4) led4 = ~led4;
	// key==0xff，无按键按下或者按键尚未平稳，无操作
}

void main()
{
	unsigned char key=0;

	timer0_config();
	while(1)
	{
		key = key_scan();
		key_action(key);
	}
}

void timer0_2ms() interrupt 1
{
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	flag = 1;
}
