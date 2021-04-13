#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <cstdio>

#ifndef _STRUCT_CLIENT_INFO
#define _STRUCT_CLIENT_INFO

#define MAX_SOCKBUF 1024	//패킷 크기

enum IOOperation
{
	RECV = 0,
	SEND,
	CLOSE
};

struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		// Overlapped I/O구조체
	WSABUF		m_wsaBuf;				// Overlapped I/O작업 버퍼
	IOOperation m_eOperation;			// 작업 동작 종류
};

struct stClientInfo
{
public:
    stClientInfo(): mIndex(-1), m_socketClient(INVALID_SOCKET)
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
        ZeroMemory(mRecvBuf, sizeof(mRecvBuf));
		ZeroMemory(mSendBuf, sizeof(mRecvBuf));
	}   
    stClientInfo(UINT32 index): mIndex(index), m_socketClient(INVALID_SOCKET)
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
        ZeroMemory(mRecvBuf, sizeof(mRecvBuf));
		ZeroMemory(mSendBuf, sizeof(mRecvBuf));
	}

    void Initialize();
    void Close();

	bool IsConnected() { return m_socketClient != INVALID_SOCKET; }
	int GetIndex() { return mIndex; }
	
	char* SendBuffer() { return mSendBuf; }
    char* RecvBuffer() { return mRecvBuf; }

    bool BindIOCompletionPort(HANDLE iocpHandle_);
    bool Connect(HANDLE iocpHandle_, SOCKET socket_);
    bool BindRecv();
    bool SendMsg(const UINT32 dataSize_, char* pMsg_, IOOperation ioType_ = SEND);


protected:
    SOCKET			m_socketClient;			
	stOverlappedEx	m_stRecvOverlappedEx;	
	stOverlappedEx	m_stSendOverlappedEx;	
    char			mRecvBuf[MAX_SOCKBUF]; 
	char			mSendBuf[MAX_SOCKBUF]; 

    int             mIndex; 				//< Vector index managed by Client Manager
};

#endif