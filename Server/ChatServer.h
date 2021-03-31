#pragma once

#include "IOCPServer.h"
#include "ChatClientManager.h"
#include "UserInfo.h"
#include "PacketManager.h"
#include <process.h>

class ChatServer : public IOCPServer
{
public:
    ChatServer(){ }
    virtual ~ChatServer(void){ WSACleanup(); }

    virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_);
    virtual void OnSend(const UINT32 clientIndex_, const UINT32 size_);
    virtual void OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_);
    virtual void OnClose(int clientIndex_);

    virtual unsigned AccepterThread();
    virtual unsigned WorkerThread();

    virtual void SetClientInfos(const UINT32 maxClientCount){
        mClientMgr = new ChatClientManager<stUserInfo>(maxClientCount);
        mPacketMgr = new PacketManager<stUserInfo>(mClientMgr);
    }

    bool Run(int maxClient){    //< Function called in main
        SetClientInfos(maxClient);

        if(false == mPacketMgr->PacketThread())
            return false;   
        
        printf("[알림] packetThread 생성 완료\n");
        
        return StartServer();   //< IOCP Server Run
    }

    void CloseServer(){
        DestroyThreads();
        mPacketMgr->Close();
    }


private:
    void ProcessRecvPacket(const UINT32 clientIndex_, const UINT32 size_, char* pData_);

    ChatClientManager<stUserInfo>*      mClientMgr;
    PacketManager<stUserInfo>*          mPacketMgr;
};

