// 没有按键按下：key=0xff，第i个按键按下，key=i			  
#include <reg51.h>
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

sbit k1 = P1^3;
sbit k2 = P1^2;
sbit k3 = P1^1;
sbit k4 = P1^0;

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
	static unsigned char j=0;
	static unsigned char key_flag[4]={0x00,0x00,0x00,0x00};  // 四行，倒数第几位为0表示第几列按键没按下
	static unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};  // 消抖，连续采样8次，8次采样值均相同才视为稳定
	unsigned char i;

	switch(j)  // 换行扫描，对应行输入置0，低电平
	{
		case 0: P1 = 0x7f; break;
		case 1: P1 = 0xbf; break;
		case 2: P1 = 0xdf; break;
		case 3: P1 = 0xef; break;
	}

	if (flag == 1)  // 每2ms检测一次
	{
		flag = 0;
		buff[j][0] = (buff[j][0]<<1) | k1;
		buff[j][1] = (buff[j][1]<<1) | k2;
		buff[j][2] = (buff[j][2]<<1) | k3;
		buff[j][3] = (buff[j][3]<<1) | k4;		

		for (i=0;i<4;i++)  //循环检测四个按键
		{
			if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )
			{
				key_flag[j] |= 0x01<<i;  // 置1，表示按键按下
				return (4*j+i + 1);
			}
			else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // 置0，表示按键松开
		}
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // 表示按键松开或者处于不稳定状态
} 

void key_action(unsigned char key)
{
	if (key != 0xff)
		P0 = led_num[key-1];							
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
