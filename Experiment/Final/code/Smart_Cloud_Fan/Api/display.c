#include "display.h"

/*******************************
digital display the module value in ch[8]
*******************************/
bit flag_digital_display = 0;	
bit flag_blink_8ms = 0;				 
void digital_display(unsigned char *ch)
{
	static unsigned char i=0;
	static bit j=0;
	static unsigned char cnt=0;

	if (flag_digital_display==1)
	{
		flag_digital_display = 0;
		i++;
		if (i==8) i=0;
		SMG_PORT = 0x00;  // 消音
	}

	if (flag_blink_8ms == 1)// 闪烁
	{
		flag_blink_8ms = 0;
		cnt++;
		if (cnt==30)  // 0.24s
		{
			cnt = 0;
			j = ~j;
		}
	}	
	if (j == 1)  // j为1时，跳过该位的显示，即不显示，实现闪烁
	{
		if (timer_mode == 1){
			if (i==2 || i==3) i = 4;
		} else if (timer_mode == 2){
			if (i==0 || i==1) i = 2;
		}
	}


   	switch(i)  // 位选和段选
	{
		case 0: LSC=1;LSB=1;LSA=1; SMG_PORT = *(ch+0); break;
		case 1: LSC=1;LSB=1;LSA=0; SMG_PORT = *(ch+1); break;
		case 2: LSC=1;LSB=0;LSA=1; SMG_PORT = *(ch+2); break;
		case 3: LSC=1;LSB=0;LSA=0; SMG_PORT = *(ch+3); break;
		case 4: LSC=0;LSB=1;LSA=1; SMG_PORT = *(ch+4); break;
		case 5: LSC=0;LSB=1;LSA=0; SMG_PORT = *(ch+5); break;
		case 6: LSC=0;LSB=0;LSA=1; SMG_PORT = *(ch+6); break;
		case 7: LSC=0;LSB=0;LSA=0; SMG_PORT = *(ch+7); break;
	}
}
