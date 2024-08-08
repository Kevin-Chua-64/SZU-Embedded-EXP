#include "uart.h"

#if USE_UART_TIMER1
/*******************************
函数名：uart_init
函数功能：串口初始化
函数形参：u8 baud
函数返回值：void 
备注：RX->P30  TX->P31
	  占用计时器1
*******************************/
void uart_init(unsigned char baud)	//baud: 0xfa->9600 0xfd->19200 0xff->57600
{
	TMOD|=0X20;	  //设置计数器1工作方式2
	SCON=0X50;	  //设置串口工作方式1
	PCON=0X80;	  //波特率加倍
	TH1=baud;	  //计数器初始值设置
	TL1=baud;
	ES=1;		  //打开接收中断
	EA=1;	   	  //打开总中断
	TR1=1;	 	  //打开计数器	
}

/*******************************
函数名：uart_SendByte
函数功能：从串口发送一个字节
函数形参：u8 dat
函数返回值：void 
备注：
*******************************/
void uart_SendByte(unsigned char dat)
{
	ES=0;         //关闭串口中断
	TI=0;         //清发送完毕中断请求标志位
	SBUF = dat;   //发送
	while(TI==0); //等待发送完毕
	TI=0;         //清发送完毕中断请求标志位
	ES=1;         //允许串口中断
}

/*******************************
函数名：uart_SendString
函数功能：从串口发送字符串
函数形参：u8 str[]
函数返回值：void 
备注：
*******************************/
void uart_SendString(unsigned char *str)
{
	while(*str != '\0') //字符串结束标志
	{
		uart_SendByte(*str);
		delay_10us(20);	// 延时，等待字节发送完
		str++;	
	}
}

xdata unsigned char UART_RX_BUF[UART_REC_LEN];  //接收缓冲
bit UART_RX_STA = 0;
/*******************************
函数名：uart_IRQn
函数功能：串口接收中断服务函数
函数形参：void
函数返回值：void 
备注：接收一个字符串
*******************************/
void uart_IRQn() interrupt 4
{
	static unsigned char i=0;
	
	if (RI)
	{
		if (i==0)
			memset(UART_RX_BUF, 0, sizeof UART_RX_BUF);  //清空数组          
		RI=0;  // 清标志
		UART_RX_BUF[i++] = SBUF;  // 读取接收到的数据存入全局变量的缓冲
		if (UART_RX_BUF[i-2]=='\r' && UART_RX_BUF[i-1]=='\n')  //检测到回车结束标志，然后软件删标志
		{
			UART_RX_BUF[i-2] = '\0';  // 手动添加'\0'
		 	UART_RX_BUF[i-1] = 0;
			i = 0;
			UART_RX_STA = 1;
		}
		if (i>UART_REC_LEN) i=0;  // 超出最大字节数，舍弃
	}		
}

//输出重定向到串口
char putchar(char c)
{
	uart_SendByte(c);
	return c;  //返回给函数的调用者printf
}

#endif
