#include "matrix_key.h"

/*******************************
single press return value: 1~16, 250Hz beep for 0.1s
1  2  3  4
5  6  7  8
9  10 11 12
13 14 15 16
long press retuen value: 17~32, add 16 to the single press return value, 500Hz beep for 0.3s
no press: return 0xff
*******************************/
bit flag_matrix_key = 0;
unsigned char flag_matrix_key_pressed = 0;
unsigned char matrix_key_hold_cnt=0;
bit flag_key_long_press_output_1s = 0;
bit flag_buzzer_250Hz=0;
bit flag_buzzer_500Hz=0;
unsigned char matrix_key_scan()
{
	unsigned char key_value=0xff;
	static unsigned char j=0, k=0;
	static xdata unsigned char key_flag[4]={0,0,0,0};  // 倒数第几位为0表示第几个按键没按下
	static xdata unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};  // 消抖，连续采样8次，8次采样值均相同才视为稳定
	static xdata unsigned char key_press_no[4]={0,0,0,0};  // 单击后记录是哪个按键单击，没有则为0
	static xdata unsigned char key_hold_no[4]={0,0,0,0};  // 已经长按的按键，没有则为0
	unsigned char i;

	switch(j)  // 换行扫描，对应行输入置0，低电平
	{
		case 0: P1 = 0x7f; break;
		case 1: P1 = 0xbf; break;
		case 2: P1 = 0xdf; break;
		case 3: P1 = 0xef; break;
	}

	if (flag_matrix_key == 1)  // 每2ms检测一次
	{
		flag_matrix_key = 0;
		buff[j][0] = (buff[j][0]<<1) | matrix_key_col_1;
		buff[j][1] = (buff[j][1]<<1) | matrix_key_col_2;
		buff[j][2] = (buff[j][2]<<1) | matrix_key_col_3;
		buff[j][3] = (buff[j][3]<<1) | matrix_key_col_4;		

		for (i=0;i<4;i++)  //循环检测四个按键
		{
			if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )  // 当前flag没按下，buff已经稳定
			{
				key_flag[j] |= 0x01<<i;	 // 置1，表示按键按下
				key_value = 4*j+i + 1;
			}
			else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // 置0，表示按键松开
		}

		// 判断长按		
		if (key_hold_no[k]!=0)  // 已经有按键处于长按状态
		{
			if (key_value != key_hold_no[k])	// 键值跟之前不一样或者没有按键按下
			{
				key_hold_no[k] = 0;
				key_press_no[k] = 0;	
			}
			else  // 仍然长按中
			{
				key_value = key_hold_no[k] + 16;
				key_flag[j] &= ~(0x01<<((key_value-1)%4));	// 置0，表示按键松开，才能再次检测单击
			}
		} else if (key_value!=0xff)	 // 目前没有长按且有按键按下
		{	
			if ( ( (flag_matrix_key_pressed>>k)&0x01 ) == 0 )	 // 为0，第一次单击
			{
				key_press_no[k] = key_value;  // 记录单击的按键
				matrix_key_hold_cnt = 0;  // 清零计数器
				flag_matrix_key_pressed |= (1<<k);  // 置1，计数器开始计数
				key_flag[j] &= ~(0x01<<((key_value-1)%4));	// 置0，表示按键松开，才能再次检测单击
			}else  // 之前已经单击了
			{
				if (key_value == key_press_no[k])  // 单击过了同一按键
				{
					key_flag[j] &= ~(0x01<<((key_value-1)%4));	// 置0，表示按键松开，才能再次检测单击
					key_value = 0xff;  // 单击过了不再返回键值
				} else	// 新的单击
				{
					key_press_no[k] = key_value;  // 记录单击的按键
					matrix_key_hold_cnt = 0;  // 清零计数器重新开始计数
					key_flag[j] &= ~(0x01<<((key_value-1)%4));	// 置0，表示按键松开，才能再次检测单击
				}
			}
			
			if (matrix_key_hold_cnt>=250)  // 判断为长按，500ms
			{
				flag_matrix_key_pressed &= ~(1<<k);  // 置0，停止计数
				key_hold_no[k] = key_press_no[k];	 // 记录长按的按键
				key_value = key_hold_no[k] + 16;  // 返回键值+16代表长按
				flag_buzzer_500Hz = 1;  // 长按开始时音效							
			}
		} else	// 0xff，松开
		{
			key_hold_no[k] = 0;
			key_press_no[k] = 0;
			flag_matrix_key_pressed &= ~(1<<k);	
		}

		if (key_value>=17 && key_value<=32)  // 长按每1s返回一次键值
		{
			if (flag_key_long_press_output_1s == 1)
				flag_key_long_press_output_1s = 0;
			else key_value = 0xff; 
		}

		k++;
		if (k==4) k = 0;  // 长按检测，每四次才会扫描同一行		
		j++;
		if (j==4) j = 0;  // 换行扫描
	}

	if (key_value < 17) flag_buzzer_250Hz = 1;	// 单击音效
	return key_value;
}
