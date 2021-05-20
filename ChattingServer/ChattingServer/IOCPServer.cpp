#include "IOCPServer.h"
#include <process.h>

bool IOCPServer::InitSocket()
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[ERROR] WSA startup fail\n");
        exit(0);
    }

    listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

    if (listenSocket_ == INVALID_SOCKET)
    {
        printf("[ERROR] WSAsocket() function : %d\n", WSAGetLastError());
        return false;
    }

    printf("[SUCCESS] SOCKET INIIALIZATION\n");
    return true;
}

bool IOCPServer::BindAndListen(int nBindPort)
{
    SOCKADDR_IN stServerAddr;
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port = htons(nBindPort);
    stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (0 != bind(listenSocket_, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN)))
    {
        printf("[ERROR] bind() function : %d\n", WSAGetLastError());
        return false;
    }

    if (0 != listen(listenSocket_, kWaitQueueCnt))  // 접속 대기 큐 5개
    {
        printf("[ERROR] listen() function : %d\n", WSAGetLastError());
        return false;
    }

    printf("[SUECCESS] SERVER CONNECTED\n");
    return true;
}

bool IOCPServer::StartServer()
{

    IOCPHandle_ = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,
        NULL,
        NULL,
        kMaxWorkerThread
    );

    if (IOCPHandle_ == NULL)
    {
        printf("[ERROR] CreateIoCompletionPort() function : %d\n", GetLastError());
        return false;
    }

    if (false == CreateThreads())
        return false;

    system("cls");
    printf("[SUECCESS] Server Running . . .\n");
    return true;
}

bool IOCPServer::CreateThreads() {
    accepterThread_ = std::thread(&IOCPServer::AccepterThread, this);
    printf("[INFO] Accepter Thread CREATED\n");

    for (int i = 0; i < kMaxWorkerThread; i++)
    {
        workerThreads_.emplace_back(std::thread(&IOCPServer::WorkerThread, this));
    }
    printf("[INFO] WorkerThread CREATED\n");
    return true;
}


bool IOCPServer::DestroyThreads()
{
    bIsWorkerRun_ = false;

    for (size_t i = 0; IOCPHandle_ != NULL && i < workerThreads_.size(); ++i)
    {
        PostQueuedCompletionStatus(IOCPHandle_, 0, NULL, NULL);
    }

    for (size_t i = 0; i < workerThreads_.size(); ++i)
    {
        workerThreads_[i].join();
    }

    bIsAccepterRun_ = false;
    closesocket(listenSocket_);
    if (IOCPHandle_ != NULL)
    {
        CloseHandle(IOCPHandle_);
    }
    accepterThread_.join();

    return true;
}



