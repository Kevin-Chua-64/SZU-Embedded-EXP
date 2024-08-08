#include "step_motor.h"

/*******************************
left and right rotation
the delay in each cycle (not consider other code delay) is at least 80，that is 800us
rotate faster when the delay is smaller
*******************************/
bit flag_step_motor=0;
void step_motor_round()
{
	static unsigned int step=1;
	static bit dir=0;
	
	if (flag_step_motor == 1)
	{
		flag_step_motor = 0;
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
		} 
		if (dir==0)	 // 正向
		{
			step++;
			if (step==2000) dir=1;
		} else
		{
			step--;	 // 反向
			if (step==0) dir=0;
		}
	}
}
