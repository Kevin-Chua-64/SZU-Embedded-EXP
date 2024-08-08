#include "wifi.h"

xdata unsigned char WIFI_RX_BUF[WIFI_REC_LEN];
bit WIFI_RX_STA=0;  // 接收完成标志，不影响下一次的接收，但需要手动清零
/*******************************
initialize the uart for wifi
input parameter baud rate
RX->P30  TX->P31
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

void wifi_SendByte(unsigned char dat)
{
	ES=0;         //关闭串口中断
	TI=0;         //清发送完毕中断请求标志位
	SBUF = dat;   //发送
	while(TI==0); //等待发送完毕
	TI=0;         //清发送完毕中断请求标志位
	ES=1;         //允许串口中断
}

void wifi_SendString(unsigned char *str)
{
	TR0 = 0;   //避免中断影响读写总线时序，暂时不使能定时器0
	while(*str != '\0') //字符串结束标志
	{
		wifi_SendByte(*str);
		delay_10us(10);	// 延时，等待字节发送完
		str++;	
	}
	TR0 = 1;
}

/*******************************
uart interrupt
receive a string from wifi through uart
*******************************/
void wifi_IRQn() interrupt 4
{
	static unsigned char i=0;
	
	if (RI)
	{
		if (i==0)
			memset(WIFI_RX_BUF, 0, sizeof WIFI_RX_BUF);  //清空数组          
		RI=0;  // 清标志
		WIFI_RX_BUF[i++] = SBUF;  // 读取接收到的数据存入wifi缓冲
		if (WIFI_RX_BUF[i-2]=='\r' && WIFI_RX_BUF[i-1]=='\n')  //检测到回车结束标志，然后软件删标志
		{
			WIFI_RX_BUF[i-2] = '\0';  // 手动添加'\0'
		 	WIFI_RX_BUF[i-1] = 0;
			i = 0;
			WIFI_RX_STA = 1;
		}
		if (i>WIFI_REC_LEN) i=0;  // 超出最大字节数，舍弃
	}		
}

/*******************************
send AT instruction in ESP8266
备注：第一个形参是指令，第二个是期待的回复，也就是正常的AT相应，如"AT+CWMODE_CUR=1","OK"
	  返回值0：正常，1：等待正确响应超时
*******************************/
bit Wifi_SendAT(char *sAT, char *expectATRes)
{
	unsigned int timeCnt=0, timeOut=1000;	// 超时计数，超时上限
	
	//wifi连接时间较长，增加上限
	if(strstr(sAT, "AT+CWJAP_CUR=\"") != NULL) timeOut = 8000;

	wifi_SendString(sAT);  //发送AT指令
	//等待接收正确指令
	while(1)
	{
		while(WIFI_RX_STA != 1)  //等待接收完成标志
		{
			timeCnt++;
			delay_ms(1);
			
			if(timeCnt > timeOut)
				return 1;  //超时
		}
		WIFI_RX_STA = 0;  //清除接收标志
		
		//查询接收的内容中是否有期望回显的内容
		if(strstr(WIFI_RX_BUF, expectATRes) != NULL)
			break;
	}
	return 0;
}

/*******************************
connect to wifi
备注：返回值0：正常，1：无法设置station模式，2：无法连接wifi
*******************************/
unsigned char Wifi_Connect()
{
	bit ret;
	
	//设置station模式
	ret = Wifi_SendAT(STATION_MODE, "OK");
	if(ret) return 1;  // 无法设置station模式
	
	//连接wifi
	ret = Wifi_SendAT(CONNECT_WIFI, "OK");
	if(ret)	return 2;  // 无法连接wifi
	
	return 0;
}

/*******************************
connect to the TCP server on the computer
备注：返回值0：正常，1：无法设置单连接，2：无法设置透传模式，3：无法连接服务器
*******************************/
unsigned char Wifi_ConnectServer()
{
	bit ret;
	
	//设置单连接模式
	ret = Wifi_SendAT(SINGLE_CONNECTED, "OK");
	if(ret)	return 1;  // 无法设置单连接
	
	//设置透传模式
	ret = Wifi_SendAT(SERIANET_MODE, "OK");
	if(ret)	return 2;  // 无法设置透传模式
	
	//连接服务器
	ret = Wifi_SendAT(CONNECT_SERVER, "OK");
	if(ret)	return 3;  // 无法连接服务器
	
	return 0;
}

/*******************************
enter seriaNet
备注：返回值0：正常，1：无法进入透传
*******************************/
void Wifi_EnterSeriaNet()
{
	wifi_SendString(ENTER_SERIANET);
}

/*******************************
exit seriaNet
*******************************/
void Wifi_ExitSeriaNet()
{
	wifi_SendString("\r\n");  //硬件bug，串口第一次发送数据，最开始会有一个空格，为了和后面指令进行区分
	delay_ms(100);
	wifi_SendString(EXIT_SERIANET);
	delay_ms(1000);	
}

/*******************************
initialize wifi
备注：返回值0：正常，1：无法连接wifi，2：无法连接服务器，3：无法进入透传
*******************************/
unsigned char Wifi_Init()
{
	unsigned char ret;
	
	uart_init(0xfd);  // 先配置串口
	Wifi_ExitSeriaNet(); 	// 退出透传
	//连接wifi
	ret = Wifi_Connect();
	if(ret)	return 1;
	//连接服务器
	ret = Wifi_ConnectServer();
	if(ret)	return 2;
	//进入透传
	Wifi_EnterSeriaNet();

	return 0;
}

/*******************************
get the time in the response
*******************************/
void Wifi_GetTime()
{
	char *p;  // 位置指针
	unsigned char sum = 0;  // 数据的值
	unsigned char bitNum = 2;  // 数据的位数

	// 指针指向对应时间名称的第一个字符
	p = strstr(WIFI_RX_BUF, "E1");
	p += 3;  //指针偏移到首个数据字符位置
	while(bitNum--)  // 把读取到的位拼接成一个整数
	{
		sum += (*p++ - '0');
		if(bitNum != 0)
			sum *= 10;
	}
	if (sum < 24)
		cur_time_hour = sum;

	sum = 0;
	bitNum = 2;
	p = strstr(WIFI_RX_BUF, "E2");
	p += 3;  //指针偏移到首个数据字符位置
	while(bitNum--)  // 把读取到的位拼接成一个整数
	{
		sum += (*p++ - '0');
		if(bitNum != 0)
			sum *= 10;
	}
	if (sum < 59)
		cur_time_min = sum;

	sum = 0;
	bitNum = 2;
	p = strstr(WIFI_RX_BUF, "E3");
	p += 3;  //指针偏移到首个数据字符位置
	while(bitNum--)  // 把读取到的位拼接成一个整数
	{
		sum += (*p++ - '0');
		if(bitNum != 0)
			sum *= 10;
	}
	if (sum < 59)
		cur_time_sec = sum;	
}
/*******************************
detect the response
*******************************/
bit Wifi_GetEventExist(char *eventNum)
{	
	//找不到buf中对应事件的位置，返回0
	if((strstr(WIFI_RX_BUF, eventNum)) != NULL)
		return 1;
	else
		return 0;
}

/*******************************
analysis the response from the server and update the global variable
*******************************/
void Wifi_Analysis_Response()
{
	bit ret0;
	unsigned char ret=0;

	while (WIFI_RX_STA == 0)  // 等待接收完成标志
	WIFI_RX_STA = 0;  // 清标志

	ret0 = Wifi_GetEventExist("K1");
	if (ret0 == 1) ret = 1;
	ret0 = Wifi_GetEventExist("K2");
	if (ret0 == 1) ret = 2;	
	ret0 = Wifi_GetEventExist("K3");
	if (ret0 == 1) ret = 3;
	ret0 = Wifi_GetEventExist("K4");
	if (ret0 == 1) ret = 4;
	ret0 = Wifi_GetEventExist("K5");
	if (ret0 == 1) ret = 5;
	ret0 = Wifi_GetEventExist("K6");
	if (ret0 == 1) ret = 6;

	if (ret==1)	 // 模式：关、标准、睡眠风、温控
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
	} else if (ret==2 && fan_mode!=0)	// 左右摇头
		rotate_speed = (rotate_speed+1)%3;	
	else if (ret==3 && fan_mode==1)  // 风速
		fan_speed = (fan_speed%3)+1;	
	else if (ret==4 && fan_mode!=0)  // 定时器倒计时关机
	{
		timer_mode = (timer_mode+1)%4;
		if (timer_mode==0)	// 关闭定时
		{
			timer_min = 0;
			timer_sec = 0;  // 定时清空	
		}
	}	
	else if (ret==5 && fan_mode!=0)  // 定时器设置时间，单击
	{
		if (timer_mode==1)
			timer_sec = (timer_sec+1)%60;
		else if (timer_mode==2)
			timer_min = (timer_min+1)%100;	
	} else if (ret==6)	// 切换显示温度或风扇状态
		dis_temp_state = ~dis_temp_state; 
}

//输出重定向到串口，调试时使用，不能连wifi芯片，否则串口冲突
char putchar(char c)
{
	wifi_SendByte(c);
	return c;  //返回给函数的调用者printf
}
