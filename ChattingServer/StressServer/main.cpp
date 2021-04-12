#include "StressServer.h"
#include <string>
#include <iostream>


void ErrorExit(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

int main()
{
	const int SERVER_PORT = 9898;
	const int MAX_CLIENT = 500;

	StressServer stressServer;

	if (false == stressServer.InitSocket()) {
		ErrorExit("���� �ʱ�ȭ ����\n");
	}

	if (false == stressServer.StartServer()) {
		ErrorExit("IOCP ���� ���� ����\n");
	}

	printf("[�˸�] quit�� �Է½� ���� �����մϴ�.\n");
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

