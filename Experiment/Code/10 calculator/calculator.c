// 0  1  2  3
// 4  5  6  7
// 8  9  +  -
// *  /  =	AC
// LED D1,D2,D3,D4分别表示加减乘除

#include <reg51.h>
unsigned char code led_num[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40, 0x00};  //12个模值，0~9，-，不显示

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

sbit k1 = P1^3;
sbit k2 = P1^2;
sbit k3 = P1^1;
sbit k4 = P1^0;

sbit led1 = P2^0;
sbit led2 = P2^1;
sbit led3 = P2^6;
sbit led4 = P2^7;

bit flag = 0;

long cur_value=0, value=0;  // 两个运算的数

unsigned char current_display[8] = {11,11,11,11,11,11,11,0};  // 当前显示的内容，十进制，11表示不显示

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
				return (4*j+i);
			}
			else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // 置0，表示按键松开
		}
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // 表示按键松开或者处于不稳定状态
} 

void display_update(long v)
{
	long tmp;
	unsigned char i=0;
	unsigned char j, k;

	if (v==0)  // 0的显示
	{
		tmp = 0;
		current_display[7] = 0;
		i++;
	}
	else if (v < 0) tmp = -v;
	else tmp = v;
	while (tmp != 0)
	{
		if (i>7)
		{
			for (k=0;k<8;k++)
				current_display[k] = 10;  // 溢出，显示全为-
			return;
		}
		current_display[7-i] = tmp%10;  // 逐步取出最低位
		tmp = tmp/10;
		i++;
	}
	if (v<0)
	{
		if (i>7)
		{
			for (k=0;k<8;k++)
				current_display[k] = 10;  // 溢出，显示全为-
			return;
		}
		current_display[7-i] = 10;  // 负号
		i++;
	}
	for (j=0;j<(8-i);j++)  // 消除不需要用到的高位
		current_display[j] = 11;
}

void key_action(unsigned char key)
{
	static char op;

	if (key>=0 && key<=9)  // 输入0~9
	{
		cur_value = cur_value*10 + key;
		display_update(cur_value);
	}
	else if (key>=10 && key<=13)  // 加减乘除
	{
		value = cur_value;
		cur_value = 0;
		switch (key)
		{
			case 10: op = '+'; led1=0; led2=1; led3=1; led4=1; break;
			case 11: op = '-'; led1=1; led2=0; led3=1; led4=1; break;
			case 12: op = '*'; led1=1; led2=1; led3=0; led4=1; break;
			case 13: op = '/'; led1=1; led2=1; led3=1; led4=0; break;
		}
	}
	else if (key == 14)  // 计算
	{
		switch (op)
		{
			case '+': cur_value = value + cur_value; led1=1; led2=1; led3=1; led4=1; break;
			case '-': cur_value = value - cur_value; led1=1; led2=1; led3=1; led4=1; break;
			case '*': cur_value = value * cur_value; led1=1; led2=1; led3=1; led4=1; break;
			case '/': cur_value = value / cur_value; led1=1; led2=1; led3=1; led4=1; break;
			default: break;
		}
		display_update(cur_value);
	}
	else if (key == 15)  // 清零
	{
		cur_value = 0;
		value = 0;
		led1=1; led2=1; led3=1; led4=1;
		display_update(cur_value);
	}						
	// key==0xff，无按键按下或者按键尚未平稳，无操作
}
						 
void display()
{
	static unsigned char i=0;

   	switch(i)  // 位选和段选
	{
		case 0: LSC=1;LSB=1;LSA=1;P0 = led_num[current_display[0]]; break;
		case 1: LSC=1;LSB=1;LSA=0;P0 = led_num[current_display[1]]; break;
		case 2: LSC=1;LSB=0;LSA=1;P0 = led_num[current_display[2]]; break;
		case 3: LSC=1;LSB=0;LSA=0;P0 = led_num[current_display[3]]; break;
		case 4: LSC=0;LSB=1;LSA=1;P0 = led_num[current_display[4]]; break;
		case 5: LSC=0;LSB=1;LSA=0;P0 = led_num[current_display[5]]; break;
		case 6: LSC=0;LSB=0;LSA=1;P0 = led_num[current_display[6]]; break;
		case 7: LSC=0;LSB=0;LSA=0;P0 = led_num[current_display[7]]; break;
	}

	delay(2);
	P0=0x00;  // 消音
	i++;
	if (i==8) i = 0;
}

void main()
{
	unsigned char key=0;
	
	timer0_config();
	while(1)
	{
		key = key_scan();
		key_action(key);
		display();
	}
}

void timer0_2ms() interrupt 1
{
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	flag = 1;
}
