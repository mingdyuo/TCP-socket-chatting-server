#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>
#define MAX_SOCKBUF 1024	//패킷 크기

enum IOOperation
{
	RECV = 0,
	SEND
};

struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		// Overlapped I/O구조체
	SOCKET		m_socketClient;			// 클라이언트 소켓
	WSABUF		m_wsaBuf;				// Overlapped I/O작업 버퍼
	IOOperation m_eOperation;			// 작업 동작 종류
};

struct stClientInfo
{
	size_t mIndex;
	SOCKET			m_socketClient;			
	stOverlappedEx	m_stRecvOverlappedEx;	

	bool			mbHasNick;
	char 			mNickname[32];
	UINT16			mRoom;

	char			mRecvBuf[MAX_SOCKBUF]; 

	stClientInfo(): mIndex(0), m_socketClient(INVALID_SOCKET), mbHasNick(false), mRoom(0)
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
	}

	stClientInfo(UINT32 index): mIndex(index), m_socketClient(INVALID_SOCKET), mbHasNick(false), mRoom(0)
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
	}

public:
	size_t GetIndex(){return mIndex;}
	SOCKET GetSock() {return m_socketClient; }
	bool IsConnected() {return m_socketClient != INVALID_SOCKET;}

	void SendCompleted(const UINT32 dataSize_){
		printf("[송신 완료] bytes : %d\n", dataSize_);
	}

	void setNickname(int size){
		for(int i=4;i<size;i++) mNickname[i - 4] = mRecvBuf[i];
		mNickname[size - 4] = '\0';
		mbHasNick = true;
	}

	void Initialize(){
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		memset(mNickname, 0, sizeof(mNickname));
	}

	void Close(bool bIsForce = false){
		struct linger stLinger = {0, 0};
		if(bIsForce)
			stLinger.l_onoff = 1;
		
		shutdown(m_socketClient, SD_BOTH);

		setsockopt(m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		closesocket(m_socketClient);

		Initialize();
		m_socketClient = INVALID_SOCKET;
	}

	bool BindIOCompletionPort(HANDLE iocpHandle_){
		HANDLE hIOCP = CreateIoCompletionPort(
			(HANDLE)GetSock(),
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

	bool OnConnect(HANDLE iocpHandle_, SOCKET socket_){
		m_socketClient = socket_;
		if(BindIOCompletionPort(iocpHandle_) == false)
			return false;

		return BindRecv(); 
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
			// printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
			// 10054 뜨는데 이거 어떻게 잘 프로세스 맞춰서 종료해야되징?
			printf("[알림] 클라이언트(%d) 연결 종료\n", GetIndex());
			Close();
			return false;
		}

		
		return true;
	}

	// 1개의 스레드 에서만 호출해야함
	bool SendMsg(const UINT32 dataSize_, char* pMsg_){
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

		//socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			// printf("[알림] 클라이언트 연결 종료 : %d\n", WSAGetLastError());
			printf("[알림] 클라이언트(%d) 연결 종료\n", GetIndex());
			Close();
			return false;
		}

		return true;
	}



};

