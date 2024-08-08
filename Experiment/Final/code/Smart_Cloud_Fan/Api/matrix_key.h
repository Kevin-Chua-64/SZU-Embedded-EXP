#ifndef _MATRIX_KEY_H
#define _MATRIX_KEY_H

#include <reg52.h>

sbit matrix_key_col_1 = P1^3;
sbit matrix_key_col_2 = P1^2;
sbit matrix_key_col_3 = P1^1;
sbit matrix_key_col_4 = P1^0;

extern bit flag_matrix_key;	// ��ʱ���ı�־
extern unsigned char flag_matrix_key_pressed;	// ����⵽����ʱ�õ���λ��Ӧ��λ1����ʱ����ʼ����
extern unsigned char matrix_key_hold_cnt;  // ��������
extern bit flag_key_long_press_output_1s;  // ����ʱÿ1s����һ�μ�ֵ
extern bit flag_buzzer_250Hz;  // ������Ч��־
extern bit flag_buzzer_500Hz;  // ������ʼʱ��Ч��־

unsigned char matrix_key_scan();

#endif
