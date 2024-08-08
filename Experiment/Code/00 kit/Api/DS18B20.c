#include "DS18B20.h"

/*******************************
函数名：ds18b20_reset
函数功能：复位
函数形参：void
函数返回值：void 
备注：
*******************************/
void ds18b20_reset()
{
	DS18B20_PORT=0;	//拉低DQ
	delay_10us(75);	//拉低750us
	DS18B20_PORT=1;	//DQ=1
	delay_10us(2);	//20us
}

/*******************************
函数名：ds18b20_check
函数功能：检测DS18B20是否存在
函数形参：void
函数返回值：bit
备注：1:未检测到DS18B20的存在，0:存在
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
函数名：ds18b20_read_bit
函数功能：读取一个bit
函数形参：void
函数返回值：bit 
备注：
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
函数名：ds18b20_read_byte
函数功能：读取一个byte
函数形参：void
函数返回值：unsigned char 
备注：
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
函数名：ds18b20_write_byte
函数功能：写入一个byte
函数形参：u8 dat
函数返回值：void
备注：
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
函数名：ds18b20_start
函数功能：温度转换
函数形参：void
函数返回值：void
备注：
*******************************/
void ds18b20_start()
{
	ds18b20_reset();//复位
	ds18b20_check();//检查DS18B20
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0x44);//转换命令	
}

/*******************************
函数名：ds18b20_init
函数功能：初始化DS18B20并检测是否存在
函数形参：void
函数返回值：bit
备注：1:未检测到DS18B20的存在，0:存在
*******************************/
bit ds18b20_init()
{
	ds18b20_reset();
	return ds18b20_check();	
}

/*******************************
函数名：ds18b20_read_temperture
函数功能：获取温度值
函数形参：void
函数返回值：float
备注：获取值为浮点数，运行一次时间较久，因此间隔一段时间读取(1s)
*******************************/
float ds18b20_read_temperture()
{
	xdata float temp;
	xdata unsigned char dath=0;
	xdata unsigned char datl=0;
	xdata unsigned int value=0;

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
		temp=value*(-0.0625);//乘以精度	
	}
	else //正温度
	{
		temp=value*0.0625;	
	}
	return temp;
}
