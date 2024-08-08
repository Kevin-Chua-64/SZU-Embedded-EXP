#include "matrix_key.h"

#if USE_MATRIX_KEY_TIMER0
/*******************************
��������matrix_key_timer0_config
�������ܣ���ʱ��0������
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void matrix_key_timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // ������0������ģʽ1
	TR0 = 1;  // ������0����
	TH0 = 0xf8;   
	TL0 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET0 = 1;  // ���ж�
}
#endif

/*******************************
��������matrix_key_scan
�������ܣ����󰴼���ɨ�裬����(250Hz)���߳�����ʼʱ(500Hz)����Ч
�����βΣ�void
��������ֵ��u8 
��ע������ֵΪ1~16��
1  2  3  4
5  6  7  8
9  10 11 12
13 14 15 16
��������ԭ��������+16
û�н���򷵻�0xff
*******************************/
bit flag_matrix_key = 0;
unsigned char flag_matrix_key_pressed = 0;
unsigned char matrix_key_hold_cnt=0;
bit flag_key_long_press_output_500ms = 0;
bit flag_buzzer_250Hz=0;
bit flag_buzzer_500Hz=0;
unsigned char matrix_key_scan()
{
	unsigned char key_value=0xff;
	static unsigned char j=0, k=0;
	static xdata unsigned char key_flag[4]={0,0,0,0};  // �����ڼ�λΪ0��ʾ�ڼ�������û����
	static xdata unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};  // ��������������8�Σ�8�β���ֵ����ͬ����Ϊ�ȶ�
	static xdata unsigned char key_press_no[4]={0,0,0,0};  // �������¼���ĸ�����������û����Ϊ0
	static xdata unsigned char key_hold_no[4]={0,0,0,0};  // �Ѿ������İ�����û����Ϊ0
	unsigned char i;

	switch(j)  // ����ɨ�裬��Ӧ��������0���͵�ƽ
	{
		case 0: P1 = 0x7f; break;
		case 1: P1 = 0xbf; break;
		case 2: P1 = 0xdf; break;
		case 3: P1 = 0xef; break;
	}

	if (flag_matrix_key == 1)  // ÿ2ms���һ��
	{
		flag_matrix_key = 0;
		buff[j][0] = (buff[j][0]<<1) | matrix_key_col_1;
		buff[j][1] = (buff[j][1]<<1) | matrix_key_col_2;
		buff[j][2] = (buff[j][2]<<1) | matrix_key_col_3;
		buff[j][3] = (buff[j][3]<<1) | matrix_key_col_4;		

		for (i=0;i<4;i++)  //ѭ������ĸ�����
		{
			if ( ((key_flag[j]>>i)&0x01)==0x00 && buff[j][i]==0x00 )  // ��ǰflagû���£�buff�Ѿ��ȶ�
			{
				key_flag[j] |= 0x01<<i;	 // ��1����ʾ��������
				key_value = 4*j+i + 1;
			}
			else if (buff[j][i]==0xff) key_flag[j] &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
		}

		// �жϳ���		
		if (key_hold_no[k]!=0)  // �Ѿ��а������ڳ���״̬
		{
			if (key_value != key_hold_no[k])	// ��ֵ��֮ǰ��һ������û�а�������
			{
				key_hold_no[k] = 0;
				key_press_no[k] = 0;	
			}
			else  // ��Ȼ������
			{
				key_value = key_hold_no[k] + 16;
				key_flag[j] &= ~(0x01<<((key_value-1)%4));	// ��0����ʾ�����ɿ��������ٴμ�ⵥ��
			}
		} else if (key_value!=0xff)	 // Ŀǰû�г������а�������
		{	
			if ( ( (flag_matrix_key_pressed>>k)&0x01 ) == 0 )	 // Ϊ0����һ�ε���
			{
				key_press_no[k] = key_value;  // ��¼�����İ���
				matrix_key_hold_cnt = 0;  // ���������
				flag_matrix_key_pressed |= (1<<k);  // ��1����������ʼ����
				key_flag[j] &= ~(0x01<<((key_value-1)%4));	// ��0����ʾ�����ɿ��������ٴμ�ⵥ��
			}else  // ֮ǰ�Ѿ�������
			{
				if (key_value == key_press_no[k])  // ��������ͬһ����
				{
					key_flag[j] &= ~(0x01<<((key_value-1)%4));	// ��0����ʾ�����ɿ��������ٴμ�ⵥ��
					key_value = 0xff;  // �������˲��ٷ��ؼ�ֵ
				} else	// �µĵ���
				{
					key_press_no[k] = key_value;  // ��¼�����İ���
					matrix_key_hold_cnt = 0;  // ������������¿�ʼ����
					key_flag[j] &= ~(0x01<<((key_value-1)%4));	// ��0����ʾ�����ɿ��������ٴμ�ⵥ��
				}
			}
			
			if (matrix_key_hold_cnt>=250)  // �ж�Ϊ������500ms
			{
				flag_matrix_key_pressed &= ~(1<<k);  // ��0��ֹͣ����
				key_hold_no[k] = key_press_no[k];
				key_value = key_hold_no[k] + 16;  // ���ؼ�ֵ+16������
				flag_buzzer_500Hz = 1;  // ������ʼʱ��Ч							
			}
		} else	// 0xff���ɿ�
		{
			key_hold_no[k] = 0;
			key_press_no[k] = 0;
			flag_matrix_key_pressed &= ~(1<<k);	
		}

		if (key_value>=17 && key_value<=32)  // ����ÿ0.5s���һ�μ�ֵ
		{
			if (flag_key_long_press_output_500ms == 1)
				flag_key_long_press_output_500ms = 0;
			else key_value = 0xff; 
		}

		k++;
		if (k==4) k = 0;  // ������⣬ÿ�ĴβŻ�ɨ��ͬһ��		
		j++;
		if (j==4) j = 0;  // ����ɨ��
	}

	if (key_value < 17) flag_buzzer_250Hz = 1;	// ������Ч
	return key_value;
} 

#if	USE_MATRIX_KEY_TIMER0_INT
/*******************************
��������matrix_key_timer0_interrupt
�������ܣ���ʱ��0���жϷ�����
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void matrix_key_timer0_interrupt() interrupt 1
{
	TH0 = 0xf8;   
	TL0 = 0xcd;
	flag_matrix_key = 1;
	if (flag_matrix_key_pressed!=0)
		matrix_key_hold_cnt++;
}
#endif
