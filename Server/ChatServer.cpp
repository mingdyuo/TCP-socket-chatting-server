#include "ChatServer.h"
#include "PacketInfo.h"

void ChatServer::OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {

    PacketInfo packetInfo;
    packetInfo.ClientIndex = clientIndex_;
    packetInfo.DataSize = size_;
    packetInfo.pPacketData = pData_;

    printf("[OnReceive] 클라이언트(%d) %d bytes\n", packetInfo.ClientIndex, packetInfo.DataSize);
    //< packetMgr->class(pData_);
    mPacketMgr->ClassifyPacket(packetInfo);

    // ProcessRecvPacket(clientIndex_, size_, movedData);
}

void ChatServer::OnSend(const UINT32 clientIndex_, const UINT32 size_) {
    printf("[OnSend] 클라이언트(%d) %d bytes\n", clientIndex_, size_);
}

void ChatServer::OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
    stUserInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);
    SERVER_ENTER_PACKET* packet = (SERVER_ENTER_PACKET*)pData_;
    client_->SetNickname(packet->Sender);

    printf("[OnCreate] 클라이언트(%d) 닉네임 생성 : [%s]\n", clientIndex_, client_->GetNickname());
}

void ChatServer::OnClose(int clientIndex_){
    printf("[OnClose] 클라이언트(%d) 연결 종료\n", clientIndex_);
    mClientMgr->CloseClient(clientIndex_);
}


void ChatServer::ProcessRecvPacket(const UINT32 clientIndex_, const UINT32 size_, char* pData_){
    PACKET_HEADER* header = (PACKET_HEADER*)pData_;

    // if(SERVER_ENTER == header->Type){

    // }

    // else if(CHAT_UNICAST == header->Type){
    //     
    // }
    // else if(CHAT_BROADCAST == header->Type){

    // }
    // else if(SERVER_EXIT == header->Type){
    // }
    

    // stUserInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);

    // if(ROOM_ENTER == header->Type){
    //     ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)pData_;
    //     client_->EnterRoom(packet->RoomNo);
    //     mClientMgr->MultiCast(clientIndex_, size_, pData_);
    // }
    // else if(ROOM_EXIT == header->Type){
    //     mClientMgr->MultiCast(clientIndex_, size_, pData_);
    //     client_->ExitRoom();
    // }
    // else if(CHAT_MULTICAST == header->Type) {
    // }
    
}


unsigned ChatServer::AccepterThread(){
    unsigned uResult = 0;
    SOCKADDR_IN stClientAddr;
    int nAddrLen = sizeof(SOCKADDR_IN);
    while(mbIsAccepterRun){
        SOCKET newSocket = accept(
            mListenSocket,
            (SOCKADDR*)&stClientAddr,
            &nAddrLen
        );

        if(INVALID_SOCKET == newSocket)
            continue;

        if(false == mClientMgr->CreateClient(mIOCPHandle, newSocket)){
            closesocket(newSocket);
            continue;
        }

    }
    printf("[알림] Accepter thread 종료\n");
    return uResult;
}

unsigned ChatServer::WorkerThread(){
    unsigned uResult = 0;
    DWORD dwIoSize = 0;
    BOOL bSuccess = TRUE;
    stClientInfo* pClientInfo = NULL;
    LPOVERLAPPED lpOverlapped = NULL;

    while(mbIsWorkerRun){
        bSuccess = GetQueuedCompletionStatus(
            mIOCPHandle,
            &dwIoSize,
            (PULONG_PTR)&pClientInfo,
            &lpOverlapped,
            INFINITE
        );

        if(bSuccess==TRUE && dwIoSize==0 && lpOverlapped==NULL){
            mbIsWorkerRun = false;
            continue;
        }

        if(lpOverlapped==NULL) continue;

        if(bSuccess == FALSE || (0 == dwIoSize && bSuccess == TRUE)){
            mClientMgr->CloseClient(pClientInfo->GetIndex());
            continue;
        }

        stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

        if(SEND == pOverlappedEx->m_eOperation){
            OnSend(pClientInfo->GetIndex(), dwIoSize);
            delete[] pOverlappedEx->m_wsaBuf.buf;
            delete pOverlappedEx;
        }
        else if(RECV == pOverlappedEx->m_eOperation){
            OnReceive(pClientInfo->GetIndex(), dwIoSize, pClientInfo->RecvBuffer());
            pClientInfo->BindRecv();
        }
        else if(CLOSE == pOverlappedEx->m_eOperation){
            OnClose(pClientInfo->GetIndex());
        }

    }
    printf("[알림] Worker thread 종료\n");
    return uResult;
}

