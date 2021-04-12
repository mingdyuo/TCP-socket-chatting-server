#pragma once
#include "IOCPServer.h"
#include <process.h>

bool IOCPServer::InitSocket() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
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

    printf("[성공] IOCP 생성, 최대 사용 스레드 %d개\n", MAX_WORKERTHREAD);

    if (mIOCPHandle == NULL) {
        printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
        return false;
    }

    if (false == CreateThreads()) {
        return false;
    }

    system("cls");

    printf("서버 시작\n");

    return true;
}

bool IOCPServer::CreateThreads() {
    mConnecterThread = (HANDLE)_beginthreadex(NULL, 0, StaticConnecterThread, this, 0, NULL);
    if (mConnecterThread == NULL) {
        printf("[에러] createAccepterThread() 함수 실패\n");
        return false;
    }

    mSenderThread = (HANDLE)_beginthreadex(NULL, 0, StaticSenderThread, this, 0, NULL);
    if (mSenderThread == NULL) {
        printf("[에러] createSenderThread() 함수 실패\n");
        return false;
    }

    HANDLE hThread;
    for (int i = 0;i < MAX_WORKERTHREAD;i++) {
        hThread = (HANDLE)_beginthreadex(NULL, 0, StaticWorkerThread, this, 0, NULL);
        if (hThread == NULL) {
            printf("[에러] %d번째 스레드 생성중 오류 발생\n", i);
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



