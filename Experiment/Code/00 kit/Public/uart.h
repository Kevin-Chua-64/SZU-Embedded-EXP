#ifndef _UART_H
#define _UART_H

#include <reg52.h>
#include "delay.h"
#include <string.h>
#include <stdio.h>

#define USE_UART_TIMER1 0

#if USE_UART_TIMER1
#define UART_REC_LEN 20  //��������ֽ���
extern xdata unsigned char UART_RX_BUF[UART_REC_LEN];
extern bit UART_RX_STA;  // ������ɱ�־����Ӱ����һ�εĽ��գ�����Ҫ�ֶ�����

void uart_init(unsigned char baud);
void uart_SendByte(unsigned char dat);
void uart_SendString(unsigned char *str);

#endif 

#endif
