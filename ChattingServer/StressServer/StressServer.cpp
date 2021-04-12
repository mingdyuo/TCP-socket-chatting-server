#include "StressServer.h"
#include <cstdio>

unsigned StressServer::ConnecterThread()
{
    getchar();

    while (mbIsConnecterRun)
    {
        CreateClient(nicknames);

        Sleep(5);
    }
    printf("[알림] Accepter thread 종료\n");
    return 0;
}

unsigned StressServer::WorkerThread()
{
    unsigned uResult = 0;
    DWORD dwIoSize = 0;
    BOOL bSuccess = TRUE;
    stClientInfo* pClientInfo = NULL;
    LPOVERLAPPED lpOverlapped = NULL;

    while (mbIsWorkerRun) {
        bSuccess = GetQueuedCompletionStatus(
            mIOCPHandle,
            &dwIoSize,
            (PULONG_PTR)&pClientInfo,
            &lpOverlapped,
            INFINITE
        );

        if (bSuccess == TRUE && dwIoSize == 0 && lpOverlapped == NULL) {
            mbIsWorkerRun = false;
            continue;
        }

        if (lpOverlapped == NULL) continue;

        if (bSuccess == FALSE || (0 == dwIoSize && bSuccess == TRUE)) {
            mClients[pClientInfo->GetIndex()]->Close();
            continue;
        }

        stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

        if (SEND == pOverlappedEx->m_eOperation) {
            printf("[SEND(%d)] %d bytes\t", pClientInfo->GetIndex(), dwIoSize);
            if (endline++ % 4 == 0) printf("\n");
            delete[] pOverlappedEx->m_wsaBuf.buf;
            delete pOverlappedEx;
        }
        else if (RECV == pOverlappedEx->m_eOperation) {
            printf("[RECV(%d)] %d bytes\t", pClientInfo->GetIndex(), dwIoSize);
            if (endline++ % 4 == 0) printf("\n");
            pClientInfo->BindRecv();
        }
    }
    printf("[알림] Worker thread 종료\n");
    return uResult;
}

unsigned StressServer::SenderThread()
{
    unsigned uResult = 0;
    int castType, senderIndex = 0;
    int contentIndex = 0;

    srand((unsigned int)time(NULL));

    while (mbIsSenderRun) { // 패킷 생성해서 보내기
        if (mClientCount == 0) continue;

        castType = rand() % 8;
        senderIndex = rand() % mClientCount; // 임의로 송신자 선택
        stClientInfo* sender = mClients[senderIndex];

        CHAT_PACKET packet;
        strcpy(packet.Sender, (sender->GetNickname()).c_str());
        strcpy(packet.Content, contents[contentIndex].c_str());
        packet.Length = MAX_NICKNAME_LEN + strlen(contents[contentIndex].c_str());

        packet.Type = (castType == 0) ? CHAT_BROADCAST : CHAT_MULTICAST;

        mClients[senderIndex]->SendMsg(packet.Length, (char*)&packet);

        senderIndex++;
        contentIndex++;
        if (contentIndex >= contents.size()) contentIndex = 0;

        Sleep(30);
    }
    printf("[알림] Sender thread 종료\n");
    return uResult;
}

bool StressServer::CreateClient(const std::vector<std::string>& names)
{

    stClientInfo* newClient = new stClientInfo(mClients.size());
    mClients.push_back(newClient);

    newClient->Create(mIOCPHandle, SERVER_PORT);

    newClient->InitPacket(names[mClientCount % names.size()]);

    printf("[CONCT] %d counts\t", mClientCount);
    if (endline++ % 4 == 0) printf("\n");

    mClientCount++;

    return true;
}