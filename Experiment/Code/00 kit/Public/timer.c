#include "timer.h"

#if USE_TIMER0
/*******************************
函数名：timer0_config
函数功能：定时器0的配置
函数形参：void
函数返回值：void
备注：
*******************************/
void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // 打开中断
}

/*******************************
函数名：timer0_interrupt
函数功能：定时器0的中断服务函数
函数形参：void
函数返回值：void
备注：
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
	
	if (flag_buzzer_500Hz == 1)	 // 500Hz蜂鸣器响0.3s
	{
		buzzer = !buzzer;  // 2ms, 500Hz
		cnt_buzzer++;
		if (cnt_buzzer == 150)
		{
			cnt_buzzer = 0;
			flag_buzzer_500Hz = 0;  // 响0.3s
		}
	}

	if (cnt0 == 2)  // 4ms，250Hz蜂鸣器响0.1s
	{
		cnt0 = 0;
		if (flag_buzzer_250Hz == 1)
		{
			buzzer = !buzzer;  // 4ms, 250Hz
			cnt_buzzer++;
			if (cnt_buzzer == 50)
			{
				cnt_buzzer = 0;
				flag_buzzer_250Hz = 0;  // 响0.1s
			}
		}	
	}

	if (flag_matrix_key_pressed != 0)  // 长按检测计数器
		matrix_key_hold_cnt++;
}
#endif

#if USE_TIMER1
/*******************************
函数名：timer1_config
函数功能：定时器1的配置
函数形参：void
函数返回值：void
备注：
*******************************/
void timer1_config()  //11.0592MHz
{
	TMOD &= 0x0f;
	TMOD |= 0x10;  // 计数器1工作在模式1
	TR1 = 1;  // 计数器1工作
	TH1 = 0xf8;   
	TL1 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET1 = 1;  // 打开中断
}		

/*******************************
函数名：timer1_interrupt
函数功能：定时器1的中断服务函数
函数形参：void
函数返回值：void
备注：
*******************************/

void timer1_interrupt() interrupt 3
{	
	TH1 = 0xf8;   
	TL1 = 0xcd;
}
#endif

#if USE_TIMER2
/*******************************
函数名：timer2_config
函数功能：定时器2的配置
函数形参：void
函数返回值：void
备注：
*******************************/
void timer2_config()  //11.0592MHz
{
	RCLK = 0;
	TCLK = 0;
	CP_RL2 = 0;  // 计数器2工作在16位重装载模式
	TR2 = 1;  // 计数器2工作
	RCAP2H = TH2 = 0xff;   
	RCAP2L = TL2 = 0xf6;  // 65536-65526=10, 10us
	EA = 1;
	ET2 = 1;  // 打开中断
}		

/*******************************
函数名：timer2_interrupt
函数功能：定时器2的中断服务函数
函数形参：void
函数返回值：void
备注：TF2要软件清零，16位的自动重装载
*******************************/
void timer2_interrupt() interrupt 5
{	
	TF2 = 0;
}
#endif
