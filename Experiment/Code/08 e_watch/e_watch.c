// ���ӱ�
// ���ܣ�ʱ��������������壻����ʱ��ʱ�������
// ʱ�ӡ����塢����ʱ���������ֵ���Դ��棬�˴˶������������ţ���ͬʱ���С�
// ʹ���ĸ�����������K1��K2��K3��K4������K1Ϊ�����л���

// ÿ����һ��K1��ѭ���л�����
// ��ʼ�����ʱ�ӣ�LED D1��D2��D7��D8Ϩ��
// ʱ�������LED D1����ʱ����ͣ������ʱ�䣬K2���л�������ʱ���룻K3���ӣ�K4����
// ���壺LED D2��������ʱ���룬��ʱ��ʱ����3�룬D1,D2����������忪������ʼΪ�رգ�K2���л�������ʱ���룻K3���ӣ�K4������/�ر�����
// ��ʱ����LED D7����������Ҫ����ʱ�ķ֡��롢���룬��ʱ������1�룬D7,D8���������ʱ��������ʼΪ�رգ�K2���л������ĵ�λ��K3���ӣ�K4����ʼ/��ͣ����ʱ
// ���LED D8���𣬼���100���Ӻ�����ѭ����K2����ʼ��K3��������K4������

// �������ʾΪ��ǰ�Ĺ��ܵ�ֵ���ο�����ָʾ��D1,D2,D7,D8��ָʾ
// �����ڿɵ�����ֵ�Ĺ���ʱ�������Ķ�Ӧλ����ֵ��˸

#include <reg51.h>	

unsigned char code led_num[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40};	// 11��ģֵ��0~9, -
unsigned char hour=0, min=0, sec=0;	 // ʱ��
unsigned char alarm_h=0, alarm_m=0, alarm_s=0;	// ����
unsigned char timer_m=0, timer_s=0, timer_ms=0;	 //	��ʱ��
unsigned char stopwatch_m=0, stopwatch_s=0, stopwatch_ms=0;  // ���

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

unsigned char disp_func_flag = 0;  // ��ǰ����
bit flag_clock=0, flag_timer=0, flag_stopwatch=0, flag_button=0, flag_beep=0, beep_flip=0, flag_blink=0;  // �жϱ�־
unsigned char clock_sw = 0; alarm_sw = 0; timer_sw = 0;  // �����Ĳ�ͬ��λ 
bit alarm_beep=0, alarm_en=0, timer_beep=0, timer_en=0, stopwatch_en=0;  // ���������־�����ӿ��أ�����ʱ�����־������ʱ������־�������

void delay(unsigned char z)	 // delay z ms
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

void timer0_config()  //11.0592MHz
{
	TMOD &= 0xf0;
	TMOD |= 0x01;  // ������0������ģʽ1
	TR0 = 1;  // ������0����
	TH0 = 0xdc;   
	TL0 = 0x00;  // 65536-56320=9216, 10ms
	EA = 1;
	ET0 = 1;  // ���ж�
}

void timer1_config()  //11.0592MHz
{
	TMOD &= 0x0f;
	TMOD |= 0x10;  // ������1������ģʽ1
	TR1 = 1;  // ������1����
	TH1 = 0xf8;   
	TL1 = 0xcd;  // 65536-63693=1843, 2ms
	EA = 1;
	ET1 = 1;  // ���ж�
}

void buzzer()  // ����
{
	static unsigned char cnt=0;	 // ��ʱ��

	if (alarm_beep==1 || timer_beep==1)  // �����־
	{
		if (beep_flip==1)  // 2ms�жϱ�־ 
		{
			beep_flip = 0;
			BEEP = ~BEEP;
		}				  	
		if (flag_beep==1)  // ����ʱ����־
		{
			flag_beep = 0;
			cnt++;
			if (cnt==3+1)
			{
				cnt = 0;
				alarm_beep = 0;  // 3�뵽������ֹͣ
				BEEP = 1;	
			}
			if (cnt==1+1)
			{
				cnt = 0;
				timer_beep = 0;  // 1�뵽������ʱ����ֹͣ
				BEEP = 1;	
			}
		}
	}
}

unsigned char key_scan()  // ɨ�谴��
{
	static unsigned char key_flag=0x00;  // �����ڼ�λΪ0��ʾ�ڼ�������û����
	static unsigned char buff[4]={0xff,0xff,0xff,0xff};  // ��������������8�Σ�8�β���ֵ����ͬ����Ϊ�ȶ�
	unsigned char i;

	if (flag_button == 1)  // ÿ2ms���һ��
	{
		flag_button = 0;
		buff[0] = (buff[0]<<1) | k1;
		buff[1] = (buff[1]<<1) | k2;
		buff[2] = (buff[2]<<1) | k3;
		buff[3] = (buff[3]<<1) | k4;		

		for (i=0;i<4;i++)  //ѭ������ĸ�����
		{
			if ( ((key_flag>>i)&0x01)==0x00 && buff[i]==0x00 )
			{
				key_flag |= 0x01<<i;  // ��1����ʾ��������
				return (i+1);
			}
			else if (buff[i]==0xff) key_flag &= ~(0x01<<i);  // ��0����ʾ�����ɿ�
		}
	}
	return 0xff;  // ��ʾ�����ɿ����ߴ��ڲ��ȶ�״̬
} 

void func_change(unsigned char key)  // ���°���K1��
{
	if (key == 1) 
	{
		disp_func_flag++;
		if (disp_func_flag==5) disp_func_flag = 0;

		if (disp_func_flag==0){  // �ı���ʾ���ܵ�LED�� 
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
	}  // key==0xff���ް������»��߰�����δƽ�ȣ��޲���
}

void clock()  // ʱ�Ӽ���
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

void adjust(unsigned char key)  // ʱ�����
{
	if (disp_func_flag == 1)
	{
		if (key == 2)  // �л�������ʱ����
		{
			clock_sw++;
			if (clock_sw==4) clock_sw = 0;
		}
		else if (key == 3)  // ��
		{
			switch(clock_sw)
			{
				case 1: sec++; if (sec==60) sec = 0; break;
				case 2: min++; if (min==60) min = 0; break;
				case 3: hour++; if (hour==24) hour = 0; break;
			}
		}
		else if (key == 4)  // ��
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

void alarm_adjust(unsigned char key)  // ��������
{
	if (disp_func_flag == 2)
	{
		if (key == 2)  // �л�������ʱ����
		{
			alarm_sw++;
			if (alarm_sw==4) alarm_sw = 0;
		}
		else if (key == 3)  // ��
		{
			switch(alarm_sw)
			{
				case 1: alarm_s++; if (alarm_s==60) alarm_s = 0; break;
				case 2: alarm_m++; if (alarm_m==60) alarm_m = 0; break;
				case 3: alarm_h++; if (alarm_h==24) alarm_h = 0; break;
			}
		}
		else if (key == 4) alarm_en = ~alarm_en;  // ���忪��

		if (alarm_en==1)  // ���忪��ָʾ��
		{
			led1 = 0; led2 = 0;
		}else led1 = 1;	
	}
}

void alarm()  // ��������
{
	if (alarm_en==1)
	{
		if (alarm_h==hour && alarm_m==min && alarm_s==sec) alarm_beep = 1;
	}
}

void timer_adjust(unsigned char key)  // ��������ʱ
{
	if (disp_func_flag == 3)
	{
		if (key == 2)  // �л�������ʱ����
		{
			timer_sw++;
			if (timer_sw==4) timer_sw = 0;
		}
		else if (key == 3)  // ��
		{
			switch(timer_sw)
			{
				case 1: timer_ms++; if (timer_ms==100) timer_ms = 0; break;
				case 2: timer_s++; if (timer_s==60) timer_s = 0; break;
				case 3: timer_m++; if (timer_m==100) timer_m = 0; break;
			}
		}
		else if (key == 4) timer_en = ~timer_en;  // ����ʱ����

		if (timer_en==1)  // ����ʱ����ָʾ��
		{
			led3 = 0; led4 = 0;
		}else led4 = 1;	
	}
}

void timer_count()	// ����ʱ����
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
					if (timer_m==0)	 // ʱ�䵽������
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

void stopwatch_config(unsigned char key)  // �������
{
	if (disp_func_flag == 4)
	{
		if (key == 2) stopwatch_en = 1;  // ���ʼ	
		else if (key == 3) stopwatch_en = 0;  // ������
		else if (key == 4)  // �������
		{
			stopwatch_m=0; stopwatch_s=0; stopwatch_ms=0;
		}
	}
}

void stopwatch_count()  // ������������100���Ӻ�����ѭ��
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
	static unsigned char i=0;  // λѡ
	unsigned char H1=0, L1=0, H2=0, L2=0, H3=0, L3=0;  // ��ǰ��ʾ
	static bit j=0;  // ������˸

	if (disp_func_flag==0 || disp_func_flag==1){  // ѡ��ǰ��ʾ��ֵ
		H1 = hour/10;L1 = hour%10; H2 = min/10;L2 = min%10; H3 = sec/10;L3 = sec%10;
	}else if (disp_func_flag==2){
		H1 = alarm_h/10;L1 = alarm_h%10; H2 = alarm_m/10;L2 = alarm_m%10; H3 = alarm_s/10;L3 = alarm_s%10;
	}else if (disp_func_flag==3){
		H1 = timer_m/10;L1 = timer_m%10; H2 = timer_s/10;L2 = timer_s%10; H3 = timer_ms/10;L3 = timer_ms%10;
	}else if (disp_func_flag==4){
		H1 = stopwatch_m/10;L1 = stopwatch_m%10; H2 = stopwatch_s/10;L2 = stopwatch_s%10; H3 = stopwatch_ms/10;L3 = stopwatch_ms%10;
	}

	// ��˸����
	if (flag_blink==1)  // 0.25���ж�
	{
		flag_blink = 0;
		j = ~j;
	}

	if (j == 1)  // jΪ1ʱ��������λ����ʾ��������ʾ��ʵ����˸
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

	switch(i)  // λѡ
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
	P0=0x00;  // ����
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
		// ���ܺ���
		clock();
		buzzer();
		alarm();
		timer_count();
		stopwatch_count();

		// ɨ�谴����ִ����Ӧ����
		key = key_scan();
		func_change(key);
		adjust(key);
		alarm_adjust(key);
		timer_adjust(key);
		stopwatch_config(key);	
		
		// ��ʾ
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
