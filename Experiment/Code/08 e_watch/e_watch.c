// 电子表
// 功能：时间调整；设置闹铃；倒计时定时器；秒表。
// 时钟、闹铃、倒计时、秒表功能数值独自储存，彼此独立，互不干扰，可同时进行。
// 使用四个独立按键：K1，K2，K3，K4。其中K1为功能切换键

// 每按下一次K1，循环切换功能
// 初始情况：时钟，LED D1，D2，D7，D8熄灭
// 时间调整：LED D1亮起，时钟暂停，调整时间，K2：切换调整的时分秒；K3：加；K4：减
// 闹铃：LED D2亮起，设置时分秒，到时间时响铃3秒，D1,D2亮起代表闹铃开启，初始为关闭，K2：切换调整的时分秒；K3：加；K4：开启/关闭闹铃
// 定时器：LED D7亮起，设置需要倒计时的分、秒、毫秒，到时间响铃1秒，D7,D8亮起代表倒计时开启，初始为关闭，K2：切换调整的单位；K3：加；K4：开始/暂停倒计时
// 秒表：LED D8亮起，计满100分钟后清零循环，K2：开始；K3：结束；K4：清零

// 数码管显示为当前的功能的值，参考功能指示灯D1,D2,D7,D8的指示
// 当处在可调整数值的功能时，调整的对应位置数值闪烁

#include <reg51.h>	

unsigned char code led_num[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40};	// 11个模值：0~9, -
unsigned char hour=0, min=0, sec=0;	 // 时钟
unsigned char alarm_h=0, alarm_m=0, alarm_s=0;	// 闹铃
unsigned char timer_m=0, timer_s=0, timer_ms=0;	 //	定时器
unsigned char stopwatch_m=0, stopwatch_s=0, stopwatch_ms=0;  // 秒表

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit BEEP = P2^5;
								 											
sbit k1 = P3^1;
sbit k2 = P3^0;
sbit k3 = P3^2;
sbit k4 = P3^3;

sbit led1 = P2^0;
sbit led2 = P2^1;
sbit led3 = P2^6;
sbit led4 = P2^7;

unsigned char disp_func_flag = 0;  // 当前功能
bit flag_clock=0, flag_timer=0, flag_stopwatch=0, flag_button=0, flag_beep=0, beep_flip=0, flag_blink=0;  // 中断标志
unsigned char clock_sw = 0; alarm_sw = 0; timer_sw = 0;  // 调整的不同的位 
bit alarm_beep=0, alarm_en=0, timer_beep=0, timer_en=0, stopwatch_en=0;  // 闹钟响铃标志，闹钟开关，倒计时响铃标志，倒计时开启标志，秒表开关

void delay(unsigned char z)	 // delay z ms
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // 计数器0工作在模式1
	TR0 = 1;  // 计数器0工作
	TH0 = 0xdc;   
	TL0 = 0x00;  // 65536-56320=9216, 10ms
	EA = 1;
	ET0 = 1;  // 打开中断
}

void timer1_config()  //11.0592MHz
{
	TMOD &= 0x0f;
	TMOD |= 0x10;  // 计数器1工作在模式1
	TR1 = 1;  // 计数器1工作
	TH1 = 0xf8;   
	TL1 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET1 = 1;  // 打开中断
}

void buzzer()  // 响铃
{
	static unsigned char cnt=0;	 // 计时用

	if (alarm_beep==1 || timer_beep==1)  // 响铃标志
	{
		if (beep_flip==1)  // 2ms中断标志 
		{
			beep_flip = 0;
			BEEP = ~BEEP;
		}				  	
		if (flag_beep==1)  // 响铃时长标志
		{
			flag_beep = 0;
			cnt++;
			if (cnt==3+1)
			{
				cnt = 0;
				alarm_beep = 0;  // 3秒到，闹铃停止
				BEEP = 1;	
			}
			if (cnt==1+1)
			{
				cnt = 0;
				timer_beep = 0;  // 1秒到，倒计时响铃停止
				BEEP = 1;	
			}
		}
	}
}

unsigned char key_scan()  // 扫描按键
{
	static unsigned char key_flag=0x00;  // 倒数第几位为0表示第几个按键没按下
	static unsigned char buff[4]={0xff,0xff,0xff,0xff};  // 消抖，连续采样8次，8次采样值均相同才视为稳定
	unsigned char i;

	if (flag_button == 1)  // 每2ms检测一次
	{
		flag_button = 0;
		buff[0] = (buff[0]<<1) | k1;
		buff[1] = (buff[1]<<1) | k2;
		buff[2] = (buff[2]<<1) | k3;
		buff[3] = (buff[3]<<1) | k4;		

		for (i=0;i<4;i++)  //循环检测四个按键
		{
			if ( ((key_flag>>i)&0x01)==0x00 && buff[i]==0x00 )
			{
				key_flag |= 0x01<<i;  // 置1，表示按键按下
				return (i+1);
			}
			else if (buff[i]==0xff) key_flag &= ~(0x01<<i);  // 置0，表示按键松开
		}
	}
	return 0xff;  // 表示按键松开或者处于不稳定状态
} 

void func_change(unsigned char key)  // 按下按键K1后
{
	if (key == 1) 
	{
		disp_func_flag++;
		if (disp_func_flag==5) disp_func_flag = 0;

		if (disp_func_flag==0){  // 改变显示功能的LED灯 
			led1 = 1; led2 = 1; led3 = 1; led4 = 1;
		}else if (disp_func_flag==1){
			led1 = 0; led2 = 1; led3 = 1; led4 = 1;
		}else if (disp_func_flag==2){
			led1 = 1; led2 = 0; led3 = 1; led4 = 1;
		}else if (disp_func_flag==3){
			led1 = 1; led2 = 1; led3 = 0; led4 = 1;
		}else if (disp_func_flag==4){
			led1 = 1; led2 = 1; led3 = 1; led4 = 0;
		}
	}  // key==0xff，无按键按下或者按键尚未平稳，无操作
}

void clock()  // 时钟计数
{
	if (disp_func_flag != 1)
	{
		if (flag_clock==1)
		{
			flag_clock = 0;
			sec++;
			if (sec==60)
			{
				sec = 0;
				min++;
				if (min==60)
				{
					min = 0;
					hour++;
					if (hour==24) hour = 0;	
				}
			}
		}	
	}	
}

void adjust(unsigned char key)  // 时间调整
{
	if (disp_func_flag == 1)
	{
		if (key == 2)  // 切换调整的时分秒
		{
			clock_sw++;
			if (clock_sw==4) clock_sw = 0;
		}
		else if (key == 3)  // 加
		{
			switch(clock_sw)
			{
				case 1: sec++; if (sec==60) sec = 0; break;
				case 2: min++; if (min==60) min = 0; break;
				case 3: hour++; if (hour==24) hour = 0; break;
			}
		}
		else if (key == 4)  // 减
		{
			switch(clock_sw)
			{
				case 1: sec--; if (sec==0xff) sec = 59; break;
				case 2: min--; if (min==0xff) min = 59; break;
				case 3: hour--; if (hour==0xff) hour = 23; break;
			}
		}	
	}	
}

void alarm_adjust(unsigned char key)  // 调整闹铃
{
	if (disp_func_flag == 2)
	{
		if (key == 2)  // 切换调整的时分秒
		{
			alarm_sw++;
			if (alarm_sw==4) alarm_sw = 0;
		}
		else if (key == 3)  // 加
		{
			switch(alarm_sw)
			{
				case 1: alarm_s++; if (alarm_s==60) alarm_s = 0; break;
				case 2: alarm_m++; if (alarm_m==60) alarm_m = 0; break;
				case 3: alarm_h++; if (alarm_h==24) alarm_h = 0; break;
			}
		}
		else if (key == 4) alarm_en = ~alarm_en;  // 闹铃开关

		if (alarm_en==1)  // 闹铃开关指示灯
		{
			led1 = 0; led2 = 0;
		}else led1 = 1;	
	}
}

void alarm()  // 闹铃响铃
{
	if (alarm_en==1)
	{
		if (alarm_h==hour && alarm_m==min && alarm_s==sec) alarm_beep = 1;
	}
}

void timer_adjust(unsigned char key)  // 调整倒计时
{
	if (disp_func_flag == 3)
	{
		if (key == 2)  // 切换调整的时分秒
		{
			timer_sw++;
			if (timer_sw==4) timer_sw = 0;
		}
		else if (key == 3)  // 加
		{
			switch(timer_sw)
			{
				case 1: timer_ms++; if (timer_ms==100) timer_ms = 0; break;
				case 2: timer_s++; if (timer_s==60) timer_s = 0; break;
				case 3: timer_m++; if (timer_m==100) timer_m = 0; break;
			}
		}
		else if (key == 4) timer_en = ~timer_en;  // 倒计时开关

		if (timer_en==1)  // 倒计时开关指示灯
		{
			led3 = 0; led4 = 0;
		}else led4 = 1;	
	}
}

void timer_count()	// 倒计时计数
{
	if (timer_en == 1)
	{
		if (flag_timer==1)
		{
			flag_timer = 0;
			if (timer_ms==0)
			{
				if (timer_s==0)
				{
					if (timer_m==0)	 // 时间到，响铃
					{
						timer_beep = 1;
						timer_en = 0;
					}else
					{
						timer_m--;
						timer_s = 59;
						timer_ms = 99;
					}
				}else
				{
					timer_s--;
					timer_ms = 99;
				}
			}else timer_ms--;
		}	
	}	
}

void stopwatch_config(unsigned char key)  // 秒表配置
{
	if (disp_func_flag == 4)
	{
		if (key == 2) stopwatch_en = 1;  // 秒表开始	
		else if (key == 3) stopwatch_en = 0;  // 秒表结束
		else if (key == 4)  // 秒表清零
		{
			stopwatch_m=0; stopwatch_s=0; stopwatch_ms=0;
		}
	}
}

void stopwatch_count()  // 秒表计数，计满100分钟后清零循环
{
	if (stopwatch_en == 1)
	{
		if (flag_stopwatch==1)
		{
			flag_stopwatch = 0;
			stopwatch_ms++;
			if (stopwatch_ms==100)
			{
				stopwatch_ms = 0;
				stopwatch_s++;
				if (stopwatch_s==60)
				{
					stopwatch_s = 0;
					stopwatch_m++;
					if (stopwatch_m==100) stopwatch_m = 0;	
				}
			}
		}	
	}	
}

void display()
{
	static unsigned char i=0;  // 位选
	unsigned char H1=0, L1=0, H2=0, L2=0, H3=0, L3=0;  // 当前显示
	static bit j=0;  // 控制闪烁

	if (disp_func_flag==0 || disp_func_flag==1){  // 选择当前显示的值
		H1 = hour/10;L1 = hour%10; H2 = min/10;L2 = min%10; H3 = sec/10;L3 = sec%10;
	}else if (disp_func_flag==2){
		H1 = alarm_h/10;L1 = alarm_h%10; H2 = alarm_m/10;L2 = alarm_m%10; H3 = alarm_s/10;L3 = alarm_s%10;
	}else if (disp_func_flag==3){
		H1 = timer_m/10;L1 = timer_m%10; H2 = timer_s/10;L2 = timer_s%10; H3 = timer_ms/10;L3 = timer_ms%10;
	}else if (disp_func_flag==4){
		H1 = stopwatch_m/10;L1 = stopwatch_m%10; H2 = stopwatch_s/10;L2 = stopwatch_s%10; H3 = stopwatch_ms/10;L3 = stopwatch_ms%10;
	}

	// 闪烁控制
	if (flag_blink==1)  // 0.25秒中断
	{
		flag_blink = 0;
		j = ~j;
	}

	if (j == 1)  // j为1时，跳过该位的显示，即不显示，实现闪烁
	{
		if (disp_func_flag == 1)
		{
			if (clock_sw == 1){
				if (i==6 || i==7) i = 0;
			}else if (clock_sw == 2){
				if (i==3 || i==4) i = 5;
			}else if (clock_sw == 3){
				if (i==0 || i==1) i = 2;
			}
		}
		else if (disp_func_flag == 2)
		{
			if (alarm_sw == 1){
				if (i==6 || i==7) i = 0;
			}else if (alarm_sw == 2){
				if (i==3 || i==4) i = 5;
			}else if (alarm_sw == 3){
				if (i==0 || i==1) i = 2;
			}
		}
		else if (disp_func_flag == 3)
		{
			if (timer_sw == 1){
				if (i==6 || i==7) i = 0;
			}else if (timer_sw == 2){
				if (i==3 || i==4) i = 5;
			}else if (timer_sw == 3){
				if (i==0 || i==1) i = 2;
			}
		}		
	}

	switch(i)  // 位选
	{
		case 0: LSC=1;LSB=1;LSA=1;P0 = led_num[H1]; break;
		case 1: LSC=1;LSB=1;LSA=0;P0 = led_num[L1]; break;
		case 2: LSC=1;LSB=0;LSA=1;P0 = led_num[10]; break;
		case 3: LSC=1;LSB=0;LSA=0;P0 = led_num[H2]; break;
		case 4: LSC=0;LSB=1;LSA=1;P0 = led_num[L2]; break;
		case 5: LSC=0;LSB=1;LSA=0;P0 = led_num[10]; break;
		case 6: LSC=0;LSB=0;LSA=1;P0 = led_num[H3]; break;
		case 7: LSC=0;LSB=0;LSA=0;P0 = led_num[L3]; break;
	}

	delay(2);
	P0=0x00;  // 消音
	i++;
	if (i==8) i=0;
}


void main()
{
	unsigned char key;

	timer0_config();
	timer1_config();
	while(1)
	{
		// 功能函数
		clock();
		buzzer();
		alarm();
		timer_count();
		stopwatch_count();

		// 扫描按键并执行相应操作
		key = key_scan();
		func_change(key);
		adjust(key);
		alarm_adjust(key);
		timer_adjust(key);
		stopwatch_config(key);	
		
		// 显示
		display();
	}
}

void timer0_interrupt() interrupt 1
{
	static unsigned char cnt=0;
	
	cnt++;
	TH0 = 0xdc;   
	TL0 = 0x00;
	flag_timer = 1; flag_stopwatch = 1;	 // 10ms
	if (cnt==100)
	{
		cnt = 0;
		flag_clock = 1;	
		flag_beep = 1;	// 1s
	}
}

void timer1_interrupt() interrupt 3
{	
	static unsigned char cnt=0;

	cnt++;
	TH1 = 0xf8;   
	TL1 = 0xcd;
	flag_button = 1;
	beep_flip = 1;  // 2ms
	if (cnt==125)
	{
		cnt = 0;
		flag_blink = 1;	// 0.25s
	}
}
