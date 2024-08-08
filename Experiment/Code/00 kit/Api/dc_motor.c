#include "dc_motor.h"

xdata unsigned char g_motor_tim_scale, g_motor_duty;
/*******************************
函数名：DC_Motor_init
函数功能：DC_Motor初始化
函数形参：u8 tim_H, u8 tim_L, u8 tim_scale, u8 duty
函数返回值：void 
备注：计数器的记一次的时间（tim_H,tim_L决定） * tim_sclae = 周期，duty占空比，取值0~tim_scale
	  占用计时器0   P1^0端口
*******************************/
void DC_Motor_init(unsigned char tim_H, unsigned char tim_L, unsigned char tim_scale, unsigned char duty)  //11.0592MHz
{
	g_motor_tim_scale = tim_scale;
	g_motor_duty = duty;

	#if USE_DC_MOTOR_TIMER2
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
函数名：DC_Motor_duty
函数功能：DC_Motor改变占空比
函数形参：u8 duty
函数返回值：void 
备注：
*******************************/
void DC_Motor_duty(unsigned char duty)
{
	g_motor_duty = duty;
}

/*******************************
函数名：DC_Motor_interrupt
函数功能：DC_Motor中断服务函数
函数形参：void
函数返回值：void 
备注：定时器0的中断
*******************************/
#if	USE_DC_MOTOR_TIMER2_INT
void DC_Motor_interrupt() interrupt 5
{
	static unsigned char cnt=0;
	
	cnt++;
	if (cnt >= g_motor_tim_scale) cnt = 0;  // 周期到了
	if (cnt <= g_motor_duty) DC_Motor = 1;
	else DC_Motor = 0;
}
#endif

/*******************************
函数名：DC_Motor_Run
函数功能：控制直流电机速度，按键扫描，1：加速，2：减速
函数形参：u8 tim_scale
函数返回值：void 
备注：motor为P1^0，占空比阈值，最小值等于0，最大值等于tim_scale
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
