#include "pwm.h"

xdata unsigned char g_pwm_tim_scale, g_pwm_duty;
/*******************************
��������pwm_init
�������ܣ�pwm��ʼ��
�����βΣ�u8 tim_H, u8 tim_L, u8 tim_scale, u8 duty
��������ֵ��void 
��ע���������ļ�һ�ε�ʱ�䣨tim_H,tim_L������ * tim_sclae = ���ڣ�dutyռ�ձȣ�ȡֵ0~tim_scale
	  ռ�ü�ʱ��2
*******************************/
void pwm_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_pwm_tim_scale = tim_scale;
	g_pwm_duty = duty;

	#if USE_PWM_TIMER2
	RCLK = 0;
	TCLK = 0;
	CP_RL2 = 0;  // ������2������16λ��װ��ģʽ
	TR2 = 1;  // ������2����
	RCAP2H = TH2 = tim_H;  // 0xff 
	RCAP2L = TL2 = tim_L;  // 0xf6 -> 0.01ms
	EA = 1;
	ET2 = 1;  // ���ж�
	#endif
}

/*******************************
��������pwm_duty
�������ܣ�pwm�ı�ռ�ձ�
�����βΣ�u8 duty
��������ֵ��void 
��ע��
*******************************/
void pwm_duty(unsigned char duty)
{
	g_pwm_duty = duty;
}

/*******************************
��������pwm_interrupt
�������ܣ�pwm�жϷ�����
�����βΣ�void
��������ֵ��void 
��ע����ʱ��2���ж�
*******************************/
#if	USE_PWM_TIMER2_INT
void pwm_interrupt() interrupt 5
{
	static unsigned char cnt=0;
	
	cnt++;
	if (cnt >= g_pwm_tim_scale) cnt = 0;  // ���ڵ���
	if (cnt <= g_pwm_duty) PWM = 1;
	else PWM = 0;
}
#endif

/*******************************
��������pwm_led2
�������ܣ�pwm����led2������
�����βΣ�u8 duty_max, u8 char duty_min
��������ֵ��void 
��ע��ledΪP2^1��ռ�ձȵ���ֵ����Сֵ�����㣬���ֵ����tim_scale��
*******************************/
void pwm_led2(unsigned char duty_max, unsigned char duty_min)
{
	static bit dic=0;
	static char duty=0;

	if (dic==0)
	{
		duty++;
		if (duty>=duty_max) dic = 1;
	} else if (dic==1)
	{
		duty--;
		if (duty<=duty_min) dic = 0;
	}	
	pwm_duty(duty);
	delay_ms(1);
}
