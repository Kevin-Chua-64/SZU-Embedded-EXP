#include "global.h"

unsigned char cur_time_hour, cur_time_min, cur_time_sec;	// 当前时间
unsigned char timer_min=0, timer_sec=0;	 // 定时功能的时间
unsigned int temp=0, start_temp=0;  // 温度，温控开始时记录的温度
unsigned char start_duty=0;	 // 温控开始时的占空比
/*  fan_mode：0：关，1：标准模式，2：睡眠风，3：温控模式
	rotate_speed：0：不摇头，1：慢摇头，2：快摇头
	fan_speed：1：低速（灯较暗），2：中速（灯适中），3：高速（灯较亮）
	timer_mode：0：定时关，1：调节定时的秒，2：调节定时的分，3：定时开启 */
unsigned char fan_mode=0, rotate_speed=0, fan_speed=1, timer_mode=0;
bit flag_temp_update_1s=0, flag_wifi_update_2s=0;  // 间隔一段时间读取数据
bit dis_temp_state=0;  // 0显示温度，1显示状态
