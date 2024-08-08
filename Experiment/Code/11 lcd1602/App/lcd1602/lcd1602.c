#include "lcd1602.h"

unsigned char read_lcd1602_state()  //����Ƭ���Ƿ�Ϊ��æ��״̬
{
	unsigned char tmp, state=0;
	LCD1602_RS = 0;	 // ����
	LCD1602_RW = 1;	 // ��
	LCD1602_E = 1;
	tmp = LCD1602_DATAPORT;
	state = (tmp&0x80);	 // ֻ�����λ
	LCD1602_E = 0;
	return state;  // Ϊ0��Ϊ����æ
}

void lcd1602_write_cmd(unsigned char cmd)  // дָ��
{
	while(read_lcd1602_state());  // �ȴ�LCD�˳���æ״̬
	LCD1602_RS = 0;	 // ����
	LCD1602_RW = 0;	 // д
	LCD1602_E = 0;
	LCD1602_DATAPORT = cmd;  // �Ȱ�ָ���͵�IO
 	LCD1602_E = 1;
 	delay_ms(1);
	LCD1602_E = 0;
}

void lcd1602_write_data(unsigned char dat)	// д����
{
	while(read_lcd1602_state());  // �ȴ�LCD�˳���æ״̬
	LCD1602_RS = 1;	 // ����
	LCD1602_RW = 0;	 // д
	LCD1602_E = 0;
	LCD1602_DATAPORT = dat;  // �Ȱ������͵�IO
 	LCD1602_E = 1;
 	delay_ms(1);
	LCD1602_E = 0;
}

void cursor_pos_set(unsigned char x,unsigned char y)  // ���ù���λ��
{
	unsigned char address;
	//x �����У�y �����У�д���ַʱҪ�����λΪ1��������0x80
	if (y>1 || x>15) return;  // ���в���������ǿ���˳�
	else if (y==0) address = 0x80+x;  // ��ʾ�ڵ�һ��
	else if (y==1) address = 0x80+0x40+x;  // ��ʾ�ڵڶ���
	lcd1602_write_cmd(address);
}		   

void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);  // ��������8λ����ʾ2�У�5*7����/�ַ�
	lcd1602_write_cmd(0x0c);  // ��ʾ���ܿ����޹�꣬�����˸
	lcd1602_write_cmd(0x06);  // д�������ݺ������ƣ���ʾ�����ƶ�
	lcd1602_write_cmd(0x01);  // ����	
}

void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

void lcd1602_show_string(char *str)
{
	while(*str != '\0')  // �ַ�������'\0'��β��ֻҪǰ�������ݾ���ʾ
	{
    	lcd1602_write_data(*str);
		str++;  // ָ�����
	}
}
