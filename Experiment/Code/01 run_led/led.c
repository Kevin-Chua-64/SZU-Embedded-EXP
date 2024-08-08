#include <reg51.h>
#define time_delay 200

void delay(unsigned char z)	 // delay z ms
{
	unsigned char i, j;
	for(j=0;j<z;j++)
		for(i=0;i<127;i++);
}

void led_running(unsigned char dir)
{
	static unsigned char i=0;
	if (dir==0){  // up
		P2 = (0xfe<<i)|(0xfe>>(8-i));
		if (i==8) i=0;
	}else if (dir==1){  // down
		P2 = (0x7f>>i)|(0x7f<<(8-i));
		if (i==8) i=0;
	}else if (dir==2){  // up and down
		if (i<7)
			P2 = (0xfe<<i)|(0xfe>>(8-i));
		else
			P2 = (0x7f>>(i-7))|(0x7f<<(8-(i-7)));
		if (i==14) i=0;
	}
	
	delay(time_delay);
	i++;	
}

void main()
{
	while(1) led_running(2);
}