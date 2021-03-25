#include "ChatServer.h"

void ChatServer::OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
    printf("[OnReceive] Ŭ���̾�Ʈ(%d) %d bytes\n", clientIndex_, size_);

    char* movedData = mClientMgr->MoveData(clientIndex_, size_);
    ProcessRecvPacket(clientIndex_, size_, movedData);
}

void ChatServer::OnSend(const UINT32 clientIndex_, const UINT32 size_) {
    printf("[OnSend] Ŭ���̾�Ʈ(%d) %d bytes\n", clientIndex_, size_);
}

void ChatServer::OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {
    printf("[OnCreate] Ŭ���̾�Ʈ(%d) %d bytes\n", clientIndex_, size_);

    stClientInfo* client_ = mClientMgr->GetClientByIndex(clientIndex_);
    SERVER_ENTER_PACKET* packet = (SERVER_ENTER_PACKET*)pData_;
    client_->SetNickname(packet->Sender);

    printf("[�˸�] Ŭ���̾�Ʈ(%d) �г��� ���� : [%s]\n", clientIndex_, client_->GetNickname());
}

void ChatServer::OnClose(int clientIndex_){
    printf("[OnClose] Ŭ���̾�Ʈ(%d) ���� ����\n", clientIndex_);
    mClientMgr->CloseClient(clientIndex_);
}




DWORD ChatServer::AccepterThread(){
    DWORD dwResult = 0;
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
            return dwResult;
        }
    }
    printf("[�˸�] Accepter thread ����\n");
    return dwResult;
}

DWORD ChatServer::WorkerThread(){
    DWORD dwResult = 0;
    stClientInfo* pClientInfo = NULL;
    BOOL bSuccess = TRUE;
    DWORD dwIoSize = 0;
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
            mClientMgr->CloseClient(pClientInfo);
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
            pClientInfo->BindRecv(RECV);
        }
        else if(ACCEPT == pOverlappedEx->m_eOperation){
            OnCreate(pClientInfo->GetIndex(), dwIoSize, pClientInfo->RecvBuffer());
            pClientInfo->BindRecv(RECV);
        }
        else if(CLOSE == pOverlappedEx->m_eOperation){
            OnClose(pClientInfo->GetIndex());
        }

    }
    printf("[�˸�] Worker thread ����\n");
    return dwResult;
}

