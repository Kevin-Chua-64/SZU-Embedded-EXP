#include "delay.h"

/*******************************
��������delay_10us
�������ܣ�10΢�뼶����ʱ
�����βΣ�u16 ten_us
��������ֵ��void
��ע��ten_us������65535
*******************************/
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

/*******************************
��������delay_ms
�������ܣ����뼶����ʱ
�����βΣ�u16 ms
��������ֵ��void
��ע��ms������65535
*******************************/
void delay_ms(unsigned int ms)
{
	unsigned int i, j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}
