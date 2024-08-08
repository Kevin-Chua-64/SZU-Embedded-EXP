#ifndef _LCD1602_H
#define _LCD1602_H

#include <reg52.h>
#include "delay.h"

sbit LCD1602_RS=P2^6;  // 0����1����ѡ��
sbit LCD1602_RW=P2^5;  // 0д1��ѡ��
sbit LCD1602_E=P2^7;  // ʹ���ź�
#define LCD1602_DATAPORT P0	//�궨��LCD1602���ݶ˿�

void lcd1602_cursor_pos_set(unsigned char x,unsigned char y);
void lcd1602_init();
void lcd1602_clear();
void lcd1602_show_string(char *str);

#endif
