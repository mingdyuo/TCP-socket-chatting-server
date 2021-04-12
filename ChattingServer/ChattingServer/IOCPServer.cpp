#include "IOCPServer.h"
#include <process.h>

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
  
bool IOCPServer::StartServer(){

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
    mAccepterThread = (HANDLE)_beginthreadex(NULL, 0, StaticAccepterThread, this, 0, NULL);
    if(mAccepterThread == NULL){
        printf("[����] createAccepterThread() �Լ� ����\n");
        return false;
    }

    HANDLE hThread;
    printf("[�˸�] Accepter Thread ���� �Ϸ�\n");
    for(int i=0;i<MAX_WORKERTHREAD;i++){
        hThread = (HANDLE)_beginthreadex(NULL, 0, StaticWorkerThread, this, 0, NULL);
        if(hThread==NULL) {
            printf("[����] %d��° ������ ������ ���� �߻�\n", i);
            return false;
        }
        mIOWorkerThreads.push_back(hThread);
    }
    printf("[�˸�] WorkerThread ���� �Ϸ�\n");
    return true;
}


bool IOCPServer::DestroyThreads(){
    mbIsWorkerRun = false;
    CloseHandle(mIOCPHandle);

    for(size_t i = 0; i < mIOWorkerThreads.size();++i){
        WaitForSingleObject(mIOWorkerThreads[i], INFINITE);
        CloseHandle(mIOWorkerThreads[i]);
    }

    mbIsAccepterRun = false;
    closesocket(mListenSocket);

    WaitForSingleObject(mAccepterThread, INFINITE);
    CloseHandle(mAccepterThread);
    return true;
}



