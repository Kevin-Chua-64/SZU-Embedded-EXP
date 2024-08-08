#include "infrared.h"

/*******************************
函数名：ired_init
函数功能：红外初始化
函数形参：void
函数返回值：void 
备注：外部中断0的配置
*******************************/
void ired_init()
{
	IT0=1;	//下降沿触发
	EX0=1;	//打开中断0允许
	EA=1;	//打开总中断
	IRED=1;	//初始化端口
}

/*******************************
函数名：ired
函数功能：外部中断0服务函数
函数形参：void
函数返回值：void 
备注：解码并且做出反应
按键键值：
开关：45H      Mode：46H      静音：47H
播放暂停：44H  快退：40H      快进：43H
EQ：07H        VOL-：15H      VOL+：09H
0：16H         RPT：19H       U/SD：0dH
1：0cH         2：18H		  3：5eH
4：08H         5：1cH		  6：5aH
7：42H		   8：52H		  9：4aH
*******************************/
void ired() interrupt 0
{
	unsigned char gired_data[4];  //存储4个字节接收码（地址码+地址反码+控制码+控制反码）
	unsigned char ired_key;       //存储解码后的键值 
	unsigned char ired_high_time=0;
	unsigned int time_cnt=0;
	unsigned char i=0,j=0;	 

	if(IRED==0)
	{
		time_cnt=1000;
		while((!IRED)&&(time_cnt))//等待引导信号9ms低电平结束，若超过10ms强制退出
		{
			delay_10us(1);//延时约10us
			time_cnt--;
			if(time_cnt==0)return;		
		}
		if(IRED)//引导信号9ms低电平已过，进入4.5ms高电平
		{
			time_cnt=500;
			while(IRED&&time_cnt)//等待引导信号4.5ms高电平结束，若超过5ms强制退出
			{
				delay_10us(1);
				time_cnt--;
				if(time_cnt==0)return;	
			}
			for(i=0;i<4;i++)//循环4次，读取4个字节数据
			{
				for(j=0;j<8;j++)//循环8次读取每位数据即一个字节
				{
					time_cnt=600;
					while((IRED==0)&&time_cnt)//等待数据1或0前面的0.56ms结束，若超过6ms强制退出
					{
						delay_10us(1);
						time_cnt--;
						if(time_cnt==0)return;	
					}
					time_cnt=20;
					while(IRED)//等待数据1或0后面的高电平结束，若超过2ms强制退出
					{
						delay_10us(10);//约0.1ms
						ired_high_time++;
						if(ired_high_time>20)return;	
					}
					gired_data[i]>>=1;//先读取的为低位，然后是高位
					if(ired_high_time>=8)//如果高电平时间大于0.8ms，数据则为1，否则为0
						gired_data[i]|=0x80;
					ired_high_time=0;//重新清零，等待下一次计算时间
				}
			}
		}
		if(gired_data[2]!=~gired_data[3])//校验控制码与反码，错误则返回
		{
			for(i=0;i<4;i++)
				gired_data[i]=0;
			return;	
		}
	}

	// 解码
	ired_key = gired_data[2]/16;  //控制码高4位
	ired_key <<= 4;
	ired_key |= gired_data[2]%16;  //控制码低4位

	// 以下进行红外解码后的操作		

}

