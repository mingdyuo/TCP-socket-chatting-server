
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
	ACCEPT,
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

    bool IsConnected() {return m_socketClient != INVALID_SOCKET;}


    int GetIndex(){return mIndex;}

    char* RecvBuffer() { return mRecvBuf; }

    void Initialize(){
        m_socketClient = INVALID_SOCKET;
        ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
        ZeroMemory(mRecvBuf, sizeof(mRecvBuf));
		ZeroMemory(mSendBuf, sizeof(mRecvBuf));
		ZeroMemory(mNickname, sizeof(mNickname));
    }

    void Close(){
        struct linger stLinger = {0, 0};
		shutdown(m_socketClient, SD_BOTH);
		setsockopt(m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
		closesocket(m_socketClient);
        Initialize();
    }

    bool BindIOCompletionPort(HANDLE iocpHandle_){
		HANDLE hIOCP = CreateIoCompletionPort(
			(HANDLE)m_socketClient,
			iocpHandle_,
			(ULONG_PTR)(this),
			0
		);

		if(hIOCP == INVALID_HANDLE_VALUE){
			printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
			return false;
		}

		return true;
	}

    bool Connect(HANDLE iocpHandle_, SOCKET socket_){
        m_socketClient = socket_;
        if(BindIOCompletionPort(iocpHandle_) == false){
			printf("[����] BindIOCompletionPort() �Լ� ����\n");
			return false;
		}
		
        if(false == BindRecv(ACCEPT)){
            printf("[����] Ŭ���̾�Ʈ(%d) ���� ����\n", mIndex);
            Close();
			return false;
        }

        return true;
    }

	void SetSendBuf(int ioSize_){
		CopyMemory(mSendBuf, mRecvBuf, ioSize_);
	}

	char* GetSendBuf(){return mSendBuf;}
    
    bool BindRecv(IOOperation ioType_){
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
		m_stRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
		m_stRecvOverlappedEx.m_eOperation = ioType_;

		int nRet = WSARecv(
			m_socketClient,
			&(m_stRecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (m_stRecvOverlappedEx),
			NULL
		);
		
		if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING)){
			printf("[�˸�] RECV_SOCKET_ERROR Ŭ���̾�Ʈ(%d) ���� ����\n", mIndex);
			Close();
			return false;
		}

		return true;
	}

    bool SendMsg(const UINT32 dataSize_, char* pMsg_, IOOperation ioType_){
		// TODO : ����ü �� sende overlapped ����ϵ��� �����غ���
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
			printf("[�˸�] SEND_SOCKET_ERROR Ŭ���̾�Ʈ(%d) ���� ����\n", mIndex);
			Close();
			return false;
		}

		return true;
	}


private:
    SOCKET			m_socketClient;			
	stOverlappedEx	m_stRecvOverlappedEx;	
	stOverlappedEx	m_stSendOverlappedEx;	
    char			mRecvBuf[MAX_SOCKBUF]; 
	char			mSendBuf[MAX_SOCKBUF]; 

    int             mIndex;
};

#endif