#include "infrared.h"

void ired_init()
{
	IT0=1;	//�½��ش���
	EX0=1;	//���ж�0����
	EA=1;	//�����ж�
	IRED=1;	//��ʼ���˿�
}

/*******************************
the external interrupt
decode the NEC data and action
key value:
���أ�45H      Mode��46H      ������47H
������ͣ��44H  ���ˣ�40H      �����43H
EQ��07H        VOL-��15H      VOL+��09H
0��16H         RPT��19H       U/SD��0dH
1��0cH         2��18H		  3��5eH
4��08H         5��1cH		  6��5aH
7��42H		   8��52H		  9��4aH
*******************************/
bit flag_infrared_1s=0;
void ired() interrupt 0
{
	unsigned char gired_data[4];  //�洢4���ֽڽ����루��ַ��+��ַ����+������+���Ʒ��룩
	unsigned char ired_key;       //�洢�����ļ�ֵ 
	unsigned char ired_high_time=0;
	unsigned int time_cnt=0;
	unsigned char i=0,j=0;	 
	static unsigned char pre_key;

	if(IRED==0)
	{		
		time_cnt=1000;
		while((!IRED)&&(time_cnt))//�ȴ������ź�9ms�͵�ƽ������������10msǿ���˳�
		{
			delay_10us(1);//��ʱԼ10us
			time_cnt--;
			if(time_cnt==0)return;		
		}
		if(IRED)//�����ź�9ms�͵�ƽ�ѹ�������4.5ms�ߵ�ƽ
		{
			time_cnt=500;
			while(IRED&&time_cnt)//�ȴ������ź�4.5ms�ߵ�ƽ������������5msǿ���˳�
			{
				delay_10us(1);
				time_cnt--;
				if(time_cnt==0)return;	
			}
			for(i=0;i<4;i++)//ѭ��4�Σ���ȡ4���ֽ�����
			{
				for(j=0;j<8;j++)//ѭ��8�ζ�ȡÿλ���ݼ�һ���ֽ�
				{
					time_cnt=600;
					while((IRED==0)&&time_cnt)//�ȴ�����1��0ǰ���0.56ms������������6msǿ���˳�
					{
						delay_10us(1);
						time_cnt--;
						if(time_cnt==0)return;	
					}
					time_cnt=20;
					while(IRED)//�ȴ�����1��0����ĸߵ�ƽ������������2msǿ���˳�
					{
						delay_10us(10);//Լ0.1ms
						ired_high_time++;
						if(ired_high_time>20)return;	
					}
					gired_data[i]>>=1;//�ȶ�ȡ��Ϊ��λ��Ȼ���Ǹ�λ
					if(ired_high_time>=8)//����ߵ�ƽʱ�����0.8ms��������Ϊ1������Ϊ0
						gired_data[i]|=0x80;
					ired_high_time=0;//�������㣬�ȴ���һ�μ���ʱ��
				}
			}
		}
		if(gired_data[2]!=~gired_data[3])//У��������뷴�룬�����򷵻�
		{
			for(i=0;i<4;i++)
				gired_data[i]=0;
			return;	
		}
	}

	// ����
	ired_key = gired_data[2]/16;  //�������4λ
	ired_key <<= 4;
	ired_key |= gired_data[2]%16;  //�������4λ

	// ���½��к�������Ĳ���	
	//1��0cH         2��18H		  3��5eH
	//4��08H         5��1cH		  6��5aH
	if (flag_infrared_1s == 1)
	{
		flag_infrared_1s = 0;
		if (ired_key==0x0c)	 // ģʽ���ء���׼��˯�߷硢�¿�
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
		} else if (ired_key==0x18 && fan_mode!=0)	// ����ҡͷ
			rotate_speed = (rotate_speed+1)%3;	
		else if (ired_key==0x5e && fan_mode==1)  // ����
			fan_speed = (fan_speed%3)+1;	
		else if (ired_key==0x08 && fan_mode!=0)  // ��ʱ������ʱ�ػ�
		{
			timer_mode = (timer_mode+1)%4;
			if (timer_mode==0)	// �رն�ʱ
			{
				timer_min = 0;
				timer_sec = 0;  // ��ʱ���	
			}
		}	
		else if (ired_key==0x1c && pre_key!=0x1c && fan_mode!=0)  // ��ʱ������ʱ�䣬����
		{
			if (timer_mode==1)
				timer_sec = (timer_sec+1)%60;
			else if (timer_mode==2)
				timer_min = (timer_min+1)%100;	
		} else if (ired_key==0x1c && pre_key==0x1c && fan_mode!=0)  // ��ʱ������ʱ�䣬����
		{
			if (timer_mode==1)
				timer_sec = (timer_sec+5)%60;
			else if (timer_mode==2)
				timer_min = (timer_min+1)%100;	
		} else if (ired_key==0x5a)	// �л���ʾ�¶Ȼ����״̬
			dis_temp_state = ~dis_temp_state;
			
		pre_key = ired_key;	
	}
}
