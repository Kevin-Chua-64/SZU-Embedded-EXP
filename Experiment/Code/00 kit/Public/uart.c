#include "uart.h"

#if USE_UART_TIMER1
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
��������uart_SendByte
�������ܣ��Ӵ��ڷ���һ���ֽ�
�����βΣ�u8 dat
��������ֵ��void 
��ע��
*******************************/
void uart_SendByte(unsigned char dat)
{
	ES=0;         //�رմ����ж�
	TI=0;         //�巢������ж������־λ
	SBUF = dat;   //����
	while(TI==0); //�ȴ��������
	TI=0;         //�巢������ж������־λ
	ES=1;         //�������ж�
}

/*******************************
��������uart_SendString
�������ܣ��Ӵ��ڷ����ַ���
�����βΣ�u8 str[]
��������ֵ��void 
��ע��
*******************************/
void uart_SendString(unsigned char *str)
{
	while(*str != '\0') //�ַ���������־
	{
		uart_SendByte(*str);
		delay_10us(20);	// ��ʱ���ȴ��ֽڷ�����
		str++;	
	}
}

xdata unsigned char UART_RX_BUF[UART_REC_LEN];  //���ջ���
bit UART_RX_STA = 0;
/*******************************
��������uart_IRQn
�������ܣ����ڽ����жϷ�����
�����βΣ�void
��������ֵ��void 
��ע������һ���ַ���
*******************************/
void uart_IRQn() interrupt 4
{
	static unsigned char i=0;
	
	if (RI)
	{
		if (i==0)
			memset(UART_RX_BUF, 0, sizeof UART_RX_BUF);  //�������          
		RI=0;  // ���־
		UART_RX_BUF[i++] = SBUF;  // ��ȡ���յ������ݴ���ȫ�ֱ����Ļ���
		if (UART_RX_BUF[i-2]=='\r' && UART_RX_BUF[i-1]=='\n')  //��⵽�س�������־��Ȼ�����ɾ��־
		{
			UART_RX_BUF[i-2] = '\0';  // �ֶ����'\0'
		 	UART_RX_BUF[i-1] = 0;
			i = 0;
			UART_RX_STA = 1;
		}
		if (i>UART_REC_LEN) i=0;  // ��������ֽ���������
	}		
}

//����ض��򵽴���
char putchar(char c)
{
	uart_SendByte(c);
	return c;  //���ظ������ĵ�����printf
}

#endif
