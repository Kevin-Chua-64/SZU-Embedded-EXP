#include "DS18B20.h"

/*******************************
reset the DS18B20 chip
*******************************/
void ds18b20_reset()
{
	DS18B20_PORT=0;	//拉低DQ
	delay_10us(75);	//拉低750us
	DS18B20_PORT=1;	//DQ=1
	delay_10us(2);	//20us
}

/*******************************
check if DS18B20 chip exist(0) or not(1)
*******************************/
bit ds18b20_check()
{
	unsigned char time_temp=0;

	while(DS18B20_PORT&&time_temp<20)	//等待DQ为低电平
	{
		time_temp++;
		delay_10us(1);	
	}
	if(time_temp>=20)return 1;	//如果超时则强制返回1
	else time_temp=0;
	while((!DS18B20_PORT)&&time_temp<20)	//等待DQ为高电平
	{
		time_temp++;
		delay_10us(1);
	}
	if(time_temp>=20)return 1;	//如果超时则强制返回1
	return 0;
}

/*******************************
read one bit data from DS18B20 chip
*******************************/
bit ds18b20_read_bit()
{
	bit dat=0;
	
	DS18B20_PORT=0;
	_nop_();_nop_();
	DS18B20_PORT=1;	
	_nop_();_nop_(); //该段时间不能过长，必须在15us内读取数据
	if(DS18B20_PORT) dat=1;	//如果总线上为1则数据dat为1，否则为0
	else dat=0;
	delay_10us(5);
	return dat;
} 

/*******************************
read one byte data from DS18B20 chip
*******************************/
unsigned char ds18b20_read_byte()
{
	unsigned char i=0;
	unsigned char dat=0;
	unsigned char temp=0;

	for(i=0;i<8;i++)//循环8次，每次读取一位，且先读低位再读高位
	{
		temp=ds18b20_read_bit();
		dat=(temp<<7)|(dat>>1);
	}
	return dat;	
}

/*******************************
write one data data to DS18B20 chip
*******************************/
void ds18b20_write_byte(unsigned char dat)
{
	unsigned char i=0;
	unsigned char temp=0;

	for(i=0;i<8;i++)//循环8次，每次写一位，且先写低位再写高位
	{
		temp=dat&0x01;//选择低位准备写入
		dat>>=1;//将次高位移到低位
		if(temp)
		{
			DS18B20_PORT=0;
			_nop_();_nop_();
			DS18B20_PORT=1;	
			delay_10us(6);
		}
		else
		{
			DS18B20_PORT=0;
			delay_10us(6);
			DS18B20_PORT=1;
			_nop_();_nop_();	
		}	
	}	
}

/*******************************
prepare for starting
*******************************/
void ds18b20_start()
{
	ds18b20_reset();//复位
	ds18b20_check();//检查DS18B20
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0x44);//转换命令	
}

/*******************************
reset and check 
*******************************/
bit ds18b20_init()
{
	ds18b20_reset();
	return ds18b20_check();	
}

/*******************************
read temperture from DS18B20 chip
it takes a long time to excute and transfer data, so it is better to run this function in 1s interval
type of return value: int, it is 10 times of the real temperature
to avoid interruption affecting read and write bus timing, disable timer0 temporarily 
*******************************/
int ds18b20_read_temperture()
{
	int cur_temp;
	unsigned char dath=0;
	unsigned char datl=0;
	unsigned int value=0;

	TR0 = 0;  // 避免中断影响读写总线时序，暂时不使能定时器0

	ds18b20_start();//开始转换
	ds18b20_reset();//复位
	ds18b20_check();
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0xbe);//读存储器

	datl=ds18b20_read_byte();//低字节
	dath=ds18b20_read_byte();//高字节
	value=(dath<<8)+datl;//合并为16位数据

	if((value&0xf800)==0xf800)//判断符号位，负温度
	{
		value=(~value)+1; //数据取反再加1
		cur_temp=(value*(-5))/8;//乘以精度	
	}
	else //正温度
		cur_temp=(value*5)/8;	

	TR0 = 1;  // 恢复定时器0

	return cur_temp;
}
