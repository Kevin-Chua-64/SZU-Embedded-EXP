#include "pwm.h"

xdata unsigned char g_pwm_tim_scale, g_pwm_duty;
void pwm_init(unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_pwm_tim_scale = tim_scale;
	g_pwm_duty = duty;
}

/*******************************
change the duty of pwm
*******************************/
void pwm_duty(unsigned char duty)
{
	g_pwm_duty = duty;
}

/*******************************
sleep wind: change the duty of pwm regularly all the time
*******************************/
bit flag_pwm_breath_8ms=0;
void pwm_sleep_wind()
{
	static bit dic=0;
	static unsigned char duty=0;

	if (flag_pwm_breath_8ms == 1)
	{
		flag_pwm_breath_8ms = 0;
		if (dic==0)
		{
			duty++;
			if (duty>=100) dic = 1;
		} else if (dic==1)
		{
			duty--;
			if (duty<=0) dic = 0;
		}	
		pwm_duty(duty);
	}
}

/*******************************
change the duty according to the temperature
*******************************/
void pwm_temp_ctrl()
{
	int duty;

	duty = start_duty + ((temp - start_temp)*3);
	if (duty>=100) duty = 100;
	else if (duty<=0) duty = 0;
	pwm_duty(duty);
}
