#include "lcd1602.h"

unsigned char read_lcd1602_state()  //读单片机是否为繁忙的状态
{
	unsigned char tmp, state=0;
	LCD1602_RS = 0;	 // 命令
	LCD1602_RW = 1;	 // 读
	LCD1602_E = 1;
	tmp = LCD1602_DATAPORT;
	state = (tmp&0x80);	 // 只看最高位
	LCD1602_E = 0;
	return state;  // 为0则为不繁忙
}

void lcd1602_write_cmd(unsigned char cmd)  // 写指令
{
	while(read_lcd1602_state());  // 等待LCD退出繁忙状态
	LCD1602_RS = 0;	 // 命令
	LCD1602_RW = 0;	 // 写
	LCD1602_E = 0;
	LCD1602_DATAPORT = cmd;  // 先把指令送到IO
 	LCD1602_E = 1;
 	delay_ms(1);
	LCD1602_E = 0;
}

void lcd1602_write_data(unsigned char dat)	// 写数据
{
	while(read_lcd1602_state());  // 等待LCD退出繁忙状态
	LCD1602_RS = 1;	 // 数据
	LCD1602_RW = 0;	 // 写
	LCD1602_E = 0;
	LCD1602_DATAPORT = dat;  // 先把数据送到IO
 	LCD1602_E = 1;
 	delay_ms(1);
	LCD1602_E = 0;
}

void cursor_pos_set(unsigned char x,unsigned char y)  // 设置光标的位置
{
	unsigned char address;
	//x 代表列，y 代表行，写入地址时要求最高位为1，即加上0x80
	if (y>1 || x>15) return;  // 行列参数不对则强制退出
	else if (y==0) address = 0x80+x;  // 显示在第一行
	else if (y==1) address = 0x80+0x40+x;  // 显示在第二行
	lcd1602_write_cmd(address);
}		   

void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);  // 数据总线8位，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c);  // 显示功能开，无光标，光标闪烁
	lcd1602_write_cmd(0x06);  // 写入新数据后光标右移，显示屏不移动
	lcd1602_write_cmd(0x01);  // 清屏	
}

void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

void lcd1602_show_string(char *str)
{
	while(*str != '\0')  // 字符串是以'\0'结尾，只要前面有内容就显示
	{
    	lcd1602_write_data(*str);
		str++;  // 指针递增
	}
}
