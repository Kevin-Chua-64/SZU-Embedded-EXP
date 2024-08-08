#include <reg51.h>

sbit matrix_k1 = P1^3;
sbit matrix_k2 = P1^2;
sbit matrix_k3 = P1^1;
sbit matrix_k4 = P1^0;

sbit DC_Motor = P1^0;
unsigned char g_tim_H, g_tim_L, g_tim_scale, g_duty;

void delay_ms(unsigned char z)
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

unsigned char matrix_key_scan()
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

	buff[j][0] = (buff[j][0]<<1) | matrix_k1;
	buff[j][1] = (buff[j][1]<<1) | matrix_k2;
	buff[j][2] = (buff[j][2]<<1) | matrix_k3;
	buff[j][3] = (buff[j][3]<<1) | matrix_k4;		

	for (i=0;i<4;i++)  //ѭ������ĸ�����
	{
		if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )
		{
			key_flag[j] |= 0x01<<i;  // ��1����ʾ��������
			return (4*j+i + 1);
		}
		else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 

void DC_Motor_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_tim_H	= tim_H;
	g_tim_L	= tim_L;
	g_tim_scale = tim_scale;
	g_duty = duty;

	TMOD &= 0xf0;
	TMOD |= 0x01;  // ������0������ģʽ1
	TR0 = 1;  // ������0����
	TH0 = tim_H;   
	TL0 = tim_L;
	EA = 1;
	ET0 = 1;  // ���ж�
}

void DC_Motor_duty(unsigned char duty)
{
	g_duty = duty;
}

void main()
{
	unsigned char key=-1;
	unsigned char duty=0;
	unsigned char period=100;

	DC_Motor_init(0xff, 0xf0, period, 100);

	while (1)
	{
		key = matrix_key_scan();
		if (key==1)
		{
			duty += 10;
			if (duty>=period) duty = period;
		} else if (key==2)
		{
			duty -= 10;
			if (duty<=0) duty = 0;
		}
		DC_Motor_duty(duty);
		delay_ms(1);
	}
}

void DC_Motor_interrupt() interrupt 1
{
	static unsigned int cnt=0;
	
	cnt++;
	TH0 = g_tim_H;   
	TL0 = g_tim_L;
	if (cnt >= g_tim_scale) cnt = 0;  // ���ڵ���
	if (cnt <= g_duty) DC_Motor = 1;
	else DC_Motor = 0;
}
