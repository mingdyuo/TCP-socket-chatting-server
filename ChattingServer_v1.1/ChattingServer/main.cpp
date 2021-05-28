#include "ChatServer.h"
#include "LogicProcess.h"
#include "UserManager.h"
#include "RoomManager.h"
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

	std::unique_ptr<LogicProcess>	logicProcess	= std::make_unique<LogicProcess>();
	std::unique_ptr<SendServer>		sendServer		= std::make_unique<SendServer>();
	std::unique_ptr<UserManager>	userManager		= std::make_unique<UserManager>();
	std::unique_ptr<RoomManager>	roomManager		= std::make_unique<RoomManager>();


	userManager->SetMgr(roomManager.get(), sendServer.get());
	roomManager->SetMgr(userManager.get(), sendServer.get());
	logicProcess->SetMgr(roomManager.get(), userManager.get(), sendServer.get());

	std::unique_ptr<ChatServer> chatServer = std::make_unique<ChatServer>(logicProcess.get());

	if(false == chatServer->Initialize(SERVER_PORT))
	{
		ErrorExit("[CLOSED] SERVER INITIALIZE FAIL\n");
	}

	if(false == chatServer->RunServer())
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

	chatServer->CloseServer();
	logicProcess->Close();

	printf("[INFO] SERVER CLOSED. PRESS ENTER TO EXIT.\n");
	getchar();

	return 0;
}


