#include "DS18B20.h"

/*******************************
reset the DS18B20 chip
*******************************/
void ds18b20_reset()
{
	DS18B20_PORT=0;	//����DQ
	delay_10us(75);	//����750us
	DS18B20_PORT=1;	//DQ=1
	delay_10us(2);	//20us
}

/*******************************
check if DS18B20 chip exist(0) or not(1)
*******************************/
bit ds18b20_check()
{
	unsigned char time_temp=0;

	while(DS18B20_PORT&&time_temp<20)	//�ȴ�DQΪ�͵�ƽ
	{
		time_temp++;
		delay_10us(1);	
	}
	if(time_temp>=20)return 1;	//�����ʱ��ǿ�Ʒ���1
	else time_temp=0;
	while((!DS18B20_PORT)&&time_temp<20)	//�ȴ�DQΪ�ߵ�ƽ
	{
		time_temp++;
		delay_10us(1);
	}
	if(time_temp>=20)return 1;	//�����ʱ��ǿ�Ʒ���1
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
	_nop_();_nop_(); //�ö�ʱ�䲻�ܹ�����������15us�ڶ�ȡ����
	if(DS18B20_PORT) dat=1;	//���������Ϊ1������datΪ1������Ϊ0
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

	for(i=0;i<8;i++)//ѭ��8�Σ�ÿ�ζ�ȡһλ�����ȶ���λ�ٶ���λ
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

	for(i=0;i<8;i++)//ѭ��8�Σ�ÿ��дһλ������д��λ��д��λ
	{
		temp=dat&0x01;//ѡ���λ׼��д��
		dat>>=1;//���θ�λ�Ƶ���λ
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
	ds18b20_reset();//��λ
	ds18b20_check();//���DS18B20
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0x44);//ת������	
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

	TR0 = 0;  // �����ж�Ӱ���д����ʱ����ʱ��ʹ�ܶ�ʱ��0

	ds18b20_start();//��ʼת��
	ds18b20_reset();//��λ
	ds18b20_check();
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0xbe);//���洢��

	datl=ds18b20_read_byte();//���ֽ�
	dath=ds18b20_read_byte();//���ֽ�
	value=(dath<<8)+datl;//�ϲ�Ϊ16λ����

	if((value&0xf800)==0xf800)//�жϷ���λ�����¶�
	{
		value=(~value)+1; //����ȡ���ټ�1
		cur_temp=(value*(-5))/8;//���Ծ���	
	}
	else //���¶�
		cur_temp=(value*5)/8;	

	TR0 = 1;  // �ָ���ʱ��0

	return cur_temp;
}
