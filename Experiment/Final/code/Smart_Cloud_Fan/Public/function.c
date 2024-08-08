#include "function.h"

/*******************************
initialize and return the wifi state
*******************************/
unsigned char Init()
{
	unsigned char ret;

	timer0_config();			  
	pwm_init(100, 100);  //	初始占空比占满，使得led不亮，方便判断wifi返回值
	ds18b20_init();
	ired_init();
	ret = Wifi_Init();
	if (ret==0)	 // 正常，获取当前时间
	{
		delay_ms(250);
		wifi_SendString("get time");
		while (WIFI_RX_STA == 0)  // 等待接收完成标志
		WIFI_RX_STA = 0;  // 清标志
		Wifi_GetTime();
	}
	return ret;
}

/*******************************
update current time
if wifi is connectted, it might be useless 
*******************************/
bit flag_cur_time_1s=0;
void cur_time_update()
{
	if (flag_cur_time_1s==1)
	{
		flag_cur_time_1s = 0;
		cur_time_sec++;
		if (cur_time_sec==60)
		{
			cur_time_sec = 0;
			cur_time_min++;
			if (cur_time_min==60)
			{
				cur_time_min = 0;
				cur_time_hour++;
				if (cur_time_hour==24) cur_time_hour = 0;	
			}
		}
	}	
}

/*******************************
count for timer
*******************************/
bit flag_timer_1s=0;
void timer_count()
{
	if (timer_mode == 3)  // 定时功能开
	{
		if (flag_timer_1s==1)
		{
			flag_timer_1s = 0;
			if (timer_sec==0)
			{
				if (timer_min==0)	 // 时间到，关机
				{
					fan_mode = 0;  // 关机
					pwm_duty(0);
					rotate_speed = 0;  // 不摇头
					fan_speed = 1;  // 速度为1档
					timer_mode = 0;	 // 定时关闭功能关
					timer_min = 0;
					timer_sec = 0;  // 定时清空
					dis_temp_state = 0; // 初始显示温度
				}else
				{
					timer_min--;
					timer_sec = 59;
				}
			}else timer_sec--;
		}	
	}	
}

/*******************************
sacn the keys and update the global variable
*******************************/
void key_state_update()
{
	unsigned char key;
	
	key = matrix_key_scan();

	if (key==1)	 // 模式：关、标准、睡眠风、温控
	{
		fan_mode = (fan_mode+1)%4;	
		if (fan_mode==0)  // 关机时复位
		{
			rotate_speed = 0;  // 不摇头
			fan_speed = 1;  // 速度为1档
			timer_mode = 0;	 // 定时关闭功能关
			timer_min = 0;
			timer_sec = 0;  // 定时清空
		} else if (fan_mode==3)	 // 温控开始时记录当时温度和占空比
		{
			start_temp = temp;
			start_duty = g_pwm_duty;	
		}	
	} else if (key==2 && fan_mode!=0)	// 左右摇头
		rotate_speed = (rotate_speed+1)%3;	
	else if (key==3 && fan_mode==1)  // 风速
		fan_speed = (fan_speed%3)+1;	
	else if (key==4 && fan_mode!=0)  // 定时器倒计时关机
	{
		timer_mode = (timer_mode+1)%4;
		if (timer_mode==0)	// 关闭定时
		{
			timer_min = 0;
			timer_sec = 0;  // 定时清空	
		}
	}	
	else if (key==5 && fan_mode!=0)  // 定时器设置时间，单击
	{
		if (timer_mode==1)
			timer_sec = (timer_sec+1)%60;
		else if (timer_mode==2)
			timer_min = (timer_min+1)%100;	
	} else if (key==21 && fan_mode!=0)  // 定时器设置时间，长按
	{
		if (timer_mode==1)
			timer_sec = (timer_sec+5)%60;
		else if (timer_mode==2)
			timer_min = (timer_min+1)%100;	
	} else if (key==6)	// 切换显示温度或风扇状态
		dis_temp_state = ~dis_temp_state;
}

/*******************************
take action according to the global variable
*******************************/
void aciton()
{
	cur_time_update();
	timer_count();
	// pwm
	if (fan_mode==0)  // 关机
		pwm_duty(0);
	else if (fan_mode==1)  // 标准模式
		pwm_duty(fan_speed*40-30);  // 1, 5, 9
	else if (fan_mode==2)  // 睡眠风
		pwm_sleep_wind();
	else if (fan_mode==3)  // 温控模式
		pwm_temp_ctrl();

	step_motor_round();  // 摇头
}

/*******************************
display on the nixie tube 
*******************************/
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
void nixie()
{
	unsigned char ch[8];

	ch[4] = 0x00;
	if (timer_mode == 0)  // 显示时间
	{
		ch[0] = led_num[cur_time_hour/10];	
		ch[1] = led_num[cur_time_hour%10];
		ch[2] = led_num[cur_time_min/10];
		ch[3] = led_num[cur_time_min%10];
	} else  // 显示定时
	{
		ch[0] = led_num[timer_min/10];	
		ch[1] = led_num[timer_min%10];
		ch[2] = led_num[timer_sec/10];
		ch[3] = led_num[timer_sec%10];
	}

	if (dis_temp_state == 0)  // 显示温度
	{
		ch[5] = led_num[temp%1000/100];
		ch[6] = led_num[temp%1000%100/10];
		ch[6] |= 0x80;  // 小数点
		ch[7] = led_num[temp%1000%100%10];
	} else  // 显示状态
	{
		ch[5] = led_num[fan_mode];
		ch[6] = led_num[rotate_speed];
		ch[7] = led_num[fan_speed];
	}
	digital_display(ch);
}

/*******************************
update the temperature
*******************************/
void temp_update()
{
	if (flag_temp_update_1s == 1)
	{
		flag_temp_update_1s = 0;
		temp = ds18b20_read_temperture();
	}
}

/*******************************
make the wifi send string and store in the send buffer
*******************************/
xdata unsigned char wifi_send_buf[23];
void make_str()
{
	sprintf(wifi_send_buf, "%c,%c,%c,%c,%c,%c,%c,%c",\							
		fan_mode+'0', rotate_speed+'0', fan_speed+'0',\
		timer_mode+'0', timer_min/10+'0', timer_min%10+'0',\
		timer_sec/10+'0', timer_sec%10+'0');
}

/*******************************
update and synchronize the wifi data
*******************************/
void wifi_update()
{
	if (flag_wifi_update_2s == 1)
	{
		flag_wifi_update_2s = 0;
		make_str();  // 合成本地数据的字符串
		wifi_SendString(wifi_send_buf);	 // 发送本地数据给wifi
		Wifi_Analysis_Response();  // wifi响应并改变全局变量  
	}
}
