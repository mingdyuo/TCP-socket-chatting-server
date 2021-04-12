#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <vector>
#include <cstring>
#include "Packet.h"

#ifndef _IOCP_SERVER
#define _IOCP_SERVER

class IOCPServer
{
public:

    IOCPServer(void) :
        mListenSocket(INVALID_SOCKET), mIOCPHandle(INVALID_HANDLE_VALUE),
        mbIsWorkerRun(true), mbIsConnecterRun(true), mbIsSenderRun(true) {}

    virtual ~IOCPServer(void) { WSACleanup(); }

    virtual void    OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {}
    virtual void    OnSend(const UINT32 clientIndex_, const UINT32 size_) {}
    virtual void    OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {}

    bool            InitSocket();
    bool            BindAndListen(int nBindPort);
    bool            StartServer();

    bool            CreateThreads();
    bool            DestroyThreads();

    virtual unsigned ConnecterThread() { return 0; }
    virtual unsigned WorkerThread() { return 0; }
    virtual unsigned SenderThread() { return 0; }

protected:
    SOCKET                  mListenSocket;

    HANDLE                  mIOCPHandle;
    HANDLE                  mConnecterThread;
    HANDLE                  mSenderThread;
    std::vector<HANDLE>     mIOWorkerThreads;

    bool                    mbIsWorkerRun;
    bool                    mbIsConnecterRun;
    bool                    mbIsSenderRun;

private:
    static const int        WAIT_QUEUE_CNT = 5;
    static const int        MAX_WORKERTHREAD = 14;

    static unsigned __stdcall StaticConnecterThread(void* arg) {
        IOCPServer* This = (IOCPServer*)arg;
        return This->ConnecterThread();
    };

    static unsigned __stdcall StaticWorkerThread(void* arg) {
        IOCPServer* This = (IOCPServer*)arg;
        return This->WorkerThread();
    }

    static unsigned __stdcall StaticSenderThread(void* arg) {
        IOCPServer* This = (IOCPServer*)arg;
        return This->SenderThread();
    }
};

#endif