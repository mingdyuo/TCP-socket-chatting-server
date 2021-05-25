#pragma once
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <mutex>
#include <thread>

class ClientBase
{

public:
    ClientBase() :socket_(INVALID_SOCKET), bIsWorkerRun_(true) {}
    virtual ~ClientBase() { WSACleanup(); }

    bool InitSocket()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
            return false;
        }

        socket_ = socket(PF_INET, SOCK_STREAM, 0);

        if (socket_ == INVALID_SOCKET) {
            printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
            return false;
        }

        return true;
    }

    bool ConnectServer(int bBindPort)
    {
        SOCKADDR_IN serverAddress;

        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddress.sin_port = htons(bBindPort);

        if (SOCKET_ERROR == connect(socket_, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) {
            printf("[에러] connect()함수 실패 : %d\n", WSAGetLastError());
            return false;
        }

        return true;
    }

    bool CreateThreads() {
        sendThread_ = std::thread(&ClientBase::SendThread, this);
        recvThread_ = std::thread(&ClientBase::RecvThread, this);
        logicThread_ = std::thread(&ClientBase::StateProcessThread, this);

        return true;
    }

    bool Close()
    {
        sendThread_.join();
        recvThread_.join();
        bIsWorkerRun_ = false;
        closesocket(socket_);
        return true;
    }

protected:
    virtual void SendThread() = 0;
    virtual void RecvThread() = 0;
    virtual void StateProcessThread() = 0;

    SOCKET          socket_;
    bool            bIsWorkerRun_;
    std::thread     sendThread_;
    std::thread     recvThread_;
    std::thread     logicThread_;

};