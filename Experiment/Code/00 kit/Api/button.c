#include "button.h"

/*******************************
��������button_scan
�������ܣ�����������ɨ��
�����βΣ�void
��������ֵ��u8 
��ע������ֵΪ1��2��3��4��û�н���򷵻�0xff
*******************************/
unsigned char button_scan()
{
	static xdata unsigned char key_flag=0x00;  // �����ڼ�λΪ0��ʾ�ڼ�������û����
	static xdata unsigned char buff[4]={0xff,0xff,0xff,0xff};  // ��������������8�Σ�8�β���ֵ����ͬ����Ϊ�ȶ�
	unsigned char i;

	delay_ms(1);  // ÿ1ms���һ��
	buff[0] = (buff[0]<<1) | button1;
	buff[1] = (buff[1]<<1) | button2;
	buff[2] = (buff[2]<<1) | button3;
	buff[3] = (buff[3]<<1) | button4;		

	for (i=0;i<4;i++)  //ѭ������ĸ�����
	{
		if ( ((key_flag>>i)&0x01)==0x00 && buff[i]==0x00 )
		{
			key_flag |= 0x01<<i;  // ��1����ʾ��������
			return (i+1);
		}
		else if (buff[i]==0xff) key_flag &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
	}
	
	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 
