#include <reg51.h>
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
bit flag = 0;

void timer0_config()  // 11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x02;  // ������0������ģʽ2
	TR0 = 1;  // ������0����
	TH0 = 0x1a;  // ÿ���������Ͱ�λ�ͣ�����ÿ�����¸�ֵTH0,TL0������΢�뼶���������ʡ���¸�ֵTH0,TL0���������ʱ��
	TL0 = 0x1a;  // ��ʼ���� 250us
}

void main()
{
	unsigned char i = 0;

	EA = 1; // �ж��ܿ���
	ET0 = 1;  // ��ʱ��0�жϷֿ���
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