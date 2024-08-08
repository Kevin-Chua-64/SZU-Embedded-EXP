#include "public.h"
#include "lcd1602.h"

void main()
{	
	lcd1602_init();  // LCD1602≥ı ºªØ
	cursor_pos_set(0, 0);
	lcd1602_show_string("Hello World!");
	while (1);	
}