#include "wifi.h"

#if USE_WIFI_TIMER1
xdata unsigned char WIFI_RX_BUF[WIFI_REC_LEN];
bit WIFI_RX_STA=0;  // 接收完成标志，不影响下一次的接收，但需要手动清零
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
函数名：wifi_SendByte
函数功能：从串口发送一个字节
函数形参：u8 dat
函数返回值：void 
备注：
*******************************/
void wifi_SendByte(unsigned char dat)
{
	ES=0;         //关闭串口中断
	TI=0;         //清发送完毕中断请求标志位
	SBUF = dat;   //发送
	while(TI==0); //等待发送完毕
	TI=0;         //清发送完毕中断请求标志位
	ES=1;         //允许串口中断
}

/*******************************
函数名：wifi_SendString
函数功能：从串口发送字符串
函数形参：u8 str[]
函数返回值：void 
备注：
*******************************/
void wifi_SendString(unsigned char *str)
{
	while(*str != '\0') //字符串结束标志
	{
		wifi_SendByte(*str);
		delay_10us(20);	// 延时，等待字节发送完
		str++;	
	}
}

/*******************************
函数名：wifi_IRQn
函数功能：wifi从串口接收的中断服务函数
函数形参：void
函数返回值：void 
备注：接收一个字符串
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
函数名：Wifi_SendAT
函数功能：发送AT指令
函数形参：s8 sAT[], s8 expectATRes[] 
函数返回值：bit
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
函数名：Wifi_Connect
函数功能：连接wifi
函数形参：void
函数返回值：u8
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
函数名：Wifi_ConnectServer
函数功能：连接TCP服务器
函数形参：void
函数返回值：u8
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
函数名：Wifi_EnterSeriaNet
函数功能：进入透传
函数形参：void
函数返回值：u8
备注：返回值0：正常，1：无法进入透传
*******************************/
unsigned char Wifi_EnterSeriaNet()
{
	bit ret;

	ret = Wifi_SendAT(ENTER_SERIANET, "OK");
	if(ret)	return 1;  // 无法进入透传

	return 0;
}

/*******************************
函数名：Wifi_ExitSeriaNet
函数功能：退出透传
函数形参：void
函数返回值：void
备注：
*******************************/
void Wifi_ExitSeriaNet()
{
	wifi_SendString("\r\n");  //硬件bug，串口第一次发送数据，最开始会有一个空格，为了和后面指令进行区分
	delay_ms(100);
	wifi_SendString("+++");
	delay_ms(1000);	
}

/*******************************
函数名：Wifi_Init
函数功能：wifi初始化
函数形参：void
函数返回值：u8
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
	ret = Wifi_EnterSeriaNet();
	if(ret)	return 3;
	
	return 0;
}

/*******************************
函数名：Wifi_GetEventData
函数功能：根据字符串里的事件编号，解析相应数据
函数形参：s8 eventNum[]
函数返回值：u8
备注：返回对应事件的数据
	  事件的格式"E1:xxx,E2:xxx,E3:xxx,E4E5E6......"
	  E1->
*******************************/
unsigned char Wifi_GetEventData(char *eventNum)
{
//	char *p;
//	s16 sum = 0;
//	s8 sign = 1;
//	u8 bitNum = 0;
//	
//	if((p = strstr(eventNum, "V6")) != NULL)
//	{
//		p = strstr(wifi_t.rxBuf, "V6");
//		return *(p+3) - '0';
//	}
//	
//	//找出V1在buf中的位置
//	if((p = strstr(wifi_t.rxBuf, eventNum)) == NULL)
//		return 0;
//	
//	if(*(p+3) != '-')
//		p += 3;//偏移到首个数据字符位置
//	else //负数
//	{
//		p += 4;//偏移到首个数据字符位置
//		sign = -1;
//	}
//	
//	for(bitNum = 1; bitNum < 4; bitNum++)//255
//	{
//		//判断是否是结束的 ',' 
//		if((*(p + bitNum) == ',') || (*(p + bitNum) == ' '))
//			break;
//	}
//	
//	while(bitNum--)//3 2 1 0
//	{
//		sum += (*p++ - '0');
//		
//		if(bitNum != 0)
//			sum *= 10;//250
//	}
//	
//	return (sign * sum);
	return 0;
}

/*******************************
函数名：Wifi_Analysis_Response
函数功能：解析wifi返回的数据并更新全局变量
函数形参：void
函数返回值：void
备注：
*******************************/
void Wifi_Analysis_Response()
{
//	u8 r, g, b;
//	u8 relay;
//	s8 dcSpeed, stepMotor;
//	
//	while(WIFI_RX_STA == 0);  // 等待接收完成标志
//	{
//		WIFI_RX_STA = 0;
//		
//		//解析数据
//		r = Wifi_GetEventData("V1");
//		g = Wifi_GetEventData("V2");
//		b = Wifi_GetEventData("V3");
//		printf("RGB:%d  %d  %d\r\n", r, g, b);
//		RGB_SetColor(r, g, b);
//		
//		dcSpeed = Wifi_GetEventData("V4");
//		printf("dcSpeed = %d\r\n", dcSpeed);
////		DcMotor_SetSpeed(dcSpeed);
//		*DCMotorSpeed = dcSpeed;
//		
//		
//		stepMotor = Wifi_GetEventData("V5");
//		printf("stepMotor = %d\r\n", stepMotor);
////		if(stepMotor != *StepMotorPos){
////			*StepMotorPos = stepMotor;
////			if(stepMotor-*StepMotorPos < 0)
////				StepMotor_Move(0, 40*(*StepMotorPos-stepMotor));	
////			else 
////				StepMotor_Move(1, 40*(stepMotor-*StepMotorPos));
////		}
//		StepMotor_Move(0, 20*(stepMotor-5));
//		*StepMotorPos = stepMotor;
//		
//		relay = Wifi_GetEventData("V6");
//		printf("relay = %d\r\n", relay);
//		*relayState = relay;
////		if(relay)
////			RELAY_ON;
////		else
////			RELAY_OFF;
//		
//	}	
	
}

//输出重定向到串口
char putchar(char c)
{
	wifi_SendByte(c);
	return c;  //返回给函数的调用者printf
}

#endif
