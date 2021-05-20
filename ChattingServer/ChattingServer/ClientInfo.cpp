//#include "ClientInfo.h"
//
//void stClientInfo::Initialize()
//{
//    m_socketClient = INVALID_SOCKET;
//    ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
//    ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
//    ZeroMemory(mRecvBuf, sizeof(mRecvBuf));
//    ZeroMemory(mSendBuf, sizeof(mRecvBuf));
//}
//
//void stClientInfo::Close()
//{
//    if (m_socketClient == INVALID_SOCKET)
//        return;
//    
//    struct linger stLinger = {0, 0};
//    shutdown(m_socketClient, SD_BOTH);
//    setsockopt(m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
//    closesocket(m_socketClient);
//    Initialize();
//}
//
//bool stClientInfo::BindIOCompletionPort(HANDLE iocpHandle_)
//{
//    HANDLE hIOCP = CreateIoCompletionPort(
//        (HANDLE)m_socketClient,
//        iocpHandle_,
//        (ULONG_PTR)(this),
//        0
//    );
//
//    if(hIOCP == INVALID_HANDLE_VALUE)
//    {
//        printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
//        return false;
//    }
//
//    return true;
//}
//
//bool stClientInfo::Connect(HANDLE iocpHandle_, SOCKET socket_)
//{
//    m_socketClient = socket_;
//    if(BindIOCompletionPort(iocpHandle_) == false)
//    {
//        printf("[에러] BindIOCompletionPort() 함수 실패\n");
//        return false;
//    }
//    
//    if(false == BindRecv())
//    {
//        printf("[에러] 클라이언트(%d) 연결 실패\n", mIndex);
//        Close();
//        return false;
//    }
//
//    return true;
//}
//
//
//bool stClientInfo::BindRecv()
//{
//    DWORD dwFlag = 0;
//    DWORD dwRecvNumBytes = 0;
//
//    m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
//    m_stRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
//    m_stRecvOverlappedEx.m_eOperation = RECV;
//
//    int nRet = WSARecv(
//        m_socketClient,
//        &(m_stRecvOverlappedEx.m_wsaBuf),
//        1,
//        &dwRecvNumBytes,
//        &dwFlag,
//        (LPWSAOVERLAPPED) & (m_stRecvOverlappedEx),
//        NULL
//    );
//    
//    if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
//    {
//        printf("[알림] RECV_SOCKET_ERROR 클라이언트(%d) 연결 종료\n", mIndex);
//        Close();
//        return false;
//    }
//
//    return true;
//}
//
//bool stClientInfo::SendMsg(const UINT32 dataSize_, char* pMsg_, IOOperation ioType_)
//{
//    stOverlappedEx* sendOverlappedEx = new stOverlappedEx;
//    ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
//    sendOverlappedEx->m_wsaBuf.len = dataSize_;
//    sendOverlappedEx->m_wsaBuf.buf = new char[dataSize_];
//    CopyMemory(sendOverlappedEx->m_wsaBuf.buf, pMsg_, dataSize_);
//    sendOverlappedEx->m_eOperation = ioType_;
//    
//    DWORD dwRecvNumBytes = 0;
//    int nRet = WSASend(m_socketClient,
//        &(sendOverlappedEx->m_wsaBuf),
//        1,
//        &dwRecvNumBytes,
//        0,
//        (LPWSAOVERLAPPED)sendOverlappedEx,
//        NULL);
//
//
//    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
//    {
//        printf("[알림] SEND_SOCKET_ERROR 클라이언트(%d) 연결 종료\n", mIndex);
//        Close();
//        return false;
//    }
//
//    return true;
//}