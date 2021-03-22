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

    for(size_t i = 0;i<mIOWorkerThreads.size();++i){
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
        mIOWorkerThreads.push_back(hThread);
    }

    printf("WorkerThread ���� �Ϸ�\n");
    return true;
}

bool IOCPServer::createAccepterThread(){
    unsigned long dwThreadId;
    mAccepterThread = CreateThread(NULL, 0, StaticAccepterThread, (void*) this, 0, &dwThreadId);
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

