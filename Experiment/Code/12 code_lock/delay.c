#include "delay.h"

/*******************************
��������delay_ms
�������ܣ����뼶����ʱ
�����βΣ�u8 z
��������ֵ��void
��ע��z������255
*******************************/
void delay_ms(unsigned char z)
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}
