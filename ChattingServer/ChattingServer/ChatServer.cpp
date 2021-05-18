#include "ChatServer.h"
#include "../NetworkLib/Package.h"
#include "LogicProcess.h"

unsigned ChatServer::AccepterThread()
{
    unsigned uResult = 0;
    SOCKADDR_IN stClientAddr;
    int nAddrLen = sizeof(SOCKADDR_IN);
    while (bIsAccepterRun_)
    {
        SOCKET newSocket = accept(
            listenSocket_,
            (SOCKADDR*)&stClientAddr,
            &nAddrLen
        );

        if (INVALID_SOCKET == newSocket)
            continue;

        if (false == logicProcess_->CreateUser(IOCPHandle_, newSocket))
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
    Session* pSession = nullptr;
    LPOVERLAPPED lpOverlapped = nullptr;

    while (bIsWorkerRun_)
    {
        try
        {
            bSuccess = GetQueuedCompletionStatus(
                IOCPHandle_,
                &dwIoSize,
                (PULONG_PTR)&pSession,
                &lpOverlapped,
                INFINITE
            );

            if (bSuccess == TRUE && dwIoSize == 0 && lpOverlapped == nullptr)
            {
                bIsWorkerRun_ = false;
                continue;
            }

            if (lpOverlapped == nullptr) continue;

            if (bSuccess == FALSE || (0 == dwIoSize && bSuccess == TRUE))
            {
                logicProcess_->RemoveUser(pSession->GetPid());
                continue;
            }

            IoData* ioData = (IoData*)lpOverlapped;

            if (IO_SEND == ioData->Type())
            {
                printf("[SEND] client(%d)\n", pSession->GetPid());
                delete ioData;
            }
            else if (IO_RECV == ioData->Type())
            {
                Packet* packet = pSession->OnRecv(dwIoSize);

                if (packet)
                {
                    RecvPackage package(pSession, packet);
                    logicProcess_->PushPackage(package);
                }

                pSession->BindRecv();
            }

        }
        catch (Session* session)
        {
            if (WSAGetLastError() != 64)
            {
                printf("[Error] GQCS error\n");
            }

            logicProcess_->RemoveUser(pSession->GetPid());
        }

    }
    printf("[알림] Worker thread 종료\n");
    return uResult;
}

