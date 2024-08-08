#ifndef _MATRIX_KEY_H
#define _MATRIX_KEY_H

#include <reg52.h>

#define USE_MATRIX_KEY_TIMER0 0
#define USE_MATRIX_KEY_TIMER0_INT 0

sbit matrix_key_col_1 = P1^3;
sbit matrix_key_col_2 = P1^2;
sbit matrix_key_col_3 = P1^1;
sbit matrix_key_col_4 = P1^0;

extern bit flag_matrix_key;	// ��ʱ���ı�־
extern unsigned char flag_matrix_key_pressed;	// ����⵽����ʱ�õ���λ��Ӧ��λ1����ʱ����ʼ����
extern unsigned char matrix_key_hold_cnt;  // ��������
extern bit flag_key_long_press_output_500ms;  // ����ʱÿ0.5s����һ�μ�ֵ
extern bit flag_buzzer_250Hz;  // ������Ч��־
extern bit flag_buzzer_500Hz;  // ������ʼʱ��Ч��־

unsigned char matrix_key_scan();

#if USE_MATRIX_KEY_TIMER0

void matrix_key_timer0_config();

#endif

#endif
