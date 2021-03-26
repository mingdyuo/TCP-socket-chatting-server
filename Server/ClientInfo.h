
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>

#include "UserInfo.h"

#ifndef _STRUCT_CLIENT_INFO
#define _STRUCT_CLIENT_INFO

#define MAX_SOCKBUF 1024	//��Ŷ ũ��

enum IOOperation
{
	RECV = 0,
	SEND,
	CLOSE,
	SELF
};

struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		// Overlapped I/O����ü
	WSABUF		m_wsaBuf;				// Overlapped I/O�۾� ����
	IOOperation m_eOperation;			// �۾� ���� ����
};

struct stClientInfo : public stUserInfo
{

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

    bool IsConnected() {return m_socketClient != INVALID_SOCKET;}
    int GetIndex(){return mIndex;}
	
	void SetSendBuf(int ioSize_){ CopyMemory(mSendBuf, mRecvBuf, ioSize_); }
	char* SendBuffer(){return mSendBuf;}
    char* RecvBuffer() { return mRecvBuf; }

    bool BindIOCompletionPort(HANDLE iocpHandle_);
    bool Connect(HANDLE iocpHandle_, SOCKET socket_);
    bool BindRecv();
    bool SendMsg(const UINT32 dataSize_, char* pMsg_, IOOperation ioType_);


private:
    SOCKET			m_socketClient;			
	stOverlappedEx	m_stRecvOverlappedEx;	
	stOverlappedEx	m_stSendOverlappedEx;	
    char			mRecvBuf[MAX_SOCKBUF]; 
	char			mSendBuf[MAX_SOCKBUF]; 

    int             mIndex;
};

#endif