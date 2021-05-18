#pragma once
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <vector>
#include <cstring>
#include <thread>


#ifndef _IOCP_SERVER
#define _IOCP_SERVER

class IOCPServer
{
public:

    IOCPServer(void) :
        listenSocket_(INVALID_SOCKET), IOCPHandle_(INVALID_HANDLE_VALUE),
        bIsWorkerRun_(true), bIsAccepterRun_(true) {}

    virtual ~IOCPServer(void) { WSACleanup(); }

    bool            InitSocket();
    bool            BindAndListen(int nBindPort);
    bool            StartServer();

    bool            CreateThreads();
    bool            DestroyThreads();

    virtual unsigned AccepterThread() { return 0; }
    virtual unsigned WorkerThread() { return 0; }

protected:
    SOCKET                  listenSocket_;

    HANDLE                  IOCPHandle_;
    std::thread             accepterThread_;
    std::vector<std::thread> workerThreads_;


    bool                    bIsWorkerRun_;
    bool                    bIsAccepterRun_;

private:
    static const int        kWaitQueueCnt = 5;
    static const int        kMaxWorkerThread = 10;

};

#endif