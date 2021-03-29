#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <cstdio>
#include <iostream>
#include "Position.h"
#include "Packet.h"
#include "CriticalSection.h"


class IOCPClient{
private:
    static const int MAX_NICKNAME_LEN = 32;
    SOCKET mSocket;
    bool mbIsWorkerRun;
    char mNickname[MAX_NICKNAME_LEN];
    int mRoom;

    Position pos;
    CriticalSection mMutex;

public:
    IOCPClient():mSocket(INVALID_SOCKET), mbIsWorkerRun(true), mRoom(0) {}
    ~IOCPClient(){WSACleanup();}

    bool OnConnect(){return mbIsWorkerRun;}
    char* getNickname(){return mNickname;}

    bool InitSocket();
    bool ConnectServer(int bBindPort);
    bool SetNickname();
    bool CreateThreads(HANDLE* sender, HANDLE* recver);
    bool Close();

    bool Lobby();
    int NicknameCheck(const char* nickname);

    DWORD RecvThread();
    DWORD SendThread();

    void processRecvMsg(char* received, char* content, char* sender);
    eAction processSendMsg(std::string& content);

    static DWORD WINAPI StaticRecvThread(LPVOID arg){
        IOCPClient* This = (IOCPClient*)arg;
        return This->RecvThread();
    }

    static DWORD WINAPI StaticSendThread(LPVOID arg){
        IOCPClient* This = (IOCPClient*)arg;
        return This->SendThread();
    }

   
};