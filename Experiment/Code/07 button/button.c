// û�а������£�key=0xff����i���������£�key=i			  
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
	TMOD |= 0x01;  // ������0������ģʽ1
	TR0 = 1;  // ������0����
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // ���ж�
}

unsigned char key_scan()
{
	static unsigned char key_flag=0x00;  // �����ڼ�λΪ0��ʾ�ڼ�������û����
	static unsigned char buff[4]={0xff,0xff,0xff,0xff};  // ��������������8�Σ�8�β���ֵ����ͬ����Ϊ�ȶ�
	unsigned char i;

	if (flag == 1)  // ÿ2ms���һ��
	{
		flag = 0;
		buff[0] = (buff[0]<<1) | k1;
		buff[1] = (buff[1]<<1) | k2;
		buff[2] = (buff[2]<<1) | k3;
		buff[3] = (buff[3]<<1) | k4;		

		for (i=0;i<4;i++)  //ѭ������ĸ�����
		{
			if ( ((key_flag>>i)&0x01)==0x00 && buff[i]==0x00 )
			{
				key_flag |= 0x01<<i;  // ��1����ʾ��������
				return (i+1);
			}
			else if (buff[i]==0xff) key_flag &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
		}
	}
	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 

void key_action(unsigned char key)
{
	if (key == 1) led1 = ~led1;
	else if (key == 2) led2 = ~led2;
	else if (key == 3) led3 = ~led3;
	else if (key == 4) led4 = ~led4;
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
