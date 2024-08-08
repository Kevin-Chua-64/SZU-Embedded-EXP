#include "button.h"

/*******************************
函数名：button_scan
函数功能：独立按键的扫描
函数形参：void
函数返回值：u8 
备注：返回值为1，2，3，4，没有结果则返回0xff
*******************************/
unsigned char button_scan()
{
	static xdata unsigned char key_flag=0x00;  // 倒数第几位为0表示第几个按键没按下
	static xdata unsigned char buff[4]={0xff,0xff,0xff,0xff};  // 消抖，连续采样8次，8次采样值均相同才视为稳定
	unsigned char i;

	delay_ms(1);  // 每1ms检测一次
	buff[0] = (buff[0]<<1) | button1;
	buff[1] = (buff[1]<<1) | button2;
	buff[2] = (buff[2]<<1) | button3;
	buff[3] = (buff[3]<<1) | button4;		

	for (i=0;i<4;i++)  //循环检测四个按键
	{
		if ( ((key_flag>>i)&0x01)==0x00 && buff[i]==0x00 )
		{
			key_flag |= 0x01<<i;  // 置1，表示按键按下
			return (i+1);
		}
		else if (buff[i]==0xff) key_flag &= ~(0x01<<i);  // 置0，表示按键松开
	}
	
	return 0xff;  // 表示按键松开或者处于不稳定状态
} 
