#include "led_running.h"

/*******************************
��������led_run
�������ܣ���ˮ��
�����βΣ�u8 dir��u8 ms
��������ֵ��void
��ע��dirΪ1��˳��
		   2������
		   3�����Һ���
	  ʱ����Ϊms
*******************************/
void led_run(unsigned char dir, unsigned char ms)
{
	static unsigned char i=0;
	if (dir==0){  // up
		P2 = (0xfe<<i)|(0xfe>>(8-i));
		if (i==8) i=0;
	}else if (dir==1){  // down
		P2 = (0x7f>>i)|(0x7f<<(8-i));
		if (i==8) i=0;
	}else if (dir==2){  // up and down
		if (i<7)
			P2 = (0xfe<<i)|(0xfe>>(8-i));
		else
			P2 = (0x7f>>(i-7))|(0x7f<<(8-(i-7)));
		if (i==14) i=0;
	}
	
	delay_ms(ms);
	i++;	
}
