#include "infrared.h"

/*******************************
��������ired_init
�������ܣ������ʼ��
�����βΣ�void
��������ֵ��void 
��ע���ⲿ�ж�0������
*******************************/
void ired_init()
{
	IT0=1;	//�½��ش���
	EX0=1;	//���ж�0����
	EA=1;	//�����ж�
	IRED=1;	//��ʼ���˿�
}

/*******************************
��������ired
�������ܣ��ⲿ�ж�0������
�����βΣ�void
��������ֵ��void 
��ע�����벢��������Ӧ
������ֵ��
���أ�45H      Mode��46H      ������47H
������ͣ��44H  ���ˣ�40H      �����43H
EQ��07H        VOL-��15H      VOL+��09H
0��16H         RPT��19H       U/SD��0dH
1��0cH         2��18H		  3��5eH
4��08H         5��1cH		  6��5aH
7��42H		   8��52H		  9��4aH
*******************************/
void ired() interrupt 0
{
	unsigned char gired_data[4];  //�洢4���ֽڽ����루��ַ��+��ַ����+������+���Ʒ��룩
	unsigned char ired_key;       //�洢�����ļ�ֵ 
	unsigned char ired_high_time=0;
	unsigned int time_cnt=0;
	unsigned char i=0,j=0;	 

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

}

