#pragma once
#include "ClientInfo.h"
#include "CriticalSection.h"
#include "IOCPServer.h"
#include "csv_parser.h"

#include <process.h>
#include <vector>
#include <cstdlib>
#include <ctime>

class StressServer : public IOCPServer
{
public:
    StressServer() : mClientCount(0), mEndline(1) {
        if (!parseCsv(this->mNicknames, "nickname_data.csv")) 
        {
            printf("[에러] 랜덤 닉네임 불러오기 실패\n");
        }
        if (!parseCsv(this->mContents, "random_content.csv")) 
        {
            printf("[에러] 랜덤 전송 내용 불러오기 실패\n");
        }
    }

    virtual ~StressServer(void)
    {
        WSACleanup();
        for (int i = 0;i < mClients.size();i++) 
        {
            if (mClients[i] == NULL) continue;
            mClients[i]->Close();
            delete mClients[i];
        }
    }

    virtual unsigned ConnecterThread();
    virtual unsigned WorkerThread();
    virtual unsigned SenderThread();

    bool CreateClient(const std::vector<std::string>& names);


private:
    const int SERVER_PORT = 9898;

    std::vector<stClientInfo*> mClients;

    std::vector<std::string> mContents;
    std::vector<std::string> mNicknames;

    int mClientCount;
    int mEndline;
};


