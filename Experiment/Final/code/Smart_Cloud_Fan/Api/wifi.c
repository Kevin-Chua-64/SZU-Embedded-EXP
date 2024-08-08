#include "wifi.h"

xdata unsigned char WIFI_RX_BUF[WIFI_REC_LEN];
bit WIFI_RX_STA=0;  // ������ɱ�־����Ӱ����һ�εĽ��գ�����Ҫ�ֶ�����
/*******************************
initialize the uart for wifi
input parameter baud rate
RX->P30  TX->P31
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

void wifi_SendByte(unsigned char dat)
{
	ES=0;         //�رմ����ж�
	TI=0;         //�巢������ж������־λ
	SBUF = dat;   //����
	while(TI==0); //�ȴ��������
	TI=0;         //�巢������ж������־λ
	ES=1;         //�������ж�
}

void wifi_SendString(unsigned char *str)
{
	TR0 = 0;   //�����ж�Ӱ���д����ʱ����ʱ��ʹ�ܶ�ʱ��0
	while(*str != '\0') //�ַ���������־
	{
		wifi_SendByte(*str);
		delay_10us(10);	// ��ʱ���ȴ��ֽڷ�����
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
send AT instruction in ESP8266
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
connect to wifi
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
connect to the TCP server on the computer
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
enter seriaNet
��ע������ֵ0��������1���޷�����͸��
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
	wifi_SendString("\r\n");  //Ӳ��bug�����ڵ�һ�η������ݣ��ʼ����һ���ո�Ϊ�˺ͺ���ָ���������
	delay_ms(100);
	wifi_SendString(EXIT_SERIANET);
	delay_ms(1000);	
}

/*******************************
initialize wifi
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
	Wifi_EnterSeriaNet();

	return 0;
}

/*******************************
get the time in the response
*******************************/
void Wifi_GetTime()
{
	char *p;  // λ��ָ��
	unsigned char sum = 0;  // ���ݵ�ֵ
	unsigned char bitNum = 2;  // ���ݵ�λ��

	// ָ��ָ���Ӧʱ�����Ƶĵ�һ���ַ�
	p = strstr(WIFI_RX_BUF, "E1");
	p += 3;  //ָ��ƫ�Ƶ��׸������ַ�λ��
	while(bitNum--)  // �Ѷ�ȡ����λƴ�ӳ�һ������
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
	p += 3;  //ָ��ƫ�Ƶ��׸������ַ�λ��
	while(bitNum--)  // �Ѷ�ȡ����λƴ�ӳ�һ������
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
	p += 3;  //ָ��ƫ�Ƶ��׸������ַ�λ��
	while(bitNum--)  // �Ѷ�ȡ����λƴ�ӳ�һ������
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
	//�Ҳ���buf�ж�Ӧ�¼���λ�ã�����0
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

	while (WIFI_RX_STA == 0)  // �ȴ�������ɱ�־
	WIFI_RX_STA = 0;  // ���־

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

	if (ret==1)	 // ģʽ���ء���׼��˯�߷硢�¿�
	{
		fan_mode = (fan_mode+1)%4;	
		if (fan_mode==0)  // �ػ�ʱ��λ
		{
			rotate_speed = 0;  // ��ҡͷ
			fan_speed = 1;  // �ٶ�Ϊ1��
			timer_mode = 0;	 // ��ʱ�رչ��ܹ�
			timer_min = 0;
			timer_sec = 0;  // ��ʱ���
		} else if (fan_mode==3)	 // �¿ؿ�ʼʱ��¼��ʱ�¶Ⱥ�ռ�ձ�
		{
			start_temp = temp;
			start_duty = g_pwm_duty;	
		}	
	} else if (ret==2 && fan_mode!=0)	// ����ҡͷ
		rotate_speed = (rotate_speed+1)%3;	
	else if (ret==3 && fan_mode==1)  // ����
		fan_speed = (fan_speed%3)+1;	
	else if (ret==4 && fan_mode!=0)  // ��ʱ������ʱ�ػ�
	{
		timer_mode = (timer_mode+1)%4;
		if (timer_mode==0)	// �رն�ʱ
		{
			timer_min = 0;
			timer_sec = 0;  // ��ʱ���	
		}
	}	
	else if (ret==5 && fan_mode!=0)  // ��ʱ������ʱ�䣬����
	{
		if (timer_mode==1)
			timer_sec = (timer_sec+1)%60;
		else if (timer_mode==2)
			timer_min = (timer_min+1)%100;	
	} else if (ret==6)	// �л���ʾ�¶Ȼ����״̬
		dis_temp_state = ~dis_temp_state; 
}

//����ض��򵽴��ڣ�����ʱʹ�ã�������wifiоƬ�����򴮿ڳ�ͻ
char putchar(char c)
{
	wifi_SendByte(c);
	return c;  //���ظ������ĵ�����printf
}
