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
    mConnecterThread = (HANDLE)_beginthreadex(NULL, 0, StaticConnecterThread, this, 0, NULL);
    if (mConnecterThread == NULL) {
        printf("[����] createAccepterThread() �Լ� ����\n");
        return false;
    }

    mSenderThread = (HANDLE)_beginthreadex(NULL, 0, StaticSenderThread, this, 0, NULL);
    if (mSenderThread == NULL) {
        printf("[����] createSenderThread() �Լ� ����\n");
        return false;
    }

    HANDLE hThread;
    for (int i = 0;i < MAX_WORKERTHREAD;i++) {
        hThread = (HANDLE)_beginthreadex(NULL, 0, StaticWorkerThread, this, 0, NULL);
        if (hThread == NULL) {
            printf("[����] %d��° ������ ������ ���� �߻�\n", i);
            return false;
        }
        mIOWorkerThreads.push_back(hThread);
    }
    return true;
}


bool IOCPServer::DestroyThreads() {
    mbIsWorkerRun = false;
    CloseHandle(mIOCPHandle);

    for (size_t i = 0; i < mIOWorkerThreads.size();++i) {
        WaitForSingleObject(mIOWorkerThreads[i], INFINITE);
        CloseHandle(mIOWorkerThreads[i]);
    }

    mbIsConnecterRun = false;
    WaitForSingleObject(mSenderThread, INFINITE);
    CloseHandle(mSenderThread);
    //closesocket(mListenSocket);

    WaitForSingleObject(mConnecterThread, INFINITE);
    CloseHandle(mConnecterThread);
    return true;
}



