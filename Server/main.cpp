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
		ErrorExit("서버 초기화 실패\n");
	}

	if(false == chatServer.Run(MAX_CLIENT)){
		ErrorExit("IOCP 서버 시작 실패\n");
	}

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

	chatServer.CloseServer();
	printf("[알림] 서버가 종료되었습니다. 엔터키를 누르면 창을 종료합니다.\n");
	getchar();
	return 0;
}

