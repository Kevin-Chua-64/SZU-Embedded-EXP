#ifndef _MATRIX_KEY_H
#define _MATRIX_KEY_H

#include <reg52.h>

#define USE_MATRIX_KEY_TIMER0 0
#define USE_MATRIX_KEY_TIMER0_INT 0

sbit matrix_key_col_1 = P1^3;
sbit matrix_key_col_2 = P1^2;
sbit matrix_key_col_3 = P1^1;
sbit matrix_key_col_4 = P1^0;

extern bit flag_matrix_key;	// 定时器的标志
extern unsigned char flag_matrix_key_pressed;	// 当检测到单击时置第四位对应的位1，计时器开始计数
extern unsigned char matrix_key_hold_cnt;  // 长按计数
extern bit flag_key_long_press_output_500ms;  // 长按时每0.5s返回一次键值
extern bit flag_buzzer_250Hz;  // 单击音效标志
extern bit flag_buzzer_500Hz;  // 长按开始时音效标志

unsigned char matrix_key_scan();

#if USE_MATRIX_KEY_TIMER0

void matrix_key_timer0_config();

#endif

#endif
