#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "ClientInfo.h"
#include "IOCPServer.h"
#include <vector>
#include <cstdio>

bool IOCPServer::InitSocket(){
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }

    mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

    if(mListenSocket == INVALID_SOCKET){
        printf("[����] socket()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }

    printf("���� �ʱ�ȭ ����\n");
    return true;
}

bool IOCPServer::BindAndListen(int nBindPort){
    SOCKADDR_IN stServerAddr;
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port = htons(nBindPort);
    stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(0 != bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN))){
        printf("[����] bind()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }

    if(0 != listen(mListenSocket, WAIT_QUEUE_CNT)){ // ���� ��� ť 5��
        printf("[����] listen()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
    }

    printf("���� ��� ����..\n");
	return true;
}

bool IOCPServer::StartServer(const UINT32 maxClientCount){

    createClient(maxClientCount);
    mMaxClientCount = maxClientCount;

    mIOCPHandle = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,
        NULL,
        NULL,
        MAX_WORKERTHREAD
    );

    printf("[����] IOCP ����, �ִ� ��� ������ %d��\n", MAX_WORKERTHREAD);

    if(mIOCPHandle == NULL){
        printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
		return false;
    }

    if(false == createWorkerThread()){
        printf("[����] createWorkerThread() �Լ� ����");
        return false;
    }

    if(false == createAccepterThread()){
        printf("[����] createAccepterThread() �Լ� ����");
        return false;
    }

    printf("���� ����\n");
    return true;
}

void IOCPServer::DestroyThread(){
    mbIsWorkerRun = false;
    CloseHandle(mIOCPHandle);

    for(size_t i = 0; i < mIOWorkerThreads.size();++i){
        WaitForSingleObject(mIOWorkerThreads[i], INFINITE);
    }

    mbIsAccepterRun = false;
    closesocket(mListenSocket);

    WaitForSingleObject(mAccepterThread, INFINITE);
    
}

void IOCPServer::createClient(const UINT32 maxClientCount){
    for(UINT32 i=0;i<maxClientCount;++i){
        mClientInfos.push_back(stClientInfo(i));
    }
}

bool IOCPServer::createWorkerThread(){
    
    HANDLE hThread;
    unsigned long dwThreadId;

    for(int i=0;i<MAX_WORKERTHREAD;i++){
        hThread = CreateThread(NULL, 0, StaticWorkerThread, this, 0, &dwThreadId);
        if(hThread==NULL) return false;
        mIOWorkerThreads.push_back(hThread);
    }

    printf("WorkerThread ���� �Ϸ�\n");
    return true;
}

bool IOCPServer::createAccepterThread(){
    unsigned long dwThreadId;
    mAccepterThread = CreateThread(NULL, 0, StaticAccepterThread, (void*) this, 0, &dwThreadId);
    if(mAccepterThread == NULL) return false;
    CloseHandle(mAccepterThread);

    printf("Accepter Thread ����\n");
    return true;
}

size_t IOCPServer::getEmptyClientInfo(){
    for(size_t i = 0;i<mMaxClientCount;i++){
        if(mClientInfos[i].IsConnected() == false)
            return i;
    }
    return mMaxClientCount;
}

stClientInfo* IOCPServer::getClientInfo(const UINT32 sessionIndex){
    return &mClientInfos[sessionIndex];
}

void IOCPServer::CloseSocket(stClientInfo* pClientInfo, bool bIsForce){
    size_t clientindex = pClientInfo->GetIndex();
    pClientInfo->Close(bIsForce);
    // onClose(clientIndex);
}

DWORD IOCPServer::AccepterThread(){
    DWORD dwResult = 0;
    SOCKADDR_IN stClientAddr;
    int nAddrLen = sizeof(SOCKADDR_IN);

    while(mbIsAccepterRun){
        size_t pClientIndex = getEmptyClientInfo();

        if(mMaxClientCount == pClientIndex){
            printf("[����] Client Full\n");
            return dwResult;
        }

        SOCKET newSocket = accept(
            mListenSocket,
            (SOCKADDR*)&stClientAddr,
            &nAddrLen
        );

        if(INVALID_SOCKET == newSocket)
            continue;

        stClientInfo* pClientInfo = &mClientInfos[pClientIndex];

        if(0 == pClientInfo->OnConnect(mIOCPHandle, newSocket)){
            pClientInfo->Close(true);
            return dwResult;
        }

        ++mClientCnt;
        OnConnect(pClientInfo->mIndex);

    }
    printf("[�˸�] Accepter thread ����\n");
    return dwResult;
};

DWORD IOCPServer::WorkerThread(){
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
            CloseSocket(pClientInfo);
            continue;
        }

        stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

        if(RECV == pOverlappedEx->m_eOperation){
            OnReceive(pClientInfo->GetIndex(), dwIoSize);
            pClientInfo->mRecvBuf[dwIoSize] = '\0';
            
            pClientInfo->BindRecv();

            printf("-> %s: \"%s\"\n", pClientInfo->mNickname, pClientInfo->mRecvBuf);
            bool bProcessed = ActionProcess(dwIoSize, pClientInfo);

            if(!bProcessed)
                printf("Client Index(%d)���� �̻� �׼� �߰�\n", pClientInfo->GetIndex());

        }
        else if(SEND == pOverlappedEx->m_eOperation){
            delete[] pOverlappedEx->m_wsaBuf.buf;
            delete pOverlappedEx;
            pClientInfo->SendCompleted(dwIoSize);
        }
        else{
            printf("Client Index(%d)���� ���ܻ�Ȳ\n", pClientInfo->GetIndex());
        }
    }
    printf("[�˸�] Worker thread ����\n");
    return dwResult;
}



bool IOCPServer::ActionProcess(DWORD dwIoSize, stClientInfo* senderClient){
        
    Parser parser = Parser(senderClient->mRecvBuf);

    switch(parser.mAction){
        case CHAT_MULTICAST:
            MultiCast(dwIoSize, senderClient);
            break;
        case CHAT_UNICAST:
            UniCast(&parser, senderClient);
            break;
        case CHAT_BROADCAST:
        case ROOM_ENTER:
        case ROOM_EXIT: 
            BroadCast(dwIoSize, senderClient);
            break;
        case SERVER_ENTER:
            if(senderClient->mbHasNick) {
                printf("[����] �г����� �̹� ���õǾ� ����\n");
                return false;
            }
            senderClient->setNickname(dwIoSize);
            BroadCast(dwIoSize, senderClient);
            break;
        case SERVER_EXIT:
            MultiCast(dwIoSize, senderClient);
            senderClient->Close();
            break;
        default:
            return false;
    }
    return true;
}

size_t IOCPServer::GetNicknameIndex(char* nickname){
    for(size_t i = 0;i<mClientCnt;i++){
        if(0 == strcmp(nickname, mClientInfos[i].mNickname))
            return i;
    }
    return mMaxClientCount;
}

void IOCPServer::BroadCast(DWORD dwIoSize, stClientInfo* senderClient){
    size_t senderIndex = senderClient->GetIndex();
    std::vector<size_t> closedClient;
    for(size_t i = 0;i<mClientCnt;i++){
        if(i == senderIndex || mClientInfos[i].IsConnected() == false) continue;
        bool bSuccess = mClientInfos[i].SendMsg(dwIoSize, senderClient->mRecvBuf);
        if(!bSuccess) closedClient.push_back(i);
    }

    for(size_t i=0;i<closedClient.size();++i){
        mClientInfos[closedClient[i]].Close();
        --mClientCnt;
    }
};

void IOCPServer::UniCast(Parser* parser, stClientInfo* senderClient){
    char nickname[32];
    parser->GetClient(nickname);

    size_t recvIndex = GetNicknameIndex(nickname);
    if(recvIndex == mMaxClientCount){
        // �׷� �г��� ����~
        bool bSuccess = senderClient->SendMsg(sizeof(notFoundUser), notFoundUser);
        if(!bSuccess) mClientInfos[recvIndex].Close();
        return;
    }
    int msgLen = parser->SetSender(senderClient->mNickname);

    bool bSuccess = mClientInfos[recvIndex].SendMsg(msgLen, senderClient->mRecvBuf);
    if(!bSuccess) mClientInfos[recvIndex].Close();
}

void IOCPServer::MultiCast(DWORD dwIoSize, stClientInfo* senderClient){
    size_t senderIndex = senderClient->GetIndex();
    std::vector<size_t> closedClient;
    for(size_t i = 0;i<mMaxClientCount;i++){
        if(i == senderIndex || mClientInfos[i].IsConnected() == false || 
            mClientInfos[i].mRoom != senderClient->mRoom) continue;

        bool bSuccess = mClientInfos[i].SendMsg(dwIoSize, senderClient->mRecvBuf);
        if(!bSuccess) closedClient.push_back(i);
    }

    for(size_t i=0;i<closedClient.size();++i){
        mClientInfos[closedClient[i]].Close();
        --mClientCnt;
    }
}