#include "ChatServer.h"
#include <string>
#include <iostream>

const int SERVER_PORT = 9898;
const int MAX_CLIENT = 500;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��

void ErrorExit(char* msg){
    printf("%s\n", msg);
    exit(1);
}

int main()
{
	ChatServer chatServer;
	bool bSuccess = true;

	bSuccess = chatServer.InitSocket();
	if(!bSuccess) ErrorExit("���� �ʱ�ȭ ����");

	bSuccess = chatServer.BindAndListen(SERVER_PORT);
	if(!bSuccess) ErrorExit("���� ���ε� ����");

	chatServer.SetClientInfos(MAX_CLIENT);

	bSuccess = chatServer.StartServer();
	if(!bSuccess) ErrorExit("IOCP ���� ���� ����");

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

