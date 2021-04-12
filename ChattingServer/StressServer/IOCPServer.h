#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <vector>
#include <cstring>
#include "Packet.h"
#include <thread>

using std::vector;
using std::thread;

#ifndef _IOCP_SERVER
#define _IOCP_SERVER

class IOCPServer
{
public:

    IOCPServer(void) :
        mIOCPHandle(INVALID_HANDLE_VALUE),
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
    HANDLE                  mIOCPHandle;

    thread                  mConnecterThread;
    thread                  mSenderThread;
    vector<thread>          mIOWorkerThreads;

    bool                    mbIsWorkerRun;
    bool                    mbIsConnecterRun;
    bool                    mbIsSenderRun;

private:
    static const int        WAIT_QUEUE_CNT = 5;
    static const int        MAX_WORKERTHREAD = 14;

};

#endif