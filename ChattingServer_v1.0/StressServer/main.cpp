#include "StressServer.h"
#include <string>
#include <iostream>


void ErrorExit(const char* msg) 
{
	printf("%s\n", msg);
	exit(1);
}

int main()
{
	const int SERVER_PORT = 9898;

	StressServer stressServer;

	if (false == stressServer.InitSocket()) 
	{
		ErrorExit("���� �ʱ�ȭ ����\n");
	}

	if (false == stressServer.StartServer()) 
	{
		ErrorExit("IOCP ���� ���� ����\n");
	}

	printf("[�˸�] Enter �Է½� ��Ʈ���� ���� �����մϴ�.\n");
	printf("[�˸�] quit�� �Է½� ��Ʈ���� ���� �����մϴ�.\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	stressServer.DestroyThreads();
	printf("[�˸�] ������ ����Ǿ����ϴ�. ����Ű�� ������ â�� �����մϴ�.\n");
	char c = getchar();
	c = getchar();
	return 0;
}

