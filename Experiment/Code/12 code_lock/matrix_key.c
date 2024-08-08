#include "matrix_key.h"

/*******************************
��������matrix_key_scan
�������ܣ����󰴼���ɨ��
�����βΣ�void
��������ֵ��u8 
��ע������ֵΪ1~16������
û�н���򷵻�0xff
*******************************/
unsigned char matrix_key_scan()
{
	static unsigned char j=0;
	static unsigned char key_flag[4]={0x00,0x00,0x00,0x00};  // ���У������ڼ�λΪ0��ʾ�ڼ��а���û����
	static unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};  // ��������������8�Σ�8�β���ֵ����ͬ����Ϊ�ȶ�
	unsigned char i;

	switch(j)  // ����ɨ�裬��Ӧ��������0���͵�ƽ
	{
		case 0: P1 = 0xf7; break;
		case 1: P1 = 0xfb; break;
		case 2: P1 = 0xfd; break;
		case 3: P1 = 0xfe; break;
	}

	buff[j][0] = (buff[j][0]<<1) | matrix_k1;
	buff[j][1] = (buff[j][1]<<1) | matrix_k2;
	buff[j][2] = (buff[j][2]<<1) | matrix_k3;
	buff[j][3] = (buff[j][3]<<1) | matrix_k4;		

	for (i=0;i<4;i++)  //ѭ������ĸ�����
	{
		if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )
		{
			key_flag[j] |= 0x01<<i;  // ��1����ʾ��������
			return (4*j+i);
		}
		else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 
