#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <cstdio>
#include <iostream>
#include "Position.h"
#include "Packet.h"



class IOCPClient{
private:
    static const int MAX_NICKNAME_LEN = 32;
    SOCKET mSocket;
    bool mbIsWorkerRun;
    char mNickname[MAX_NICKNAME_LEN];
    INT16 mRoom;

    Position pos;

public:
    IOCPClient():mSocket(INVALID_SOCKET), mbIsWorkerRun(true), mRoom(0) {}
    ~IOCPClient(){WSACleanup();}

    bool OnConnect(){return mbIsWorkerRun;}
    char* getNickname(){return mNickname;}

    bool InitSocket();
    bool ConnectServer(int bBindPort);
    bool SetNickname();
    bool CreateThreads(HANDLE* sender, HANDLE* recver);
    void ShowHelpBox();
    bool Close();

    DWORD RecvThread();
    DWORD SendThread();

    static DWORD WINAPI StaticRecvThread(LPVOID arg){
        IOCPClient* This = (IOCPClient*)arg;
        return This->RecvThread();
    }

    static DWORD WINAPI StaticSendThread(LPVOID arg){
        IOCPClient* This = (IOCPClient*)arg;
        return This->SendThread();
    }

    void parseContent(char* received, char* content, char* sender);
    void processRecvMsg(char* received, char* content, char* sender);
    int processSendMsg(std::string& content);
    

   
};