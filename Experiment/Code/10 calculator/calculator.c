// 0  1  2  3
// 4  5  6  7
// 8  9  +  -
// *  /  =	AC
// LED D1,D2,D3,D4�ֱ��ʾ�Ӽ��˳�

#include <reg51.h>
unsigned char code led_num[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40, 0x00};  //12��ģֵ��0~9��-������ʾ

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

long cur_value=0, value=0;  // �����������

unsigned char current_display[8] = {11,11,11,11,11,11,11,0};  // ��ǰ��ʾ�����ݣ�ʮ���ƣ�11��ʾ����ʾ

void delay(unsigned char z)	 // delay z ms
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // ������0������ģʽ1
	TR0 = 1;  // ������0����
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // ���ж�
}

unsigned char key_scan()
{
	static unsigned char j=0;
	static unsigned char key_flag[4]={0x00,0x00,0x00,0x00};  // ���У������ڼ�λΪ0��ʾ�ڼ��а���û����
	static unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};  // ��������������8�Σ�8�β���ֵ����ͬ����Ϊ�ȶ�
	unsigned char i;

	switch(j)  // ����ɨ�裬��Ӧ��������0���͵�ƽ
	{
		case 0: P1 = 0x7f; break;
		case 1: P1 = 0xbf; break;
		case 2: P1 = 0xdf; break;
		case 3: P1 = 0xef; break;
	}

	if (flag == 1)  // ÿ2ms���һ��
	{
		flag = 0;
		buff[j][0] = (buff[j][0]<<1) | k1;
		buff[j][1] = (buff[j][1]<<1) | k2;
		buff[j][2] = (buff[j][2]<<1) | k3;
		buff[j][3] = (buff[j][3]<<1) | k4;		

		for (i=0;i<4;i++)  //ѭ������ĸ�����
		{
			if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )
			{
				key_flag[j] |= 0x01<<i;  // ��1����ʾ��������
				return (4*j+i);
			}
			else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
		}
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 

void display_update(long v)
{
	long tmp;
	unsigned char i=0;
	unsigned char j, k;

	if (v==0)  // 0����ʾ
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
				current_display[k] = 10;  // �������ʾȫΪ-
			return;
		}
		current_display[7-i] = tmp%10;  // ��ȡ�����λ
		tmp = tmp/10;
		i++;
	}
	if (v<0)
	{
		if (i>7)
		{
			for (k=0;k<8;k++)
				current_display[k] = 10;  // �������ʾȫΪ-
			return;
		}
		current_display[7-i] = 10;  // ����
		i++;
	}
	for (j=0;j<(8-i);j++)  // ��������Ҫ�õ��ĸ�λ
		current_display[j] = 11;
}

void key_action(unsigned char key)
{
	static char op;

	if (key>=0 && key<=9)  // ����0~9
	{
		cur_value = cur_value*10 + key;
		display_update(cur_value);
	}
	else if (key>=10 && key<=13)  // �Ӽ��˳�
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
	else if (key == 14)  // ����
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
	else if (key == 15)  // ����
	{
		cur_value = 0;
		value = 0;
		led1=1; led2=1; led3=1; led4=1;
		display_update(cur_value);
	}						
	// key==0xff���ް������»��߰�����δƽ�ȣ��޲���
}
						 
void display()
{
	static unsigned char i=0;

   	switch(i)  // λѡ�Ͷ�ѡ
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
	P0=0x00;  // ����
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
