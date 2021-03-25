#pragma once
#pragma comment(lib, "ws2_32")
// #include <winsock2.h>
// #include <Ws2tcpip.h>
#include "ClientInfo.h"
#include <vector>
#include <cstring>
#include "Packet.h"
#include "ClientManager.h"

class IOCPServer
{
public:

    IOCPServer(void):
    mListenSocket(INVALID_SOCKET), mIOCPHandle(INVALID_HANDLE_VALUE),
    mbIsWorkerRun(true), mbIsAccepterRun(true), mClientMgr(NULL){}

    virtual ~IOCPServer(void){ WSACleanup(); }
    
    virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {}
    virtual void OnSend(const UINT32 clientIndex_, const UINT32 size_) {}
    virtual void OnCreate(const UINT32 clientIndex_, const UINT32 size_, char* pData_) {}
    virtual void OnClose(int clientIndex_){}

    bool InitSocket();
    bool BindAndListen(int nBindPort);    
    bool StartServer(const UINT32 maxClientCount);

    bool CreateThreads();
    bool DestroyThreads();
    virtual DWORD AccepterThread();
    virtual DWORD WorkerThread();

protected:
    ClientManager*          mClientMgr;

private:
    SOCKET                  mListenSocket;

    HANDLE                  mIOCPHandle;
    HANDLE                  mAccepterThread;
    std::vector<HANDLE>     mIOWorkerThreads;

    bool                    mbIsWorkerRun;
    bool                    mbIsAccepterRun;

    static const int        WAIT_QUEUE_CNT = 5;
    static const int        MAX_WORKERTHREAD = 16;


    static DWORD WINAPI StaticAccepterThread(void* arg){
        IOCPServer* This = (IOCPServer*) arg;
        return This->AccepterThread();
    };

    static DWORD WINAPI StaticWorkerThread(LPVOID arg){
        IOCPServer* This = (IOCPServer*) arg;
        return This->WorkerThread();
    }
};