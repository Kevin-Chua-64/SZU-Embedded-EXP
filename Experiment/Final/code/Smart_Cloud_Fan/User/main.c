#include "main.h"

void main()
{
	unsigned char ret;

	ret = Init();  // 初始化函数

	if (ret)  // wifi出错
	{
		P2 = 0xff&~(1<<(ret-1));  // led显示对应出错
		while (1);
	}
	else
	{
		while (1)
		{	
			key_state_update();	 // 按键扫描更新变量
			temp_update();	// 更新温度
			wifi_update();	// wifi数据更新

			aciton();  // 根据全局变量实现功能
			nixie();  // 数码管显示
		}
	}
}
