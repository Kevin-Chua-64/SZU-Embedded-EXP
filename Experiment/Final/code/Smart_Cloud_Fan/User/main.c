#include "main.h"

void main()
{
	unsigned char ret;

	ret = Init();  // ��ʼ������

	if (ret)  // wifi����
	{
		P2 = 0xff&~(1<<(ret-1));  // led��ʾ��Ӧ����
		while (1);
	}
	else
	{
		while (1)
		{	
			key_state_update();	 // ����ɨ����±���
			temp_update();	// �����¶�
			wifi_update();	// wifi���ݸ���

			aciton();  // ����ȫ�ֱ���ʵ�ֹ���
			nixie();  // �������ʾ
		}
	}
}
