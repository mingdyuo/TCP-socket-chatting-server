#include "ChatServer.h"
#include <string>
#include <iostream>

const int SERVER_PORT = 9898;
const int MAX_CLIENT = 100;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��

int main()
{
	ChatServer chatServer;

	chatServer.InitSocket();
	chatServer.BindAndListen(SERVER_PORT);
	chatServer.SetClientInfos(MAX_CLIENT);
	chatServer.StartServer();

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

	chatServer.DestroyThreads();
	printf("[�˸�] ������ ����Ǿ����ϴ�. ����Ű�� ������ â�� �����մϴ�.\n");
	getchar();
	return 0;
}

