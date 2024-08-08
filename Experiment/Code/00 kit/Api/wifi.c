#include "wifi.h"

#if USE_WIFI_TIMER1
xdata unsigned char WIFI_RX_BUF[WIFI_REC_LEN];
bit WIFI_RX_STA=0;  // ������ɱ�־����Ӱ����һ�εĽ��գ�����Ҫ�ֶ�����
/*******************************
��������uart_init
�������ܣ����ڳ�ʼ��
�����βΣ�u8 baud
��������ֵ��void 
��ע��RX->P30  TX->P31
	  ռ�ü�ʱ��1
*******************************/
void uart_init(unsigned char baud)	//baud: 0xfa->9600 0xfd->19200 0xff->57600
{
	TMOD|=0X20;	  //���ü�����1������ʽ2
	SCON=0X50;	  //���ô��ڹ�����ʽ1
	PCON=0X80;	  //�����ʼӱ�
	TH1=baud;	  //��������ʼֵ����
	TL1=baud;
	ES=1;		  //�򿪽����ж�
	EA=1;	   	  //�����ж�
	TR1=1;	 	  //�򿪼�����	
}

/*******************************
��������wifi_SendByte
�������ܣ��Ӵ��ڷ���һ���ֽ�
�����βΣ�u8 dat
��������ֵ��void 
��ע��
*******************************/
void wifi_SendByte(unsigned char dat)
{
	ES=0;         //�رմ����ж�
	TI=0;         //�巢������ж������־λ
	SBUF = dat;   //����
	while(TI==0); //�ȴ��������
	TI=0;         //�巢������ж������־λ
	ES=1;         //�������ж�
}

/*******************************
��������wifi_SendString
�������ܣ��Ӵ��ڷ����ַ���
�����βΣ�u8 str[]
��������ֵ��void 
��ע��
*******************************/
void wifi_SendString(unsigned char *str)
{
	while(*str != '\0') //�ַ���������־
	{
		wifi_SendByte(*str);
		delay_10us(20);	// ��ʱ���ȴ��ֽڷ�����
		str++;	
	}
}

/*******************************
��������wifi_IRQn
�������ܣ�wifi�Ӵ��ڽ��յ��жϷ�����
�����βΣ�void
��������ֵ��void 
��ע������һ���ַ���
*******************************/
void wifi_IRQn() interrupt 4
{
	static unsigned char i=0;
	
	if (RI)
	{
		if (i==0)
			memset(WIFI_RX_BUF, 0, sizeof WIFI_RX_BUF);  //�������          
		RI=0;  // ���־
		WIFI_RX_BUF[i++] = SBUF;  // ��ȡ���յ������ݴ���wifi����
		if (WIFI_RX_BUF[i-2]=='\r' && WIFI_RX_BUF[i-1]=='\n')  //��⵽�س�������־��Ȼ�����ɾ��־
		{
			WIFI_RX_BUF[i-2] = '\0';  // �ֶ����'\0'
		 	WIFI_RX_BUF[i-1] = 0;
			i = 0;
			WIFI_RX_STA = 1;
		}
		if (i>WIFI_REC_LEN) i=0;  // ��������ֽ���������
	}		
}

/*******************************
��������Wifi_SendAT
�������ܣ�����ATָ��
�����βΣ�s8 sAT[], s8 expectATRes[] 
��������ֵ��bit
��ע����һ���β���ָ��ڶ������ڴ��Ļظ���Ҳ����������AT��Ӧ����"AT+CWMODE_CUR=1","OK"
	  ����ֵ0��������1���ȴ���ȷ��Ӧ��ʱ
*******************************/
bit Wifi_SendAT(char *sAT, char *expectATRes)
{
	unsigned int timeCnt=0, timeOut=1000;	// ��ʱ��������ʱ����
	
	//wifi����ʱ��ϳ�����������
	if(strstr(sAT, "AT+CWJAP_CUR=\"") != NULL) timeOut = 8000;

	wifi_SendString(sAT);  //����ATָ��
	//�ȴ�������ȷָ��
	while(1)
	{
		while(WIFI_RX_STA != 1)  //�ȴ�������ɱ�־
		{
			timeCnt++;
			delay_ms(1);
			
			if(timeCnt > timeOut)
				return 1;  //��ʱ
		}
		WIFI_RX_STA = 0;  //������ձ�־
		
		//��ѯ���յ��������Ƿ����������Ե�����
		if(strstr(WIFI_RX_BUF, expectATRes) != NULL)
			break;
	}
	return 0;
}

/*******************************
��������Wifi_Connect
�������ܣ�����wifi
�����βΣ�void
��������ֵ��u8
��ע������ֵ0��������1���޷�����stationģʽ��2���޷�����wifi
*******************************/
unsigned char Wifi_Connect()
{
	bit ret;
	
	//����stationģʽ
	ret = Wifi_SendAT(STATION_MODE, "OK");
	if(ret) return 1;  // �޷�����stationģʽ
	
	//����wifi
	ret = Wifi_SendAT(CONNECT_WIFI, "OK");
	if(ret)	return 2;  // �޷�����wifi
	
	return 0;
}

/*******************************
��������Wifi_ConnectServer
�������ܣ�����TCP������
�����βΣ�void
��������ֵ��u8
��ע������ֵ0��������1���޷����õ����ӣ�2���޷�����͸��ģʽ��3���޷����ӷ�����
*******************************/
unsigned char Wifi_ConnectServer()
{
	bit ret;
	
	//���õ�����ģʽ
	ret = Wifi_SendAT(SINGLE_CONNECTED, "OK");
	if(ret)	return 1;  // �޷����õ�����
	
	//����͸��ģʽ
	ret = Wifi_SendAT(SERIANET_MODE, "OK");
	if(ret)	return 2;  // �޷�����͸��ģʽ
	
	//���ӷ�����
	ret = Wifi_SendAT(CONNECT_SERVER, "OK");
	if(ret)	return 3;  // �޷����ӷ�����
	
	return 0;
}

/*******************************
��������Wifi_EnterSeriaNet
�������ܣ�����͸��
�����βΣ�void
��������ֵ��u8
��ע������ֵ0��������1���޷�����͸��
*******************************/
unsigned char Wifi_EnterSeriaNet()
{
	bit ret;

	ret = Wifi_SendAT(ENTER_SERIANET, "OK");
	if(ret)	return 1;  // �޷�����͸��

	return 0;
}

/*******************************
��������Wifi_ExitSeriaNet
�������ܣ��˳�͸��
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void Wifi_ExitSeriaNet()
{
	wifi_SendString("\r\n");  //Ӳ��bug�����ڵ�һ�η������ݣ��ʼ����һ���ո�Ϊ�˺ͺ���ָ���������
	delay_ms(100);
	wifi_SendString("+++");
	delay_ms(1000);	
}

/*******************************
��������Wifi_Init
�������ܣ�wifi��ʼ��
�����βΣ�void
��������ֵ��u8
��ע������ֵ0��������1���޷�����wifi��2���޷����ӷ�������3���޷�����͸��
*******************************/
unsigned char Wifi_Init()
{
	unsigned char ret;
	
	uart_init(0xfd);  // �����ô���
	Wifi_ExitSeriaNet(); 	// �˳�͸��
	//����wifi
	ret = Wifi_Connect();
	if(ret)	return 1;
	//���ӷ�����
	ret = Wifi_ConnectServer();
	if(ret)	return 2;
	//����͸��
	ret = Wifi_EnterSeriaNet();
	if(ret)	return 3;
	
	return 0;
}

/*******************************
��������Wifi_GetEventData
�������ܣ������ַ�������¼���ţ�������Ӧ����
�����βΣ�s8 eventNum[]
��������ֵ��u8
��ע�����ض�Ӧ�¼�������
	  �¼��ĸ�ʽ"E1:xxx,E2:xxx,E3:xxx,E4E5E6......"
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
//	//�ҳ�V1��buf�е�λ��
//	if((p = strstr(wifi_t.rxBuf, eventNum)) == NULL)
//		return 0;
//	
//	if(*(p+3) != '-')
//		p += 3;//ƫ�Ƶ��׸������ַ�λ��
//	else //����
//	{
//		p += 4;//ƫ�Ƶ��׸������ַ�λ��
//		sign = -1;
//	}
//	
//	for(bitNum = 1; bitNum < 4; bitNum++)//255
//	{
//		//�ж��Ƿ��ǽ����� ',' 
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
��������Wifi_Analysis_Response
�������ܣ�����wifi���ص����ݲ�����ȫ�ֱ���
�����βΣ�void
��������ֵ��void
��ע��
*******************************/
void Wifi_Analysis_Response()
{
//	u8 r, g, b;
//	u8 relay;
//	s8 dcSpeed, stepMotor;
//	
//	while(WIFI_RX_STA == 0);  // �ȴ�������ɱ�־
//	{
//		WIFI_RX_STA = 0;
//		
//		//��������
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

//����ض��򵽴���
char putchar(char c)
{
	wifi_SendByte(c);
	return c;  //���ظ������ĵ�����printf
}

#endif
