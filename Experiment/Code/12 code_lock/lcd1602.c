#include "lcd1602.h"

/*******************************
函数名：read_lcd1602_state
函数功能：检测lcd是否处在繁忙状态
函数形参：void
函数返回值：u8 
备注：不繁忙返回0
*******************************/
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

/*******************************
函数名：lcd1602_write_cmd
函数功能：lcd指令写入
函数形参：u8 cmd
函数返回值：void 
备注：
*******************************/
void lcd1602_write_cmd(unsigned char cmd)  // 写指令
{
	//while(read_lcd1602_state());  // 等待LCD退出繁忙状态
	LCD1602_RS = 0;	 // 命令
	LCD1602_RW = 0;	 // 写
	LCD1602_E = 0;
	LCD1602_DATAPORT = cmd;  // 先把指令送到IO
 	LCD1602_E = 1;
 	delay_ms(1);
	LCD1602_E = 0;
}

/*******************************
函数名：lcd1602_write_data
函数功能：lcd一个字节数据写入
函数形参：u8 dat
函数返回值：void 
备注：
*******************************/
void lcd1602_write_data(unsigned char dat)	// 写数据
{
	//while(read_lcd1602_state());  // 等待LCD退出繁忙状态
	LCD1602_RS = 1;	 // 数据
	LCD1602_RW = 0;	 // 写
	LCD1602_E = 0;
	LCD1602_DATAPORT = dat;  // 先把数据送到IO
 	LCD1602_E = 1;
 	delay_ms(1);
	LCD1602_E = 0;
}

/*******************************
函数名：lcd1602_cursor_pos_set
函数功能：lcd设置光标位置
函数形参：u8 x，y
函数返回值：void 
备注：x为列0~15，y为行0~1
*******************************/
void lcd1602_cursor_pos_set(unsigned char x,unsigned char y)  // 设置光标的位置
{
	unsigned char address;
	//x 代表列，y 代表行，写入地址时要求最高位为1，即加上0x80
	if (y>1 || x>15) return;  // 行列参数不对则强制退出
	else if (y==0) address = 0x80+x;  // 显示在第一行
	else if (y==1) address = 0x80+0x40+x;  // 显示在第二行
	lcd1602_write_cmd(address);
}		   

/*******************************
函数名：lcd1602_init
函数功能：lcd初始化
函数形参：void
函数返回值：void 
备注：数据总线8位，显示2行，5*7点阵/字符显
	  显示功能开，无光标，光标闪烁
	  写入新数据后光标右移，显示屏不移动
	  最后清屏
*******************************/
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);  // 数据总线8位，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c);  // 显示功能开，无光标，光标闪烁
	lcd1602_write_cmd(0x06);  // 写入新数据后光标右移，显示屏不移动
	lcd1602_write_cmd(0x01);  // 清屏	
}

/*******************************
函数名：lcd1602_clear
函数功能：lcd清屏
函数形参：void
函数返回值：void 
备注：
*******************************/
void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

/*******************************
函数名：lcd1602_show_string
函数功能：lcd显示字符串
函数形参：char str[]
函数返回值：void 
备注：
*******************************/
void lcd1602_show_string(char *str)
{
	while(*str != '\0')  // 字符串是以'\0'结尾，只要前面有内容就显示
	{
    	lcd1602_write_data(*str);
		str++;  // 指针递增
	}
}
