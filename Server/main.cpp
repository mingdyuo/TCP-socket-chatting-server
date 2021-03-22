#include "IOCPServer.h"
#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 9898;
const UINT16 MAX_CLIENT = 100;		//총 접속할수 있는 클라이언트 수

int main()
{
	IOCPServer iocpServer;

	iocpServer.InitSocket();
	iocpServer.BindAndListen(SERVER_PORT);
	iocpServer.StartServer(MAX_CLIENT);

	printf("[알림] quit을 입력시 서버 종료합니다.\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	iocpServer.DestroyThread();
	printf("[알림] 서버가 종료되었습니다. 아무키나 누르면 창을 종료합니다.\n");
	getchar();
	return 0;
}

