#include "global.h"

unsigned char cur_time_hour, cur_time_min, cur_time_sec;	// ��ǰʱ��
unsigned char timer_min=0, timer_sec=0;	 // ��ʱ���ܵ�ʱ��
unsigned int temp=0, start_temp=0;  // �¶ȣ��¿ؿ�ʼʱ��¼���¶�
unsigned char start_duty=0;	 // �¿ؿ�ʼʱ��ռ�ձ�
/*  fan_mode��0���أ�1����׼ģʽ��2��˯�߷磬3���¿�ģʽ
	rotate_speed��0����ҡͷ��1����ҡͷ��2����ҡͷ
	fan_speed��1�����٣��ƽϰ�����2�����٣������У���3�����٣��ƽ�����
	timer_mode��0����ʱ�أ�1�����ڶ�ʱ���룬2�����ڶ�ʱ�ķ֣�3����ʱ���� */
unsigned char fan_mode=0, rotate_speed=0, fan_speed=1, timer_mode=0;
bit flag_temp_update_1s=0, flag_wifi_update_2s=0;  // ���һ��ʱ���ȡ����
bit dis_temp_state=0;  // 0��ʾ�¶ȣ�1��ʾ״̬
