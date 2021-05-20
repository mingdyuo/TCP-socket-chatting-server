#include "ChatServer.h"
#include "PacketInfo.h"

void ChatServer::OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) 
{

    PacketInfo packetInfo;
    packetInfo.Set(clientIndex_, size_, pData_);

    printf("[OnReceive] 클라이언트(%d) %d bytes\n", packetInfo.ClientIndex, packetInfo.DataSize);
    mPacketMgr->ClassifyPacket(packetInfo);

}

void ChatServer::OnSend(const UINT32 clientIndex_, const UINT32 size_) 
{
    printf("[OnSend] 클라이언트(%d) %d bytes\n", clientIndex_, size_);
}


void ChatServer::OnClose(int clientIndex_)
{
    printf("[OnClose] 클라이언트(%d) 연결 종료\n", clientIndex_);
    mClientMgr->CloseClient(clientIndex_);
}


unsigned ChatServer::AccepterThread()
{
    unsigned uResult = 0;
    SOCKADDR_IN stClientAddr;
    int nAddrLen = sizeof(SOCKADDR_IN);
    while(mbIsAccepterRun)
    {
        SOCKET newSocket = accept(
            mListenSocket,
            (SOCKADDR*)&stClientAddr,
            &nAddrLen
        );

        if(INVALID_SOCKET == newSocket)
            continue;

        if(false == mClientMgr->CreateClient(mIOCPHandle, newSocket))
        {
            closesocket(newSocket);
            continue;
        }

    }
    printf("[알림] Accepter thread 종료\n");
    return uResult;
}

unsigned ChatServer::WorkerThread()
{
    unsigned uResult = 0;
    DWORD dwIoSize = 0;
    BOOL bSuccess = TRUE;
    stClientInfo* pClientInfo = NULL;
    LPOVERLAPPED lpOverlapped = NULL;

    while(mbIsWorkerRun)
    {
        bSuccess = GetQueuedCompletionStatus(
            mIOCPHandle,
            &dwIoSize,
            (PULONG_PTR)&pClientInfo,
            &lpOverlapped,
            INFINITE
        );

        if(bSuccess==TRUE && dwIoSize==0 && lpOverlapped==NULL)
        {
            mbIsWorkerRun = false;
            continue;
        }

        if(lpOverlapped==NULL) continue;

        if(bSuccess == FALSE || (0 == dwIoSize && bSuccess == TRUE))
        {
            mClientMgr->CloseClient(pClientInfo->GetIndex());
            continue;
        }

        stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

        if(SEND == pOverlappedEx->m_eOperation)
        {
            OnSend(pClientInfo->GetIndex(), dwIoSize);
            delete[] pOverlappedEx->m_wsaBuf.buf;
            delete pOverlappedEx;
        }
        else if(RECV == pOverlappedEx->m_eOperation)
        {
            OnReceive(pClientInfo->GetIndex(), dwIoSize, pClientInfo->RecvBuffer());
            pClientInfo->BindRecv();
        }
        else if(CLOSE == pOverlappedEx->m_eOperation)
        {
            delete[] pOverlappedEx->m_wsaBuf.buf;
            delete pOverlappedEx;
            OnClose(pClientInfo->GetIndex());
        }

    }
    printf("[알림] Worker thread 종료\n");
    return uResult;
}

