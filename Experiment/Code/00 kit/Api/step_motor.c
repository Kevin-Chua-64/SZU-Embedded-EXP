#include "step_motor.h"

/*******************************
函数名：step_motor_send_pulse
函数功能：步进电机的节拍运动
函数形参：bit dir
函数返回值：void
备注：dir为0正向A->AB->B->BC->C->CD->D->DA， dir为1反向
*******************************/
void step_motor_send_pulse(bit dir)
{
	static unsigned char step=1;
	
	switch(step)//8个节拍控制：A->AB->B->BC->C->CD->D->DA
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
	if (dir==0)	 // 正向
	{
		step++;
		if (step==9) step=1;
	} else
	{
		step--;	 // 反向
		if (step==0) step=8;
	}
}

/*******************************
函数名：step_motor_round
函数功能：步进电机的节拍运动
函数形参：u8 speed
函数返回值：void
备注：左右摇头，不算其他代码延时，speed至少80，即800us
*******************************/
void step_motor_round(unsigned char speed)
{
	static unsigned int step=1;
	static bit dir=0;
	
	switch((step%8)+1)//8个节拍控制：A->AB->B->BC->C->CD->D->DA
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
	if (dir==0)	 // 正向
	{
		step++;
		if (step==2000) {dir=1; delay_10us(80);}
	} else
	{
		step--;	 // 反向
		if (step==0) {dir=0; delay_10us(80);}
	}
}
