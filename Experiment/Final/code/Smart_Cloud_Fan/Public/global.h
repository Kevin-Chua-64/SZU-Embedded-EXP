#ifndef _GLOBAL_H
#define _GLOBAL_H

extern unsigned char cur_time_hour, cur_time_min, cur_time_sec;
extern unsigned char timer_min, timer_sec;
extern unsigned int temp, start_temp;
extern unsigned char start_duty;  
extern unsigned char fan_mode, rotate_speed, fan_speed, timer_mode;
extern bit flag_temp_update_1s, flag_wifi_update_2s;
extern bit dis_temp_state;

#endif
