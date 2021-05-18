#include "ChatServer.h"
#include "LogicProcess.h"
#include <string>
#include <iostream>
#include <memory>

void ErrorExit(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

int main()
{
	const int SERVER_PORT 	= 9898;
	const int MAX_CLIENT 	= 20000;

	std::unique_ptr<LogicProcess> logicProcess = std::make_unique<LogicProcess>();
	ChatServer chatServer(logicProcess.get());

	if(false == chatServer.Initialize(SERVER_PORT))
	{
		ErrorExit("[CLOSED] SERVER INITIALIZE FAIL\n");
	}

	if(false == chatServer.Run())
	{
		ErrorExit("[CLOSED] IOCP SERVER RUN FAIL\n");
	}

	printf("[INFO] quit을 입력시 서버 종료합니다.\n");
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
	printf("[INFO] SERVER CLOSED. PRESS ENTER TO EXIT.\n");
	getchar();
	return 0;
}

