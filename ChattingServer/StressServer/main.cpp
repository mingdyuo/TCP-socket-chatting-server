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
		ErrorExit("서버 초기화 실패\n");
	}

	if (false == stressServer.StartServer()) 
	{
		ErrorExit("IOCP 서버 시작 실패\n");
	}

	printf("[알림] Enter 입력시 스트레스 서버 시작합니다.\n");
	printf("[알림] quit을 입력시 스트레스 서버 종료합니다.\n");
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
	printf("[알림] 서버가 종료되었습니다. 엔터키를 누르면 창을 종료합니다.\n");
	char c = getchar();
	c = getchar();
	return 0;
}

