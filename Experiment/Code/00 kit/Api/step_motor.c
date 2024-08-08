#include "step_motor.h"

/*******************************
��������step_motor_send_pulse
�������ܣ���������Ľ����˶�
�����βΣ�bit dir
��������ֵ��void
��ע��dirΪ0����A->AB->B->BC->C->CD->D->DA�� dirΪ1����
*******************************/
void step_motor_send_pulse(bit dir)
{
	static unsigned char step=1;
	
	switch(step)//8�����Ŀ��ƣ�A->AB->B->BC->C->CD->D->DA
	{
		case 1: IN1_A=1;IN2_B=0;IN3_C=0;IN4_D=0;break;
		case 2: IN1_A=1;IN2_B=1;IN3_C=0;IN4_D=0;break;
		case 3: IN1_A=0;IN2_B=1;IN3_C=0;IN4_D=0;break;
		case 4: IN1_A=0;IN2_B=1;IN3_C=1;IN4_D=0;break;
		case 5: IN1_A=0;IN2_B=0;IN3_C=1;IN4_D=0;break;
		case 6: IN1_A=0;IN2_B=0;IN3_C=1;IN4_D=1;break;
		case 7: IN1_A=0;IN2_B=0;IN3_C=0;IN4_D=1;break;
		case 8: IN1_A=1;IN2_B=0;IN3_C=0;IN4_D=1;break;
	} delay_10us(80);	
	if (dir==0)	 // ����
	{
		step++;
		if (step==9) step=1;
	} else
	{
		step--;	 // ����
		if (step==0) step=8;
	}
}

/*******************************
��������step_motor_round
�������ܣ���������Ľ����˶�
�����βΣ�u8 speed
��������ֵ��void
��ע������ҡͷ����������������ʱ��speed����80����800us
*******************************/
void step_motor_round(unsigned char speed)
{
	static unsigned int step=1;
	static bit dir=0;
	
	switch((step%8)+1)//8�����Ŀ��ƣ�A->AB->B->BC->C->CD->D->DA
	{
		case 1: IN1_A=1;IN2_B=0;IN3_C=0;IN4_D=0;break;
		case 2: IN1_A=1;IN2_B=1;IN3_C=0;IN4_D=0;break;
		case 3: IN1_A=0;IN2_B=1;IN3_C=0;IN4_D=0;break;
		case 4: IN1_A=0;IN2_B=1;IN3_C=1;IN4_D=0;break;
		case 5: IN1_A=0;IN2_B=0;IN3_C=1;IN4_D=0;break;
		case 6: IN1_A=0;IN2_B=0;IN3_C=1;IN4_D=1;break;
		case 7: IN1_A=0;IN2_B=0;IN3_C=0;IN4_D=1;break;
		case 8: IN1_A=1;IN2_B=0;IN3_C=0;IN4_D=1;break;
	} delay_10us(speed);	
	if (dir==0)	 // ����
	{
		step++;
		if (step==2000) {dir=1; delay_10us(80);}
	} else
	{
		step--;	 // ����
		if (step==0) {dir=0; delay_10us(80);}
	}
}
