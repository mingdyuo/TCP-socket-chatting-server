#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "../NetworkLib/IOCPServer.h"

class LogicProcess;

class ChatServer : public IOCPServer
{
public:
    ChatServer() = delete;

    ChatServer(LogicProcess* logicProcess) :logicProcess_(logicProcess) {}

    virtual ~ChatServer(void)
    {}

    virtual unsigned AccepterThread() override;
    virtual unsigned WorkerThread() override;


    bool Initialize(int serverPort)
    {
        if (false == InitSocket())
            return false;

        if (false == BindAndListen(serverPort))
            return false;

        return true;
    }

    bool Run()    //< Function called in main
    {
        //< 다른 초기화 해줘야 하는 작업들 있다면 실행 


        return StartServer();   //< IOCP Server Run
    }

    void CloseServer()
    {
        // 다른 닫아야 하는 작업들 종료
        DestroyThreads();
    }


private:
    LogicProcess* logicProcess_;
};



#endif // !CHAT_SERVER_H

