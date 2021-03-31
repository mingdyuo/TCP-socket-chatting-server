#include "ChatServer.h"
#include <string>
#include <iostream>


void ErrorExit(char* msg){
    printf("%s\n", msg);
    exit(1);
}

int main()
{
	const int SERVER_PORT 	= 9898;
	const int MAX_CLIENT 	= 500;

	ChatServer chatServer;
	bool bSuccess = true;

	bSuccess = chatServer.InitSocket();
	if(!bSuccess) ErrorExit("���� �ʱ�ȭ ����");

	bSuccess = chatServer.BindAndListen(SERVER_PORT);
	if(!bSuccess) ErrorExit("���� ���ε� ����");

	bSuccess = chatServer.Run(MAX_CLIENT);
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

	chatServer.CloseServer();
	printf("[�˸�] ������ ����Ǿ����ϴ�. ����Ű�� ������ â�� �����մϴ�.\n");
	getchar();
	return 0;
}

