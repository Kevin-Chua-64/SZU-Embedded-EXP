#include "timer.h"

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xff;   
	TL0 = 0xa4;  // 65536-65444=92, 100us
	EA = 1;
	ET0 = 1;  // 打开中断
}

void timer0_interrupt() interrupt 1
{
	static unsigned char cnt1=0, cnt2=0, cnt3=0, cnt4=0;
	static unsigned char low_freq=0, cnt_buzzer=0, cnt_pwm=0;

	cnt1++;
	TH0 = 0xff;   
	TL0 = 0xa4;  // 65536-65444=92, 100us

	cnt_pwm++;
	if (cnt_pwm >= g_pwm_tim_scale) cnt_pwm = 0;  // 周期到了
	if (cnt_pwm <= g_pwm_duty) PWM = 1;
	else PWM = 0;

	if (cnt1 == 20)	// 2ms
	{
		cnt1 = 0;
		low_freq++;  // 250Hz发声
		cnt2++;
		
		flag_digital_display = 1;
		flag_matrix_key = 1;

		if (flag_buzzer_500Hz == 1)	 // 500Hz蜂鸣器响0.3s
		{
			BUZZER = !BUZZER;  // 2ms, 500Hz
			cnt_buzzer++;
			if (cnt_buzzer == 150)
			{
				cnt_buzzer = 0;
				flag_buzzer_500Hz = 0;  // 响0.3s
			}
		}
		if (low_freq == 2)  // 4ms，250Hz蜂鸣器响0.1s
		{
			low_freq = 0;
			if (flag_buzzer_250Hz == 1)
			{
				BUZZER = !BUZZER;  // 4ms, 250Hz
				cnt_buzzer++;
				if (cnt_buzzer == 25)
				{
					cnt_buzzer = 0;
					flag_buzzer_250Hz = 0;  // 响0.1s
				}
			}	
		}
	
		if (flag_matrix_key_pressed != 0)  // 长按检测计数器
			matrix_key_hold_cnt++;

		if (rotate_speed == 2)	// 快摇头
			flag_step_motor = 1;

		if (cnt2 == 4)  // 8ms
		{
			cnt2 = 0;
			cnt3++;

			flag_pwm_breath_8ms = 1;
			flag_blink_8ms = 1;

			if (rotate_speed == 1)	// 慢摇头
				flag_step_motor = 1;

			if (cnt3 == 125)	// 1s
			{
				cnt3 = 0;
				cnt4++;

				flag_timer_1s = 1;
				flag_cur_time_1s = 1;
				flag_key_long_press_output_1s = 1;
				flag_infrared_1s = 1;
				flag_temp_update_1s = 1;

				if (cnt4 == 2)	// 2s
				{
					cnt4 = 0;
					flag_wifi_update_2s = 1;
				}
			}	
		}
	}
}
