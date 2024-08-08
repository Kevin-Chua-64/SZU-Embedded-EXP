#include "main.h"

void main()
{
	unsigned char ret;
	unsigned char key[8]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07};
	unsigned char num;
	static unsigned char i=0;

	timer0_config();			  
//	lcd1602_init();
//	pwm_init(0xff, 0xf6, 100, 0);
//	DC_Motor_init(0xff, 0xf6, 100, 0);
//	ds18b20_init();
//	ired_init();

//	uart_init(0xfd);  // 波特率19200
    ret = Wifi_Init();

	if (ret)  // wifi出错
	{
		P2 = 0xff&~(1<<(ret-1));  // led显示对应出错
		while (1);
	}
	else
	{
		while (1)
		{	
			num = matrix_key_scan();
			if (num != 0xff)
			{
				//printf("%c\n", num);
				key[i] = led_num[num/10];
				key[i+1] = led_num[num%10];
				i += 2;
				if (i==8) i = 0;


			}
			digital_display(key);

			Wifi_Analysis_Response();
	//		if (UART_RX_STA == 1)
	//		{
	//			UART_RX_STA = 0;
	//			uart_SendString(UART_RX_BUF);
	//		}


			

	
		}
	}
}
