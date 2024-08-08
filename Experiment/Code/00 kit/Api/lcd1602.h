#ifndef _LCD1602_H
#define _LCD1602_H

#include <reg52.h>
#include "delay.h"

sbit LCD1602_RS=P2^6;  // 0命令1数据选择
sbit LCD1602_RW=P2^5;  // 0写1读选择
sbit LCD1602_E=P2^7;  // 使能信号
#define LCD1602_DATAPORT P0	//宏定义LCD1602数据端口

void lcd1602_cursor_pos_set(unsigned char x,unsigned char y);
void lcd1602_init();
void lcd1602_clear();
void lcd1602_show_string(char *str);

#endif
