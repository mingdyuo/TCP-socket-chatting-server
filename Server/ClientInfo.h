#ifndef _STRUCT_CLIENT_INFO
#define _STRUCT_CLIENT_INFO

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>

#define MAX_SOCKBUF 1024	//패킷 크기

enum IOOperation
{
	RECV = 0,
	SEND,
	ACCEPT
};

struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		// Overlapped I/O구조체
	WSABUF		m_wsaBuf;				// Overlapped I/O작업 버퍼
	IOOperation m_eOperation;			// 작업 동작 종류
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
			printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
			return false;
		}

		return true;
	}

    bool AcceptRecv(){
        DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

        m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
		m_stRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
		m_stRecvOverlappedEx.m_eOperation = ACCEPT;

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
			return false;
		}
		
		return true;
    }

    bool Connect(HANDLE iocpHandle_, SOCKET socket_){
        m_socketClient = socket_;
        if(BindIOCompletionPort(iocpHandle_) == false)
			return false;

        if(false == AcceptRecv()){
            printf("[에러] 클라이언트(%d) 연결 실패\n", mIndex);
            Close();
			return false;
        }

        return true;
    }

	void SetSendBuf(int ioSize_){
		CopyMemory(mSendBuf, mRecvBuf, ioSize_);
	}
    
    bool BindRecv(){
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
		
		if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING)){
			printf("[알림] 클라이언트(%d) 연결 종료\n", mIndex);
			Close();
			return false;
		}

		return true;
	}

    bool SendMsg(const UINT32 dataSize_, char* pMsg_){
		// TODO : 구조체 내 sende overlapped 사용하도록 변경해보자
		stOverlappedEx* sendOverlappedEx = new stOverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
		sendOverlappedEx->m_wsaBuf.len = dataSize_;
		sendOverlappedEx->m_wsaBuf.buf = new char[dataSize_];
		CopyMemory(sendOverlappedEx->m_wsaBuf.buf, pMsg_, dataSize_);
		sendOverlappedEx->m_eOperation = SEND;
		
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
			printf("[알림] 클라이언트(%d) 연결 종료\n", mIndex);
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