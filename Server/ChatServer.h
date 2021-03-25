#pragma once

#include "IOCPServer.h"

class ChatServer : public IOCPServer
{
public:
    ChatServer(){}
    virtual ~ChatServer(void){ WSACleanup(); }

    virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_);
    virtual void OnSend(const UINT32 clientIndex_, const UINT32 size_);
    virtual void OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_);
    virtual void OnClose(int clientIndex_);

    virtual DWORD AccepterThread();
    virtual DWORD WorkerThread();

    virtual void SetClientInfos(const UINT32 maxClientCount){
        mClientMgr = new ClientManager(maxClientCount);
    }


private:
    void ProcessRecvPacket(const UINT32 clientIndex_, const UINT32 size_, char* pData_);

    ClientManager*          mClientMgr;

};