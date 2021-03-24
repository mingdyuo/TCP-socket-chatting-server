#pragma once

#include "IOCPServer.h"
#include <mutex>

class ChatServer : public IOCPServer
{
public:
    virtual ~ChatServer(void){ WSACleanup(); }

    virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
        printf("[OnReceive] 클라이언트(%d) %d bytes\n", clientIndex_, size_);

        char* movedData = mClientMgr->MoveData(clientIndex_, size_);
        ProcessRecvPacket(clientIndex_, size_, movedData);
    }

    virtual void OnSend(const UINT32 clientIndex_, const UINT32 size_) {
        printf("[OnSend] 클라이언트(%d) %d bytes\n", clientIndex_, size_);
    }

    virtual void OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
        printf("[OnReceive] 클라이언트(%d) %d bytes\n", clientIndex_, size_);

        stClientInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);
        SERVER_ENTER_PACKET* packet = (SERVER_ENTER_PACKET*)pData_;
        client_->SetNickname(packet->Sender);
        mClientMgr->BroadCast(clientIndex_, size_, pData_);
    }


private:
    void ProcessRecvPacket(const UINT32 clientIndex_, const UINT32 size_, char* pData_){
        PACKET_HEADER* header = (PACKET_HEADER*)pData_;

        if(header->Type == CHAT_UNICAST){
            UNICAST_PACKET* packet = (UNICAST_PACKET*)pData_;
            // TODO : 닉네임으로 클라이언트 찾아서 전송하기
            // 닉네임 : packet->Recver

            return;
        }
        else if(header->Type == CHAT_BROADCAST){
            mClientMgr->BroadCast(clientIndex_, size_, pData_);
            return;
        }
        
        if(ROOM_ENTER == header->Type){
            stClientInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);
            ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)pData_;
            client_->EnterRoom(packet->RoomNo);
        }
        else if(ROOM_EXIT == header->Type){
            stClientInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);
            ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)pData_;
            client_->ExitRoom();
        }
        
        mClientMgr->MultiCast(clientIndex_, size_, pData_);

        if(SERVER_EXIT == header->Type){
            mClientMgr->CloseClient(clientIndex_);
        }
        
    }

};