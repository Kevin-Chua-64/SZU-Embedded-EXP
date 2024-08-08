#ifndef _DS18B20_H
#define _DS18B20_H

#include <reg52.h>
#include "intrins.h"
#include "delay.h"

sbit DS18B20_PORT=P3^7;	//DS18B20数据口定义

bit ds18b20_init();
int ds18b20_read_temperture();

#endif
