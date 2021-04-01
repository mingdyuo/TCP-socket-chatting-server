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

	if(false == chatServer.Initialize(SERVER_PORT)){
		ErrorExit("���� �ʱ�ȭ ����\n");
	}

	if(false == chatServer.Run(MAX_CLIENT)){
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

	chatServer.CloseServer();
	printf("[�˸�] ������ ����Ǿ����ϴ�. ����Ű�� ������ â�� �����մϴ�.\n");
	getchar();
	return 0;
}

