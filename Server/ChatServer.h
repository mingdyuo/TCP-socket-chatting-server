#pragma once

#include "IOCPServer.h"

class ChatServer : public IOCPServer
{
public:
    virtual ~ChatServer(void){ WSACleanup(); }

    virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
        printf("[OnReceive] Ŭ���̾�Ʈ(%d) %d bytes\n", clientIndex_, size_);

        char* movedData = mClientMgr->MoveData(clientIndex_, size_);
        ProcessRecvPacket(clientIndex_, size_, movedData);
    }

    virtual void OnSend(const UINT32 clientIndex_, const UINT32 size_) {
        printf("[OnSend] Ŭ���̾�Ʈ(%d) %d bytes\n", clientIndex_, size_);
    }

    virtual void OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
        printf("[OnCreate] Ŭ���̾�Ʈ(%d) %d bytes\n", clientIndex_, size_);

        stClientInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);
        SERVER_ENTER_PACKET* packet = (SERVER_ENTER_PACKET*)pData_;
        client_->SetNickname(packet->Sender);

        printf("[�˸�] Ŭ���̾�Ʈ(%d) �г��� ���� : [%s]\n", clientIndex_, client_->GetNickname());
    }

    virtual void OnClose(int clientIndex_){
        printf("[OnClose] Ŭ���̾�Ʈ(%d) ���� ����\n", clientIndex_);
        mClientMgr->CloseClient(clientIndex_);
    }


private:
    void ProcessRecvPacket(const UINT32 clientIndex_, const UINT32 size_, char* pData_){
        PACKET_HEADER* header = (PACKET_HEADER*)pData_;

        if(CHAT_UNICAST == header->Type){
            UNICAST_PACKET* packet = (UNICAST_PACKET*)pData_;
            // TODO : �г������� Ŭ���̾�Ʈ ã�Ƽ� �����ϱ�
            // �г��� : packet->Recver
            return;
        }
        else if(CHAT_BROADCAST == header->Type){
            printf("CHAT_BROADCAST\n");
            mClientMgr->BroadCast(clientIndex_, size_, pData_);
            return;
        }
        else if(SERVER_EXIT == header->Type){
            printf("SERVER_EXIT\n");
            mClientMgr->MultiCast(clientIndex_, size_, pData_, CLOSE);
            return;
        }
        

        stClientInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);

        if(ROOM_ENTER == header->Type){
            ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)pData_;
            client_->EnterRoom(packet->RoomNo);
            mClientMgr->MultiCast(clientIndex_, size_, pData_);
        }
        else if(ROOM_EXIT == header->Type){
            mClientMgr->MultiCast(clientIndex_, size_, pData_, SELF);
            client_->ExitRoom();
        }
        else if(CHAT_MULTICAST == header->Type) {
            mClientMgr->MultiCast(clientIndex_, size_, pData_);
        }
        
    }

};