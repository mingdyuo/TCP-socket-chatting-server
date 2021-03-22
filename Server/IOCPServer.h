#pragma once
#pragma comment(lib, "ws2_32")
// #include <winsock2.h>
// #include <Ws2tcpip.h>
#include "ClientInfo.h"
#include <vector>
#include <cstring>
#include "Parser.h"

static char* notFoundUser = "31|해당 유저가 존재하지 않습니다.|시스템";

class IOCPServer
{
private:
    std::vector<stClientInfo> mClientInfos;
    SOCKET mListenSocket;
    size_t mClientCnt;
    static const int WAIT_QUEUE_CNT = 5;
    static const int MAX_WORKERTHREAD = 16;
    size_t mMaxClientCount;

    std::vector<HANDLE> mIOWorkerThreads;

    HANDLE mAccepterThread;
    HANDLE mIOCPHandle;

    bool mbIsWorkerRun;
    bool mbIsAccepterRun;

    char mSocketBuf[1024];
    
public:
    IOCPServer(void):
        mListenSocket(INVALID_SOCKET), mClientCnt(0), mIOCPHandle(INVALID_HANDLE_VALUE),
        mbIsWorkerRun(true), mbIsAccepterRun(true), mMaxClientCount(0){
            memset(mSocketBuf, 0, sizeof(mSocketBuf));
        }

    virtual ~IOCPServer(void){ WSACleanup(); }

    bool InitSocket();
    bool BindAndListen(int nBindPort);
    bool StartServer(const UINT32 maxClientCount);
    void DestroyThread();

    virtual void OnConnect(const UINT32 clientIndex_) {
        printf("[OnConnect] 클라이언트: Index(%d)\n", clientIndex_);
    }
	virtual void OnClose(const UINT32 clientIndex_) {
        printf("[OnClose] 클라이언트: Index(%d)\n", clientIndex_);
    }
	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_) {
        printf("[OnReceive] 클라이언트: Index(%d), dataSize(%d)\n", clientIndex_, size_);

    }

private:
    void createClient(const UINT32 maxClientCount);
    bool createWorkerThread();
    bool createAccepterThread();
    size_t getEmptyClientInfo();
    stClientInfo* getClientInfo(const UINT32 sessionIndex);
  
    void CloseSocket(stClientInfo* pClientInfo, bool bIsForce = false);
    DWORD AccepterThread();
    DWORD WorkerThread();

    size_t GetNicknameIndex(char* nickname);
    bool ActionProcess(DWORD dwIoSize, stClientInfo* senderClient);
    void BroadCast(DWORD dwIoSize, stClientInfo* senderClient);
    void UniCast(Parser* parser, stClientInfo* senderClient);
    void MultiCast(DWORD dwIoSize, stClientInfo* senderClient);

    stClientInfo* GetClientInfo(const UINT32 sessionIndex)
	{
		return &mClientInfos[sessionIndex];		
	}

    static DWORD WINAPI StaticAccepterThread(void* arg){
        IOCPServer* This = (IOCPServer*) arg;
        return This->AccepterThread();
    };

    static DWORD WINAPI StaticWorkerThread(LPVOID arg){
        IOCPServer* This = (IOCPServer*) arg;
        return This->WorkerThread();
    }
};
