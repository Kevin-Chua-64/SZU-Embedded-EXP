#include "function.h"

/*******************************
initialize and return the wifi state
*******************************/
unsigned char Init()
{
	unsigned char ret;

	timer0_config();			  
	pwm_init(100, 100);  //	��ʼռ�ձ�ռ����ʹ��led�����������ж�wifi����ֵ
	ds18b20_init();
	ired_init();
	ret = Wifi_Init();
	if (ret==0)	 // ��������ȡ��ǰʱ��
	{
		delay_ms(250);
		wifi_SendString("get time");
		while (WIFI_RX_STA == 0)  // �ȴ�������ɱ�־
		WIFI_RX_STA = 0;  // ���־
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
	if (timer_mode == 3)  // ��ʱ���ܿ�
	{
		if (flag_timer_1s==1)
		{
			flag_timer_1s = 0;
			if (timer_sec==0)
			{
				if (timer_min==0)	 // ʱ�䵽���ػ�
				{
					fan_mode = 0;  // �ػ�
					pwm_duty(0);
					rotate_speed = 0;  // ��ҡͷ
					fan_speed = 1;  // �ٶ�Ϊ1��
					timer_mode = 0;	 // ��ʱ�رչ��ܹ�
					timer_min = 0;
					timer_sec = 0;  // ��ʱ���
					dis_temp_state = 0; // ��ʼ��ʾ�¶�
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

	if (key==1)	 // ģʽ���ء���׼��˯�߷硢�¿�
	{
		fan_mode = (fan_mode+1)%4;	
		if (fan_mode==0)  // �ػ�ʱ��λ
		{
			rotate_speed = 0;  // ��ҡͷ
			fan_speed = 1;  // �ٶ�Ϊ1��
			timer_mode = 0;	 // ��ʱ�رչ��ܹ�
			timer_min = 0;
			timer_sec = 0;  // ��ʱ���
		} else if (fan_mode==3)	 // �¿ؿ�ʼʱ��¼��ʱ�¶Ⱥ�ռ�ձ�
		{
			start_temp = temp;
			start_duty = g_pwm_duty;	
		}	
	} else if (key==2 && fan_mode!=0)	// ����ҡͷ
		rotate_speed = (rotate_speed+1)%3;	
	else if (key==3 && fan_mode==1)  // ����
		fan_speed = (fan_speed%3)+1;	
	else if (key==4 && fan_mode!=0)  // ��ʱ������ʱ�ػ�
	{
		timer_mode = (timer_mode+1)%4;
		if (timer_mode==0)	// �رն�ʱ
		{
			timer_min = 0;
			timer_sec = 0;  // ��ʱ���	
		}
	}	
	else if (key==5 && fan_mode!=0)  // ��ʱ������ʱ�䣬����
	{
		if (timer_mode==1)
			timer_sec = (timer_sec+1)%60;
		else if (timer_mode==2)
			timer_min = (timer_min+1)%100;	
	} else if (key==21 && fan_mode!=0)  // ��ʱ������ʱ�䣬����
	{
		if (timer_mode==1)
			timer_sec = (timer_sec+5)%60;
		else if (timer_mode==2)
			timer_min = (timer_min+1)%100;	
	} else if (key==6)	// �л���ʾ�¶Ȼ����״̬
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
	if (fan_mode==0)  // �ػ�
		pwm_duty(0);
	else if (fan_mode==1)  // ��׼ģʽ
		pwm_duty(fan_speed*40-30);  // 1, 5, 9
	else if (fan_mode==2)  // ˯�߷�
		pwm_sleep_wind();
	else if (fan_mode==3)  // �¿�ģʽ
		pwm_temp_ctrl();

	step_motor_round();  // ҡͷ
}

/*******************************
display on the nixie tube 
*******************************/
unsigned char code led_num[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
void nixie()
{
	unsigned char ch[8];

	ch[4] = 0x00;
	if (timer_mode == 0)  // ��ʾʱ��
	{
		ch[0] = led_num[cur_time_hour/10];	
		ch[1] = led_num[cur_time_hour%10];
		ch[2] = led_num[cur_time_min/10];
		ch[3] = led_num[cur_time_min%10];
	} else  // ��ʾ��ʱ
	{
		ch[0] = led_num[timer_min/10];	
		ch[1] = led_num[timer_min%10];
		ch[2] = led_num[timer_sec/10];
		ch[3] = led_num[timer_sec%10];
	}

	if (dis_temp_state == 0)  // ��ʾ�¶�
	{
		ch[5] = led_num[temp%1000/100];
		ch[6] = led_num[temp%1000%100/10];
		ch[6] |= 0x80;  // С����
		ch[7] = led_num[temp%1000%100%10];
	} else  // ��ʾ״̬
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
		make_str();  // �ϳɱ������ݵ��ַ���
		wifi_SendString(wifi_send_buf);	 // ���ͱ������ݸ�wifi
		Wifi_Analysis_Response();  // wifi��Ӧ���ı�ȫ�ֱ���  
	}
}
