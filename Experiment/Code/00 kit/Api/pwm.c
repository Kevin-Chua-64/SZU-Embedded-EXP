#include "pwm.h"

xdata unsigned char g_pwm_tim_scale, g_pwm_duty;
/*******************************
函数名：pwm_init
函数功能：pwm初始化
函数形参：u8 tim_H, u8 tim_L, u8 tim_scale, u8 duty
函数返回值：void 
备注：计数器的记一次的时间（tim_H,tim_L决定） * tim_sclae = 周期，duty占空比，取值0~tim_scale
	  占用计时器2
*******************************/
void pwm_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_pwm_tim_scale = tim_scale;
	g_pwm_duty = duty;

	#if USE_PWM_TIMER2
	RCLK = 0;
	TCLK = 0;
	CP_RL2 = 0;  // 计数器2工作在16位重装载模式
	TR2 = 1;  // 计数器2工作
	RCAP2H = TH2 = tim_H;  // 0xff 
	RCAP2L = TL2 = tim_L;  // 0xf6 -> 0.01ms
	EA = 1;
	ET2 = 1;  // 打开中断
	#endif
}

/*******************************
函数名：pwm_duty
函数功能：pwm改变占空比
函数形参：u8 duty
函数返回值：void 
备注：
*******************************/
void pwm_duty(unsigned char duty)
{
	g_pwm_duty = duty;
}

/*******************************
函数名：pwm_interrupt
函数功能：pwm中断服务函数
函数形参：void
函数返回值：void 
备注：定时器2的中断
*******************************/
#if	USE_PWM_TIMER2_INT
void pwm_interrupt() interrupt 5
{
	static unsigned char cnt=0;
	
	cnt++;
	if (cnt >= g_pwm_tim_scale) cnt = 0;  // 周期到了
	if (cnt <= g_pwm_duty) PWM = 1;
	else PWM = 0;
}
#endif

/*******************************
函数名：pwm_led2
函数功能：pwm控制led2呼吸灯
函数形参：u8 duty_max, u8 char duty_min
函数返回值：void 
备注：led为P2^1，占空比的阈值（最小值等于零，最大值等于tim_scale）
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
