#pragma once
#include "IOCPServer.h"
#include <process.h>

bool IOCPServer::InitSocket() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }

    return true;
}


bool IOCPServer::StartServer() {

    mIOCPHandle = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,
        NULL,
        NULL,
        MAX_WORKERTHREAD
    );

    printf("[����] IOCP ����, �ִ� ��� ������ %d��\n", MAX_WORKERTHREAD);

    if (mIOCPHandle == NULL) {
        printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
        return false;
    }

    if (false == CreateThreads()) {
        return false;
    }

    system("cls");

    printf("���� ����\n");

    return true;
}

bool IOCPServer::CreateThreads() {
    mConnecterThread = thread(&IOCPServer::ConnecterThread, this);

    mSenderThread = thread(&IOCPServer::SenderThread, this);

    for (int i = 0;i < MAX_WORKERTHREAD;i++) {
        mIOWorkerThreads.push_back(thread(&IOCPServer::WorkerThread, this));
    }
    return true;
}


bool IOCPServer::DestroyThreads() {
    CloseHandle(mIOCPHandle);

    mbIsWorkerRun = false;
    for (size_t i = 0; i < mIOWorkerThreads.size();++i) {
        mIOWorkerThreads[i].join();
    }

    mbIsSenderRun = false;
    mSenderThread.join();

    mbIsConnecterRun = false;
    mConnecterThread.join();
    
    return true;
}



