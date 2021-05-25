#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "IOCPServer.h"

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

    bool RunServer();
    void CloseServer();



private:
    LogicProcess* logicProcess_;
};



#endif // !CHAT_SERVER_H

