#include "infrared.h"

void ired_init()
{
	IT0=1;	//下降沿触发
	EX0=1;	//打开中断0允许
	EA=1;	//打开总中断
	IRED=1;	//初始化端口
}

/*******************************
the external interrupt
decode the NEC data and action
key value:
开关：45H      Mode：46H      静音：47H
播放暂停：44H  快退：40H      快进：43H
EQ：07H        VOL-：15H      VOL+：09H
0：16H         RPT：19H       U/SD：0dH
1：0cH         2：18H		  3：5eH
4：08H         5：1cH		  6：5aH
7：42H		   8：52H		  9：4aH
*******************************/
bit flag_infrared_1s=0;
void ired() interrupt 0
{
	unsigned char gired_data[4];  //存储4个字节接收码（地址码+地址反码+控制码+控制反码）
	unsigned char ired_key;       //存储解码后的键值 
	unsigned char ired_high_time=0;
	unsigned int time_cnt=0;
	unsigned char i=0,j=0;	 
	static unsigned char pre_key;

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
	//1：0cH         2：18H		  3：5eH
	//4：08H         5：1cH		  6：5aH
	if (flag_infrared_1s == 1)
	{
		flag_infrared_1s = 0;
		if (ired_key==0x0c)	 // 模式：关、标准、睡眠风、温控
		{
			fan_mode = (fan_mode+1)%4;	
			if (fan_mode==0)  // 关机时复位
			{
				rotate_speed = 0;  // 不摇头
				fan_speed = 1;  // 速度为1档
				timer_mode = 0;	 // 定时关闭功能关
				timer_min = 0;
				timer_sec = 0;  // 定时清空
			} else if (fan_mode==3)	 // 温控开始时记录当时温度和占空比
			{
				start_temp = temp;
				start_duty = g_pwm_duty;	
			}	
		} else if (ired_key==0x18 && fan_mode!=0)	// 左右摇头
			rotate_speed = (rotate_speed+1)%3;	
		else if (ired_key==0x5e && fan_mode==1)  // 风速
			fan_speed = (fan_speed%3)+1;	
		else if (ired_key==0x08 && fan_mode!=0)  // 定时器倒计时关机
		{
			timer_mode = (timer_mode+1)%4;
			if (timer_mode==0)	// 关闭定时
			{
				timer_min = 0;
				timer_sec = 0;  // 定时清空	
			}
		}	
		else if (ired_key==0x1c && pre_key!=0x1c && fan_mode!=0)  // 定时器设置时间，单击
		{
			if (timer_mode==1)
				timer_sec = (timer_sec+1)%60;
			else if (timer_mode==2)
				timer_min = (timer_min+1)%100;	
		} else if (ired_key==0x1c && pre_key==0x1c && fan_mode!=0)  // 定时器设置时间，长按
		{
			if (timer_mode==1)
				timer_sec = (timer_sec+5)%60;
			else if (timer_mode==2)
				timer_min = (timer_min+1)%100;	
		} else if (ired_key==0x5a)	// 切换显示温度或风扇状态
			dis_temp_state = ~dis_temp_state;
			
		pre_key = ired_key;	
	}
}
