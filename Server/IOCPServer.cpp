#include "IOCPServer.h"

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

    mClientMgr  = new ClientManager(maxClientCount);

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

    if(false == CreateThreads()){
        return false;
    }

    printf("���� ����\n");
    return true;
}

bool IOCPServer::CreateThreads(){
    HANDLE mAccepterThread = CreateThread(NULL, 0, StaticAccepterThread, (void*) this, 0, NULL);
    if(mAccepterThread == NULL){
        printf("[����] createAccepterThread() �Լ� ����");
    }

    HANDLE hThread;
    printf("[�˸�] Accepter Thread ���� �Ϸ�\n");
    for(int i=0;i<MAX_WORKERTHREAD;i++){
        hThread = CreateThread(NULL, 0, StaticWorkerThread, (void*) this, 0, NULL);
        if(hThread==NULL) {
            printf("[����] %d��° ������ ������ ���� �߻�\n", i);
            return false;
        }
        mIOWorkerThreads.push_back(hThread);
    }
    printf("[�˸�] WorkerThread ���� �Ϸ�\n");
    return hThread;
}


// TODO
bool IOCPServer::DestroyThreads(){
    mbIsWorkerRun = false;
    CloseHandle(mIOCPHandle);

    for(size_t i = 0; i < mIOWorkerThreads.size();++i){
        WaitForSingleObject(mIOWorkerThreads[i], INFINITE);
    }

    mbIsAccepterRun = false;
    closesocket(mListenSocket);

    WaitForSingleObject(mAccepterThread, INFINITE);
}

DWORD IOCPServer::AccepterThread(){
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
            pClientInfo->BindRecv();
        }
        else if(ACCEPT == pOverlappedEx->m_eOperation){
            OnCreate(pClientInfo->GetIndex(), dwIoSize, pClientInfo->RecvBuffer());
            pClientInfo->BindRecv();
        }

    }
    printf("[�˸�] Worker thread ����\n");
    return dwResult;
}

