#include "dc_motor.h"

xdata unsigned char g_motor_tim_scale, g_motor_duty;
/*******************************
��������DC_Motor_init
�������ܣ�DC_Motor��ʼ��
�����βΣ�u8 tim_H, u8 tim_L, u8 tim_scale, u8 duty
��������ֵ��void 
��ע���������ļ�һ�ε�ʱ�䣨tim_H,tim_L������ * tim_sclae = ���ڣ�dutyռ�ձȣ�ȡֵ0~tim_scale
	  ռ�ü�ʱ��0   P1^0�˿�
*******************************/
void DC_Motor_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_motor_tim_scale = tim_scale;
	g_motor_duty = duty;

	#if USE_DC_MOTOR_TIMER2
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
��������DC_Motor_duty
�������ܣ�DC_Motor�ı�ռ�ձ�
�����βΣ�u8 duty
��������ֵ��void 
��ע��
*******************************/
void DC_Motor_duty(unsigned char duty)
{
	g_motor_duty = duty;
}

/*******************************
��������DC_Motor_interrupt
�������ܣ�DC_Motor�жϷ�����
�����βΣ�void
��������ֵ��void 
��ע����ʱ��0���ж�
*******************************/
#if	USE_DC_MOTOR_TIMER2_INT
void DC_Motor_interrupt() interrupt 5
{
	static unsigned char cnt=0;
	
	cnt++;
	if (cnt >= g_motor_tim_scale) cnt = 0;  // ���ڵ���
	if (cnt <= g_motor_duty) DC_Motor = 1;
	else DC_Motor = 0;
}
#endif

/*******************************
��������DC_Motor_Run
�������ܣ�����ֱ������ٶȣ�����ɨ�裬1�����٣�2������
�����βΣ�u8 tim_scale
��������ֵ��void 
��ע��motorΪP1^0��ռ�ձ���ֵ����Сֵ����0�����ֵ����tim_scale
*******************************/
void DC_Motor_Run(unsigned char tim_scale)
{
	unsigned char key=-1;
	static char duty=0;

	key = matrix_key_scan();
	if (key==1)
	{
		duty += 10;
		if (duty>=tim_scale) duty = tim_scale;
	} else if (key==2)
	{
		duty -= 10;
		if (duty<=0) duty = 0;
	}	
	DC_Motor_duty(duty);
	delay_ms(1);
}
