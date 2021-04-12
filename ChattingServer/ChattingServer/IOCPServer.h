#pragma once
#pragma comment(lib, "ws2_32")
#include "ClientInfo.h"
#include <vector>
#include <cstring>
#include <thread>
#include "Packet.h"

using std::thread;
using std::vector;


#ifndef _IOCP_SERVER
#define _IOCP_SERVER

class IOCPServer
{
public:

    IOCPServer(void):
    mListenSocket(INVALID_SOCKET), mIOCPHandle(INVALID_HANDLE_VALUE),
    mbIsWorkerRun(true), mbIsAccepterRun(true){}

    virtual ~IOCPServer(void){ WSACleanup(); }
    
    virtual void    OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {}
    virtual void    OnSend(const UINT32 clientIndex_, const UINT32 size_) {}
    virtual void    OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {}
    virtual void    OnClose(int clientIndex_){}

    bool            InitSocket();
    bool            BindAndListen(int nBindPort);    
    bool            StartServer();
    virtual void    InitializeManagers(const UINT32 maxClientCount){}

    bool            CreateThreads();
    bool            DestroyThreads();

    virtual unsigned AccepterThread(){ return 0;}
    virtual unsigned WorkerThread(){ return 0;}

protected:
    SOCKET                  mListenSocket;

    HANDLE                  mIOCPHandle;
    thread                  mAccepterThread;
    vector<thread>          mIOWorkerThreads;


    bool                    mbIsWorkerRun;
    bool                    mbIsAccepterRun;

private:
    static const int        WAIT_QUEUE_CNT = 5;
    static const int        MAX_WORKERTHREAD = 14;

};

#endif