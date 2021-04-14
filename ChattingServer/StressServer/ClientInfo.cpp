#pragma once
#include "ClientInfo.h"

void stClientInfo::Initialize() 
{
    m_socketClient = INVALID_SOCKET;
    ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
    ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
    ZeroMemory(mRecvBuf, sizeof(mRecvBuf));
    ZeroMemory(mSendBuf, sizeof(mRecvBuf));
}

void stClientInfo::Close() 
{
    struct linger stLinger = { 0, 0 };
    shutdown(m_socketClient, SD_BOTH);
    setsockopt(m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
    closesocket(m_socketClient);
    Initialize();
}

void stClientInfo::Create(HANDLE iocpHandle_, int bBindPort) 
{
    m_socketClient = socket(PF_INET, SOCK_STREAM, 0);

    if (m_socketClient == INVALID_SOCKET) 
    {
        printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
        return;
    }

    SOCKADDR_IN serverAddress;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(bBindPort);

    if (SOCKET_ERROR == connect(m_socketClient, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) 
    {
        printf("[에러] connect()함수 실패 : %d\n", WSAGetLastError());
        return;
    }

    HANDLE hIOCP = CreateIoCompletionPort(
        (HANDLE)m_socketClient,
        iocpHandle_,
        (ULONG_PTR)(this),
        0
    );

    if (hIOCP == INVALID_HANDLE_VALUE) 
    {
        printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
        return;
    }

    if (false == BindRecv()) 
    {
        printf("[에러] 클라이언트(%d) 연결 실패\n", mIndex);
        Close();
        return;
    }
}

bool stClientInfo::InitPacket(const std::string& name)
{
    nickname = std::string(name);

    // 닉네임 생성 & 입장 처리
    SERVER_ENTER_PACKET serverPacket;
    serverPacket.Length = (UINT16)strlen(name.c_str()) + 1;


    strncpy(serverPacket.Sender, name.c_str(), serverPacket.Length);
    serverPacket.Type = SERVER_ENTER;

    SendMsg(serverPacket.Length, (char*)&serverPacket);

    ROOM_ENTER_PACKET roomPacket;
    roomPacket.Type = ROOM_ENTER;
    roomPacket.RoomNo = mIndex % 5 + 1;
    strcpy(roomPacket.Sender, name.c_str());
    roomPacket.Length = strlen(name.c_str());

    SendMsg(ROOM_ENTER_PACKET_LENGTH, (char*)&roomPacket);
}


bool stClientInfo::BindRecv() 
{
    DWORD dwFlag = 0;
    DWORD dwRecvNumBytes = 0;

    m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
    m_stRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
    m_stRecvOverlappedEx.m_eOperation = RECV;

    int nRet = WSARecv(
        m_socketClient,
        &(m_stRecvOverlappedEx.m_wsaBuf),
        1,
        &dwRecvNumBytes,
        &dwFlag,
        (LPWSAOVERLAPPED) & (m_stRecvOverlappedEx),
        NULL
    );

    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING)) 
    {
        printf("[알림] RECV_SOCKET_ERROR 클라이언트(%d) 연결 종료\n", mIndex);
        Close();
        return false;
    }

    return true;
}

bool stClientInfo::SendMsg(const UINT32 dataSize_, char* pMsg_, IOOperation ioType_) 
{
    stOverlappedEx* sendOverlappedEx = new stOverlappedEx;
    ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
    sendOverlappedEx->m_wsaBuf.len = dataSize_;
    sendOverlappedEx->m_wsaBuf.buf = new char[dataSize_];
    CopyMemory(sendOverlappedEx->m_wsaBuf.buf, pMsg_, dataSize_);
    sendOverlappedEx->m_eOperation = ioType_;

    DWORD dwRecvNumBytes = 0;
    int nRet = WSASend(m_socketClient,
        &(sendOverlappedEx->m_wsaBuf),
        1,
        &dwRecvNumBytes,
        0,
        (LPWSAOVERLAPPED)sendOverlappedEx,
        NULL);


    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
    {
        printf("[알림] SEND_SOCKET_ERROR 클라이언트(%d) 연결 종료\n", mIndex);
        Close();
        return false;
    }

    return true;
}