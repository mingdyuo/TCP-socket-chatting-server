#include "IOCPServer.h"
#include <process.h>

bool IOCPServer::InitSocket(){
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
        return false;
    }

    mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

    if(mListenSocket == INVALID_SOCKET){
        printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
        return false;
    }

    printf("소켓 초기화 성공\n");
    return true;
}

bool IOCPServer::BindAndListen(int nBindPort){
    SOCKADDR_IN stServerAddr;
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port = htons(nBindPort);
    stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(0 != bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN))){
        printf("[에러] bind()함수 실패 : %d\n", WSAGetLastError());
        return false;
    }

    if(0 != listen(mListenSocket, WAIT_QUEUE_CNT)){ // 접속 대기 큐 5개
        printf("[에러] listen()함수 실패 : %d\n", WSAGetLastError());
		return false;
    }

    printf("서버 등록 성공..\n");
	return true;
}
  
bool IOCPServer::StartServer(){

    mIOCPHandle = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,
        NULL,
        NULL,
        MAX_WORKERTHREAD
    );

    printf("[성공] IOCP 생성, 최대 사용 스레드 %d개\n", MAX_WORKERTHREAD);

    if(mIOCPHandle == NULL){
        printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
		return false;
    }

    if(false == CreateThreads()){
        return false;
    }

    printf("서버 시작\n");
    return true;
}

bool IOCPServer::CreateThreads(){
    mAccepterThread = thread(&IOCPServer::AccepterThread, this);
    printf("[알림] Accepter Thread 생성 완료\n");

    for(int i=0;i<MAX_WORKERTHREAD;i++){
        mIOWorkerThreads.push_back(thread(&IOCPServer::WorkerThread, this));
    }
    printf("[알림] WorkerThread 생성 완료\n");
    return true;
}


bool IOCPServer::DestroyThreads(){
    mbIsWorkerRun = false;
    CloseHandle(mIOCPHandle);

    for(size_t i = 0; i < mIOWorkerThreads.size();++i){
        mIOWorkerThreads[i].join();
    }

    mbIsAccepterRun = false;
    closesocket(mListenSocket);

    mAccepterThread.join();
    return true;
}



