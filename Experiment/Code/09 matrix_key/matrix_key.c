// û�а������£�key=0xff����i���������£�key=i			  
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
				return (4*j+i + 1);
			}
			else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
		}
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 

void key_action(unsigned char key)
{
	if (key != 0xff)
		P0 = led_num[key-1];							
	// key==0xff���ް������»��߰�����δƽ�ȣ��޲���
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
