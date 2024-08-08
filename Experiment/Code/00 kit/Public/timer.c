#include "timer.h"

#if USE_TIMER0
/*******************************
��������timer0_config
�������ܣ���ʱ��0������
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
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

/*******************************
��������timer0_interrupt
�������ܣ���ʱ��0���жϷ�����
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void timer0_interrupt() interrupt 1
{
	static unsigned char cnt0=0, cnt1=0, cnt_buzzer=0;

	cnt0++;
	cnt1++;
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms

	flag_digital_display = 1;
	flag_matrix_key = 1;
		
	if (cnt1 == 250)  // 500ms
	{
		cnt1 = 0;
		flag_key_long_press_output_500ms = 1;	
	}
	
	if (flag_buzzer_500Hz == 1)	 // 500Hz��������0.3s
	{
		buzzer = !buzzer;  // 2ms, 500Hz
		cnt_buzzer++;
		if (cnt_buzzer == 150)
		{
			cnt_buzzer = 0;
			flag_buzzer_500Hz = 0;  // ��0.3s
		}
	}

	if (cnt0 == 2)  // 4ms��250Hz��������0.1s
	{
		cnt0 = 0;
		if (flag_buzzer_250Hz == 1)
		{
			buzzer = !buzzer;  // 4ms, 250Hz
			cnt_buzzer++;
			if (cnt_buzzer == 50)
			{
				cnt_buzzer = 0;
				flag_buzzer_250Hz = 0;  // ��0.1s
			}
		}	
	}

	if (flag_matrix_key_pressed != 0)  // ������������
		matrix_key_hold_cnt++;
}
#endif

#if USE_TIMER1
/*******************************
��������timer1_config
�������ܣ���ʱ��1������
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void timer1_config()  //11.0592MHz
{
	TMOD &= 0x0f;
	TMOD |= 0x10;  // ������1������ģʽ1
	TR1 = 1;  // ������1����
	TH1 = 0xf8;   
	TL1 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET1 = 1;  // ���ж�
}		

/*******************************
��������timer1_interrupt
�������ܣ���ʱ��1���жϷ�����
�����βΣ�void
��������ֵ��void
��ע��
*******************************/

void timer1_interrupt() interrupt 3
{	
	TH1 = 0xf8;   
	TL1 = 0xcd;
}
#endif

#if USE_TIMER2
/*******************************
��������timer2_config
�������ܣ���ʱ��2������
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void timer2_config()  //11.0592MHz
{
	RCLK = 0;
	TCLK = 0;
	CP_RL2 = 0;  // ������2������16λ��װ��ģʽ
	TR2 = 1;  // ������2����
	RCAP2H = TH2 = 0xff;   
	RCAP2L = TL2 = 0xf6;  // 65536-65526=10, 10us
	EA = 1;
	ET2 = 1;  // ���ж�
}		

/*******************************
��������timer2_interrupt
�������ܣ���ʱ��2���жϷ�����
�����βΣ�void
��������ֵ��void
��ע��TF2Ҫ������㣬16λ���Զ���װ��
*******************************/
void timer2_interrupt() interrupt 5
{	
	TF2 = 0;
}
#endif
