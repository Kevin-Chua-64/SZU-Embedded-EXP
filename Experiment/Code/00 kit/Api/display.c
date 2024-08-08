#include "display.h"

#if USE_DIGITAL_DISPLAY_TIMER0
/*******************************
��������digital_display_timer0_config
�������ܣ���ʱ��0������
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void digital_display_timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // ������0������ģʽ1
	TR0 = 1;  // ������0����
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // ���ж�
}
#endif

/*******************************
��������digital_display
�������ܣ��������ʾ
�����βΣ�u8 ch[8]
��������ֵ��void
��ע����ʾch��ģֵ�����ֵ��������Ϊch[0]��ch[7]	
      ռ�ö�ʱ��0
*******************************/
bit flag_digital_display = 0;					 
void digital_display(unsigned char *ch)
{
	static unsigned char i=0;

	if (flag_digital_display==1)
	{
		flag_digital_display = 0;
		i++;
		if (i==8) i=0;
		SMG_PORT = 0x00;  // ����
	}

   	switch(i)  // λѡ�Ͷ�ѡ
	{
		case 0: LSC=1;LSB=1;LSA=1; SMG_PORT = *(ch+0); break;
		case 1: LSC=1;LSB=1;LSA=0; SMG_PORT = *(ch+1); break;
		case 2: LSC=1;LSB=0;LSA=1; SMG_PORT = *(ch+2); break;
		case 3: LSC=1;LSB=0;LSA=0; SMG_PORT = *(ch+3); break;
		case 4: LSC=0;LSB=1;LSA=1; SMG_PORT = *(ch+4); break;
		case 5: LSC=0;LSB=1;LSA=0; SMG_PORT = *(ch+5); break;
		case 6: LSC=0;LSB=0;LSA=1; SMG_PORT = *(ch+6); break;
		case 7: LSC=0;LSB=0;LSA=0; SMG_PORT = *(ch+7); break;
	}
}

#if	USE_DIGITAL_DISPLAY_TIMER0_INT
/*******************************
��������digital_display_timer0_interrupt
�������ܣ���ʱ��0���жϷ�����
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
bit flag_digital_display=0;
void digital_display_timer0_interrupt() interrupt 1
{
	TH0 = 0xf8;   
	TL0 = 0xcd;
	flag_digital_display = 1;
}
#endif
