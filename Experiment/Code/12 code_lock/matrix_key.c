#include "matrix_key.h"

/*******************************
函数名：matrix_key_scan
函数功能：矩阵按键的扫描
函数形参：void
函数返回值：u8 
备注：返回值为1~16，按列
没有结果则返回0xff
*******************************/
unsigned char matrix_key_scan()
{
	static unsigned char j=0;
	static unsigned char key_flag[4]={0x00,0x00,0x00,0x00};  // 四行，倒数第几位为0表示第几列按键没按下
	static unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};  // 消抖，连续采样8次，8次采样值均相同才视为稳定
	unsigned char i;

	switch(j)  // 换行扫描，对应行输入置0，低电平
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

	for (i=0;i<4;i++)  //循环检测四个按键
	{
		if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )
		{
			key_flag[j] |= 0x01<<i;  // 置1，表示按键按下
			return (4*j+i);
		}
		else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // 置0，表示按键松开
	}

	j++;
	if (j==4) j = 0;

	return 0xff;  // 表示按键松开或者处于不稳定状态
} 
