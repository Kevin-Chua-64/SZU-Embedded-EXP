#ifndef _WIFI_H
#define _WIFI_H

#include <reg52.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "global.h"
#include "pwm.h"

#define WIFI_REC_LEN 15  //接收最大字节数

// 指令参数
#define STATION_MODE "AT+CWMODE_CUR=1\r\n"
#define CONNECT_WIFI "AT+CWJAP_CUR=\"hhh\",\"babababa\"\r\n"
#define SINGLE_CONNECTED "AT+CIPMUX=0\r\n"
#define SERIANET_MODE "AT+CIPMODE=1\r\n"
#define CONNECT_SERVER "AT+CIPSTART=\"TCP\",\"192.168.137.1\",9999\r\n"
#define ENTER_SERIANET "AT+CIPSEND\r\n"
#define EXIT_SERIANET "+++"

extern xdata unsigned char WIFI_RX_BUF[WIFI_REC_LEN];
extern bit WIFI_RX_STA; 

void wifi_SendString(unsigned char *str);
unsigned char Wifi_Init();
void Wifi_GetTime();
void Wifi_Analysis_Response();

#endif
