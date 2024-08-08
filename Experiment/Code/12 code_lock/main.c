/********************************
密码锁
功能：
开锁、关锁时lcd有流水灯加载显示
设有管理员模式，进入管理员模式时，可跳过设置的密码，直接用管理员密码解锁
开锁时，显示密码锁自初始化以来被打开的总次数
********************************/
#include <reg51.h>
#include <stdio.h>
#include "delay.h"
#include "matrix_key.h"
#include "lcd1602.h"

unsigned char password[]={' ',' ',' ',' ',' ',' ','\0'};
unsigned char lockword[]={'*','*','*','*','*','*','\0'};
unsigned char adminword[]="123456";
bit state=0; // open
bit admin=0;
unsigned char wrong=0;
unsigned char total=0;  // 总开锁次数
sbit led = P2^0;

bit compare(unsigned char arr1[], unsigned char arr2[])  // 字符串前6个字符比较
{
	unsigned char i;
	for(i = 0; i < 6; i++)
	{
		if (arr1[i] != arr2[i]) return 0; // wrong
		i++;
	}
	return 1; // right
}

void word_init(bit correct)	 // 重置密码
{
	unsigned char i;
	for(i = 0; i < 6; i++)
	{	
		if (correct) password[i] = ' ';
		lockword[i] = '*';
	}
}

void loading()	// 加载loading…………
{
	unsigned char i;
	unsigned char str[]="  --------------||--------------||";
	lcd1602_clear();
	lcd1602_cursor_pos_set(0,0);
	for (i=0;i<34;i++)
	{
		if (i==17)
		{
			lcd1602_cursor_pos_set(15,1);
			lcd1602_write_cmd(0x04);// 写入新数据后光标左移，显示屏不移动;
		}
		if (i==33)
		{
			lcd1602_cursor_pos_set(0,0);
			lcd1602_write_cmd(0x06);// 写入新数据后光标右移，显示屏不移动;
		}
		lcd1602_write_data(str[i]);
		delay_ms(100);
	}
	delay_ms(255);
	lcd1602_clear();
}

void key_action(unsigned char key)
{
	static unsigned char i=0;
	if (key>=0 && key<=9)
	{
		if (state == 0)
		{
			if (i<6)
			{
				password[i] = key+'0';
				i++;
			}
		}
		else if (state==1 || admin==1)
		{
			if (i<6)
			{
				lockword[i] = key+'0';
				i++;
			}
		}
	}
	else if (key==10 && state==0) // lock
	{
		i = 0;
		state = 1;
		loading();
	}
	else if (key==11 && state==1) // unlock
	{
		i = 0;
		if ( (admin==0 && compare(password, lockword)) || (admin==1 && compare(adminword, lockword)) )
		{
			state = 0;
			admin = 0;
			wrong = 0;
			total++;
			word_init(1);
			loading();			
		}
		else // 密码错误
		{
			word_init(0);
			wrong++;	
		}
	}
	else if (key==12 && state==1)  // 管理员密码强行开锁模式
	{
		i = 0;
		admin = ~admin;
		lcd1602_clear();
		word_init(0);
	}
}

void display()
{
	if (state==0)
	{
		lcd1602_cursor_pos_set(0,0);
		lcd1602_show_string(password);
		lcd1602_cursor_pos_set(0,1);
		lcd1602_show_string("Open  Total:");
		lcd1602_write_data(total/100+'0');
		lcd1602_write_data(total/10%10+'0');
		lcd1602_write_data(total%10+'0');
	}
	else if (state==1)
	{
		lcd1602_cursor_pos_set(0,0);
		lcd1602_show_string(lockword);
		lcd1602_cursor_pos_set(0,1);
		if (admin==0) lcd1602_show_string("Lock");
		else if (admin==1) lcd1602_show_string("Admin");
	}
}

void main()
{
	unsigned char key;

	lcd1602_init();

	while(1)
	{
		display();	
		key = matrix_key_scan();
		key_action(key);

		if (wrong>=3) led = 0;
		else led = 1;
	}
}
